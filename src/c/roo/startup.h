#include <jack/types.h>

/*
Creates a jack_client_t at specified address with given options and name
Returns 1 on failure
*/
int 
create_client(jack_client_t **client,
                const char *client_name, 
                jack_options_t options, 
                const char *server_name);