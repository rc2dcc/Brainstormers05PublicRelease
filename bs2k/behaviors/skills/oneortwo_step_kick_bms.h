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

#ifndef _ONEORTWO_STEP_KICK_BMS_H_
#define _ONEORTWO_STEP_KICK_BMS_H_

/* This behavior is a port of Move_1or2_Step_Kick into the behavior
   framework. get_cmd will try to kick in one step and otherwise return
   a cmd that will start a two-step kick. There are some functions
   that return information about the reachable velocities and the needed
   steps, so that you can prepare in your code for what this behavior will
   do.

   This behavior usually takes the current player position as reference point,
   but you can override this by calling set_state() prior to any other function.
   This makes it possible to "fake" the player's position during the current cycle.
   Use reset_state to re-read the WS information, or wait until the next cycle.

   Note that kick_to_pos_with_final_vel() is rather unprecise concerning the
   final velocity of the ball. I don't know how to calculate the needed starting
   vel precisely, so I have taken the formula from the original Neuro_Kick2 move
   (which was even more unprecise...) and tweaked it a bit, but it is still
   not perfect.

   Note also that this behavior, as opposed to the original move, has a working
   collision check - the original move ignored the player's vel...

   (w) 2002 Manuel Nickschas
*/


#include "../base_bm.h"
#include "one_step_kick_bms.h"
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
#include "mystate.h"
#include "../../policy/abstract_mdp.h"

class OneOrTwoStepKickItrActions {
  static const Value kick_pwr_min = 20;
  static const Value kick_pwr_inc = 10;
  static const Value kick_pwr_max = 100;
  static const Value kick_ang_min = 0;
  static const Value kick_ang_inc = 2*PI/8.;
  //  static const Value kick_ang_inc = 2*PI/36.; // ridi: I think it should be as much! too much
  static const Value kick_ang_max = 2*PI-kick_ang_inc;
  static const Value dash_pwr_min = 20;
  static const Value dash_pwr_inc = 20;
  static const Value dash_pwr_max = 100;
  static const Value turn_ang_min = 0;
  static const Value turn_ang_inc = 2*PI/18.;
  //  static const Value turn_ang_max = 2*PI-turn_ang_inc;
  static const Value turn_ang_max = 0;  // ridi: do not allow turns

  static const Value kick_pwr_steps = (kick_pwr_max-kick_pwr_min)/kick_pwr_inc + 1;
  static const Value dash_pwr_steps = (dash_pwr_max-dash_pwr_min)/dash_pwr_inc + 1;
  static const Value turn_ang_steps = (turn_ang_max-turn_ang_min)/turn_ang_inc + 1;
  static const Value kick_ang_steps = (kick_ang_max-kick_ang_min)/kick_ang_inc + 1;
  
  Cmd_Main action;

  Value kick_pwr,dash_pwr;
  ANGLE kick_ang,turn_ang;
  int kick_pwr_done,kick_ang_done,dash_pwr_done,turn_ang_done;

 public:
  void reset() {
    kick_pwr_done=0;kick_ang_done=0;dash_pwr_done=0;turn_ang_done=0;
    kick_pwr=kick_pwr_min;kick_ang= ANGLE(kick_ang_min);dash_pwr=dash_pwr_min;
    turn_ang=ANGLE(turn_ang_min);
  }

  Cmd_Main *next() {
    if(kick_pwr_done<kick_pwr_steps && kick_ang_done<kick_ang_steps) {
      action.unset_lock();
      action.unset_cmd();
      action.set_kick(kick_pwr,kick_ang.get_value_mPI_pPI());
      kick_ang+= ANGLE(kick_ang_inc);
      if(++kick_ang_done>=kick_ang_steps) {
	kick_ang=ANGLE(kick_ang_min);
	kick_ang_done=0;
	kick_pwr+=kick_pwr_inc;
	kick_pwr_done++;
      }
      return &action;
    }
    if(dash_pwr_done<dash_pwr_steps) {
      action.unset_lock();
      action.unset_cmd();
      action.set_dash(dash_pwr);
      dash_pwr+=dash_pwr_inc;
      dash_pwr_done++;
      return &action;
    }
    if(turn_ang_done<turn_ang_steps) {
      action.unset_lock();
      action.unset_cmd();
      action.set_turn(turn_ang);
      turn_ang+= ANGLE(turn_ang_inc);
      turn_ang_done++;
      return &action;
    }
    return NULL;
  }
};

class OneOrTwoStepKick: public BaseBehavior {
  static bool initialized;
#if 0
  struct MyState {
    Vector my_vel;
    Vector my_pos;
    ANGLE  my_angle;
    Vector ball_pos;
    Vector ball_vel;
    Vector op_pos;
    ANGLE op_bodydir;
  };
#endif

  OneStepKick *onestepkick;

  OneOrTwoStepKickItrActions itr_actions;
  
  Cmd_Main result_cmd1,result_cmd2;
  Value result_vel1,result_vel2;
  bool result_status;
  bool need_2_steps;
  long set_in_cycle;
  Vector target_pos;
  ANGLE target_dir;
  Value target_vel;
  bool kick_to_pos;
  bool calc_done;

  MyState fake_state;
  long fake_state_time; 
  void get_ws_state(MyState &state);

  MyState get_cur_state();
  
  bool calculate(const MyState &state,Value vel,const ANGLE &dir,const Vector &pos,bool to_pos,
		 Cmd_Main &res_cmd1,Value &res_vel1,Cmd_Main &res_cmd2,Value &res_vel2,
		 bool &need_2steps); 
  bool do_calc();

 public:

  /** This makes it possible to "fake" WS information.
      This must be called _BEFORE_ any of the kick functions, and is valid for 
      the current cycle only.
  */
  void set_state(const Vector &mypos,const Vector &myvel,const ANGLE &myang,
		 const Vector &ballpos,const Vector &ballvel, 
		 const Vector &op_pos = Vector(1000,1000), 
		 const ANGLE &op_bodydir = ANGLE(0),
		 const int op_bodydir_age = 1000);

  void set_state( const AState & state );
  
  /** Resets the current state to that found in WS.
      This must be called _BEFORE_ any of the kick functions.
  */
  void reset_state();
  
  void kick_in_dir_with_initial_vel(Value vel,const ANGLE &dir);
  void kick_in_dir_with_max_vel(const ANGLE &dir);
  void kick_to_pos_with_initial_vel(Value vel,const Vector &point);
  void kick_to_pos_with_final_vel(Value vel,const Vector &point);
  void kick_to_pos_with_max_vel(const Vector &point);

  /** false is returned if we do not reach our desired vel within two cycles.
     Note that velocities are set to zero if the resulting pos is not ok,
     meaning that even if a cmd would reach the desired vel, we will ignore
     it if the resulting pos is not ok.
  */
  bool get_vel(Value &vel_1step,Value &vel_2step);
  bool get_cmd(Cmd &cmd_1step,Cmd &cmd_2step);
  bool get_vel(Value &best_vel);    // get best possible vel (1 or 2 step)
  bool get_cmd(Cmd &best_cmd);      // get best possible cmd (1 or 2 step)

  // returns 0 if kick is not possible, 1 if kick in 1 step is possible, 2 if in 2 steps. probably modifies vel
  int is_kick_possible(Value &speed,const ANGLE &dir);

  bool need_two_steps();

  bool  can_keep_ball_in_kickrange();
    
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    if(OneStepKick::init(conf_file,argc,argv)) {
      cout << "\nOneOrTwoStepKick behavior initialized.";
    } else { 
      ERROR_OUT << "\nCould not initialize OneStepKick behavior - stop loading.";
      exit(1);
    }
    return true;
  }

  OneOrTwoStepKick() {
    set_in_cycle = -1;
    onestepkick = new OneStepKick();
    onestepkick->set_log(false); // we don't want OneStepKick-Info in our logs!
  }
  virtual ~OneOrTwoStepKick() {
    delete onestepkick;
  }
};
    

#endif
