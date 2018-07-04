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

#ifndef _SORT_H_
#define _SORT_H_


class Sort{
 private:
  typedef struct SortType{
    int key;
    float value;
  };
  SortType *sortarray;
  int sortarray_size, sortarray_maxsize;
 public:
  Sort(int number);
  ~Sort() {
    delete[] sortarray;
  };
  void add(int key, float value);
  void do_sort(int type = 0); //type 0 : smallest value first
  void reset();
  int get_key(int pos);
  float get_value(int pos);
};
#endif
