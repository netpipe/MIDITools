#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <alsa/asoundlib.h>     /* Interface to the ALSA system */
#include <unistd.h>             /* for sleep() function */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <QDebug>

#define DEVICE "/dev/midi"
int status;
int mode = SND_RAWMIDI_SYNC;
    snd_rawmidi_t* midiout = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      const char* portname = "hw:2,0,0";

    if ((status = snd_rawmidi_open(NULL, &midiout, portname, mode)) < 0) {
        qDebug() << "Problem opening MIDI output:" << status;
       exit(1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
        snd_rawmidi_close(midiout);
}


void MainWindow::on_pushButton_clicked()
{


    char noteon[3]  = {static_cast<char>(0x90), 60, 100};
    char noteoff[3] = {static_cast<char>(0x90), 60, 0};

    if ((status = snd_rawmidi_write(midiout, noteon, 3)) < 0) {
      qDebug() << "Problem writing to MIDI output:" << status;
       exit(1);
    }

    sleep(1);  // pause the program for one second to allow note to sound.

    if ((status = snd_rawmidi_write(midiout, noteoff, 3)) < 0) {
       qDebug() << "Problem writing to MIDI output:" << status;
       exit(1);
    }

    // midiout = NULL;    // snd_rawmidi_close() does not clear invalid pointer,
}

void MainWindow::on_pushButton_2_clicked()
{
    int fd;

    unsigned char note_on[] = { 0xc0, 0,	/* Program change */
      0x90, 60, 60
    };				/* Note on */
    unsigned char note_off[] = { 0x80, 60, 60 };	/* Note off */

    if ((fd = open (DEVICE, O_WRONLY, 0)) == -1)
      {
        perror ("open " DEVICE);
        exit (-1);
      }

    if (write (fd, note_on, sizeof (note_on)) == -1)
      {
        perror ("write " DEVICE);
        exit (-1);
      }

    sleep (1);			/* Delay one second */

    if (write (fd, note_off, sizeof (note_off)) == -1)
      {
        perror ("write " DEVICE);
        exit (-1);
      }

   // close (fd);
}

#include "src/alsarawportlist.c"
extern void print_midi_ports(void);
void MainWindow::on_pushButton_3_clicked()
{
    print_midi_ports();
}
