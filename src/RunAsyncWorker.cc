// YD
#include "pqAsyncWorker.h"

Value runAsyncWorker(const CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint32_t pcount =  info.Length();
    if(pcount < 1) {
        Napi::TypeError::New(env, "Expected minimum 1 args. SQL.")
                .ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string sql = info[0].As<Napi::String>().Utf8Value();
    pqAsyncWorker* asyncWorker;
    std::vector<std::string> vparams;
    if(pcount == 1){
        // simple mode
        asyncWorker = new pqAsyncWorker(env, sql);
    } else{
        for (uint32_t i = 1; i < pcount; ++i) {
            std::string s = info[i].As<Napi::String>().Utf8Value();
            vparams.push_back(s);
        }
        asyncWorker = new pqAsyncWorker(env, sql, vparams);
    }
    auto promise = asyncWorker->GetPromise();
    asyncWorker->Queue();
    return promise;
};

Object Init(Env env, Object exports) {
    exports["run"] = Function::New(
            env, runAsyncWorker, std::string("run"));
    return exports;
}

NODE_API_MODULE(addon, Init)
