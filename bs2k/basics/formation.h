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

#ifndef _FORMATION_H_
#define _FORMATION_H_

#include "globaldef.h"
#include "ws_pset.h"

class BaseFormation {
 public:
  virtual int get_role(int number)= 0;
 
  virtual Vector get_grid_pos(int number)= 0;
  virtual bool   need_fine_positioning(int number)= 0;
  virtual Vector get_fine_pos(int number)= 0;

  virtual void get_boundary(Value & defence, Value & offence)= 0;
};

class Formation433 : BaseFormation {
  struct Home {
    Vector pos;
    Value stretch_pos_x;
    Value stretch_neg_x;
    Value stretch_y;
    int role;
  };
  Home home[NUM_PLAYERS+1];
  int boundary_update_cycle;
  Value defence_line, offence_line; Vector intercept_ball_pos; //this values are set in get_boundary, and can be used as cached values in the same cycle
 public:
  //Formation433() { init(0,0,0); }// test
  bool init(char const * conf_file, int argc, char const* const* argv);
  int get_role(int number);
  Vector get_grid_pos(int number);
  bool   need_fine_positioning(int number);
  Vector get_fine_pos(int number);

  void get_boundary(Value & defence, Value & offence);
  Value defence_line_ball_offset;
};


#endif
