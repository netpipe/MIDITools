#ifndef MIDI_IN_H
#define MIDI_IN_H
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


#include <QtGlobal>
#include <QMainWindow>
#include <QApplication>
#include <QEvent>
#include <QThread>
#include <QMutex>
#include "/usr/include/alsa/asoundlib.h"

#define MIDI_IN_EV_CONTROL 1
#define MIDI_IN_EV_PITCHBEND 2
#define MIDI_IN_EV_NOTE_ON 3
#define MIDI_IN_EV_NOTE_OFF 1

class MidiThread : public QThread
{
public :
        snd_seq_t *seq_handle;
        int in_port ;
        QMainWindow *GUI ;
        QApplication *App ;
        QEvent *MidiEvent;
        QMutex *Mutex ;
        int last_event ;
        int last_channel ;
        int last_controller ;
        int last_data ;
        int last_note ;
        int last_velocity ;

public:
    ~MidiThread() ;
    void MidiOpen(void) ;
    void MidiProcess(void);
    void run();
    void SetGUI(QApplication *app,QMainWindow *gui) ;
};



#endif // MIDI_IN_H
