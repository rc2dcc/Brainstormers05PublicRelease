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
 * Copyright (c) 1999 - 2000, Artur Merke 
 */

#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

#include "globaldef.h"

struct Point2d {
  Point2d() { x= 0.0; y= 0.0; }
  Point2d(Value xx, Value yy) { x= xx; y= yy;}
  Value x,y;
};

struct Line2d {
  Point2d p1;
  Point2d p2;
  Line2d() {}
  Line2d(const Point2d& pp1, const Point2d& pp2) { p1= pp1; p2= pp2;}
};

struct Circle2d {
  Point2d center;
  Value radius;
  Circle2d() { radius= 0.0; }
  Circle2d(const Point2d& c,Value r) { center= c; radius= r; }
};

#endif
