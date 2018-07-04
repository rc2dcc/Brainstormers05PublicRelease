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

#include "positioning.h"
#include "planning.h"
#include "mdp_info.h"
#include "tools.h"
#include "log_macros.h"
#include "options.h"
#include "valueparser.h"
#include "policy_tools.h"
#include "tools.h"
#include "ws_memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "policy_goalie_kickoff.h"
//#include "strategy_defs.h"

//#define LOG_DAN(YYY,XXX) LOG_DEB(YYY,XXX)
#define LOG_DAN(YYY,XXX)


#ifdef NEW433HACK
Formation433 DeltaPositioning::formation433;
Formation433Attack DeltaPositioning::attack433;
bool DeltaPositioning::use_new_formation433= false;
#endif

int           DeltaPositioning :: recommended_attentionto[NUM_PLAYERS+1];
DeltaPosition DeltaPositioning :: pos[NUM_CONSIDERED_POSITIONS];
bool          DeltaPositioning :: consider_player[11];
Formation     DeltaPositioning :: form[MAX_NUM_FORMATIONS];
int           DeltaPositioning :: cnt_formations;
int           DeltaPositioning :: current_formation;
int           DeltaPositioning :: current_matching;
int           DeltaPositioning :: current_evaluation;
Value         DeltaPositioning :: ball_weight_x_defense; 
Value         DeltaPositioning :: ball_weight_y_defense;
Value         DeltaPositioning :: ball_weight_x_buildup;
Value         DeltaPositioning :: ball_weight_y_buildup;
Value         DeltaPositioning :: ball_weight_x_attack;
Value         DeltaPositioning :: ball_weight_y_attack;
Value         DeltaPositioning :: stretch;
Value         DeltaPositioning :: max_defense_line;
Value         DeltaPositioning :: min_defense_line_offset;
Value         DeltaPositioning :: defense_line_ball_offset;
//int           DeltaPositioning :: cycles_after_catch;
double        DeltaPositioning :: move_defense_line_to_penalty_area;
int           DeltaPositioning :: four_chain_established = 0;
bool          DeltaPositioning :: use_BS02_gamestates = 0;
Value         DeltaPositioning :: before_goal_line = 28.0;

DashStamina   Stamina::dashstamina[3];
int Stamina::stamina_management_type;
Value Stamina::stamina_reserve_defenders;
Value Stamina::stamina_reserve_midfielders;
Value Stamina::stamina_reserve_attackers;
Value Stamina::stamina_full_level;
int Stamina::state;
int Stamina::last_update;
Value Stamina::stamina_min_reserve_level;
Value Stamina::stamina_min_reserve;

const Value NARROW_4_CHAIN_LINE = -30.0;

void DeltaPositioning :: init_formations(){
  //cout << "\nInit Formations...";
  current_formation = -1;
  cnt_formations = 0;
  move_defense_line_to_penalty_area = -20.0;
  char tchar[50];
  char key_pos[50];
  ValueParser vp(CommandLineOptions::formations_conf,"Definitions");
  //vp.set_verbose(true);
#ifdef NEW433HACK
  vp.get("use_new_formation433",use_new_formation433);
  formation433.init(CommandLineOptions::formations_conf,0,0);
  attack433.init(CommandLineOptions::formations_conf,0,0);
#endif
  vp.get("Initial_Formation", current_formation);
  vp.get("Initial_Matching" , current_matching);
  vp.get("Initial_Evaluation", current_evaluation);
  //vp.get("Ball_Weight_X", ball_weight_x);
  //vp.get("Ball_Weight_Y", ball_weight_y);
  vp.get("Ball_Weight_X_Defense", ball_weight_x_defense);
  vp.get("Ball_Weight_Y_Defense", ball_weight_y_defense);
  vp.get("Ball_Weight_X_Buildup", ball_weight_x_buildup);
  vp.get("Ball_Weight_Y_Buildup", ball_weight_y_buildup);
  vp.get("Ball_Weight_X_Attack", ball_weight_x_attack);
  vp.get("Ball_Weight_Y_Attack", ball_weight_y_attack);
  
  vp.get("Stretch", stretch);
  vp.get("min_defense_line_offset",min_defense_line_offset);
  vp.get("max_defense_line",max_defense_line);
  vp.get("defense_line_ball_offset",defense_line_ball_offset);
  vp.get("move_defense_line_to_penalty_area",move_defense_line_to_penalty_area);
  vp.get("use_BS02_gamestates", use_BS02_gamestates);
  vp.get("before_goal_line", before_goal_line);

  cout << " use_BS02_gamestates = " << use_BS02_gamestates;
  cout << " before_goal_line = " << before_goal_line;

  sprintf(key_pos, "%d", cnt_formations);
  while (vp.get(key_pos, tchar, 50)>0){
    form[cnt_formations].number = cnt_formations;
    strncpy(form[cnt_formations].name, tchar, 50);
    ValueParser vp2(CommandLineOptions::formations_conf,tchar);
    //vp2.set_verbose(true);
    for(int i=1;i<=10;i++){
      sprintf(key_pos, "pos_%d",i);
      vp2.get(key_pos, form[cnt_formations].pos[i-1], 3);
    }
    cnt_formations++;
    sprintf(key_pos, "%d", cnt_formations);
  }
  cnt_formations--;
  if (cnt_formations == -1){
    cout << "\nNo Formations found.";
  }
  /****************************STAMINA*********************/
  Stamina::init();
#if 0
  cout<<" Positioning: Initial Formation" << current_formation
      <<"Initial Matching" << current_matching
      <<"Initial Evaluation"<< current_evaluation
      <<"Ball Weight X"<< ball_weight_x
      <<"Ball Weight Y" <<ball_weight_y
      <<"Stretch"<<stretch
      <<"min_defense_line_offset"<<min_defense_line_offset
      <<"max_defense_line"<<max_defense_line
      <<"defense_line_ball_offset"<<defense_line_ball_offset;
#endif

  //ValueParser vp3(CommandLineOptions::formations_conf,"Attention");
                        
  ValueParser vp3("conf/attention.conf","Attention");
  //vp3.set_verbose(true);
  char key_player[40];
  for(int i=1; i < NUM_PLAYERS + 1; i++) {
    sprintf(key_player, "player_%d",i);
    recommended_attentionto[i]= -1;
    vp3.get(key_player, recommended_attentionto[i]);
  }
}

DeltaPositioning :: DeltaPositioning(){
  for(int i=0; i<11;i++){
    consider_player[i] = true;
  }
}

void DeltaPositioning :: init_players(bool *players){
  for(int i=0; i<11;i++){
    consider_player[i] = players[i];
  }
}

DashPosition DeltaPositioning::get_position(int player_number){
  DashPosition temp;
  if ( use_new_formation433 ) {
    Vector res= formation433.get_fine_pos(player_number);
    temp.x= res.x;
    temp.y= res.y;
  }
  return temp; 
}

Vector DeltaPositioning::get_position_base(int player_number){
  if ( use_new_formation433 ) 
    return formation433.get_grid_pos(player_number);
  ERROR_OUT << "\nshould never reach this point";
  return Vector(0,0); 
}

Value DeltaPositioning :: get_my_defence_line() {
  if ( use_new_formation433 ) {
    Value defence_line, offence_line;
    formation433.get_boundary(defence_line, offence_line);
    return defence_line;
  }
 ERROR_OUT << "\nshould never reach this point (DeltaPositioning::get_my_defence_line()";
 return 0.0;  
  //Daniel return get_my_defence_line_BS02();
}


Value DeltaPositioning :: get_my_offence_line(){
  if ( use_new_formation433 ) {
    Value defence_line, offence_line;
    formation433.get_boundary(defence_line, offence_line);
    return offence_line;
  }
  ERROR_OUT << "\nshould never reach this point (DeltaPositioning::get_my_offence_line()";
  return 0.0;  
}

int DeltaPositioning :: get_role(int player_number){
  if ( use_new_formation433 ) {
    return formation433.get_role(player_number);
  }
  ERROR_OUT << "\nshould never reach this point (DeltaPositioning::get_role()";
  return 0;
}

/** 
    #####################################################
    STAMINA MANAGEMENT
    #####################################################
 */


Value Stamina::economy_level(){
  int role = DeltaPositioning::get_role(mdpInfo::mdp->me->number);
  Value min_level = ServerOptions::recover_dec_thr*ServerOptions::stamina_max;

  if(role==0) // defender
    return(min_level + stamina_reserve_defenders);
  else if(role==1) // midfielder
    return(min_level + stamina_reserve_midfielders);
  else // attacker
    return(min_level + stamina_reserve_attackers);
}

void Stamina::update_state(){
#define THRESHHOLD 200

  if(mdpInfo::mdp->time_current == last_update)
    return;
  Value stamina = mdpInfo::mdp->me->stamina.v;

  switch(state){
  case STAMINA_STATE_FULL:
    if(stamina<stamina_full_level)
      state = STAMINA_STATE_OK;
    break;
  case STAMINA_STATE_OK:
    if(stamina>stamina_full_level + THRESHHOLD)
      state = STAMINA_STATE_FULL;
    else if(stamina<economy_level())
      state= STAMINA_STATE_ECONOMY;
    break;
  case STAMINA_STATE_ECONOMY:
    if(stamina>economy_level()+THRESHHOLD)
      state=STAMINA_STATE_OK;
    else if(stamina<stamina_min_reserve_level)
      state=STAMINA_STATE_RESERVE;
    break;
  case STAMINA_STATE_RESERVE:
    if (stamina>stamina_min_reserve_level+THRESHHOLD)
      state=STAMINA_STATE_ECONOMY;
    break;
  default:
    if(stamina>stamina_full_level)
      state=STAMINA_STATE_FULL;
    else if(stamina>economy_level())
      state=STAMINA_STATE_OK;
    else if(stamina>stamina_min_reserve_level)
      state=STAMINA_STATE_ECONOMY;
    else
      state=STAMINA_STATE_RESERVE;
    break;
  }
  last_update = mdpInfo::mdp->time_current;
}

int Stamina::get_state(){
  update_state();
  return state;
}


int Stamina::dash_power(){
  int role = DeltaPositioning::get_role(mdpInfo::mdp->me->number);
  Value stamina = WSinfo::me->stamina;

  if(stamina_management_type == 0){ // using default Stamina Management
    if(role==0){
      //Verteidiger orientieren sich an Ball
      if(mdpInfo::is_my_team_attacking() && WSinfo::ball->pos.x > WSinfo::me->pos.x){
	if(stamina >= dashstamina[role].stamina_offence) return (int)dashstamina[role].dash_offence;
      }
      if(mdpInfo::is_my_team_attacking() && WSinfo::ball->pos.x <= WSinfo::me->pos.x){
	if(stamina >= dashstamina[role].stamina_defence) return (int)dashstamina[role].dash_defence;
      }
    } 
    
    if(mdpInfo::is_my_team_attacking()){
      if(stamina >= dashstamina[role].stamina_offence) return (int)dashstamina[role].dash_offence;
    } else{
      if(stamina >= dashstamina[role].stamina_defence) return (int)dashstamina[role].dash_defence;
    }
    return 0;
  }
  else{  // using new stamina management
    if(get_state() != STAMINA_STATE_RESERVE){
      LOG_POL(4,"Stamina Management: My stamina "<<stamina<<" is still ok "
	      <<" -> Dashing with Maximum Power: "<<ServerOptions::maxpower);
      return (int)ServerOptions::maxpower;
    }
    else{ // I'm close to end with my stamina, keeping iron reserve
      LOG_POL(4,"Stamina Management: My stamina "<<stamina<<" in Reserve State! "
	      <<" -> Dashing with Minimum Power: "<<ServerOptions::stamina_inc_max);
      return (int)ServerOptions::stamina_inc_max;
    }

#if 0 // ridi: that was wrong, because it does not distinguish between urgent situations    
    float stamina_reserve;
    if(role==0) // defender
      stamina_reserve = stamina_reserve_defenders;
    else if(role==1) // midfielder
      stamina_reserve = stamina_reserve_midfielders;
    else // attacker
      stamina_reserve = stamina_reserve_attackers;
    if(stamina > stamina_reserve + ServerOptions::recover_dec_thr*ServerOptions::stamina_max){
      LOG_POL(4,"Stamina Management: My stamina "<<stamina<<" is larger than min.reserve "
	      <<stamina_reserve+ ServerOptions::recover_dec_thr*ServerOptions::stamina_max
	      <<" -> Dashing with Maximum Power: "<<ServerOptions::maxpower);
      return (int)ServerOptions::maxpower;
    }
    else{    
      LOG_POL(4,"Stamina Management: My stamina "<<stamina<<" is smaller than min.reserve "
	      <<stamina_reserve+ ServerOptions::recover_dec_thr*ServerOptions::stamina_max
	      <<" -> Dashing with Minimum Power: "<<ServerOptions::stamina_inc_max);
      return (int)ServerOptions::stamina_inc_max;
    }
#endif
  } // end new management
}


void Stamina::init(){
  Value temp[4];
  //cout << "\nInit Stamina...";
  stamina_reserve_defenders = 1000;
  stamina_reserve_midfielders = 500;
  stamina_reserve_attackers = 500;
  stamina_min_reserve = 200;
  stamina_full_level = ServerOptions::stamina_max * 0.8; // all above this is considered as full

  ValueParser vp(CommandLineOptions::formations_conf,"Stamina");
  //vp.set_verbose(true);
  stamina_management_type = 0; // this is the default: Andis (BS2k) Stamina Management
  vp.get("stamina_for_defence", temp,4);
  dashstamina[0].stamina_defence = temp[0];
  dashstamina[0].dash_defence    = temp[1];
  dashstamina[0].stamina_offence = temp[2];
  dashstamina[0].dash_offence    = temp[3];
  vp.get("stamina_for_middle", temp,4);
  dashstamina[1].stamina_defence = temp[0];
  dashstamina[1].dash_defence    = temp[1];
  dashstamina[1].stamina_offence = temp[2];
  dashstamina[1].dash_offence    = temp[3];
  vp.get("stamina_for_offence", temp,4);
  dashstamina[2].stamina_defence = temp[0];
  dashstamina[2].dash_defence    = temp[1];
  dashstamina[2].stamina_offence = temp[2];
  dashstamina[2].dash_offence    = temp[3];
  vp.get("stamina_management_type", stamina_management_type);
  vp.get("stamina_reserve_defenders", stamina_reserve_defenders);
  vp.get("stamina_reserve_midfielders", stamina_reserve_midfielders);
  vp.get("stamina_reserve_attackers", stamina_reserve_attackers);
  stamina_min_reserve_level = stamina_min_reserve +
    ServerOptions::recover_dec_thr*ServerOptions::stamina_max;
  if(stamina_management_type == 0)
    //LOG_ERR(0,<<"Initialized Stamina Management. using original stamina management (BS2k)");
    cout<<"Initialized Stamina Management. using original stamina management (BS2k)"<<endl;
  else if(stamina_management_type == 1)
    //LOG_ERR(0,<<"Initialized Stamina Management. using new stamina management (BS01)");
    cout<<"Initialized Stamina Management. using new stamina management (BS01)"<<endl;
  else
    stamina_management_type = 0; // default
  state = STAMINA_STATE_FULL;
  last_update = 0;
}

/*
void DeltaPositioning :: match_positions_by_moveup(){
  //gegeben: Position der Spieler von mdpInfo + Position der Formation
  //gesucht: matching zu jeder Position

  bool conflict = true;
  int unmatched[10]; //haelt Menge der ungematchten Positionen
  int match[10];     //haelt das Matching fuer Spieler auf Positionen
  int unmatch_cnt = 0; //haelt Anzahl der ungematchten Positionen

  for(int i=0;i<10;i++){
    match[i] = -1;
  }

  do{

    //0. Match unmatched Position
    if(unmatch_cnt>0){
      //bestime schnellsten Spieler zu unmatched[0]
      Value min=1000.0;
      Value temp;
      int fastest_player = -1;
      for(int i=0;i<10;i++){
        if(match[i] >= 0) continue;
        temp = evaluate(i+2, unmatched[0]);
        if(temp<min){
          min=temp;
          fastest_player = i;
	}
      }
      if(fastest_player >= 0) 
        match[fastest_player] = unmatched[0];
      else
        break;
    }

    //1. Greedy Matching with remaining players
    int tpos[10];
    Value min, temp;
    for(int i=0;i<10;i++){
      tpos[i] = -1;
      if(match[i] >= 0) continue;
      min=10000.0;
      for(int j=0;j<NUM_CONSIDERED_POSITIONS;j++){
        temp = evaluate(i+2, j);
        if (temp<min){
          min = temp;
          tpos[i] = j;
        }
      }
    }
    
    //2. Still Conflicts?

    conflict=false;
    for(int i=0;i<10;i++){
      if(tpos[i]<0) continue;
      for(int j=i+1;j<10;j++){
        if(tpos[j]<0) continue;
        if(tpos[i] == tpos[j])  conflict = true;
      }
    }
    //Bestimme Menge der ungematchten Positionen
    if(conflict==true){
      unmatch_cnt = 0;
      bool matched = false;
      for(int i=0;i<10;i++){
        matched = false;
        //1. Temp-Matching
        for(int j=0;j<10;j++){
          if(tpos[j] == i) matched = true;
        }
	//2. End-Matching
        for(int j=0;j<10;j++){
          if(match[j] == i) matched = true;
	}
        if(matched==false){
          unmatched[unmatch_cnt]=i;
          unmatch_cnt++;
	}
      }
    } else{
      //?bernehme Temp-Matching in End-Matching
      for(int i=0;i<10;i++){
        if(tpos[i]<0) continue;
        match[i] = tpos[i];
      }
    }
  } while(conflict == true);
  // da Matching nicht stabil, pruefe mit vorigem Matching ab
  // bestimme maximalen Wegstrecke fuer neues Matching
  Value max = 0;
  Value temp;
  for(int i=0;i<10;i++){
    temp = evaluate(i+2, match[i]);
    if(max < temp) max = temp;
  }
  //bestimme maximale Wegstrecke fuer altes Matching
  Value max2 = 0;
  for(int i=0;i<10;i++){
    temp = evaluate(pos[i].player_assigned, i);
    if(max2 < temp) max2 = temp;
  }  
  //wenn altes Matching besser, nehme altes!
  if(max2 < max) return;  

  //schreiben auf wahre variablen
  for(int i=0;i<10;i++){
    if(match[i]<0) continue;
    pos[match[i]].player_assigned = i+2; 
    LOG_POL(4, << "Player " << i+2 << " soll auf Position " << match[i] << " " << pos[match[i]] << "gehen");
  }
}
*/


Value DeltaPositioning::get_radius(int player_number){
  //stelle fest, ob ich im Radius um meine Zielposition bin
  Value radius = 1000;

  int posid=-1;
  Vector p = Vector(0.0,0.0);
  for(int i=0; i<NUM_CONSIDERED_POSITIONS;i++){
    if (pos[i].player_assigned == mdpInfo::mdp->me->number){
      posid = i;
      p.x = pos[i].x;
      p.y = pos[i].y;
    }
  }
  if(posid==-1) return 0.0;
  
  //ich wurde gematcht
  for(int i=0;i<NUM_CONSIDERED_POSITIONS;i++){
    if(i==posid) continue;
    if(p.distance(pos[i])<radius){
      radius = p.distance(pos[i]);
    }
  }
  if(radius == 1000) return 0.0;
  radius = radius / 2.0;
  return radius;
}

bool DeltaPositioning::is_position_valid(Vector p){
  //Abseits?
  if(p.x > get_my_offence_line()) return false;
  
  //ausserhalb des Spielfeldes?
  if(p.x >  FIELD_BORDER_X ||
     p.x < -FIELD_BORDER_X ||
     p.y >  FIELD_BORDER_Y ||
     p.y < -FIELD_BORDER_Y) return false;

  //ausserhalb meines zugewiesenen Zielbereiches?
  DashPosition default_pos = get_position(mdpInfo::mdp->me->number);
  if(default_pos.distance(p) >= default_pos.radius) return false;
  return true;
}

int DeltaPositioning::get_num_players_with_role(int role){
  int num = 0;
  for(int i=0; i<NUM_CONSIDERED_POSITIONS;i++){
    if((int)form[current_formation].pos[i][2]==role) num++;
  }
  return num;
}

Value DeltaPositioning::get_defence_line_ball_offset() {
#ifdef NEW433HACK
  if ( use_new_formation433 ) {
    return formation433.defence_line_ball_offset;
  }
#endif
  return defense_line_ball_offset;
}


