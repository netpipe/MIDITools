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


int playNote(QString note);
bool done;
static void finish( int /*ignore*/ ){ done = true; }

std::vector< unsigned char > *message2;

void mycallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
    std::cout << (int)message->at(0) << ", "<< (int)message->at(1) << ", " << (int)message->at(2) << std::endl;

//  message2 = message;
//  qDebug()<< message;

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

//playNote("80");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete midiin;
    delete midiout;
}


void MainWindow::on_pushButton_clicked()
{
playNote("80");
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

void MainWindow::on_getlastmessage_clicked()
{
    sendMessage2();
}

#include "fluidlite.h"
#include <alsa/asoundlib.h>


  fluid_synth_t* synth;
  //float* flbuffer;
  int audio_channels;

int playNote(QString note) {
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;


#define SAMPLE_RATE 44100
#define SAMPLE_SIZE sizeof(float)
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 2
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)

//flstuff
  audio_channels = 2;
  fluid_settings_t* settings = new_fluid_settings();
  synth = new_fluid_synth(settings);
 // fluid_synth_sfload(synth, "../sf_/Boomwhacker.sf2", 1);
  fluid_synth_sfload(synth, "./fluidlite/soundfont.sf2", 1);
 // flbuffer = (float *)calloc( SAMPLE_SIZE, NUM_SAMPLES );
   float* flbuffer = (float *)calloc(SAMPLE_SIZE, NUM_SAMPLES);

  fluid_synth_noteon(synth, 0, 80, 127);
 // printf("triggered note\n");
//  fluid_synth_noteon(synth, 0, note, 127);
  printf("triggered note\n");
 // fluid_synth_noteon(synth, 0, 60, 127);
 // printf("triggered note\n");

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
  //  exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                               SND_PCM_FORMAT_S32_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = SAMPLE_RATE;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
  //  exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
  size = SAMPLE_SIZE * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);
  /* 5 seconds in microseconds divided by
   * period time */
  loops = 1000 / val;


// time to loop the buffer 1 second
  while (loops > 0) {
    loops--;

    fluid_synth_write_float(synth, NUM_SAMPLES, flbuffer, 0, audio_channels, flbuffer, 0, audio_channels );
    rc = snd_pcm_writei(handle, flbuffer, NUM_FRAMES);

  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  return 0;
}
