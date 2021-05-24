/** @file roo.c
 *
 * @brief This simple roo_client demonstrates the basic features of JACK
 * as they would be used by many applications.
 */

#include <jack/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <jack/jack.h>
#include "startup.h"
#include "callbacks.h"
#include <js_native_api.h>
#include "roo.h"
#include "interface.h"
#include "log.h"
#include "plugin.h"
#include "lane.h"

jack_port_t *roo_out1, *roo_out2, *roo_in;
jack_port_t *unroo_out, *unroo_in1, *unroo_in2;
jack_client_t *roo_client, *unroo_client;
config_t *args;
napi_value NAPI_TRUE;
napi_value NAPI_FALSE;

#ifndef M_PI
#define M_PI (3.14159265)
#endif

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the roo/unroo_client.
 */
void jack_shutdown(void *arg)
{
	exit(1);
}

static napi_value init_roo(napi_env env, napi_callback_info info)
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

	/* create two ports */

	roo_out1 = jack_port_register(roo_client, "output1",
								  JACK_DEFAULT_AUDIO_TYPE,
								  JackPortIsOutput, 0);

	roo_out2 = jack_port_register(roo_client, "output2",
								  JACK_DEFAULT_AUDIO_TYPE,
								  JackPortIsOutput, 0);

	roo_in = jack_port_register(roo_client, "input",
								JACK_DEFAULT_AUDIO_TYPE,
								JackPortIsInput, 0);

	unroo_out = jack_port_register(unroo_client, "output",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsOutput, 0);

	unroo_in1 = jack_port_register(unroo_client, "input1",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsInput, 0);

	unroo_in2 = jack_port_register(unroo_client, "input2",
								   JACK_DEFAULT_AUDIO_TYPE,
								   JackPortIsInput, 0);

	if ((roo_in == NULL) || (roo_out1 == NULL) || (roo_out2 == NULL) || (unroo_in1 == NULL) || (unroo_in2 == NULL) || (unroo_out == NULL))
	{
		log_error("no more JACK ports available\n");
		return NAPI_FALSE;
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate(roo_client))
	{
		log_error("cannot activate roo_client");
		return NAPI_FALSE;
	}
	if (jack_activate(unroo_client))
	{
		log_error("cannot activate unroo_client");
		return NAPI_FALSE;
	}
	return NAPI_TRUE;
}


static napi_value update_config(napi_env env, napi_callback_info info){
	log_debug("[roo]Update config called\n");
	size_t argc = 1;
	napi_value *argv = malloc(2 * sizeof(napi_value));
	napi_value this;
	NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &this, NULL));
	config_from_js(env,*argv, args);
	log_debug("[roo]Update_config called with values: { \"window_size\" : %d, \"mode\" : %d }\n",args->window_size,args->mode);
	init_lanes_state(roo_client,unroo_client, 2);
	return NAPI_TRUE;
}

static napi_value instantiate_plugin(napi_env env, napi_callback_info info){
	log_debug("[roo] Instantiate plugin called\n");
	size_t argc = 1;
	napi_value *argv = malloc(sizeof(napi_value));
	napi_value this;
	NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &this, NULL));
	instantiate_request_t *request = instantiate_request_from_js(env,*argv);
	if(!add_plugin(request->plugin_name))
		return NAPI_FALSE;
	return NAPI_TRUE;
}

static napi_value get_plugin_list(napi_env env, napi_callback_info info){
	uint32_t count = 0;
	const char ** list = get_lv2_uri_list(&count);
	return string_list_to_js(env,list,count);
}

static napi_value rooda_shutdown(napi_env env, napi_callback_info info){
	carla_cleanup();
	return NAPI_TRUE;
}
napi_value create_addon(napi_env env) {
	log_set_level(LOG_DEBUG);
	napi_value module;
	NAPI_CALL(env, napi_create_object(env, &module));

	napi_value init;
	NAPI_CALL(env, napi_create_function(env,
										"init",
										NAPI_AUTO_LENGTH,
										init_roo,
										NULL,
										&init));

	NAPI_CALL(env, napi_set_named_property(env,
											module,
											"init",
											init));

	napi_value update;
	NAPI_CALL(env, napi_create_function(env,
									"updateConfig",
									NAPI_AUTO_LENGTH,
									update_config,
									NULL,
									&update));

	NAPI_CALL(env, napi_set_named_property(env,
										module,
										"updateConfig",
										update));

	napi_value instantiate;
	NAPI_CALL(env, napi_create_function(env,
									"instantiatePlugin",
									NAPI_AUTO_LENGTH,
									instantiate_plugin,
									NULL,
									&instantiate));

	NAPI_CALL(env, napi_set_named_property(env,
										module,
										"instantiatePlugin",
										instantiate));
	napi_value get_list;
	NAPI_CALL(env, napi_create_function(env,
									"getPluginList",
									NAPI_AUTO_LENGTH,
									get_plugin_list,
									NULL,
									&get_list));

	NAPI_CALL(env, napi_set_named_property(env,
										module,
										"getPluginList",
										get_list));
	napi_value shutdown;
	NAPI_CALL(env, napi_create_function(env,
									"shutdown",
									NAPI_AUTO_LENGTH,
									rooda_shutdown,
									NULL,
									&shutdown));

	NAPI_CALL(env, napi_set_named_property(env,
										module,
										"shutdown",
										shutdown));
	return module;
}