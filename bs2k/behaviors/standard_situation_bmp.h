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

#ifndef _STANDARD_SITUATION_BMP_H_
#define _STANDARD_SITUATION_BMP_H_

#include "base_bm.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/neuro_kick_bms.h"
#include "wball_demo_bmp.h"
#include "skills/face_ball_bms.h"
#include "wball_demo_bmp.h"
#include "noball_demo_bmp.h"


// "Wildcard"-IDs for all situations where we or opponent has the ball
//const int PMS_my_Kick  = PM_MAX+1;
//const int PMS_his_Kick  = PM_MAX+2;
const int PMS_my_Kick  = 30;
const int PMS_his_Kick  = 31;

// Struct to hold all information for one specific situation.
// It contains mostly static information and is filled at program start
// with information from the *.conf file
// Exceptions are the effective_xx variables which are updated during game
struct SituationData {

  // position info for one player
  struct Position {
    Vector target_pos;  // targetposition for player
    bool is_target_abs; // 0=relative to ball ,1=absolute coordinates
    int target_player;  // 2 - 11= number of player, 0=arbitrarily,
    
    Vector effective_position; // target position with consideration of ballpos
    int effective_player;      // -1 = no player assigned yet, 0=error (not initialized), otherwise playernumber
  };

  int  playmode;  // playmode (PM_x or PMS_my_Kick,PMS_hisKick
  char name[50];  // name of this situation
  int  number;    // unique id
  
  int cnt_positions;  // how many positions are stored (including default position for startplayer)
  Vector reference_position; // ball position must be near this so that this position-struct will be chosen
  Value reference_radius;    // do not choose this if ball is further away from reference_position than radius
    
  // if we have initiative, ball will (usually) be kicked by startPlayer when time%startModTime==0
  bool we_have_ball;              // for this position we have ball
  int start_wait_time;            // default time to wait before kicking the ball
  int start_mod_time;             // modulo of time, when ball will be kicked: to coordinate players
  int start_player;               // who kicks the ball; not used if we do not have the ball
  int effective_start_wait_time;  // actual time to wait; depending on stamina
  
  // target data in struct Position assumes ball is in upper half of field (ballpos.y > 0).
  // if not, target player numbers must be mirrored at horizontal horizon.
  bool mirror_vertically;

  // goalie always uses his own behavior, so only data for 10 players
  Position pos[10];
  int assigned_pos_to_player[10]; // playernumber i = pos[i-2]
};

class StandardSituation: public BaseBehavior
{
  // global behavior parameters
  static bool initialized;
  static bool use_standard_situations;

  #define MAX_SITUATIONS 20
  static SituationData situation[MAX_SITUATIONS];
  static int cnt_situations;

  static int active_situation;
  
  #define situation_hysterese_value 2
  static int default_start_wait_time;
  static int default_start_mod_time;

  static int time_last_update;

  static Value homepos_tolerance;
  static int homepos_stamina_min;

  static Vector StandardSituation::kick_off_target;
  static Value  StandardSituation::kick_off_speed;

  static Value clearance_radius_min;
  static Value preference_exact_sit;
  static Value go_to_kick_tolerance;
  static Value go_to_kick_border_tolerance;
  static Value kick_border_setoff;
  
  static int pass_announce_time;

  NeuroGo2Pos * go2pos;
  NeuroIntercept * neuroIntercept;
  NeuroKick * neuroKick;
  WballDemo *wball_demo;
  NoballDemo *noball_demo;
  FaceBall *faceBall;
  WballDemo *playPass;
  
  // to be able to continue 1-2-step kicks 
  BaseBehavior * last_behavior;
  Value last_kick_speed;         
  Vector last_kick_target;
  Intention last_intention;
  
  // prevent unsuccessful kicking, e.g. caused by bad localization
  int kicks_in_a_row;
  static int max_kicks_in_a_row;
  static int max_ball_age_when_kick;

  static int time_replace_missing_start_player;
  static double dist_factor_replace_missing_start_player;
  
  static Value defenders_safety_x_factor;
  
public:
  static bool init(char const * conf_file, int argc, char const* const* argv);
  StandardSituation();
  bool get_cmd(Cmd & cmd);

protected:
  // active situation handling: recognize special playmodes und keep up-to-date
  void init_new_active_situation();
  void update_active_situation();
  int find_best_situation_data();
  SituationData* get_active_situation();
  
  void update_effective_target_positions(SituationData* sit);
  Vector refine_target_position(Vector target, SituationData* sit);
  void match_players_to_positions(SituationData* sit);
  int get_mirror_player(int number);
  static bool do_we_have_ball(int playmode);
  void update_ball_pos();
  void write2blackboard();

  void update_time_left(SituationData* sit);
  void panic_kick(Cmd & cmd, SituationData* sit);
  void set_wait_time(SituationData* sit);
  void idle_behavior(Cmd & cmd);
  bool go_to_pos(Cmd & cmd, Vector target);
  
  bool is_start_player_missing();
  
  bool behave_start_player(Cmd & cmd, SituationData* sit);
  bool behave_non_start_player(Cmd & cmd, SituationData* sit);

  bool continue_playon(Cmd & cmd);

  // specify the actual situation
  int act_sit_starttime; // time when special playmode began
  int act_sit_playmode;
  int act_sit_lastupdate;
  int act_sit_duration;
  Vector act_sit_ballpos;
  int act_sit_tried_passes;
  
  Vector homepos;
  int time_left;
  int mynumber;

  bool we_have_ball;
};


#endif
