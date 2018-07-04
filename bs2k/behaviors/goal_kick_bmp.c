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

#include "goal_kick_bmp.h"
#include "tools.h"
#include "options.h"
#include "ws_info.h"
#include "mdp_info.h"
#include "blackboard.h"
#include "log_macros.h"
#include "valueparser.h"
#include "../policy/positioning.h"
#include "../policy/policy_tools.h"
#include "../policy/planning.h"
#include <math.h>

#define LB 0
#define DBLOG_POL(LLL,XXX) LOG_POL(LLL,<<"GoalKick: "<<XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D<<XXX)
//#define DBLOG_POL(LLL,XXX) 
//#define DBLOG_DRAW(LLL,XXX)


#define GFK_LEFT_RECEIVER 6
#define GFK_RIGHT_RECEIVER 8
#define GFK_FINAL_VEL 0.8

int GoalKick::gfk_pos_num;
Vector GoalKick::gfk_kickoff_pos[MAX_GFK_POS];
Vector GoalKick::gfk_target_pos[MAX_GFK_POS];
Value GoalKick::gfk_kickvel[MAX_GFK_POS];

bool GoalKick::initialized=false;

Value GoalKick::homepos_tolerance;
int GoalKick::homepos_stamina_min;
int GoalKick::wait_after_catch;
int GoalKick::max_wait_after_catch;

GoalKick::GoalKick() {
  go2pos = new NeuroGo2Pos();
  basiccmd = new BasicCmd();
  faceball = new FaceBall();
  onestepkick = new OneStepKick();
  onetwokick = new OneOrTwoStepKick();
  neurokick = new NeuroKick05();
  intercept = new NeuroIntercept();
  last_called = -1;

  for(int i=0;i<MAX_GFK_POS;i++) {
    gfk_rand_arr[i]=i;
  }
}

bool GoalKick::init(char const * conf_file, int argc, char const* const* argv) {
  if(initialized) return true;
  bool res = NeuroGo2Pos::init(conf_file,argc,argv) &&
    BasicCmd::init(conf_file,argc,argv) &&
    FaceBall::init(conf_file,argc,argv) &&
    NeuroIntercept::init(conf_file,argc,argv) &&
    OneStepKick::init(conf_file,argc,argv) &&
    OneOrTwoStepKick::init(conf_file,argc,argv) &&
    NeuroKick05::init(conf_file,argc,argv);
    
  if(res) {
    // read params
    ValueParser vp(conf_file,"goal_kick_bmp");
    vp.set_verbose();
    vp.get("homepos_tolerance",homepos_tolerance,1);
    vp.get("homepos_stamina_min",homepos_stamina_min,1400);
    vp.get("wait_after_catch",wait_after_catch,40);
    vp.get("max_wait_after_catch",max_wait_after_catch,160);

    // read goalie free kick positions
    gfk_pos_num = -1;
    vp.get("gfk_pos_num",gfk_pos_num);
    if(gfk_pos_num<=0) {
      ERROR_OUT << "\nCould not read positions for goalie free kicks!";
      return false;
    }
    if(gfk_pos_num>.5*MAX_GFK_POS) {
      ERROR_OUT << "\ngfk_pos_num too big!";
      return false;
    }
    float posarr[5];
    char key[50];
    for(int i=0;i<gfk_pos_num;i++) {
      sprintf(key,"gfk_pos_%02d",i+1);
      if(vp.get(key,posarr,5)!=5) {
	ERROR_OUT << "Syntax error in goalie free kick position table?";
	return false;
      }
      gfk_kickoff_pos[2*i]=Vector(posarr[0],posarr[1]);
      gfk_target_pos[2*i]=Vector(posarr[2],posarr[3]);
      gfk_kickvel[2*i]=posarr[4];
      gfk_kickoff_pos[2*i+1]=Vector(posarr[0],-posarr[1]);
      gfk_target_pos[2*i+1]=Vector(posarr[2],-posarr[3]);
      gfk_kickvel[2*i+1]=posarr[4];
    }
    gfk_pos_num*=2;

    initialized = true;
    std::cout << "\nGoalKick behavior initialized.";
    return true;
  }
  return false;
}

bool GoalKick::get_cmd(Cmd & cmd){
  //LOG_POL(0,<<"GoalKick behavior called.");
  if(last_called != WSinfo::ws->time -1) {
    LOG_POL(LB+1,<<"GoalKick: Sequence starting...");
    seq_started = WSinfo::ws->time;
    play_on_cnt = 0;

    gk_goalie_mode = 0;
    gfk_goalie_mode = 0;
    move_to_home = true;
    mode=0;
    wing_not_possible=false;
    
    switch(WSinfo::ws->play_mode) {
    case PM_my_GoalieFreeKick: goal_kick_mode = GFK_MODE;break;
    case PM_my_GoalKick: goal_kick_mode = GK_MODE;break;
    }
  }
  
  last_called = WSinfo::ws->time;
  if(WSinfo::ws->play_mode == PM_PlayOn) play_on_cnt++;
  
  bool res;
  Blackboard::need_goal_kick = true;
  if(ClientOptions::consider_goalie) {
    res = get_goalie_cmd(cmd);
  } else {
    res = get_player_cmd(cmd);
  }
  // debug
  if(!cmd.cmd_main.is_cmd_set()) {
    cmd.cmd_main.set_turn(0);
  }

  //LOG_POL(0,<<"GoalKick: res = "<<res);
  //Blackboard::need_goal_kick = res;  // let this behavior decide when it does no longer need
                                     // to be called!
  if(!res) {
    //if(cmd.cmd_att.is_cmd_set()) {
    //  cmd.cmd_att.unset_lock();
    //  cmd.cmd_att.unset_cmd();
    //}
  }
  if(WSinfo::ws->play_mode == PM_PlayOn) {
    if(!ClientOptions::consider_goalie || !WSinfo::is_ball_kickable()) {
      Blackboard::need_goal_kick=false;
      //Daniel: don't return false and set a command!!!
      cmd.cmd_main.unset_lock();
      cmd.cmd_main.unset_cmd();
      return false;
    }
  }
  return res;
}

bool GoalKick::get_goalie_cmd(Cmd &cmd) {
  if(WSinfo::ws->play_mode == PM_PlayOn) {
    if((WSinfo::me->pos - WSinfo::ball->pos).norm()>3) {
      Blackboard::need_goal_kick = false;
      return false;
    }
  }
  switch(goal_kick_mode) {
  case GFK_MODE: return get_gk_goalie_cmd(cmd);
  case GK_MODE: return get_gk_goalie_cmd(cmd);
  }
  return false;
}

bool GoalKick::get_gk_goalie_cmd(Cmd &cmd) {
  long cyc = WSinfo::ws->time - seq_started;
  if(!WSinfo::is_ball_pos_valid() || WSinfo::ball->age>5) {
    faceball->look_to_ball();
    faceball->get_cmd(cmd);
    return true;
  }
  DBLOG_POL(0,"GOAL KICK");
  switch(gk_goalie_mode) {
  case 0: { // run to ball
    if(goal_kick_mode==GFK_MODE) {
      gk_goalie_mode++;
    } else {
      DBLOG_POL(0,"go to ball...");
      Vector ko_pos(-47,9.16);
      if(WSinfo::ball->pos.y<0) ko_pos.y*=-1;
      Vector target=ko_pos;
      if(ko_pos.y<0) target.y+=.6;else target.y-=.6;
      if((WSinfo::me->pos-target).sqr_norm()>SQUARE(.4) || !WSinfo::is_ball_kickable()) {
	DBLOG_POL(0,"dist to target: "<<(WSinfo::me->pos-target).norm());
	go2pos->set_target(target,.1);
	go2pos->get_cmd(cmd);
	return true;
      }
      if(WSinfo::me->vel.norm()<.05) gk_goalie_mode++;
      else return true;
    }
  }
    // no break
  case 1: // wait and see
    if(cyc<wait_after_catch-20) {
      return scan_field(cmd);
    }
    if(cyc<wait_after_catch-10) {
      DBLOG_POL(0,"ang: "<<Tools::my_angle_to(Vector(0,0)));
      //basiccmd->set_turn(-Tools::my_angle_to(Vector(0,0)).get_value_mPI_pPI());
      basiccmd->set_turn(-WSinfo::me->ang.get_value_mPI_pPI());
      basiccmd->get_cmd(cmd);
      return true;
    }
    gk_goalie_mode++;
  case 2: // decide: left or right wing?
    if(goal_kick_mode==GFK_MODE) {
      gk_left=(WSinfo::me->pos.y>0)?false:true;
    } else {
      gk_left=(WSinfo::me->pos.y<0)?false:true;
    }
    cyc_cnt=7;
    gk_goalie_mode++;
  case 3: {// communicate and wait for players
    Tools::force_highq_view();
    if(gk_left) Blackboard::pass_intention.set_pass(WSinfo::ball->pos,3.0,WSinfo::ws->time);
    else        Blackboard::pass_intention.set_pass(WSinfo::ball->pos,2.8,WSinfo::ws->time);
    PPlayer kplayer1=0,kplayer2=0;
    if(gk_left) {
      kplayer1=WSinfo::valid_teammates.get_player_by_number(2);
      kplayer2=WSinfo::valid_teammates.get_player_by_number(3);
    }
    else {
      kplayer1=WSinfo::valid_teammates.get_player_by_number(5);
      kplayer2=WSinfo::valid_teammates.get_player_by_number(4);
    }
    if((kplayer1 && kplayer2 && kplayer1->age>kplayer2->age) || (kplayer1 && !kplayer2)) 
      Tools::set_neck_request(NECK_REQ_LOOKINDIRECTION,Tools::my_abs_angle_to(kplayer1->pos));
    else if(kplayer2)
      Tools::set_neck_request(NECK_REQ_LOOKINDIRECTION,Tools::my_abs_angle_to(kplayer2->pos));
    
    if(--cyc_cnt<0) gk_goalie_mode++;
    break;
    }
  case 4: { // find kick
    Vector kpos,tpos;ANGLE kickdir;int tnum;
    Value kickspeed=2.5;
    if(goal_kick_mode==GK_MODE) kickspeed=ServerOptions::ball_speed_max;
    //DBLOG_POL(0,"goal kick mode="<<goal_kick_mode);
    if(goal_kick_mode==GFK_MODE && !wing_not_possible) {
      
      if(gk_left) {
	kpos=Vector(-42.50,18.60);
	kickdir=ANGLE(DEG2RAD(20));
	tnum=2;
      } else {
	kpos=Vector(-42.50,-18.60);
	kickdir=ANGLE(DEG2RAD(-20));
	tnum=5;
      }
      Vector interceptpos,playerpos;
      int advantage,number;
      PPlayer tplayer;
      if(!(tplayer=WSinfo::valid_teammates_without_me.get_player_by_number(tnum))) {
	tpos=Vector(-30.6,-26.0);if(gk_left) tpos.y*=-1;
      } else tpos=tplayer->pos;
      Tools::set_neck_request(NECK_REQ_LOOKINDIRECTION,(tpos-WSinfo::me->pos).arg());
      if(fabs(tpos.y)<22) {
	DBLOG_POL(0,"Still waiting for receiver");
	return true;
      }
      if(gk_left) tpos.y+=2;else tpos.y-=2;
      kickdir=(tpos-kpos).ARG();
      DBLOG_DRAW(0,"CIRCLE("<<tpos.x<<","<<tpos.y<<",2.0);");
      if(Planning::is_laufpass_successful(kpos,(float)kickspeed,(float)kickdir.get_value(),interceptpos,
					  advantage,number,playerpos)) {
	//DBLOG_DRAW(0,"CIRCLE("<<tpos.x<<","<<tpos.y<<",2.0);");
	DBLOG_POL(0,"Laufpass successful, starting move...");
	gk_goalie_mode++;
	cyc_cnt=2;
	target_pos=tpos;
	target_vel=kickspeed;
	basiccmd->set_move(kpos);
	basiccmd->get_cmd(cmd);
	return true;
      }
      if(fabs(tpos.y)>25.9 || cyc>=max_wait_after_catch) {
	DBLOG_POL(0,"Laufpass to wing NOT successful!");
	//std::cerr << "\nunsuccessful";
	wing_not_possible=true;
	if(gk_left) basiccmd->set_move(Vector(-32,9.16));
	else basiccmd->set_move(Vector(-32,-9.16));
	basiccmd->get_cmd(cmd);
	
	return true;
      }
      
    } else {
      Vector tpos;
      tpos=WSinfo::ball->pos;
      Vector kpos1,kpos2,kpos3;
      Vector kvel1,kvel2,kvel3;
      int sbo1,sbo2,sbo3;
      PPlayer kplayer;
      Value kickvel=ServerOptions::ball_speed_max;
      //kvel.init_polar(kickvel,(tpos-kpos).angle());
      if(gk_left) {
	kpos3=kpos2=kpos1=Vector(-45,33);
	sbo3=sbo2=sbo1=0;
	if((kplayer=WSinfo::valid_teammates.get_player_by_number(2))) {
	  kpos1=kplayer->pos;
	  kpos1.y=FIELD_BORDER_Y-5;
	}
	if((kplayer=WSinfo::valid_teammates.get_player_by_number(3))) {
	  kpos2=kplayer->pos;
	  kpos2.y=10;
	}
      } else {
	kpos3=kpos2=kpos1=Vector(-45,-33);
	if((kplayer=WSinfo::valid_teammates.get_player_by_number(5))) {
	  kpos1=kplayer->pos;
	  kpos1.y=-FIELD_BORDER_Y+5;
	}
	if((kplayer=WSinfo::valid_teammates.get_player_by_number(4))) {
	  kpos2=kplayer->pos;
	  kpos2.y=-10;
	}
      }
      kvel1.init_polar(kickvel,(tpos-kpos1).angle());
      kvel2.init_polar(kickvel,(tpos-kpos2).angle());
      kvel3.init_polar(ServerOptions::ball_speed_max,(tpos-kpos3).angle()); 
      
      //bestpos=kpos3;
      //bestvel=kvel3;
      
      WSpset pstmp=WSinfo::valid_teammates_without_me;
      pstmp+=WSinfo::valid_opponents;
      InterceptResult ires[22];
      //pstmp.keep_players_in_quadrangle(kpos,tpos,10,20);
      pstmp.keep_players_in_rectangle(Vector(-FIELD_BORDER_X,FIELD_BORDER_Y),Vector(0,-FIELD_BORDER_Y));
      pstmp.keep_and_sort_best_interceptors(22,kpos3,kvel3,ires);
      if(pstmp.num>0 && pstmp[0]->team==HIS_TEAM && ires[0].time<15) {
	kpos3=Vector(-52,-12);
	if(gk_left) kpos3.y*=-1;
	kvel3.init_polar(ServerOptions::ball_speed_max,(tpos-kpos3).angle());
      }
      bestpos=kpos3;
      bestvel=kvel3;
      pstmp.keep_and_sort_best_interceptors(22,kpos2,kvel2,ires);
      if(pstmp.num>0 && pstmp[0]->team==MY_TEAM) {
	sbo2=-1;
	for(int p=1;p<pstmp.num;p++) {
	  if(pstmp[p]->team==MY_TEAM) continue;
	  if(ires[p].time-ires[0].time>=4) {
	    sbo2=ires[p].time-ires[0].time;
	    //bestpos=kpos2;
	    //bestvel=kvel2;
	    break;
	  }
	  sbo2=0;
	  break;
	}
      }
      pstmp.keep_and_sort_best_interceptors(22,kpos1,kvel1,ires);
      if(pstmp.num>0 && pstmp[0]->team==MY_TEAM) {
	sbo1=-1;
	for(int p=1;p<pstmp.num;p++) {
	  if(pstmp[p]->team==MY_TEAM) continue;
	  if(ires[p].time-ires[0].time>=1) {
	    sbo1=ires[p].time-ires[0].time;
	    //bestpos=kpos1;
	    //bestvel=kvel1;
	    break;
	  }
	  sbo1=0;
	  break;
	}
      }
      DBLOG_POL(0,"sbo1: "<<sbo1<<" sbo2: "<<sbo2);
      if(sbo1==-1) {
	bestpos=kpos1;bestvel=kvel1;
      } else if((sbo1>0 && sbo1>=sbo2) || sbo1>5) {
	bestpos=kpos1;bestvel=kvel1;
      } else if(sbo2 != 0) {
	bestpos=kpos2;bestvel=kvel2;
      }
      gk_goalie_mode++;
      if(goal_kick_mode==GFK_MODE) {
	basiccmd->set_move(tpos);
	basiccmd->get_cmd(cmd);
	return true;
      }
    }
    break;
  }
    break;
  case 5:
    if(!wing_not_possible && goal_kick_mode==GFK_MODE) {
      if(WSinfo::is_ball_kickable()) {
	Blackboard::pass_intention.set_laufpass(target_pos,target_vel,
						WSinfo::ws->time);
	if(--cyc_cnt>0) return true;
	neurokick->kick_to_pos_with_initial_vel(target_vel,target_pos);
	neurokick->get_cmd(cmd);
	return true;
      }
      goal_kick_mode++;
    } else {
      Blackboard::pass_intention.set_laufpass(bestpos,bestvel.norm(),WSinfo::ws->time);
      if(WSinfo::is_ball_kickable()) {
	//if(cyc>wait_after_catch) {
	neurokick->kick_to_pos_with_initial_vel(bestvel.norm(),bestpos);
	neurokick->get_cmd(cmd);
	return true;
	//}
      }
      goal_kick_mode++;
    }
  case 6:  // go home
    go2pos->set_target(Vector(-48.70,0));
    go2pos->get_cmd(cmd);
    return true;
  }  return true;
}

bool GoalKick::get_player_cmd(Cmd &cmd) {
  long cyc = WSinfo::ws->time - seq_started;
  if(WSinfo::ws->my_goalie_number>0) {
    cmd.cmd_att.set_attentionto(WSinfo::ws->my_goalie_number);
  } else {
    // hack - assume that our goalie has number 1!
    cmd.cmd_att.set_attentionto(1);
  }
  Vector ballpos,ballvel;
  ANGLE dir;
  Value speed;
  PPlayer p= WSinfo::get_teammate_with_newest_pass_info();
  if(p) {
    Tools::force_highq_view();
    speed = p->pass_info.ball_vel.norm();
    dir = ANGLE(p->pass_info.ball_vel.arg());
    ballpos = p->pass_info.ball_pos;
    ballvel = p->pass_info.ball_vel;
    DBLOG_POL(0, "got pass info from player " 
	      << p->number  
	      << " a= " << p->pass_info.age
	      << " p= " << ballpos
	      << " v= " << p->pass_info.ball_vel
	      << " at= " << p->pass_info.abs_time
	      << " rt= " << p->pass_info.rel_time
	      <<" speed= "<<speed <<" dir "<<RAD2DEG(dir.get_value()));
    if(speed>2.9 && speed <= 3.1) {
      mode = 2;  // left wing
    } else if(speed>2.71 && speed<=2.9) {
      mode = 3;  // right wing
    } else {
      //InterceptResult ires[2];
      //WSpset pstmp=WSinfo::valid_teammates;
      //pstmp.keep_and_sort_best_interceptors(2,ballpos,ballvel,ires);
      //#if 0
      Vector ipos,ballpos,ballvel;
      if(Policy_Tools::check_go4pass(ipos,ballpos,ballvel)) {
	DBLOG_POL(0,"intercepting... "<<ipos);
	Cmd dumcmd;
	MyState state,nextstate;
	state.get_from_WS();
	intercept->set_virtual_state(ballpos,ballvel);
	intercept->get_cmd(dumcmd);
	
	//go2pos->set_target(ipos);
	//go2pos->get_cmd(dumcmd);
	
	mode=4;
	Tools::get_successor_state(state,dumcmd.cmd_main,nextstate);
	if(nextstate.my_pos.x<-35 && fabs(nextstate.my_pos.y)<21.5) {
	  Vector newpos=nextstate.my_pos;
	  if(newpos.x<-35) newpos.x=-35;
	  if(fabs(newpos.y)<21.5) if(newpos.y<0) newpos.y=-21.5;else newpos.y=21.5;
	  go2pos->set_target(newpos);
	  go2pos->get_cmd(cmd);
	  mdpInfo::set_my_intention(DECISION_TYPE_INTERCEPTBALL);	  
	  return true;
	}
	mdpInfo::set_my_intention(DECISION_TYPE_INTERCEPTBALL);
	cmd.cmd_main.clone(dumcmd.cmd_main);
	return true;
      }
      //#endif
    }
  }
  if(mode==4) {
    DBLOG_POL(0,"intercepting...");
    Cmd dumcmd;
    MyState state,nextstate;
    state.get_from_WS();
    intercept->get_cmd(dumcmd);
    Tools::get_successor_state(state,dumcmd.cmd_main,nextstate);
    if(nextstate.my_pos.x<-35 && fabs(nextstate.my_pos.y)<21.5) {
      Vector newpos=nextstate.my_pos;
      if(newpos.x<-35) newpos.x=-35;
      if(fabs(newpos.y)<21.5) if(newpos.y<0) newpos.y=-21.5;else newpos.y=21.5;
      go2pos->set_target(newpos);
      go2pos->get_cmd(cmd);
      mdpInfo::set_my_intention(DECISION_TYPE_INTERCEPTBALL);
      return true;
    }
    mdpInfo::set_my_intention(DECISION_TYPE_INTERCEPTBALL);
    cmd.cmd_main.clone(dumcmd.cmd_main);
    return true;
  }
  
  Vector mypos=WSinfo::me->pos;
  Vector homepos,targetpos;
  if(DeltaPositioning::get_role(WSinfo::me->number)==PT_DEFENDER) {
    homepos=DeltaPositioning::get_position(WSinfo::me->number);
  } else {
    homepos=DeltaPositioning::attack433.get_grid_pos(WSinfo::me->number);
  }
  targetpos=homepos;
  switch(mode) {
  case 0:  // init, go2pos, wait
    if((mypos-homepos).sqr_norm()>SQUARE(1)) 
    {
      if (WSinfo::me->stamina < 2500) return scan_field(cmd); //TG      
      go2pos->set_target(homepos,0.5);
      go2pos->get_cmd(cmd);
      return true;
    }
    break;
  case 2:  // left wing
    switch(WSinfo::me->number) {
    case 2:
      //targetpos.y=homepos.y+15;
      targetpos.y=FIELD_BORDER_Y-5;
      break;
    case 6:
      //targetpos.y=homepos.y+10;
      targetpos.y=FIELD_BORDER_Y-9;
      break;
    case 3:
      targetpos.y=12;
      break;
    }
    break;
  case 3:  // right wing
    switch(WSinfo::me->number) {
    case 5:
      //targetpos.y=homepos.y+15;
      targetpos.y=-FIELD_BORDER_Y+5;
      break;
    case 8:
      //targetpos.y=homepos.y+10;
      targetpos.y=-FIELD_BORDER_Y+9;
      break;
    case 4:
      targetpos.y=-12;
      break;
    }
    break;
  }
  DBLOG_POL(0,"mypos: "<<mypos<<" target: "<<targetpos);
  if((mypos-targetpos).sqr_norm()>SQUARE(1)) {
    go2pos->set_target(targetpos,0.5);
    go2pos->get_cmd(cmd);
    return true;
  }
  if(cyc<wait_after_catch-10) return scan_field(cmd);
  faceball->turn_to_ball();
  faceball->get_cmd(cmd);
    
  return true;
}

bool GoalKick::get_gfk_goalie_cmd(Cmd &cmd) {
  

  return false;
}

bool GoalKick::panic_gfk(Cmd &cmd) {
  LOG_POL(0,<<"GoalKick: No free teammate found, choosing panic kick!");
  std::cerr << "\nGoalKick:: No free teammate found, choosing panic kick!";
  
  
  return true;
}

bool GoalKick::panic_gk(Cmd &cmd) { return panic_gfk(cmd); }

bool GoalKick::scan_field(Cmd &cmd) 
{
  //avoiding to carry the ball out of the field
  if (   WSinfo::me->number == WSinfo::ws->my_goalie_number
      && WSinfo::me->pos.y < -FIELD_BORDER_X+1.5)
  {
    Vector target = WSinfo::me->pos;
    target.y = -FIELD_BORDER_X+1.5;
    go2pos->set_target(target);
    return go2pos->get_cmd(cmd);
  }
  //standard scanning of the field
  Tools::set_neck_request(NECK_REQ_SCANFORBALL);
  if(WSinfo::ws->view_quality == Cmd_View::VIEW_QUALITY_LOW) {
    cmd.cmd_main.set_turn(0);
    return true;
  } 
  Angle turn = .5*(Tools::next_view_angle_width()+Tools::cur_view_angle_width()).get_value();
  basiccmd->set_turn_inertia(turn);
  return basiccmd->get_cmd(cmd);
}
