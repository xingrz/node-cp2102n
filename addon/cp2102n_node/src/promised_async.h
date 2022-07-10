#pragma once

#include "node_api.h"

namespace {

template <class T>
class PromisedAsync {
   public:
    PromisedAsync(napi_env env, const char *name, napi_async_execute_callback execute,
        napi_async_complete_callback complete)
        : env(env) {
        napi_create_promise(env, &this->deferred, &this->promise);
        napi_create_async_work(
            env, NULL, String::New(env, name), execute, complete, this, &this->worker);
    }

    ~PromisedAsync() { napi_delete_async_work(this->env, this->worker); }

    Value Queue() {
        napi_queue_async_work(this->env, this->worker);
        return Value(env, this->promise);
    }

    void Resolve(napi_value resolution) {
        napi_resolve_deferred(this->env, this->deferred, resolution);
    }

    void Reject(napi_value rejection) {
        napi_reject_deferred(this->env, this->deferred, rejection);
    }

   private:
    napi_env env;
    napi_value promise;
    napi_deferred deferred;
    napi_async_work worker;

   public:
    T context;
};

}  // namespace
