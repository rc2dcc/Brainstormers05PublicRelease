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

#ifndef _BLACKBOARD_
#define _BLACKBOARD_

#include "intention.h"

class Blackboard {

  /* stuff for view behavior */
  struct ViewInfo {
    long ang_set;
    int next_view_angle;
    int next_view_qty;
    Vector last_ball_pos;
    long ball_pos_set;
    long last_lowq_cycle;
  };
  static ViewInfo viewInfo;
  
  
 public:
  static NeckRequest neckReq;
  static Intention main_intention;
  static Intention pass_intention;
  static bool get_pass_info(const int current_time, Value &speed, Vector & target, int & target_player_number);

  /* stuff for view behavior */
  static int get_next_view_angle();        // planned view angle for next cycle
  static int get_next_view_quality();      // planned view quality for next cycle
  static void set_next_view_angle_and_quality(int,int);
  static long get_last_lowq_cycle();       // last cycle where we had only LOW QUALITY view
  static void set_last_lowq_cycle(long);
  static Vector get_last_ball_pos();       // last position ball had when we saw something
  static void set_last_ball_pos(Vector);

  static bool force_highq_view;            // interrupt any view sync behavior
  static bool force_wideang_view; 
 
  /* stuff for neck behavior */
  static void set_neck_request(int req_type, Value param = 0);
  static int get_neck_request(Value &param);    // returns NECK_REQ_NONE if not set

  /* stuff for goal kick */
  static bool need_goal_kick;
  
  static void init();
};


#endif
