#include "roo.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "interface.h"

config_t *fromJs(napi_env env,napi_value object, config_t *old){
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
            log_debug("failed to copy mode value!\n");
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
        log_debug("[roo] updating sine window [");
        for (unsigned t = 0; t < conf->window_size; t ++)
            log_debug("%.2f,",conf->window[t]);
        log_debug("]\n");
    } 

    conf->_roo = old->_roo;
    memcpy(old,conf,sizeof(config_t));
    return conf;
    // conf->window_size = 
}

napi_value toJS(napi_env env, config_t object){
    return NULL;
}