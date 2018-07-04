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

#include "intention.h"
#include "ws_info.h"

void Intention::confirm_intention(const Intention intention, const int valid_at){
  valid_at_cycle = valid_at; // update
  valid_since_cycle = intention.valid_since_cycle; 
  kick_speed = intention.kick_speed;
  kick_target = intention.kick_target;
  type = intention.type;
  player_target = intention.player_target;
  target_body_dir = intention.target_body_dir;
  target_player = intention.target_player;
  risky_pass = intention.risky_pass;
  potential_pos = intention.potential_pos;
}

void Intention::set_valid_since(const long cycle){
  valid_since_cycle = cycle;
}

long Intention::valid_since(){
  if(valid_at_cycle >0) 
    return valid_since_cycle;
  else
    return 999999;  // a large value to indicate that its not valid
}



void Intention::reset(){
  V=0;
  valid_at_cycle = 0; 
  valid_since_cycle = 999999;
  kick_speed = 0;
  kick_target = Vector(0);
  type = 0;
  player_target = Vector(0);
  target_body_dir = ANGLE(0);
  target_player = 0;
  risky_pass = false;
  potential_pos = Vector(0);
}

void Intention::set_goto(const Vector &target, const int valid_at) {
  type = GOTO;
  player_target = target;
  valid_at_cycle = valid_at;
  valid_since_cycle = valid_at;
}

void Intention::set_score(const Vector &target, const Value &speed, const int valid_at){
  type = SCORE;
  kick_speed = speed;
  kick_target = target;
  valid_at_cycle = valid_at;
  valid_since_cycle = valid_at;
}

void Intention::set_pass(const Vector &target, const Value &speed, const int valid_at, 
			 int target_player_number,const Value tmp_priority, const Vector ipos, 
			 const   Vector potential_position ){
  type = PASS;
  kick_speed = speed;
  kick_target = target;
  valid_at_cycle = valid_at;
  valid_since_cycle = valid_at;
  target_player= target_player_number;
  resultingpos= ipos;
  potential_pos = potential_position;
  priority = tmp_priority;
}


void Intention::set_laufpass(const Vector &target, const Value &speed, const int valid_at, 
			     int target_player_number,const Value tmp_priority, 
			     const Vector ipos,
			     const bool is_risky,const   Vector potential_position ){
  type = LAUFPASS;
  kick_speed = speed;
  kick_target = target;
  valid_at_cycle = valid_at;
  valid_since_cycle = valid_at;
  target_player= target_player_number;
  resultingpos = ipos;
  priority = tmp_priority;
  risky_pass = is_risky;
  potential_pos = potential_position;
}


void Intention::set_opening_seq(const Vector &target, const Value &speed, const int valid_at, int target_player_number){
  type = OPENING_SEQ;
  kick_speed = speed;
  kick_target = target;
  valid_at_cycle = valid_at;
  valid_since_cycle = valid_at;
  target_player= target_player_number;
}

void Intention::set_selfpass(const ANGLE & targetdir, const Vector &target, 
			     const Value &speed, const int valid_at){
  type = SELFPASS;
  target_body_dir = targetdir;
  kick_speed = speed;
  kick_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_immediateselfpass(const Vector &target, const Value &speed, const int valid_at){
  type = IMMEDIATE_SELFPASS;
  kick_speed = speed;
  kick_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}


void Intention::set_dribble(const Vector &target, const int valid_at){
  type = DRIBBLE;
  player_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_dribblequickly(const Vector &target, const int valid_at){
  type = DRIBBLE_QUICKLY;
  player_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}


void Intention::set_kicknrush(const Vector &target, const Value &speed, const int valid_at){
  type = KICKNRUSH;
  kick_speed = speed;
  kick_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_panic_kick(const Vector &target, const int valid_at){
  type = PANIC_KICK;
  kick_target = target;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_tackling( const Value &speed, const int valid_at )
{
  type = TACKLING;
  kick_speed = speed;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_backup(const int valid_at){
  type = BACKUP;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_turnanddash(const int valid_at){
  type = TURN_AND_DASH;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_waitandsee(const int valid_at){
  type = WAITANDSEE;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}

void Intention::set_holdturn(const ANGLE body_dir, const int valid_at){
  type = HOLDTURN;
  target_body_dir = body_dir;
  valid_since_cycle = valid_at;
  valid_at_cycle = valid_at;
}


int Intention::get_type(){
  return type;
}

bool Intention::get_kick_info(Value &speed, Vector &target, int & target_player_number){
  if(type <SELFPASS || type > PANIC_KICK){
    speed  = 0.0;
    target = Vector(0);
    return false;
  }
  speed = kick_speed;
  target = kick_target;
  target_player_number= target_player;
  return true;
}

/********************************************************************/

void NeckRequest::set_request(int t,Value p) {
  valid_at_cycle = WSinfo::ws->time;
  type = t;
  p0 = p;
}

int NeckRequest::get_type() {
  if(valid_at_cycle != WSinfo::ws->time) return NECK_REQ_NONE;
  return type;
}

Value NeckRequest::get_param() {
  if(valid_at_cycle != WSinfo::ws->time) {
return 0;
}
  return p0;
}

bool NeckRequest::is_set() {
  return (valid_at_cycle == WSinfo::ws->time);
}
