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

#include "train_behaviors_bmc.h"

#include "ws_info.h"
#include "log_macros.h"
#include "tools.h"
#include "blackboard.h"

bool TrainBehaviors::initialized=false;

void TrainBehaviors::reset_intention() {
  //ERROR_OUT << "bs03 reset intention";
  noball_demo->reset_intention();
  learn_wball->reset_intention();
  Blackboard::init();
}

bool TrainBehaviors::get_cmd(Cmd & cmd) {
  bool cmd_set = false;
  //cout << "\nbs03 " << WSinfo::ws->time;

  if(WSinfo::ws->time_of_last_update!=WSinfo::ws->time) 
    LOG_POL(0,<<"WARNING - did NOT get SEE update!");

  LOG_POL(0, << "In TRAIN_BEHAVIORS_BMC : ");

  if(!ClientOptions::consider_goalie) {
    switch(WSinfo::ws->play_mode) {
    case PM_his_BeforePenaltyKick:
    case PM_his_PenaltyKick:
      cmd.cmd_main.set_turn(0);
      cmd_set=true;
      break;
    case PM_my_BeforePenaltyKick: {
      int resp_player = (WSinfo::ws->penalty_count) % NUM_PLAYERS + 1;
      if(resp_player!=WSinfo::me->number) {
	if(WSinfo::is_ball_pos_valid() && (WSinfo::ball->pos-WSinfo::me->pos).norm()<8) {
	  Vector target;
	  target.init_polar(10.0,(WSinfo::ball->pos-WSinfo::me->pos).ARG());
	  target+=WSinfo::ball->pos;
	  go2pos->set_target(target);
	  go2pos->get_cmd(cmd);
	  cmd_set=true;
	} else {
	  cmd.cmd_main.set_turn(0);
	  cmd_set=true;
	}
      } else {
	if(!WSinfo::is_ball_pos_valid()) {
	  faceball->turn_to_ball();
	  faceball->get_cmd(cmd);
	  cmd_set = true;
	} else {
	  if(!WSinfo::is_ball_kickable()) {
	    go2pos->set_target(WSinfo::ball->pos);
	    go2pos->get_cmd(cmd);
	    cmd_set=true;
	  } else {
	    cmd.cmd_main.set_turn(-WSinfo::me->ang.get_value());
	    cmd_set=true;
	  }
	}
      }
      break;
    }
    case PM_my_PenaltyKick: {
      Blackboard::need_goal_kick=false; // just to be sure!
      do_standard_kick=false;
      int resp_player = (WSinfo::ws->penalty_count) % NUM_PLAYERS + 1;
      if(resp_player!=WSinfo::me->number) {
	cmd.cmd_main.set_turn(0);
	cmd_set = true;
	break;
      }
    }
    case PM_PlayOn:
      if(Blackboard::need_goal_kick) {
	cmd_set = goalkick->get_cmd(cmd);
      }
      if(!cmd_set) {
	Blackboard::need_goal_kick = false;
	if(WSinfo::is_ball_pos_valid() && WSinfo::is_ball_kickable()) { // Ball is kickable
	  if(do_standard_kick == true) //Ball is kickable and I started a standardsit
	    cmd_set = standardSit->get_cmd(cmd);
	  else
	    cmd_set =  learn_wball->get_cmd(cmd);
	} else{    // Ball is not kickable
	  do_standard_kick = false; // just to be sure...
	  if (noball_demo->amIAttacker()) {
	    cmd_set = noball_demo->get_cmd(cmd);
	  } else {
	    cmd_set = noball_demo->get_cmd(cmd);
	  }
	}
      }
      break;
    case PM_my_GoalKick:
    case PM_my_GoalieFreeKick:
      cmd_set = goalkick->get_cmd(cmd);break;
    case PM_my_KickIn:
    case PM_his_KickIn:
    case PM_my_CornerKick:
    case PM_his_CornerKick:
    case PM_my_FreeKick:
    case PM_his_FreeKick:
    case PM_my_OffSideKick:
    case PM_his_OffSideKick:
    case PM_my_KickOff:
      cmd_set = standardSit->get_cmd(cmd);
      if(WSinfo::is_ball_pos_valid() && WSinfo::is_ball_kickable()) { // Ball is kickable
	do_standard_kick = true;
      }
      break;
    case PM_my_BeforeKickOff:
    case PM_his_BeforeKickOff:
    case PM_my_AfterGoal:
    case PM_his_AfterGoal:
    case PM_Half_Time:

      //sput03: standard policy does never look to ball...
      cmd_set = line_up->get_cmd(cmd);
      /*
	if((WSinfo::me->pos-DeltaPositioning::get_position(WSinfo::me->number)).sqr_norm()>5*5)
	return false;  // first let standard policy move to homepos
	faceball->turn_to_ball();
	cmd_set = faceball->get_cmd(cmd);
      */
      break;
    default:
      ERROR_OUT << "time " << WSinfo::ws->time << " player nr. " << WSinfo::me->number 
		<< " play_mode is " << WSinfo::ws->play_mode << " no command was set by behavior";
      return false;  // behaviour is currently not responsible for that case
    }
  } else {
    LOG_POL(0, << "In BS03_BMC [goalie mode]: ");
    cmd_set = goalie_bs03->get_cmd(cmd);
    /*
      switch(WSinfo::ws->play_mode) {
      case PM_PlayOn:
      case PM_my_FreeKick:
      case PM_his_FreeKick:
      case PM_my_KickIn:
      case PM_his_KickIn:
      case PM_my_CornerKick:
      case PM_his_CornerKick:
      case PM_my_OffSideKick:
      case PM_his_OffSideKick:
      if(Blackboard::need_goal_kick) {
      cmd_set = goalkick->get_cmd(cmd);
      }
      if(!cmd_set) {
      Blackboard::need_goal_kick = false;
       cmd_set = goalie03->get_cmd(cmd);
       //return false; // no behavior for goalie yet
       }
       break;
       case PM_my_GoalKick:
       case PM_my_GoalieFreeKick:
       cmd_set = goalkick->get_cmd(cmd);
       break;
       default:
       return false;
      }*/
  }
  
  Angle angle;
  Value power, x, y;

  switch( cmd.cmd_main.get_type()) {
  case Cmd_Main::TYPE_KICK:
    cmd.cmd_main.get_kick(power, angle);
    LOG_POL(0, << "bs03_bmc: cmd KICK, power "<<power<<", angle "<< RAD2DEG(angle) );
    break;
  case Cmd_Main::TYPE_TURN:
    cmd.cmd_main.get_turn(angle);
    LOG_POL(0, << "bs03_bmc: cmd Turn, angle "<< RAD2DEG(angle) );
    break;
  case Cmd_Main::TYPE_DASH:
    cmd.cmd_main.get_dash(power);
    LOG_POL(0, << "bs03_bmc: cmd DASH, power "<< (power) );
    break;
  case Cmd_Main::TYPE_CATCH:
    cmd.cmd_main.get_catch(angle);
    LOG_POL(0, << "bs03_bmc: cmd Catch, angle "<< RAD2DEG(angle) );
    break;
  case Cmd_Main::TYPE_TACKLE:
    cmd.cmd_main.get_tackle(power);
    LOG_POL(0, << "bs03_bmc: cmd Tackle, power "<< power );
    break;
  case Cmd_Main::TYPE_MOVETO:
    cmd.cmd_main.get_moveto(x, y);
    LOG_POL(0, << "bs03_bmc: cmd Moveto, target "<< x << " " << y );
  default:
    LOG_POL(0, << "bs03_bmc: No CMD was set " );
  }

  LOG_POL(0, << "Out BS03_BMC : intention was set "<<cmd_set);
  return cmd_set;  
}
