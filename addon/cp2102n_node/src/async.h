#pragma once

#include <stdlib.h>

#include "node_api.h"

#define NAPI_ASYNC(NAME, CTX)                          \
	typedef struct CTX NAME##_context_t;               \
	typedef struct {                                   \
		napi_deferred deferred;                        \
		napi_async_work worker;                        \
		NAME##_context_t ctx;                          \
	} NAME##_data_t;                                   \
	static void NAME##_work(napi_env env, void *data); \
	static void NAME##_done(napi_env env, napi_status status, void *data);

#define NAPI_ASYNC_WORK(NAME, FUNC)                   \
	static void NAME##_work(napi_env env, void *data) \
	{                                                 \
		NAME##_data_t *d = (NAME##_data_t *)data;     \
		NAME##_context_t *ctx = &d->ctx;              \
		FUNC;                                         \
	}

#define NAPI_ASYNC_DONE(NAME, FUNC)                                       \
	static void NAME##_done(napi_env env, napi_status status, void *data) \
	{                                                                     \
		NAME##_data_t *d = (NAME##_data_t *)data;                         \
		NAME##_context_t *ctx = &d->ctx;                                  \
		FUNC;                                                             \
		napi_delete_async_work(env, d->worker);                           \
		free(d);                                                          \
	}

#define NAPI_ASYNC_RESOLVE(VAR) napi_resolve_deferred(env, d->deferred, VAR)
#define NAPI_ASYNC_REJECT(VAR) napi_reject_deferred(env, d->deferred, VAR)

#define NAPI_ASYNC_CREATE(VAR, CTX_VAR, NAME)                                                     \
	NAME##_data_t *VAR##_data = (NAME##_data_t *)malloc(sizeof(NAME##_data_t));                   \
	NAME##_context_t *CTX_VAR = &VAR##_data->ctx;                                                 \
	napi_value VAR;                                                                               \
	{                                                                                             \
		napi_create_promise(env, &VAR##_data->deferred, &VAR);                                    \
		NAPI_CREATE_STRING(resource_name, #NAME);                                                 \
		napi_create_async_work(                                                                   \
			env, NULL, resource_name, NAME##_work, NAME##_done, VAR##_data, &VAR##_data->worker); \
	}

#define NAPI_ASYNC_QUEUE(VAR)                           \
	{                                                   \
		napi_queue_async_work(env, VAR##_data->worker); \
	}
