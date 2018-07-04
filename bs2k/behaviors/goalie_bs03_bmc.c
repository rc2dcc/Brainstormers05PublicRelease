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

/** \file strategy_goalie.c
    The strategy for the goalie is defined here.
*/

#include "goalie_bs03_bmc.h"
#include "log_macros.h"
#include "../policy/policy_tools.h"
#include "valueparser.h"
#include "options.h"
#include "blackboard.h"

bool Goalie_Bs03::i_have_ball = false;
int Goalie_Bs03::intercept_ban = 0;
int Goalie_Bs03::INTERCEPT_BAN_STEPS = 0;
int Goalie_Bs03::catch_ban = 0;
int Goalie_Bs03::time_flag = 0;

bool Goalie_Bs03::initialized = false;

Goalie_Bs03::Goalie_Bs03(){
  ValueParser parser( CommandLineOptions::strategy_conf, "Goalie_Strategy" );
  parser.get("intercept_ban_steps", Goalie_Bs03::INTERCEPT_BAN_STEPS);

  goalie03 = new Goalie03;
  goalkick = new GoalKick;
  line_up = new LineUp;
  go2pos = new NeuroGo2Pos;
}

void Goalie_Bs03::reset_intention() {
  goalie03->reset_intention();
}

bool Goalie_Bs03::get_cmd(Cmd &cmd) {
  bool cmd_set = false;
  switch(WSinfo::ws->play_mode) {
  case PM_his_PenaltyKick:
    LOG_POL(0,<<"PM_his_PenaltyKick: I am goalie");
    Blackboard::need_goal_kick=false;
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
  case PM_my_BeforeKickOff:
  case PM_my_KickOff:
  case PM_his_BeforeKickOff:
  case PM_his_KickOff:
    return line_up->get_cmd(cmd);
    //return false;
    //policy = Policy_Factory::get_Line_Up_Policy();
    break;
  case PM_his_BeforePenaltyKick:{
    LOG_POL(0,<<"PM_his_BeforePenaltyKick: I am goalie");
    go2pos->set_target(Vector(-50,0));
    go2pos->get_cmd(cmd);
    cmd_set = true;
    break;
  }
  case PM_my_PenaltyKick:
  case PM_my_BeforePenaltyKick:
    LOG_POL(0,<<"PM_my_(Before)PenaltyKick: I am goalie");
    go2pos->set_target(Vector(55,20));
    go2pos->get_cmd(cmd);
    cmd_set = true;
    break;
  default:
    return false;
  }

  return cmd_set;  
}
