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

/*
 *  Author:   Artur Merke 
 */

#ifndef _WMSTATE_H_
#define _WMSTATE_H_

#include "wmdef.h"
#include "globaldef.h"
#include "sensorbuffer.h"
#include "angle.h"
#include "mdpstate.h"
#include "cmd.h"

struct WMtime {
  WMtime() { time= 0; cycle= 0; }
  WMtime(int t) { time= t; cycle= 0; }
  void operator=(int t) { time= t; cycle= 0; } 
  int operator()() { return time; };
  int time;
  int cycle;
};

ostream& operator<< (ostream& o, const WMtime & wmtime) ;

/** World Model state */
class WMstate {
public:
  void import_msg_mdpstate(const Msg_mdpstate &);
  void compare_msg_mdpstate(const Msg_mdpstate &) const;
  void export_mdpstate(MDPstate &) const;

  void incorporate_cmd_and_msg_sense_body(const Cmd &, const Msg_sense_body &);
  void incorporate_msg_hear(const Msg_hear &);
  void incorporate_msg_see(const Msg_see &);
  void compute_pos_from_markers(Vector & pos, Vector &  vel, ANGLE & n_ang, const Msg_see & see) const;
  void compute_pos_from_markers2(Vector & pos, Vector &  vel, ANGLE & n_ang, const Msg_see & see) const;

  WMtime time;
  PlayMode play_mode;
  int my_score;
  int his_score;
  int view_quality;
  int view_width;

  ANGLE my_angle;
  ANGLE my_neck_angle;
  Value my_effort;
  Value my_recovery;

  int kick_count;
  int dash_count;
  int turn_count;
  int say_count;
  int turn_neck_count;

  Value my_speed_value;
  ANGLE my_speed_angle;
  ANGLE my_neck_angle_rel;

  struct _wm_player {
    WMtime time;
    bool alive;
    Vector pos;
    Vector vel;
    Value  stamina;
  };

  struct _wm_ball {
    WMtime time;
    Vector pos;
    Vector vel;
  };

  _wm_player my_team[NUM_PLAYERS+1];
  _wm_player his_team[NUM_PLAYERS+1];
  _wm_ball ball;
};

#endif
