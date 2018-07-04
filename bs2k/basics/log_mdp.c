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

#include "log_mdp.h"
#include <stdio.h>
#include <strstream>
#include "log_macros.h"
#include "mdp_info.h"

void log_mdp_me(const MDPstate *mdp,char *tag){
  char buffer[200];
  Vector my_pos =  mdp->me->pos();
  Vector my_vel =  mdp->me->vel();

  sprintf(buffer, 
	  "Me: Abs (%.1f %.1f age: %d (%.2f)) V (%d %.2f /%.2f %.2f age: %d (%.2f)) NeckAngle %d BodyAngle %d Stamina: %d TheirGoalie: %d OurGoalie: %d", 
	  my_pos.x, my_pos.y,
	  //mdpInfo::mdp->time_current- mdp->me->pos_seen_at(),
	  mdpInfo::age_playerpos(mdp->me),
	  mdp->me->pos_x.p,
	  RAD2DEG(my_vel.arg()),my_vel.norm(),
	  my_vel.x, my_vel.y, 
	  //mdpInfo::mdp->time_current- mdp->me->vel_seen_at(),
	  mdpInfo::age_playervel(mdp->me),
	  mdp->me->vel_x.p,
	  RAD2DEG(mdp->me->neck_angle.v), 
	  RAD2DEG(mdp->me->ang.v), (int) mdp->me->stamina.v,
	  mdp->his_goalie_number,mdp->my_goalie_number);

  // show neck angle
  float x1,x2,x3,x4;
  x1=my_pos.x;
  x2=my_pos.y;
  Vector target;
  target.init_polar(1.,mdpInfo::mdp->me->neck_angle.v);
  x3 = x1 + target.x;
  x4 = x2 + target.y;
  if(mdp->time_current>0){
    //LOG_MDP(1,<< tag <<" "<< buffer);
    LOG_MDP(1,<< _2D << L2D( x1,x2,x3,x4,"red")); 
  }
};

void log_mdp_ball(const MDPstate *mdp,char *tag){
  char buffer[200];
  Vector my_pos =  mdp->me->pos();
  Vector ball_pos =  mdp->ball.pos();
  Vector rel_ball_pos = ball_pos-my_pos;

  sprintf(buffer, 
     "Ball: Abs (%.1f %.1f age: %d (%.2f)) Rel (%.1f %.1f;%.2f) V (%d %.2f /%.1f %.1f age %d)", 
      ball_pos.x,ball_pos.y,
	  WSinfo::ball->age,
	  mdp->ball.pos_x.p,
	  rel_ball_pos.x, rel_ball_pos.y,
	  rel_ball_pos.norm(),
	  RAD2DEG(WSinfo::ball->vel.arg()),WSinfo::ball->vel.norm(),
	  WSinfo::ball->vel.x, 
	  WSinfo::ball->vel.y,
	  WSinfo::ball->age);

  // show ball, ball vel
  float x1,x2,x3,x4;
  Vector ball =WSinfo::ball->pos;

  x1= ball.x;
  x2= ball.y;
  ball = ball + mdp->ball.vel();
  x3= ball.x;
  x4= ball.y;
  LOG_MDP(1,<< _2D <<C2D( x1,x2,0.085,"blue")<<L2D( x1,x2,x3,x4,"blue"));
  if(mdpInfo::server_state!=NULL){
    if(mdpInfo::server_state->time_current >= mdpInfo::mdp->time_current){
      ball = mdpInfo::server_state->ball.pos();
      ball = ball - mdpInfo::server_state->me->pos();
      ball = ball + mdp->me->pos();
      x1= ball.x;
      x2= ball.y;
      ball = ball + mdpInfo::server_state->ball.vel();
      x3= ball.x;
      x4= ball.y;
      LOG_MDP(1,<< _2D << C2D( x1,x2,0.085,"black")<<L2D( x1,x2,x3,x4,"black"));
    }
  }
}

void log_mdp_all(const MDPstate *mdp){
  log_mdp_me(mdp,"W");
  if(mdpInfo::server_state!=NULL){
    if(mdpInfo::server_state->time_current >= mdpInfo::mdp->time_current)
      log_mdp_me(mdpInfo::server_state,"SW");
  }
  log_mdp_ball(mdp,"W");
  if(mdpInfo::server_state!=NULL){
    if(mdpInfo::server_state->time_current >= mdpInfo::mdp->time_current)
      log_mdp_ball(mdpInfo::server_state,"SW");
  }
  log_mdp_all(mdp,"W");
  if(mdpInfo::server_state!=NULL){
    if(mdpInfo::server_state->time_current >= mdpInfo::mdp->time_current)
      log_mdp_all(mdpInfo::server_state,"SW");
  }
}

void log_mdp_all(const MDPstate *mdp,char* tag){
  char buffer[2000];
  static char buffer_2d[2048];
  std::strstream stream_2d(buffer_2d,2048);
  int i;
  int seen_before;

  sprintf(buffer,"%s","");
  for(i=0;i<11;i++){  // I am always my_team[0]
    if(mdp->my_team[i].pos_x.p){  // >0: position valid
      seen_before = mdpInfo::age_playerpos(&(mdp->my_team[i]));
      sprintf(buffer,"%s (F%d (%.0f,%.0f);(%.2f,%.2f) age: %d (%.2f)) ",
	      buffer,
	      mdp->my_team[i].number,
	      mdp->my_team[i].pos_x.v,mdp->my_team[i].pos_y.v,
	      mdp->my_team[i].vel_x.v,mdp->my_team[i].vel_y.v,
	      seen_before,
	      mdp->my_team[i].pos_x.p);

      //@andi: um den Radius fuer Feinpositionierung zu zeigen
      DashPosition t = DeltaPositioning::get_position(mdp->my_team[i].number);
      //LOG_DRAWCIRCLE_D<<(int)t.x<<" "<<(int)t.y<<" "<<(int)t.radius<<" purple1";
      stream_2d <<C2D(t.x, t.y, t.radius, "purple1");

      if(strcmp(tag,"W") == 0){
	if(seen_before < 4){
	  //LOG_DRAWCIRCLE_D<<(int)mdp->my_team[i].pos_x.v<<" "<<(int)mdp->my_team[i].pos_y.v<<" 2 blue";
	  stream_2d<<C2D(mdp->my_team[i].pos_x.v,mdp->my_team[i].pos_y.v ,.3,"blue");	  
	}
	else{
	  //LOG_DRAWCIRCLE_D<<(int)mdp->my_team[i].pos_x.v<<" "<<(int)mdp->my_team[i].pos_y.v<<" 2 pink";
	  stream_2d<<C2D(mdp->my_team[i].pos_x.v,mdp->my_team[i].pos_y.v ,.3,"pink");	  
	}
      }
    }
  }
  //LOG_MDP(1,<<tag<<"      "<<buffer);
  sprintf(buffer,"%s","");

  // Information about Opponents:
  for(i=0;i<11;i++){  
    if(mdp->his_team[i].pos_x.p){  // >0: position valid
      seen_before = mdpInfo::age_playerpos(&(mdp->his_team[i]));
      sprintf(buffer,"%s (O%d (%.0f,%.0f);(%.2f,%.2f) age: %d (%.2f)) ",
	      buffer,
	      mdp->his_team[i].number,
	      mdp->his_team[i].pos_x.v,mdp->his_team[i].pos_y.v,
	      mdp->his_team[i].vel_x.v,mdp->his_team[i].vel_y.v,
	      seen_before,
	      mdp->his_team[i].pos_x.p);
      if(strcmp(tag,"W") == 0){
	if(seen_before < 4){
	  stream_2d<<C2D(mdp->his_team[i].pos_x.v,mdp->his_team[i].pos_y.v ,.3,"red");	  
	}
	else{
	  stream_2d<<C2D(mdp->his_team[i].pos_x.v,mdp->his_team[i].pos_y.v ,.3,"magenta");
	}
      }
    }
  }
  
  stream_2d << '\0';
  LOG_MDP(1,<< _2D << buffer_2d);
};

