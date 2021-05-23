#include <jack/types.h>
#include <jack/jack.h>
#include "types.h"
#include "log.h"

/*UNROO CALLBACK FUNCTION + HELPERS */
extern jack_port_t *unroo_out, *unroo_in1, *unroo_in2;
static void 
unroo_zero_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes){
	for (unsigned int i = 0; i < nframes; i ++){
		out[i] = in1[i] + in2[i];
	}

}

static void 
unroo_no_fill(jack_default_audio_sample_t *in1,
				jack_default_audio_sample_t *in2,
				jack_default_audio_sample_t * out,
				jack_nframes_t nframes){
	static int roo = 0;
	int init_roo = roo;
	for (unsigned int i = 0; i < nframes; i ++){
		if(roo % 2){
			out[i] = in1[(i + init_roo)/ 2];
		}
		else{
			out[i] = in2[(i + init_roo)/ 2];
		}
		roo++;
		roo = roo % 2;
	}

}
int
unroo_callback (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *out, *in1, *in2;
	config_t args = *(config_t*)arg;
	
	out = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_out, nframes);
	in1 = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_in1, nframes);
	in2 = (jack_default_audio_sample_t*)jack_port_get_buffer (unroo_in2, nframes);

	switch(args.mode){
		case SINE:
		case ZERO_FILL:
			unroo_zero_fill(in1, in2, out, nframes);
			break;
		case NO_FILL:
			unroo_no_fill(in1, in2, out, nframes);
			break;
		default:
			break;
	}
	return 0;      
}


/*ROO CALLBACK FUNCTION + HELPERS*/
extern jack_port_t *roo_out1, *roo_out2, *roo_in;
static void 
roo_zero_fill(jack_default_audio_sample_t *in,
				jack_default_audio_sample_t *out1,
				jack_default_audio_sample_t * out2,
				jack_nframes_t nframes,
				config_t *conf){
	log_trace("win_size is %d and _roo is %d and mode is %d\n",conf->window_size,conf->_roo, conf->mode);
	for (unsigned int i = 0; i < nframes; i ++){
		if(conf->_roo < conf->window_size / 2){
			out1[i] = in[i];
			out2[i] = 0;
		}
		else{
			out2[i] = in[i];
			out1[i] = 0;
		}
		conf->_roo++;
		conf->_roo = conf->_roo % conf->window_size;
	}
}

static void 
roo_no_fill(jack_default_audio_sample_t *in,
				jack_default_audio_sample_t *out1,
				jack_default_audio_sample_t * out2,
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
roo_sine(jack_default_audio_sample_t *in,
				jack_default_audio_sample_t *out1,
				jack_default_audio_sample_t * out2,
				jack_nframes_t nframes,
				config_t *conf){
	log_trace("win_size is %d and _roo is %d and mode is %d\n",conf->window_size,conf->_roo, conf->mode);
	for (unsigned int i = 0; i < nframes; i ++){
		out1[i] = in[i] * conf->window[conf->_roo];
		out2[i] = in[i] * (1 - conf->window[conf->_roo]);
		conf->_roo++;
		conf->_roo = conf->_roo % conf->window_size;
	}
}

int
roo_callback (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *out1, *out2, *in;
	config_t *conf = (config_t*)arg;
	
	out1 = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_out1, nframes);
	out2 = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_out2, nframes);
	in = (jack_default_audio_sample_t*)jack_port_get_buffer (roo_in, nframes);
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