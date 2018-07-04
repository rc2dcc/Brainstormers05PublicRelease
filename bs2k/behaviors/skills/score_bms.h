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

#ifndef _SCORE_BMS_H_
#define _SCORE_BMS_H_

#include "../base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"
#include "intention.h"
#include "oneortwo_step_kick_bms.h"

class Score : public BaseBehavior {
  static bool initialized;

  OneOrTwoStepKick *oneortwo;

 private:

  int nr_of_looks_to_goal;
  Value saved_velocity;
  Value saved_direction;
  Vector saved_target;
  int saved_multi_step;
  int risky_goalshot_possible;
  int last_time_look_to_goal;
  float goalshot_param1;
  int goalshot_mode;


  // auxillary functions for shoot2goal:
  float get_orientation_and_speed_handicap_add(Vector target);
  Value player_action_radius_at_time(int time, PPlayer player, 
				     Value player_dist_to_ball, int player_handicap);
  int intercept_opponents(Value direction, Value b_v, int max_steps);
  Value goalie_action_radius_at_time(int time, Value goalie_size, int goalie_handicap);
  int intercept_goalie(Vector ball_pos, Vector ball_vel, Vector goalie_pos, Value goalie_size);
  bool goalie_needs_turn_for_intercept(int time, Vector initial_ball_pos, Vector initial_ball_vel, 
				       Vector b_pos, Vector b_vel, Value goalie_size);
  Vector intersection_point(Vector p1, Vector steigung1, Vector p2, Vector steigung2);
  Vector point_on_line(Vector steigung, Vector line_point, Value x);
  bool is_pos_in_quadrangle(Vector pos, Vector p1, Vector p2, Vector p3, Vector p4);
  bool is_pos_in_quadrangle(Vector pos, Vector p1, Vector p2, Value width);

  int get_goalshot_chance(int &multi_step, Value &velocity, 
			  Value &direction, Vector &target, int &best_index);
  void consider_special_cases(int goalie_age, int goalie_vel_age, Vector goalie_vel,
			      Value &goalie_size, Vector &goalie_pos, PPlayer goalie);
  int select_best_kick(int *kick_possible, int nr_of_targets);
  void fill_target_arrays(Vector *test_targets, Value *test_dirs, int nr_of_targets, Vector ball_pos);
  void fill_velocity_arrays(Value *test_vels_1step, Value *test_vels_multi, 
			    Vector *test_targets, int nr_of_targets);

  // end of auxillary functions for shoot2goal:


 public:

  Score();
  virtual ~Score();

  bool test_shoot2goal(Intention &intention);

  bool get_cmd(Cmd &cmd);         
				    
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    std::cout << "\nScore behavior initialized.";
    return OneOrTwoStepKick::init(conf_file, argc, argv);
  }
};

#endif // _SCORE_BMS_H_
