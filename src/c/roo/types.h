#include <stdint.h>
#include <jack/types.h>

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
    uint32_t _roo;
    float *window;
}config_t;

typedef struct instantiate_request{
    char *plugin_name;
    uint32_t lane;
}instantiate_request_t;

typedef struct roo_plugin {
    char *name;
    const jack_port_t **in_ports;
    const jack_port_t **out_ports;

}roo_plugin_t;

typedef struct roo_lane_state_iter {
    roo_plugin_t *plugin;
    struct roo_lane_state_iter *next;
}roo_lane_state_iter_t;

typedef struct roo_lane_state{
    roo_lane_state_iter_t *iter;
    uint32_t plugins_no;
}roo_lane_state_t;

typedef struct roo_lanes_state {
    roo_lane_state_t *lanes;
    uint32_t lanes_no;
}roo_lanes_state_t;