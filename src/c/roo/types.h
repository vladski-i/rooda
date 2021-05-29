#ifndef TYPES_H
# define TYPES_H

#include <stdint.h>
#include <jack/types.h>
#include "stereo.h"

/* Enum of possible splitting / zipping modes*/
typedef enum mode{
    ZERO_FILL,
    NO_FILL,
    SINE
}roo_mode_t;

/* Struct for passing arguments from the main thread to the jack callbacks*/
typedef struct config {
    uint32_t window_size;
    roo_mode_t mode;
    uint32_t _roo[CHANNELS];
    float *window;
}config_t;

/* JS object model coming from the frontend*/
typedef struct instantiate_request{
    char *plugin_name;
    uint32_t lane;
    uint32_t index;
}instantiate_request_t;

/* Relevant info regarding internal state of a plugin*/
typedef struct roo_plugin {
    char *name;
    const jack_port_t **in_ports;
    const jack_port_t **out_ports;    
    const char **in_port_names;
    const char **out_port_names;
    uint32_t id;
    uint32_t no_ports;

}roo_plugin_t;

/* Linked list iterator*/
typedef struct roo_lane_state_iter {
    roo_plugin_t *plugin;
    struct roo_lane_state_iter *next;
}roo_lane_state_iter_t;

/* State of a lane*/
typedef struct roo_lane_state{
    roo_lane_state_iter_t *iter;
    uint32_t plugins_no;
}roo_lane_state_t;

/* State of all lanes*/
typedef struct roo_lanes_state {
    roo_lane_state_t *lanes;
    uint32_t lanes_no;
}roo_lanes_state_t;

#endif