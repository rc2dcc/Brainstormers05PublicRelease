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

#ifndef _NEURO_KICK_BMS_H_
#define _NEURO_KICK_BMS_H_

/* This behavior is derived from the (now deprecated) move neuro_kick2.
   Only its operation mode 1 (using the BS01 nets) is implemented.
   Operation mode 0 (using BS2k nets) has not been converted, since this
   mode has not been used for quite a while now.
   Operation mode 2 is not possible either, meaning that there are no learning
   functions in this behavior.

   NeuroKick usually takes the current player position as reference point,
   but you can override this by calling set_state() prior to any other function.
   This makes it possible to "fake" the player's position during the current cycle.
   Use reset_state to re-read the WS information, or wait until the next cycle.

   Note that kick_to_pos_with_final_vel() is rather unprecise concerning the
   final velocity of the ball. I don't know how to calculate the needed starting
   vel precisely, so I have taken the formula from the original Neuro_Kick2 move
   (which was even more unprecise...) and tweaked it a bit, but it is still
   not perfect.

   Some bugs of the original Neuro_Kick2 move have been found and fixed:

   * Collision check did not work correctly (player's vel was ignored)
   * kick_to_pos used the player as reference point instead of the ball pos,
     resulting in rather unprecise kicks.
   
   The NeuroKick behavior can make use of the OneOrTwoStepKick (default).
   
   (w) 2002 by Manuel Nickschas
*/

#include "../base_bm.h"
#include "angle.h"
#include "Vector.h"
#include "tools.h"
#include "cmd.h"
#include "n++.h"
#include "macro_msg.h"
#include "valueparser.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"
#include "oneortwo_step_kick_bms.h"
#include "mystate.h"


class NeuroKickItrActions {
  /* set params */

  static const Value kick_pwr_min = 10;
  static const Value kick_pwr_inc = 10;
  static const Value kick_pwr_steps = 10;
  static const Value kick_fine_pwr_inc = 1;
  static const Value kick_fine_pwr_steps = 10;
  static const Value kick_ang_min = 0;
  static const Value kick_ang_inc = 2*PI/45;
  static const Value kick_ang_steps = 45;
  static const Value kick_fine_ang_inc = 2*PI/360;
  static const Value kick_fine_ang_steps = 90;

  Value pwr_min,pwr_inc,pwr_steps,ang_steps;
  ANGLE ang_min,ang_inc;
  
  Cmd_Main action;
  int ang_done,pwr_done;
  Value pwr;ANGLE ang;
 public:
  void reset(bool finetune=false,Value orig_pwr=0,ANGLE orig_ang=ANGLE(0)) {
    if(!finetune) {
      ang_min = ANGLE(kick_ang_min);
      ang_inc = ANGLE(kick_ang_inc);
      ang_steps = kick_ang_steps;
      pwr_min = kick_pwr_min;
      pwr_inc = kick_pwr_inc;
      pwr_steps = kick_pwr_steps;
    } else {
      ang_min = orig_ang-ANGLE(.5*kick_fine_ang_steps*kick_fine_ang_inc);
      ang_inc = ANGLE(kick_fine_ang_inc);
      ang_steps = kick_fine_ang_steps + 1;
      pwr_min = orig_pwr-(.5*kick_fine_pwr_steps*kick_fine_pwr_inc);
      pwr_inc = kick_fine_pwr_inc;
      pwr_steps = kick_fine_pwr_steps + 1;
    }
    ang_done = 0;pwr_done=0;
    ang = ang_min; pwr = pwr_min;
  }
  
  Cmd_Main *next() {
    if(pwr_done<pwr_steps && ang_done<ang_steps) {
      action.unset_lock();
      action.unset_cmd();
      action.set_kick(pwr,ang.get_value_mPI_pPI());
      ang+=ang_inc;
      if(++ang_done>=ang_steps) {
	ang=ang_min;ang_done=0;
	pwr+=pwr_inc;
	pwr_done++;
      }
      return &action;
    }
    return NULL;
  }
};

class NeuroKick: public BaseBehavior {
  static bool initialized;

#if 0
  struct State {
    Vector my_vel;
    Vector my_pos;
    ANGLE  my_ang;
    Vector ball_pos;
    Vector ball_vel;
  };
#endif
  
  static const Value kick_finetune = 2*PI/360.;
  static const Value kick_finetune_power = 1;
  static const Value turn_finetune = 0;
  
  static const Value tolerance_velocity = 0.2;
  static const Value tolerance_direction = 0.05;
  static const Value min_ball_dist = 0.15;
  static const Value kickable_tolerance = 0.15;

  static bool use_12step_def;
  static bool do_finetuning;
  
  static Net *nets[2];
  Net *net; // chosen net
    
  NeuroKickItrActions itr_actions;

  OneOrTwoStepKick *twostepkick;

  long init_in_cycle;
  Value target_vel;
  ANGLE target_dir;
  Vector target_pos;
  bool do_target_tracking;
  bool use_12step;

  MyState fake_state;
  long fake_state_time;
  void get_ws_state(MyState &);
  MyState get_cur_state();
  
  void get_features(const MyState &state, ANGLE dir, Value vel,float *net_in);
  Net *choose_net();
  bool is_success(const MyState&);
  bool is_failure(const MyState&);

  Value evaluate(MyState const &state);
  bool decide(Cmd &cmd);

 public:
  static bool init(char const *conf_file, int argc, char const* const* argv);

  /** This makes it possible to "fake" WS information.
      This must be called _BEFORE_ any of the kick functions, and is valid for 
      the current cycle only.
  */
  void set_state(const Vector &mypos,const Vector &myvel,const ANGLE &myang,
		 const Vector &ballpos,const Vector &ballvel);
  
  /** Resets the current state to that found in WS.
      This must be called _BEFORE_ any of the kick functions.
  */
  void reset_state();
  
  void kick_to_pos_with_initial_vel(Value vel,const Vector &pos,bool use_12step = use_12step_def);
  void kick_to_pos_with_final_vel(Value vel, const Vector &pos,bool use_12step = use_12step_def);
  void kick_to_pos_with_max_vel(const Vector &pos,bool use_12step = use_12step_def); 
  void kick_in_dir_with_initial_vel(Value vel, const ANGLE &dir,bool use_12step = use_12step_def);
  void kick_in_dir_with_max_vel(const ANGLE &dir,bool use_12step = use_12step_def);
  
  bool get_cmd(Cmd & cmd);
  NeuroKick() {
    init_in_cycle = -1;
    fake_state_time = -1;
    twostepkick = new OneOrTwoStepKick();
  }
  virtual ~NeuroKick();
};
    
#endif
