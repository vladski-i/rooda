/** @file roo.c
 *
 * @brief This simple roo_client demonstrates the basic features of JACK
 * as they would be used by many applications.
 */

#ifndef WIN32
#include <unistd.h>
#endif
#include <jack/jack.h>
#include <js_native_api.h>
#include "roo.h"
#include "log.h"
#include "operations.h"


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