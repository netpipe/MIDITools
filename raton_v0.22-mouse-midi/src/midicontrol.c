#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <alsa/asoundlib.h>
#define NAME_LENGTH 128

// alsa data
snd_seq_t *seq_handle;
snd_seq_event_t ev;

//midicontrol data
int channel;
int controller;

void midicontrol_init(char *name)
{
	char client_name[32];
	char port_name[48];
	snd_seq_addr_t src;

	/* init */
	channel = 1;
	controller = 1;
	
	/* ALSA stuff */	
	/* Create the sequencer port. */
	
	strncpy(client_name, name, 31);
	//sprintf(port_name , "%s Control", client_name);
	strncpy(port_name , "Control", 47);

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
		puts("Error: Failed to access the ALSA sequencer.");
		exit(-1);
	}

	snd_seq_set_client_name(seq_handle, client_name);
	src.client = snd_seq_client_id(seq_handle);
	src.port = snd_seq_create_simple_port(seq_handle, "Output",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION);

	/* Init the event structure */
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, src.port);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
}

void set_controller(int cntrl)
{
	controller = cntrl;
}

void set_channel(int chan)
{
	channel = chan;
}

void send_control(int val_i)
{
	snd_seq_ev_set_controller(&ev, channel, controller, val_i);
	snd_seq_event_output_direct(seq_handle, &ev);
}

void send_control_chan(int chan, int val_i)
{
	snd_seq_ev_set_controller(&ev, chan, controller, val_i);
	snd_seq_event_output_direct(seq_handle, &ev);
}

void send_control_all(int channel_in, int controller_in, int val_i)
{
	snd_seq_ev_set_controller(&ev, channel_in, controller_in, val_i);
	snd_seq_event_output_direct(seq_handle, &ev);
}

