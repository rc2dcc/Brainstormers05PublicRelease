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

#ifndef _ONE_STEP_KICK_BMS_H_
#define _ONE_STEP_KICK_BMS_H_

/* This behavior deals with kicking to a given point or in a given
   direction within one cycle (or step). It is kind of a basic move.
   In addition to the usual kick functions, there are also some static
   functions to retrieve some basic information.

   This behavior usually takes the current player position as reference point,
   but you can override this by calling set_state() prior to any other function.
   This makes it possible to "fake" the player's position during the current cycle.
   Use reset_state to re-read the WS information, or wait until the next cycle.

   Note that kick_to_pos_with_final_vel() is rather unprecise concerning the
   final velocity of the ball. I don't know how to calculate the needed starting
   vel precisely, so I have taken the formula from the original Neuro_Kick2 move
   (which was even more unprecise...) and tweaked it a bit, but it is still
   not perfect.

   You can check if a given ball pos is valid using the static function is_pos_ok().
   This checks if the ball collides with the player itself (note that this collision
   check, as opposed to that of the original move, is working - the original move
   ignored the player's vel...) or if the ball pos is near the kickrange of an
   opponent. Note that only opponents near the player's pos are checked to save
   computing time, so don't use this function for a general purpose test...
   Nevertheless, we check _all_ opponents near enough that the ball could reach them
   after a single kick. The original move only checked the closest player. 

   The player's velocity was also ignored in the original can_keep_ball_in_kickrange().
   This has been fixed in this behavior as well.
   
   (w) 2002 Manuel Nickschas
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
#include "mystate.h"

class OneStepKick : public BaseBehavior {
  static bool initialized;
 public:
#if 0
  struct State {
    Vector my_pos;
    Vector my_vel;
    ANGLE my_ang;
    Vector ball_pos;
    Vector ball_vel;
  };
#endif
 private:
  struct CritOpp {
    Vector pos;
    Value radius;
    PPlayer player;
  };
  
  Cmd_Main result_cmd;
  Value result_vel;
  bool result_status;

  Vector target_pos;
  ANGLE target_dir;
  Value target_vel;

  bool kick_to_pos;
  bool calc_done;
  long set_in_cycle;

  bool do_not_log;

  MyState fake_state;
  long fake_state_time;
  
  CritOpp crit_opp[NUM_PLAYERS];
  int crit_opp_num;
  long crit_opp_time;
  Vector crit_opp_basepos;

  static CritOpp mypos_crit_opp[NUM_PLAYERS];
  static int mypos_crit_opp_num;
  static long mypos_crit_opp_time;
  
  bool calculate(const MyState&,Value vel,const ANGLE &dir,Cmd_Main &res_cmd,Value &res_vel);

  void get_ws_state(MyState &);
  MyState get_cur_state();
  
  Value get_kick_decay(const MyState&);
  Value get_max_acc_vel(const MyState&);
  Value get_max_acc_vel(Value decay);
  void kick_vector_to_pwr_and_dir(const MyState &state,const Vector &kick_vec,
				  Value &res_power, ANGLE &res_dir);

  void compute_critical_opponents(const MyState &state);

  /* too complicated to make static functions, so use these and a dummy object... */
  static OneStepKick *onestepkick;
  bool  nonstatic_can_keep_ball_in_kickrange();
  Value nonstatic_get_max_vel_to_pos(const Vector &pos,bool check_pos=true);
  Value nonstatic_get_max_vel_in_dir(const ANGLE &dir,bool check_pos=true);
  
 public:
  /** This makes it possible to "fake" WS information.
      This must be called _BEFORE_ any of the kick functions, and is valid for 
      the current cycle only.
  */
  void set_state(const Vector &mypos,const Vector &myvel,const ANGLE &myang,
		 const Vector &ballpos,const Vector &ballvel,		 
		 const Vector &op_pos, 
		 const ANGLE &op_bodydir,
		 const int op_bodydir_age,
		 const PPlayer op = 0);
  
  /** Resets the current state to that found in WS.
      This must be called _BEFORE_ any of the kick functions.
  */
  void reset_state();

  Value get_max_vel_in_dir(const MyState&,const ANGLE&);
  void kick_in_dir_with_initial_vel(Value vel,const ANGLE &dir);
  void kick_in_dir_with_max_vel(const ANGLE &dir);
  void kick_to_pos_with_initial_vel(Value vel,const Vector &point);
  void kick_to_pos_with_final_vel(Value vel,const Vector &point);
  void kick_to_pos_with_max_vel(const Vector &point);

  bool  get_cmd(Cmd &cmd);
  bool  get_vel(Value &vel);      
  Value get_vel(); // for convenience only, returns 0 if there were problems

  bool is_pos_ok(const Vector &ballpos); // nonstatic, uses current state...
  bool is_pos_ok(const MyState&,const Vector &ballpos);
  
  /* checkpos == true -> result is 0 if resulting pos is invalid */ 
  static Value get_max_vel_to_pos(const Vector &pos,bool check_pos=true); // does not set kick command!
  static Value get_max_vel_in_dir(const ANGLE &dir,bool check_pos=true);  // does not set kick command!
  static bool  can_keep_ball_in_kickrange();
  static bool  is_pos_ok(const Vector &mypos,const Vector &ballpos);

  void set_log(bool do_log = true) {do_not_log=!do_log;} // set_log(false) disables all logging
  
  OneStepKick() {
    set_in_cycle = -1; crit_opp_time = -1;
    fake_state_time = -1;
    do_not_log=false;
  }
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    onestepkick = new OneStepKick(); // init static dummy object
    cout << "\nOneStepKick behavior initialized.";
    return true;
  }  
};

#endif
