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

/* Author: Manuel "Sputnick" Nickschas
 *
 * This file contains structs that hold the server and player parameters as
 * well as the player types.
 * Each of those struct has a function that parses the param message sent from the
 * server. The ServerParam struct also reads default values from the config file
 * during initialisation (most options will be overwritten by the server_param message
 * afterwards).
 *
 * NOTE: The server config file won't be read for now, maybe it will implemented later... 
 *
 */

#ifndef _PARAM_H
#define _PARAM_H

#include "defs.h"

/** Contains all data sent with the player_param message by the server. */
struct PlayerParam {

  static int player_types;
  static int subs_max;
  static int pt_max;
  static Value player_speed_max_delta_min;
  static Value player_speed_max_delta_max;
  static Value stamina_inc_max_delta_factor;
  static Value new_stamina_inc_max_delta_factor;
  static Value player_decay_delta_min;
  static Value player_decay_delta_max;
  static Value inertia_moment_delta_factor;
  static Value dash_power_rate_delta_min;
  static Value dash_power_rate_delta_max;
  static Value new_dash_power_rate_delta_min;
  static Value new_dash_power_rate_delta_max;
  static Value player_size_delta_factor;
  static Value kickable_margin_delta_min;
  static Value kickable_margin_delta_max;
  static Value kick_rand_delta_factor;
  static Value extra_stamina_delta_min;
  static Value extra_stamina_delta_max;
  static Value effort_max_delta_factor;
  static Value effort_min_delta_factor;
  static Value random_seed;
  
  static void init();
  static bool parseMsg(const char*);
};

/** Contains all data sent with the player_type message by the server. */
struct PlayerType {
  
  int id;
  Value player_speed_max;
  Value stamina_inc_max;
  Value player_decay;
  Value inertia_moment;
  Value dash_power_rate;
  Value player_size;
  Value kickable_margin;
  Value kick_rand;
  Value extra_stamina;
  Value effort_max;
  Value effort_min;

  /* these values are calculated */
  Value real_player_speed_max;
  Value dash_to_keep_max_speed;
  Value stamina_demand_per_meter;
  Value speed_progress[5];

  Value stamina_10m;
  Value stamina_20m;
  Value stamina_30m;
  
  void init();
  bool parseMsg(const char*);
};

/** Contains all data sent with the server_param message by the server.
    WARNING: These are not all parameters that server.conf may contain!
*/

struct ServerParam {

  static Value goal_width;
  static Value inertia_moment;
  static Value player_size;
  static Value player_decay;
  static Value player_rand;
  static Value player_weight;
  static Value player_speed_max;
  static Value player_accel_max;
  static Value stamina_max;
  static Value stamina_inc_max;
  static Value recover_init;
  static Value recover_dec_thr;
  static Value recover_min;
  static Value recover_dec;
  static Value effort_init;
  static Value effort_dec_thr;
  static Value effort_min;
  static Value effort_dec;
  static Value effort_inc_thr;
  static Value effort_inc;
  static Value kick_rand;
  static bool team_actuator_noise;
  static Value prand_factor_l;
  static Value prand_factor_r;
  static Value kick_rand_factor_l;
  static Value kick_rand_factor_r;
  static Value ball_size;
  static Value ball_decay;
  static Value ball_rand;
  static Value ball_weight;
  static Value ball_speed_max;
  static Value ball_accel_max;
  static Value dash_power_rate;
  static Value kick_power_rate;
  static Value kickable_margin;
  static Value control_radius;
  static Value control_radius_width;
  static Value maxpower;
  static Value minpower;
  static Value maxmoment;
  static Value minmoment;
  static Value maxneckmoment;
  static Value minneckmoment;
  static Value maxneckang;
  static Value minneckang;
  static Value visible_angle;
  static Value visible_distance;
  static Value wind_dir;
  static Value wind_force;
  static Value wind_ang;
  static Value wind_rand;
  static Value kickable_area;
  static Value catchable_area_l;
  static Value catchable_area_w;
  static Value catch_probability;
  static int goalie_max_moves;
  static Value ckick_margin;
  static Value offside_active_area_size;
  static bool wind_none;
  static bool wind_random;
  static int say_coach_cnt_max;
  static int say_coach_msg_size;
  static int clang_win_size;
  static int clang_define_win;
  static int clang_meta_win;
  static int clang_advice_win;
  static int clang_info_win;
  static int clang_mess_delay;
  static int clang_mess_per_cycle;
  static int clang_del_win;
  static int clang_rule_win;
  static int freeform_send_period;
  static int freeform_wait_period;
  static int half_time;
  static int simulator_step;
  static int send_step;
  static int recv_step;
  static int sense_body_step;
  static int lcm_step;
  static int say_msg_size;
  static int hear_max;
  static int hear_inc;
  static int hear_decay;
  static int catch_ban_cycle;
  static int slow_down_factor;
  static bool use_offside;
  static bool forbid_kick_off_offside;
  static Value offside_kick_margin;
  static Value audio_cut_dist;
  static Value quantize_step;
  static Value quantize_step_l;
  static Value quantize_step_dir;
  static Value quantize_step_dist_team_l;
  static Value quantize_step_dist_team_r;
  static Value quantize_step_dist_l_team_l;
  static Value quantize_step_dist_l_team_r;
  static Value quantize_step_dir_team_l;
  static Value quantize_step_dir_team_r;
  static bool coach;
  static bool coach_w_referee;
  static bool old_coach_hear;
  static int send_vi_step;
  static int start_goal_l;
  static int start_goal_r;
  static bool fullstate_l;
  static bool fullstate_r;
  static int drop_ball_time;
  static int port;
  static int coach_port;
  static int olcoach_port;
  static int verbose;
  static int replay;
  static int synch_mode;
  static int synch_offset;
  static int synch_micro_sleep;

  static int max_goal_kicks;
  static int point_to_ban;
  static int point_to_duration;
  static int tackle_cycles;
  static int back_passes;
  static int free_kick_faults;
  static int proper_goal_kicks;
  static int record_messages;
  static int send_comms;
  static Value stopped_ball_vel;
  static Value tackle_back_dist;
  static Value tackle_dist;
  static Value tackle_exponent;
  static Value tackle_power_rate;
  static Value tackle_width;

  static void init();
  static bool parseMsg(const char*);
};
  
  
  


#if 0
struct ServerParam {

  static Value maxneckmoment;
  static Value minneckmoment;
  static Value maxneckang;
  static Value minneckang;
  static Value offside_kick_margin;

  static Value goal_width;               
  static Value player_size;              
  static Value player_decay;             
  static Value player_rand;              
  static Value player_weight;            
  static Value player_speed_max;
  static Value player_accel_max;
  static Value stamina_max;        
  static Value stamina_inc_max;        
  static Value recover_dec_thr;
  static Value recover_dec;        
  static Value recover_min;        
  static Value effort_dec_thr;      
  static Value effort_dec;
  static Value effort_inc_thr;      
  static Value effort_inc;         
  static Value effort_min;
  static int hear_max;
  static int hear_inc;
  static int hear_decay;
  static Value audio_cut_dist;
  static Value inertia_moment;
  static Value catchable_area_l;
  static Value catchable_area_w;
  static Value catch_probability;
  static int catch_ban_cycle;
  static int goalie_max_moves;
  static Value ball_size;                
  static Value ball_decay;               
  static Value ball_rand;                
  static Value ball_weight;              
  static Value ball_speed_max;
  static Value ball_accel_max;
  static Value wind_force;
  static Value wind_dir;
  static Value wind_rand;
  static Value kick_margin;
  static Value kickable_margin;
  static Value kick_rand;
  static Value ckick_margin;
  static Value corner_kick_margin;
  static Value dash_power_rate;
  static Value kick_power_rate;
  static Value visible_angle;
  static Value quantize_step;
  static Value quantize_step_l;
  static Value maxpower;
  static Value minpower;
  static Value maxmoment;
  static Value minmoment;
  static int port;
  static int coach_port;
  static int simulator_step;
  static int send_step;
  static int recv_step;
  static int half_time;
  static int say_msg_size;
  static bool use_offside;
  static Value offside_active_area_size;
  static bool forbid_kick_off_offside;
  static bool verbose;
  static int record_version;
  static bool record_log;
  static bool send_log;
  static int sense_body_step;
  static int say_coach_msg_size;
  static int say_coach_cnt_max;
  static int send_vi_step;
  static Value server_port;
  static Value kickable_area;

  /* these are not from server.conf... */
  static Vector own_goal_pos;
  static Vector their_goal_pos;
  static Value pitch_length;
  static Value pitch_width;
  static Value penalty_area_width;
  static Value penalty_area_length;
  static Value ctlradius ;				/*< control radius */
  static Value ctlradius_width ;			/*< (control radius) - (plyaer size) */
  static Value maxn ;					/*< max neck angle */
  static Value minn ;					/*< min neck angle */
  static Value visible_distance ;			/*< visible distance */
  static int wind_no ;					/*< wind factor is none */
  static int wind_random ;				/*< wind factor is random */

  static int log_times;
  static int clang_win_size;
  static int clang_define_win;
  static int clang_meta_win;
  static int clang_advice_win;
  static int clang_info_win;
  static int clang_mess_delay;
  static int clang_mess_per_cycle;

  void init();
  bool readFromFile(const char *name);
  bool parseMsg(const char*);
};

#endif
#endif
