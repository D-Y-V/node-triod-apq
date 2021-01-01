#pragma once
#include <napi.h>
#include <libpq-fe.h>
#include <iostream> // cin && cout
#include <vector>

using namespace Napi;

class pqAsyncWorker : public AsyncWorker {
public:
    pqAsyncWorker(Napi::Env &env,std::string sql, std::vector<std::string> params);
    pqAsyncWorker(Napi::Env &env,std::string sql);

    virtual ~pqAsyncWorker(){};
    void Execute();
    void OnOK();
    Napi::Promise GetPromise() { return deferred.Promise(); };
private:
    Napi::Promise::Deferred deferred;
    bool simple_mode;
    std::string sql;
    std::vector<std::string> params;
    std::vector<std::string> sql_result;
    PGconn     *conn;
    PGresult   *res;
    int nfields;
    int ntuples;
    std::string res_status;
};