//Simple Velocity Converter.
//ncurses as UI.
//
//Feature and Usage: Creates 1 input and 1 output port.
//Connect midi-out of a device to the input port of Velocity Converter.
//Connect output port of Velocity Converter to midi-in of a device.
//Then, note-on events from a device will be amplified (modified)
//and sent to proper device.
//For example,
//            +---------------------+
//            |                     |
// Keyboard =>| IN => Modify => OUT | => Sound module
//            |                     |
//            | Velocity Converter  |
//            +---------------------+
//Formular used to Modify the velocity is : 127*(IN/max)^gamma
//where IN is velocity value inputted (i.e. from Keyboard) and
//max and gamma are variables user can modify.
//
//Keyboard Input:
//q - quits program and saves current max and gamma to velconvertrc
//UP DOWN arrow - changes max
//LEFT RIGHT arrow - changes gamma
//
//Bug: Works only when midi note-on events are sent to the program.
//If no event is sent, it hangs..no keyboard input updates the screen.
//So, always quit this program first before disconnecting midi device.
//If you quit this program, which saves max and gamma to a file
//named velconvertrc, IN and OUT port disappears.
//So, you don't have to disconnect them. However previously connected
//applications to these ports will loose the connection.
//
//THIS BUG IS FIXED NOW.
//
// $ g++ velconvert_ncurses.cpp -lncurses -lasound 
// $ ./a.out
// $ aconnect -o
// client 20: 'E-MU XMidi1X1' [type=kernel]
//    0 'E-MU XMidi1X1 MIDI 1'
// $ aconnect -i
// client 128: 'MIDI Velocity Converter' [type=user]
//    1 'MIDI Velocity Converter OUT 0'
// $ aconnect 20 128
//

#include <iostream>
#include <alsa/asoundlib.h>
#include <cmath>
#include <ncurses.h>
#include <fstream>
#include <string>

int init_seq( snd_seq_t **seq_handle, int& in, int& out );
void velocity_convert( snd_seq_t *seq_handle, double max, double gamma,
	int in, int out, WINDOW *dump_display );

int init_seq( snd_seq_t **seq_handle, int& in, int& out ) {
	if( snd_seq_open(seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0 ) {
		std::cerr<<"Error : snd_seq_open : sequencer open failed"<<std::endl;
		return -1;
	}
    snd_seq_set_client_name( *seq_handle, "MIDI Velocity Converter" );
	in = snd_seq_create_simple_port(
		*seq_handle,
		"MIDI Velocity Converter IN 0",
		SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION );
	if( in < 0 ) {
		std::cerr<<"Error : snd_seq_create_simple_port : failed"<<std::endl;
		return -1;
	}
	out = snd_seq_create_simple_port(
		*seq_handle,
		"MIDI Velocity Converter OUT 0",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION );
	if( out < 0 ) {
		std::cerr<<"Error : snd_seq_create_simple_port : failed"<<std::endl;
		return -1;
	}
	return 0;
}

void velocity_convert( snd_seq_t *seq_handle, double max, double gamma,
	int in, int out, WINDOW *dump_display ) {
	snd_seq_event_t *ev;


	do {
		snd_seq_event_input( seq_handle, &ev );

		if( (ev->type == SND_SEQ_EVENT_NOTEON) /*||
			(ev->type == SND_SEQ_EVENT_NOTEOFF)*/ ) {
			int old_velocity = ev->data.note.velocity;
			int new_velocity =
				static_cast<int>(
					127 * std::pow(old_velocity/max, gamma) );
				//static_cast<int>( (ev->data.note.velocity)*ratio );
			ev->data.note.velocity = (new_velocity >= 127) ?
				127 : new_velocity;
			wprintw(dump_display, "%d => %d\n",
				old_velocity, ev->data.note.velocity);
			wrefresh(dump_display);
		}

		snd_seq_ev_set_source( ev, out );
        snd_seq_ev_set_subs( ev );
        snd_seq_ev_set_direct( ev );
		snd_seq_event_output_direct( seq_handle, ev );
	} while( snd_seq_event_input_pending(seq_handle, 0) > 0 );
}

int main( int argc, char *argv[] ) {

	//file read to set max and gamma
	double max, gamma;
	std::string fn = "velconvertrc";
	std::ifstream fin;
	fin.open(fn.c_str());
	if(!fin) {
		std::cout<<"Error opening file "<<fn<<" to read."<<std::endl;
		max = 90;
		gamma = 0.3;
	}
	else {
		fin>>max>>gamma;
		fin.close();
	}

	//alsa sequencer initialization
    snd_seq_t * seq_handle;
	int in, out;
    if ( init_seq(&seq_handle, in, out) < 0 ) {
		std::cerr<<"Error : init_seq : failed"<<std::endl;
		exit( 1 );
	}
    int npfd = snd_seq_poll_descriptors_count( seq_handle, POLLIN );
    pollfd *pfd = ( pollfd * ) alloca( npfd * sizeof( struct pollfd ) );
    snd_seq_poll_descriptors( seq_handle, pfd, npfd, POLLIN );


	//ncurses initialization
	initscr();
	clear();
	cbreak();
	noecho();
	keypad( stdscr, TRUE );
	nodelay( stdscr, TRUE );

	int x, y, max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);

	int input_display_height = 7;
	WINDOW *input_display = newwin(input_display_height, max_x, 0, 0);
	WINDOW *dump_display =
		newwin(max_y - input_display_height, max_x, input_display_height, 0 );
	refresh();

	box(input_display, 0, 0);
	wrefresh(input_display);

	int input;
	getmaxyx(dump_display, max_y, max_x);
	while( (input = getch()) != 'q' ) {
		switch( input ) {
			case KEY_UP:
				max = ((max + 1) > 127)? 127 : (max + 1);
				break;
			case KEY_DOWN:
				max = ((max - 1) < 0)? 0 : (max - 1);
				break;
			case KEY_RIGHT:
				gamma = ((gamma + 0.01) > 2.0)? 2.0 : (gamma + 0.01);
				break;
			case KEY_LEFT:
				gamma = ((gamma - 0.01) < 0.0)? 0.0 : (gamma - 0.01);
				break;
		}

		mvwprintw(input_display, 0, 0,
			"VELOCITY CONVERTER\n"
			"Please connect and send midi note-on signal first.\n"
			"q: quit // LEFT, RIGHT: changes gamma // UP, DOWN changes max\n");
		wprintw(input_display, "              max  gamma\n");
		wprintw(input_display, "127*(velocity/%3.0f)^%1.2f", max, gamma);
		wrefresh(input_display);

		getyx(dump_display, y, x);
		if( y == max_y - 1 ) {
			wclear(dump_display);
		}
		if ( poll( pfd, npfd, 0 ) > 0 ) {
			velocity_convert( seq_handle, max, gamma, in, out,
				dump_display );
		}
	}
	endwin();

	//save max and gamma value to file for the next time.
	std::ofstream fout;
	fout.open(fn.c_str());
	if(!fout) {
		std::cout<<"Error opening file "<<fn<<" to save"<<std::endl;
		exit(1);
	}
	fout<<max<<std::endl
		<<gamma<<std::endl;
	fout.close();

	return 0;
}
