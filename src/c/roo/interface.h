#include "config.h"
#include <js_native_api.h>

config_t *fromJs(napi_env env,napi_value object);
napi_value toJS(napi_env env, config_t object);