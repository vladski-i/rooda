#include <jack/jack.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "lane.h"
#include "log.h"

roo_plugin_t *get_roo_plugin_for_lane(jack_client_t *roo, int lane_no){
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));
    plugin->name = calloc(4,sizeof(char));
    sprintf(plugin->name,"roo");
    char *name_regex = calloc(20, sizeof(char));
    sprintf(name_regex,"^roo:output%d",lane_no);
    unsigned i = 0;
    const char **ports = jack_get_ports(roo,name_regex,"",JackPortIsOutput);
    i = 0;
    while(*ports){
        log_debug("%s_out_port: %s\n","roo",*ports);
        jack_port_t *port = jack_port_by_name(roo, *ports);
        log_debug("jack_port_t :%d\n",port);
        plugin->out_ports[i] = port;
        ports ++;
    }
    return plugin;
}

roo_plugin_t *get_unroo_plugin_for_lane(jack_client_t *unroo, int lane_no){
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));
    plugin->name = calloc(7,sizeof(char));
    sprintf(plugin->name,"unroo");
    char *name_regex = calloc(20, sizeof(char));
    sprintf(name_regex,"^unroo:input%d",lane_no);
    unsigned i = 0;
    const char **ports = jack_get_ports(unroo,name_regex,"",JackPortIsInput);
    i = 0;
    while(*ports){
        log_debug("%s_in_port: %s\n","roo",*ports);
        jack_port_t *port = jack_port_by_name(unroo, *ports);
        log_debug("jack_port_t :%d\n",port);
        plugin->out_ports[i] = port;
        ports ++;
    }
    return plugin;
}

roo_plugin_t *get_plugin_data_by_jack_client(jack_client_t *client, char *plugin_name){
    log_debug("get_plugin_data_by_jack_client(%d,%s)",client,plugin_name);
    roo_plugin_t *plugin = malloc(sizeof(roo_plugin_t));
    plugin->in_ports = calloc(4, sizeof(jack_port_t*));
    plugin->out_ports = calloc(4, sizeof(jack_port_t*));
    plugin->name = plugin_name;
    char *name_regex = calloc(strlen(plugin_name) + 2,sizeof(char));
    name_regex[0] = '^'; //begin string regex
    memcpy(name_regex + 1, plugin_name, strlen(plugin_name));
    const char **ports = jack_get_ports(client, name_regex,"",JackPortIsInput);
    unsigned i = 0;
    while(*ports){
        log_debug("%s_in_port: %s\n",plugin_name,*ports);
        jack_port_t *port = jack_port_by_name(client, *ports);
        log_debug("jack_port_t :%d\n",port);
        plugin->in_ports[i] = port;
        ports ++;
    }
    ports = jack_get_ports(client,name_regex,"",JackPortIsOutput);
    i = 0;
    while(*ports){
        log_debug("%s_out_port: %s\n",plugin_name,*ports);
        jack_port_t *port = jack_port_by_name(client, *ports);
        log_debug("jack_port_t :%d\n",port);
        plugin->out_ports[i] = port;
        ports ++;
    }
    return plugin;
}

static void print_lane_state(roo_lane_state_t *lane_state, int lane_no){
    roo_lane_state_iter_t *i = lane_state->iter;
    char *show = calloc(1000, sizeof(char));
    while(i != NULL){
        strcat(show,i->plugin->name);
        strcat(show,"->");
        i = i ->next;
    }
    log_debug("lane %d:%s\n",lane_no,show);
    free(show);
}

roo_lane_state_t *init_lane_state(jack_client_t *roo, jack_client_t *unroo, int lane_no){
    log_debug("init_lane_state(%d,%d,%d)",roo,unroo,lane_no);
    roo_plugin_t *roo_plugin = get_roo_plugin_for_lane(roo,1);
    roo_plugin_t *unroo_plugin = get_unroo_plugin_for_lane(unroo,1);
    roo_lane_state_t *lane_state = malloc(sizeof(roo_lane_state_t));
    lane_state->plugins_no = 2;
    roo_lane_state_iter_t *start = malloc(sizeof(roo_lane_state_iter_t));
    start->plugin = roo_plugin;
    roo_lane_state_iter_t *end = malloc(sizeof(roo_lane_state_iter_t));
    end->plugin = unroo_plugin;
    end->next = NULL;
    start->next = end;
    lane_state->iter = start;
    print_lane_state(lane_state, lane_no);
    return lane_state;
}

roo_lanes_state_t *init_lanes_state(jack_client_t *roo, jack_client_t *unroo, int lanes_no){
    log_debug("init_lanes_state(%d,%d,%d)",roo,unroo,lanes_no);
    roo_lanes_state_t *lanes_state = malloc(sizeof(roo_lanes_state_t));
    lanes_state->lanes_no=lanes_no;
    lanes_state->lanes = malloc(lanes_no * sizeof(roo_lane_state_t));
    for(unsigned i = 0; i < lanes_no; i ++){
        lanes_state->lanes[i] = *init_lane_state(roo,unroo,i);
    }
    return lanes_state;
}