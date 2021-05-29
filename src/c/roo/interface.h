#ifndef INTERFACE_H
# define INTERFACE_H

#include "types.h"
#include <js_native_api.h>

config_t *config_from_js(napi_env env,napi_value object, config_t *args);
instantiate_request_t *instantiate_request_from_js(napi_env env,napi_value object);
napi_value string_list_to_js(napi_env env, const char **list, uint32_t count);

#endif