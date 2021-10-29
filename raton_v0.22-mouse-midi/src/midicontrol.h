
/* protos */

void midicontrol_init();
void set_controller(int cntrl);
void set_channel(int chan);
void send_control(int val_i);
void send_control_chan(int chan, int val_i);
void send_control_all(int channel_in, int controller_in, int val_i);
