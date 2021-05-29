#ifndef OPERATIONS_H
# define OPERATIONS_H
#include <js_native_api.h>

napi_value init_roo(napi_env env, napi_callback_info info);

napi_value update_config(napi_env env, napi_callback_info info);

napi_value instantiate_plugin(napi_env env, napi_callback_info info);

napi_value get_plugin_list(napi_env env, napi_callback_info info);

napi_value rooda_shutdown(napi_env env, napi_callback_info info);
#endif