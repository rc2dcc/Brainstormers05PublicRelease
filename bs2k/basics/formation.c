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

#include "formation.h"
#include "macro_msg.h"
#include "ws_info.h"
#include "valueparser.h"
#include "log_macros.h"
#include "ws_memory.h"
#include "mdp_info.h"
#include <stdio.h>

bool Formation433::init(char const * conf_file, int argc, char const* const* argv) {
  if (NUM_PLAYERS < 11) {
    ERROR_OUT << "\nwrong number of players: " << NUM_PLAYERS; 
    return false;
  }
  //the following entries are (usually) in [0,1]x[-1,1]
  //x values smaller then 0 violate our defence line
  //x values greater then 1 violate our offence line
  //y values > 1 or < -1 violate the top or bottom field borders
  //goalie doesn't retrieve his pos from a Formation, but just to be consistent: he would run out of the field!
  home[1].pos = Vector(0, 0); 
  home[2].pos = Vector(0, 0.6);
  home[3].pos = Vector(0, 0.2);
  home[4].pos = Vector(0,-0.2);
  home[5].pos = Vector(0,-0.6);
  home[6].pos = Vector(0.5, 0.4);
  home[7].pos = Vector(0.5, 0.0);
  home[8].pos = Vector(0.5,-0.4);
  home[9].pos = Vector(1.0, 0.4);
  home[10].pos= Vector(1.0, 0.0);
  home[11].pos= Vector(1.0,-0.4);

  home[1].stretch_pos_x = 0.0;   home[1].stretch_neg_x = 0.0;   home[1].stretch_y = 0.0; 
  home[2].stretch_pos_x = 0.0;   home[2].stretch_neg_x = 0.0;   home[2].stretch_y = 0.0; 
  home[3].stretch_pos_x = 0.0;   home[3].stretch_neg_x = 0.0;   home[3].stretch_y = 0.0; 
  home[4].stretch_pos_x = 0.0;   home[4].stretch_neg_x = 0.0;   home[4].stretch_y = 0.0; 
  home[5].stretch_pos_x = 0.0;   home[5].stretch_neg_x = 0.0;   home[5].stretch_y = 0.0; 
  home[6].stretch_pos_x = 0.5;   home[6].stretch_neg_x = 0.0;   home[6].stretch_y = 0.5; 
  home[7].stretch_pos_x = 0.2;   home[7].stretch_neg_x = 0.0;   home[7].stretch_y = 0.5; 
  home[8].stretch_pos_x = 0.5;   home[8].stretch_neg_x = 0.0;   home[8].stretch_y = 0.5; 
  home[9].stretch_pos_x = 0.0;   home[9].stretch_neg_x = 0.0;   home[9].stretch_y = 0.25; 
  home[10].stretch_pos_x= 0.0;   home[10].stretch_neg_x= 0.0;   home[10].stretch_y= 0.25; 
  home[11].stretch_pos_x= 0.0;   home[11].stretch_neg_x= 0.0;   home[11].stretch_y= 0.25; 

  home[1].role = PT_GOALIE;
  home[2].role = PT_DEFENDER;
  home[3].role = PT_DEFENDER;
  home[4].role = PT_DEFENDER;
  home[5].role = PT_DEFENDER;
  home[6].role = PT_MIDFIELD;
  home[7].role = PT_MIDFIELD;
  home[8].role = PT_MIDFIELD;
  home[9].role = PT_ATTACKER;
  home[10].role= PT_ATTACKER;
  home[11].role= PT_ATTACKER;


  defence_line_ball_offset= 5.0;

  if ( ! conf_file ) {
    WARNING_OUT << " using hard wired formation";
    return true;
  }

  ValueParser vp(conf_file,"Formation433");
  INFO_OUT << "\nconf_file= " << conf_file;
  //vp.set_verbose(true);
  char key_pos[10];

#ifdef TRAINING
  Value values[6];
#else
  Value values[5];
#endif
  for(int i=1;i<=11;i++){
    sprintf(key_pos, "player_%d",i);
#ifdef TRAINING
    int num= vp.get(key_pos, values,6);
    if (num != 6) {
      ERROR_OUT << "\n wrong number of arguments (" << num << ") for [" << key_pos << "]";
      return false;
    }
#else
    int num= vp.get(key_pos, values,5);
    if (num != 5) {
      ERROR_OUT << "\n wrong number of arguments (" << num << ") for [" << key_pos << "]";
      return false;
    }
#endif

    home[i].pos.x= values[0];
    home[i].pos.y= values[1];
    home[i].stretch_pos_x= values[2];
    home[i].stretch_neg_x= values[3];
    home[i].stretch_y= values[4];
#ifdef TRAINING
    home[i].role= static_cast<int>(values[5]);
#endif
    std::cout << "\n" << i << " pos= " << home[i].pos << " stretch= " << home[i].stretch_pos_x << ", " << home[i].stretch_neg_x << ", " << home[i].stretch_y << " role= " << home[i].role;
  }

  vp.get("defence_line_ball_offset",defence_line_ball_offset);
  //INFO_OUT << "\ndefence_line_ball_offset= " << defence_line_ball_offset;
  return true;
}

int Formation433::get_role(int number) {
  /*
  if ((WSinfo::ball->pos.x < -10.0) && (number == 7)) {
    return PT_DEFENDER;
    }*/
  if(WSinfo::ws->play_mode == PM_my_PenaltyKick)
    return 2;  

  return home[number].role;
}

Vector Formation433::get_grid_pos(int number) {
  //LOG_DEB(0,"play_mode= " << PLAYMODE_STR[WSinfo::ws->play_mode] << " ball_pos= " << WSinfo::ball->pos << " ball_age= " << WSinfo::ball->age);

  Vector res;
  
  Home & h = home[number]; 

  if (number == 7) {
    if ( (WSinfo::ball->pos.x < -10.0) && !mdpInfo::is_my_team_attacking() ) h.pos = Vector(0.2, 0.0);
    else if ( (WSinfo::ball->pos.x > -3.0) ||
	      (WSinfo::ball->pos.x > -10.0) && mdpInfo::is_my_team_attacking() )
      h.pos = Vector(0.5, 0.0);
  } else if (number == 6) {
    if ( (WSinfo::ball->pos.x < -25.0) && !mdpInfo::is_my_team_attacking() ) h.pos = Vector(0.3, 0.4);
    else if ( (WSinfo::ball->pos.x > -20.0) || 
	      (WSinfo::ball->pos.x > -25.0) && mdpInfo::is_my_team_attacking() )
      h.pos = Vector(0.5, 0.4);
  } else if (number == 8) {
    if ( (WSinfo::ball->pos.x < -25.0) && !mdpInfo::is_my_team_attacking() ) h.pos = Vector(0.3, -0.4);
    else if ( (WSinfo::ball->pos.x > -20.0) || 
	      (WSinfo::ball->pos.x > -25.0) && mdpInfo::is_my_team_attacking() )
	      h.pos = Vector(0.5, -0.4);
  }

  get_boundary(defence_line, offence_line); 

  Value x_stretch= offence_line-defence_line;
  Value y_stretch= FIELD_BORDER_Y;
  if ( get_role(number) == PT_ATTACKER && offence_line < FIELD_BORDER_X - 16.0 )
    y_stretch *= 1.5;

  switch(WSinfo::ws->play_mode) {
  case PM_my_GoalieFreeKick:
    if(get_role(number)==PT_DEFENDER) { 
      //y_stretch*=1.3;
    } else if(get_role(number)==PT_MIDFIELD) {
      //y_stretch*=1.5;
      x_stretch/=1.5;
    }
    break;
  case PM_my_GoalKick:
    if(get_role(number)==PT_DEFENDER) { 
      //y_stretch*=1.3;
    } else if(get_role(number)==PT_MIDFIELD) {
      //y_stretch*=1.5;
      x_stretch/=1.8;
    }
    break;
  }
  res.x= defence_line + h.pos.x * x_stretch;
  res.y= h.pos.y * y_stretch ;
  switch ( WSinfo::ws->play_mode ) {
  case PM_his_BeforeKickOff:
  case PM_his_KickOff:
  case PM_my_AfterGoal:
    if ( number == 10 ) {
      res.x -= 9.5;
      //LOG_DEB(0,"play_mode2= " << PLAYMODE_STR[WSinfo::ws->play_mode] << " res_pos= " << res);
    }
    break;
#if 0
  case PM_my_GoalieFreeKick:
    switch(number) {
    case 7: res.x = -16.5; break;
    }
    break;
  case PM_my_GoalKick:
    switch(number) {
    case 3: res.y = 0; break;
    case 7: res.x = -16.5; break;
    }
    break;
#endif
  }
#if 1
  if ( (defence_line < -33) && (get_role(number) == PT_DEFENDER) )
    res.y= h.pos.y * y_stretch / 1.8 ;
#endif

  Vector attr_to_ball= WSinfo::ball->pos - WSinfo::me->pos;
  Value xxx= fabs(WSinfo::ball->pos.y/ FIELD_BORDER_Y)*1.5;
  if ( xxx > 1.0 )
    xxx= 1.0;
  attr_to_ball.y *= xxx;
  //attr_to_ball.y= WSinfo::ball->pos.y; //go2003

  switch ( WSinfo::ws->play_mode ) {
  case PM_my_BeforeKickOff:
  case PM_his_BeforeKickOff:
  case PM_my_KickOff:
  case PM_his_KickOff:
  case PM_my_AfterGoal:
  case PM_his_AfterGoal:
  case PM_his_GoalieFreeKick:
  case PM_his_GoalKick:
    attr_to_ball= Vector(0,0);
  }

  if ( attr_to_ball.x > 0 )
    attr_to_ball.x *= h.stretch_pos_x;
  else
    attr_to_ball.x *= h.stretch_neg_x;

  //Ball is more attractive if near the opponents goal!
  Value reinforce_towards_x;
  if ( WSinfo::ball->pos.x < 0.0 )
    //reinforce_towards_x= 0.0;
    reinforce_towards_x= 1.0;
  else
    reinforce_towards_x= 1.0 +  WSinfo::ball->pos.x/ FIELD_BORDER_X;

  attr_to_ball.y *= h.stretch_y;
  attr_to_ball.y *= reinforce_towards_x;

  res += attr_to_ball;

#if 0
  if ( defence_line < -5.0 ) {
    Value factor= Value(-defence_line)/FIELD_BORDER_X; 
    if ( number == 2 ) 
      res.y -= 7* factor;

    if ( number == 3 ) 
      res.y -= 2* factor;

    if ( number == 4 ) 
      res.y += 2* factor;

    if ( number == 5 ) 
      res.y += 7* factor;
  }
#endif

  //GO2003
  if ((number == 7) && (WSinfo::ws->play_mode == PM_PlayOn) && (defence_line < -FIELD_BORDER_X + 15.0)) {
    res.x = defence_line + 6.0;
    res.y = 0.0;
  }

  LOG_DEB(0, << _2D  << C2D(res.x,res.y,1.5,"ff0000") << STRING2D(res.x+0.4,res.y, number,"ff0000"));
  return res;
}

bool Formation433::need_fine_positioning(int number) {
  Vector grid_pos= get_grid_pos(number);

  if ( WSinfo::me->pos.sqr_distance( grid_pos ) > SQUARE(3.0) )
    return true;
  return false;
}

Vector Formation433::get_fine_pos(int number) {
  Vector res= get_grid_pos(number);

  //don't go into offside
  if ( WSinfo::me->pos.x > offence_line )
    return res;

  //don't be lazy if you are a defender, and should be closer to your own goal
  if ( get_role(number) == PT_DEFENDER && 
       WSinfo::me->pos.x > intercept_ball_pos.x  - defence_line_ball_offset )
    return res;

  Value sqr_ball_dist= WSinfo::me->pos.sqr_distance( WSinfo::ball->pos);
  Value lazyness;
  if ( sqr_ball_dist < SQUARE(30) )
    lazyness= 5.0;
  else
    lazyness= 9.0;
  
  if ( WSinfo::me->stamina > 3500 ) 
    lazyness /= 2;

  //if ( get_role(number) == PT_ATTACKER  && WSinfo::me->pos.x < offence_line -2.0 )
    
  //be lazy
  if ( res.sqr_distance( WSinfo::me->pos) < SQUARE(lazyness) )
    return WSinfo::me->pos;

  return res;
}

void Formation433::get_boundary(Value & defence, Value & offence) {
  if ( boundary_update_cycle == WSinfo::ws->time ) {
    defence= defence_line;
    offence= offence_line;
  }

  boundary_update_cycle= WSinfo::ws->time;

  switch ( WSinfo::ws->play_mode ) {
  case PM_my_BeforeKickOff:
  case PM_his_BeforeKickOff:
  case PM_my_KickOff:
  case PM_his_KickOff:
  case PM_my_AfterGoal:
  case PM_his_AfterGoal:
    defence_line= -20.0;
    offence_line= -0.1;
    defence= defence_line;
    offence= offence_line;
    return;
  case PM_my_GoalieFreeKick:
  case PM_my_GoalKick:
    defence_line= -33.0;
    offence_line= 5.0;
    defence= defence_line;
    offence= offence_line;
    return;
  case PM_his_GoalieFreeKick:
  case PM_his_GoalKick:
    defence_line= -5.0;
    offence_line= 33.0;
    defence= defence_line;
    offence= offence_line;
    return;
  }

  Vector ball_pos= WSinfo::ball->pos;
  Vector ball_vel= WSinfo::ball->vel;
  if ( ! WSinfo::is_ball_pos_valid() ) //don't use the velocity, it the ball is too old!
    ball_vel= Vector(0,0);

  WSpset pset= WSinfo::valid_teammates;
  pset+= WSinfo::valid_opponents;
  InterceptResult ires[1];
  
  pset.keep_and_sort_best_interceptors(1, ball_pos, ball_vel, ires);
  intercept_ball_pos= ires[0].pos;

  //go back defence_line_ball_offset meters behind the point, where the ball will be intercepted
  if ( (WSinfo::ws->play_mode == PM_his_KickIn) ||
       (WSinfo::ws->play_mode == PM_his_FreeKick) ) {
    defence_line= ires[0].pos.x - 15.0;
  } else {
    //    if(WSmemory::team_last_at_ball() == MY_TEAM){
    if(WSmemory::team_last_at_ball() == 0){
      defence_line= ires[0].pos.x ;
    }
    else
      defence_line= ires[0].pos.x - defence_line_ball_offset;
  }

  //don't go into the opponent's half
  if ( defence_line > -4.0 )
    defence_line= -4.0;

  //don't go out of the field
  if ( defence_line < -FIELD_BORDER_X + 10 )
    defence_line= -FIELD_BORDER_X + 10;

  offence_line= WSinfo::his_team_pos_of_offside_line() - 10;
  if ( offence_line > FIELD_BORDER_X - 5.0 )
    offence_line= FIELD_BORDER_X- 5.0;
  
  if ( intercept_ball_pos.x < -10.0) //usopen2003 hack, to get the attackers nearer to the midfield
    offence_line -= 5.0;

  if ( offence_line - defence_line > 50.0 )
    offence_line= defence_line+ 50.0;
  
  defence= defence_line;
  offence= offence_line;
};


