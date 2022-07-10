#include "async.h"
#include "cp2102.h"
#include "node_api.h"
#include "utils.h"

static napi_value open_interface(napi_env env, napi_callback_info info);
static napi_value interface_get(napi_env env, napi_callback_info info);
static napi_value interface_set(napi_env env, napi_callback_info info);
static napi_value interface_get_serial_number(napi_env env, napi_callback_info info);
static napi_value interface_close(napi_env env, napi_callback_info info);

NAPI_ASYNC(open_interface, {
	char path[255];
	cp2102_dev_t *dev;
})

static napi_value
open_interface(napi_env env, napi_callback_info info)
{
	NAPI_ASSERT_ARGC(1);
	NAPI_ASSERT_ARG_TYPE(0, napi_string);

	NAPI_ASYNC_CREATE(promise, ctx, open_interface);
	napi_get_value_string_utf8(env, args[0], ctx->path, sizeof(ctx->path), NULL);

	NAPI_ASYNC_QUEUE(promise);
	return promise;
}

NAPI_ASYNC_WORK(open_interface, { ctx->dev = cp2102_open((const char *)ctx->path); })

NAPI_ASYNC_DONE(open_interface, {
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

		NAPI_ASYNC_RESOLVE(result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed opening device");
		NAPI_ASYNC_REJECT(error);
	}
})

NAPI_ASYNC(interface_get, {
	cp2102_dev_t *dev;
	uint8_t read_bits;
	bool succeed;
})

static napi_value
interface_get(napi_env env, napi_callback_info info)
{
	cp2102_dev_t *dev;
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&dev);

	NAPI_ASYNC_CREATE(promise, ctx, interface_get);
	ctx->dev = dev;

	NAPI_ASYNC_QUEUE(promise);
	return promise;
}

NAPI_ASYNC_WORK(interface_get, { ctx->succeed = cp2102_get_value(ctx->dev, &ctx->read_bits); })

NAPI_ASYNC_DONE(interface_get, {
	if (ctx->succeed) {
		NAPI_CREATE_UINT32(result, ctx->read_bits);
		NAPI_ASYNC_RESOLVE(result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed reading device");
		NAPI_ASYNC_REJECT(error);
	}
})

NAPI_ASYNC(interface_set, {
	cp2102_dev_t *dev;
	uint8_t write_bits;
	uint8_t write_mask;
	uint8_t read_bits;
	bool succeed;
})

static napi_value
interface_set(napi_env env, napi_callback_info info)
{
	NAPI_ASSERT_ARGC(2);
	NAPI_ASSERT_ARG_TYPE(0, napi_number);
	NAPI_ASSERT_ARG_TYPE(1, napi_number);

	NAPI_ASYNC_CREATE(promise, ctx, interface_set);
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&ctx->dev);
	napi_get_value_uint32(env, args[0], (uint32_t *)&ctx->write_bits);
	napi_get_value_uint32(env, args[1], (uint32_t *)&ctx->write_mask);

	NAPI_ASYNC_QUEUE(promise);
	return promise;
}

NAPI_ASYNC_WORK(interface_set, {
	ctx->succeed = cp2102_set_value(ctx->dev, ctx->write_bits, ctx->write_mask) &&
				   cp2102_get_value(ctx->dev, &ctx->read_bits);
})

NAPI_ASYNC_DONE(interface_set, {
	if (ctx->succeed) {
		NAPI_CREATE_UINT32(result, ctx->read_bits);
		NAPI_ASYNC_RESOLVE(result);
	} else {
		NAPI_CREATE_ERROR(error, "IoError", "Failed writing device");
		NAPI_ASYNC_REJECT(error);
	}
})

NAPI_ASYNC(interface_get_serial_number, {
	cp2102_dev_t *dev;
	const char *serial_number;
})

static napi_value
interface_get_serial_number(napi_env env, napi_callback_info info)
{
	NAPI_ASYNC_CREATE(promise, ctx, interface_get_serial_number);
	napi_get_cb_info(env, info, NULL, NULL, NULL, (void **)&ctx->dev);

	NAPI_ASYNC_QUEUE(promise);
	return promise;
}

NAPI_ASYNC_WORK(
	interface_get_serial_number, { ctx->serial_number = cp2102_get_serial_number(ctx->dev); })

NAPI_ASYNC_DONE(interface_get_serial_number, {
	NAPI_CREATE_STRING(result, ctx->serial_number);
	NAPI_ASYNC_RESOLVE(result);
})

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
