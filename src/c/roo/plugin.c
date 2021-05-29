// #include <CarlaHost.h>
#include "carla/CarlaNativePlugin.h"
#include <string.h>
#include "plugin.h"
#include "log.h"
#include "map.h"
#include <lilv-0/lilv/lilv.h>
#include <stdlib.h>
#include "types.h"
#include <jack/jack.h>
#include "lane.h"

CarlaHostHandle carla_handle;
map plugin_map;


bool init_carla_backend(){
    plugin_map = map_create();
    carla_handle = carla_standalone_host_init();
    uint32_t driver_count = carla_get_engine_driver_count();
    bool have_jack = false;
    bool err = false;
    log_info("Found %d drivers\n",driver_count);
    for (unsigned int i = 0; i < driver_count; i++){
        const char *driver_name = carla_get_engine_driver_name(i);
        log_debug("driver: %s|",driver_name);
        if(!strncmp("JACK",driver_name,4)){
            carla_engine_init(carla_handle,driver_name,"Roo");
            have_jack = true;
            break;
        }
    }
    if(!have_jack){
        log_error("JACK driver not available!");
        err = true;
    }
    return !err;
}

const char ** get_lv2_uri_list(uint32_t *count){
    log_debug("get_lv2_uri_list()\n");
    LilvWorld *world = lilv_world_new();
    lilv_world_load_all(world);
    const LilvPlugins* plugins = lilv_world_get_all_plugins(world);
    unsigned plugins_count = lilv_plugins_size(plugins);
    *count = plugins_count;
    LilvIter *i = lilv_plugins_begin(plugins);
    const char ** plugin_name_list = calloc(plugins_count,sizeof(char *));
    unsigned j = 0;
    while (!lilv_plugins_is_end(plugins,i)){
        const LilvPlugin *plugin = lilv_plugins_get(plugins,i);
        const LilvNode *uri = lilv_plugin_get_uri(plugin);
        const LilvNode *name = lilv_plugin_get_name(plugin);
        const char *plugin_name = lilv_node_as_string(name);
        const char *plugin_uri = lilv_node_as_uri(uri);
        log_trace("plugin name: %s",plugin_name);
        log_trace("plugin uri: %s",plugin_uri);
        map_set(plugin_map,plugin_name,(void*)plugin_uri);
        plugin_name_list[j++] = plugin_name;
        i = lilv_plugins_next(plugins,i);
    }
    return plugin_name_list;
}

void carla_cleanup(){
    carla_engine_close(carla_handle);
}

//dog shit: no support for multiple instances of the same plugin
bool add_plugin(instantiate_request_t *request){
    char *plugin_name = request->plugin_name;
    uint lane = request->lane;
    uint index = request->index;
    log_debug("add_plugin(%s,%d,%d)",plugin_name,index,lane);
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->name = plugin_name;
    plugin->id = lane * 100 + index;
    const char *plugin_uri = map_get(plugin_map,plugin_name);
    if(!plugin_uri)
        return false;
    if(!carla_add_plugin(carla_handle, BINARY_NATIVE, PLUGIN_LV2, plugin->name, "", plugin_uri, 0, NULL, 0x0))
        return false;
    add_plugin_to_lane(request,plugin);
    return true;
}