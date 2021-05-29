#ifndef LANE_H
# define LANE_H

#include <jack/types.h>
#include "types.h"
#include "stdbool.h"

void init_lanes_state(jack_client_t *roo, jack_client_t *unroo, uint lanes_no);
bool add_plugin_to_lane(instantiate_request_t *request, roo_plugin_t *plugin);
bool check_for_replace(instantiate_request_t *request);
bool remove_plugin_from_lane(instantiate_request_t *request);

#endif