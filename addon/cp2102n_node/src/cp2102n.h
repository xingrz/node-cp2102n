#pragma once

#include <napi.h>

extern "C" {
#include "cp2102.h"
}

using namespace Napi;

namespace {

class CP2102N {
   public:
    static Value Open(const CallbackInfo &info);
    CP2102N(cp2102_dev_t *dev);

    static Value Get(const CallbackInfo &info);
    static Value Set(const CallbackInfo &info);
    static Value SerialNumber(const CallbackInfo &info);
    static Value Close(const CallbackInfo &info);

   private:
    static void AsyncOpen(napi_env env, void *data);
    static void FinishAsyncOpen(napi_env env , napi_status status, void *data);
    static void AsyncGet(napi_env env, void *data);
    static void FinishAsyncGet(napi_env env, napi_status status, void *data);
    static void AsyncSet(napi_env env, void *data);
    static void FinishAsyncSet(napi_env env , napi_status status, void *data);
    static void AsyncSerialNumber(napi_env env, void *data);
    static void FinishAsyncSerialNumber(napi_env env , napi_status status, void *data);

   private:
    cp2102_dev_t *dev;
};

}  // namespace
