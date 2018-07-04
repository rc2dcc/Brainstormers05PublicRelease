/*
Brainstormers 2D (Soccer Simulation League 2D)
PUBLIC SOURCE CODE RELEASE 2005
Copyright (C) 1998-2005 Neuroinformatics Group,
                        University of Osnabrueck, Germany

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
 *  Author:   Artur Merke 
 */

#ifndef _GEOMETRY2D_H_
#define _GEOMETRY2D_H_

#include "object2d.h"

class Geometry2d {
public:
  static int intersect_circle_with_circle( Point2d & p1, Point2d & p2, const Circle2d & c1, const Circle2d & c2);


#if 0
   static int intesect_circle_with_x_line(float &x1,float &y1, float &x2, float &y2,
                   float Cx, float Cy, float r,float  xline);

   static int intesect_circle_with_y_line(float &x1,float &y1, float &x2, float &y2,
                   float Cx, float Cy, float r, float yline);

   /* gegeben 2 Punkte (x1,y1) (x2,y2), gesucht sind die beiden Kreise, auf denen die Punkte
      (px,py) liegen, so dass der Winkel zwischen den Vektoren (x1-px,y1-py) und 
      (x2-px,y2-py) den Wert von "angle" hat. Bem: ist der Winkel >90 Grad, so liegen die
      gesuchten Punkte auf dem "laengeren" Bogen des Kreise, wobei der Kreis durch die 
      Strecke von (x1,y1) und (x2,y2) in zwei Bogenabschnitte geteilt wird */
   static int circles_with_angle(float &Cx1, float &Cy1, float &Cx2, float &Cy2,float &radius,
                       float x1, float y1, float x2, float y2, float  angle);

  

#endif
};


#endif








