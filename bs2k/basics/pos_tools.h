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

#ifndef _POS_TOOLS_H_
#define _POS_TOOLS_H_

#include "ws_info.h"

struct PosValue {
  //PosValue () {};
  Vector pos;

  bool valid;

  PPlayer first;
  Value first_time_to_pos;
  
  PPlayer second;
  Value second_time_to_pos;

  PPlayer first_opponent;
  Value first_opponent_time_to_pos;

  Value initial_ball_vel_to_get_to_pos_in_time_of_first;
  Value initial_ball_vel_to_get_to_pos_in_time_of_second;
  Value initial_ball_vel_to_get_to_pos_in_time_of_first_opponent;
};


class PosSet {
  Value time_to_pos( Vector & pos, PPlayer );
  void evaluate_position( PosValue & pos, WSpset & pset );
public:
  PosSet() {
    require_my_team_to_be_first= true;
    require_me_to_be_first= false;
    require_my_team_to_be_at_least_second= true;
    require_me_to_be_at_least_second= false;
  }

  static const int max_num= 120;
  PosValue position[max_num]; 
  int num;

  void reset_positions() { num= 0; }

  //following parameter can customize (and accelerate) the computations
  bool require_my_team_to_be_first;
  bool require_me_to_be_first;
  bool require_my_team_to_be_at_least_second;
  bool require_me_to_be_at_least_second;

  void draw_positions() const;
  void evaluate_positions( WSpset & pset );
  bool add_grid(Vector pos, int res1, Vector & dir1, int res2, Vector & dir2);
  bool add_his_goal_area();
};



#endif
