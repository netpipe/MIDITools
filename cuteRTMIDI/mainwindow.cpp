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

#include <iostream>
#include <cstdlib>
#include <signal.h>

// Platform-dependent sleep routines.
#if defined(WIN32)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif



bool done;
static void finish( int /*ignore*/ ){ done = true; }

std::vector< unsigned char > *message2;

void mycallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";

  message2 = message;
  qDebug()<< message;

}


void MainWindow::sendMessage2(){

    QString test2;
    unsigned int nBytes = message2->size();
    for ( unsigned int i=0; i<nBytes; i++ )
      test2 += QString((int)message2->at(i)) ;

  ui->messagesList->addItem(test2.toLatin1());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::map<int, std::string> apiMap;
 //   apiMap[RtMidi::MACOSX_CORE] = "OS-X CoreMIDI";
 //   apiMap[RtMidi::WINDOWS_MM] = "Windows MultiMedia";
 //   apiMap[RtMidi::UNIX_JACK] = "Jack Client";
    apiMap[RtMidi::LINUX_ALSA] = "Linux ALSA";
  //  apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

    std::vector< RtMidi::Api > apis;
    RtMidi :: getCompiledApi( apis );

    for ( unsigned int i=0; i<apis.size(); i++ )
      std::cout << "  " << apiMap[ apis[i] ] << std::endl;

    RtMidiIn  *midiin = 0;
    RtMidiOut *midiout = 0;

    try {

    midiin = new RtMidiIn();

    std::cout << "\nCurrent input API: " << apiMap[ midiin->getCurrentApi() ] << std::endl;

    // Check inputs.
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";

    for ( unsigned i=0; i<nPorts; i++ ) {
      std::string portName = midiin->getPortName(i);
      std::cout << "  Input Port #" << i << ": " << portName << '\n';
        ui->inoutcmb->addItem( portName.c_str() + QString("a    ")  + QString::number(i) );
    }

    // RtMidiOut constructor ... exception possible
    midiout = new RtMidiOut();

    std::cout << "\nCurrent output API: " << apiMap[ midiout->getCurrentApi() ] << std::endl;

    // Check outputs.
    nPorts = midiout->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";

    for ( unsigned i=0; i<nPorts; i++ ) {
      std::string portName = midiout->getPortName(i);
      std::cout << "  Output Port #" << i << ": " << portName << std::endl;
      ui->inoutcmb->addItem( portName.c_str() + QString("a    ") + QString::number(i).toLatin1() );
    }
    std::cout << std::endl;

  } catch ( RtMidiError &error ) {
    error.printMessage();
  }

midiin->closePort();
midiout->closePort();

midiout->openVirtualPort();

midiin->openVirtualPort();
midiin->setCallback( &mycallback );
midiin->ignoreTypes( false, false, false );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete midiin;
    delete midiout;
}


void MainWindow::on_pushButton_clicked()
{

}


void MainWindow::on_pushButton_3_clicked()
{
      std::vector<unsigned char> message;
    message[0] = 144;
    message[1] = 64;
    message[2] = 90;
    midiout->sendMessage( &message );
}

void MainWindow::on_listPortsBTN_clicked()
{

}

void MainWindow::on_virtInBTN_clicked()
{
      //  if ( chooseMidiPort( midiin ) == false ) goto cleanup;


//cleanup:

// delete midiin;
}
