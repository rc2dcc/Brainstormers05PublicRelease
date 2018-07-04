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

#include "jkstate.h"
#include "sort.h"
#define CYCLES_TO_SAVE 10000
#define MAX_PROT_MEM 30000 //number of actions which can be memorized maximally

char * inttostr(int n){
  ostrstream os;
  os << n << ends ;
  char * s = os.str() ;
  return s;
}

int get_closest_opp_to(jkState state,Vector point){
Sort * opps=new Sort(NUM_OPP);
for(int i=0;i<NUM_OPP;i++){
  opps->add(i,state.opp_pos[i].sqr_distance(point));
  }
opps->do_sort();
return opps->get_key(0);
}

void print_state(jkState state){
for(int i=0;i<NUM_OPP;i++){
  cerr <<"\nOpponent "<< i <<":\n";
  cerr <<"Pos: "<< state.opp_pos[i] <<" Vel: "<< state.opp_vel[i] <<" Angle: "<< state.opp_ang[i];
  }
for(int i=0;i<NUM_FRIENDS;i++){
  cerr <<"\nFriend "<<i <<":\n";
  cerr <<"Pos: "<< state.friend_pos[i] <<" Vel: "<< state.friend_vel[i] <<" Angle: "<< state.friend_ang[i];
  }
cerr <<"\nBall "<< ":\n";
cerr <<"Pos: "<< state.ball_pos <<" Vel: "<< state.ball_vel <<"\n";
cerr <<"me ("<< WSinfo::me->number <<"):\n";
cerr <<"Pos: "<< state.my_pos <<" Vel: "<< state.my_vel <<" Angle: "<< state.my_ang<<"\n\n";
}

void print_scoreState(Score04State state){
for(int i=0;i<NUM_OPP;i++){
  cerr <<"Opponent "<< i <<":\n";
  cerr <<"Dist: "<< state.opp_dist[i] <<" Ang: "<< state.opp_ang[i] <<" \n ";
  }
for(int i=0;i<NUM_FRIENDS;i++){
  cerr <<"Friend "<<i <<":\n";
  cerr <<"Dist: "<< state.friend_dist[i] <<" Ang: "<< state.friend_ang[i] <<" \n";
  }
cerr <<"Goal " <<":\n";
cerr <<"Dist: "<< state.goal_dist <<" Ang: "<< state.goal_ang <<" \n";
cerr <<"Ball " <<":\n";
cerr <<"Dist: "<< state.ball_dist <<" Ang: "<< state.ball_ang <<" \n\n";
}

class Prot{
public:
int time;
int action;
char my_string[200];

Score04State stat;

char * toString(){
  strcpy(my_string,inttostr(time));
  strcat(my_string," ");
  strcat(my_string,inttostr(action));
  strcat(my_string," ");
  strcat(my_string,stat.toString());
  return my_string;
}

Prot(){}
};
