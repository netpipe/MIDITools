//////////////////////////
// MIDIBALL PROJECT     //
// Author : S. Hanneton //
// 2011                 //
//////////////////////////
/*
Midiball : a program that animates a bouncing ball and triggers midi events

   Copyright (C) 2011  Sylvain Hanneton

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "midi_in.h"


MidiThread::~MidiThread()
{
    delete MidiEvent ;
    delete Mutex ;
}

// Open Midi In port
void MidiThread::MidiOpen(void)
{
    // Sequencer initialization
    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0)<0)
    {
            fprintf(stderr,"Error opening Alsa device !\n") ;
            exit(1) ;
    }

    snd_seq_set_client_name(this->seq_handle, "Midiball-Control");
    this->in_port = snd_seq_create_simple_port(this->seq_handle, "Midiball:in",
                      SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
                      SND_SEQ_PORT_TYPE_APPLICATION);
    if (this->in_port<0) { fprintf(stderr,"Error creating sequencer input port !\n") ; exit(1) ; }

    // Midi Event
    MidiEvent = new QEvent(QEvent::User);

    // Mutex
    Mutex = new QMutex(QMutex::NonRecursive) ;
}
void    MidiThread::MidiProcess(void)
{
    snd_seq_event_t *ev;

    do {
    snd_seq_event_input(seq_handle, &ev);
    switch (ev->type) {
      case SND_SEQ_EVENT_CONTROLLER:
        //fprintf(stderr, "Control event on Channel %2d: %5d       \r",ev->data.control.channel, ev->data.control.value);
        Mutex->lock();
        last_event = MIDI_IN_EV_CONTROL ;
        last_channel = ev->data.control.channel ;
        last_controller = ev->data.control.param ;
        last_data = ev->data.control.value ;
        Mutex->unlock();
        App->sendEvent(GUI, MidiEvent);

        break;
      case SND_SEQ_EVENT_PITCHBEND:
        //fprintf(stderr, "Pitchbender event on Channel %2d: %5d   \r",ev->data.control.channel, ev->data.control.value);
        Mutex->lock();
        last_event = MIDI_IN_EV_PITCHBEND ;
        last_channel = ev->data.control.channel ;
        last_data = ev->data.control.value ;
        Mutex->unlock();
        App->sendEvent(GUI, MidiEvent);

        break;
      case SND_SEQ_EVENT_NOTEON:
        //fprintf(stderr, "Note On event on Channel %2d: %5d       \r",ev->data.control.channel, ev->data.note.note);
        Mutex->lock();
        last_event = MIDI_IN_EV_NOTE_ON ;
        last_channel = ev->data.control.channel ;
        last_note = ev->data.note.note ;
        last_velocity = ev->data.note.velocity ;
        Mutex->unlock();
        App->sendEvent(GUI, MidiEvent);

        break;
      case SND_SEQ_EVENT_NOTEOFF:
        //fprintf(stderr, "Note Off event on Channel %2d: %5d      \r",ev->data.control.channel, ev->data.note.note);
        Mutex->lock();
        last_event = MIDI_IN_EV_NOTE_OFF ;
        last_channel = ev->data.control.channel ;
        last_note = ev->data.note.note ;
        Mutex->unlock();
        App->sendEvent(GUI, MidiEvent);

        break;
    }


  } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}
void MidiThread::SetGUI(QApplication *app,QMainWindow *gui) {App = app ; GUI=gui;}

//////////////////////////
////// MAIN LOOP /////////
//////////////////////////
void MidiThread::run()
{
    int npfd;
    struct pollfd *pfd;

    printf("Starting Midi in process...\n") ;
    npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
    pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
    snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
    while (1) {
    if (poll(pfd, npfd, 100000) > 0)
        {
          MidiProcess();
        }
    } // end of while
}

