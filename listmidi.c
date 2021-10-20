#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>

int main(int argc, char **argv)
{
   register int  err;
   int           cardNum;

   // Start with first card
   cardNum = -1;

   for (;;)
   {
      snd_ctl_t *cardHandle;

      // Get next sound card's card number. When "cardNum" == -1, then ALSA
      // fetches the first card
      if ((err = snd_card_next(&cardNum)) < 0)
      {
         printf("Can't get the next card number: %s\n", snd_strerror(err));
         break;
      }

      // No more cards? ALSA sets "cardNum" to -1 if so
      if (cardNum < 0) break;

      // Open this card's control interface. We specify only the card number -- not
      // any device nor sub-device too
      {
      char   str[64];

      sprintf(str, "hw:%i", cardNum);
      if ((err = snd_ctl_open(&cardHandle, str, 0)) < 0)
      {
         printf("Can't open card %i: %s\n", cardNum, snd_strerror(err));
         continue;
      }
      }

      {
      int      devNum;

      // Start with the first MIDI device on this card
      devNum = -1;
		
      for (;;)
      {
         snd_rawmidi_info_t  *rawMidiInfo;
         register int        subDevCount, i;

         // Get the number of the next MIDI device on this card
         if ((err = snd_ctl_rawmidi_next_device(cardHandle, &devNum)) < 0)
         {
            printf("Can't get next MIDI device number: %s\n", snd_strerror(err));
            break;
         }

         // No more MIDI devices on this card? ALSA sets "devNum" to -1 if so.
         // NOTE: It's possible that this sound card may have no MIDI devices on it
         // at all, for example if it's only a digital audio card
         if (devNum < 0) break;

         // To get some info about the subdevices of this MIDI device (on the card), we need a
         // snd_rawmidi_info_t, so let's allocate one on the stack
         snd_rawmidi_info_alloca(&rawMidiInfo);
         memset(rawMidiInfo, 0, snd_rawmidi_info_sizeof());

         // Tell ALSA which device (number) we want info about
         snd_rawmidi_info_set_device(rawMidiInfo, devNum);

         // Get info on the MIDI outs of this device
         snd_rawmidi_info_set_stream(rawMidiInfo, SND_RAWMIDI_STREAM_OUTPUT);

         i = -1;
         subDevCount = 1;

         // More subdevices?
         while (++i < subDevCount)
         {
            // Tell ALSA to fill in our snd_rawmidi_info_t with info on this subdevice
            snd_rawmidi_info_set_subdevice(rawMidiInfo, i);
            if ((err = snd_ctl_rawmidi_info(cardHandle, rawMidiInfo)) < 0)
            {
               printf("Can't get info for MIDI output subdevice hw:%i,%i,%i: %s\n", cardNum, devNum, i, snd_strerror(err));
               continue;
            }

            // Print out how many subdevices (once only)
            if (!i)
            {
               subDevCount = snd_rawmidi_info_get_subdevices_count(rawMidiInfo);
               printf("\nFound %i MIDI output subdevices on card %i\n", subDevCount, cardNum);
            }

            // NOTE: If there's only one subdevice, then the subdevice number is immaterial,
            // and can be omitted when you specify the hardware name
            printf((subDevCount > 1 ? "    hw:%i,%i,%i\n" : "    hw:%i,%i\n"), cardNum, devNum, i);
         }
      }

      }

      // Close the card's control interface after we're done with it
      snd_ctl_close(cardHandle);
   }
snd_rawmidi_t *midiOutHandle;

// Open output MIDI device hw:0,0,0
if ((err = snd_rawmidi_open(0, &midiOutHandle, "hw:1,0", 0)) < 0)
{
   printf("Can't open MIDI output: %s\n", snd_strerror(err));
}
unsigned char buffer[3];

// Send a middle C note on the first MIDI channel, velocity of 100
int yn;
yn=0;
if (yn) {
buffer[0] = 176;
buffer[1] = 00;
buffer[2] = 127;
}else
{
//buffer[0] = 0xb0; // set duty cycle
//buffer[1] = 0x1e;
//buffer[2] = 0x14;
//buffer[0] = 0xb0;
//buffer[1] = 0x1e;
//buffer[2] = 0x02;
buffer[0] = 0xb0;
buffer[1] = 0x00;
buffer[2] = 0x00;
}
snd_rawmidi_write(midiOutHandle, buffer, 3);
   snd_config_update_free_global();
}
