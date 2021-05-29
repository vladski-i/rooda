#include "roo.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "interface.h"

config_t *config_from_js(napi_env env,napi_value object, config_t *old){
    config_t *conf = malloc(sizeof(config_t));
    char *buf = malloc(20 * sizeof(char));
    size_t copied;
    napi_value result;
    bool has;
    NAPI_CALL(env, napi_has_named_property(env,object,"mode",&has));
    if(has){
        NAPI_CALL(env, napi_get_named_property(env, object, "mode", &result));
        NAPI_CALL(env,napi_get_value_string_latin1(env, result, buf, 10, &copied));
        if(copied <= 0){
            log_error("failed to copy mode value!\n");
            free(buf);
            return NULL;
        }
        if(!strncmp("ZERO_FILL",buf,10)){
            log_debug("[roo] adding ZERO_FILL\n");
            conf->mode = ZERO_FILL;
        }
        else if (!strncmp("NO_FILL",buf,8)){
            log_debug("[roo] adding NO_FILL\n");
            conf->mode = NO_FILL;
        }        
        else if (!strncmp("SINE",buf,4)){
            log_debug("[roo] adding SINE\n");
            conf->mode = SINE;
        }
        else {
            log_error("Unrecognized mode %s\n!",buf);
            return NULL;
        }
    }
    else{
        log_debug("[roo] using old mode value\n");
        conf->mode = old->mode;
        free(buf);
        log_debug("[roo] using old mode value\n");
    }
    NAPI_CALL(env, napi_has_named_property(env,object,"windowSize",&has));
    if(has){
        NAPI_CALL(env, napi_get_named_property(env, object, "windowSize", &result));
        NAPI_CALL(env, napi_get_value_uint32(env, result, &(conf->window_size)));
        log_debug("[roo] window size copied\n");
    }
    else
        conf->window_size = old->window_size; 
    if(conf->mode == SINE || has){
        conf->window = calloc(conf->window_size, sizeof(float));
        for (unsigned t = 0; t < conf->window_size; t ++){
            conf->window[t] = sin(M_PI * (1.0 / conf->window_size) * t);
        }
        log_trace("[roo] updating sine window [");
        for (unsigned t = 0; t < conf->window_size; t ++)
            log_trace("%.2f,",conf->window[t]);
        log_trace("]\n");
    } 
    CHANNELED{
        conf->_roo[channel] = old->_roo[channel];
    }
    memcpy(old,conf,sizeof(config_t));
    return conf;
}

instantiate_request_t *instantiate_request_from_js(napi_env env,napi_value object){
    log_debug("[roo] instantiate_request_from_js called\n");
    instantiate_request_t *request = malloc(sizeof(instantiate_request_t));
    bool has;
    napi_value field;
    char *plugin_name = calloc(100, sizeof(char));
    long unsigned copied = 0;
    NAPI_CALL(env, napi_has_named_property(env,object,"plugin_name",&has));
    if(!has)
        return NULL;
    NAPI_CALL(env, napi_get_named_property(env, object, "plugin_name", &field));
    NAPI_CALL(env,napi_get_value_string_latin1(env, field, plugin_name, 100, &copied));
    request->plugin_name = plugin_name;
    NAPI_CALL(env, napi_has_named_property(env,object,"lane",&has));
    if(!has){
        free(request);
        return NULL;
    }
    NAPI_CALL(env, napi_get_named_property(env, object, "lane", &field));
    NAPI_CALL(env, napi_get_value_uint32(env, field, &(request->lane)));

    NAPI_CALL(env, napi_has_named_property(env,object,"index",&has));
    if(!has){
        free(request);
        return NULL;
    }
    NAPI_CALL(env, napi_get_named_property(env, object, "index", &field));
    NAPI_CALL(env, napi_get_value_uint32(env, field, &(request->index)));
    return request;
}

napi_value string_list_to_js(napi_env env, const char **list, uint32_t count){
    napi_value array;
    NAPI_CALL(env,napi_create_array_with_length(env, count, &array));
    for (unsigned i = 0; i < count; i++){
        napi_value string_value;
        NAPI_CALL(env, napi_create_string_latin1(env,list[i],strlen(list[i]),&string_value));
        NAPI_CALL(env,napi_set_element(env,array,i,string_value));
    }
    return array;
}