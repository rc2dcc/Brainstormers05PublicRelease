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

#ifndef _BIT_FIFO_H_
#define _BIT_FIFO_H_

#include <iostream>


class BitFIFO {
  static const int MAX_NUM_BITS= 100;
  int first, next_free;
  
  bool tab[MAX_NUM_BITS];
  int get_free_size();
public:
  BitFIFO() { reset(); }
  int get_size();
  void reset() { first= 0; next_free= 0; };
  bool put(int num_bits, unsigned int in);
  bool fill_with_zeros(int num_bits);
  bool get(int num_bits, unsigned int & out);
  void show(std::ostream & out) const;
};

#endif







