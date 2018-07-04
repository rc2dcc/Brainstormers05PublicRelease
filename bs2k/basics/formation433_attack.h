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

#ifndef _FORMATION433_ATTACK_H_
#define _FORMATION433_ATTACK_H_

#include "globaldef.h"
#include "formation.h"

#define NUM_FORMATIONS 4
#define STANDARD 0
#define CLOSE2GOAL 1
#define LEFT_WING 2
#define RIGHT_WING 3

class Formation433Attack : BaseFormation {
  struct Home {
    Vector pos;
    Value delta_x;  // deviation from standard homepos
    Value delta_y;  //
    int role;
  };
  Home home[NUM_FORMATIONS][NUM_PLAYERS+1];
  int boundary_update_cycle;
  Value defence_line, offence_line; //this values are set in get_boundary, and can be used as cached values in the same cycle
  int formation_state_updated_at;
  int previous_formation_state, current_formation_state;
  int get_formation_state();

 public:
  bool init(char const * conf_file, int argc, char const* const* argv);
  int get_role(int number);
  Vector get_grid_pos(int number);
  bool   need_fine_positioning(int number);
  Vector get_fine_pos(int number);
  void get_boundary(Value & defence, Value & offence);
};

#endif
