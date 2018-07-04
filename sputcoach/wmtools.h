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

#ifndef _WMTOOLS_H_
#define _WMTOOLS_H_

#include <math.h>

class WMTOOLS {
 public:
  static const double TwoPi= 2*M_PI;

  static double conv_server_angle_to_angle_0_2Pi(double s_ang) {
    double tmp= (-s_ang * M_PI)/ 180.0;
    tmp= fmod(tmp,TwoPi);
    if (tmp < 0) tmp+= TwoPi;
    return tmp;
  }

  /*
  static double conv_server_x_to_x(double x) { return x; }
  static double conv_server_y_to_y(double y) { return -y; }
  */

  /** \short singned 18 bit integer to 3 byte ascii using 64 fix characters

      this function encodes an integer in the range [2^18-1, ..., 2^18-1]
      other integers are treated as 2^18, which represents the infinity value
      
      the pointer dst should point to a memeory with 3 bytes free after it!
  */
  static bool int18_to_a3x64(int src, char * dst);
  /** \short 3 byte ascii using 64 fix characters to signed integer with max 18 bit
      The value 2^18 indicates an overflow!
      
      the pointer src should point to a memeory with 3 bytes free after it and
      with valid characters.
  */
  static bool a3x64_to_int18(const char * src, int & a);


  /// [0,...,62] -> ascii char
  static bool uint6_to_a64(int src, char * dst);
  
  /// ascii char to [0,...,62], 63 indicates overflow
  static bool a64_to_uint6(const char * src, int & dst);


};
  
#endif
