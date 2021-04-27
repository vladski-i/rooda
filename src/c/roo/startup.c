#include <stdio.h>
#include <jack/types.h>
#include <jack/jack.h>

int 
create_client(jack_client_t **client,
                const char *client_name, 
                jack_options_t options, 
                const char *server_name)
{
    jack_status_t status;
    *client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		return 1;
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(*client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}
    return 0;
}