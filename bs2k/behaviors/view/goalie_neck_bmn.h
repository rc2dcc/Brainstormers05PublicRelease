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

/** \file policy_dynamic_neck_goalie.h
*/

#ifndef _GOALIE_NECK_BMN_H_
#define _GOALIE_NECK_BMN_H_

#include "../base_bm.h"
//#include "move_factory.h"
#include "../policy/policy_tools.h"
#include "neck_cmd_bms.h"
//#include "globals.h"

/**
 */
class GoalieNeck : public NeckBehavior {
  static bool initialized;
  
  NeckCmd *neck_cmd;

 public:

 GoalieNeck();
 virtual ~GoalieNeck();

  /** get the unique id of the policy.
      \return policy id.
  */ 
 bool get_cmd(Cmd &cmd);

 static bool init(char const * conf_file, int argc, char const* const* argv) {
   if(initialized) return true;
   initialized = true;
   std::cout << "GoalieNeck behavior initialized.";
   return ( NeckCmd::init(conf_file, argc, argv) ); 
 }

 private:
  Go2Ball_Steps* go2ball_list;
  struct{
    float me;
    float my_goalie;
    float teammate;
    int teammate_number;
    Vector teammate_pos;
    Bool ball_kickable_for_teammate;
    float opponent;
    int opponent_number;
    Vector opponent_pos;
    Bool ball_kickable_for_opponent;
  } steps2go;
  
  int time_of_turn;
  int time_attacker_seen;
  int last_time_look_to_ball;
  Value turnback2_scandir;
  void compute_steps2go();
  bool goalie_neck(Cmd &cmd);
  bool neck_standard(Cmd &cmd);
  bool neck_lock(Cmd &cmd);
};

#endif //_GOALIE_NECK_BMN_H_

