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
#include "ws.h"
#include "cmd.h"
#include "wmoptions.h"
#include "joystick.h"
#include "pfilter.h"

struct WMtime {
  WMtime() { reset(); }
  //WMtime(int t) { time= t; cycle= 0; total_cycle= 0; }
  void reset() { time= -100; cycle= 0; total_cycle= 0;}
  void reset2() { time= -101; cycle= 0; total_cycle= 0;}

  bool after_reset() const { return time <= -100; }
  bool after_reset2() const { return time <= -101; }
  
  //void operator=(int t) { time= t; cycle= 0; } 
  bool operator==(const WMtime & t) const { return time == t.time && cycle == t.cycle; } 
  bool operator!=(const WMtime & t) const { return time != t.time || cycle != t.cycle; } 
  bool operator<=(const WMtime & t) const { return time < t.time || (time == t.time && cycle <= t.cycle); } 
  
  int operator()() { return time; };
  int time;
  int cycle;
  int total_cycle;
};

std::ostream& operator<< (std::ostream& o, const WMtime & wmtime) ;

class PSet {
  bool players[2][NUM_PLAYERS+1];
 public:
  PSet() {}
  void set_all();
  void unset_all();
  bool get(int team, int num) const { return players[team][num]; }
  void set(int team, int num) { players[team][num]= true; }
  void unset(int team, int num) { players[team][num]= false; }
};

/** World Model state */
class WMstate {
 public:
  void init();

  double my_distance_to_ball() {
    return ball.pos.distance( my_team[WM::my_number].pos );
  }

  long ms_time_between_see_messages() {
    long res= 150;
    if ( view_quality == LOW )
      res /= 2;
    if ( view_width == WIDE )
      res *= 2;
    else if ( view_width == NARROW)
      res /= 2;
    return res;
  }

  void import_msg_fullstate(const Msg_fullstate &);
  void import_msg_fullstate(const Msg_fullstate_v8 &);
  void incorporate_msg_fullstate_wrt_view_area(const Msg_fullstate &);
  void compare_with_wmstate(const WMstate &) const;
  void show_object_info() const;
  void export_mdpstate(MDPstate &) const;
  void export_ws(WS &) const;

  void export_msg_teamcomm(Msg_teamcomm &) const;
  void export_msg_teamcomm(Msg_teamcomm2 &,const Cmd_Say & say) const;

  void incorporate_cmd_and_msg_sense_body(const Cmd &, const Msg_sense_body &);
  void incorporate_cmd(const Cmd &);

  void incorporate_msg_hear(const Msg_hear &);
  void incorporate_msg_see(const Msg_see &, long ms_time, long ms_time_delay);
  void incorporate_msg_change_player_type(const Msg_change_player_type &);
  void incorporate_joystick_info(const Joystick & joystick);

  void reduce_dash_power_if_possible(Cmd_Main & cmd) const;
 protected:
  double determine_KONV(int user_space_play_mode) const;
  void check_statistics() const;
  void export_unknown_player(Vector pos, WS::Player & wstate_p, double KONV ) const;
  void export_ws_player( int team, int number, WS::Player & wstate_p, double KONV ) const;
  void incorporate_msg_teamcomm(const Msg_teamcomm &);
  void incorporate_msg_teamcomm(int time, const Msg_teamcomm2 &);
  int server_playmode_to_player_playmode(PlayMode server_playmode) const;
  void update_me_from_msg_see(const Msg_see & see);
  void update_ball_from_msg_see(const Vector & my_pos_before_update, const Msg_see & see);
  void update_players_from_msg_see(const Msg_see & see);
  void handle_inconsistent_objects_in_view_area();
  void stop_ball_in_immediate_vicinity_of_other_players(); //heuristics, shuould be done by the agent himself (in the future)

  double time_diff_ball_to_mdpstate_probability(int tdiff) const { if (tdiff > 7) return 0.0; return 1.0;  }
  double time_diff_player_to_mdpstate_probability(int team, int tdiff) const { 
    if ( team == my_TEAM ) {
      if (tdiff > WMoptions::max_cycles_to_forget_my_player) 
	return 0.0; 
      return 1.0;  
    }
    if (tdiff > WMoptions::max_cycles_to_forget_his_player) 
      return 0.0;
    return 1.0;
  }
 public:
  WMtime time;
  WMtime time_of_last_msg_see;
  long   ms_time_of_last_msg_see;
  long   ms_time_of_last_msg_see_after_sb;
  PlayMode play_mode;
  int    penalty_side;
  int    penalty_count; //only counts own penalties

  int my_score;
  int his_score;
  int view_quality; ///< takes values {HIGH,LOG}
  int view_width;  ///< takes values {WIDE,NORMAL,NARROW}
  int next_cycle_view_quality;  
  int next_cycle_view_width;  

  int my_goalie_number;  ///< value 0 means unknown, value -1 means not existent
  int his_goalie_number; ///< value 0 means unknown, value -1 means not existent

  int my_attentionto;
  int my_attentionto_duration;

  ANGLE my_angle;
  ANGLE my_neck_angle;
  double my_effort;
  double my_recovery;

  int kick_count;
  int dash_count;
  int turn_count;
  int say_count;
  int turn_neck_count;
  int catch_count;
  int move_count;
  int change_view_count;

  double my_speed_value;
  ANGLE my_speed_angle;
  ANGLE my_neck_angle_rel;

  struct _teamcomm_statistics {
    _teamcomm_statistics() { teamcomm_count= 0; teamcomm_partial_count= 0; pass_info_count= 0; }
    WMtime send_time_of_last_teamcomm;
    int    sender_of_last_teamcomm;
    WMtime recv_time_of_last_teamcomm;
    int    teamcomm_count;
    int    pass_info_count;
    int    teamcomm_partial_count;
  };

  struct _wm_player {
    _wm_player() { type= -1; pass_info.valid= false; }
    bool alive;  ///< if a player is not alive, don't rely on the other values


    WMtime time_pos; ///< time of last update
    WMtime time_vel;
    WMtime time_angle;

    Vector pos;  
    Vector vel;
    ANGLE body_angle;
    ANGLE neck_angle; //this is an absolute angle!
    double stamina;
    int type;
    
    bool tackle_flag;  //true if the oppoent is tackling (it's a snapshop from time 'time')
    bool pointto_flag; //pointto_dir is only valid, if pointto_flag == true
    ANGLE pointto_dir;

    struct {
      bool valid;
      int recv_time;
      Vector ball_pos;
      Vector ball_vel;
      int time; //this is the absolute time when ball_pos and ball_vel will be valid!
    } pass_info;
    
    bool unsure_number;
  };

  struct _unknown_players {
    WMtime time;
    static const int max_num= 3;
    int num;
    Vector pos[max_num];
  };
  
  struct _wm_ball {
    WMtime time_pos;
    WMtime time_vel;
    Vector pos;
    Vector vel;
  };

  struct _wm_me_and_ball {
    WMtime time;
    Vector old_ball_rel_pos;
    Vector approx_ball_rel_pos;
    //Vector my_vel;
    Vector my_move;
    Vector my_old_pos;
    bool probable_collision;
  };

  struct _joystick_info {
    _joystick_info() { valid= false; }
    bool valid;
    Vector stick1;
    Vector stick2;
  };

  _teamcomm_statistics teamcomm_statistics;
  _unknown_players unknown_players;
  _wm_player my_team[NUM_PLAYERS+1];
  _wm_player his_team[NUM_PLAYERS+1];
  _wm_ball ball;
  _wm_me_and_ball me_and_ball;
  _joystick_info joystick_info;
 protected:
  ParticleFilter pfilter;

  int get_obj_id(int team, int number) const {
    if ( team== my_TEAM )
      return number;
    return number+NUM_PLAYERS;
  }
  int get_obj_team(int obj_id) const {
    if (obj_id <= NUM_PLAYERS)
      return my_TEAM;
    return his_TEAM;
  }
  int get_obj_number(int obj_id) const {
    if (obj_id <= NUM_PLAYERS)
      return obj_id;
    return obj_id -11;
  }
  static const int ball_id= 0;
  //static const int ball_vel_id= NUM_PLAYERS*2+1;
  static const int obj_id_MAX_NUM= NUM_PLAYERS*2+2;
  int teamcomm_times_buffer[obj_id_MAX_NUM];
  int * teamcomm_times;   //just a hack to allow writing to teamcomm_time in a const method (very ugly!!)

  Vector compute_object_pos(const Vector & observer_pos, ANGLE observer_neck_angle_abs, 
			    double dist, double dir) const;
  ///very sophisticated method to compute velocity of an object (see the implementation for more documentation)
  Vector compute_object_vel(const Vector & observer_pos, const Vector & observer_vel, 
			    const Vector & object_pos, double dist, double dist_change, double dir_change) const;

  ///simulates a collision of the object with an obstacle, position of the obstacle never changes!
  Vector compute_object_pos_after_collision(const Vector & object_pos, const Vector & object_vel, double object_radius, const Vector obstacle_pos, double obstacle_radius) const; 
  //Vector compute_object_pos_after_collision(const Vector & object_pos, const Vector & object_vel, double object_radius, const Vector & obstacle_pos, const Vector & obstacle_pos, double obstacle_radius) const; 

  ///simulates a collision of the object with an obstacle, position of the obstacle never changes!
  Vector compute_object_vel_after_collision(const Vector & object_vel) const { return -0.1*object_vel; }

  ///gets a reference to a player
  inline _wm_player * ref_player(int team, int number);
  ///gets a constant reference to a player
  inline const _wm_player * const_ref_player(int team, int number) const;

  ///gets a constant reference to a player using an object id
  inline const _wm_player * const_ref_player(int obj_id) const;

  ///only not too old players are considered (max_age), the result is the minimal distance squared!!!
  bool get_nearest_player_to_pos(int team, const Vector & pos, const PSet & players, int max_age, int & res_team, int & res_number, double & sqr_res_distance) const;


  ///
  bool in_feel_area(const Vector & pos, const Vector & object_pos, double tolerance= 0.0) const;

  ///
  bool in_view_area(const Vector & pos, const ANGLE & neck_angle, int v_width, const Vector & object_pos) const;

  void show_view_area(const Vector & pos, const ANGLE & neck_angle, int v_width) const;

};

WMstate::_wm_player * WMstate::ref_player(int team, int number) {
  if (number < 0 || number > NUM_PLAYERS )
    return 0;
  if (team == my_TEAM)
    return my_team+number;
    if (team == his_TEAM)
      return his_team+number;
    return 0;
}

const WMstate::_wm_player * WMstate::const_ref_player(int team, int number) const {
  if (number < 0 || number > NUM_PLAYERS )
    return 0;
  if (team == my_TEAM)
    return my_team+number;
  if (team == his_TEAM)
    return his_team+number;
  return 0;
}

const WMstate::_wm_player * WMstate::const_ref_player(int obj_id) const {
  if (obj_id <= NUM_PLAYERS)
    return const_ref_player(my_TEAM,obj_id);
  return const_ref_player(his_TEAM,obj_id-NUM_PLAYERS);
}

#endif
