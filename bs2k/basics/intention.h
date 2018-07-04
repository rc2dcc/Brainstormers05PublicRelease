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

#ifndef _INTENTION_H
#define _INTENTION_H

#include "Vector.h"
#include "angle.h"

#define SELFPASS 1
#define IMMEDIATE_SELFPASS 2
#define PASS 3
#define LAUFPASS 4
#define KICKNRUSH 5
#define SCORE 6
#define OPENING_SEQ 7
#define PANIC_KICK 8
#define TACKLING 9

#define DRIBBLE 10
#define SOLO 11
#define WAITANDSEE 12
#define HOLDTURN 13

#define TURN_AND_DASH 20
#define DRIBBLE_QUICKLY 21

#define GOTO 30
#define GO2BALL 31
#define STAY 32
#define GOHOME 33
#define BACKUP 34

/* we could add more here... */
enum { NECK_REQ_NONE, NECK_REQ_LOOKINDIRECTION, NECK_REQ_PASSINDIRECTION, NECK_REQ_BLOCKBALLHOLDER,
       NECK_REQ_SCANFORBALL, NECK_REQ_FACEBALL, NECK_REQ_DIRECTOPPONENTDEFENSE };

class Intention{
 private:
  long valid_since_cycle;  // cycle at which intention was first initialized
  long valid_at_cycle;  // cycle at which intention is valid
 public:
  Value V;
  bool risky_pass;
  bool immediatePass;
  int type;
  int target_player;
  Value priority; // gives priority/ quality of a pass or laufpass
  Vector resultingpos;
  ANGLE target_body_dir;
  Value kick_speed;
  Vector kick_target;
  Vector player_target; // for go2s...
  Vector potential_pos;

  void set_pass(const Vector &target, const Value &speed, const int valid_at, int target_player_number= 0,
		Value priority = 0, const Vector ipos = Vector(0), const   Vector potential_position = Vector(0));
  void set_laufpass(const Vector &target, const Value &speed, const int valid_at, int target_player_number= 0,
		    Value priority = 0, const Vector ipos = Vector(0),const bool is_risky = false, 
		    const   Vector potential_position = Vector(0));
  void set_opening_seq(const Vector &target, const Value &speed, const int valid_at, 
		       int target_player_number= 0);
  void set_selfpass(const ANGLE & targetdir, const Vector &target, const Value &speed, const int valid_at);
  void set_immediateselfpass(const Vector &target, const Value &speed, const int valid_at);
  void set_dribble(const Vector &target, const int valid_at);
  void set_dribblequickly(const Vector &target, const int valid_at);
  void set_holdturn(const ANGLE body_dir, const int valid_at);
  void set_score(const Vector &target, const Value &speed, const int valid_at);
  void set_kicknrush(const Vector &target, const Value &speed, const int valid_at);
  void set_panic_kick(const Vector &target, const int valid_at);
  void set_tackling( const Value &speed, const int valid_at );    
  void set_backup(const int valid_at);
  void set_waitandsee(const int valid_at);
  void set_turnanddash(const int valid_at);
  void set_goto(const Vector &target, const int valid_at);
  void confirm_intention(const Intention intention, const int valid_at);
  void correct_target(const Vector &target){kick_target = target;};
  void correct_speed(const Value &speed){kick_speed = speed;};
  void reset();
  int get_type();
  long valid_at(){return valid_at_cycle;};
  long valid_since();
  void set_valid_since(const long cycle);
  bool get_kick_info(Value &speed, Vector &target, int & target_player_number); // false if intention is not a shoot
  bool get_kick_info(Value &speed, Vector &target) { // false if intention is not a shoot
    int dum;
    return get_kick_info(speed,target,dum);
  } 
};

class NeckRequest {
  int valid_at_cycle;
  int type;
  Value p0;
 public:
  void set_request(int type, Value p0 = 0);
  int get_type();
  Value get_param();
  bool is_set();

  NeckRequest() { valid_at_cycle = -1;}
};
#endif 
