#ifndef __CP2102N_UTILS__
#define __CP2102N_UTILS__

#define NAPI_ASSERT_ARGC(N)                                                \
	size_t argc = N;                                                       \
	napi_value args[N];                                                    \
	{                                                                      \
		napi_get_cb_info(env, info, &argc, args, NULL, NULL);              \
		if (argc != N) {                                                   \
			napi_throw_error(env, "Error", "Invalid number of arguments"); \
			return NULL;                                                   \
		}                                                                  \
	}

#define NAPI_ASSERT_ARG_TYPE(N, TYPE)                                \
	{                                                                \
		napi_valuetype value_type;                                   \
		napi_typeof(env, args[N], &value_type);                      \
		if (value_type != TYPE) {                                    \
			napi_throw_error(env, "Error", "Invalid argument type"); \
			return NULL;                                             \
		}                                                            \
	}

#define NAPI_GET_STRING(VAR, SIZE, FROM_VALUE)                               \
	char VAR[SIZE];                                                          \
	{                                                                        \
		napi_get_value_string_utf8(env, FROM_VALUE, VAR, sizeof(VAR), NULL); \
	}

#define NAPI_CREATE_OBJECT(VAR)        \
	napi_value VAR;                    \
	{                                  \
		napi_create_object(env, &VAR); \
	}

#define NAPI_CREATE_FUNTION(VAR, NAME, CB, DATA)                                   \
	napi_value VAR;                                                                \
	{                                                                              \
		napi_create_function(env, NAME, NAPI_AUTO_LENGTH, CB, (void *)DATA, &VAR); \
	}

#define NAPI_CREATE_STRING(VAR, FROM_VALUE)                               \
	napi_value VAR;                                                       \
	{                                                                     \
		napi_create_string_utf8(env, FROM_VALUE, NAPI_AUTO_LENGTH, &VAR); \
	}

#define NAPI_CREATE_ERROR(VAR, CODE, MSG)        \
	napi_value VAR;                              \
	{                                            \
		NAPI_CREATE_STRING(code, CODE);          \
		NAPI_CREATE_STRING(msg, MSG);            \
		napi_create_error(env, code, msg, &VAR); \
	}

#define NAPI_CREATE_UINT32(VAR, FROM_VALUE)                  \
	napi_value VAR;                                          \
	{                                                        \
		napi_create_uint32(env, (uint32_t)FROM_VALUE, &VAR); \
	}

#define NAPI_DEFINE_WORK_CTX() \
	napi_deferred deferred;    \
	napi_async_work worker;

#define NAPI_DEFINE_WORK_HANDLER(HANDLER)                 \
	static void HANDLER##_work(napi_env env, void *data); \
	static void HANDLER##_done(napi_env env, napi_status status, void *data);

#define NAPI_CREATE_PROMISED_WORK(VAR, CTX, HANDLER)                                      \
	napi_value VAR;                                                                       \
	{                                                                                     \
		napi_create_promise(env, &CTX->deferred, &VAR);                                   \
		NAPI_CREATE_STRING(resource_name, #HANDLER);                                      \
		napi_create_async_work(                                                           \
			env, NULL, resource_name, HANDLER##_work, HANDLER##_done, CTX, &CTX->worker); \
		napi_queue_async_work(env, CTX->worker);                                          \
	}

#endif  // __CP2102N_UTILS__
