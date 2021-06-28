#ifndef CONF_H
# define CONF_H

#define MONO
// #define STEREO

#ifdef STEREO
# define CHANNELS 2
#define ONCE if(channel == 0)

#else
# define CHANNELS 1
#define ONCE 
#endif


#define CHANNELED for(uint channel = 0; channel < CHANNELS; channel ++)
#endif