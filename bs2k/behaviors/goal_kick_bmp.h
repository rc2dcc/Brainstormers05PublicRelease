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

#ifndef _GOAL_KICK_BMP_H_
#define _GOAL_KICK_BMP_H_

/* 
   
*/

#include "base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"
#include "skills/basic_cmd_bms.h"
#include "skills/face_ball_bms.h"
//#include "skills/dribble_bms.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/one_step_kick_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/neuro_kick05_bms.h"

#define MAX_GFK_POS 20

class GoalKick : public BaseBehavior {
  static bool initialized;

  static Value homepos_tolerance;
  static int homepos_stamina_min;
  static int wait_after_catch;
  static int max_wait_after_catch;

  BasicCmd *basiccmd;
  NeuroGo2Pos *go2pos;
  FaceBall *faceball;
  OneStepKick *onestepkick;
  OneOrTwoStepKick *onetwokick;
  NeuroKick05 *neurokick;
  NeuroIntercept *intercept;

  enum {GFK_MODE,GK_MODE};
  
  long last_called;
  long seq_started;
  long play_on_cnt;
  int goal_kick_mode;

  bool move_to_home;

  int mode;
  
  Value wished_kick_vel;
  Vector wished_kick_pos;
  Vector kickoff_pos;
  Vector target_pos;
  Value target_vel;

  Vector bestpos,bestvel;


  
  bool get_goalie_cmd(Cmd &cmd);
  bool get_player_cmd(Cmd &cmd);

  /* data for goalie free kick */
  static int gfk_pos_num;
  static Vector gfk_kickoff_pos[];
  static Vector gfk_target_pos[];
  static Value gfk_kickvel[];
  int gfk_rand_arr[MAX_GFK_POS];

  int gfk_goalie_mode;
  Vector gfk_tpos;
  
  /* methods for goalie free kick */
  bool get_gfk_player_cmd(Cmd &cmd);
  bool get_gfk_goalie_cmd(Cmd &cmd);
  bool panic_gfk(Cmd &cmd);

  /* data for goal kick */
  int gk_goalie_mode;
  Vector gk_tpos;
  bool gk_left;
  int cyc_cnt;
  bool wing_not_possible;
  
  /* methods for goal kick */
  bool get_gk_goalie_cmd(Cmd &cmd);
  bool panic_gk(Cmd &cmd);
  
  bool scan_field(Cmd &cmd);
 public:
  bool get_cmd(Cmd &cmd);
  
  static bool init(char const * conf_file, int argc, char const* const* argv);

  GoalKick();
  virtual ~GoalKick() {
    delete go2pos; delete basiccmd; delete onetwokick; delete faceball;
    delete intercept; delete onestepkick;
  }
};

#endif
