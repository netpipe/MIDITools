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


#include "math.h"
#include "stdio.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BALL_SIZE 5

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qsrand(223) ;

    // create the ball
    MyBall = new Ball() ;

    // create the timer
    timer = new QTimer(this);
    this->connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    // Scene
    MyScene = new QGraphicsScene(this) ;
    MyEllipse = MyScene->addEllipse ( ui->graphicsView->width()/2, ui->graphicsView->height()/2, 4, 4,Qt::SolidLine,MyScene->foregroundBrush()) ;
    ui->graphicsView->setScene(MyScene);
    ui->graphicsView->show();

    // Initialisation
    MidiChannel = 1 ;
    MidiVelocity = 64 ;
    TimerPeriod = 15 ;
    ui->NoPC->setChecked(true);
    MidiOpen() ;
}

MainWindow::~MainWindow()
{
    snd_seq_close(seq_handle) ;
    MyThread->quit();
    delete MyBall ;
    delete MyScene ;
    delete ui;
}

void MainWindow::UpdateEllipse(void)
{

    MyEllipse->setRect(MyBall->x*ui->graphicsView->width(), MyBall->y*ui->graphicsView->height(),BALL_SIZE,BALL_SIZE);
    ui->graphicsView->show();
}

void MainWindow::ResetBall(void)
{
    MyBall->x = (float)(qrand())/(float)(RAND_MAX) ;
    MyBall->y = (float)(qrand())/(float)(RAND_MAX) ;
    this->UpdateEllipse();
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->pushButton->text()=="START")
    {
        ResetBall() ;
        ui->pushButton->setText("STOP");
        timer->start(TimerPeriod);
        ui->spinBoxXMidiChannel->setDisabled(true);
        ui->spinBoxYMidiChannel->setDisabled(true);
        printf("Running...\n") ;
    }
    else
    {
        ui->pushButton->setText("START");
        timer->stop() ;
        ui->spinBoxXMidiChannel->setEnabled(true);
        ui->spinBoxYMidiChannel->setEnabled(true);
        printf("Not running...\n") ;
    }
}

void MainWindow::on_spinBox_X_valueChanged(int )
{

}

void MainWindow::on_spinBox_Y_valueChanged(int )
{

}
void MainWindow::on_Velocity_valueChanged(int value)
{
    float sX=1,sY=1 ;

    if (MyBall->xvel<0) sX = -1 ;
    if (MyBall->yvel<0) sY = -1 ;
    MyBall->xvel = sX * ui->Velocity->value()/100*MyBall->maxvel ;
    MyBall->yvel = sY * ui->Velocity->value()/100*MyBall->maxvel ;
    ui->lcdBallVelocity->display(ui->Velocity->value()) ;
}

void MainWindow::on_MidiVelocity_valueChanged(int value)
{
    ui->lcdMidiVelocity->display(ui->MidiVelocity->value());
}

void MainWindow::update(void)
{
    bool    sendpc ;
    int note ;

    sendpc = ui->NoPC->isChecked() ;
    switch (this->MyBall->Move())
    {
        case 1 :
            MidiAllNotesOff(ui->spinBoxYMidiChannel->value()) ;
            if (!sendpc) MidiSendPC(ui->spinBoxYMidiChannel->value(),ui->spinBox_Y->value());
            note = MyBall->y*127 ;
            MidiNote(ui->spinBoxYMidiChannel->value(),true,note,ui->MidiVelocity->value());
            break ;
        case 2 :
            MidiAllNotesOff(ui->spinBoxYMidiChannel->value()) ;
            if (!sendpc) MidiSendPC(ui->spinBoxYMidiChannel->value(),ui->spinBox_Y->value());
            note = MyBall->y*127 ;
            MidiNote(ui->spinBoxYMidiChannel->value(),true,note,ui->MidiVelocity->value());
            break ;
        case 3 :
            MidiAllNotesOff(ui->spinBoxXMidiChannel->value()) ;
            if (!sendpc) MidiSendPC(ui->spinBoxXMidiChannel->value(),ui->spinBox_X->value());
            note = MyBall->x*127 ;
            MidiNote(ui->spinBoxXMidiChannel->value(),true,note,ui->MidiVelocity->value());
            break ;
        case 4 :
            MidiAllNotesOff(ui->spinBoxXMidiChannel->value()) ;
            if (!sendpc) MidiSendPC(ui->spinBoxXMidiChannel->value(),ui->spinBox_X->value());
            note = MyBall->x*127 ;
            MidiNote(ui->spinBoxXMidiChannel->value(),true,note,ui->MidiVelocity->value());
            break ;
    } ;
    this->UpdateEllipse();
}
void MainWindow::on_NoPC_stateChanged(int )
{
    if (ui->NoPC->isChecked()==true)
    {
        ui->spinBox_X->setEnabled(false);
        ui->spinBox_Y->setEnabled(false);
    }
    else
    {
        ui->spinBox_X->setEnabled(true);
        ui->spinBox_Y->setEnabled(true);
    }
}
void MainWindow::on_ButtonResetBall_pressed()
{
    this->ResetBall();
}
void MainWindow::on_XANO_pressed()
{
    MidiAllNotesOff(ui->spinBoxXMidiChannel->value()) ;
}
void MainWindow::on_YANO_pressed()
{
    MidiAllNotesOff(ui->spinBoxYMidiChannel->value()) ;
}
void MainWindow::on_Jitter_sliderMoved(int position)
{
    MyBall->jitter = (float)(ui->Jitter->value())/100. ;
}
void MainWindow::showEvent ( QShowEvent * event )
{
        printf("Midiball : starting Midi In Queue...\n") ;
        MyThread->start();

}
//////////////////////
////// MIDI //////////
//////////////////////
void MainWindow::MidiOpen(void)
{
    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0)<0)
    {
            fprintf(stderr,"Error opening Alsa device !\n") ;
            exit(1) ;
    }


    snd_seq_set_client_name(this->seq_handle, "Midiball");
    this->out_port = snd_seq_create_simple_port(this->seq_handle, "Midiball:out",
                      SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
                      SND_SEQ_PORT_TYPE_APPLICATION);
    if (this->out_port<0) { fprintf(stderr,"Error creating sequencer output port !\n") ; exit(1) ; }

    // Create Midi In Thread
    MyThread = new MidiThread() ;
    MyThread->SetGUI(this->App,this);
    MyThread->MidiOpen();

}
void MainWindow::MidiPrepareEvent(void)
{
    // prepare event
    snd_seq_ev_clear(&ev) ;
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev) ;
    snd_seq_ev_set_source(&ev,out_port) ;
}

void MainWindow::MidiSendPC(int CH,int PC)
{
    // prepare event
    MidiPrepareEvent() ;

    // send event
    ev.data.control.channel = CH ;
    ev.type = SND_SEQ_EVENT_PGMCHANGE ;
    ev.data.control.value = PC ;
    snd_seq_event_output_direct(seq_handle,&ev) ;
}


void MainWindow::MidiNote(int CH,bool ON,int NUM,int VEL)
{
    // prepare event
    MidiPrepareEvent() ;

    // send event
    ev.data.control.channel = CH ;
    if (ON) ev.type = SND_SEQ_EVENT_NOTEON ;
    else ev.type = SND_SEQ_EVENT_NOTEOFF ;
    ev.data.note.note = NUM ;
    ev.data.note.velocity = VEL ;
    snd_seq_event_output_direct(seq_handle,&ev) ;
}
void MainWindow::MidiAllNotesOff(int CH)
{
    // prepare event
    MidiPrepareEvent() ;

    // send event
    ev.data.control.channel = CH ;
    ev.type = SND_SEQ_EVENT_CONTROLLER ;
    ev.data.control.param = MIDI_CTL_ALL_NOTES_OFF ;
    ev.data.control.value = 0 ;
    snd_seq_event_output_direct(seq_handle,&ev) ;
}
// MidiControl Events
void MainWindow::customEvent(QEvent *event)
{
    if (event->type()==QEvent::User)
    {
        MyThread->Mutex->lock();
        switch(MyThread->last_event)
        {
            /////////////// CONTROLLER EVENT /////////////////////////
            case 1 :
                    switch(MyThread->last_controller)
                    {
                        // Modulation wheel -> Jitter
                        case CTRL_JITTER : ui->Jitter->setValue(MyThread->last_data*100/127) ;
                            break ;

                        // Volume control -> Midi Velocity
                        case CTRL_MIDI_VEL : ui->MidiVelocity->setValue(MyThread->last_data*100/127);
                            break ;

                        // Control N°8 : Ball velocity control like PitchBend (but 0-127)
                        case CTRL_BALL_VEL : ui->Velocity->setValue(100*(float)(MyThread->last_data)/(float)(127));
                                break ;

                        default : break ;
                    }

                    break ;
            /////////////// NOTE ON EVENT /////////////////////////
            case 3 : if (MyThread->last_velocity!=0)
                        switch(MyThread->last_note)
                    {
                        case NOTE_ON_START : ui->pushButton->click() ; break ;
                        case NOTE_ON_XANO : ui->XANO->click(); break ;
                        case NOTE_ON_YANO : ui->YANO->click(); break ;
                        case NOTE_ON_RESET : ui->ButtonResetBall->click(); break ;
                        default : break ;
                     }
                     break ;

            /////////////// PITCH BEND EVENT /////////////////////////
            case 2 : if (MyThread->last_data>0)
                        {
                            ui->Velocity->setValue(100*(float)(MyThread->last_data)/(float)(8192));
                        }
                    break ;
            default : break ;
        } // end of switch
        MyThread->Mutex->unlock();

    } // end of if
}

