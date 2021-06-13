#include <jack/types.h>
#include <jack/jack.h>
#include <math.h>
#include "types.h"
#include "log.h"
/*UNROO CALLBACK FUNCTION + HELPERS */
extern jack_port_t *unroo_out[CHANNELS], *unroo_in1[CHANNELS], *unroo_in2[CHANNELS];
static void 
unroo_zero_fill(jack_default_audio_sample_t **in1,
				jack_default_audio_sample_t **in2,
				jack_default_audio_sample_t **out,
				jack_nframes_t nframes,
				config_t *conf){
	float balance1 = fmin(( 1.0 * conf->balance / 100.0),1.0);
	float balance2 = fmin(( 1.0 - 1.0 * conf->balance / 100.0),1.0);
	float master_vol = (1.0 * conf->master_volume / 100.0);
	// printf("%f %f %f\n",balance1,balance2, master_vol);
	CHANNELED{
		for (unsigned int i = 0; i < nframes; i ++){
			out[channel][i] = (in1[channel][i] * balance1 + in2[channel][i] * balance2) * master_vol;
		}
	}

}

static void 
unroo_no_fill(jack_default_audio_sample_t **in1,
				jack_default_audio_sample_t **in2,
				jack_default_audio_sample_t **out,
				jack_nframes_t nframes,
				config_t *conf){
	static int roo = 0;
	int init_roo = roo;
	CHANNELED{
		for (unsigned int i = 0; i < nframes; i ++){
			if(roo % 2){
				out[channel][i] = in1[channel][(i + init_roo)/ 2] * (float)( 1.0 * conf->balance / 50) * (conf->master_volume / 100);
			}
			else{
				out[channel][i] = in2[channel][(i + init_roo)/ 2] * ( 1.0 - 1.0 * conf->balance / 50) * (conf->master_volume / 100);
			}
			roo++;
			roo = roo % 2;
		}
	}

}
int
unroo_callback (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *out[CHANNELS], *in1[CHANNELS], *in2[CHANNELS];
	config_t *conf = (config_t*)arg;
	CHANNELED{
		out[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_out[channel], nframes);
		in1[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_in1[channel], nframes);
		in2[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_in2[channel], nframes);
	}
	switch(conf->mode){
		case SINE:
		case ZERO_FILL:
			unroo_zero_fill(in1, in2, out, nframes, conf);
			break;
		case NO_FILL:
			unroo_no_fill(in1, in2, out, nframes, conf);
			break;
		default:
			break;
	}
	return 0;      
}


/*ROO CALLBACK FUNCTION + HELPERS*/
extern jack_port_t *roo_out1[CHANNELS], *roo_out2[CHANNELS], *roo_in[CHANNELS];
static void 
roo_zero_fill(jack_default_audio_sample_t **in,
				jack_default_audio_sample_t **out1,
				jack_default_audio_sample_t **out2,
				jack_nframes_t nframes,
				config_t *conf){
	log_trace("win_size is %d and _roo is %d and mode is %d\n",conf->window_size,conf->_roo, conf->mode);
	CHANNELED{
		for (unsigned int i = 0; i < nframes; i ++){
			if(conf->_roo[channel] < conf->window_size / 2){
				out1[channel][i] = in[channel][i];
				out2[channel][i] = 0;
			}
			else{
				out2[channel][i] = in[channel][i];
				out1[channel][i] = 0;
			}
			conf->_roo[channel]++;
			conf->_roo[channel] = conf->_roo[channel] % conf->window_size;
		}
	}
}

static void 
roo_no_fill(jack_default_audio_sample_t **in,
				jack_default_audio_sample_t **out1,
				jack_default_audio_sample_t **out2,
				jack_nframes_t nframes){
	static int roo = 0;
	int init_roo = roo;
	for (unsigned int i = 0; i < nframes; i ++){
		if(roo % 2){
			out1[(i + init_roo)/ 2] = in[i];
		}
		else{
			out2[(i + init_roo)/ 2] = in[i];
		}
		roo++;
		roo = roo % 2;
	}

}

static void 
roo_sine(jack_default_audio_sample_t **in,
				jack_default_audio_sample_t **out1,
				jack_default_audio_sample_t **out2,
				jack_nframes_t nframes,
				config_t *conf){
	log_trace("win_size is %d and _roo is %d and mode is %d\n",conf->window_size,conf->_roo, conf->mode);
	CHANNELED{
		for (unsigned int i = 0; i < nframes; i ++){
			out1[channel][i] = in[channel][i] * conf->window[conf->_roo[channel]];
			out2[channel][i] = in[channel][i] * (1 - conf->window[conf->_roo[channel]]);
			conf->_roo[channel]++;
			conf->_roo[channel] = conf->_roo[channel] % conf->window_size;
		}
	}
}

int
roo_callback (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *out1[CHANNELS], *out2[CHANNELS], *in[CHANNELS];
	config_t *conf = (config_t*)arg;
	CHANNELED{
		out1[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_out1[channel], nframes);
		out2[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_out2[channel], nframes);
		in[channel] = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_in[channel], nframes);
	}
	switch (conf->mode)
	{
		case SINE:
			roo_sine(in,out1,out2,nframes,conf);
			break;
		case ZERO_FILL:
			roo_zero_fill(in, out1, out2, nframes, conf);
			break;
		case NO_FILL:
			roo_no_fill(in, out1, out2, nframes);
			break;
		default:
			break;
	}
	return 0;      
}