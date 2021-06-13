#include "operations.h"
#include <jack/jack.h>
#include <jack/types.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "callbacks.h"
#include "plugin.h"
#include "plugin.h"
#include "interface.h"
#include "startup.h"
#include "log.h"
#include "roo.h"
#include "lane.h"
#include "stereo.h"


#ifdef STEREO
const char *channel_names_suffix[] = {"L","R"};
#else
const char *channel_names_suffix[] = {"\0"};
#endif


jack_port_t *roo_out1[CHANNELS], *roo_out2[CHANNELS], *roo_in[CHANNELS];
jack_port_t *unroo_out[CHANNELS], *unroo_in1[CHANNELS], *unroo_in2[CHANNELS];
jack_client_t *roo_client, *unroo_client;
config_t *args;
napi_value NAPI_TRUE;
napi_value NAPI_FALSE;
#ifndef M_PI
#define M_PI (3.14159265)
#endif

static void jack_shutdown(void *arg)
{
	exit(1);
}

napi_value init_roo(napi_env env, napi_callback_info info)
{
	NAPI_CALL(env, napi_get_boolean(env,true,&NAPI_TRUE));
	NAPI_CALL(env, napi_get_boolean(env,false,&NAPI_FALSE));
	log_info("Roo initializing...\n");
	if(!init_carla_backend()){
		log_error("Failed to initialize carla\n");
		return NAPI_FALSE;
	}
	const char *roo_client_name = "roo";
	const char *unroo_client_name = "unroo";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	args = malloc(sizeof(config_t));
	/* initialize args*/
	args->mode = ZERO_FILL;
	args->window_size = 1;
	/* open a client connection to the JACK server */
	if (create_client(&roo_client, roo_client_name, options, server_name))
	{
		return NAPI_FALSE;
	}

	if (create_client(&unroo_client, unroo_client_name, options, server_name))
	{
		return NAPI_FALSE;
	}

	/* tell the JACK server to call the correct callback for each client whenever
	   there is work to be done.
	*/

	jack_set_process_callback(roo_client, roo_callback, args);
	jack_set_process_callback(unroo_client, unroo_callback, args);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown(roo_client, jack_shutdown, 0);
	jack_on_shutdown(unroo_client, jack_shutdown, 0);
    char ** roo_output1_names = calloc(CHANNELS, sizeof(char *));
    char ** roo_output2_names = calloc(CHANNELS, sizeof(char *));
    char ** roo_input_names = calloc(CHANNELS, sizeof(char *));
    char ** unroo_input1_names = calloc(CHANNELS, sizeof(char *));
    char ** unroo_input2_names = calloc(CHANNELS, sizeof(char *));
    char ** unroo_output_names = calloc(CHANNELS, sizeof(char *));
    log_debug("names arrays allocd\n");
    CHANNELED{
        roo_output1_names[channel] = calloc(20, sizeof(char));
        roo_output2_names[channel] = calloc(20, sizeof(char));
        roo_input_names[channel] = calloc(20, sizeof(char));
        unroo_input1_names[channel] = calloc(20, sizeof(char));
        unroo_input2_names[channel] = calloc(20, sizeof(char));
        unroo_output_names[channel] = calloc(20, sizeof(char));
        log_debug("names allocd\n");
        strcat(roo_output1_names[channel], "output1"); 
        strcat(roo_output2_names[channel], "output2"); 
        strcat(roo_input_names[channel], "input"); 
        strcat(unroo_input1_names[channel], "input1"); 
        strcat(unroo_input2_names[channel], "input2"); 
        strcat(unroo_output_names[channel], "output"); 
        log_debug("base name cat\n");
        strcat(roo_output1_names[channel], channel_names_suffix[channel]); 
        strcat(roo_output2_names[channel], channel_names_suffix[channel]); 
        strcat(roo_input_names[channel], channel_names_suffix[channel]); 
        strcat(unroo_input1_names[channel], channel_names_suffix[channel]); 
        strcat(unroo_input2_names[channel], channel_names_suffix[channel]); 
        strcat(unroo_output_names[channel], channel_names_suffix[channel]); 
        log_debug("suffix cat\n");
    }

	/* create two ports */
    CHANNELED{
        roo_out1[channel] = jack_port_register(roo_client, roo_output1_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);
    }
    CHANNELED{
        roo_out2[channel] = jack_port_register(roo_client, roo_output2_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);
    }
    CHANNELED{
        roo_in[channel] = jack_port_register(roo_client, roo_input_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    }
    CHANNELED{
        unroo_out[channel] = jack_port_register(unroo_client, unroo_output_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);
    }
    CHANNELED{
        unroo_in1[channel] = jack_port_register(unroo_client, unroo_input1_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    }
    CHANNELED{
        unroo_in2[channel] = jack_port_register(unroo_client, unroo_input2_names[channel],
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    }
    CHANNELED{
        if ((roo_in[channel] == NULL) || (roo_out1[channel] == NULL) || (roo_out2 [channel]== NULL) || 
            (unroo_in1[channel] == NULL) || (unroo_in2[channel] == NULL) || (unroo_out[channel] == NULL))
        {
            log_error("[roo] no more JACK ports available\n");
            return NAPI_FALSE;
        }
    }

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate(roo_client))
	{
		log_error("[roo] cannot activate roo_client");
		return NAPI_FALSE;
	}
	if (jack_activate(unroo_client))
	{
		log_error("[roo] cannot activate unroo_client");
		return NAPI_FALSE;
	}
	init_lanes_state(roo_client, unroo_client, 2);
	return NAPI_TRUE;
}


napi_value update_config(napi_env env, napi_callback_info info){
	log_debug("[roo]Update config called\n");
	size_t argc = 1;
	napi_value *argv = malloc(2 * sizeof(napi_value));
	napi_value this;
	NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &this, NULL));
	config_from_js(env,*argv, args, this);
	return NAPI_TRUE;
}

napi_value instantiate_plugin(napi_env env, napi_callback_info info){
	log_debug("[roo] Instantiate plugin called\n");
	size_t argc = 1;
	napi_value *argv = malloc(sizeof(napi_value));
	napi_value this;
	NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &this, NULL));
	instantiate_request_t *request = instantiate_request_from_js(env,*argv);
	if(!add_plugin(request))
		return NAPI_FALSE;
	return NAPI_TRUE;
}

napi_value get_plugin_list(napi_env env, napi_callback_info info){
	log_debug("[roo] Get plugin list called\n");
	uint32_t count = 0;
	const char ** list = get_lv2_uri_list(&count);
	return string_list_to_js(env,list,count);
}

napi_value rooda_shutdown(napi_env env, napi_callback_info info){
	log_debug("[roo] Rooda shutdown called\n");
	carla_cleanup();
	return NAPI_TRUE;
}