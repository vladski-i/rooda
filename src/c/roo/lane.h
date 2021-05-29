#include <jack/types.h>
#include "types.h"

void init_lanes_state(jack_client_t *roo, jack_client_t *unroo, uint lanes_no);
uint add_plugin_to_lane(instantiate_request_t *request, roo_plugin_t *plugin);