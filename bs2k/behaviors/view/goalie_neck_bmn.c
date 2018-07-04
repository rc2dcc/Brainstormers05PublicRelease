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

#include "goalie_neck_bmn.h"
#include "mdp_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "tools.h"
#include "valueparser.h"
#include "options.h"
#include "cmd.h"
#include "log_macros.h"

bool GoalieNeck::initialized = false;

GoalieNeck::GoalieNeck() {
  time_of_turn = 0;
  time_attacker_seen = 0;
  last_time_look_to_ball = 0;
  
  neck_cmd = new NeckCmd();
}

GoalieNeck::~GoalieNeck() {
  delete neck_cmd;
}

bool GoalieNeck::get_cmd(Cmd &cmd) {
  return goalie_neck(cmd);
}

#define GOALIE_MAX_ANGLE_TO_BALL 1/180.*PI

/** locks neck angle to body angle */
bool GoalieNeck::neck_lock(Cmd &cmd) {
  LOG_POL(3,<<"Goalie_Neck - neck_lock: Locking neck angle to body angle");
  neck_cmd->set_turn_neck_rel(0.0);
  return neck_cmd->get_cmd(cmd);
  //return Move_Factory::get_Neck_Turn_Rel(0);
}


void GoalieNeck::compute_steps2go(){
  /* calculate number of steps to intercept ball, for every player on the pitch, return sorted list */
  Policy_Tools::go2ball_steps_update();
  go2ball_list = Policy_Tools::go2ball_steps_list();

  steps2go.me = 1000;
  steps2go.opponent = 1000;
  steps2go.opponent_number = 0;
  steps2go.teammate = 1000;
  steps2go.teammate_number = 0;
  steps2go.my_goalie = 1000;

  
  /* get predicted steps to intercept ball for me, my goalie , fastest teammate and fastest opponent */
  for(int i=0;i<22;i++){
    if(go2ball_list[i].side == Go2Ball_Steps::MY_TEAM){
      if(WSinfo::me->number == go2ball_list[i].number){
	steps2go.me = go2ball_list[i].steps;
      }
      else if(WSinfo::ws->my_goalie_number == go2ball_list[i].number){
	steps2go.my_goalie = go2ball_list[i].steps;
	
      }
      else if(steps2go.teammate == 1000){
	steps2go.teammate = go2ball_list[i].steps;
	steps2go.teammate_number = go2ball_list[i].number;
      }
    }
    if(go2ball_list[i].side == Go2Ball_Steps::THEIR_TEAM){
      if(steps2go.opponent == 1000){
	steps2go.opponent = go2ball_list[i].steps;
	steps2go.opponent_number = go2ball_list[i].number;
      }
    } 
  }

  PPlayer p_tmp = WSinfo::get_opponent_by_number(steps2go.opponent_number);
  if (p_tmp != NULL) {
    steps2go.opponent_pos = p_tmp->pos;
    steps2go.ball_kickable_for_opponent = WSinfo::is_ball_kickable_for(p_tmp);
  }
  else {
    steps2go.opponent_pos = Vector(0.0, 0.0);
    steps2go.ball_kickable_for_opponent = false;
  }

  p_tmp = WSinfo::get_teammate_by_number(steps2go.teammate_number);
  if (p_tmp != NULL) {
    steps2go.teammate_pos = p_tmp->pos;
    steps2go.ball_kickable_for_teammate = WSinfo::is_ball_kickable_for(p_tmp);
  }
  else {
    steps2go.teammate_pos = Vector(0.0, 0.0);
    steps2go.ball_kickable_for_teammate = false;
  }

}


bool GoalieNeck::goalie_neck(Cmd &cmd){
  double VZ;
  IntentionType it;
  Value neck2ball_dir;
  mdpInfo::get_my_neck_intention(it);


  if(it.type==NECK_INTENTION_SCANFORBALL) {
    LOG_POL(3,<<"Goalie_Neck: Got neck intention type SCANFORBALL!");
    return neck_lock(cmd);
  } 

  compute_steps2go();

  if((WSinfo::ws->play_mode == PM_my_GoalKick) || 
     (WSinfo::ws->play_mode == PM_my_GoalieFreeKick) && 
     (it.type!=NECK_INTENTION_LOOKINDIRECTION))
    return neck_standard(cmd);

  if (it.type == NECK_INTENTION_LOOKINDIRECTION) {
    LOG_POL(0, << "Neck_Intention ist gesetzt, Drehung nach" << it.p1);
    time_of_turn = WSinfo::ws->time;
    neck2ball_dir = it.p1;
  } else {
    Vector ball_pos = WSinfo::ball->pos;
    Vector my_pos = WSinfo::me->pos;

    neck2ball_dir = mdpInfo::my_neck_angle_to(ball_pos);
    if ((!steps2go.ball_kickable_for_opponent) && 
	(!steps2go.ball_kickable_for_teammate)) {
      neck2ball_dir = mdpInfo::my_neck_angle_to(WSinfo::ball->pos + WSinfo::ball->vel);
    } else if ((my_pos - ball_pos).sqr_norm() < 36.0) {
      if (steps2go.ball_kickable_for_opponent) {
	LOG_DEB(0, << "ball kickable for opponent!");
	neck2ball_dir = mdpInfo::my_neck_angle_to(steps2go.opponent_pos);
      } else {
	LOG_DEB(0, << "ball kickable for teammate!");
	neck2ball_dir = mdpInfo::my_neck_angle_to(steps2go.teammate_pos);
      }
    }
  }

  /*
  if (((mdpInfo::my_distance_to_ball() <= 24.0) || 
       (mdpInfo::is_ball_infeelrange_next_time())) &&
      (mdpInfo::time_current() == last_time_look_to_ball + 1) &&
      (!mdpInfo::is_ball_kickable_for_teammate(mdpInfo::teammate_closest_to_ball())) &&
      (!mdpInfo::is_ball_kickable_for_opponent(mdpInfo::opponent_closest_to(WSinfo::ball->pos)))) {
    //neck2ball_dir = mdpInfo::my_neck_angle_to();
    Value rel_neck = mdpInfo::mdp->me->neck_angle_rel();
    if (mdpInfo::mdp->me->ang.v < M_PI) {
      neck2ball_dir = 1.5 * M_PI - rel_neck;
    } else {
      neck2ball_dir = M_PI/2.0 - rel_neck;
    }
  } else {
    last_time_look_to_ball = mdpInfo::time_current();
    }*/

  if (cmd.cmd_main.get_type() == 
      cmd.cmd_main.TYPE_TURN){
    Value turnangle = 0;
    cmd.cmd_main.get_turn(turnangle);
    if (turnangle > M_PI) {
      turnangle = 2*M_PI - turnangle;
      VZ = 1.0;
    } else {
      VZ = -1.0;
    }
    turnangle = turnangle / 
      (1.0 + ServerOptions::inertia_moment * WSinfo::me->vel.norm());
    neck2ball_dir = neck2ball_dir + VZ * turnangle;
  }

  if(Tools::get_abs_angle(neck2ball_dir) > GOALIE_MAX_ANGLE_TO_BALL){
    LOG_MOV(1,<<"Turn Neck in Goalie-mode:  try to turn to ball "<<RAD2DEG(neck2ball_dir));
    time_of_turn = WSinfo::ws->time;
    neck_cmd->set_turn_neck(neck2ball_dir);
    return neck_cmd->get_cmd(cmd);
    //return Move_Factory::get_Turn_Neck(neck2ball_dir);
  }
  else{
    LOG_MOV(1,<<"Turn Neck in Goalie-mode:  Already looking in ball direction ");
    time_of_turn = WSinfo::ws->time;
    neck_cmd->set_turn_neck(0.0);
    return neck_cmd->get_cmd(cmd);
    //return Move_Factory::get_Neck_Move();
  }

}

bool GoalieNeck::neck_standard(Cmd &cmd) {
    
  Value desired_turn;

  if (cmd.cmd_main.get_type() == 
      cmd.cmd_main.TYPE_TURN){ // body turn command -> do not turn neck
    LOG_MOV(1,<<"Turn Neck: body turn command -> do not turn neck ");
    time_of_turn = WSinfo::ws->time;
    neck_cmd->set_turn_neck(0.0);
    return neck_cmd->get_cmd(cmd);
    //return Move_Factory::get_Neck_Move();
  }

  if(mdpInfo::mdp->time_of_last_update > time_of_turn){
    Value rel_neck = WSinfo::me->neck_ang_rel.get_value();
    //LOG_MOVE_D<<"Turn Neck: rel neck "<<rel_neck<<" " <<RAD2DEG(rel_neck);
    if((rel_neck < PI) && (rel_neck > 
			   fabs(ServerOptions::maxneckang.get_value()) - 10./180. *PI) )
      desired_turn = - 2*fabs(ServerOptions::minneckang.get_value());
    else
      desired_turn = 0.9 * mdpInfo::view_angle_width_rad();
  }
  else
    desired_turn =0.0;
  if(desired_turn){
    // do a  turn neck
    desired_turn = Tools::get_angle_between_null_2PI(desired_turn);
    //LOG_MOVE_D<<"Turn Neck: try to turn neck by " <<RAD2DEG(desired_turn);
    time_of_turn = WSinfo::ws->time;
    neck_cmd->set_turn_neck(desired_turn);
    return neck_cmd->get_cmd(cmd);
    //return Move_Factory::get_Turn_Neck(desired_turn);
  }
  neck_cmd->set_turn_neck(0.0);
  return neck_cmd->get_cmd(cmd);
  //return Move_Factory::get_Neck_Move();
}

