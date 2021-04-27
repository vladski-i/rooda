#include "config.h"
#include <js_native_api.h>
#include "roo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

config_t *fromJs(napi_env env,napi_value object){
    config_t *conf = malloc(sizeof(config_t));
    char *buf = malloc(20 * sizeof(char));
    size_t copied;
    napi_value result;
    NAPI_CALL(env, napi_get_named_property(env, object, "windowSize", &result));
    NAPI_CALL(env, napi_get_value_uint32(env, result, &(conf->window_size)));
    NAPI_CALL(env, napi_get_named_property(env, object, "mode", &result));
    NAPI_CALL(env,napi_get_value_string_latin1(env, result, buf, 10, &copied));
    if(copied <= 0){
        printf("failed to copy mode value!\n");
        free(buf);
        return NULL;
    }
    if(strncmp("ZERO_FILL",buf,10)){
        conf->mode = ZERO_FILL;
    }
    else if (strncmp("NO_FILL",buf,8)){
        conf->mode = NO_FILL;
    }
    else {
        printf("Unrecognized mode %s\n!",buf);
        return NULL;
    }
    return conf;
    // conf->window_size = 
}

napi_value toJS(napi_env env, config_t object){
    return NULL;
}