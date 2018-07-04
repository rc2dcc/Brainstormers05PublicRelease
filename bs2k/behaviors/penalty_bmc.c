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

#include "penalty_bmp.h"
#include "ws_info.h"
#include "ws_memory.h"
#include "log_macros.h"
#include "mdp_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "options.h"
#include "log_macros.h"
#include "geometry2d.h"

#include "../policy/policy_tools.h"
#include "../policy/positioning.h"  // get_role()
#include "blackboard.h"

#if 1
#define MYGETTIME (Tools::get_current_ms_time())
#define BASELEVEL 0 // level for logging; should be 3 for quasi non-logging
#else
#define BASELEVEL 3 // level for logging; should be 3 for quasi non-logging
#define MYGETTIME (0)
#endif

/* constructor method */
Penalty::Penalty() {
  /* read with ball params from config file*/
  cyclesI_looked2goal = 0;
  last_at_ball = -100;
  at_ball_for_cycles = 0;
  at_ball_patience = 1000;
  cycles_in_waitandsee = 0;
  last_waitandsee_at = -1;

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

  cout<<"\nPenalty status: Date: 7.02.03 / 1"<<endl;
}

Penalty::~Penalty() {
  delete neurokick;
  delete dribblestraight;
  delete basiccmd;
  delete onestepkick;
  delete oneortwo;
  delete onetwoholdturn;
  delete score;
  delete neuro_wball;
}

bool Penalty::get_cmd(Cmd & cmd) {
  Intention intention;

  LOG_POL(BASELEVEL, << "In PENALTY_BMC : ");
  if(!WSinfo::is_ball_kickable())
    return false;
  switch(WSinfo::ws->play_mode) {
  case PM_my_PenaltyKick:
  case PM_PlayOn:
    if(get_intention(intention)){
      intention2cmd(intention,cmd);
      if(cmd.cmd_main.get_type() == Cmd_Main::TYPE_KICK ||
	 cmd.cmd_main.get_type() == Cmd_Main::TYPE_TURN)
	last_waitandsee_at = WSinfo::ws->time;
      //LOG_POL(BASELEVEL, << "PENALTY: intention was set! ");
      return true;
    }
    else{
      LOG_POL(BASELEVEL, << "PENALTY: WARNING: NO CMD WAS SET");
      return false;
    }
    break;
  default:
    return false;  // behaviour is currently not responsible for that case
  }
  return false;  // behaviour is currently not responsible for that case
}

bool Penalty::get_intention(Intention &intention){

  long ms_time= MYGETTIME;
  LOG_POL(BASELEVEL+2, << "Entering Penalty");

  my_role= DeltaPositioning::get_role(WSinfo::me->number);
  //  my_role= 2; // test only !!!!
  // determine closest opponent (frequently needed)
  WSpset pset = WSinfo::valid_opponents;
  pset.keep_and_sort_closest_players_to_point(1, WSinfo::me->pos);
  if(pset.num >0)
    closest_opponent = pset[0];
  else
    closest_opponent = NULL;

  if(onestepkick->can_keep_ball_in_kickrange() == false)
    LOG_POL(0,<<"PENALTY: CANNOT keep ball in kickrange");



  

  bool result = false;

  if ( result = score->test_shoot2goal(intention) );
  else if ( (result = test_advance(intention)));
  else if ( (result = test_holdturn(intention)));
  else result = test_default(intention); // modified version



  ms_time = MYGETTIME - ms_time;
  LOG_POL(BASELEVEL+1, << "PENALTY policy needed " << ms_time << "millis to decide");

  return result;
}

/** default move */
bool Penalty::test_default(Intention &intention)
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

  if(cycles_in_waitandsee < at_ball_patience && onetwoholdturn->is_holdturn_safe() == true){
    ANGLE target_dir = (opgoalpos - WSinfo::me->pos).ARG();
    LOG_POL(BASELEVEL+0,<<"PENALTY DEFAULT Move - Hold Turn is Safe");
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
    LOG_POL(BASELEVEL+0,<<"PENALTY DEFAULT Clearance to "<<target<<" dir "<<RAD2DEG(dir)<<" w.speed "<<speed);
    return true;
  }

  LOG_POL(BASELEVEL+0,<<"PENALTY DEFAULT Move - Hold Turn is NOT Safe, but no other alternative, TRY");
  intention.set_holdturn((opgoalpos - WSinfo::me->pos).ARG(), WSinfo::ws->time);
  return true;
}

bool Penalty::test_holdturn(Intention &intention){
  if(cycles_in_waitandsee >= at_ball_patience){
    LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn NOT desired. Wait and see patience expired");
    return false;
  }
  if(onestepkick->can_keep_ball_in_kickrange() == false){
    LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn NOT possible. Can not keep ball in kickrange");
    return false;
  }

  ANGLE targetdir;

  if(WSinfo::me->pos.x > 42.0){  // if I'm close to goal, then turn2goal
    targetdir = (Vector(47,0) - WSinfo::me->pos).ARG();
  }
  else{
    targetdir = ANGLE(0.);  // turn straight ahead
  }


  int targetplayer_age= 1000;

  PPlayer targetplayer = WSinfo::get_teammate_by_number(my_blackboard.pass_or_dribble_intention.target_player);
  if(targetplayer != NULL){
    targetplayer_age = targetplayer->age;
    LOG_POL(0,"Holdturn: Check pass candidate: Age of targetplayer "<<targetplayer_age);
  }

  WSpset pset= WSinfo::valid_opponents;
  pset.keep_players_in_circle(WSinfo::me->pos, 3.); 

  if (my_blackboard.pass_or_dribble_intention.valid_at() == WSinfo::ws->time){ 
    // pass or dribble int. is set
    if((pset.num >0) && // opponent is close and pass intention communicated
       (my_blackboard.pass_or_dribble_intention.valid_since() < WSinfo::ws->time)){ 
      LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn ok, but pass set and op. close -> no holdturn");
      return false;
    }
    if((my_blackboard.pass_or_dribble_intention.valid_since() < WSinfo::ws->time) // I had 1 cycle time to communicate
       && targetplayer_age <=1 // targetplayer is reasonably young
       //	 && ((WSinfo::me->pos.x > 20) || // I'm advanced, so play quickly
       && (my_blackboard.pass_or_dribble_intention.kick_target.x>WSinfo::me->pos.x - 3.)){
      // or kick target is before me
      LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn possible, but pass set since "
		<<WSinfo::ws->time<<" and advancing pass -> play");
      return false;
    }
    if(Tools::get_abs_angle(targetdir.get_value() - WSinfo::me->ang.get_value()) <15./180. *PI){
      if(my_blackboard.pass_or_dribble_intention.valid_since() < WSinfo::ws->time-1){
	// I had 2 cycles time to communicate
	LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn possible, but pass set since "
		  <<"WSinfo::ws->time"<<"-> play");
	return false;
      }
    }
  }
    
  if(onetwoholdturn->is_holdturn_safe() == false){
    LOG_POL(BASELEVEL, <<"PENALTY: HoldTurn NOT possible. Not Safe");
    return false;
  }

  LOG_POL(BASELEVEL, <<"PENALTY: Do HOLD and TURN.");

  intention.set_holdturn(targetdir, WSinfo::ws->time);
  return true;
}

void Penalty::get_onestepkick_params(Value &speed, Value &dir){
  Value tmp_speed;
  Vector final;
  Vector ballpos = WSinfo::ball->pos;
  const int max_targets = 360;
  Value testdir[max_targets];
  Value testspeed[max_targets];

  int num_dirs = 0;

  //  for(ANGLE angle=ANGLE(0);angle.get_value()<2*PI;angle+=ANGLE(5./180.*PI)){
  for(float ang=0.;ang<PI;ang+=5./180.*PI){
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
	else
	  LOG_POL(0,"test onestep_kicks: Warning: too many targets");
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
  if(ipos.x < WSinfo::me->pos.x -20){
    speed = 0;
    return;
  }

  if(speed >0){
    LOG_POL(0,<<"Penalty: found onestepkick with advantage "<<advantage<<" resulting pos : "
	      <<ipos<<" closest teammate "<<closest_teammate);
  }
  else{
    LOG_POL(0,<<"Penalty: NO onestepkick found ");
  }
}

bool Penalty::is_dribblestraight_possible(){
  ANGLE opgoaldir = (Vector(52,0) - WSinfo::me->pos).ARG(); //go towards goal


  if(my_role == 0){// I'm a defender: 
    if(WSinfo::me->pos.x > -5)
      return false;
    int stamina = Stamina::get_state();
    if((stamina == STAMINA_STATE_ECONOMY || stamina == STAMINA_STATE_RESERVE )
       && WSinfo::me->pos.x >-35){// stamina low
      LOG_POL(0,<<"DRIBBLE STRAIGHT: I'm a defender and stamina level not so good -> do not advance");
      return false;
    }
  }

  if((WSinfo::me->ang.diff(ANGLE(0))>20/180.*PI || WSinfo::me->pos.x > 45) &&
     (WSinfo::me->ang.diff(opgoaldir)>20/180.*PI)){
    return false;
  }

  //  LOG_POL(0,<<"DRIBBLE STRAIGHT: me->ang "<<RAD2DEG(WSinfo::me->ang.get_value())<<" OK for Dribble");

  if(dribblestraight->is_dribble_safe(0)){
    if(my_blackboard.pass_or_dribble_intention.valid_since() < WSinfo::ws->time){
      // check area before me
      WSpset pset = WSinfo::valid_opponents;
      Vector endofregion;
      endofregion.init_polar(4, WSinfo::me->ang);
      endofregion += WSinfo::me->pos;
      Quadrangle2d check_area = Quadrangle2d(WSinfo::me->pos, endofregion, 2);
      pset.keep_players_in(check_area);
      if(pset.num >0){
	LOG_POL(0,<<"DRIBBLE STRAIGHT: Player directly before me and pass intention is set -> do not dribble");
	return false; // go on in any case
      }
    }
    LOG_POL(0,<<"DRIBBLE STRAIGHT: possible");
    return true;
  }
  return false;
}

bool Penalty::test_advance(Intention &intention){
  if(WSinfo::me->pos.x >25.0){//
    WSpset pset = WSinfo::valid_opponents;
    Vector endofregion;
    const Value scanrange = 5;
    endofregion.init_polar(scanrange, WSinfo::me->ang);
    endofregion += WSinfo::me->pos;
    Quadrangle2d check_area = Quadrangle2d(WSinfo::me->pos, endofregion,scanrange/2., scanrange);
    pset.keep_players_in(check_area);
    if(pset.num > 0){
      if(pset.num > 1 || pset[0] != WSinfo::his_goalie){
	LOG_POL(0,<<"TEST ADVANCE: in attack area: I CAN, but do NOT DRIBBLE, crowded area ");
	return false;
      }
    }
  }

  bool is_dribble_ok = is_dribblestraight_possible();

  if(is_dribble_ok && WSinfo::me->pos.x + 3< WSinfo::his_team_pos_of_offside_line()){
    WSpset pset = WSinfo::valid_opponents;
    Vector endofregion;
    const Value scanrange = 10;
    endofregion.init_polar(scanrange, WSinfo::me->ang);
    endofregion += WSinfo::me->pos;
    Quadrangle2d check_area = Quadrangle2d(WSinfo::me->pos, endofregion,scanrange/2., scanrange);
    pset.keep_players_in(check_area);
    if(pset.num > 0){
      if(pset.num > 1 || pset[0] != WSinfo::his_goalie){
	LOG_POL(0,<<"TEST ADVANCE: Area before me is crowded -> rather dribble ");
	Vector target;
	target.init_polar(2.0,WSinfo::me->ang); 
	intention.set_dribble(target, WSinfo::ws->time);

	return true;
      }
    }
  }

  if(is_dribble_ok){
    LOG_POL(0,<<"Test advance: Dribble is possible ");
    Vector target;
    target.init_polar(2.0,WSinfo::me->ang); 
    intention.set_dribble(target, WSinfo::ws->time);
    return true;
  }

  return false;
}

void Penalty::aaction2intention(const AAction &aaction, Intention &intention){

  Value speed = aaction.kick_velocity;
  Vector target = aaction.target_position;
  Value kickdir = aaction.kick_dir;

  Vector op_pos = aaction.actual_resulting_position;

  switch(aaction.action_type){
  case  AACTION_TYPE_PASS:
    intention.set_pass(target,speed, WSinfo::ws->time, aaction.targetplayer_number, 0, 
		       aaction.actual_resulting_position);
    break;
  case  AACTION_TYPE_LAUFPASS:
    intention.set_laufpass(target,speed, WSinfo::ws->time, aaction.targetplayer_number, 0, 
			   aaction.actual_resulting_position, aaction.risky_pass);
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
    LOG_POL(0,<<"PENALTY aaction2intention: AActionType not known");
    LOG_ERR(0,<<"PENALTY aaction2intention: AActionType not known");
  }
  intention.V = aaction.V;
}

bool Penalty::get_turn_and_dash(Cmd &cmd){
  // used for turn_and_dash
  Vector dummy1, dummy3;
  Vector mynewpos,ballnewpos;
  Value dummy2;
  int dash = 0;  
  Cmd_Main testcmd;
  Value required_turn;
  bool dash_found;

  required_turn = 
    Tools::get_angle_between_null_2PI(WSinfo::ball->vel.arg() - 
				      WSinfo::me->ang.get_value());
  // If I can get ball by dashing, dash
  dash_found=false;
  if(Tools::get_abs_angle(required_turn)<40./180. *PI){
    for(dash=100;dash>=30;dash-=10){
      testcmd.unset_lock();
      testcmd.unset_cmd();
      testcmd.set_dash(dash);
      Tools::model_cmd_main(WSinfo::me->pos, WSinfo::me->vel, WSinfo::me->ang.get_value(), 
			    WSinfo::ball->pos,
			    WSinfo::ball->vel,
			    testcmd, mynewpos, dummy1, dummy2, ballnewpos, dummy3);
      if((mynewpos-ballnewpos).norm()<0.8*ServerOptions::kickable_area){
	dash_found=true;
	break;
      }
      }
  }
  if(dash_found ==true){
    LOG_POL(BASELEVEL,<<"TURN_AND_DASH: I found a dash "<<dash);
    basiccmd->set_dash(dash);
    basiccmd->get_cmd(cmd);
    return true;
    }
  
  if(Tools::get_abs_angle(required_turn)>10./180. *PI){
    LOG_POL(BASELEVEL,<<"Intention2cmd: Turn and Dash: turn 2 balldir "
	    <<RAD2DEG(WSinfo::ball->vel.arg())<<" Have to turn by "
	    <<RAD2DEG(required_turn));
    basiccmd->set_turn_inertia(required_turn);
    basiccmd->get_cmd(cmd);
    return true;
  }
  // dash forward
  basiccmd->set_dash(100);
  basiccmd->get_cmd(cmd);
  return true;
}

void Penalty::reset_intention()
{
}

bool Penalty::intention2cmd(Intention &intention, Cmd &cmd){
  Value speed; 
  Vector target;
  speed = intention.kick_speed;
  target = intention.kick_target;
  ANGLE targetdir = intention.target_body_dir; 
  int targetplayer_number = intention.target_player;
  //  Value ball2targetdir = (target - WSinfo::ball->pos).arg(); // direction of the target
  bool cankeepball = onestepkick->can_keep_ball_in_kickrange();
  //Value speed1step = Move_1Step_Kick::get_vel_in_dir(speed, ball2targetdir);
  //bool need_only1kick = (fabs(speed -speed1step) < .2);
  Value opposite_balldir;
  Value kick_dir;
  oot_intention = intention;//hauke

  switch(intention.get_type()){
  case  PASS:
  case  LAUFPASS:
    speed = intention.kick_speed;
    target = intention.kick_target;
    LOG_POL(BASELEVEL,<<"PENALTY: AAction2Cmd: passing to teammate "<<targetplayer_number
	      //<<" onestepkick is possible (0=false) "<<need_only1kick
	      <<" speed "<<speed<<" to target "<<target.x<<" "<<target.y);
    if(intention.risky_pass == true){
      LOG_POL(BASELEVEL,<<"PENALTY: RISKY PASS!!!");
    }
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
      LOG_ERR(0,<<"Can't keep the ball in my kickrange, and I do not plan to pass. Kick to "
	      <<RAD2DEG(opposite_balldir));
      //return Move_Factory::get_Kick(100,opposite_balldir,1);
      basiccmd->set_kick(100,ANGLE(opposite_balldir));
      basiccmd->get_cmd(cmd);
      return true;
    }

    break;
  case TURN_AND_DASH:
    return get_turn_and_dash(cmd);
    break;
  case  DRIBBLE:  
    dribblestraight->get_cmd(cmd);
    return true;
    break;
  case SCORE:
    speed = intention.kick_speed;
    target = intention.kick_target;
    mdpInfo::set_my_intention(DECISION_TYPE_SCORE, speed, 0, target.x, target.y,0);
    LOG_POL(BASELEVEL,<<"PENALTY aaction2cmd: try to score w speed "<<speed<<" to target "<<target);
    neurokick->kick_to_pos_with_initial_vel(speed,target);
    neurokick->get_cmd(cmd);
    return true;
    break;
  case KICKNRUSH:
    speed = intention.kick_speed;
    target = intention.kick_target;
    mdpInfo::set_my_intention(DECISION_TYPE_KICKNRUSH, speed,0,0,0, 0);
    LOG_POL(BASELEVEL,<<"PENALTY aaction2cmd: kicknrush w speed "<<speed<<" to target "<<target);
    neurokick->kick_to_pos_with_initial_vel(speed,target);
    neurokick->get_cmd(cmd);
    return true;
    break;
  case PANIC_KICK:
    target = intention.kick_target;
    kick_dir = (target - WSinfo::me->pos).arg() - WSinfo::me->ang.get_value();
    basiccmd->set_kick(100,ANGLE(kick_dir));
    basiccmd->get_cmd(cmd);
    return true;
    break;
  case BACKUP:
    LOG_POL(BASELEVEL,<<"PENALTY aaction2cmd: back up (two teammates at ball)  not yet implemented");
    LOG_ERR(BASELEVEL,<<"PENALTY aaction2cmd: back up (two teammates at ball)  not yet implemented");
    return false;
    break;
  case HOLDTURN:
    LOG_POL(BASELEVEL,<<"PENALTY Intention: holdturn in dir "<<RAD2DEG(intention.target_body_dir.get_value()));
    if(cankeepball){
      if(onetwoholdturn->is_holdturn_safe() == false){
	LOG_POL(BASELEVEL,<<"PENALTY Intention: holdturn NOT safe, relaxed trial (should only occur in troubled sits)");
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
      LOG_ERR(0,<<"Can't keep the ball in my kickrange, and I do not plan to pass. Kick to "
	      <<RAD2DEG(opposite_balldir));
      //return Move_Factory::get_Kick(100,opposite_balldir,1);
      basiccmd->set_kick(100,ANGLE(opposite_balldir));
      basiccmd->get_cmd(cmd);
      return true;
    }

    return true;
  default:
    LOG_POL(0,<<"PENALTY aaction2cmd: AActionType not known");
    LOG_ERR(0,<<"PENALTY aaction2cmd: AActionType not known");
    return false;
  }
}
