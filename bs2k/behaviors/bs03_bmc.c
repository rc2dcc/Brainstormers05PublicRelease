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

#include "bs03_bmc.h"

#include "ws_info.h"
#include "ws_memory.h"
#include "log_macros.h"
#include "tools.h"
#include "blackboard.h"

#define OOTRAP 0
//#define OOTRAP 1
//#define USE_SCORE04 0
#define USE_SCORE04 1

bool Bs03::initialized=false;
int  Bs03::cvHisOffsideCounter=0;

bool
Bs03::init(char const * conf_file, int argc, char const* const* argv) 
{
  if ( initialized )
    return true;

  initialized= true;

  bool returnValue = true;

  returnValue &= NoballDemo::init(conf_file,argc,argv);
  returnValue &= Attack_Move1_Wb::init(conf_file,argc,argv);
  returnValue &= WballDemo::init(conf_file,argc,argv);
  returnValue &= Score04::init(conf_file,argc,argv);
  returnValue &= GoalKick::init(conf_file,argc,argv);
  returnValue &= FaceBall::init(conf_file,argc,argv);
  returnValue &= StandardSituation::init(conf_file,argc,argv);
  returnValue &= Goalie_Bs03::init(conf_file, argc, argv);
  returnValue &= NeuroGo2Pos::init(conf_file, argc, argv);
  returnValue &= LineUp::init(conf_file, argc, argv);
  returnValue &= OvercomeOffsideTrap::init(conf_file, argc, argv);   //hauke

  return returnValue;
}



void Bs03::reset_intention() {
  //ERROR_OUT << "bs03 reset intention";
  noball_demo->reset_intention();
  wball_demo->reset_intention();
  penalty->reset_intention();
#if 0
  score04->reset_intention();
#endif
  Blackboard::init();
//  ootrap->reset_intention();
}

void Bs03::select_relevant_teammates()
{ // player dependend
  switch(WSinfo::me->number)
  {
  case 2:
    if(WSmemory::team_last_at_ball() != 0){ // we defend!
      WSinfo::set_relevant_teammates(4,3,6,7,1);  //TG: old:  3 4 7 6 1
    }
    else 
      WSinfo::set_relevant_teammates(6,7,3,9);
    break;
  case 3:
    if(WSmemory::team_last_at_ball() != 0){ // we defend!
      WSinfo::set_relevant_teammates(4,2,5,7,1);  //TG: old: 2 4 7 1
    }
    else 
    WSinfo::set_relevant_teammates(6,7,2,4);
    break;
  case 4:
    if(WSmemory::team_last_at_ball() != 0){ // we defend!
      WSinfo::set_relevant_teammates(1,3,2,5,7);  //TG: old: 3 5 7 1
    }
    WSinfo::set_relevant_teammates(8,7,5,3);
    break;
  case 5:
    if(WSmemory::team_last_at_ball() != 0){ // we defend!
      WSinfo::set_relevant_teammates(4,3,8,7,1); //TG: old: 4 3 7 8 1
    }
    WSinfo::set_relevant_teammates(8,7,4,11);
    break;
  case 6:
    if(WSinfo::me->pos.x > 20) 
      WSinfo::set_relevant_teammates(9,10,7);
    else
      WSinfo::set_relevant_teammates(9,10,7,4,3,2); //TG: old: 9 10 7 3 2
    break;
  case 7:
    if(WSinfo::me->pos.x > 20) 
      WSinfo::set_relevant_teammates(10,6,8,9,11); // care only for attack players 
    else  // default attention
      WSinfo::set_relevant_teammates(10,6,8,9,11,4,3); //TG: old: 10 6 8 9 11 3 4
    break;
  case 8:
    if(WSinfo::me->pos.x > 20) 
      WSinfo::set_relevant_teammates(11,10,7);
    else
      WSinfo::set_relevant_teammates(11,10,7,4,5,3);  //TG: old: 11 10 7 5 4
    break;
  case 9:
    WSinfo::set_relevant_teammates(10,6,7,11,8);
    break;
  case 10:
    WSinfo::set_relevant_teammates(11,9,7,6,8);
    break;
  case 11:
    WSinfo::set_relevant_teammates(10,8,7,9,6);
    break;
  }
}


bool 
Bs03::get_cmd(Cmd & cmd) 
{
  bool cmd_set = false;
  //cout << "\nbs03 " << WSinfo::ws->time;
LOG_POL(0,<<"BLACKBOARD::main_intention.get_type()=="<<Blackboard::main_intention.get_type());
LOG_POL(0,<<"BLACKBOARD::pass_intention.get_type()=="<<Blackboard::pass_intention.get_type());

  long start_bs03 = Tools::get_current_ms_time();
  LOG_POL(0,<<"Entered BS03 "<<start_bs03 - WSinfo::ws->ms_time_of_sb<<" ms after sense body");

  select_relevant_teammates();

  if(WSinfo::ws->time_of_last_update!=WSinfo::ws->time) 
    LOG_POL(0,<<"WARNING - did NOT get SEE update!");

  WSinfo::visualize_state();
  int resp_player=0;
  if(WSinfo::ws->play_mode==PM_my_BeforePenaltyKick || WSinfo::ws->play_mode==PM_my_PenaltyKick)
    resp_player = 12-((WSinfo::ws->penalty_count) % NUM_PLAYERS);
  if(resp_player==12)
    resp_player=1;

  if(!ClientOptions::consider_goalie || ((WSinfo::ws->play_mode==PM_my_BeforePenaltyKick || WSinfo::ws->play_mode==PM_my_PenaltyKick) && resp_player==WSinfo::me->number)) 
  {
   switch(WSinfo::ws->play_mode) 
    {
      case PM_his_BeforePenaltyKick:
      case PM_his_PenaltyKick:
      {
            Vector dir = Vector(6,0);
            dir.rotate(2*PI/10*(WSinfo::me->number-1));
            LOG_POL(0,<<"PM_his_(Before)PenaltyKick: I am passive"<<dir);
            go2pos->set_target(Vector(0,0)+dir);
            go2pos->get_cmd(cmd);
            cmd_set=true;
            break;
      }
      case PM_my_BeforePenaltyKick: 
      {
        if(resp_player!=WSinfo::me->number) 
        {
   //if(WSinfo::is_ball_pos_valid() && (WSinfo::ball->pos-WSinfo::me->pos).norm()<8) {
   // Vector target;
   // target.init_polar(10.0,(WSinfo::ball->pos-WSinfo::me->pos).ARG());
   // target+=WSinfo::ball->pos;
          Vector dir = Vector(6,0);
          dir.rotate(2*PI/10*(WSinfo::me->number-1));
          LOG_POL(0,<<"PM_my_BeforePenaltyKick: I am passive"<<dir);
          go2pos->set_target(Vector(0,0)+dir);
          go2pos->get_cmd(cmd);
          cmd_set=true;
  //} else {
  //  cmd.cmd_main.set_turn(0);
  //  cmd_set=true;
  //}
        } 
        else 
        {
          LOG_POL(0,<<"PM_my_BeforePenaltyKick: I am ACTIVE!");
          penalty->reset_intention();
          if(!WSinfo::is_ball_pos_valid()) 
          {
            faceball->turn_to_ball();
            faceball->get_cmd(cmd);
            cmd_set = true;
          } 
          else
          {
            if(!WSinfo::is_ball_kickable()) 
            {
              go2pos->set_target(WSinfo::ball->pos);
              go2pos->get_cmd(cmd);
              cmd_set=true;
            } 
            else 
            {
              cmd.cmd_main.set_turn(-WSinfo::me->ang.get_value());
              cmd_set=true;
            }
          }
        }
        break;
      }
      case PM_my_PenaltyKick: 
      {
        Blackboard::need_goal_kick=false; // just to be sure!
        do_standard_kick=false;
        if(resp_player!=WSinfo::me->number) 
        {
          LOG_POL(0,<<"PM_my_PenaltyKick: I am passive");
          cmd.cmd_main.set_turn(0);
          cmd_set = true;
          break;
        }
        else
        {
          LOG_POL(0,<<"PM_my_PenaltyKick: I am ACTIVE!");
          if (!penalty->get_cmd(cmd))
          {
            if(!WSinfo::is_ball_pos_valid()) 
            {
              faceball->turn_to_ball();
              faceball->get_cmd(cmd);
              cmd_set = true;
            } 
            else 
            {
              go2pos->set_target(WSinfo::ball->pos);
              go2pos->get_cmd(cmd);
              cmd_set=true;
            }
          }
          cmd_set = true;
          break;
        }
      }
      case PM_PlayOn:
        if(Blackboard::need_goal_kick) 
        {
          cmd_set = goalkick->get_cmd(cmd);
        }
        if(!cmd_set) 
        {
          Blackboard::need_goal_kick = false;

#if 0 	  // ridi 05: deactivated before Osaka
          bool we_control=false;
          if(WSinfo::is_ball_pos_valid())
          {
            WSpset team=WSinfo::alive_teammates;
            team.keep_and_sort_closest_players_to_point(1,WSinfo::ball->pos);
            if(team.num>0)
            if(WSinfo::is_ball_kickable_for(team[0]))
              we_control=true;
          }
          if((!do_standard_kick)&&(WSinfo::ball->pos.distance(Vector(52.5,0.0))<20.0)&&(WSinfo::me->number>8)&&(we_control))
          {
            cmd_set = score04->get_cmd(cmd);
            if(cmd_set)
            break;
          }
#endif
          if(WSinfo::is_ball_pos_valid() && WSinfo::is_ball_kickable()) 
          { // Ball is kickable
            if(do_standard_kick == true) //Ball is kickable and I started a standardsit
              cmd_set = standardSit->get_cmd(cmd);
#if 0
            else 
              if (Attack_Move1_Wb::do_move()) 
              {
                cmd_set = attack_move1_wb->get_cmd(cmd);
              }
#endif
#if 0 // ridi: deactivate before osaka; time restriction
            else 
              if (ootrap->test_wb(cmd))
              {
                if (ootrap->get_cmd(cmd)) 
                  cmd_set =true; //hauke
                else  
                  cmd_set = wball_demo->get_cmd(cmd);   //hauke
              }
#endif
          else
              cmd_set =  wball_demo->get_cmd(cmd);
          } // end ball is kickable
          else // Ball is not kickable
          {    
            do_standard_kick = false; // just to be sure...
            if(0)
            { // ridi: just used to start  else if structure ...
            }
#if OOTRAP
            else 
              if (ootrap->test_nb(cmd))
              {
                if (ootrap->get_cmd(cmd)) 
                  cmd_set =true; //hauke
                else  
                  cmd_set = noball_demo->get_cmd(cmd); //hauke
              }
#endif
            else 
              //make use of your own noball behavior here,
              //e.g. one for attacking and a different one for defending
              if (
                   noball_demo->amIAttacker()
                 ) 
              {
                cmd_set = noball_demo->get_cmd(cmd);
              }
              else 
              {
                cmd_set = noball_demo->get_cmd(cmd);
              }

          } // end of: ball is not kickable
      } // end cmd is not set
      break;
    case PM_my_GoalieFreeKick:
      cmd_set = goalkick->get_cmd(cmd);break;
    case PM_his_GoalKick:
    case PM_his_GoalieFreeKick: 
    {
      go2pos->set_target(DeltaPositioning::get_position(WSinfo::me->number));
      //go2pos->set_target(target);
      cmd_set=go2pos->get_cmd(cmd);
      break;
    }
    case PM_my_GoalKick:
    case PM_my_KickIn:
    case PM_his_KickIn:
    case PM_my_CornerKick:
    case PM_his_CornerKick:
    case PM_my_FreeKick:
    case PM_his_FreeKick:
    case PM_my_OffSideKick:
    case PM_his_OffSideKick:
      if (   WSinfo::ws->play_mode == PM_his_OffSideKick
          && ivLastOffSideTime != WSinfo::ws->time
          && ivLastOffSideTime + 200 < WSinfo::ws->time
         )
      { 
        cvHisOffsideCounter ++ ;
        ivLastOffSideTime = WSinfo::ws->time;  
        LOG_POL(0,<<"WE ARE IN OFFSIDE");
      }
    case PM_my_KickOff:
      cmd_set = standardSit->get_cmd(cmd);
      if(WSinfo::is_ball_pos_valid() && WSinfo::is_ball_kickable()) 
      { // Ball is kickable
        do_standard_kick = true;
      }
      break;
    case PM_my_BeforeKickOff:
    case PM_his_BeforeKickOff:
    case PM_my_AfterGoal:
    case PM_his_AfterGoal:
    case PM_Half_Time:
      //sput03: standard policy does never look to ball...
      cmd_set = line_up->get_cmd(cmd, true);//true==sector-based scanning
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
  } 
  else 
  {
    LOG_POL(0, << "In BS03_BMC [goalie mode]: ");
    if(WSinfo::ws->play_mode == PM_my_GoalKick)
      cmd_set = standardSit->get_cmd(cmd);
    else
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


  LOG_POL(0,<<"BS03. Decision needed "<<Tools::get_current_ms_time() - start_bs03<<" ms"<<std::flush);

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
    LOG_POL(0, << "bs03_bmc: No CMD was set " << std::flush);
  }


  Vector newmypos,newmyvel,newballpos,newballvel;
  ANGLE newmyang;

  long nowtime = Tools::get_current_ms_time();
  LOG_POL(0,<<"BS03. Alltogehter needed "<<nowtime - start_bs03
    <<" ms Finishing "<<  nowtime - WSinfo::ws->ms_time_of_sb<<" ms after sense body"<<std::flush);

  LOG_POL(0, << "Out BS03_BMC : intention was set "<<cmd_set<<std::flush);
  return cmd_set;  
}
