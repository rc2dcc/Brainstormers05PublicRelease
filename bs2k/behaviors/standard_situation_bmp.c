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

#include "standard_situation_bmp.h"

#include "ws_info.h"
#include "log_macros.h"
#include "tools.h"
#include "blackboard.h"
#include "../policy/positioning.h"

#define LOG_LEVEL 0

// Static variables
bool StandardSituation::initialized = false;
bool StandardSituation::use_standard_situations = true;

SituationData StandardSituation::situation[MAX_SITUATIONS];
int    StandardSituation::cnt_situations;
int    StandardSituation::active_situation;

int    StandardSituation::default_start_wait_time = 30;
int    StandardSituation::default_start_mod_time = 10;
int    StandardSituation::time_last_update = -1;

Value  StandardSituation::homepos_tolerance = 2;
int    StandardSituation::homepos_stamina_min = 2000;

Vector StandardSituation::kick_off_target = Vector(-13.0, 20.0);
Value  StandardSituation::kick_off_speed = 2.3;

Value  StandardSituation::clearance_radius_min = 10.0;
Value  StandardSituation::preference_exact_sit = 2;
Value  StandardSituation::go_to_kick_tolerance  = 1.0;
Value  StandardSituation::go_to_kick_border_tolerance  = 0.3;
Value  StandardSituation::kick_border_setoff = 0.65;

int    StandardSituation::pass_announce_time = 3;
int    StandardSituation::max_kicks_in_a_row = 6;
int    StandardSituation::max_ball_age_when_kick = 4;

int    StandardSituation::time_replace_missing_start_player = 60;
double StandardSituation::dist_factor_replace_missing_start_player = 1.0;

Value  StandardSituation::defenders_safety_x_factor = 0.3;

void DEBUG_VECTOR(double x, double y, double radius, const char* color) {
  LOG_DEB(LOG_LEVEL, << _2D << L2D(WSinfo::me->pos.x, WSinfo::me->pos.y, x, y, color) );
  LOG_DEB(LOG_LEVEL, << _2D << C2D(x, y, radius, color) );
}

int my_round(double x) { return (x>0) ? (int)(x+0.5) : (int)(x-0.5); }


StandardSituation::StandardSituation() { 
  go2pos= new NeuroGo2Pos; 
  neuroIntercept = new NeuroIntercept;
  neuroKick = new NeuroKick;
  wball_demo = new WballDemo;
  noball_demo = new NoballDemo;
  faceBall = new FaceBall;
  playPass = new WballDemo;
  last_behavior= 0;
  last_kick_speed = 0;
  kicks_in_a_row = 0;
}

bool StandardSituation::init(char const * conf_file, int argc, char const* const* argv)
{
  if ( initialized ) return true;
  initialized = true;

  time_last_update = -1;
  active_situation = -1;

  bool result = true;
  cnt_situations = 0;
  char s[50];
  Value v_input[4];

  // init all other used behaviors
  result &= NeuroGo2Pos::init(conf_file,argc,argv);
  result &= NeuroIntercept::init(conf_file,argc,argv);
  result &= NeuroKick::init(conf_file,argc,argv);
  result &= WballDemo::init(conf_file,argc,argv);
  result &= FaceBall::init(conf_file,argc,argv);
  result &= WballDemo::init(conf_file,argc,argv);

  // read in parameters from config file  
  ValueParser vp(conf_file,"StandardSituation");

  if (argc > 1) {
    argv++;argc--;
    vp.append_from_command_line(argc,argv,"StandardSituation");
  }

  vp.get("use_standard_situations", use_standard_situations);
  vp.get("default_start_wait_time", default_start_wait_time);
  vp.get("default_start_mod_time", default_start_mod_time);
  
  sprintf(s, "Situ_%d", cnt_situations);
  while (vp.get(s, situation[cnt_situations].name, 50)>0) {
    ValueParser vp2(conf_file, situation[cnt_situations].name);
	  situation[cnt_situations].start_wait_time = default_start_wait_time;
	  situation[cnt_situations].start_mod_time = default_start_mod_time;
    vp2.get("playmode", situation[cnt_situations].playmode);
    vp2.get("start_player", situation[cnt_situations].start_player);
    vp2.get("start_mod_time", situation[cnt_situations].start_mod_time);
    vp2.get("start_wait_time", situation[cnt_situations].start_wait_time);

    v_input[0] = 0;
    v_input[1] = 0;
    v_input[2] = 1000;
    vp2.get("reference_position", v_input, 3);
    situation[cnt_situations].reference_position.x = v_input[0];
    situation[cnt_situations].reference_position.y = v_input[1];
    situation[cnt_situations].reference_radius = v_input[2];

    int playmode = situation[cnt_situations].playmode;
    
    situation[cnt_situations].cnt_positions = 0;
    int i=0, fileidx= 0;
    
    situation[cnt_situations].we_have_ball = do_we_have_ball(playmode);

    if (situation[cnt_situations].we_have_ball) {
      // automatically insert one position at the ball for start_player
      situation[cnt_situations].pos[i].target_pos = Vector(0,0);
      situation[cnt_situations].pos[i].is_target_abs = false;
      situation[cnt_situations].pos[i].target_player = situation[cnt_situations].start_player;
      situation[cnt_situations].cnt_positions++;
      i++;
    }
    
    for( ;i<=10; i++, fileidx++) { // max. 10 positions, goalie does his own positioning
      sprintf(s, "pos_%d",fileidx);
			memset(v_input, 0, 4*sizeof(double));
      if (vp2.get(s, v_input, 4) > 0)
        situation[cnt_situations].cnt_positions++;
      situation[cnt_situations].pos[i].target_pos = Vector(v_input[0], v_input[1]);
      situation[cnt_situations].pos[i].is_target_abs = (v_input[2]<0.5) ? 0 : 1;
      situation[cnt_situations].pos[i].target_player = my_round(v_input[3]);
    }
    cnt_situations++;
    sprintf(s, "Situ_%d", cnt_situations);
  }

	/*
	// debug output for one situation (i)
  int i=1;
  cout << "\n ---------- StandardSituation init -------------------------";
  cout << "\n sit(" << i << ").name = " << situation[i].name;
  cout << "\n sit.[0] = " << situation[i].pos[0].target_pos << "  " << situation[i].pos[0].is_target_abs 
       << "  " << situation[i].pos[0].target_player;
  cout << "\n sit.playmode = " << situation[i].playmode;
  cout << "\n sit.start_player = " << situation[i].start_player;
  cout << "\n sit.start_mod_time = " << situation[i].start_mod_time;
  cout << "\n sit.reference_pos = " << situation[i].reference_position;
  cout << "\n sit.cnt_pos = " << situation[i].cnt_positions;
  cout << "\n--------------------------------------------------------";
	*/

  return result;
}

// Playmode change was detected. Prepare specialized positioning of players
void StandardSituation::init_new_active_situation()
{  
  LOG_DEB(LOG_LEVEL, <<"Standardsit: init_new_active_sit:  time="<<WSinfo::ws->time<<"  ,mode=" << WSinfo::ws->play_mode);
  act_sit_starttime = WSinfo::ws->time;
  act_sit_playmode  = WSinfo::ws->play_mode;
  act_sit_ballpos   = WSinfo::ws->ball.pos;
  act_sit_tried_passes = 0;
  act_sit_duration  = 0;
  
  active_situation = -1;

  we_have_ball = do_we_have_ball(act_sit_playmode);
  last_kick_speed = 0;
  last_behavior = 0;  
  
  kicks_in_a_row = 0;
/*
  // debug output
  if (WSinfo::ws->play_mode != PM_PlayOn)
    cout << "\n Player " << WSinfo::me->number << ": new situation ("
      << active_situation << ") !! (acball " << act_sit_ballpos <<")" << flush;
*/
}

// Updates effective positions and matching to players
void StandardSituation::update_active_situation()
{
  LOG_DEB(LOG_LEVEL, <<"Standardsit: update_active_sit");
  act_sit_lastupdate = WSinfo::ws->time;
 
  // find data-struct which best matches the current game situation
  // i.e. playmode must match, and reference position closer to ball
  // than for all other structs
  active_situation = find_best_situation_data();

  // if anything to do at all..
  SituationData* sit = get_active_situation();
  if (!sit) return; 

  act_sit_ballpos   = WSinfo::ws->ball.pos;
  sit->mirror_vertically = (act_sit_ballpos.y < 0);

  update_time_left(sit);
  update_effective_target_positions(sit);
  match_players_to_positions(sit);
  set_wait_time(sit);
}

// Returns number of data-struct which best corresponds to the actual game situation, or -1
// if no appropriate data-struct is found.
// Best correspondence is defined by 1) usable for the actual playmode and 2) the structs
// reference-position is nearest to the ball.
int StandardSituation::find_best_situation_data()
{
  int datanumber = -1;
  Value best_dist = 1e+10;
  Vector ball_pos = act_sit_ballpos;
  if (ball_pos.y < 0) ball_pos.y *= -1; // norm to upper half of field

  for (int i=0; i<cnt_situations; i++) {
    if (situation[i].playmode == act_sit_playmode
      || situation[i].playmode == PMS_my_Kick && do_we_have_ball(act_sit_playmode)
      || situation[i].playmode == PMS_his_Kick && !do_we_have_ball(act_sit_playmode))
    {
      Value dist = situation[i].reference_position.sqr_distance(ball_pos);
      
      if (dist > situation[i].reference_radius * situation[i].reference_radius)
        dist = 1e+10;

      // use hysterese to prevent switching between situations
      if (i == active_situation) dist -= situation_hysterese_value;

      //prefer situations with excatly same playmode
      if (situation[i].playmode == act_sit_playmode) dist -= preference_exact_sit;
      
      if (dist < best_dist) {
	best_dist = dist;
	datanumber = i;  
      }
    }
  }

  return datanumber;
}

// Update effective target positions, by taking absolute oder resolving relative
// position to ball. Also mirrors on x-axis if necessary.
void StandardSituation::update_effective_target_positions(SituationData* sit)
{
  Vector ball_pos = act_sit_ballpos;
  Vector eff;
  if (sit->mirror_vertically) ball_pos.y *= -1; // norm to upper half of field

  for (int i=0; i<sit->cnt_positions; i++) {
    if (sit->pos[i].is_target_abs) // absolute or relative to ball?
      eff = sit->pos[i].target_pos;
    else
      eff = ball_pos + sit->pos[i].target_pos;
      
    if (sit->mirror_vertically) // positions always related to ball in upper half of field
      eff.y *= -1;    
         
    if (i>0 && sit->we_have_ball) {
      // check alternate position if passes not possible
      Vector delta, tmp = eff - act_sit_ballpos;
      if ((act_sit_tried_passes == 8 && time_left <= 5) || act_sit_tried_passes >= 9) {
         delta.x = -0.3*tmp.x;
         delta.y = -0.3*tmp.y;
	 eff += delta;
      } else if ((act_sit_tried_passes == 5 && time_left <= 5) || act_sit_tried_passes >= 6) {
         delta.x = -tmp.y;
         delta.y = tmp.x;
	 eff += delta;
      } else if ((act_sit_tried_passes == 3 && time_left <= 5) || act_sit_tried_passes >= 4) {
        delta.x = tmp.y;
        delta.y = -tmp.x;
        eff += delta;
      }

      // don't run into offside or outside playfield if we have ball
      // (but not for the kicking player.. i>0 !)
      eff.x = Tools::min(eff.x, WSinfo::his_team_pos_of_offside_line() -2);
      eff.x = Tools::min(eff.x, 52.0);
      eff.x = Tools::max(eff.x, -52.0);
      eff.y = Tools::min(eff.y, 30.0);
      eff.y = Tools::max(eff.y, -30.0);
    }

    sit->pos[i].effective_position = eff;
  }
}

// locally refine given target positions, only for players not kicking the ball
Vector StandardSituation::refine_target_position(Vector target, SituationData* sit)
{
  Vector target_ball = target - act_sit_ballpos;
  double target_ball_dist = target_ball.norm();

  // don't confuse kicking player
  if (target_ball_dist < 4.0) {
    target = target + 6.0 / Tools::max(target_ball_dist, 0.1) * target_ball;
    LOG_DEB(LOG_LEVEL, "Standardsit: refine_target: too close to ball! (dist="<<target_ball_dist<<")-->set to 6.0");  
  }
  
  // defenders go way too far: keep them back
  if (DeltaPositioning::get_role(mynumber) == 0) { //am i defender?
    target.x = Tools::min(target.y, 
      defenders_safety_x_factor*-FIELD_BORDER_X + (1.0-defenders_safety_x_factor)*act_sit_ballpos.x);
  }
  
  return target;
}


// Match effective positions in sit to players #2-#11 (sit must not be 0).
// All positions will be matched (if not more than 10 positions exist).
// Also sets start_player if necessary and not already done.
void StandardSituation::match_players_to_positions(SituationData* sit)
{   
  int i;

  // reset mapping from players to positions
  for (i=0; i<10; i++) sit->assigned_pos_to_player[i] = -1;
  for (i=0; i<sit->cnt_positions; i++) sit->pos[i].effective_player = -1;
  
  // get positions for fixed player numbers
  int number;
  for (i=0; i<sit->cnt_positions; i++) {
    number = sit->pos[i].target_player;
    if (sit->mirror_vertically) number = get_mirror_player(number);
    if (number > 0 && sit->assigned_pos_to_player[number-2] == -1) {
      sit->assigned_pos_to_player[number-2] = i;
      sit->pos[i].effective_player = number;
    }
  }
  
  if (sit->we_have_ball) {
    // first position was set at the ball --> player to kick
    sit->start_player = sit->pos[0].effective_player;    
  }
}



// Returns number of player who plays on the vertically mirrored position
// Returns 0 if given number is invalid
int StandardSituation::get_mirror_player(int number)
{
  if (number < 1 || number > 11) return 0; // invalid number

  static int mirror_number_4_3_3[11] = {1, 5, 4, 3,  2, 8, 7, 6,  11, 10, 9};
  return mirror_number_4_3_3[number-1];
}

bool StandardSituation::do_we_have_ball(int playmode)
{
  return  ( playmode == PM_my_KickIn     || playmode == PM_my_FreeKick
	|| playmode == PM_my_CornerKick || playmode == PM_my_OffSideKick 
	|| playmode == PM_my_KickOff
	|| playmode == PMS_my_Kick);
}



// Set the effective start_wait_time for sit, depending on players stamina
// and general game state (goals).
void StandardSituation::set_wait_time(SituationData* sit)
{
  int wait_time = sit->start_wait_time;
  
  int stamina_regen = int(4000 - WSinfo::me->stamina) / 40; // 40 as compromise
	if (wait_time < stamina_regen) 
		wait_time = stamina_regen;

  int goal_diff = WSinfo::ws->my_team_score - WSinfo::ws->his_team_score;
  if (goal_diff > 0 && goal_diff < 3) wait_time += 30*goal_diff;
  if (goal_diff < 0) wait_time += 20*goal_diff;
		
  if (wait_time < 0) wait_time = 0;
  if (wait_time > 150) wait_time = 150;
  sit->effective_start_wait_time = wait_time;
}

// set value of time_left, i.e. estimated time until ball will be kicked
void StandardSituation::update_time_left(SituationData* sit)
{
  if (!sit) {
    time_left = 200 - act_sit_duration;
  } else {
    int kick_time = act_sit_starttime + sit->effective_start_wait_time;
    if (kick_time < WSinfo::ws->time) kick_time = WSinfo::ws->time;
    kick_time += situation[active_situation].start_mod_time-1 - 
      (kick_time-1) % situation[active_situation].start_mod_time;
    time_left = kick_time - WSinfo::ws->time;
  }
}

// sets the internal used ball position and evtl. corrects it using playmode information
void StandardSituation::update_ball_pos()
{
  act_sit_ballpos = WSinfo::ball->pos;
  
  if (act_sit_playmode == PM_my_KickOff || act_sit_playmode == PM_his_KickOff)
    act_sit_ballpos = Vector(0, 0);
    
  if (act_sit_playmode == PM_my_CornerKick)
    act_sit_ballpos.x = FIELD_BORDER_X - 1.0;
    
  if (act_sit_playmode == PM_his_CornerKick)
    act_sit_ballpos.x = -(FIELD_BORDER_X  - 1.0);
    
  if (act_sit_playmode == PM_my_KickIn || act_sit_playmode == PM_his_KickIn) {
    if (act_sit_ballpos.y > FIELD_BORDER_Y - 2.0)
      act_sit_ballpos.y = FIELD_BORDER_Y;
    if (act_sit_ballpos.y < -FIELD_BORDER_Y + 2.0)
      act_sit_ballpos.y = -FIELD_BORDER_Y;  
  }
}

// returns pointer to the currently used Sit.data structure,
// or 0 if no such is used at the moment
SituationData* StandardSituation::get_active_situation()
{
  if (active_situation >= 0 && active_situation < cnt_situations)
    return &situation[active_situation];
  else
    return 0;
}

bool StandardSituation::is_start_player_missing()
{
  WSpset players_near_ball = WSinfo::valid_teammates_without_me;
  players_near_ball.keep_players_in_circle(act_sit_ballpos, 3.0);
  return (players_near_ball.num == 0);
}


// do something useful if nothing special needs to be done
void StandardSituation::idle_behavior(Cmd & cmd)
{
  LOG_DEB(LOG_LEVEL, "Standardsit: idle_behave:  ballvalid=" << WSinfo::is_ball_pos_valid());
  // search ball if its position not well known
  if (WSinfo::ball->age >= act_sit_duration || !WSinfo::is_ball_pos_valid()) {
    // look for ball if not recently saw it
	// remember if playmode changed, ballposition might have drastically changed
    faceBall->turn_to_ball();
    faceBall->get_cmd(cmd);
  }  else {
    // go to home position
    go2pos->set_target(homepos); // homepos must be set before
    go2pos->get_cmd(cmd);
  }
}

// try to kick at all circumstances, even if bad kick results
void StandardSituation::panic_kick(Cmd & cmd, SituationData* sit)
{
  // must see ball, otherwise 2-step-kicks etc. will not function correctly
  if (WSinfo::ball->age > max_ball_age_when_kick) {
    LOG_DEB(LOG_LEVEL, "Standardsit: PANIC-kick: faceBall.");
    kicks_in_a_row = 0;
    faceBall->turn_to_ball();
    faceBall->get_cmd(cmd);
    return;
  }
  
  // continue started kicks
    if (playPass->get_cmd(cmd)) {
    kicks_in_a_row++;
    Intention intention;
    if (playPass->get_intention(intention)) {
      intention.get_kick_info(last_kick_speed, last_kick_target);
      last_intention = intention;
    }
    last_behavior= playPass;
    LOG_DEB(LOG_LEVEL, "Standardsit: PANIC-kick: pass played.");
    return;
  }

  // determine where to shoot
  Vector target;
  if (sit->cnt_positions > 1)
    target = sit->pos[1].effective_position;
  else
    target = Vector(50, 0); // just shoot at goal
  
  // kick the ball if possible 
  neuroKick->reset_state();
  neuroKick->kick_to_pos_with_max_vel(target);
  if (WSinfo::ball->age <= max_ball_age_when_kick && neuroKick->get_cmd(cmd)) {
    LOG_DEB(LOG_LEVEL, "Standardsit: PANIC-kick: neuroKick.");
    kicks_in_a_row++;
    last_behavior= neuroKick;
    last_kick_target = target;
    last_kick_speed = ServerOptions::ball_speed_max;
    Intention intention;
    intention.set_panic_kick(target, WSinfo::ws->time);
    last_intention = intention;
    return;
  }

  // go to ball if still not reached it
  LOG_DEB(LOG_LEVEL, "Standardsit: PANIC-kick: interceptBall.");
  kicks_in_a_row = 0;
  neuroIntercept->get_cmd(cmd);
}


// uses go2pos-behavior, with additional stamina-management and 
// avoidance of clearance zone (if opponent has freekick e.g.)
// returns true if cmd is set
bool StandardSituation::go_to_pos(Cmd & cmd, Vector target)
{
  Vector to_target = target - WSinfo::me->pos;
  double dist = to_target.norm();

  if (dist < homepos_tolerance || 
    (we_have_ball == false && WSinfo::me->stamina < homepos_stamina_min)) {
    LOG_DEB(LOG_LEVEL, "Standardsit: go_to_pos: near target (d="<<dist<<") or low stamina (="<<WSinfo::me->stamina<<")--> stay here.");	    return false;
  }

  if (we_have_ball) { // if so, just call go2pos
    go2pos->set_target(target);
    return go2pos->get_cmd(cmd);
  }

  // check if direct line to target crosses forbidden area
  Vector unit_to_target =  (1.0/dist) * to_target;  // dist > homepos_tolerance > 0
  Vector normal_to_target(-unit_to_target.y, unit_to_target.x);
  Vector to_ball = WSinfo::ball->pos - WSinfo::me->pos;
  Value dist_ball = to_ball.norm();
  Value s = to_ball.x * unit_to_target.x + to_ball.y * unit_to_target.y;
  Value t = to_ball.x * normal_to_target.x + to_ball.y * normal_to_target.y;

  Vector v = WSinfo::me->pos + t*unit_to_target;
  Vector w = v - to_ball;

  Vector normal_to_ball;
  if (t > 0)
    normal_to_ball = Vector(to_ball.y, -to_ball.x);
  else
    normal_to_ball = Vector(-to_ball.y, to_ball.x);
  normal_to_ball.normalize();

  LOG_DEB(LOG_LEVEL, "StandardSit: go_to_pos: me="<<WSinfo::me->pos<<", target="<<target<<", ball="<<WSinfo::ball->pos<<", dist="<<dist<<", s="<<s<<", t="<<t<<", v="<<v<<", w="<<w);

  if ((target - WSinfo::ball->pos).norm() < clearance_radius_min) {
    //target must not be in forbbiden area around ball
    //but approach, if player still far away from clearance area
    if (dist_ball <= clearance_radius_min + 2) { // + 2 as safety
      target = WSinfo::me->pos;
      LOG_DEB(LOG_LEVEL, "Standardsit: go_to_pos: wanted to enter clearance area-->stay here.");
      return false;
    }
  } else if (s > 0) { // only consider if player is approaching ball
    if (dist_ball < clearance_radius_min) {
      LOG_DEB(LOG_LEVEL, "StandardSit: go_to_pos: too near to clearance area, correct target");
      target = WSinfo::me->pos + 10 * normal_to_ball;
      DEBUG_VECTOR(target.x, target.y, 1, "f8f800");
    } else {
      if (fabs(t) < clearance_radius_min && s < dist)
        target = target + (clearance_radius_min - fabs(t)) * dist / s * normal_to_ball;
      DEBUG_VECTOR(target.x, target.y, 1, "f0f000");
    } 
  }


  LOG_DEB(LOG_LEVEL, << _2D << C2D(WSinfo::ball->pos.x, WSinfo::ball->pos.y, 9.5, "666600") );
  
  go2pos->set_target(target);
  return go2pos->get_cmd(cmd);
}



//  general behavior for player kicking the ball
bool StandardSituation::behave_start_player(Cmd & cmd, SituationData* sit)
{
  Vector target;

  // if for 150 cycles nothing happened, just kick somewhere
  if (200 - act_sit_duration < 50) {
    panic_kick(cmd, sit);
    return true;
  }

  // must search ball?
  if (WSinfo::ball->age >= act_sit_duration || WSinfo::ball->age > max_ball_age_when_kick) {
    kicks_in_a_row = 0;
    faceBall->turn_to_ball();
    faceBall->get_cmd(cmd);
    LOG_DEB(LOG_LEVEL, "StandardSit: search Ball (duration=" << act_sit_duration << ",  ball_age=" << WSinfo::ball->age << ")");
    return true;
  }
  
  // kick-off: hardcoded kick
  if (act_sit_playmode == PM_my_KickOff && WSinfo::is_ball_kickable()) {
    target = kick_off_target;
    if (Tools::int_random(2)) target.y *= -1;
    neuroKick->reset_state();
    neuroKick->kick_to_pos_with_initial_vel(kick_off_speed, target);
    if (neuroKick->get_cmd(cmd)) {
      LOG_DEB(LOG_LEVEL, "Standardsit: made neuroKick for KickOff.");
      kicks_in_a_row++;
      last_behavior= neuroKick;
      last_kick_target = target;
      last_kick_speed = kick_off_speed;
      Intention intention;
      intention.set_panic_kick(target, WSinfo::ws->time);
      last_intention = intention;
      return true;
    }
  }
  
  
  // anounce pass before actually played
  if (time_left <= pass_announce_time && WSinfo::is_ball_kickable()) {
    Intention intention;
    LOG_DEB(LOG_LEVEL, "StandardSit: check if pass to announce.");
    if (playPass->get_intention(intention)) {
      LOG_DEB(LOG_LEVEL, "StandardSit: got pass-intention.");
      Blackboard::pass_intention = intention;

      Value new_kick_speed, old_kick_speed;
      Vector new_kick_target, old_kick_target;
      intention.get_kick_info(new_kick_speed, new_kick_target);
      last_intention.get_kick_info(old_kick_speed, old_kick_target);
      if (new_kick_speed != old_kick_speed || new_kick_target.sqr_distance(old_kick_target) != 0) {
        // new target: look in that direction
        faceBall->turn_to_point(new_kick_target);
        LOG_DEB(LOG_LEVEL, "StandardSit: New intention: look in that direction.");
      }
      last_intention = intention;
    }
  }
  
  // most important: never forget to kick the ball if possible
  if (time_left == 0 && playPass->get_cmd(cmd)) {
    last_behavior= playPass;
    kicks_in_a_row++;
    Intention intention;
    if (playPass->get_intention(intention)) {
      intention.get_kick_info(last_kick_speed, last_kick_target);
      last_intention = intention;
    }
    LOG_DEB(LOG_LEVEL, "Standardsit: i'm startplayer, and passed !");
    return true;
  }

	
  // update on ball depending target-positions
  update_effective_target_positions(sit);
  target = act_sit_ballpos;

  Value tolerance = go_to_kick_tolerance;
   
  if (act_sit_ballpos.y > FIELD_BORDER_Y - 0.5) {
    target.y = act_sit_ballpos.y - kick_border_setoff*WSinfo::me->kick_radius;
    tolerance = go_to_kick_border_tolerance*WSinfo::me->kick_radius;
  }
  if (act_sit_ballpos.y < -FIELD_BORDER_Y + 0.5) {
    target.y = act_sit_ballpos.y + kick_border_setoff*WSinfo::me->kick_radius;
    tolerance = go_to_kick_border_tolerance*WSinfo::me->kick_radius;
  }
  
  DEBUG_VECTOR(target.x, target.y, 2, "ff0000");

  // turn to an interesting direction
  if (WSinfo::is_ball_kickable() && time_left < 2) {
    Vector look_to(50, 0);
    if (sit->cnt_positions > 1)
      look_to = sit->pos[1].effective_position;
      
    cmd.cmd_main.set_turn(Tools::my_angle_to(look_to));
    kicks_in_a_row = 0;
    return true;
  }

  	
  // go to target position
  LOG_DEB(LOG_LEVEL, "Standardsit: go to homepos.");
  kicks_in_a_row = 0;
  go2pos->set_target(target, tolerance);
  return go2pos->get_cmd(cmd);
  //return neuroIntercept->get_cmd(cmd);
}



//  general behavior for player NOT kicking the ball
bool StandardSituation::behave_non_start_player(Cmd & cmd, SituationData* sit)
{
  // must search ball?
  if (!WSinfo::is_ball_pos_valid()) {
    faceBall->turn_to_ball();
    faceBall->get_cmd(cmd);
    LOG_DEB(LOG_LEVEL, "StandardSit: search Ball (duration=" << act_sit_duration << ",  ball_age=" << WSinfo::ball->age << ")");
    return true;
  }
	
  // update on ball pos depending target-positions
  if (act_sit_duration < 20 || (time_left % 10) == 5) 
    update_effective_target_positions(sit);

  // check pass info and start intercepting ball
  PPlayer p= WSinfo::get_teammate_with_newest_pass_info();
  if (p) {
    Vector ballpos,ballvel;
    ANGLE dir;
    Value speed;
    speed = p->pass_info.ball_vel.norm();
    dir = ANGLE(p->pass_info.ball_vel.arg());
    ballpos = p->pass_info.ball_pos;
    ballvel = p->pass_info.ball_vel;
    LOG_DEB(LOG_LEVEL, << "StandardSit: got pass info from player " 
	    << p->number  
	    << " a= " << p->pass_info.age
	    << " p= " << ballpos
	    << " v= " << p->pass_info.ball_vel
	    << " at= " << p->pass_info.abs_time
	    << " rt= " << p->pass_info.rel_time
	    <<" speed= "<<speed <<" dir "<<RAD2DEG(dir.get_value()));

    // check if i should intercept
    InterceptResult ires[8];
    WSpset ps = WSinfo::valid_teammates;
    ps.keep_and_sort_best_interceptors(3,ballpos,ballvel,ires);
    for(int p=0;p<ps.num;p++) {
      LOG_DEB(LOG_LEVEL, <<"StandardSit: best intercept: p="<<p<<",#="<<ps[p]->number <<",pos="<<ires[p].pos);
      if(ps[p]->number == WSinfo::me->number) {
 	Vector ipos,ballpos,ballvel;
        if(Policy_Tools::check_go4pass(ipos,ballpos,ballvel) == false) {
       	  LOG_DEB(LOG_LEVEL, <<"StandardSit: check_go4pass failed, i don't intercept!");
	} else {
      	  LOG_DEB(LOG_LEVEL, <<"StandardSit: i intercept!");
	  DEBUG_VECTOR(ires[p].pos.x, ires[p].pos.y, 1.5, "0000ff");
   	  return go_to_pos(cmd, ires[p].pos);     
	}
      }
    }
  }

  // nobody is near ball!?! then let me go there
  if (sit->we_have_ball && is_start_player_missing() && 
    act_sit_duration > (time_replace_missing_start_player + dist_factor_replace_missing_start_player*
      (WSinfo::me->pos - act_sit_ballpos).norm()))
  {
    LOG_DEB(LOG_LEVEL, <<"StandardSit: i'm n-s-pl, but nobody near ball --> i will kick!!");
    sit->start_player = mynumber;
    return behave_start_player(cmd, sit);
  }

  // find my target-position
  Vector target;
  int pos_number = sit->assigned_pos_to_player[mynumber-2];
  if (pos_number >= 0)
    target = sit->pos[pos_number].effective_position;
  else
    target = homepos;

  DEBUG_VECTOR(target.x, target.y, 1.5, "802000");
   
  target = refine_target_position(target, sit);
  
  DEBUG_VECTOR(target.x, target.y, 2, "ff0000");
  

  if (go_to_pos(cmd, target)) return true;
  
  if (WSinfo::is_ball_pos_valid() && time_left > 2) {
    LOG_DEB(LOG_LEVEL, "Standardsit: non-s-pl: dont move to target, turning.");
    cmd.cmd_main.set_turn(1.57);
    return true;
  } else {
    // turn to ball
    LOG_DEB(LOG_LEVEL, "Standardsit: non-s-pl: dont move to target, faceBall");
    faceBall->turn_to_ball();
    return faceBall->get_cmd(cmd);
  }
  
} 


bool StandardSituation::continue_playon(Cmd & cmd)
{
  if (WSinfo::ws->play_mode != PM_PlayOn && kicks_in_a_row == 0)
    return false;

  // some deadlock !? / player thinks can kick, but can't
  if (kicks_in_a_row >= max_kicks_in_a_row) {
    kicks_in_a_row = 0;
    last_behavior = 0;
    last_kick_speed = 0;
    return false;
  }

  // just continue last kick, if possible
  if (last_kick_speed > 0) {
    LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn, try continue kick (spd="<<last_kick_speed<<",target="<<last_kick_target<<").");
    neuroKick->reset_state();
    neuroKick->kick_to_pos_with_initial_vel(last_kick_speed, last_kick_target);
    if (neuroKick->get_cmd(cmd));
      kicks_in_a_row++;
      last_intention.confirm_intention(last_intention, WSinfo::ws->time);
      LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn kick continued, int. set");
      return true;
  }

  // otherwise try to continue last behavior
  if ( last_behavior == playPass ) {
    LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn, try pass again.");
    last_behavior= 0;
    if (playPass->get_cmd(cmd)) {
      last_behavior = playPass;
      kicks_in_a_row++;
      Intention intention;
      if (playPass->get_intention(intention)) {
        intention.get_kick_info(last_kick_speed, last_kick_target);
	last_intention = intention;
      }
      return true;
    }
  } else if ( last_behavior == neuroKick ) {
    LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn, try neurokick again.");
    last_behavior= 0;
    neuroKick->reset_state();
    neuroKick->kick_to_pos_with_max_vel(last_kick_target);
    if ( WSinfo::is_ball_kickable() && neuroKick->get_cmd(cmd)) {
      LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn, call neuro_kick again.");
      kicks_in_a_row++;
      last_behavior= neuroKick;
      last_kick_speed = ServerOptions::ball_speed_max;
      last_intention.confirm_intention(last_intention, WSinfo::ws->time);
      return true;
    }
  }
  LOG_DEB(LOG_LEVEL, <<"StandardSit: PM_PlayOn, turn to ball.");
  kicks_in_a_row = 0;
  faceBall->turn_to_ball();
  faceBall->get_cmd(cmd);
  return true;
}

void StandardSituation::write2blackboard()
{
  if (last_intention.valid_at() == WSinfo::ws->time) {
    Blackboard::pass_intention = last_intention;
    Value sp; Vector t; int i;
    last_intention.get_kick_info(sp, t, i);
    LOG_DEB(LOG_LEVEL, <<"StandardSit: blackb::p_int. set: type="<<last_intention.get_type() <<",validSince="<<last_intention.valid_since()<<",valid="<<last_intention.valid_at()<<",sp="<<sp<<",targ="<<t<<",player="<<i);
  }
}


// main behavior function
bool StandardSituation::get_cmd(Cmd & cmd)
{
/*
  cout << "\n-------- StandardSituation::get_cmd() -------------------"; {
  init_new_active_situation();
  act_sit_playmode = PM_my_KickIn;
  act_sit_ballpos = Vector(1,-1);
  active_situation = find_best_situation_data();
  cout << "\n active_sit: " << active_situation;
  cout << "\n ball: " << act_sit_ballpos;
  SituationData* sit = get_active_situation();
  if (!sit) {cout << "\n no situation!!!"; return false; }
  update_active_situation();
  for (int i=0; i<sit->cnt_positions; i++)
    cout << "\n eff.pos #" << i <<": "<< sit->pos[i].effective_position
      << "  \t pl " << sit->pos[i].effective_player;
  for (int i=2; i<=11; i++)
  cout << "\n home.pos #" << i << ": " << DeltaPositioning::get_position(i);
  } cout << "\n---------------------------------------------------------";
*/  

  // update my home position, depending on role
  mynumber = WSinfo::me->number;
  if (noball_demo->amIAttacker() && 
    (do_we_have_ball(WSinfo::ws->play_mode) || WSinfo::ws->play_mode == PM_PlayOn))
    homepos = DeltaPositioning::attack433.get_grid_pos(mynumber);
  else
    homepos = DeltaPositioning::get_position(mynumber);
  
  LOG_DEB(LOG_LEVEL, <<"StandardSit: INFO: ball=" << WSinfo::ball->pos << "  ,Age=" << WSinfo::ball->age << "  ,valid=" << int(WSinfo::is_ball_pos_valid()) << "  ,kickable=" << int(WSinfo::is_ball_kickable())<<", kicksIR="<<kicks_in_a_row);
  DEBUG_VECTOR(homepos.x, homepos.y, 1.5, "0000ff");
  DEBUG_VECTOR(WSinfo::ball->pos.x, WSinfo::ball->pos.y, 1, "22ff22");


  // goalie-player or no situations loaded?
  // then nothing we can do here
  if (use_standard_situations == false || mynumber == 1 || cnt_situations == 0) 
    return false; 

  // check if situation changed since last call
  if (WSinfo::ws->time - time_last_update > 2  ||
   (WSinfo::ws->play_mode != PM_PlayOn && WSinfo::ws->play_mode != act_sit_playmode))
    init_new_active_situation();

  time_last_update = WSinfo::ws->time;
  act_sit_duration = WSinfo::ws->time - act_sit_starttime;

  // we don?t handle normal play behavior, but let player complete 2-step-kicks and prevent
  // self-passes just after the end of a standard situation
  if (continue_playon(cmd)) {
    write2blackboard();
    return true;
  }

  // wait to receive new ball position
  if (WSinfo::ball->age < act_sit_duration)
    update_active_situation();

  // if no situation data available, some standard behavior
  SituationData* sit = get_active_situation();
  if (!sit) {
    idle_behavior(cmd);
    return true;
  }

  update_time_left(sit);
  update_ball_pos();
  sit->mirror_vertically = (act_sit_ballpos.y < 0);

  LOG_DEB(LOG_LEVEL, <<"StandardSit: INFO: #sit=" << active_situation << "  ,duration=" <<  act_sit_duration << "  ,time_left=" << time_left << "  ,eff_time=" << sit->effective_start_wait_time << "  ,tried_passes=" << act_sit_tried_passes);

  bool res;

  if (mynumber == sit->start_player)
    res = behave_start_player(cmd, sit);
  else
    res = behave_non_start_player(cmd, sit);

  write2blackboard();
  
  if (time_left == 0 && we_have_ball) act_sit_tried_passes++;
  
  return res;
}
