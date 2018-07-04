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

#ifndef _JKSTATE_H_
#define _JKSTATE_H_


#include "angle.h"
#include "Vector.h"
#include "cmd.h"
#include "ws_info.h"
#include "mystate.h"
#include "tools.h"
#include "log_macros.h"

#include <strstream>
#include "valueparser.h"
#include "sort.h"

#define NUM_OPP 5      //number of opponents (e.g. 5 : 4 + goalie)
#define OPP_GOALIE 0 //array number of goalie
#define NUM_FRIENDS 2 //is not really changeable since some stuff hard coded, sorry

#define LOG_LEVEL 0
#define DBLOG_POL(LLL,XXX) LOG_DEB(LLL,<<"Score04: "<<XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_DEB(LLL,<<_2D<<XXX)

class Score04Action{
public:

int type;          //type of action:
/*
1 : pass_to_closest_friend
2 : pass_to_snd_closest_friend
3 : dribblestraight
4 : immediate_selfpass
     (takes a parameter for the direction)
5 : selfpass
     (takes parameter for direction, ball_speed, target_pos)
*/
float param;   //some actions might need a parameter
float param2; //two
int param3;    //even three
Vector target; //or a target
void set_pass(int closeness){
  if(closeness<1)
    closeness=1;
  if(closeness>2)
    closeness=2;
  type=closeness;
  param=0.0;
  }

void set_pass(int closeness,Vector where,int player){
  if(closeness<1)
    closeness=1;
  if(closeness>2)
    closeness=2;
  type=closeness;
  param=1.0;
  param3=player;
  target=where;
  }

void set_dribble(){
  type=3;
  param=.0;
  }

void set_immediate_selfpass(float dir){
  type=4;
  param=dir;
  }

void set_selfpass(float dir,float speed,Vector target_pos){
  type=5;
  param=dir;
  param2=speed;
  target=target_pos;
  }

Score04Action() {}
};

class Score04State{ //used as input for the net
public:
  float opp_dist[NUM_OPP];  //polar coordinates of opps
  float opp_ang[NUM_OPP];
  float friend_dist[NUM_FRIENDS];  //polar coordinates of friends
  float friend_ang[NUM_FRIENDS];
  float goal_dist;  //polar coordinates of goal (middle)
  float goal_ang;
  float ball_dist;  //polar coordinates of ball
  float ball_ang;
  float ball_vel_norm;  //velocity of ball
  float ball_vel_ang;

 bool fromString(char * input){
  istrstream iss(input);
  iss >> ball_dist;
  iss >> ball_ang;
  iss >> ball_vel_norm;
  iss >> ball_vel_ang;
  iss >> goal_dist;
  iss >> goal_ang;
  for(int i=0;i<NUM_OPP;i++){
    iss >> opp_dist[i];
    iss >> opp_ang[i];
    }
  for(int i=0;i<NUM_FRIENDS;i++){
    iss >> friend_dist[i];
    iss >> friend_ang[i];
    }
  return true;
  }

 char * toString(){
  ostrstream os ;
  os << ball_dist << " " << ball_ang << " " << ball_vel_norm << " " << ball_vel_ang << " " << goal_dist << " " << goal_ang << " ";
  for(int i=0;i<NUM_OPP;i++)
    os << opp_dist[i] << " " << opp_ang[i] << " ";
  for(int i=0;i<NUM_FRIENDS;i++)
    os << friend_dist[i] << " " << friend_ang[i] << " ";
  os << ends ;
  char * s = os.str() ;
  return s;
}

Score04State() {}

};

class jkState {

 public:

  Vector opp_pos[NUM_OPP];  //coordinates of opps
  Vector opp_vel[NUM_OPP];
  ANGLE  opp_ang[NUM_OPP];
  Vector friend_pos[NUM_FRIENDS];  //coordinates of friends
  Vector friend_vel[NUM_FRIENDS];
  ANGLE  friend_ang[NUM_FRIENDS];
  Vector ball_pos;  //coordinates of ball
  Vector ball_vel;
  Vector my_pos;
  Vector my_vel;
  ANGLE  my_ang;


  Vector get_opp_pos(int i){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
    return opp_pos[i];
  }

  Vector get_opp_vel(int i){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
    return opp_vel[i];
  }

  ANGLE  get_opp_ang(int i){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      return opp_ang[i];
  }

  Vector get_friend_pos(int i){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      return friend_pos[i];
  }

  Vector get_friend_vel(int i){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      return friend_vel[i];
  }

  ANGLE  get_friend_ang(int i){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      return friend_ang[i];
  }

  Vector get_ball_pos(){
  return ball_pos;
  }
  Vector get_ball_vel(){
  return ball_vel;
  }
  Vector get_my_pos(){
  return my_pos;
  }
  Vector get_my_vel(){
  return my_vel;
  }
  ANGLE  get_my_ang(){
  return my_ang;
  }


  void set_opp_pos(int i,Vector what){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
    opp_pos[i]=what;
  }

  void set_opp_vel(int i,Vector what){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
    opp_vel[i]=what;
  }

  void  set_opp_ang(int i,ANGLE what){
  if(!(i<NUM_OPP)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      opp_ang[i]=what;
  }

  void set_friend_pos(int i,Vector what){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      friend_pos[i]=what;
  }

  void set_friend_vel(int i,Vector what){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      friend_vel[i]=what;
  }

  void set_friend_ang(int i,ANGLE what){
  if(!(i<NUM_FRIENDS)){
      cerr<<"jkState: ARRAY OUT OF BOUNDS ERROR\n";
      DBLOG_POL(LOG_LEVEL,"jkState: ARRAY OUT OF BOUNDS ERROR\n");
      }
      friend_ang[i]=what;
  }

  void set_ball_pos(Vector what){
  ball_pos=what;
  }
  void set_ball_vel(Vector what){
  ball_vel=what;
  }
  void set_my_pos(Vector what){
  my_pos=what;
  }
  void set_my_vel(Vector what){
  my_vel=what;
  }
  void set_my_ang(ANGLE what){
  my_ang=what;
  }


void resort_opps(){  //sort opponents acc. to their distance to "my_pos", useful e.g. after ballholder changed
Vector opp_pos2[NUM_OPP];  //temporary
Vector opp_vel2[NUM_OPP];
ANGLE  opp_ang2[NUM_OPP];
Sort * eval_pos=new Sort(NUM_OPP);
for(int i=0;i<NUM_OPP;i++){
  opp_pos2[i]=opp_pos[i];
  opp_vel2[i]=opp_vel[i];
  opp_ang2[i]=opp_ang[i];
  eval_pos->add(i,opp_pos[i].distance(my_pos));
  }
eval_pos->do_sort();  //smallest distance first
int ind;
for(int i=0;i<NUM_OPP;i++){
  ind=eval_pos->get_key(i);
  opp_pos[i]=opp_pos2[ind];
  opp_vel[i]=opp_vel2[ind];
  opp_ang[i]=opp_ang2[ind];
  }
}

void resort_friends(){  //sort friends acc. to their distance to "my_pos"
Vector friend_pos2[NUM_FRIENDS];  //temporary
Vector friend_vel2[NUM_FRIENDS];
ANGLE  friend_ang2[NUM_FRIENDS];
Sort * eval_pos=new Sort(NUM_FRIENDS);
for(int i=0;i<NUM_FRIENDS;i++){
  friend_pos2[i]=friend_pos[i];
  friend_vel2[i]=friend_vel[i];
  friend_ang2[i]=friend_ang[i];
  eval_pos->add(i,friend_pos[i].distance(my_pos));
  }
eval_pos->do_sort();  //smallest distance first
int ind;
for(int i=0;i<NUM_FRIENDS;i++){
  ind=eval_pos->get_key(i);
  friend_pos[i]=friend_pos2[ind];
  friend_vel[i]=friend_vel2[ind];
  friend_ang[i]=friend_ang2[ind];
  }
}

 void debug_out(char* color){
DBLOG_DRAW(LOG_LEVEL,C2D(my_pos.x,my_pos.y,0.5, color));
DBLOG_DRAW(LOG_LEVEL,C2D(ball_pos.x,ball_pos.y,0.7, color));
for(int i=0;i<NUM_OPP;i++)
  DBLOG_DRAW(LOG_LEVEL,C2D(opp_pos[i].x,opp_pos[i].y,0.3, color));
for(int i=0;i<NUM_FRIENDS;i++)
  DBLOG_DRAW(LOG_LEVEL,C2D(friend_pos[i].x,friend_pos[i].y,0.1, color));
}

 MyState get_old_version_State(){ //from the times where a state consisted only of me and one opp.
   MyState tmp;
   tmp.my_pos=my_pos;
   tmp.my_vel=my_vel;
   tmp.my_angle=my_ang;
   tmp.ball_pos=ball_pos;
   tmp.ball_vel=ball_vel;
   tmp.op_pos=opp_pos[1]; //1 = consider closest player (0 goalie)
//   tmp.op_vel=opp_vel[1];
   tmp.op_bodydir=opp_ang[1];
   tmp.op=NULL;                //not used
   tmp.op_bodydir_age=0; //not used
   return tmp;
   }

 void copy_to(jkState &target_state){
 target_state.my_pos=my_pos;
 target_state.my_vel=my_vel;
 target_state.my_ang=my_ang;
 target_state.ball_pos=ball_pos;
 target_state.ball_vel=ball_vel;
 for(int i=0;i<NUM_OPP;i++){
   target_state.opp_pos[i]=opp_pos[i];
   target_state.opp_vel[i]=opp_vel[i];
   target_state.opp_ang[i]=opp_ang[i];
   }
 for(int i=0;i<NUM_FRIENDS;i++){
   target_state.friend_pos[i]=friend_pos[i];
   target_state.friend_vel[i]=friend_vel[i];
   target_state.friend_ang[i]=friend_ang[i];
   }
 }

 float angle_player2player(Vector relative_to,Vector oppt,float rel_to_norm){
  Vector p2p=oppt-relative_to;  //Cosinus zwichen 2 Vektoren ist  Skalarprodukt(a,b) / |a|*|b|
  double result=(relative_to.x*p2p.x+relative_to.y*p2p.y)/(rel_to_norm*p2p.norm());
  if(!(result>-10000000.0)) //should be !false only with NAN
    return 0.0;  //use 0.0 instead of NAN
  return result;  //return Cosinus \in [-1,1]
              //acosf((relative_to.x*p2p.x+relative_to.y*p2p.y)/(rel_to_norm*p2p.norm()));
  }

 float scale_dist(float input_dist){
 return (Tools::min(input_dist,20.0)-10.0)/10.0; //maximum distance of 20, scale to [-1,1]
 }

 Score04State get_scoreState(){  //change representation to a format usable by a neural net
   Score04State cst;
  int i;
  float my_norm=my_pos.norm();  //just to save some calculation time
  for(i=1;i<NUM_OPP;i++){
    cst.opp_dist[i]=scale_dist(my_pos.distance(opp_pos[i]));
    cst.opp_ang[i]=angle_player2player(my_pos,opp_pos[i],my_norm);
    }
  for(i=0;i<NUM_FRIENDS;i++){
    cst.friend_dist[i]=scale_dist(my_pos.distance(friend_pos[i]));
    cst.friend_ang[i]=angle_player2player(my_pos,friend_pos[i],my_norm);
    }
  cst.goal_dist=scale_dist(my_pos.distance(Vector(52.5,0.0)));
  cst.goal_ang=angle_player2player(my_pos,Vector(52.5,0.0),my_norm);
  cst.opp_dist[OPP_GOALIE]=scale_dist(my_pos.distance(opp_pos[0]));
  cst.opp_ang[OPP_GOALIE]=angle_player2player(my_pos,opp_pos[0],my_norm);
  cst.ball_dist=scale_dist(my_pos.distance(ball_pos));
  cst.ball_ang=angle_player2player(my_pos,ball_pos,my_norm);
  cst.ball_vel_norm=(ball_vel.norm()-1.35)/1.35;  //max ballspeed is 2.7, scale to [-1,1]
  cst.ball_vel_ang=angle_player2player(my_pos,ball_vel,my_norm);
  return cst;
  }

 bool fromString(char * input){
  istrstream iss(input);
  iss >> my_pos.x;
  iss >> my_pos.y;
  iss >> my_vel.x;
  iss >> my_vel.y;
  iss >> my_ang;
  iss >> ball_pos.x;
  iss >> ball_pos.y;
  iss >> ball_vel.x;
  iss >> ball_vel.y;
  for(int i=0;i<NUM_OPP;i++){
    iss >> opp_pos[i].x;
    iss >> opp_pos[i].y;
    iss >> opp_vel[i].x;
    iss >> opp_vel[i].y;
    iss >> opp_ang[i];
    }
  for(int i=0;i<NUM_FRIENDS;i++){
    iss >> friend_pos[i].x;
    iss >> friend_pos[i].y;
    iss >> friend_vel[i].x;
    iss >> friend_vel[i].y;
    iss >> friend_ang[i];
    }
  return true;
  }

  char * toString(){
  ostrstream os ;
  os << my_pos.x << " " << my_pos.y << " " << my_vel.x<< " " << my_vel.y << " " << my_ang << " " << ball_pos.x << " " << ball_pos.y << " "<< ball_vel.x<<" "<<ball_vel.y<<" ";
  for(int i=0;i<NUM_OPP;i++)
    os << opp_pos[i].x << " "<< opp_pos[i].y << " " << opp_vel[i].x << " " << opp_vel[i].y << " " << opp_ang[i] << " ";
  for(int i=0;i<NUM_FRIENDS;i++)
    os << friend_pos[i].x << " " << friend_pos[i].y << " "<< friend_vel[i].x << " " << friend_vel[i].y << " " << friend_ang[i] << " ";
  os << ends ;
  char * s = os.str() ;
  return s;
}


  void get_from_WS() {
  get_from_WS(WSinfo::me);
  }

  void get_from_WS(PPlayer me){
  get_from_WS(me,me->pos);
  }

  int rand_in_range(int a,int b){
  int off=round(drand48()*32767);
  return a+(off%(b-a+1));
  }

  void get_from_WS(PPlayer me,Vector next_pos) {  //might be used to simulate viewpoint of another player
  my_pos=next_pos;                         //("sich in jemand reinversetzen")
  my_vel=me->vel;
  my_ang=me->ang;

  int i;
  WSpset opp=WSinfo::alive_opponents;
  if(WSinfo::his_goalie!=NULL)
    opp.remove(WSinfo::his_goalie);
  opp.keep_and_sort_closest_players_to_point(NUM_OPP-1,me->pos);
  for(i=1;i<NUM_OPP;i++){  //array space 0 reserved for goalie
    if(opp.num<i-1){
      opp_pos[0]=Vector(-52.0,0.0);
      opp_vel[0]=Vector(.0,.0);
      opp_ang[0]=ANGLE(0);
      }
    else{
     opp_pos[i]=opp[i-1]->pos;
     opp_vel[i]=opp[i-1]->vel;
     opp_ang[i]=opp[i-1]->ang;
     }
    }
  WSpset team=WSinfo::alive_teammates;
  team.remove(me);
  team.keep_and_sort_closest_players_to_point(team.num,me->pos);
  int j=0;int count=0;
  do{                                                               //determine number of close players
  if(team[j]->pos.sqr_distance(me->pos)<25)
    count++;
  }
  while(j++<team.num);
  while(count>NUM_FRIENDS){                    //if too many close players remove some players randomly
    team.remove(team[rand_in_range(0,count-1)]);
    count--;
    }
  for(i=0;i<NUM_FRIENDS;i++){
    friend_pos[i]=team[i]->pos;
    friend_vel[i]=team[i]->vel;
    friend_ang[i]=team[i]->ang;
    }
  if(WSinfo::his_goalie!=NULL){
    opp_pos[0]=WSinfo::his_goalie->pos;
    opp_vel[0]=WSinfo::his_goalie->vel;
    opp_ang[0]=WSinfo::his_goalie->ang;
    }
  else{
    opp_pos[0]=Vector(-52.0,0.0);
    opp_vel[0]=Vector(.0,.0);
    opp_ang[0]=ANGLE(0);
    }
  ball_pos=WSinfo::ball->pos;
  ball_vel=WSinfo::ball->vel;
  }

  jkState() { }
};

#endif
