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
   the Free Software Foundation, e	Synaptics Palm Detection (301):	0
        Synaptics Palm Dimensions (302):	10, 199
        Synaptics Coasting Speed (303):	0.000000
        Synaptics Pressure Motion (304):	29, 159
        Synaptics Pressure Motion Factor (305):	1.00ither version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QApplication>
#include "QTimer"
#include "mainwindow.h"
#include "math.h"
#include "stdio.h"

MainWindow *w;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    w = new MainWindow();
    w->App = &a ;

    w->show();
    printf("midiball started\n") ;

    return a.exec();
}
