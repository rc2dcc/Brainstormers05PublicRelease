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

#include "ws_memory.h"
#include "ws_info.h"
#include "cmd.h"
#include "log_macros.h"

#define MAX_VIEW_INFO 10          // number of view infos stored (one for every cycle)
#define MAX_VISIBLE_DISTANCE 50.0 // ignore objects further away (last_seen_to_point())

WSmemory::ViewInfo WSmemory::view_info[MAX_VIEW_INFO];
long WSmemory::ball_was_kickable4me_at;

int WSmemory::counter;
float WSmemory::momentum[MAX_MOMENTUM];
int WSmemory::opponent_last_at_ball_number;
long WSmemory::opponent_last_at_ball_time;
int WSmemory::teammate_last_at_ball_number; // include myself!
long WSmemory::teammate_last_at_ball_time;
int WSmemory::saved_team_last_at_ball;
int WSmemory::saved_team_in_attack; //gibt an, welches Team im Angriff ist
Value WSmemory::his_offside_line_lag2;
Value WSmemory::his_offside_line_lag1;
Value WSmemory::his_offside_line_lag0;
int WSmemory::last_update_at;

void WSmemory::init() {
  for(int i=0;i<MAX_VIEW_INFO;i++) {
    view_info[i].time=-1;
  }
  ball_was_kickable4me_at = -1000; // init
  his_offside_line_lag2 = 0;
  his_offside_line_lag1 = 0;
  his_offside_line_lag0 = 0;
  last_update_at = -1;
}

void WSmemory::update_fastest_to_ball() {
  InterceptResult intercept_res;
  if (WSinfo::is_ball_pos_valid()) {
    //berechnet mit Gedaechtnis wer im Angriff ist (=schnellster Spieler zum Ball)
    int summe = 0;
    WSpset pset_tmp = WSinfo::valid_teammates;
    pset_tmp += WSinfo::valid_opponents;
    pset_tmp.keep_and_sort_best_interceptors(1, WSinfo::ball->pos, WSinfo::ball->vel, &intercept_res);
    if (pset_tmp.num == 1) {
      momentum[counter] = pset_tmp[0]->team;
    } else {
      momentum[counter] = 0;
    }
    counter = (counter + 1 ) % 5;
    for(int i=0; i<MAX_MOMENTUM;i++){
      summe += momentum[i];
    }
    saved_team_in_attack = summe / MAX_MOMENTUM;

    pset_tmp = WSinfo::valid_teammates;
    pset_tmp.keep_players_in_circle(WSinfo::ball->pos, ServerOptions::kickable_area);
    if (pset_tmp.num > 0) {
      teammate_last_at_ball_number = pset_tmp[0]->number;
      teammate_last_at_ball_time = WSinfo::ws->time;
    }

    pset_tmp = WSinfo::valid_opponents;
    pset_tmp.keep_players_in_circle(WSinfo::ball->pos, ServerOptions::kickable_area);
    if (pset_tmp.num > 0) {
      opponent_last_at_ball_number = pset_tmp[0]->number;
      opponent_last_at_ball_time = WSinfo::ws->time;
    }
    
    if(teammate_last_at_ball_time >= opponent_last_at_ball_time)
      saved_team_last_at_ball = 0;
    else
      saved_team_last_at_ball = 1;
  }   
}

void WSmemory::update_offside_line_history() {
  if(last_update_at == WSinfo::ws->time) // ridi: do update only once per cycle
    return;
  his_offside_line_lag2 = his_offside_line_lag1;
  his_offside_line_lag1 = his_offside_line_lag0;
  his_offside_line_lag0 = WSinfo::his_team_pos_of_offside_line();
}

Value WSmemory::get_his_offsideline_movement() {
#define TOLERANCE 0.4

  if(his_offside_line_lag0 > his_offside_line_lag1 + TOLERANCE)
    return 1.; // offsideline moves forward
  if(his_offside_line_lag0 < his_offside_line_lag1 - TOLERANCE)
    return -1.; // offsideline moves back;
  // no movement between now and previous, check lag 2
  if(his_offside_line_lag0 > his_offside_line_lag2 + TOLERANCE)
    return 1.; // offsideline moves forward
  if(his_offside_line_lag0 < his_offside_line_lag2 - TOLERANCE)
    return -1.; // offsideline moves backward
  return 0;
}

void WSmemory::update() {
  if(WSinfo::ws->time_of_last_update == WSinfo::ws->time) {
    add_view_info(WSinfo::ws->time,WSinfo::ws->view_angle,
		  WSinfo::ws->view_quality,WSinfo::me->neck_ang,WSinfo::me->pos);
  }
  if(WSinfo::is_ball_kickable())
    ball_was_kickable4me_at = WSinfo::ws->time;
  update_fastest_to_ball();
  update_offside_line_history();
  last_update_at = WSinfo::ws->time;
}

void WSmemory::add_view_info(long time,int vang,int vqty,ANGLE vdir,Vector ppos) {
  if(vqty == Cmd_View::VIEW_QUALITY_LOW) return;
  for(int i=MAX_VIEW_INFO-2;i>=0;i--) {
    view_info[i+1]=view_info[i];
  }
  view_info[0].time=time;
  view_info[0].view_width=Tools::get_view_angle_width(vang);
  view_info[0].view_dir=vdir;
  view_info[0].ppos=ppos;
}

long WSmemory::last_seen_in_dir(ANGLE dir) {
  if(WSinfo::ws->ms_time_of_see<0)
    return WSinfo::ws->time - WSinfo::ws->time_of_last_update;
  for(int i=0;i<MAX_VIEW_INFO;i++) {
    if(view_info[i].time < 0) continue;
    Value dir_diff = fabs((view_info[i].view_dir-dir).get_value_mPI_pPI());
    if(dir_diff < .5*view_info[i].view_width.get_value()) {
      return WSinfo::ws->time - view_info[i].time;
    }
  }
  return (WSinfo::ws->time +1); // ridi: return time+1 instead of -1
}
    
long WSmemory::last_seen_to_point(Vector target) {
  if(WSinfo::ws->ms_time_of_see<0)
    return WSinfo::ws->time - WSinfo::ws->time_of_last_update;
  for(int i=0;i<MAX_VIEW_INFO;i++) {
    if(view_info[i].time < 0) continue;
    if((target-view_info[i].ppos).sqr_norm()>SQUARE(MAX_VISIBLE_DISTANCE)) continue;
    ANGLE tmp_dir = (target-view_info[i].ppos).ARG();
    Value dir_diff = fabs((view_info[i].view_dir-tmp_dir).get_value_mPI_pPI());
    if(dir_diff < .5*view_info[i].view_width.get_value()) {
      return WSinfo::ws->time - view_info[i].time;
    }
  }
  return (WSinfo::ws->time +1); // ridi: return time+1 instead of -1
}




/* I won't implement this without having PolicyTools independent of mdpInfo! */
#if 0
/* code taken from MDPmemory::update(), should be checked for correctness! */
void WSmemory::update_attack_info() {
  Vector ball, player;
  if(WSinfo::is_ball_pos_valid() {
    int summe = 0;
#endif
