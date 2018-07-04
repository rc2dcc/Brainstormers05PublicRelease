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


#ifndef _ANGLE_H_
#define _ANGLE_H_

#include <math.h>
#include <iostream>

class ANGLE {
  friend std::ostream& operator<< (std::ostream&,const ANGLE&);
  friend std::istream& operator>> (std::istream&,ANGLE&);
  friend double cos(const ANGLE &);
  friend double sin(const ANGLE &);
  friend ANGLE operator+(const ANGLE &, const ANGLE &);
  friend ANGLE operator-(const ANGLE &, const ANGLE &);
  
  double ang; 
  static const double TwoPi= 2*M_PI;
  //important class invariant:  0.0 <= ang < 2*PI
 public:
  ANGLE();
  ANGLE( const ANGLE& );
  //init to the angle of the incoming vector
  ANGLE( double x, double y);
  explicit ANGLE( double );
  void operator+=(const ANGLE &);
  void operator-=(const ANGLE &);
  double get_value() const { return ang; }
  double get_value_0_p2PI() const { return ang; }
  double get_value_mPI_pPI() const { return ang<= M_PI ? ang: ang-TwoPi; }
  /** sign \in {-1,1}, value \in [0,PI) and 
      ang= sign * value mod 2PI
   */
  void signed_value_0_PI(double & sign, double & val) const {
    if ( ang <= M_PI ) {
      val= ang;	
      sign= 1.0;
      return;
    }
    val= TwoPi -ang;
    sign= -1.0;
  }
  /** computes the angle difference between self.ang and the parameter.ang
      the result should be in [0,Pi] */
  double diff(const ANGLE & a) const;
  void set_value(double);
};

#endif

