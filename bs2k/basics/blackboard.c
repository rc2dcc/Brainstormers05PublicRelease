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

#include "blackboard.h"
#include "cmd.h"
#include "ws_info.h"

Intention Blackboard::main_intention;
Intention Blackboard::pass_intention;
Blackboard::ViewInfo Blackboard::viewInfo;
NeckRequest Blackboard::neckReq;

bool Blackboard::need_goal_kick;
bool Blackboard::force_highq_view;
bool Blackboard::force_wideang_view;

bool Blackboard::get_pass_info(const int current_time, Value &speed, Vector & target, int & target_player_number){
  if (pass_intention.valid_at() != current_time)
    return false;
  return pass_intention.get_kick_info(speed,target, target_player_number);
}

void Blackboard::init() {
  viewInfo.ang_set=-1;
  viewInfo.last_lowq_cycle = -1;
  viewInfo.ball_pos_set = -1;
  force_highq_view=false;
  force_wideang_view=false;
}

void Blackboard::set_next_view_angle_and_quality(int ang,int qty) {
  viewInfo.ang_set = WSinfo::ws->time;
  viewInfo.next_view_angle = ang;
  viewInfo.next_view_qty = qty;
}

int Blackboard::get_next_view_angle() {
  if(viewInfo.ang_set<WSinfo::ws->time) {
    ERROR_OUT << "WARNING: View behavior has not set next_view_angle!";
    return Cmd_View::VIEW_ANGLE_NARROW;
  }
  return viewInfo.next_view_angle;
}

int Blackboard::get_next_view_quality() {
  if(viewInfo.ang_set<WSinfo::ws->time) {
    ERROR_OUT << "WARNING: View behavior has not set next_view_quality!";
    return Cmd_View::VIEW_QUALITY_HIGH;
  }
  return viewInfo.next_view_qty;
}

void Blackboard::set_last_lowq_cycle(long cyc) {
  viewInfo.last_lowq_cycle = cyc;
}

long Blackboard::get_last_lowq_cycle() {
  return viewInfo.last_lowq_cycle;
}

void Blackboard::set_last_ball_pos(Vector pos) {
  viewInfo.ball_pos_set = WSinfo::ws->time;
  viewInfo.last_ball_pos = pos;
}

Vector Blackboard::get_last_ball_pos() {
  if(viewInfo.ball_pos_set == -1) {
    return Vector(0,0);
  }
  return viewInfo.last_ball_pos;
}

/**********************************************************************/

void Blackboard::set_neck_request(int type, Value param) {
  neckReq.set_request(type,param);
}

int Blackboard::get_neck_request(Value &param) {
  if(!neckReq.is_set()) return NECK_REQ_NONE;
  param = neckReq.get_param();
  return neckReq.get_type();
}
