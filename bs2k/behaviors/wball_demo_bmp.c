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

#include "wball_demo_bmp.h"
#include "ws_info.h"
#include "ws_memory.h"
#include "log_macros.h"
#include "mdp_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "../policy/planning.h"
#include "../policy/policy_tools.h"
#include "../policy/positioning.h"  
#include "blackboard.h"

#define MIN(X,Y) ((X<Y)?X:Y)

/* constructor method */
WballDemo::WballDemo() {
  last_at_ball = -100;
  at_ball_for_cycles = 0;
  cycles_in_waitandsee = 0;
  last_waitandsee_at = -1;
  last_heavy_attack_at = -1;

  
  neurokick = new NeuroKick05;
  dribblestraight = new DribbleStraight;
  basiccmd = new BasicCmd;
  onestepkick = new OneStepKick;
  oneortwo = new OneOrTwoStepKick;
  onetwoholdturn = new OneTwoHoldTurn;
  score = new Score;
  neuro_wball = new NeuroWball;

  my_blackboard.pass_or_dribble_intention.reset();
  my_blackboard.intention.reset();
}

WballDemo::~WballDemo() {
  delete neurokick;
  delete dribblestraight;
  delete basiccmd;
  delete onestepkick;
  delete oneortwo;
  delete onetwoholdturn;
  delete score;
  delete neuro_wball;
}

void WballDemo::reset_intention() {
  my_blackboard.pass_or_dribble_intention.reset();
  my_blackboard.intention.reset();
}

bool WballDemo::get_cmd(Cmd & cmd) {
  Intention intention;
  if(!WSinfo::is_ball_kickable())
    return false;
  switch(WSinfo::ws->play_mode) {
  case PM_PlayOn:
    if(get_intention(intention)){
      intention2cmd(intention,cmd);
      if(cmd.cmd_main.get_type() == Cmd_Main::TYPE_KICK ||
	 cmd.cmd_main.get_type() == Cmd_Main::TYPE_TURN)
	last_waitandsee_at = WSinfo::ws->time;
      Blackboard::main_intention = intention; 
      return true;
    }
    else{
      LOG_POL(0, << "WBALLDEMO: WARNING: NO CMD WAS SET");
      return false;
    }
    break;
  default:
    return false;  // behaviour is currently not responsible for that case
  }
  return false;  // behaviour is currently not responsible for that case
}

bool WballDemo::get_intention(Intention &intention){

  LOG_POL(0, << "Entering WballDemo");

  if(last_at_ball == WSinfo::ws->time -1)
    at_ball_for_cycles ++;
  else
    at_ball_for_cycles =1;
  last_at_ball = WSinfo::ws->time;

  if(last_waitandsee_at == WSinfo::ws->time -1) // waitandsee selected again
    cycles_in_waitandsee ++;
  else
    cycles_in_waitandsee = 0; // reset

  I_am_heavily_attacked_since();  //* call once to update

  my_role= DeltaPositioning::get_role(WSinfo::me->number);
  // determine closest opponent (frequently needed)
  WSpset pset = WSinfo::valid_opponents;
  pset.keep_and_sort_closest_players_to_point(1, WSinfo::me->pos);
  if(pset.num >0)
    closest_opponent = pset[0];
  else
    closest_opponent = NULL;

  Intention tmp, tmp_previous, alternative_pass_intention;

  tmp_previous = my_blackboard.pass_or_dribble_intention; // copy old intention

  if(my_blackboard.pass_or_dribble_intention.valid_at() != WSinfo::ws->time)
  { // not set yet
    if(get_pass_or_dribble_intention(tmp))
    { // check for a new intention.
      if(tmp_previous.valid_at() == WSinfo::ws->time -1)
      {// if previous intention was set in last cycle
	if(tmp_previous.kick_target.ARG().diff(tmp.kick_target.ARG()) < 20./180. *PI){
	  tmp.set_valid_since(WSinfo::ws->time-1);  // indicate
	}
      } // previous intention was valid last cycle
      my_blackboard.pass_or_dribble_intention = tmp;
    }
    else
      my_blackboard.pass_or_dribble_intention.reset();      
  }

  bool result = false;
  // daisy
  if ( 0 )
  {}
  else if ( result = score->test_shoot2goal(intention) ){
    LOG_POL(0,"Daisy chain: test SCORE successful");
  }
  else if ( (result = test_dribble(intention)) ){
    LOG_POL(0,"Daisy chain: test DRIBBLING successful");
  }
  else if ( in_penalty_mode == false && (result = test_pass_or_dribble(intention))){
    LOG_POL(0,"Daisy chain: test PASS successful");
  }
  else if ( (result = test_holdturn(intention))){
    LOG_POL(0,"Daisy chain: test HOLDTURN successful");
  }
  else result = test_default(intention); 

  my_blackboard.intention = intention;

  return result;
}


bool WballDemo::get_pass_or_dribble_intention(Intention &intention){
  AState current_state;
  AbstractMDP::copy_mdp2astate(current_state);

  return get_pass_or_dribble_intention(intention, current_state);
}

bool WballDemo::get_pass_or_dribble_intention(Intention &intention, AState &state){
  AAction best_aaction;


  intention.reset();

  if (neuro_wball->evaluate_passes(best_aaction, state) == false){ // nothing found
    return false;
  }

  if(best_aaction.action_type == AACTION_TYPE_WAITANDSEE)
    last_waitandsee_at = WSinfo::ws->time;
  
  aaction2intention(best_aaction, intention);

  // found a pass or dribbling
  return true;
}

/** default move */
bool WballDemo::test_default(Intention &intention)
{

  if(onestepkick->can_keep_ball_in_kickrange() == false){
    WSpset pset= WSinfo::valid_opponents;
    Vector endofregion;
    endofregion.init_polar(5, WSinfo::ball->vel.ARG());
    endofregion += WSinfo::me->pos;
    Quadrangle2d check_area = Quadrangle2d(WSinfo::me->pos, endofregion, 3);
    pset.keep_players_in(check_area);
    if(pset.num == 0){
      intention.set_holdturn(ANGLE(0.), WSinfo::ws->time); // this will maximally stop the ball
      return true;
    }
  }

  Vector opgoalpos = Vector (52.,0); // opponent goalpos

  if(cycles_in_waitandsee < 50 && onetwoholdturn->is_holdturn_safe() == true){
    ANGLE target_dir = (opgoalpos - WSinfo::me->pos).ARG();
    intention.set_holdturn(target_dir, WSinfo::ws->time);
    return true;
  }

  Vector target;
  Value speed, dir;
  get_onestepkick_params(speed,dir);
  if(speed > 0){
    target.init_polar(speed/(1-ServerOptions::ball_decay),dir);
    target += WSinfo::ball->pos;
    intention.set_kicknrush(target,speed, WSinfo::ws->time);
    return true;
  }

  intention.set_holdturn((opgoalpos - WSinfo::me->pos).ARG(), WSinfo::ws->time);
  return true;
}


bool WballDemo::am_I_attacked(){
  WSpset pset= WSinfo::valid_opponents;
  Value radius_of_attacked_circle =  2 * ServerOptions::kickable_area + 2 * ServerOptions::player_speed_max;
  pset.keep_players_in_circle(WSinfo::me->pos,radius_of_attacked_circle); 
  return  (pset.num >0);

}

int WballDemo::I_am_heavily_attacked_since(){
  // returns -1, if no attacking occured. Must be called at least once every cycle to be effective.
  WSpset pset= WSinfo::valid_opponents;
  Value radius_of_attacked_circle =  2*ServerOptions::kickable_area;
  pset.keep_players_in_circle(WSinfo::me->pos,radius_of_attacked_circle); 

  if(pset.num ==0){ //* no attack
      last_heavy_attack_at = -1; // reset
    return -1;
  }
  // I am heavily attacked this cycle
  if(last_heavy_attack_at <0){
    last_heavy_attack_at = WSinfo::ws->time; // remember start of heavy attack
    return 1;
  }
  return ((WSinfo::ws->time - last_heavy_attack_at) +1);
}

bool WballDemo::test_holdturn(Intention &intention){
  if(onetwoholdturn->is_holdturn_safe() == false){
    return false;
  }

  if(cycles_in_waitandsee >= 50){
    return false;
  }
  if(onestepkick->can_keep_ball_in_kickrange() == false){
    return false;
  }

  ANGLE targetdir;

  if(WSinfo::me->pos.x > 42.0){  // if I'm close to goal, then turn2goal
    targetdir = (Vector(47,0) - WSinfo::me->pos).ARG();
  }
  else{
    targetdir = ANGLE(0.);  // turn straight ahead
  }
  intention.set_holdturn(targetdir, WSinfo::ws->time);
  return true;
}

void WballDemo::get_onestepkick_params(Value &speed, Value &dir){
  Value tmp_speed;
  Vector final;
  Vector ballpos = WSinfo::ball->pos;
  const int max_targets = 360;
  Value testdir[max_targets];
  Value testspeed[max_targets];

  int num_dirs = 0;

  for(float ang=0.;ang<PI/2.;ang+=5./180.*PI){ //ridi 05: allow only forward directions!
    for(int sign = -1; sign <= 1; sign +=2){
      ANGLE angle=ANGLE((float)(sign * ang));
      tmp_speed = onestepkick->get_max_vel_in_dir(angle);
      if(tmp_speed <0.1){
	final.init_polar(1.0, angle);
	final += ballpos;
      }
      else{
	testdir[num_dirs] = angle.get_value();
	testspeed[num_dirs] = neuro_wball->adjust_speed(WSinfo::ball->pos, angle.get_value(),tmp_speed);
	if(num_dirs < max_targets)
	  num_dirs ++;
	final.init_polar(tmp_speed, angle);
	final += ballpos;
      }
    }
  }
  int advantage;
  Vector ipos;
  int closest_teammate;
  Policy_Tools::get_best_kicknrush(WSinfo::me->pos,num_dirs,testdir,
				   testspeed,speed,dir,ipos,advantage, closest_teammate);
}


void WballDemo::get_opening_pass_params(Value &speed, Value &dir, Vector &ipos, int &advantage, 
				      int & closest_teammate){
  float min_angle = -50;
  float max_angle = 50;

  const int max_targets = 200;
  Value testdir[max_targets];
  Value testspeed[max_targets];

  int i= 0;
  float tmp_speed;
  Angle tmp_dir;
  for(float angle=min_angle;angle<max_angle;angle+=5){
    for(float speed=2.5;speed<2.6;speed+=.5){
      if (i>=max_targets)
	break;
      tmp_speed = speed;
      tmp_dir = angle/180. *PI;
      if(Planning::is_kick_possible(tmp_speed,tmp_dir) == false){
	continue;
      }
      testdir[i]= tmp_dir;
      testspeed[i]= tmp_speed;
      i++;
    }
  }

  Policy_Tools::get_best_kicknrush(WSinfo::me->pos,i,testdir,
				   testspeed,speed,dir,ipos,advantage, closest_teammate);
}

bool WballDemo::is_dribble_possible(){
  ANGLE opgoaldir = (Vector(52,0) - WSinfo::me->pos).ARG(); //go towards goal


  if(my_role == 0){// I'm a defender: 
    if(WSinfo::me->pos.x > -5)
      return false;
    int stamina = Stamina::get_state();
    if((stamina == STAMINA_STATE_ECONOMY || stamina == STAMINA_STATE_RESERVE )
       && WSinfo::me->pos.x >-35){// stamina low
      return false;
    }
  }

  if((WSinfo::me->ang.diff(ANGLE(0))>20/180.*PI || WSinfo::me->pos.x > 45) &&
     (WSinfo::me->ang.diff(opgoaldir)>20/180.*PI)){
    return false;
  }

  if(dribblestraight->is_dribble_safe(0)){
    if(my_blackboard.pass_or_dribble_intention.valid_since() < WSinfo::ws->time){
      // check area before me
      WSpset pset = WSinfo::valid_opponents;
      Vector endofregion;
      Vector startofregion;
      startofregion.init_polar(-.5, WSinfo::me->ang);
      startofregion += WSinfo::me->pos;
      endofregion.init_polar(4, WSinfo::me->ang);
      endofregion += WSinfo::me->pos;
      Quadrangle2d check_area = Quadrangle2d(startofregion, endofregion, 3);
      pset.keep_players_in(check_area);
      if(pset.num >0){
	return false; // go on in any case
      }
    }
    return true;
  }
  return false;
}

bool WballDemo::test_pass_or_dribble(Intention &intention){
  if(my_blackboard.pass_or_dribble_intention.valid_at() == WSinfo::ws->time){
    intention = my_blackboard.pass_or_dribble_intention;
    return true;
  }
  return false;
}

void WballDemo::aaction2intention(const AAction &aaction, Intention &intention){

  Value speed = aaction.kick_velocity;
  Vector target = aaction.target_position;
  Value kickdir = aaction.kick_dir;

  Vector op_pos = aaction.actual_resulting_position;

  switch(aaction.action_type){
  case  AACTION_TYPE_PASS:
    intention.set_pass(target,speed, WSinfo::ws->time, aaction.targetplayer_number, 0, 
		       aaction.actual_resulting_position, aaction.potential_position );
    break;
  case  AACTION_TYPE_LAUFPASS:
    intention.set_laufpass(target,speed, WSinfo::ws->time, aaction.targetplayer_number, 0, 
			   aaction.actual_resulting_position, aaction.risky_pass,  aaction.potential_position);
    break;
  case  AACTION_TYPE_WAITANDSEE:
    intention.set_waitandsee(WSinfo::ws->time);
    break;
  case AACTION_TYPE_TURN_AND_DASH:
    intention.set_turnanddash(WSinfo::ws->time);
    break;
  case  AACTION_TYPE_DRIBBLE:
    intention.set_dribble(target, WSinfo::ws->time);
    break;
  default:
    LOG_POL(0,<<"WBALLDEMO aaction2intention: AActionType not known");
    LOG_ERR(0,<<"WBALLDEMO aaction2intention: AActionType not known");
  }
  intention.V = aaction.V;
}

bool WballDemo::intention2cmd(Intention &intention, Cmd &cmd){
  Value speed; 
  Vector target;
  speed = intention.kick_speed;
  target = intention.kick_target;
  bool cankeepball = onestepkick->can_keep_ball_in_kickrange();
  //Value speed1step = Move_1Step_Kick::get_vel_in_dir(speed, ball2targetdir);
  //bool need_only1kick = (fabs(speed -speed1step) < .2);
  Value opposite_balldir;
  Value kick_dir;

  Value toRightCornerFlag = abs((Vector(FIELD_BORDER_X,-FIELD_BORDER_Y) - WSinfo::me->pos).ARG().get_value_mPI_pPI());
  Value toLeftCornerFlag = abs((Vector(FIELD_BORDER_X,FIELD_BORDER_Y) - WSinfo::me->pos).ARG().get_value_mPI_pPI());
  Value toGoal = abs(WSinfo::me->ang.get_value_mPI_pPI());
    

  //tell the blackboard's pass intention whether this pass is really
  //intended to be actively played (i.e. a kick is already initiated
  //within the current cycle
  if (
         intention.get_type() == PASS
      || intention.get_type() == LAUFPASS
      || intention.get_type() == KICKNRUSH
      || intention.get_type() == PANIC_KICK
     )
    Blackboard::pass_intention.immediatePass = true;
  else
    Blackboard::pass_intention.immediatePass = false;

  switch(intention.get_type()){
  case  PASS:
  case  LAUFPASS:
    speed = intention.kick_speed;
    target = intention.kick_target;
    neurokick->kick_to_pos_with_initial_vel(speed,target);
    neurokick->get_cmd(cmd);
    return true;
    break;
  case  WAITANDSEE:
    if(cankeepball){
      mdpInfo::set_my_intention(DECISION_TYPE_WAITANDSEE);
      onetwoholdturn->get_cmd(cmd);
      last_waitandsee_at = WSinfo::ws->time;
      return true;
    }
    else{
      opposite_balldir = WSinfo::ball->vel.arg() + PI;
      opposite_balldir = opposite_balldir - WSinfo::me->ang.get_value();
      basiccmd->set_kick(100,ANGLE(opposite_balldir));
      basiccmd->get_cmd(cmd);
      return true;
    }

    break;
  case  DRIBBLE:  
    dribblestraight->get_cmd(cmd);
    return true;
    break;
  case SCORE:
    speed = intention.kick_speed;
    target = intention.kick_target;
    if (intention.immediatePass == false)
    {
      mdpInfo::set_my_intention(DECISION_TYPE_SCORE, speed, 0, target.x, target.y,0);
      neurokick->kick_to_pos_with_initial_vel(speed,target);
      neurokick->get_cmd(cmd);
    }
    else
    {
      onestepkick->kick_to_pos_with_initial_vel(speed,target);      
      onestepkick->get_cmd(cmd);
    }
    return true;
    break;
  case HOLDTURN:
    if(cankeepball){
      if(onetwoholdturn->is_holdturn_safe() == false){
	last_waitandsee_at = WSinfo::ws->time;
	onetwoholdturn->get_cmd_relaxed(cmd);
	return true;
      }
      last_waitandsee_at = WSinfo::ws->time;
      onetwoholdturn->get_cmd(cmd,intention.target_body_dir);
      return true;
    }
    else{
      opposite_balldir = WSinfo::ball->vel.arg() + PI;
      opposite_balldir = opposite_balldir - WSinfo::me->ang.get_value();
      basiccmd->set_kick(100,ANGLE(opposite_balldir));
      basiccmd->get_cmd(cmd);
      return true;
    }
    return true;
  default:
    LOG_ERR(0,<<"WBALLDEMO intention2cmd: AActionType not known");
    return false;
  }
}

bool WballDemo::test_dribble(Intention &intention){
  if(is_dribble_possible() == false)
    return false;

  Vector target;
  target.init_polar(2.0,WSinfo::me->ang); 
  intention.set_dribble(target, WSinfo::ws->time);
  return true;
}

