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

#ifndef _WS_MEMORY_H_
#define _WS_MEMORY_H_

/* Author: Manuel Nickschas */

#include "tools.h"

class WSmemory {

  struct ViewInfo {
    ANGLE view_width;
    ANGLE view_dir;
    Vector ppos;
    long time;
  };

  static ViewInfo view_info[];
  
  static void add_view_info(long cyc, int view_ang, int view_qty, ANGLE view_dir,Vector ppos);

  static const int MAX_MOMENTUM=5;
  static float momentum[MAX_MOMENTUM];
  static int counter;
  static int opponent_last_at_ball_number;
  static long opponent_last_at_ball_time;
  static long teammate_last_at_ball_time;
  static int saved_team_last_at_ball;
  static int saved_team_in_attack; //gibt an, welches Team im Angriff ist
  static Value his_offside_line_lag2,his_offside_line_lag1,his_offside_line_lag0;
  static void update_offside_line_history();
  static int last_update_at;

 public:

  //jk change: was private
  static int teammate_last_at_ball_number; // include myself!

  static long ball_was_kickable4me_at;

  /* ridi 18.6.03: these return (ws->time +1) if we haven't seen object within
     the last MAX_VIEW_INFO cycles, this makes the use much easier */
  static long last_seen_in_dir(ANGLE direction);
  static long last_seen_to_point(Vector point);
  
  static int team_last_at_ball() {
    return saved_team_last_at_ball;
  }
  static int team_in_attack() {
    return saved_team_in_attack;
  }

  static void update();

  static Value get_his_offsideline_movement();

  static void update_fastest_to_ball();

  static void init();


};

#endif
