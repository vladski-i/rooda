#include <jack/jack.h>
#include <jack/types.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "lane.h"
#include "log.h"
#include "regex.h"

roo_lanes_state_t *lanes;
jack_client_t *client;

//TODO: use regex capture groups to replace ( with \(
static void sanitize_parens(char *name){
    char *parens = strchr(name, '(');
    while(parens){
        *parens = '.';
        parens = strchr(parens, '(');
    }
    parens = strchr(name, ')');
    while(parens){
        *parens = '.';
        parens = strchr(parens, ')');
    }
}

roo_plugin_t *get_roo_plugin_for_lane(jack_client_t *roo, int lane_no){
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));    
    plugin->in_port_names = calloc(4, sizeof(char*));
    plugin->out_port_names = calloc(4, sizeof(char*));
    plugin->name = calloc(4,sizeof(char));
    sprintf(plugin->name,"roo");
    char *name_regex = calloc(20, sizeof(char));
    sprintf(name_regex,"^roo:output%d",(unsigned short)(lane_no + 1));
    unsigned i = 0;
    const char **ports = jack_get_ports(roo,name_regex,"",JackPortIsOutput);
    i = 0;
    while(*ports){
        log_debug("%s_out_port: %s\n","roo",*ports);
        jack_port_t *port = jack_port_by_name(roo, *ports);
        log_debug("jack_port_t :%d\n",port);
        plugin->out_ports[i] = port;
        plugin->out_port_names[i] = strdup(*ports);
        ports ++;
        i++;
    }
    plugin->no_ports = i;
    return plugin;
}

roo_plugin_t *get_unroo_plugin_for_lane(jack_client_t *unroo, int lane_no){
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));
    plugin->in_port_names = calloc(4, sizeof(char*));
    plugin->out_port_names = calloc(4, sizeof(char*));
    plugin->name = calloc(7,sizeof(char));
    sprintf(plugin->name,"unroo");
    char *name_regex = calloc(20, sizeof(char));
    sprintf(name_regex,"^unroo:input%d",(unsigned short)(lane_no + 1));
    unsigned i = 0;
    const char **ports = jack_get_ports(unroo,name_regex,"",JackPortIsInput);
    while(*ports){
        log_debug("[roo] %s_in_port: %s\n","unroo",*ports);
        jack_port_t *port = jack_port_by_name(unroo, *ports);
        log_debug("[roo] jack_port_t :%d\n",port);
        plugin->in_ports[i] = port;
        plugin->in_port_names[i] = strdup(*ports);
        ports ++;
        i++;
    }
    plugin->no_ports = i;
    return plugin;
}

void fill_plugin_data(jack_client_t *client, roo_plugin_t *plugin){
    log_debug("fill_plugin_data(%d,%d)",client,plugin);
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));
    plugin->in_port_names = calloc(4, sizeof(char*));
    plugin->out_port_names = calloc(4, sizeof(char*));
    char *name_regex = calloc(strlen(plugin->name) + 2,sizeof(char));
    sprintf(name_regex, "^%s",plugin->name);
    // Sanitize parens out of regex
    sanitize_parens(name_regex);
    log_debug("name regex: %s\n",name_regex);
    const char **ports = jack_get_ports(client, name_regex,"",JackPortIsInput);
    log_debug("ports: %d\n",ports);
    unsigned i = 0;
    while(ports && *ports){
        log_debug("[roo] %s_in_port: %s\n",plugin->name,*ports);
        jack_port_t *port = jack_port_by_name(client, *ports);
        log_debug("[roo] jack_port_t :%d\n",port);
        plugin->in_ports[i] = port;
        plugin->in_port_names[i] = strdup(*ports);
        ports ++;
        i++;
    }
    plugin->no_ports = i;
    ports = jack_get_ports(client,name_regex,"",JackPortIsOutput);
    i = 0;
    while(ports && *ports){
        log_debug("[roo] %s_out_port: %s\n",plugin->name,*ports);
        jack_port_t *port = jack_port_by_name(client, *ports);
        log_debug("[roo] jack_port_t :%d\n",port);
        plugin->out_ports[i] = port;
        plugin->out_port_names[i] = strdup(*ports);
        ports ++;
        i++;
    }
    plugin->no_ports = fmin(plugin->no_ports,i);
    log_debug("[roo] %d ports for %s\n",plugin->no_ports,plugin->name);
}

static void print_lane_state(roo_lane_state_t *lane_state, int lane_no){
    roo_lane_state_iter_t *i = lane_state->iter;
    char *show = calloc(1000, sizeof(char));
    while(i != NULL){
        strcat(show,i->plugin->name);
        strcat(show,"->");
        i = i ->next;
    }
    log_debug("[roo] lane %d:%s\n",lane_no,show);
    free(show);
}

roo_lane_state_t *init_lane_state(jack_client_t *roo, jack_client_t *unroo, int lane_no){
    log_debug("init_lane_state(%d,%d,%d)",roo,unroo,lane_no);
    roo_plugin_t *roo_plugin = get_roo_plugin_for_lane(roo,lane_no);
    roo_plugin_t *unroo_plugin = get_unroo_plugin_for_lane(unroo,lane_no);
    roo_lane_state_t *lane_state = malloc(sizeof(roo_lane_state_t));
    lane_state->plugins_no = 2;
    roo_lane_state_iter_t *start = malloc(sizeof(roo_lane_state_iter_t));
    start->plugin = roo_plugin;
    roo_lane_state_iter_t *end = malloc(sizeof(roo_lane_state_iter_t));
    end->plugin = unroo_plugin;
    end->next = NULL;
    start->next = end;
    lane_state->iter = start;
    for (uint i = 0; i < fmin(roo_plugin->no_ports, unroo_plugin->no_ports); i ++){
        log_debug("[roo] connecting %s to %s\n",roo_plugin->out_port_names[i],unroo_plugin->in_port_names[i]);
        jack_connect(roo, roo_plugin->out_port_names[i], unroo_plugin->in_port_names[i]);
    }
    print_lane_state(lane_state, lane_no);
    return lane_state;
}

void init_lanes_state(jack_client_t *roo, jack_client_t *unroo, uint lanes_no){
    log_debug("init_lanes_state(%d,%d,%d)",roo,unroo,lanes_no);
    client = roo;
    roo_lanes_state_t *lanes_state = malloc(sizeof(roo_lanes_state_t));
    lanes_state->lanes_no=lanes_no;
    lanes_state->lanes = malloc(lanes_no * sizeof(roo_lane_state_t));
    for(unsigned i = 0; i < lanes_no; i ++){
        lanes_state->lanes[i] = *init_lane_state(roo,unroo,i);
    }
    lanes = lanes_state;
}

bool check_for_replace(instantiate_request_t *request){
    log_debug("check_for_replace(%d)\n",request);
    log_debug("index: %d, plugins: %d\n",request->index,lanes->lanes[request->lane].plugins_no );
    if(request->index < lanes->lanes[request->lane].plugins_no - 2){
        log_debug("plugin %d on lane %d is replaceable\n", request->index, request->lane);
        return true;
    }
    return false;
}

bool remove_plugin_from_lane(instantiate_request_t *request){
    log_debug("remove_plugin_from_lane(%d_\n",request);
    roo_lane_state_t *lane = &(lanes->lanes[request->lane]);
    roo_lane_state_iter_t *prev = lane->iter;
    uint index = 0;
    for(roo_lane_state_iter_t *i = lane->iter->next; i != NULL; i = i->next, index ++){
        if(index == request->index){
            prev->next = i->next;
            free(i->plugin);
            free(i);
            lane->plugins_no--;
            print_lane_state(lane,request->lane);
            return true;
        }
        prev = i;
    }
    return false;
}

bool add_plugin_to_lane(instantiate_request_t *request, roo_plugin_t *plugin){
    log_debug("add_plugin_to_lane(%d,%d)",lanes,request);
    log_debug("at index %d\n",request->index);
    fill_plugin_data(client, plugin);
    uint lane_no = request->lane;
    roo_lane_state_t *lane = &(lanes->lanes[lane_no]);
    roo_lane_state_iter_t *prev = lane->iter;
    roo_lane_state_iter_t *new = malloc(sizeof(roo_lane_state_iter_t));
    new->plugin = plugin;
    uint index = 0;
    for(roo_lane_state_iter_t *i = lane->iter->next; i != NULL; i = i->next, index ++){
        if(index == request->index){
            prev->next = new;
            new->next = i;
            for (uint j = 0; j < fmin(prev->plugin->no_ports, i->plugin->no_ports); j ++){
                log_debug("[roo] disconnecting %s and %s\n",prev->plugin->out_port_names[j],i->plugin->in_port_names[j]);
                jack_disconnect(client,prev->plugin->out_port_names[j],i->plugin->in_port_names[j]);
            }
            for (uint j = 0; j < fmin(prev->plugin->no_ports, plugin->no_ports); j ++){
                log_debug("[roo] connecting %s and %s\n",prev->plugin->out_port_names[j],plugin->in_port_names[j]);
                jack_connect(client,prev->plugin->out_port_names[j],plugin->in_port_names[j]);
            }
            for (uint j = 0; j < fmin(plugin->no_ports, i->plugin->no_ports); j ++){
                log_debug("[roo] connecting %s and %s\n",plugin->out_port_names[j],i->plugin->in_port_names[j]);
                jack_connect(client, plugin->out_port_names[j],i->plugin->in_port_names[j]);
            }
            break;
        }
        prev = i;
    }
    lane->plugins_no++;
    print_lane_state(lane, lane_no);
    return true;
}