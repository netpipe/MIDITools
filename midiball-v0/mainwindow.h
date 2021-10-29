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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ball.h"
#include "midi_in.h"
#include "/usr/include/alsa/asoundlib.h"
#include <QtGlobal>
#include "QTimer"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>
#include <QShowEvent>
//#include <QCustomEvent>


// MIDI EVENT NUMBER FOR BALL CONTROL
// You can change the control numbers before
// compilation if you want
#define NOTE_ON_START   36  // to start and stop the ball
#define NOTE_ON_XANO    37  // Horizontal walls All Note Off
#define NOTE_ON_YANO    38  // Vertical walls All Note Off
#define NOTE_ON_RESET   39  // to reset the ball
#define CTRL_JITTER     1   // Jitter control : default = modulation wheel
#define CTRL_BALL_VEL   3   // Ball velocity control = n°8 (0-127)
#define CTRL_MIDI_VEL   7   // Midi velocity control = volume controller
// NB : Ball velocity is also controlled via the PITCH BEND controller

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Public properties
    Ball *MyBall;
    QTimer *timer ;
    QGraphicsScene *MyScene ;
    QGraphicsEllipseItem *MyEllipse ;
    QApplication *App ;
    int MidiChannel ;
    int MidiVelocity ;
    int TimerPeriod;

    // ALSA midi properties
    snd_seq_t *seq_handle;
    int out_port ;
    snd_seq_addr_t  seq_addr;
    snd_seq_event_t ev ;
    MidiThread  *MyThread ;

    // public methods
    void    UpdateEllipse(void) ;
    void    ResetBall(void) ;
    void    MidiOpen(void) ;
    void    MidiPrepareEvent(void) ;
    void    MidiSendPC(int CH,int PC) ;
    void    MidiNote(int CH,bool ON,int NUM,int VEL) ;
    void    MidiAllNotesOff(int CH) ;
    void    showEvent ( QShowEvent * event ) ;
    void    customEvent(QEvent *event) ;

public slots :
     // function called by the timer...
     void   update(void) ;

private:
    Ui::MainWindow *ui;

private slots:
    void on_Velocity_valueChanged(int value);
    void on_MidiVelocity_valueChanged(int value);
    void on_Jitter_sliderMoved(int position);
    void on_YANO_pressed();
    void on_XANO_pressed();
    void on_ButtonResetBall_pressed();
    void on_NoPC_stateChanged(int );
    void on_spinBox_Y_valueChanged(int );
    void on_spinBox_X_valueChanged(int );
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
