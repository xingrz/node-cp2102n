#include "cp2102n.h"

#include "promised_async.h"

using namespace Napi;

namespace {

typedef struct {
    char path[255];
    cp2102_dev_t *dev;
} OpenContext;

Value CP2102N::Open(const CallbackInfo &info) {
    Env env = info.Env();
    if (info.Length() != 1) {
        Error::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    if (!info[0].IsString()) {
        TypeError::New(env, "Argument 1 should be a string").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto baton =
        new PromisedAsync<OpenContext>(env, "open", CP2102N::AsyncOpen, CP2102N::FinishAsyncOpen);
    strcpy(baton->context.path, info[0].ToString().Utf8Value().c_str());

    return baton->Queue();
}

void CP2102N::AsyncOpen(napi_env env, void *data) {
    PromisedAsync<OpenContext> *baton = (PromisedAsync<OpenContext> *)data;
    baton->context.dev = cp2102_open(baton->context.path);
}

void CP2102N::FinishAsyncOpen(napi_env env, napi_status status, void *data) {
    PromisedAsync<OpenContext> *baton = (PromisedAsync<OpenContext> *)data;
    auto ctx = &baton->context;
    if (ctx->dev != NULL) {
        Object result = Object::New(env);

        result.Set(String::New(env, "get"), Function::New<CP2102N::Get>(env, "get", ctx->dev));
        result.Set(String::New(env, "set"), Function::New<CP2102N::Set>(env, "set", ctx->dev));
        result.Set(String::New(env, "getSerialNumber"),
            Function::New<CP2102N::SerialNumber>(env, "getSerialNumber", ctx->dev));
        result.Set(
            String::New(env, "close"), Function::New<CP2102N::Close>(env, "close", ctx->dev));

        baton->Resolve(result);
    } else {
        baton->Reject(String::New(env, "Failed opening device"));
    }
    delete baton;
}

typedef struct {
    cp2102_dev_t *dev;
    uint8_t read_bits;
    bool succeed;
} GetContext;

Value CP2102N::Get(const CallbackInfo &info) {
    Env env = info.Env();

    auto baton =
        new PromisedAsync<GetContext>(env, "get", CP2102N::AsyncGet, CP2102N::FinishAsyncGet);
    baton->context.dev = (cp2102_dev_t *)info.Data();

    return baton->Queue();
}

void CP2102N::AsyncGet(napi_env env, void *data) {
    PromisedAsync<GetContext> *baton = (PromisedAsync<GetContext> *)data;
    baton->context.succeed = cp2102_get_value(baton->context.dev, &baton->context.read_bits);
}

void CP2102N::FinishAsyncGet(napi_env env, napi_status status, void *data) {
    PromisedAsync<GetContext> *baton = (PromisedAsync<GetContext> *)data;
    if (baton->context.succeed) {
        baton->Resolve(Number::New(env, baton->context.read_bits));
    } else {
        baton->Reject(String::New(env, "Failed reading device"));
    }
}

typedef struct {
    cp2102_dev_t *dev;
    uint8_t write_bits;
    uint8_t write_mask;
    uint8_t read_bits;
    bool succeed;
} SetContext;

Value CP2102N::Set(const CallbackInfo &info) {
    Env env = info.Env();
    if (info.Length() != 2) {
        Error::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    if (!info[0].IsNumber()) {
        TypeError::New(env, "Argument 1 and 2 should be numbers").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto baton =
        new PromisedAsync<SetContext>(env, "set", CP2102N::AsyncSet, CP2102N::FinishAsyncSet);
    baton->context.dev = (cp2102_dev_t *)info.Data();
    baton->context.write_bits = (uint8_t)info[0].ToNumber().Uint32Value();
    baton->context.write_mask = (uint8_t)info[1].ToNumber().Uint32Value();

    return baton->Queue();
}

void CP2102N::AsyncSet(napi_env env, void *data) {
    PromisedAsync<SetContext> *baton = (PromisedAsync<SetContext> *)data;
    auto ctx = &baton->context;
    ctx->succeed = cp2102_set_value(ctx->dev, ctx->write_bits, ctx->write_mask) &&
                   cp2102_get_value(ctx->dev, &ctx->read_bits);
}

void CP2102N::FinishAsyncSet(napi_env env, napi_status status, void *data) {
    PromisedAsync<SetContext> *baton = (PromisedAsync<SetContext> *)data;
    if (baton->context.succeed) {
        baton->Resolve(Number::New(env, baton->context.read_bits));
    } else {
        baton->Reject(String::New(env, "Failed writing device"));
    }
}

typedef struct {
    cp2102_dev_t *dev;
    const char *serial_number;
} SerialNumberContext;

Value CP2102N::SerialNumber(const CallbackInfo &info) {
    Env env = info.Env();

    auto baton = new PromisedAsync<SerialNumberContext>(
        env, "serialNumber", CP2102N::AsyncSerialNumber, CP2102N::FinishAsyncSerialNumber);
    baton->context.dev = (cp2102_dev_t *)info.Data();

    return baton->Queue();
}

void CP2102N::AsyncSerialNumber(napi_env env, void *data) {
    PromisedAsync<SerialNumberContext> *baton = (PromisedAsync<SerialNumberContext> *)data;
    baton->context.serial_number = cp2102_get_serial_number(baton->context.dev);
}

void CP2102N::FinishAsyncSerialNumber(napi_env env, napi_status status, void *data) {
    PromisedAsync<SerialNumberContext> *baton = (PromisedAsync<SerialNumberContext> *)data;
    baton->Resolve(String::New(env, baton->context.serial_number));
}

Value CP2102N::Close(const CallbackInfo &info) {
    cp2102_dev_t *dev = (cp2102_dev_t *)info.Data();
    cp2102_close(&dev);
    return info.Env().Undefined();
}

}  // namespace

static Object Init(Env env, Object exports) {
    cp2102_init();
    env.AddCleanupHook([&]() { cp2102_exit(); });

    exports.Set(
        String::New(env, "openInterface"), Function::New<CP2102N::Open>(env, "openInterface"));

    return exports;
}

NODE_API_MODULE(cp2102n, Init);
