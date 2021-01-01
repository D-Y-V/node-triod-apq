#include "pqAsyncWorker.h"
#include <thread>

pqAsyncWorker::pqAsyncWorker(Napi::Env &env, std::string sql, std::vector<std::string> params)
        : AsyncWorker(env),
          deferred(Napi::Promise::Deferred::New(env)),
          sql(sql),
          params(params)
{ simple_mode = false; };
pqAsyncWorker::pqAsyncWorker(Napi::Env &env, std::string sql)
        : AsyncWorker(env),
          deferred(Napi::Promise::Deferred::New(env)),
          sql(sql)
{simple_mode = true;};

void pqAsyncWorker::Execute() {
    conn = PQconnectdb("");
    if(!simple_mode) {
        char **parameters = new char *[params.size()];
        for (long unsigned int i = 0; i < params.size(); ++i) {
            parameters[i] = (char *) params[i].c_str();
        };
        res = PQexecParams(conn,
                           sql.c_str(),
                           params.size(),
                           NULL,    /* let the backend deduce param type */
                           parameters,
                           NULL,    /* don't need param lengths since text */
                           NULL,    /* default to all text params */
                           0);      /* ask for text results */
        delete[] parameters;
    } else{
        res = PQexec(conn, sql.c_str());
    }
    res_status = PQresStatus(PQresultStatus(res));
    nfields = PQnfields(res);
    ntuples = PQntuples(res);
    for (int i = 0; i< ntuples; i++)
    {
        for (int j = 0; j < nfields; j++) {
            int l = PQgetlength(res, i, j);
            char *s = PQgetvalue(res, i, j);
            sql_result.push_back(std::string(s,l));
        }
    }
    PQclear(res);
    PQfinish(conn);
};

void pqAsyncWorker::OnOK() {
    Napi::Env env = Env();
    Napi::Object ret_tmp = Napi::Object::New( env );
    int index = 0;
    for (int i = 0; i < ntuples; ++i) {
        for (int j = 0; j <nfields ; ++j) {
            ret_tmp.Set(index, Napi::String::New( env, sql_result[index] ));
            index++;
        }
    }
    ret_tmp.Set("ntuples", Napi::Number::New( env, ntuples ));
    ret_tmp.Set("nfields", Napi::Number::New( env, nfields ));
    ret_tmp.Set("ExecStatus", Napi::String::New( env, res_status ));
    deferred.Resolve(ret_tmp);
};
