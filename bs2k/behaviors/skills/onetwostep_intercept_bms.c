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

#include "onetwostep_intercept_bms.h"
#include "tools.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"

#define BASELEVEL 3

bool OneTwoStep_Intercept::initialized=false;

//Value OneTwoStep_Intercept::step1success_sqrdist;
//Value OneTwoStep_Intercept::step2success_sqrdist;


#if 0
#define PROT(XXX)(cout<<XXX)
#define MYLOG_POL(XXX,YYY) LOG_POL(XXX,YYY);
#else
#define MYLOG_POL(XXX,YYY)
#define PROT(XXX)
#endif

#if 0 // ridi: considerd harmful before Lisbon, probably rechange
#define onestep_safety_margin  0.
#define twostep_safety_margin  0.
#endif

#define onestep_safety_margin  0.04
#define twostep_safety_margin  0.15


ANGLE OneTwoStep_Intercept::my_angle_to(const Vector & my_pos, const ANGLE &my_angle, 
				 Vector target){
  ANGLE result;
  target -= my_pos;
  result = target.ARG() - my_angle;
  return result;
}


OneTwoStep_Intercept::OneTwoStep_Intercept(){
    //    step1success_sqrdist = SQUARE(WSinfo::me->kick_radius - onestep_safety_margin);
    //step2success_sqrdist = SQUARE(WSinfo::me->kick_radius - twostep_safety_margin);
    step1success_sqrdist = SQUARE(ServerOptions::kickable_area - onestep_safety_margin);
    step2success_sqrdist = SQUARE(ServerOptions::kickable_area - twostep_safety_margin);
}


bool OneTwoStep_Intercept::init(char const * conf_file, int argc, char const* const* argv){
    if(initialized) return true;
    initialized = true;
    INFO_OUT << "\n1or2step_Intercept behavior initialized.";
   return true;
}

bool OneTwoStep_Intercept::get_1step_cmd(Cmd & cmd, Value &sqrdist, const MyState &state){
  MyState next_state;
  Cmd_Main stepone_action;
  OneTwoStepInterceptItrActions itr_actions;
  Value best1step_sqrdist = step1success_sqrdist;
  Value best1step_dashpower = 10000;
  Value surely_save = WSinfo::me->kick_radius - 0.3;
  Value power, angle;

  itr_actions.reset();
  while ( Cmd_Main * action = itr_actions.next() ) {
    Tools::get_successor_state(state,*action,next_state);
    //MYLOG_POL(0,<<"1Icpt: Test action "<<*action<<" successor "<<next_state);    
    Value sqrdist = next_state.my_pos.sqr_distance(next_state.ball_pos);

    if(sqrdist < best1step_sqrdist){ // this action improves current best
      if(best1step_sqrdist > SQUARE(surely_save)){// optimum not reached yet, so take action!
	best1step_sqrdist = sqrdist;
	stepone_action = *action;
	PROT("1Icpt: Wow! One step action improves dist "<<stepone_action
	     <<" new square dist: "<<best1step_sqrdist<<endl);
      }
      if(best1step_sqrdist <= SQUARE(surely_save)){ // do finetuning of actions
	if((action->get_type(power,angle) == Cmd_Main::TYPE_DASH)){
	  if((fabs(power) < best1step_dashpower)){
	    best1step_sqrdist = sqrdist;
	    stepone_action = *action;
	    best1step_dashpower = fabs(power);
	    PROT("1Icpt: Fine Improvement SUCCESS! Found: "<<stepone_action
		 <<" new square dist: "<<best1step_sqrdist<<endl);
	  }
	}
	else if((action->get_type(power,angle) == Cmd_Main::TYPE_TURN)){
	  // Hey, I can get the Ball without dashing, so let's turn toward opponent goal
	  Vector const opponent_goalpos(52.5,0.);
	  Angle turn2goal_angle = (opponent_goalpos-state.my_pos).arg()-state.my_angle.get_value();
	  turn2goal_angle = Tools::get_angle_between_mPI_pPI(turn2goal_angle);
	  turn2goal_angle=turn2goal_angle*(1.0+(WSinfo::me->inertia_moment*
						(state.my_vel.norm())));
	  if (turn2goal_angle > 3.14) turn2goal_angle = 3.14;
	  if (turn2goal_angle < -3.14) turn2goal_angle = -3.14;
	  turn2goal_angle = Tools::get_angle_between_null_2PI(turn2goal_angle);
	  stepone_action.unset_lock();
	  stepone_action.set_turn(turn2goal_angle);
	  PROT("1Icpt: Turn is safe! "<<stepone_action
	       <<" new square dist: "<<best1step_sqrdist<<" -> Turn2goal "<<RAD2DEG(turn2goal_angle)<<endl);
	  break;
	}

      }
      if (best1step_sqrdist <= SQUARE(surely_save)){// action is safe, give other actions a chance
	PROT("1Icpt: YEAH, current best action is surely save!"<<endl);
	best1step_sqrdist = SQUARE(surely_save-0.01);
      }
    }
  }
  //  PROT(endl);

  if (best1step_sqrdist < step1success_sqrdist){ // successful!
    cmd.cmd_main.unset_lock();
    cmd.cmd_main.clone( stepone_action );
    sqrdist = best1step_sqrdist;
    return true;
  }
  return false;
}

bool OneTwoStep_Intercept::get_1step_cmd_virtual(Value &step2sqrdist, Value &step3sqrdist, const MyState &state){
  MyState next_state;
  OneTwoStepInterceptItrActions itr_actions;
  Value best2step_sqrdist = step1success_sqrdist;
  Value best3step_sqrdist = step1success_sqrdist;

  itr_actions.reset();
  while ( Cmd_Main * action = itr_actions.next() ) {
    Tools::get_successor_state(state,*action,next_state);
    Value sqrdist = next_state.my_pos.sqr_distance(next_state.ball_pos);
    if(sqrdist >step1success_sqrdist){ // second chance -> what happens, if I just wait for ball???
      Vector new_my_pos,  new_ball_pos;      
      new_my_pos = next_state.my_pos + next_state.my_vel;     //me
      new_ball_pos = next_state.ball_pos + next_state.ball_vel; //ball

      sqrdist =new_my_pos.sqr_distance(new_ball_pos);
      if(sqrdist <= best3step_sqrdist){ // this action improves current best
	best3step_sqrdist = sqrdist;	
      }
      continue;
    }

    if(sqrdist < best2step_sqrdist){ // this action improves current best
      best2step_sqrdist = sqrdist;
    }
  }
  
  if (best2step_sqrdist < step1success_sqrdist || best3step_sqrdist < step1success_sqrdist ){ // successful!
    step2sqrdist = best2step_sqrdist;
    step3sqrdist = best3step_sqrdist;
    return true;
  }
  return false;
}



bool OneTwoStep_Intercept::get_2step_cmd(Cmd & cmd, Value &sqrdist,int & steps, const MyState &state){

  MyState next_state;
  Cmd_Main steptwo_action,stepthree_action;
  OneTwoStepInterceptItrActions itr_actions;
  Value best2step_sqrdist = step2success_sqrdist;
  Value best3step_sqrdist = step2success_sqrdist;
  Value step1sqrdist, step2sqrdist, step3sqrdist;
  Cmd cmd_tmp;

  if(get_1step_cmd(cmd_tmp, step1sqrdist, state) == true){
    LOG_MOV(0,<<"OneTwoIntercept: 1StepCmd Found -> Done.");
    steps =1;
    sqrdist = step1sqrdist;
    cmd.cmd_main.clone(cmd_tmp.cmd_main);
    return true;
  }

  itr_actions.reset();
  while ( Cmd_Main * action = itr_actions.next() ) {
    Tools::get_successor_state(state,*action,next_state);
    //PROT("2Icpt: Test action "<<*action<<" successor "<<next_state<<endl);    
    if(get_1step_cmd_virtual(step2sqrdist, step3sqrdist, next_state) == true){
      PROT("2Icpt: There is a one step command from the successor state"<<endl);
      if(step2sqrdist < best2step_sqrdist){
	best2step_sqrdist = step2sqrdist;
	steptwo_action = *action;
	PROT("2Icpt: Wow! Two step action improves dist "<<steptwo_action
	     <<" new square dist: "<<best2step_sqrdist<<endl);
      }
      if(step3sqrdist < best3step_sqrdist){
	best3step_sqrdist = step3sqrdist;
	stepthree_action = *action;
	PROT("2Icpt: Wow! Two step action improves dist "<<steptwo_action
	     <<" new square dist: "<<best2step_sqrdist<<endl);
      }
    }
  }
  //  PROT(endl);

  if (best2step_sqrdist <step2success_sqrdist){ // successful!
    LOG_MOV(0,<<"OneTwoIntercept: 2StepCmd Found -> 2 steps 2 go.");
    cmd.cmd_main.clone( steptwo_action );
    sqrdist = best2step_sqrdist;
    steps = 2;
    return true;
  }
  if (best3step_sqrdist <step2success_sqrdist){ // successful!
    LOG_MOV(0,<<"OneTwoIntercept:  3StepCmd Found -> 3 steps 2 go.");
    cmd.cmd_main.clone( stepthree_action );
    sqrdist = best3step_sqrdist;
    steps = 3;
    return true;
  }
  return false;
}



bool OneTwoStep_Intercept::get_cmd(Cmd & cmd, const MyState &state){
  int steps;

  return(get_cmd(cmd,state,steps));
}

bool OneTwoStep_Intercept::get_cmd(Cmd & cmd, const MyState &state, int &steps){
  Value dist;

  step1success_sqrdist = SQUARE(WSinfo::me->kick_radius - onestep_safety_margin);
  step2success_sqrdist = SQUARE(WSinfo::me->kick_radius - twostep_safety_margin);

  if(state.my_pos.sqr_distance(state.ball_pos) > SQUARE(2*(state.ball_vel.norm() +
							WSinfo::me->speed_max) +
							WSinfo::me->kick_radius)){
    PROT("Check intercept in "<<2<<" steps "<<" Ball too far "<<
	 state.my_pos.distance(state.ball_pos)<<endl);
    return false;
  }

  return(get_2step_cmd(cmd, dist,steps, state));
}

bool OneTwoStep_Intercept::get_cmd(Cmd & cmd){
  int steps;
  return get_cmd(cmd,steps);
}

bool OneTwoStep_Intercept::get_cmd(Cmd & cmd, int &steps){
  MyState state;
  
  state.my_pos = WSinfo::me->pos;
  state.my_vel = WSinfo::me->vel;
  state.ball_pos = WSinfo::ball->pos;
  state.ball_vel = WSinfo::ball->vel;
  state.my_angle = WSinfo::me->ang;

  return get_cmd(cmd,state,steps);

}

