#pragma once

#include <stdint.h>

#include "node_api.h"

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
