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


#ifndef BALL_CPP
#define BALL_CPP

#endif // BALL_CPP

#include <stdlib.h>
#include <cstdlib>
#include "ball.h"
#include <math.h>

float Ball::Random(void) { return((float)(rand())/(float)(RAND_MAX)) ; } ;
float Ball::Random2(void) { return(2*(float)(rand())/(float)(RAND_MAX)-1) ; } ;

Ball::Ball() { x=Random() ; y=Random() ; xvel=.01 ; yvel=0.01 ; maxvel = 0.1 ; jitter=0 ;}

int Ball::Move(void)
{
    int code ;

    code = 0 ;
    if (x+xvel>1) { xvel=-xvel*(1+jitter*Random2()); code = 1 ;}
    if (x+xvel<0) { xvel=-xvel*(1+jitter*Random2()); code = 2 ;}
    if (y+yvel>1) { yvel=-yvel*(1+jitter*Random2()); code = 3 ;}
    if (y+yvel<0) { yvel=-yvel*(1+jitter*Random2()); code = 4 ;}
    x+=xvel ; y+=yvel  ;
    return(code) ;
}




