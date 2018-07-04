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

#include "bit_fifo.h"

int BitFIFO::get_free_size() {
  return MAX_NUM_BITS - get_size();
}

int BitFIFO::get_size() {
  if (next_free >= first)
    return next_free-first;

  return MAX_NUM_BITS - first + next_free;
}

bool BitFIFO::put(int num_bits, unsigned int in) {  
  int idx= next_free;
  for (int i=0; i<num_bits; i++) {
    tab[idx]= in & 1;
    in >>= 1;
    idx++;
    if ( idx >= MAX_NUM_BITS )
      idx= 0;
    if ( idx == first )
      return false;
  }
  next_free= idx;
  return true;
}

bool BitFIFO::fill_with_zeros(int num_bits) {
  int idx= next_free;
  for (int i=0; i<num_bits; i++) {
    tab[idx]= 0;
    idx++;
    if ( idx >= MAX_NUM_BITS )
      idx= 0;
    if ( idx == first )
      return false;
  }
  next_free= idx;
  return true;
}

bool BitFIFO::get(int num_bits, unsigned int & output) {
  int idx= first+ num_bits;
  if (idx >= MAX_NUM_BITS)
    idx -= MAX_NUM_BITS;

  int new_first= idx;

  unsigned int out = 0;
  for (int i=0; i<num_bits; i++) {
    if (idx == 0)
      idx= MAX_NUM_BITS;
    idx--;

    if ( idx == next_free )
      return false;

    out <<= 1;
    if ( tab[idx] ) 
      out |= 1;
  }
  first= new_first;
  output= out;
  return true;
}

void BitFIFO::show( std::ostream & out) const {
  out << "[" << first << ":" << next_free <<  " |";
  int idx= first;
  while (idx != next_free) {
    if ( idx % 5 == 0 )
      out << ' ';

    out << tab[idx];

    idx++;
    if (idx == MAX_NUM_BITS)
      idx=0;
  }
  out << "]";
}

#if 0
int main() {
  BitFIFO fifo;
  for (;;) {
    int tmp;
    //fifo.reset();
    cout << "\n------------------------------------------";
    cout << "\nbefore= "; fifo.show(cout);
    cout << "\n<int>%";
    cin >> tmp;
    cout << "\nyour input = " << tmp;
    bool res= fifo.put(32,tmp);
    if ( ! res )
      cout << "\nput wasn't successful";
    cout << "\nafter = "; fifo.show(cout);
    unsigned int tmp2;
    res= fifo.get(32, tmp2);
    cout << "\n fifo delivers= " << tmp2 << " (res= " << res << ")";
    cout << "\nafter (2nd) = "; fifo.show(cout);
  }
  return 1;
}


#endif
