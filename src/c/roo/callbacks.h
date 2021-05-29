#ifndef CALLBACKS_H
# define CALLBACKS_H

#include <jack/types.h>


/*UNROO CALLBACK FUNCTION*/
int
unroo_callback (jack_nframes_t nframes, void *arg);



/*ROO CALLBACK FUNCTION*/
int
roo_callback (jack_nframes_t nframes, void *arg);

#endif