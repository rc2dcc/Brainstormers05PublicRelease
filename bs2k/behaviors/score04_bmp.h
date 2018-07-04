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

#ifndef _SCORE04_BMP_H_
#define _SCORE04_BMP_H_

/*
Johannes Knabe, 2004
This behaviour will do for the last meters before opponents goal

*/

#include "jkstate.h"
#include "base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"
//#include "../n++/src/n++.h"
#include "jk_wball03_bmp.h"
#include "skills/basic_cmd_bms.h"
#include "skills/face_ball_bms.h"
#include "skills/dribble_straight_bms.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/one_step_kick_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/selfpass_bms.h"
#include "skills/neuro_kick05_bms.h"
#include "skills/score_bms.h"
//#include "blackboard.h"
#include "intention.h"

class Score04 : public BaseBehavior {

int score04TriedCounter;
int score04ExecutedCounter;


  static bool initialized;
  static Net * net;
  Vector last_good_pos;

  BasicCmd *basiccmd;
  NeuroGo2Pos *go2pos;
  FaceBall *faceball;
  OneStepKick *onestepkick;
  OneOrTwoStepKick *onetwokick;
  NeuroKick05 *neurokick;
  NeuroIntercept *intercept;
  Score *score;
  JKWball03 *jkwball03;
  DribbleStraight *dribblestraight;
  Selfpass *selfpass;

  long last_called;
  long seq_started;
  long play_on_cnt;
  long sequence_number;
  int last_action_type;
  long last_action_time;
  bool last_action_pass;
  int last_action_pass_target_num;
  long last_action_pass_time;

  bool get_player_cmd(Cmd &cmd);

  float get_V_for_State(jkState state);

  bool scan_field(Cmd &cmd);

  bool in_kickrange(Vector player_pos,Vector ball_pos);

  bool is_loose_situation(jkState state);

  bool default_model(Cmd &sim,jkState state,jkState &resulting_state);

  //simulate playing pass to target (/to teammate), returns number of steps used
  //until catched by teammate (using intercept target) or an error value otherwise
  //(since recursive: ball_shot to signal that the ball was kicked already; set initially to zero)
  int model_pass(jkState state,jkState &resulting_state,Vector target,int pass_shot,int count,int player);

  int model_selfpass(jkState state,jkState &resulting_state,Value speed,Vector target, ANGLE ang);

  bool model_turn_and_dash(Cmd &cmd,jkState state);

  //the most simple way: no prediction at all, just continue with current speed and vel
  void model_continue(Vector pos,Vector vel,Vector &new_pos,Vector &new_vel);

  void model_opp_continue(jkState state,int opp_num,Vector &new_my_pos,Vector &new_my_vel);

  void model_friend_continue(jkState state,int friend_num,Vector &new_my_pos,Vector &new_my_vel);


  void model_goalie(Vector my_pos,Vector my_vel,ANGLE my_ang,Vector ball_pos,Vector ball_vel,
                                              Vector &new_my_pos,Vector &new_my_vel, ANGLE &new_my_ang);

  //models somebodys try to intercept the ball - returns true if ball gets into the kickrange of this somebody
  bool model_intercept(Vector my_pos,Vector my_vel,ANGLE my_ang,Vector ball_pos,Vector ball_vel,
           Vector &new_my_pos,Vector &new_my_vel,ANGLE &new_my_ang,Vector &new_ball_pos,Vector &new_ball_vel);

  //given an action and a (current) state this predicts the expected state
  //after applying the action - even if it takes some cycles; returns number of cycles needed for the action
  int predict_outcome(jkState state,Score04Action* actions,int which,jkState &resulting_state);

  //selects the action with the expected highest payoff from array
  int select_action(Score04Action* actions, int num, jkState &state, float & bestval);

  //maps the selected action to an intention
  bool apply_action(Score04Action &do_it,Intention &intention);


bool Score04::protocol();
void Score04::memorize(int action);

 public:
  bool get_cmd(Cmd &cmd);
  void reset_intention(bool count=true);
  static bool init(char const * conf_file, int argc, char const* const* argv);
  int Score04::exit_num();

  Score04();
  virtual ~Score04() {
    delete go2pos; delete basiccmd; delete onetwokick; delete faceball; delete selfpass;
    delete intercept; delete onestepkick; delete score; delete jkwball03;   delete dribblestraight;
  }
};

#endif
