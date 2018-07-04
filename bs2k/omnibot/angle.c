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
 *
 * This file is part of FrameView2d.
 *
 * FrameView2d is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FrameView2d is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FrameView2d; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "angle.h"
#include <math.h>

double cos(const ANGLE &a) {
  return cos(a.ang);
}

double sin(const ANGLE &a) {
  return sin(a.ang);
}

#if 1
ostream& operator<< (ostream& o,const ANGLE &a) {
  return o << a.ang;
}

istream& operator>> (istream& i,ANGLE &a) {
  cout << "\n istream& operator>> (istream& i,ANGLE &a)";
  return i >> a.ang;
}
#endif

ANGLE operator+(const ANGLE &a1, const ANGLE &a2) {
  ANGLE res;
  res.ang= a1.ang+a2.ang;
  if (res.ang >= ANGLE::TwoPi) 
    res.ang-= ANGLE::TwoPi;
  return res;
}

ANGLE operator-(const ANGLE &a1, const ANGLE &a2) {
  ANGLE res;
  res.ang= a1.ang-a2.ang;
  if (res.ang < 0.0) 
    res.ang+= ANGLE::TwoPi;
  return res;
}

void ANGLE::operator +=(const ANGLE & a) {
  ang += a.ang;
  if (ang >= TwoPi) 
    ang-= TwoPi;
}

void ANGLE::operator -=(const ANGLE & a) {
  ang -= a.ang;
  if (ang < 0.0) 
    ang+= TwoPi;
}

ANGLE::ANGLE() {
  ang= 0.0;
}

ANGLE::ANGLE( const ANGLE& a) {
  ang= a.ang;
}

ANGLE::ANGLE( double d) {
  set_value(d);
}

void ANGLE::set_value( double d) {
  ang= fmod(d,TwoPi);
  if (ang < 0.0) 
    ang+= TwoPi;
}
