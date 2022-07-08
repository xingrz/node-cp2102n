#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cp2102.h"
#include "node_api.h"
#include "utils.h"

static napi_value open_interface(napi_env env, napi_callback_info info);
static napi_value interface_get(napi_env env, napi_callback_info info);
static napi_value interface_set(napi_env env, napi_callback_info info);
static napi_value interface_get_serial_number(napi_env env, napi_callback_info info);
static napi_value interface_close(napi_env env, napi_callback_info info);

typedef struct {
	char path[255];
	cp2102_dev_t *dev;
	NAPI_DEFINE_WORK_CTX();
} open_interface_t;

NAPI_DEFINE_WORK_HANDLER(open_interface);

static napi_value
open_interface(napi_env env, napi_callback_info info)
{
	NAPI_ASSERT_ARGC(1);
	NAPI_ASSERT_ARG_TYPE(0, napi_string);

	open_interface_t *ctx = (open_interface_t *)malloc(sizeof(open_interface_t));
	napi_get_value_string_utf8(env, args[0], ctx->path, sizeof(ctx->path), NULL);

	NAPI_CREATE_PROMISED_WORK(promise, ctx, open_interface);
	return promise;
}

static void
open_interface_work(napi_env env, void *data)
{
	open_interface_t *ctx = (open_interface_t *)data;
	ctx->dev = cp2102_open((const char *)ctx->path);
}

static void
open_interface_done(napi_env env, napi_status status, void *data)
{
	open_interface_t *ctx = (open_interface_t *)data;

	if (ctx->dev != NULL) {
		NAPI_CREATE_OBJECT(result);

		NAPI_CREATE_FUNTION(func_get, "get", interface_get, ctx->dev);
		napi_set_named_property(env, result, "get", func_get);

		NAPI_CREATE_FUNTION(func_set, "set", interface_set, ctx->dev);
		napi_set_named_property(env, result, "set", func_set);

		NAPI_CREATE_FUNTION(
			func_get_serial_number, "get_serial_number", interface_get_serial_number, ctx->dev);
		napi_set_named_property(env, result, "getSerialNumber", func_get_serial_number);

		NAPI_CREATE_FUNTION(func_close, "close", interface_close, ctx->dev);
		napi_set_named_property(env, result, "close", func_close);

		napi_resolve_deferred(env, ctx->deferred, result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed opening device");
		napi_reject_deferred(env, ctx->deferred, error);
	}

	napi_delete_async_work(env, ctx->worker);
	free(ctx);
}

typedef struct {
	cp2102_dev_t *dev;
	uint8_t read_bits;
	bool succeed;
	NAPI_DEFINE_WORK_CTX();
} interface_get_t;

NAPI_DEFINE_WORK_HANDLER(interface_get);

static napi_value
interface_get(napi_env env, napi_callback_info info)
{
	cp2102_dev_t *dev;
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&dev);

	interface_get_t *ctx = (interface_get_t *)malloc(sizeof(interface_get_t));
	ctx->dev = dev;

	NAPI_CREATE_PROMISED_WORK(promise, ctx, interface_get);
	return promise;
}

static void
interface_get_work(napi_env env, void *data)
{
	interface_get_t *ctx = (interface_get_t *)data;
	ctx->succeed = cp2102_get_value(ctx->dev, &ctx->read_bits);
}

static void
interface_get_done(napi_env env, napi_status status, void *data)
{
	interface_get_t *ctx = (interface_get_t *)data;

	if (ctx->succeed) {
		NAPI_CREATE_UINT32(result, ctx->read_bits);
		napi_resolve_deferred(env, ctx->deferred, result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed reading device");
		napi_reject_deferred(env, ctx->deferred, error);
	}

	napi_delete_async_work(env, ctx->worker);
	free(ctx);
}

typedef struct {
	cp2102_dev_t *dev;
	uint8_t write_bits;
	uint8_t write_mask;
	uint8_t read_bits;
	bool succeed;
	NAPI_DEFINE_WORK_CTX();
} interface_set_t;

NAPI_DEFINE_WORK_HANDLER(interface_set);

static napi_value
interface_set(napi_env env, napi_callback_info info)
{
	NAPI_ASSERT_ARGC(2);
	NAPI_ASSERT_ARG_TYPE(0, napi_number);
	NAPI_ASSERT_ARG_TYPE(1, napi_number);

	cp2102_dev_t *dev;
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&dev);

	interface_set_t *ctx = (interface_set_t *)malloc(sizeof(interface_set_t));
	ctx->dev = dev;
	napi_get_value_uint32(env, args[0], (uint32_t *)&ctx->write_bits);
	napi_get_value_uint32(env, args[1], (uint32_t *)&ctx->write_mask);

	NAPI_CREATE_PROMISED_WORK(promise, ctx, interface_set);
	return promise;
}

static void
interface_set_work(napi_env env, void *data)
{
	interface_set_t *ctx = (interface_set_t *)data;
	ctx->succeed = cp2102_set_value(ctx->dev, ctx->write_bits, ctx->write_mask) &&
				   cp2102_get_value(ctx->dev, &ctx->read_bits);
}

static void
interface_set_done(napi_env env, napi_status status, void *data)
{
	interface_set_t *ctx = (interface_set_t *)data;

	if (ctx->succeed) {
		NAPI_CREATE_UINT32(result, ctx->read_bits);
		napi_resolve_deferred(env, ctx->deferred, result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed writing device");
		napi_reject_deferred(env, ctx->deferred, error);
	}

	napi_delete_async_work(env, ctx->worker);
	free(ctx);
}

typedef struct {
	cp2102_dev_t *dev;
	const char *serial_number;
	NAPI_DEFINE_WORK_CTX();
} interface_get_serial_number_t;

NAPI_DEFINE_WORK_HANDLER(interface_get_serial_number);

static napi_value
interface_get_serial_number(napi_env env, napi_callback_info info)
{
	cp2102_dev_t *dev;
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&dev);

	interface_get_serial_number_t *ctx =
		(interface_get_serial_number_t *)malloc(sizeof(interface_get_serial_number_t));
	ctx->dev = dev;

	NAPI_CREATE_PROMISED_WORK(promise, ctx, interface_get_serial_number);
	return promise;
}

static void
interface_get_serial_number_work(napi_env env, void *data)
{
	interface_get_serial_number_t *ctx = (interface_get_serial_number_t *)data;
	ctx->serial_number = cp2102_get_serial_number(ctx->dev);
}

static void
interface_get_serial_number_done(napi_env env, napi_status status, void *data)
{
	interface_get_serial_number_t *ctx = (interface_get_serial_number_t *)data;

	NAPI_CREATE_STRING(result, ctx->serial_number);
	napi_resolve_deferred(env, ctx->deferred, result);

	napi_delete_async_work(env, ctx->worker);
	free(ctx);
}

static napi_value
interface_close(napi_env env, napi_callback_info info)
{
	cp2102_dev_t *dev;
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&dev);
	cp2102_close(&dev);
	return NULL;
}

static void
module_deinit(napi_env env, void *finalize_data, void *finalize_hint)
{
	cp2102_exit();
}

NAPI_MODULE_INIT()
{
	cp2102_init();
	napi_set_instance_data(env, NULL, module_deinit, NULL);

	NAPI_CREATE_FUNTION(func_open_interface, "openInterface", open_interface, NULL);
	napi_set_named_property(env, exports, "openInterface", func_open_interface);

	return exports;
}
