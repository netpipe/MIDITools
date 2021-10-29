#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "midicontrol.h"

#define MYPI 3.14159
#define CONTROL_RES 128
#define RATON_VERSION "0.2"

/* flags to reuse openfile dialog */
#define OPEN_FILE 1
#define OPEN_CURVEX 2
#define OPEN_CURVEY 3
int openfunction = OPEN_FILE;

/* flags to reuse saveasfile dialog */
#define SAVE_FILE 1
#define SAVE_CURVEX 2
#define SAVE_CURVEY 3
int savefunction = SAVE_FILE;

/* pref struct */
struct ratprefs {
	char feedback;
	char colorfb;
	int x_stepnum;
	gfloat x_stepoffset;
	int x_randomnum;
	int y_stepnum;
	gfloat y_stepoffset;
	int y_randomnum;
};

struct ratprefs prefs, prefsbak;

int ginit = 0;
int width = 256;
int height = 256;
int pid;
char pidstr[32];
char titlestr[48];
char midititle[48];

int controllerx = 1;
int controllery = 1;
int channelx = 0;
int channely = 1;
int invertx = 0;
int inverty = 0;
int reversex = 0;
int reversey = 0;

int mouseactive = 1;
int freezex = 64;
int freezey = 64;

int lastx;
int lasty;
int curvetypeX = GTK_CURVE_TYPE_LINEAR;
int curvetypeY = GTK_CURVE_TYPE_LINEAR;

GtkWidget *aboutdialog;
GtkWidget *customXdialog;
GtkWidget *customYdialog;
GtkWidget *opendialog;
GtkWidget *savedialog;
GtkWidget *prefsdialog;
  
/* widget pointers, intialized early in run */
GtkWidget *drawptr = NULL;
GtkWidget *xcurve = NULL;
GtkWidget *ycurve = NULL;

/* color gc */
GdkGC	*gcx = NULL;
GdkGC	*gcy = NULL;
GdkColor cx = {0,0xaa00,0xaa00,0xaa00};
GdkColor cy = {0,0x0000,0xaa00,0x0000};

/* console color str */
char consolex[32];
char consoley[32]; 

/* tranformations */
gfloat outX[128];
gfloat outY[128];

/* store the orig float vals (pre invert & reverse) */
gfloat origX[128];
gfloat origY[128];

/* curve input (file) buffers */
gfloat cX[128];
gfloat cY[128];

/* range check/brute-force repair */
void fixrange(gfloat *trans)
{
int loop; 
gfloat item;
	for (loop = 0; loop < 128; loop++)
	{
		item = trans[loop];
		
		if ( item > 127.0)
			trans[loop] = 127.0;
  		if ( item < 0.0 )
			trans[loop] = 0.0;
	}
}

void transCopy(gfloat *to, gfloat *from)
{
int loop;
	for (loop = 0; loop < 128; loop++)
		to[loop] = from[loop];
}

void transReverseCopy(gfloat *to, gfloat *from)
{
int loop;
	for (loop = 0; loop < 128; loop++)
		to[loop] = from[(int)abs(loop - 127)];
}

void transInvertCopy(gfloat *to, gfloat *from)
{
int loop;
	for (loop = 0; loop < 128; loop++)
		to[loop] = abs( from[loop] - 127);
}

void transReverse(gfloat *trans)
{
int loop;
gfloat tmp;
	for (loop = 0; loop < 64; loop++)
	{
		tmp = trans[loop];	
		trans[loop] = trans[127 - loop];
		trans[127 - loop] = tmp;
	}
}

void transInvert(gfloat *trans)
{
int loop;
	for (loop = 0; loop < 128; loop++)
		trans[loop] = abs(trans[loop] - 127);
}

void transPrint(gchar *prefix, gfloat *trans)
{
int loop;
	for (loop = 0; loop < 128; loop++)
		g_print("%s%d %f\n", prefix, loop, trans[loop]);
}

/* BEGIN unused transformations, to be used someday in new editor 

void transShift(gfloat *trans, gfloat amount)
{
int loop;
	for (loop = 0; loop < 128; loop++)
	{
		gfloat nval = trans[loop] + amount;
		if (nval < 0.0)
			nval = 0.0;
		if (nval > 127.0)
			nval = 127.0; 
		trans[loop] = nval;
	}
}

void transForm(gfloat *trans, gfloat amount)
{
int loop;
	for (loop = 0; loop < 128; loop++)
	{
		gfloat nval = trans[loop] * amount;
		if (nval < 0.0)
			nval = 0.0;
		if (nval > 127.0)
			nval = 127.0; 
		trans[loop] = nval;
	}
}

END unused transformations */

void transModifyX(gfloat *in)
{
  transCopy(outX, in);
  if (reversex)
	transReverse(outX);

  if (invertx)
  	transInvert(outX);
}

void transModifyY(gfloat *in)
{
  transCopy(outY, in);
  if (reversey)
	transReverse(outY);

  if (inverty)
  	transInvert(outY);
}

/* file IO */

void saverc()
{
char *homedir;
char pathnm[255];
FILE* file;
int loop;

	homedir = (char *)g_get_home_dir();
	strncpy (pathnm, homedir, 230);
	strcat (pathnm, "/.ratonrc");  

	if (file = fopen(pathnm, "w"))
	{
		fprintf(file, "raton %s\n", RATON_VERSION);
		fprintf(file, "#\n# raton rc file\n#\n");
		fprintf(file, "feedback %d %d\n", prefs.feedback, prefs.colorfb);
		fprintf(file, "stepnum %d %d\n", prefs.x_stepnum, prefs.y_stepnum);
		fprintf(file, "stepoffset %f %f\n", prefs.x_stepoffset, prefs.y_stepoffset);
		fprintf(file, "randomnum %d %d\n", prefs.x_randomnum, prefs.y_randomnum);
		fprintf(file, "\n# Edit entries below directly (no gui exists)\n\n");
		fprintf(file, "colorx %d %d %d\n", cx.red/255, cx.green/255, cx.blue/255);
		fprintf(file, "colory %d %d %d\n", cy.red/255, cy.green/255, cy.blue/255);
		fprintf(file, "consolecolor %s %s\n", consolex, consoley);

		fclose(file);
	}
	else
	{
	g_print("File Error: cannot create .ratonrc file\n");
	}
}

void getrc()
{
char *homedir;
char filename[255];
FILE* file;
char str[80];

int loop, count;
char *curtoken;
char *tokens[10];

	homedir = (char *)g_get_home_dir();
	strncpy (filename, homedir, 230);
	strcat (filename, "/.ratonrc"); 

	if (file = fopen(filename, "r"))
	{
		while ( fgets(str, 80, file) )
		{
			count = 0;
			curtoken = strtok(str, " \n\t");
			while (curtoken)
			{
				tokens[count] = curtoken;
				count++;
				curtoken = strtok(NULL, " \n\t");
			}
			if (tokens[0][0] == '#')
			{
				//g_print("a comment\n");
			}
			else if (! strcmp(tokens[0], "feedback"))
			{
				prefs.feedback = atoi(tokens[1]);
				prefs.colorfb = atoi(tokens[2]);
			}
			else if (! strcmp(tokens[0], "stepnum"))
			{
				prefs.x_stepnum = atoi(tokens[1]);
				prefs.y_stepnum = atoi(tokens[2]);
			}
			else if (! strcmp(tokens[0], "stepoffset"))
			{
				prefs.x_stepoffset = atof(tokens[1]);
				prefs.y_stepoffset = atof(tokens[2]);
			}
			else if (! strcmp(tokens[0], "randomnum"))
			{
				prefs.x_randomnum = atoi(tokens[1]);
				prefs.y_randomnum = atoi(tokens[2]);
			}
			else if (! strcmp(tokens[0], "colorx"))
			{
				cx.red = atoi(tokens[1]) * 255;
				cx.green = atoi(tokens[2]) * 255;
				cx.blue = atoi(tokens[3]) * 255;
			}
			else if (! strcmp(tokens[0], "colory"))
			{
				cy.red = atoi(tokens[1]) * 255;
				cy.green = atoi(tokens[2]) * 255;
				cy.blue = atoi(tokens[3]) * 255;
			}
			else if (! strcmp(tokens[0], "consolecolor"))
			{
				strncpy(consolex, tokens[1], 31);
				strncpy(consoley, tokens[2], 31);
			}
		}
	}
}

void saveFile(gchar *fname)
{
FILE* file;
int loop;

	if (file = fopen(fname, "w"))
	{
		fprintf(file, "raton %s\n", RATON_VERSION);
		fprintf(file, "control %d %d\n", controllerx, controllery);
		fprintf(file, "channel %d %d\n", channelx, channely);
		fprintf(file, "inv %d %d\n", invertx, inverty);
		fprintf(file, "rev %d %d\n", reversex, reversey);
		fprintf(file, "crvtype %d %d\n", curvetypeX, curvetypeY);
		fprintf(file, "freezept %d %d\n", freezex, freezey);
		for (loop = 0; loop < 128; loop++)
			fprintf(file, "x %d %f\n", loop, origX[loop]);
		for (loop = 0; loop < 128; loop++)
			fprintf(file, "y %d %f\n", loop, origY[loop]);

		fclose(file);
	}
}

void saveCurve(gchar *fname, gfloat *trans, char x_or_y)
{
FILE* file;
int loop;

	if (file = fopen(fname, "w"))
	{
		fprintf(file, "raton %s\n", RATON_VERSION);
		/* save current curvetype info in both tokens, loading as X or Y will get same crvtype */
		if (x_or_y == 'x')
			fprintf(file, "crvtype %d %d\n", curvetypeX, curvetypeX);
		else
			fprintf(file, "crvtype %d %d\n", curvetypeY, curvetypeY);

		for (loop = 0; loop < 128; loop++)
			fprintf(file, "n %d %f\n", loop, trans[loop]);

		fclose(file);
	}
}

void getFile(gchar *fname)
{
FILE* file;
char str[80];

int loop, count;
char *curtoken;
char *tokens[10];

	if (file = fopen(fname, "r"))
	{
		while ( fgets(str, 80, file) )
		{
			count = 0;
			curtoken = strtok(str, " \n\t");
			while (curtoken)
			{
				tokens[count] = curtoken;
				count++;
				curtoken = strtok(NULL, " \n\t");
			}
			if (tokens[0][0] == '#')
			{
				//g_print("a comment\n");
			}
			else if (tokens[0][0] == 'x')
			{
				int index = atoi(tokens[1]);
				gfloat value = (gfloat)atof(tokens[2]);
				if (index <= 127 && index >= 0)
					origX[index] = value;
				//g_print("x vector: %d %f\n", index, value);
			}
			else if (tokens[0][0] == 'y')
			{
				int index = atoi(tokens[1]);
				gfloat value = (gfloat)atof(tokens[2]);
				if (index <= 127 && index >= 0)
					origY[index] = value;
				//g_print("y vector: %d %f\n", index, value);
			}
			else if (! strcmp(tokens[0], "control"))
			{
				controllerx = atoi(tokens[1]);
				controllery = atoi(tokens[2]);
				g_print("controllers: %d %d\n", controllerx, controllery);

				GtkWidget *tempwidg1 = lookup_widget (GTK_WIDGET (drawptr), "controllerX");
				gtk_entry_set_text (GTK_ENTRY (tempwidg1), tokens[1]);

				GtkWidget *tempwidg2 = lookup_widget (GTK_WIDGET (drawptr), "controllerY");
				gtk_entry_set_text (GTK_ENTRY (tempwidg2), tokens[2]);
			}
			else if (! strcmp(tokens[0], "channel"))
			{
				channelx = atoi(tokens[1]);
				channely = atoi(tokens[2]);
				g_print("channels: %d %d\n", channelx, channely);

				GtkWidget *tempwidg1 = lookup_widget (GTK_WIDGET (drawptr), "chanX");
				gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg1), channelx + 1);

				GtkWidget *tempwidg2 = lookup_widget (GTK_WIDGET (drawptr), "chanY");
				gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg2), channely + 1);
			}
			else if (! strcmp(tokens[0], "inv"))
			{
				invertx = atoi(tokens[1]);
				inverty = atoi(tokens[2]);
				g_print("invert: %d %d\n", invertx, inverty);

				/* this checkbutton was named incorrectly (lowercase x) */
				GtkWidget *tempwidg1 = lookup_widget (GTK_WIDGET (drawptr), "invertx");
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg1), invertx);

				GtkWidget *tempwidg2 = lookup_widget (GTK_WIDGET (drawptr), "invertY");
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg2), inverty);
			}
			else if (! strcmp(tokens[0], "rev"))
			{
				reversex = atoi(tokens[1]);
				reversey = atoi(tokens[2]);
				g_print("reverse: %d %d\n", reversex, reversey);

				GtkWidget *tempwidg1 = lookup_widget (GTK_WIDGET (drawptr), "reverseX");
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg1), reversex);

				GtkWidget *tempwidg2 = lookup_widget (GTK_WIDGET (drawptr), "reverseY");
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg2), reversey);
			}
			else if (! strcmp(tokens[0], "crvtype"))
			{
				curvetypeX = atoi(tokens[1]);
				curvetypeY = atoi(tokens[2]);
				g_print("crvtype: %d %d\n", curvetypeX, curvetypeY);

			}
			else if (! strcmp(tokens[0], "freezept"))
			{
				freezex = atoi(tokens[1]);
				freezey = atoi(tokens[2]);
				g_print("freeze point: %d %d\n", freezex, freezey);

			}
		}
	}
	else
	{
	g_print("File Error: cannot load file %s\n", fname);
	
	char errortxt[255];
	perror(errortxt);
	g_print("%s", errortxt);
	}
}

void getCurve(gchar *fname, gfloat *trans, char x_or_y)
{
FILE* file;
char str[80];

int loop, count;
char *curtoken;
char *tokens[10];

	if (file = fopen(fname, "r"))
	{
		while ( fgets(str, 80, file) )
		{
			count = 0;
			curtoken = strtok(str, " \n\t");
			while (curtoken)
			{
				tokens[count] = curtoken;
				count++;
				curtoken = strtok(NULL, " \n\t");
			}

			if (tokens[0][0] == '#')
			{
				//g_print("a comment\n");
			}
			// will load curves stored in '.raton' files (x_or_y sets which dimension)
			// 'n' for neutral (load as either x or y), format of curve save
			else if (tokens[0][0] == x_or_y || tokens[0][0] == 'n')
			{
				int index = atoi(tokens[1]);
				gfloat value = (gfloat)atof(tokens[2]);
				if (index <= 127 && index >= 0)
					trans[index] = value;
				//g_print("x vector: %d %f\n", index, value);
			}
			else if (! strcmp(tokens[0], "crvtype"))
			{
				if (x_or_y == 'x')
					curvetypeX = atoi(tokens[1]);
				else
					curvetypeY = atoi(tokens[2]);
				//g_print("crvtype: %d %d\n", curvetypeX, curvetypeY);
			}
		}
	}
	else
	{
	g_print("File Error: cannot load file %s\n", fname);
	
	char errortxt[255];
	perror(errortxt);
	g_print("%s", errortxt);
	}
}

void redraw()
{
GdkRectangle rect;
	if (drawptr != NULL)
	{
	rect.x = 0;
	rect.y = 0;
	rect.width = drawptr->allocation.width;
	rect.height = drawptr->allocation.height;

	gtk_widget_draw (drawptr, &rect );
	}
}
/**************************************************/
void getcustomX()
{
	if (xcurve != NULL)
		gtk_curve_get_vector((GtkCurve *)xcurve, 128, origX);
}

void getcustomY()
{
	if (ycurve != NULL)
		gtk_curve_get_vector((GtkCurve *)ycurve, 128, origY);
}

void setcurveX()
{
	if (xcurve != NULL)
		gtk_curve_set_vector((GtkCurve *)xcurve, 128, origX);
}

void setcurveY()
{
	if (ycurve != NULL)
		gtk_curve_set_vector((GtkCurve *)ycurve, 128, origY);
}
/**************************************************/
// this is the 'std' transformation
void transLinear(gfloat *trans)
{
int loop;
	for (loop = 0; loop < 128; loop++)
	{
		trans[loop] = (gfloat)loop;
	}
	fixrange(trans);
}

void transStepped(gfloat *trans, int steps, int offset)
{
int loop, increm;
	increm = 128 / steps;
	
	//g_print("increm: %d\n",increm);
	for (loop = 0; loop < 128; loop++)
	{
		trans[loop] = (gfloat)(( loop / increm ) * increm) + offset;
	}
	fixrange(trans);
}

void transAddNoise(gfloat *trans, int noiseval)
{
int loop, rnum;

for (loop = 0; loop < 128; loop++)
	{
		rnum = (int)  ( ((double)rand() / (double)RAND_MAX) * noiseval);
		trans[loop] = (gfloat)(trans[loop] + rnum - noiseval / 2);
	}
	fixrange(trans);
}

void transRandom_old(gfloat *trans)
{
int loop, rnum;

	for (loop = 0; loop < 128; loop++)
	{
		rnum = (int)  ( ((double)rand() / (double)RAND_MAX) * 127.0);
		trans[loop] = (gfloat)rnum;
	}
	fixrange(trans);
}

void transRandom(gfloat *trans, int steps)
{
int loop, increm, count, rval;
	increm = 128 / steps;
	count = 0;
	
	//g_print("increm: %d\n",increm);
	rval = (int)( ((double)rand() / (double)RAND_MAX) * 127.0);
	for (loop = 0; loop < 128; loop++)
	{
		if (count >= increm)
		{
			rval = (int)( ((double)rand() / (double)RAND_MAX) * 127.0);
			count = 0;
		}
		trans[loop] = rval;
		count ++;
	}
	fixrange(trans);
}


void transFixed(gfloat *trans, gfloat value)
{
int loop;
	for (loop = 0; loop < 128; loop++)
	{
		trans[loop] = value;
	}
	fixrange(trans);
}

void transSine(gfloat *trans, double start)
{
int loop;
double in = start;
double out;
double period = MYPI / 2;
double increm = period/127;

	for (loop = 0; loop < 128; loop++)
	{
		out = sin(in);
		trans[loop] = (abs(out * 129));
		in = in + increm; 
		//g_print("%f ", out);
	}
	fixrange(trans);
}

void transinit()
{
	freezex = 64;
	freezey = 64;

	srand(time(NULL));
	transLinear(origX);
	transModifyX(origX);
	transLinear(origY);
	transModifyY(origY);
	redraw();
}

void localinit(GtkWidget *widget)
{

	prefs.feedback = 0;
	prefs.colorfb = 1;
	prefs.x_stepnum = 10;;
	prefs.x_stepoffset = 0.0;
	prefs.x_randomnum = 32;
	prefs.y_stepnum = 10;
	prefs.y_stepoffset = 0.0;
	prefs.y_randomnum = 32;

	strcpy(consolex, "1m");
	strcpy(consoley, "1;32m");

	getrc();
	transinit();

	pid = getpid();
	sprintf(titlestr, "ratón [%d]", pid);
	sprintf(midititle, "raton [%d]", pid);
	gtk_window_set_title (GTK_WINDOW (gtk_widget_get_toplevel(GTK_WIDGET(widget))), titlestr);
}

void sendcontrol(int x, int y)
{
  if ( x > 127)
	x = 127;
  if ( x < 0 )
	x = 0;
  if ( y > 127 )
	y = 127;
  if ( y < 0 )
	y = 0; 
 
  if (mouseactive)
  {
	  if (x != lastx)
	  {
		send_control_all(channelx, controllerx, (int)outX[x]);
		if (prefs.feedback)
		{
			if (prefs.colorfb)
				g_print("%c[%s", 0x1B, consolex );
			g_print("X: %d \tchan: %d \tcont: %d\n", (int)outX[x], channelx + 1, controllerx);
		}
	  }
	  if (y != lasty)
	  {
	  	send_control_all(channely, controllery, (int)outY[y]);
		if (prefs.feedback)
		{
			if (prefs.colorfb)
				g_print("%c[%s", 0x1B, consoley );
			g_print("Y: %d \tchan: %d \tcont: %d\n", (int)outY[y], channely + 1, controllery);
		}
	  }
	  g_print("%c[0m", 0x1B );
  }
  lastx = x;
  lasty = y;
}
/*****************************************************************/

gboolean
on_ralphwindow_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
  return FALSE;
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_linearX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transLinear(origX);
	transModifyX(origX);
	curvetypeX = GTK_CURVE_TYPE_LINEAR;
	redraw();
}


void
on_steppedX_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transStepped(origX, prefs.x_stepnum, prefs.x_stepoffset);
	transModifyX(origX);
	curvetypeX = GTK_CURVE_TYPE_FREE;
	redraw();
}


void
on_randomX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transRandom(origX, prefs.x_randomnum);
	transModifyX(origX);
	curvetypeX = GTK_CURVE_TYPE_FREE;
	redraw();
}


void
on_sineX_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transSine(origX, 0.0);
	transModifyX(origX);
	curvetypeX = GTK_CURVE_TYPE_SPLINE;
	redraw();
}

void
on_linearY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transLinear(origY);
	transModifyY(origY);
	curvetypeY = GTK_CURVE_TYPE_LINEAR;
	redraw();
}


void
on_steppedY_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transStepped(origY, prefs.y_stepnum, prefs.y_stepoffset);
	transModifyY(origY);
	curvetypeY = GTK_CURVE_TYPE_FREE;
	redraw();
}


void
on_randomY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transRandom(origY, prefs.y_randomnum);
	transModifyY(origY);
	curvetypeY = GTK_CURVE_TYPE_FREE;
	redraw();
}


void
on_sineY_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{	transSine(origY, 0.0);
	transModifyY(origY);
	curvetypeY = GTK_CURVE_TYPE_SPLINE;
	redraw();

}

void
on_customX_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  
  customXdialog = create_customXdialog ();
  gtk_widget_show (customXdialog);
  setcurveX();

	if (xcurve)
		gtk_curve_set_curve_type((GtkCurve *)xcurve, curvetypeX);
}


void
on_customY_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  customYdialog = create_customYdialog ();
  gtk_widget_show (customYdialog);
  setcurveY();

	if (ycurve)
		gtk_curve_set_curve_type((GtkCurve *)ycurve, curvetypeY);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	aboutdialog = create_aboutdialog ();
	gtk_widget_show (aboutdialog);
}


gboolean
on_drawingarea1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	//g_print("mouse pressed %d\n", event->button);
	if (event->button == 1)
		mouseactive = 0;
	else if (event->button == 2)
	{
		/* set the 'freeze' point*/
		freezex = (double)event->x / (double)width * 127.5;
		freezey = (double)event->y / (double)height * 127.5;
		redraw();
	}
	else if (event->button == 3)
	{
		sendcontrol(freezex, freezey); /* 'freeze' the controller at this point (button 1 thaws) */
		mouseactive = 0;
	}
  return FALSE;
}


gboolean
on_drawingarea1_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	//g_print("mouse released %d\n", event->button);
	if (event->button == 1)
		mouseactive = 1;
  return FALSE;
}


gboolean
on_drawingarea1_motion_notify_event    (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
 int x, y;
 
  x = (int)event->x;
  y = (int)event->y;
  
  x = (double)x / (double)width * 127.5;
  y = (double)y / (double)height * 127.5;
  
  sendcontrol(x, y);
  return FALSE;
}


void
on_drawingarea1_size_allocate          (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	width = (int)allocation->width;
	height = (int)allocation->height;
}


gboolean
on_drawingarea1_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
	/* if needed, change drawingarea to 'can get focus' */
	g_print("a key\n");
  return FALSE;
}

void setupgc()
{

	gcx = gdk_gc_new(drawptr->window);
	gcy = gdk_gc_new(drawptr->window);

	gdk_color_alloc(gtk_widget_get_colormap(drawptr), &cx);
	gdk_gc_set_rgb_fg_color(gcx, &cx);
	gdk_gc_set_line_attributes(gcx, 2, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);

	gdk_color_alloc(gtk_widget_get_colormap(drawptr), &cy);
	gdk_gc_set_rgb_fg_color(gcy, &cy);
	gdk_gc_set_line_attributes(gcy, 2, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
}

gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
int loop, curwidth, curheight;
int x1, y1, x2, y2;

if (ginit == 0)
{
	localinit(widget);
  	midicontrol_init(midititle);
	drawptr = widget;
	setupgc();
	ginit = 1;
}

curwidth = widget->allocation.width;
curheight = widget->allocation.height;

gdk_draw_rectangle (widget->window,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      curwidth,
		      curheight);

/* draw X dimension curve */
x1 = 0;
y1 = curheight - outX[0] * curheight / CONTROL_RES;
for (loop = 0; loop < 128; loop++)
{
	x2 = loop * curwidth / CONTROL_RES;
	y2 = curheight - outX[loop] * curheight / CONTROL_RES;
	gdk_draw_line(widget->window,
		gcx,
		(gint)x1 + 1, (gint)y1 - 1, (gint)x2 + 1, (gint)y2 - 1);
	x1 = x2;
	y1 = y2;
	
}

/* draw Y dimension curve */
x1 = outY[0] * curwidth / CONTROL_RES;
y1 = 0;
for (loop = 1; loop < 128; loop++)
{
	x2 = outY[loop] * curwidth / CONTROL_RES;
	y2 = loop * curheight / CONTROL_RES;
	gdk_draw_line(widget->window,
		gcy,
		(gint)x1 + 1, (gint)y1 + 1, (gint)x2 + 1, (gint)y2 + 1);
	x1 = x2;
	y1 = y2;
}

/* draw freeze point */
gdk_draw_rectangle (widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)], FALSE,
		      freezex * curwidth / CONTROL_RES - 2, freezey * curheight / CONTROL_RES - 2,
			5, 5);
  return FALSE;
}


gboolean
on_drawingarea1_leave_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{

  return FALSE;
}


gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  return FALSE;
}


gboolean
on_drawingarea1_enter_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{

  return FALSE;
}

/* X dimension interface */

void
on_invertx_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	/* this widget should have a uppercase 'X' in name*/
	GtkWidget *tempwidg = lookup_widget (GTK_WIDGET (button), "invertx");
	invertx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempwidg));

	transModifyX(origX);
	redraw();
}


void
on_reverseX_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tempwidg = lookup_widget (GTK_WIDGET (button), "reverseX");
	reversex = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempwidg));

	transModifyX(origX);
	redraw();
}

/* Y dimension interface */

void
on_invertY_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tempwidg = lookup_widget (GTK_WIDGET (button), "invertY");
	inverty = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempwidg));
	
	transModifyY(origY);
	redraw();
}


void
on_reverseY_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tempwidg = lookup_widget (GTK_WIDGET (button), "reverseY");
	reversey = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempwidg));
	
	transModifyY(origY);
	redraw();
}

void
on_dialog1_close                       (GtkDialog       *dialog,
                                        gpointer         user_data)
{

}


gboolean
on_dialog1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_curveX_curve_type_changed           (GtkCurve        *curve,
                                        gpointer         user_data)
{

}

gboolean
on_curveX_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	xcurve = widget;
  return FALSE;
}


void
on_curveXsetlinear_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	if (xcurve)
		gtk_curve_set_curve_type((GtkCurve *)xcurve, GTK_CURVE_TYPE_LINEAR);

	curvetypeX = GTK_CURVE_TYPE_LINEAR;
}


void
on_curveXsetspline_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	if (xcurve)
		gtk_curve_set_curve_type((GtkCurve *)xcurve, GTK_CURVE_TYPE_SPLINE);

	curvetypeX = GTK_CURVE_TYPE_SPLINE;
}


void
on_curveXsetfree_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	if (xcurve)
		gtk_curve_set_curve_type((GtkCurve *)xcurve, GTK_CURVE_TYPE_FREE);

	curvetypeX = GTK_CURVE_TYPE_FREE;
}


void
on_curveXreset_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	if (xcurve)
		gtk_curve_reset((GtkCurve *)xcurve);

	curvetypeX = GTK_CURVE_TYPE_LINEAR;
}


void
on_curveXopen_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	openfunction = OPEN_CURVEX;
	opendialog = create_opendialog1();
	gtk_widget_show (opendialog);
}


void
on_curveXsaveas_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	savefunction = SAVE_CURVEX;
	savedialog = create_savedialog1();
	gtk_widget_show (savedialog);
}


void
on_cancelbutton_curveX_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_okbutton1_curveX_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	getcustomX();
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	transModifyX(origX);
	redraw();
}


void
on_curveY_curve_type_changed           (GtkCurve        *curve,
                                        gpointer         user_data)
{

}


gboolean
on_curveY_configure_event              (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	ycurve = widget;
  return FALSE;
}


void
on_customYsetlinear_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	if (ycurve)
		gtk_curve_set_curve_type((GtkCurve *)ycurve, GTK_CURVE_TYPE_LINEAR);

	curvetypeY = GTK_CURVE_TYPE_LINEAR;
}


void
on_curveYsetspline_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	if (ycurve)
		gtk_curve_set_curve_type((GtkCurve *)ycurve, GTK_CURVE_TYPE_SPLINE);

	curvetypeY = GTK_CURVE_TYPE_SPLINE;
}


void
on_curveYsetfree_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	if (ycurve)
		gtk_curve_set_curve_type((GtkCurve *)ycurve, GTK_CURVE_TYPE_FREE);

	curvetypeY = GTK_CURVE_TYPE_FREE;
}


void
on_curveYreset_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	if (ycurve)
		gtk_curve_reset((GtkCurve *)ycurve);

	curvetypeY = GTK_CURVE_TYPE_LINEAR;
}


void
on_curveYopen_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	openfunction = OPEN_CURVEY;
	opendialog = create_opendialog1();
	gtk_widget_show (opendialog);
}


void
on_curveYsaveas_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	savefunction = SAVE_CURVEY;
	savedialog = create_savedialog1();
	gtk_widget_show (savedialog);
}


void
on_cancelbutton_curveY_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_okbutton_curveY_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	getcustomY();
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	transModifyY(origY);
	redraw();
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	openfunction = OPEN_FILE;
	opendialog = create_opendialog1();
	gtk_widget_show (opendialog);
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	savefunction = SAVE_FILE;
	savedialog = create_savedialog1();
	gtk_widget_show (savedialog);
}


void
on_openfile_cancel                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_savefile_cancel                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_savefile_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gchar *fname = gtk_file_chooser_get_filename((GtkFileChooser *)savedialog);
	g_print("save filename: %s\n", fname);

	if (fname)
	{
		switch (savefunction)
		{
		case SAVE_FILE:
		saveFile(fname);
		break;

		case SAVE_CURVEX:
		if (xcurve != NULL)
			{
			gtk_curve_get_vector((GtkCurve *)xcurve, 128, cX);
			saveCurve(fname, cX, 'x');
			}
		break;

		case SAVE_CURVEY:
		if (ycurve != NULL)
			{
			gtk_curve_get_vector((GtkCurve *)ycurve, 128, cY);
			saveCurve(fname, cY, 'y');
			}
		break;
		}
	}
	g_free(fname);

	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_openfile_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
 	gchar *fname = gtk_file_chooser_get_filename((GtkFileChooser *)opendialog);
	g_print("open filename: %s\n", fname);

	if (fname)
	{
		switch (openfunction)
		{
		case OPEN_FILE:
		getFile(fname);
		transModifyX(origX);
		transModifyY(origY);
		break;

		case OPEN_CURVEX:
		getCurve(fname, cX, 'x');
		if (xcurve != NULL)
			{
			gtk_curve_set_vector((GtkCurve *)xcurve, 128, cX);
			gtk_curve_set_curve_type((GtkCurve *)xcurve, curvetypeX);
			}
		break;

		case OPEN_CURVEY:
		getCurve(fname, cY, 'y');
		if (ycurve != NULL)
			{
			gtk_curve_set_vector((GtkCurve *)ycurve, 128, cY);
			gtk_curve_set_curve_type((GtkCurve *)ycurve, curvetypeY);
			}
		break;
		}
	}

	g_free(fname);
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));

	redraw();
}

void
on_reset1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	transinit();
}

/* All the transformations (v0.17) have been removed, as the correct way to proceed
is to create a new curve editing window (or widget), and not to depend on the
GTKcurve, which is inadequate (and depreciated.) The previous method involved dozens of
menu entries, and could easily be replaced with a decent editor.

They included:
shift 8 in 2 dimensions (16 total)
expand 4 in 2 dimensions (8)
compress 4 in 2 dimensions (8)
	or 32 menu entries!

The beta transformation functions are still included, but commented out. 

For now, using the GTKcurve editor will do. */

void
on_preferences_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	prefsbak = prefs;
	prefsdialog = create_prefsdialog();
	gtk_widget_show (prefsdialog);

	GtkWidget *tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "stdfeedback_button");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg), prefs.feedback);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "colorfeedback_button");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tempwidg), prefs.colorfb);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "xpre_stepnumber");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.x_stepnum);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "xpre_stepoffset");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.x_stepoffset);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "xpre_randnumber");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.x_randomnum);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "ypre_stepnumber");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.y_stepnum);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "ypre_stepoffset");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.y_stepoffset);

	tempwidg = lookup_widget (GTK_WIDGET (prefsdialog), "ypre_randnumber");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tempwidg), prefs.y_randomnum);
}


void
on_stdfeedback_button_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	prefsbak.feedback = gtk_toggle_button_get_active (togglebutton);
}


void
on_colorfeedback_button_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	prefsbak.colorfb = gtk_toggle_button_get_active (togglebutton);
}


void
on_prefsapply_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	prefs = prefsbak;
}


void
on_prefscancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	redraw();
}


void
on_prefsok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	prefs = prefsbak;
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	redraw();
	/* save prefs to file */
	saverc();
}


/* v0.22 */
/* callback functions below replaced the incorrect (incomplete) spinbutton callbacks */

void
on_controllerXspin_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	controllerx = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}


void
on_chanX_value_changed                 (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
int val;

	val = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
	channelx = val - 1;
}


void
on_controllerYspin_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	controllery = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}


void
on_chanY_value_changed                 (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
int val;

	val = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
	channely = val - 1;
}


void
on_xpre_stepnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.x_stepnum = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}


void
on_xpre_stepoffset_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.x_stepoffset = gtk_spin_button_get_value_as_float ((GtkSpinButton *)spinbutton);
}


void
on_xpre_randnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.x_randomnum = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}


void
on_ypre_stepnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.y_stepnum = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}


void
on_ypre_stepoffset_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.y_stepoffset = gtk_spin_button_get_value_as_float ((GtkSpinButton *)spinbutton);
}


void
on_ypre_randnumber_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	prefsbak.y_randomnum = gtk_spin_button_get_value_as_int ((GtkSpinButton *)spinbutton);
}

