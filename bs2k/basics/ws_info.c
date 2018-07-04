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

#include "ws_info.h"
#include "options.h"
#include "tools.h"
#include "log_macros.h" //test
#include "macro_msg.h"

WS const* WSinfo::ws= 0;
WS const* WSinfo::ws_full= 0;

int WSinfo::relevant_teammate[11];
int WSinfo::num_relevant_teammates;

//JK PASS_MSG_HACK begin
  bool WSinfo::jk_pass_msg_set=false;
  bool WSinfo::jk_pass_msg_rec=false;
  char WSinfo::jk_pass_msg[80];
  long WSinfo::jk_pass_msg_rec_time=-99;
  float WSinfo::jk_pass_msg_x;
  float WSinfo::jk_pass_msg_y;
//JK PASS_MSG_HACK end

PPlayer WSinfo::me= 0;
PPlayer WSinfo::me_full= 0;
PPlayer WSinfo::his_goalie= 0;

WS::Ball const* WSinfo::ball= 0;
WS::Ball const* WSinfo::ball_full= 0;

Cmd *WSinfo::current_cmd = 0;

WSpset WSinfo::alive_teammates;
WSpset WSinfo::alive_teammates_without_me;
WSpset WSinfo::alive_opponents;

WSpset WSinfo::valid_teammates;
WSpset WSinfo::valid_teammates_without_me;
WSpset WSinfo::valid_opponents;

WSpset WSinfo::pset_tmp;

Value WSinfo::my_team_pos_of_offside_line_cache;
bool WSinfo::my_team_pos_of_offside_line_cache_ok;
Value WSinfo::his_team_pos_of_offside_line_cache;
bool WSinfo::his_team_pos_of_offside_line_cache_ok;
PPlayer WSinfo::teammate_with_newest_pass_info;
bool WSinfo::teammate_with_newest_pass_info_ok;


PPlayer WSinfo::numbered_valid_players[2*NUM_PLAYERS+1];


void WSinfo::set_relevant_teammates_default(){
  int my_number= ClientOptions::player_no;
  
  num_relevant_teammates = 0; // reset
  switch(my_number){
  case 2:
    set_relevant_teammates(6,7,3,9);
    break;
  case 3:
    set_relevant_teammates(6,7,2,4);
    break;
  case 4:
    set_relevant_teammates(8,7,5,3);
    break;
  case 5:
    set_relevant_teammates(8,7,4,11);
    break;
  case 6:
    set_relevant_teammates(9,10,7,3,2);
    break;
  case 7:
    set_relevant_teammates(10,6,8,9,11,3,4);
    break;
  case 8:
    set_relevant_teammates(11,10,7,5,4);
    break;
  case 9:
    set_relevant_teammates(10,6,7,11,8);
    break;
  case 10:
    set_relevant_teammates(11,9,7,6,8);
    break;
  case 11:
    set_relevant_teammates(10,8,7,9,6);
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////
bool WSinfo::init(const WS * worldstate, const WS * worldstate_full) {
  ws= worldstate;
  ws_full= worldstate_full;
  int my_number= ClientOptions::player_no;

  set_relevant_teammates_default();

  me= 0;
  for (int i=0; i< ws->my_team_num; i++)
    if ( ws->my_team[i].alive && ws->my_team[i].number == my_number ) {
      me= &(ws->my_team[i]);
      break;
    }
  if (me == 0) {
    ERROR_OUT << "\n-------------\nme was not set! "
              << "\n" << *ws << "\n---------------";
  }
  

  ball= &(ws->ball);

  if ( ws_full) {
    for (int i=0; i< ws_full->my_team_num; i++)
      if ( ws_full->my_team[i].alive && ws_full->my_team[i].number == my_number ) {
        me_full= &(ws_full->my_team[i]);
        break;
      }
    ball_full= &(ws_full->ball);
  }
  else {
    me_full= 0;
    ball_full= 0;
  }
  
  ////////////////////////////////////////////////////////////////
  alive_teammates.num=0;
  alive_teammates_without_me.num= 0;
  for (int i=0; i< ws->my_team_num; i++)
    if ( ws->my_team[i].alive ) {
      alive_teammates.p[alive_teammates.num]= &(ws->my_team[i]);
      alive_teammates.num++;
      if ( ws->my_team[i].number != my_number ) {
        alive_teammates_without_me.p[alive_teammates_without_me.num]= &(ws->my_team[i]);
        alive_teammates_without_me.num++;
      }
    }
  alive_opponents.num=0;
  for (int i=0; i< ws->his_team_num; i++)
    if ( ws->his_team[i].alive ) {
      alive_opponents.p[alive_opponents.num]= &(ws->his_team[i]);
      alive_opponents.num++;
    }

  valid_teammates.num=0;
  valid_teammates_without_me.num= 0;
  for (int i=0; i< ws->my_team_num; i++)
    if ( is_teammate_pos_valid( & ws->my_team[i] ) ) {
      valid_teammates.p[valid_teammates.num]= &(ws->my_team[i]);
      valid_teammates.num++;
      if ( ws->my_team[i].number != my_number ) {
        valid_teammates_without_me.p[valid_teammates_without_me.num]= &(ws->my_team[i]);
        valid_teammates_without_me.num++;
      }
    }
  valid_opponents.num=0;
  for (int i=0; i< ws->his_team_num; i++)
    if ( is_opponent_pos_valid( & ws->his_team[i] ) ) {
      valid_opponents.p[valid_opponents.num]= &(ws->his_team[i]);
      valid_opponents.num++;
    }

  ////////////////////////////////////////////////////////////////
  my_team_pos_of_offside_line_cache_ok= false;
  his_team_pos_of_offside_line_cache_ok= false;
  teammate_with_newest_pass_info_ok= false;
  ////////////////////////////////////////////////////////////////
  for (int i=0; i<2*NUM_PLAYERS; i++)
    numbered_valid_players[i]= 0;

  for (int i=0; i< valid_teammates.num; i++) {
    int number= valid_teammates[i]->number;
    if (number > 0)
      numbered_valid_players[number]= valid_teammates[i];
  }

  for (int i=0; i< valid_opponents.num; i++) {
    int number= valid_opponents[i]->number;
    if (number > 0)
      numbered_valid_players[number+NUM_PLAYERS]= valid_opponents[i];
  }


#if 0
  //TEST
  PPlayer p= get_teammate_with_newest_pass_info();
  if ( p ) {
    LOG_DEB(0, << "got pass info from player " 
            << p->number  
            << "\n a= " << p->pass_info.age
            << "\n p= " << p->pass_info.ball_pos
            << "\n v= " << p->pass_info.ball_vel
            << "\n at= " << p->pass_info.abs_time
            << "\n rt= " << p->pass_info.rel_time);
  }
  else
    LOG_DEB(0, "no pass info");

#endif 

  if (ws->his_goalie_number > 0 )
    his_goalie= get_opponent_by_number( ws->his_goalie_number );
  else
    his_goalie= 0;

  return true;
}

bool WSinfo::is_ball_pos_valid() {
  return (ws->time - ws->ball.time) <= 7; //7 is the value which was used in Seattle
}

bool WSinfo::is_teammate_pos_valid(PPlayer player) {
  return player->alive && (ws->time - player->time) <= 30; //30 is the value which was used in Seattle
}

bool WSinfo::is_opponent_pos_valid(PPlayer player) {
  if(player){
    if(player->pos.distance(WSinfo::me->pos)<ServerOptions::visible_distance-.1 && player->age >1){ 
      LOG_POL(0,"WSINFO: PLAYER should be in feel range, but age too old -> do not consider!!!");
      return false;
    }
  }
  return player->alive && (ws->time - player->time) <= 60; //60 is the value which was used in Seattle
  //in Seattle there was some extra treatment of the opponent goalie which never
  //expired, but this should probably be done at a higher level!
}

int WSinfo::num_teammates_within_circle(const Vector &centre, const Value radius){
  WSpset pset = valid_teammates_without_me;
  pset.keep_players_in_circle(centre, radius);
  return pset.num;
} 

PPlayer WSinfo::teammate_closest2ball(){
  WSpset pset = valid_teammates;
  return pset.closest_player_to_point(ball->pos);
}



bool WSinfo::is_ball_kickable_for(PPlayer player) {
  return ( player->pos.distance( ball->pos ) <= player->kick_radius );
}

Value WSinfo::my_team_pos_of_offside_line() {
  if (my_team_pos_of_offside_line_cache_ok)
    return my_team_pos_of_offside_line_cache;
  
  pset_tmp= WSinfo::valid_teammates;
  PPlayer myGoalie = NULL;
  if (WSinfo::ws->my_goalie_number != 0)
  myGoalie = WSinfo::get_teammate_by_number(WSinfo::ws->my_goalie_number);
  if (myGoalie) pset_tmp.remove(myGoalie);  
  pset_tmp.keep_and_sort_players_by_x_from_left(1); //keep teammates in penalty area
  if (pset_tmp.num > 0)
    my_team_pos_of_offside_line_cache= pset_tmp[pset_tmp.num-1]->pos.x;
  else {
    my_team_pos_of_offside_line_cache= -FIELD_BORDER_X+1.0;
  }
  my_team_pos_of_offside_line_cache_ok= true;

  //take the ball into account
  if ( WSinfo::is_ball_pos_valid() && WSinfo::ball->pos.x < my_team_pos_of_offside_line_cache )
    my_team_pos_of_offside_line_cache= WSinfo::ball->pos.x;
  
  if ( my_team_pos_of_offside_line_cache < - FIELD_BORDER_X ) {
    //ERROR_OUT << "wrong my offside pos, ball_pos= " << WSinfo::ball->pos << " offside_line= " << my_team_pos_of_offside_line_cache;
    //LOG_ERR(0, << "wrong my offside pos" << WSinfo::ball->pos);
    my_team_pos_of_offside_line_cache= - FIELD_BORDER_X;
  }

  if (my_team_pos_of_offside_line_cache > 0.0)
    my_team_pos_of_offside_line_cache = 0.0;

  return my_team_pos_of_offside_line_cache;
}

Value WSinfo::his_team_pos_of_offside_line() {
  if (his_team_pos_of_offside_line_cache_ok)
    return his_team_pos_of_offside_line_cache;
  
  pset_tmp= WSinfo::valid_opponents;
  PPlayer hisGoalie = NULL;
  if (WSinfo::ws->his_goalie_number != 0)
    hisGoalie = WSinfo::get_opponent_by_number(WSinfo::ws->his_goalie_number);
  if (hisGoalie) pset_tmp.remove(hisGoalie);
  pset_tmp.keep_and_sort_players_by_x_from_right(1); //keep oponents in penalty area
  if (pset_tmp.num > 0) 
    his_team_pos_of_offside_line_cache= pset_tmp[pset_tmp.num-1]->pos.x;
  else
    //his_team_pos_of_offside_line_cache= FIELD_BORDER_X;
    his_team_pos_of_offside_line_cache= FIELD_BORDER_X*0.5; //test
  
  //take the ball into account
  if ( WSinfo::is_ball_pos_valid() && WSinfo::ball->pos.x > his_team_pos_of_offside_line_cache )
    his_team_pos_of_offside_line_cache= WSinfo::ball->pos.x;
  
  if ( his_team_pos_of_offside_line_cache > FIELD_BORDER_X ) {
    //ERROR_OUT << "wrong his offside pos, ball_pos= " << WSinfo::ball->pos << " offside_line= " << his_team_pos_of_offside_line_cache;
    //LOG_ERR(0, << "wrong his offside pos" << WSinfo::ball->pos);
    his_team_pos_of_offside_line_cache= FIELD_BORDER_X;
  }
  if (his_team_pos_of_offside_line_cache < 0.0){
    //LOG_MOV(0,"Offside line < 0!!(his_team_pos_of_offside_line_cache < 0.){correcting his team offside line to 0");
    his_team_pos_of_offside_line_cache =0.0;
  }
  return his_team_pos_of_offside_line_cache;
}
 
PPlayer WSinfo::get_teammate_with_newest_pass_info() {
  if ( ! teammate_with_newest_pass_info_ok ) {
    teammate_with_newest_pass_info_ok= true;
    teammate_with_newest_pass_info= valid_teammates_without_me.get_player_with_newest_pass_info();
  }
  return teammate_with_newest_pass_info;  
}

void  WSinfo::visualize_state(){
  if ( LogOptions::is_off() )
    return;

  const Value PLAYER_RADIUS= 1.8;
  const Value BALL_RADIUS= 1.2;
  const int LOG_LEVEL= 0;//2

  //  const char*  MY_COLORS[]={"0000ff","000070","000050","000010"};
  //  const char* HIS_COLORS[]={"ff0000","700000","500000","100000"}; 
  const char*  MY_COLORS[]={"0000ff","0000f0","000050","000010"};
  const char* HIS_COLORS[]={"ff0000","f00000","500000","100000"}; 
  const char* BALL_COLORS[]={"ff66ff","cc66cc","996699","666666","336633"};
  //  const int PCOL_INT[]={3,3,2,2,2,2,1,1,1,1,0,0,0,0};
  const int PCOL_INT[]={0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  if(ws->time==0) return;  
  int i,age;
  Vector vel,pos;

  WSpset pset= WSinfo::valid_teammates;
  pset += WSinfo::valid_opponents;
  int logLevIncr = 0;
  for(i=0;i< pset.num;i++ ){
    PPlayer p= pset[i]; //shortcut
    if (p->pos.distance(WSinfo::me->pos) >  4.0) logLevIncr = 1;
    if (p->pos.distance(WSinfo::me->pos) > 12.0) logLevIncr = 2;
    age=p->age;
    if(age>13)
      age =13;
    pos=p->pos;

    char const* color= "ff0000";
    if ( p->team == MY_TEAM )
      color= MY_COLORS[PCOL_INT[age]];
    else if ( p->team == HIS_TEAM )
      color= HIS_COLORS[PCOL_INT[age]];
	  

    LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << C2D(pos.x,pos.y,PLAYER_RADIUS,color));
    LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << STRING2D(pos.x+PLAYER_RADIUS, pos.y,p->number << ", a=(" << p->age << ',' << p->age_vel << ',' <<  p->age_ang << ')',color) );
# if 0
    LOG_MDP(LOG_LEVEL,<< _2D  
	    << STRING2D(pos.x+PLAYER_RADIUS, pos.y,p->number << ',' << p->age<<','<< p->kick_radius << ',' << p->speed_max,color));
# endif
    //LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  
    //    << STRING2D(pos.x+PLAYER_RADIUS, pos.y,p->number << ',' << p->age,color));

    //orientation of player

    Vector tmp;
    tmp.init_polar(PLAYER_RADIUS, p->ang );
    tmp+= pos;

    if(pset[i]->age_ang == 0){
      LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << L2D(pos.x,pos.y, tmp.x+0.1,tmp.y+0.1, "green") );
    }
    else{
      LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << L2D(pos.x,pos.y, tmp.x,tmp.y, "magenta") );
    }

    if( p->vel.sqr_norm() > SQUARE(0.2) ) {
      LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << L2D(pos.x,pos.y,pos.x+5*p->vel.x,pos.y+5*p->vel.y,color));
      LOG_MDP(LOG_LEVEL+logLevIncr,<< _2D  << STRING2D(pos.x, pos.y+PLAYER_RADIUS, (((int)(100.0*p->vel.x))/100.0) 
        <<","<<((int)(100.0*p->vel.y)/100.0)<<"="<<p->vel.norm(), color));
    }      
  }

  if(is_ball_pos_valid()) {
    pos=ws->ball.pos;
    vel=ws->ball.vel;
    LOG_MDP(LOG_LEVEL,<< _2D  << C2D(pos.x,pos.y,BALL_RADIUS,BALL_COLORS[0]) << C2D(pos.x,pos.y,BALL_RADIUS+.2,BALL_COLORS[0]) );  
    LOG_MDP(LOG_LEVEL,<< _2D  << L2D(pos.x,pos.y,pos.x+10*vel.x,pos.y+10*vel.y,BALL_COLORS[0]));



  }

  Value line=my_team_pos_of_offside_line();
  LOG_MDP(LOG_LEVEL,<< _2D  << L2D(line,-0.5*ServerOptions::pitch_width,
				   line,0.5*ServerOptions::pitch_width,"#006600") );
  line=his_team_pos_of_offside_line();
  LOG_MDP(LOG_LEVEL,<< _2D  << L2D(line,-0.5*ServerOptions::pitch_width,
				   line,0.5*ServerOptions::pitch_width,"#006600") );

}


bool WSinfo::get_teammate(int number, PPlayer & p) {
  p= get_teammate_by_number(number);
  if ( p && p->alive ){ //otherwise attentionto causes (wrong command form) messages
    return true;
  }
  return false;
}

void WSinfo::set_relevant_teammates(const int t1,const int t2,const int t3,const int t4,
				    const int t5,const int t6,const int t7,const int t8,
				    const int t9,const int t10,const int t11){

  num_relevant_teammates = 0; // reset
  if(t1>0) 
    relevant_teammate[num_relevant_teammates++] = t1;
  if(t2>0) 
    relevant_teammate[num_relevant_teammates++] = t2;
  if(t3>0) 
    relevant_teammate[num_relevant_teammates++] = t3;
  if(t4>0) 
    relevant_teammate[num_relevant_teammates++] = t4;
  if(t5>0) 
    relevant_teammate[num_relevant_teammates++] = t5;
  if(t6>0) 
    relevant_teammate[num_relevant_teammates++] = t6;
  if(t7>0) 
    relevant_teammate[num_relevant_teammates++] = t7;
  if(t8>0) 
    relevant_teammate[num_relevant_teammates++] = t8;
  if(t9>0) 
    relevant_teammate[num_relevant_teammates++] = t9;
  if(t10>0) 
    relevant_teammate[num_relevant_teammates++] = t10;
  if(t11>0) 
    relevant_teammate[num_relevant_teammates++] = t11;
}


