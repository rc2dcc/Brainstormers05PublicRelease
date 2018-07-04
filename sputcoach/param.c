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
 * See param.h for description.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "param.h"
#include "str2val.h"
#include "options.h"

/******************************************
 * PlayerParam
 ******************************************/

int PlayerParam::player_types;
int PlayerParam::subs_max;
int PlayerParam::pt_max;
Value PlayerParam::player_speed_max_delta_min;
Value PlayerParam::player_speed_max_delta_max;
Value PlayerParam::stamina_inc_max_delta_factor;
Value PlayerParam::new_stamina_inc_max_delta_factor;
Value PlayerParam::player_decay_delta_min;
Value PlayerParam::player_decay_delta_max;
Value PlayerParam::inertia_moment_delta_factor;
Value PlayerParam::dash_power_rate_delta_min;
Value PlayerParam::dash_power_rate_delta_max;
Value PlayerParam::new_dash_power_rate_delta_max;
Value PlayerParam::new_dash_power_rate_delta_min;
Value PlayerParam::player_size_delta_factor;
Value PlayerParam::kickable_margin_delta_min;
Value PlayerParam::kickable_margin_delta_max;
Value PlayerParam::kick_rand_delta_factor;
Value PlayerParam::extra_stamina_delta_min;
Value PlayerParam::extra_stamina_delta_max;
Value PlayerParam::effort_max_delta_factor;
Value PlayerParam::effort_min_delta_factor;
Value PlayerParam::random_seed;

void PlayerParam::init() {}

bool PlayerParam::parseMsg(const char *str) {

  const char *origstr=str;
  cout << "PlayerParam Message:\n\n" << str;
  bool res;
  if(!Options::server_94) {
    res=(strskip(str,"(player_param ",str) &&
	 strskip(str,"(player_types",str) && str2val(str,player_types,str) && 
	 strskip(str,")(pt_max",str) && str2val(str,pt_max,str) && 
	 strskip(str,")(random_seed",str) && str2val(str,random_seed,str) &&
	 strskip(str,")(subs_max",str) && str2val(str,subs_max,str) &&
	 strskip(str,")(dash_power_rate_delta_max",str) && str2val(str,dash_power_rate_delta_max,str) &&
	 strskip(str,")(dash_power_rate_delta_min",str) && str2val(str,dash_power_rate_delta_min,str) &&
	 strskip(str,")(effort_max_delta_factor",str) && str2val(str,effort_max_delta_factor,str) &&
	 strskip(str,")(effort_min_delta_factor",str) && str2val(str,effort_min_delta_factor,str) &&
	 strskip(str,")(extra_stamina_delta_max",str) && str2val(str,extra_stamina_delta_max,str) &&
	 strskip(str,")(extra_stamina_delta_min",str) && str2val(str,extra_stamina_delta_min,str) &&
	 strskip(str,")(inertia_moment_delta_factor",str) && str2val(str,inertia_moment_delta_factor,str) &&
	 strskip(str,")(kick_rand_delta_factor",str) && str2val(str,kick_rand_delta_factor,str) &&
	 strskip(str,")(kickable_margin_delta_max",str) && str2val(str,kickable_margin_delta_max,str) &&
	 strskip(str,")(kickable_margin_delta_min",str) && str2val(str,kickable_margin_delta_min,str) &&
	 strskip(str,")(new_dash_power_rate_delta_max",str) && str2val(str,new_dash_power_rate_delta_max,str) &&
	 strskip(str,")(new_dash_power_rate_delta_min",str) && str2val(str,new_dash_power_rate_delta_min,str) &&
	 strskip(str,")(new_stamina_inc_max_delta_factor",str) && str2val(str,new_stamina_inc_max_delta_factor,str) &&
	 strskip(str,")(player_decay_delta_max",str) && str2val(str,player_decay_delta_max,str) &&
	 strskip(str,")(player_decay_delta_min",str) && str2val(str,player_decay_delta_min,str) &&
	 strskip(str,")(player_size_delta_factor",str) && str2val(str,player_size_delta_factor,str) &&
	 strskip(str,")(player_speed_max_delta_max",str) && str2val(str,player_speed_max_delta_max,str) &&
	 strskip(str,")(player_speed_max_delta_min",str) && str2val(str,player_speed_max_delta_min,str) &&
	 strskip(str,")(stamina_inc_max_delta_factor",str) && str2val(str,stamina_inc_max_delta_factor,str) &&
	 strskip(str,')',str) && 
	 strskip(str,')',str));
  } else {
    res=(strskip(str,"(player_param ",str) &&
	 strskip(str,"(dash_power_rate_delta_max",str) && str2val(str,dash_power_rate_delta_max,str) &&
	 strskip(str,")(dash_power_rate_delta_min",str) && str2val(str,dash_power_rate_delta_min,str) &&
	 strskip(str,")(effort_max_delta_factor",str) && str2val(str,effort_max_delta_factor,str) &&
	 strskip(str,")(effort_min_delta_factor",str) && str2val(str,effort_min_delta_factor,str) &&
	 strskip(str,")(extra_stamina_delta_max",str) && str2val(str,extra_stamina_delta_max,str) &&
	 strskip(str,")(extra_stamina_delta_min",str) && str2val(str,extra_stamina_delta_min,str) &&
	 strskip(str,")(inertia_moment_delta_factor",str) && str2val(str,inertia_moment_delta_factor,str) &&
	 strskip(str,")(kick_rand_delta_factor",str) && str2val(str,kick_rand_delta_factor,str) &&
	 strskip(str,")(kickable_margin_delta_max",str) && str2val(str,kickable_margin_delta_max,str) &&
	 strskip(str,")(kickable_margin_delta_min",str) && str2val(str,kickable_margin_delta_min,str) &&
	 strskip(str,")(new_dash_power_rate_delta_max",str) && str2val(str,new_dash_power_rate_delta_max,str) &&
	 strskip(str,")(new_dash_power_rate_delta_min",str) && str2val(str,new_dash_power_rate_delta_min,str) &&
	 strskip(str,")(new_stamina_inc_max_delta_factor",str) && str2val(str,new_stamina_inc_max_delta_factor,str) &&
	 strskip(str,")(player_decay_delta_max",str) && str2val(str,player_decay_delta_max,str) &&
	 strskip(str,")(player_decay_delta_min",str) && str2val(str,player_decay_delta_min,str) &&
	 strskip(str,")(player_size_delta_factor",str) && str2val(str,player_size_delta_factor,str) &&
	 strskip(str,")(player_speed_max_delta_max",str) && str2val(str,player_speed_max_delta_max,str) &&
	 strskip(str,")(player_speed_max_delta_min",str) && str2val(str,player_speed_max_delta_min,str) &&
	 strskip(str,")(player_types",str) && str2val(str,player_types,str) && 
	 strskip(str,")(pt_max",str) && str2val(str,pt_max,str) && 
	 strskip(str,")(random_seed",str) && str2val(str,random_seed,str) &&
	 strskip(str,")(stamina_inc_max_delta_factor",str) && str2val(str,stamina_inc_max_delta_factor,str) &&
	 strskip(str,")(subs_max",str) && str2val(str,subs_max,str) &&
	 strskip(str,')',str) && 
	 strskip(str,')',str));
  }
  if(!res) {
    cout << "ERROR! Parsed so far:\n";
    for(;origstr<str;origstr++) {
      cout << origstr[0];
    }
    cout << endl << flush;
    return false;
  }
  
  return true;
}

/******************************************
 * PlayerType
 ******************************************/

void PlayerType::init() {}

bool PlayerType::parseMsg(const char *str) {
  const char *origstr=str;
  //cout << "PlayerType Message:\n\n" << str;
  bool res=(strskip(str,"(player_type ",str) &&
	    strskip(str,"(id",str) && str2val(str,id,str) &&
	    strskip(str,")(player_speed_max",str) && str2val(str,player_speed_max,str) &&
	    strskip(str,")(stamina_inc_max",str) && str2val(str,stamina_inc_max,str) &&
	    strskip(str,")(player_decay",str) && str2val(str,player_decay,str) &&
	    strskip(str,")(inertia_moment",str) && str2val(str,inertia_moment,str) &&
	    strskip(str,")(dash_power_rate",str) && str2val(str,dash_power_rate,str) &&
	    strskip(str,")(player_size",str) && str2val(str,player_size,str) &&
	    strskip(str,")(kickable_margin",str) && str2val(str,kickable_margin,str) &&
	    strskip(str,")(kick_rand",str) && str2val(str,kick_rand,str) &&
	    strskip(str,")(extra_stamina",str) && str2val(str,extra_stamina,str) &&
	    strskip(str,")(effort_max",str) && str2val(str,effort_max,str) &&
	    strskip(str,")(effort_min",str) && str2val(str,effort_min,str) &&
	    strskip(str,"))",str));
  if(!res) {
    cout << "ERROR! Parsed so far:\n";
    for(;origstr<str;origstr++) {
      cout << origstr[0];  
    }
    cout << endl << flush;
    return false;
  }
  inertia_moment=DEG2RAD(inertia_moment);

  /* calculate hetero data */
  
  double speed= 0;
  double old_speed= speed;
  while (true) {
    //std::cout << "*" << std::flush;
    old_speed= speed;
    speed *= player_decay;
    speed += 100*dash_power_rate*effort_max;
    if ( speed > player_speed_max ) {
      speed= player_speed_max;
      break;
    }
    if ( speed - old_speed < 0.001 )
      break;
  }
  real_player_speed_max= speed * 1.01; // 1 percent tolerance!
  if ( real_player_speed_max > player_speed_max ) 
    real_player_speed_max= player_speed_max;
  
  dash_to_keep_max_speed=(real_player_speed_max-real_player_speed_max*player_decay)/
    (dash_power_rate*effort_max);
  if ( dash_to_keep_max_speed > 100.0 ) {
    dash_to_keep_max_speed= 100.0;
  }
  stamina_demand_per_meter=(dash_to_keep_max_speed-stamina_inc_max)/real_player_speed_max;

  speed_progress[0]= 100.0 * dash_power_rate * effort_max;
  for(int i=1; i<5; i++) {
    speed_progress[i] = speed_progress[i-1]*player_decay;
    speed_progress[i] += 100.0*dash_power_rate*effort_max;
    if ( speed_progress[i] > player_speed_max )
      speed_progress[i]= player_speed_max;
  }

  struct StaminaPos {
    StaminaPos() { dist= 0; stamina= 0; }
    double dist;
    double stamina;
  };

  const int max_demand=40;
  StaminaPos stamina_demand[max_demand];
  
  double player_speed= 0;
  for (int i=1; i < max_demand; i++) {
    player_speed *= player_decay;
    double dash_power= 100.0;
    double dash_to_keep= ( player_speed_max - player_speed ) / (dash_power_rate*effort_max);
    if ( dash_to_keep < dash_power )
      dash_power= dash_to_keep;
    
    player_speed += dash_power * dash_power_rate * effort_max;
    
    stamina_demand[i].dist += stamina_demand[i-1].dist + player_speed;
    stamina_demand[i].stamina += stamina_demand[i-1].stamina + dash_power - stamina_inc_max;
    if(stamina_demand[i].dist>=10 && stamina_demand[i-1].dist<10)
      stamina_10m=stamina_demand[i].stamina/stamina_demand[i].dist;
    if(stamina_demand[i].dist>=20 && stamina_demand[i-1].dist<20)
      stamina_20m=stamina_demand[i].stamina/stamina_demand[i].dist;
    if(stamina_demand[i].dist>=30 && stamina_demand[i-1].dist<30)
      stamina_30m=stamina_demand[i].stamina/stamina_demand[i].dist;
  }

  
  
  return true;
}

/******************************************
 * ServerParam
 ******************************************/

void ServerParam::init() {}

bool ServerParam::parseMsg(const char *str) {
  //cout << "ServerParam Message:\n\n" << str;
  const char *origstr=str;
  bool res;
  if(!Options::server_94) {
  res=(strskip(str,"(server_param ",str) &&
       strskip(str,"(catch_ban_cycle",str) && str2val(str,catch_ban_cycle,str) && 
       strskip(str,")(clang_advice_win",str) && str2val(str,clang_advice_win,str) && 
       strskip(str,")(clang_define_win",str) && str2val(str,clang_define_win,str) && 
       strskip(str,")(clang_del_win",str) && str2val(str,clang_del_win,str) &&
       strskip(str,")(clang_info_win",str) && str2val(str,clang_info_win,str) && 
       strskip(str,")(clang_mess_delay",str) && str2val(str,clang_mess_delay,str) && 
       strskip(str,")(clang_mess_per_cycle",str) && str2val(str,clang_mess_per_cycle,str) &&
       strskip(str,")(clang_meta_win",str) && str2val(str,clang_meta_win,str) && 
       strskip(str,")(clang_rule_win",str) && str2val(str,clang_rule_win,str) && 
       strskip(str,")(clang_win_size",str) && str2val(str,clang_win_size,str) && 
       strskip(str,")(coach_port",str) && str2val(str,coach_port,str) && 
       strskip(str,")(drop_ball_time",str) && str2val(str,drop_ball_time,str) && 
       strskip(str,")(freeform_send_period",str) && str2val(str,freeform_send_period,str) && 
       strskip(str,")(freeform_wait_period",str) && str2val(str,freeform_wait_period,str) && 
       strskip(str,")(game_log_compression",str) && strfind(str,')',str) &&// str2val(str,game_log_compression,str) && 
       strskip(str,")(game_log_version",str) && strfind(str,')',str) &&// str2val(str,game_log_version,str) && 
       strskip(str,")(goalie_max_moves",str) && str2val(str,goalie_max_moves,str) && 
       strskip(str,")(half_time",str) && str2val(str,half_time,str) && 
       strskip(str,")(hear_decay",str) && str2val(str,hear_decay,str) && 
       strskip(str,")(hear_inc",str) && str2val(str,hear_inc,str) && 
       strskip(str,")(hear_max",str) && str2val(str,hear_max,str) && 
       strskip(str,")(max_goal_kicks",str) && str2val(str,max_goal_kicks,str) && 
       strskip(str,")(olcoach_port",str) && str2val(str,olcoach_port,str) && 
       strskip(str,")(point_to_ban",str) && str2val(str,point_to_ban,str) && 
       strskip(str,")(point_to_duration",str) && str2val(str,point_to_duration,str) && 
       strskip(str,")(port",str) && str2val(str,port,str) && 
       strskip(str,")(recv_step",str) && str2val(str,recv_step,str) && 
       strskip(str,")(say_coach_cnt_max",str) && str2val(str,say_coach_cnt_max,str) &&
       strskip(str,")(say_coach_msg_size",str) && str2val(str,say_coach_msg_size,str) && 
       strskip(str,")(say_msg_size",str) && str2val(str,say_msg_size,str) && 
       strskip(str,")(send_step",str) && str2val(str,send_step,str) && 
       strskip(str,")(send_vi_step",str) && str2val(str,send_vi_step,str) &&
       strskip(str,")(sense_body_step",str) && str2val(str,sense_body_step,str) && 
       strskip(str,")(simulator_step",str) && str2val(str,simulator_step,str) && 
       strskip(str,")(slow_down_factor",str) && str2val(str,slow_down_factor,str) && 
       strskip(str,")(start_goal_l",str) && str2val(str,start_goal_l,str) && 
       strskip(str,")(start_goal_r",str) && str2val(str,start_goal_r,str) && 
       strskip(str,")(synch_micro_sleep",str) && str2val(str,synch_micro_sleep,str) && 
       strskip(str,")(synch_offset",str) && str2val(str,synch_offset,str) && 
       strskip(str,")(tackle_cycles",str) && str2val(str,tackle_cycles,str) && 
       strskip(str,")(text_log_compression",str) && strfind(str,')',str) &&// str2val(str,text_log_compression,str) && 
       strskip(str,")(game_log_dir",str)  && strfind(str,')',str) &&// str2val(str,game_log_dir,str) && 
       strskip(str,")(game_log_fixed_name",str) && strfind(str,')',str) &&// str2val(str,game_log_fixed_name,str) && 
       strskip(str,")(landmark_file",str) && strfind(str,')',str) &&// str2val(str,landmark_file,str) && 
       strskip(str,")(log_date_format",str) && strfind(str,')',str) &&// str2val(str,log_date_format,str) && 
       //    strskip(str,")(replay",str) && str2val(str,replay,str) && read_replay.set_ok() && 
       strskip(str,")(text_log_dir",str) && strfind(str,')',str) &&// str2val(str,text_log_dir,str) && 
       strskip(str,")(text_log_fixed_name",str) && strfind(str,')',str) &&// str2val(str,text_log_fixed_name,str) && 
       strskip(str,")(coach",str) && str2val(str,coach,str) && 
       strskip(str,")(coach_w_referee",str) && str2val(str,coach_w_referee,str) && 
       strskip(str,")(old_coach_hear",str) && str2val(str,old_coach_hear,str) && 
       strskip(str,")(wind_none",str) && str2val(str,wind_none,str) && 
       strskip(str,")(wind_random",str) && str2val(str,wind_random,str) && 
       strskip(str,")(back_passes",str) && str2val(str,back_passes,str) && 
       strskip(str,")(forbid_kick_off_offside",str) && str2val(str,forbid_kick_off_offside,str) && 
       strskip(str,")(free_kick_faults",str) && str2val(str,free_kick_faults,str) && 
       strskip(str,")(fullstate_l",str) && str2val(str,fullstate_l,str) && 
       strskip(str,")(fullstate_r",str) && str2val(str,fullstate_r,str) && 
       strskip(str,")(game_log_dated",str) && strfind(str,')',str) &&// str2val(str,game_log_dated,str) && 
       strskip(str,")(game_log_fixed",str) && strfind(str,')',str) &&// str2val(str,game_log_fixed,str) && 
       strskip(str,")(game_logging",str) && strfind(str,')',str) &&// str2val(str,game_logging,str) && 
       strskip(str,")(log_times",str) && strfind(str,')',str) &&// str2val(str,log_times,str) && 
       strskip(str,")(profile",str) && strfind(str,')',str) &&// str2val(str,profile,str) && 
       strskip(str,")(proper_goal_kicks",str) && str2val(str,proper_goal_kicks,str) && 
       strskip(str,")(record_messages",str) && str2val(str,record_messages,str) && 
       strskip(str,")(send_comms",str) && str2val(str,send_comms,str) &&
       strskip(str,")(synch_mode",str) && str2val(str,synch_mode,str) &&
       strskip(str,")(team_actuator_noise",str) && str2val(str,team_actuator_noise,str) &&
       strskip(str,")(text_log_dated",str) && strfind(str,')',str) &&// str2val(str,text_log_dated,str) &&
       strskip(str,")(text_log_fixed",str) && strfind(str,')',str) &&// str2val(str,text_log_fixed,str) &&
       strskip(str,")(text_logging",str) && strfind(str,')',str) &&// str2val(str,text_logging,str) &&
       strskip(str,")(use_offside",str) && str2val(str,use_offside,str) &&
       strskip(str,")(verbose",str) && str2val(str,verbose,str) &&
       strskip(str,")(audio_cut_dist",str) && str2val(str,audio_cut_dist,str) &&
       strskip(str,")(ball_accel_max",str) && str2val(str,ball_accel_max,str) &&
       strskip(str,")(ball_decay",str) && str2val(str,ball_decay,str) &&
       strskip(str,")(ball_rand",str) && str2val(str,ball_rand,str) &&
       strskip(str,")(ball_size",str) && str2val(str,ball_size,str) &&
       strskip(str,")(ball_speed_max",str) && str2val(str,ball_speed_max,str) &&
       strskip(str,")(ball_weight",str) && str2val(str,ball_weight,str) &&
       strskip(str,")(catch_probability",str) && str2val(str,catch_probability,str) &&
       strskip(str,")(catchable_area_l",str) && str2val(str,catchable_area_l,str) &&
       strskip(str,")(catchable_area_w",str) && str2val(str,catchable_area_w,str) &&
       strskip(str,")(ckick_margin",str) && str2val(str,ckick_margin,str) &&
       strskip(str,")(control_radius",str) && str2val(str,control_radius,str) &&
       strskip(str,")(dash_power_rate",str) && str2val(str,dash_power_rate,str) &&
       strskip(str,")(effort_dec",str) && str2val(str,effort_dec,str) &&
       strskip(str,")(effort_dec_thr",str) && str2val(str,effort_dec_thr,str) &&
       strskip(str,")(effort_inc",str) && str2val(str,effort_inc,str) &&
       strskip(str,")(effort_inc_thr",str) && str2val(str,effort_inc_thr,str) &&
       strskip(str,")(effort_init",str) && str2val(str,effort_init,str) &&
       strskip(str,")(effort_min",str) && str2val(str,effort_min,str) &&
       strskip(str,")(goal_width",str) && str2val(str,goal_width,str) &&
       strskip(str,")(inertia_moment",str) && str2val(str,inertia_moment,str) &&
       strskip(str,")(kick_power_rate",str) && str2val(str,kick_power_rate,str) &&
       strskip(str,")(kick_rand",str) && str2val(str,kick_rand,str) &&
       strskip(str,")(kick_rand_factor_l",str) && str2val(str,kick_rand_factor_l,str) &&
       strskip(str,")(kick_rand_factor_r",str) && str2val(str,kick_rand_factor_r,str) &&
       strskip(str,")(kickable_margin",str) && str2val(str,kickable_margin,str) &&
       strskip(str,")(maxmoment",str) && str2val(str,maxmoment,str) &&
       strskip(str,")(maxneckang",str) && str2val(str,maxneckang,str) &&
       strskip(str,")(maxneckmoment",str) && str2val(str,maxneckmoment,str) &&
       strskip(str,")(maxpower",str) && str2val(str,maxpower,str) &&
       strskip(str,")(minmoment",str) && str2val(str,minmoment,str) &&
       strskip(str,")(minneckang",str) && str2val(str,minneckang,str) &&
       strskip(str,")(minneckmoment",str) && str2val(str,minneckmoment,str) &&
       strskip(str,")(minpower",str) && str2val(str,minpower,str) &&
       strskip(str,")(offside_active_area_size",str) && str2val(str,offside_active_area_size,str) &&
       strskip(str,")(offside_kick_margin",str) && str2val(str,offside_kick_margin,str) &&
       strskip(str,")(player_accel_max",str) && str2val(str,player_accel_max,str) &&
       strskip(str,")(player_decay",str) && str2val(str,player_decay,str) &&
       strskip(str,")(player_rand",str) && str2val(str,player_rand,str) &&
       strskip(str,")(player_size",str) && str2val(str,player_size,str) &&
       strskip(str,")(player_speed_max",str) && str2val(str,player_speed_max,str) &&
       strskip(str,")(player_weight",str) && str2val(str,player_weight,str) &&
       strskip(str,")(prand_factor_l",str) && str2val(str,prand_factor_l,str) &&
       strskip(str,")(prand_factor_r",str) && str2val(str,prand_factor_r,str) &&
       strskip(str,")(quantize_step",str) && str2val(str,quantize_step,str) &&
       strskip(str,")(quantize_step_l",str) && str2val(str,quantize_step_l,str) &&
       strskip(str,")(recover_dec",str) && str2val(str,recover_dec,str) &&
       strskip(str,")(recover_dec_thr",str) && str2val(str,recover_dec_thr,str) &&
       //strskip(str,")(recover_init",str) && strfind(str,')',str) &&
       strskip(str,")(recover_min",str) && str2val(str,recover_min,str) &&
       strskip(str,")(slowness_on_top_for_left_team",str) && strfind(str,')',str) &&// str2val(str,slowness_on_top_for_left_team,str) &&
       strskip(str,")(slowness_on_top_for_right_team",str) && strfind(str,')',str) &&// str2val(str,slowness_on_top_for_right_team,str) &&
       strskip(str,")(stamina_inc_max",str) && str2val(str,stamina_inc_max,str) &&
       strskip(str,")(stamina_max",str) && str2val(str,stamina_max,str) &&
       strskip(str,")(stopped_ball_vel",str) && str2val(str,stopped_ball_vel,str) &&
       strskip(str,")(tackle_back_dist",str) && str2val(str,tackle_back_dist,str) &&
       strskip(str,")(tackle_dist",str) && str2val(str,tackle_dist,str) &&
       strskip(str,")(tackle_exponent",str) && str2val(str,tackle_exponent,str) &&
       strskip(str,")(tackle_power_rate",str) && str2val(str,tackle_power_rate,str) &&
       strskip(str,")(tackle_width",str) && str2val(str,tackle_width,str) &&
       strskip(str,")(visible_angle",str) && str2val(str,visible_angle,str) &&
       strskip(str,")(visible_distance",str) && str2val(str,visible_distance,str) &&
       strskip(str,")(wind_ang",str) && str2val(str,wind_ang,str) &&
       strskip(str,")(wind_dir",str) && str2val(str,wind_dir,str) &&
       strskip(str,")(wind_force",str) && str2val(str,wind_force,str) &&
       strskip(str,")(wind_rand",str) && str2val(str,wind_rand,str) &&
       strskip(str,"))",str));
  } else {
    // version for server 9.4.2
    res=(strskip(str,"(server_param ",str) &&
	 strskip(str,"(audio_cut_dist",str) && str2val(str,audio_cut_dist,str) &&
	 strskip(str,")(back_passes",str) && str2val(str,back_passes,str) && 
       strskip(str,")(ball_accel_max",str) && str2val(str,ball_accel_max,str) &&
       strskip(str,")(ball_decay",str) && str2val(str,ball_decay,str) &&
       strskip(str,")(ball_rand",str) && str2val(str,ball_rand,str) &&
       strskip(str,")(ball_size",str) && str2val(str,ball_size,str) &&
       strskip(str,")(ball_speed_max",str) && str2val(str,ball_speed_max,str) &&
       strskip(str,")(ball_weight",str) && str2val(str,ball_weight,str) &&
       strskip(str,")(catch_ban_cycle",str) && str2val(str,catch_ban_cycle,str) &&
	 strskip(str,")(catch_probability",str) && str2val(str,catch_probability,str) &&
       strskip(str,")(catchable_area_l",str) && str2val(str,catchable_area_l,str) &&
       strskip(str,")(catchable_area_w",str) && str2val(str,catchable_area_w,str) &&
       strskip(str,")(ckick_margin",str) && str2val(str,ckick_margin,str) &&	 
       strskip(str,")(clang_advice_win",str) && str2val(str,clang_advice_win,str) && 
       strskip(str,")(clang_define_win",str) && str2val(str,clang_define_win,str) && 
       strskip(str,")(clang_del_win",str) && str2val(str,clang_del_win,str) &&
       strskip(str,")(clang_info_win",str) && str2val(str,clang_info_win,str) && 
       strskip(str,")(clang_mess_delay",str) && str2val(str,clang_mess_delay,str) && 
       strskip(str,")(clang_mess_per_cycle",str) && str2val(str,clang_mess_per_cycle,str) &&
       strskip(str,")(clang_meta_win",str) && str2val(str,clang_meta_win,str) && 
       strskip(str,")(clang_rule_win",str) && str2val(str,clang_rule_win,str) && 
       strskip(str,")(clang_win_size",str) && str2val(str,clang_win_size,str) && 
	 strskip(str,")(coach",str) && str2val(str,coach,str) &&
	 strskip(str,")(coach_port",str) && str2val(str,coach_port,str) && 
	 strskip(str,")(coach_w_referee",str) && str2val(str,coach_w_referee,str) && 
	 strskip(str,")(control_radius",str) && str2val(str,control_radius,str) &&
       strskip(str,")(dash_power_rate",str) && str2val(str,dash_power_rate,str) && 
       strskip(str,")(drop_ball_time",str) && str2val(str,drop_ball_time,str) &&
	 strskip(str,")(effort_dec",str) && str2val(str,effort_dec,str) &&
       strskip(str,")(effort_dec_thr",str) && str2val(str,effort_dec_thr,str) &&
       strskip(str,")(effort_inc",str) && str2val(str,effort_inc,str) &&
       strskip(str,")(effort_inc_thr",str) && str2val(str,effort_inc_thr,str) &&
       strskip(str,")(effort_init",str) && str2val(str,effort_init,str) &&
       strskip(str,")(effort_min",str) && str2val(str,effort_min,str) &&
	strskip(str,")(forbid_kick_off_offside",str) && str2val(str,forbid_kick_off_offside,str) && 
       strskip(str,")(free_kick_faults",str) && str2val(str,free_kick_faults,str) &&  
       strskip(str,")(freeform_send_period",str) && str2val(str,freeform_send_period,str) && 
       strskip(str,")(freeform_wait_period",str) && str2val(str,freeform_wait_period,str) &&
	 strskip(str,")(fullstate_l",str) && str2val(str,fullstate_l,str) && 
	 strskip(str,")(fullstate_r",str) && str2val(str,fullstate_r,str) &&	 
       strskip(str,")(game_log_compression",str) && strfind(str,')',str) &&// str2val(str,game_log_compression,str) && 
	 strskip(str,")(game_log_dated",str) && strfind(str,')',str) &&// str2val(str,game_log_dated,str) &&
	 strskip(str,")(game_log_dir",str)  && strfind(str,')',str) &&// str2val(str,game_log_dir,str) && 
	 strskip(str,")(game_log_fixed",str) && strfind(str,')',str) &&// str2val(str,game_log_fixed,str) && 
	 strskip(str,")(game_log_fixed_name",str) && strfind(str,')',str) &&// str2val(str,game_log_fixed_name,str) &&
	 strskip(str,")(game_log_version",str) && strfind(str,')',str) &&// str2val(str,game_log_version,str) &&
	 strskip(str,")(game_logging",str) && strfind(str,')',str) &&// str2val(str,game_logging,str) &&
	 strskip(str,")(goal_width",str) && str2val(str,goal_width,str) &&
       strskip(str,")(goalie_max_moves",str) && str2val(str,goalie_max_moves,str) && 
       strskip(str,")(half_time",str) && str2val(str,half_time,str) && 
       strskip(str,")(hear_decay",str) && str2val(str,hear_decay,str) && 
       strskip(str,")(hear_inc",str) && str2val(str,hear_inc,str) && 
       strskip(str,")(hear_max",str) && str2val(str,hear_max,str) &&
	strskip(str,")(inertia_moment",str) && str2val(str,inertia_moment,str) &&
       strskip(str,")(kick_power_rate",str) && str2val(str,kick_power_rate,str) &&
       strskip(str,")(kick_rand",str) && str2val(str,kick_rand,str) &&
       strskip(str,")(kick_rand_factor_l",str) && str2val(str,kick_rand_factor_l,str) &&
       strskip(str,")(kick_rand_factor_r",str) && str2val(str,kick_rand_factor_r,str) &&
       strskip(str,")(kickable_margin",str) && str2val(str,kickable_margin,str) &&
	strskip(str,")(landmark_file",str) && strfind(str,')',str) &&// str2val(str,landmark_file,str) && 
       strskip(str,")(log_date_format",str) && strfind(str,')',str) &&// str2val(str,log_date_format,str) &&
	 strskip(str,")(log_times",str) && strfind(str,')',str) &&// str2val(str,log_times,str) && 	 
       strskip(str,")(max_goal_kicks",str) && str2val(str,max_goal_kicks,str) &&
	strskip(str,")(maxmoment",str) && str2val(str,maxmoment,str) &&
       strskip(str,")(maxneckang",str) && str2val(str,maxneckang,str) &&
       strskip(str,")(maxneckmoment",str) && str2val(str,maxneckmoment,str) &&
       strskip(str,")(maxpower",str) && str2val(str,maxpower,str) &&
       strskip(str,")(minmoment",str) && str2val(str,minmoment,str) &&
       strskip(str,")(minneckang",str) && str2val(str,minneckang,str) &&
       strskip(str,")(minneckmoment",str) && str2val(str,minneckmoment,str) &&
       strskip(str,")(minpower",str) && str2val(str,minpower,str) &&
	 strskip(str,")(offside_active_area_size",str) && str2val(str,offside_active_area_size,str) &&
       strskip(str,")(offside_kick_margin",str) && str2val(str,offside_kick_margin,str) &&
       strskip(str,")(olcoach_port",str) && str2val(str,olcoach_port,str) &&
	strskip(str,")(old_coach_hear",str) && str2val(str,old_coach_hear,str) &&
	strskip(str,")(player_accel_max",str) && str2val(str,player_accel_max,str) &&
       strskip(str,")(player_decay",str) && str2val(str,player_decay,str) &&
       strskip(str,")(player_rand",str) && str2val(str,player_rand,str) &&
       strskip(str,")(player_size",str) && str2val(str,player_size,str) &&
       strskip(str,")(player_speed_max",str) && str2val(str,player_speed_max,str) &&
       strskip(str,")(player_weight",str) && str2val(str,player_weight,str) && 
       strskip(str,")(point_to_ban",str) && str2val(str,point_to_ban,str) && 
       strskip(str,")(point_to_duration",str) && str2val(str,point_to_duration,str) && 
       strskip(str,")(port",str) && str2val(str,port,str) &&
	strskip(str,")(prand_factor_l",str) && str2val(str,prand_factor_l,str) &&
       strskip(str,")(prand_factor_r",str) && str2val(str,prand_factor_r,str) &&
	strskip(str,")(profile",str) && strfind(str,')',str) &&// str2val(str,profile,str) && 
       strskip(str,")(proper_goal_kicks",str) && str2val(str,proper_goal_kicks,str) && 
	 strskip(str,")(quantize_step",str) && str2val(str,quantize_step,str) &&
       strskip(str,")(quantize_step_l",str) && str2val(str,quantize_step_l,str) &&
	strskip(str,")(record_messages",str) && str2val(str,record_messages,str) &&
	strskip(str,")(recover_dec",str) && str2val(str,recover_dec,str) &&
       strskip(str,")(recover_dec_thr",str) && str2val(str,recover_dec_thr,str) &&
       //strskip(str,")(recover_init",str) && strfind(str,')',str) &&
       strskip(str,")(recover_min",str) && str2val(str,recover_min,str) && 
       strskip(str,")(recv_step",str) && str2val(str,recv_step,str) &&
	 
       strskip(str,")(say_coach_cnt_max",str) && str2val(str,say_coach_cnt_max,str) &&
       strskip(str,")(say_coach_msg_size",str) && str2val(str,say_coach_msg_size,str) && 
       strskip(str,")(say_msg_size",str) && str2val(str,say_msg_size,str) &&
	 strskip(str,")(send_comms",str) && str2val(str,send_comms,str) && 
       strskip(str,")(send_step",str) && str2val(str,send_step,str) && 
       strskip(str,")(send_vi_step",str) && str2val(str,send_vi_step,str) &&
       strskip(str,")(sense_body_step",str) && str2val(str,sense_body_step,str) && 
       strskip(str,")(simulator_step",str) && str2val(str,simulator_step,str) && 
       strskip(str,")(slow_down_factor",str) && str2val(str,slow_down_factor,str) &&
	 strskip(str,")(slowness_on_top_for_left_team",str) && strfind(str,')',str) &&// str2val(str,slowness_on_top_for_left_team,str) &&
       strskip(str,")(slowness_on_top_for_right_team",str) && strfind(str,')',str) &&// str2val(str,slowness_on_top_for_right_team,str) &&
       strskip(str,")(stamina_inc_max",str) && str2val(str,stamina_inc_max,str) &&
       strskip(str,")(stamina_max",str) && str2val(str,stamina_max,str) && 
       strskip(str,")(start_goal_l",str) && str2val(str,start_goal_l,str) && 
       strskip(str,")(start_goal_r",str) && str2val(str,start_goal_r,str) &&
	strskip(str,")(stopped_ball_vel",str) && str2val(str,stopped_ball_vel,str) && 
       strskip(str,")(synch_micro_sleep",str) && str2val(str,synch_micro_sleep,str) &&
	strskip(str,")(synch_mode",str) && str2val(str,synch_mode,str) && 
       strskip(str,")(synch_offset",str) && str2val(str,synch_offset,str) &&
	 strskip(str,")(tackle_back_dist",str) && str2val(str,tackle_back_dist,str) &&
       strskip(str,")(tackle_cycles",str) && str2val(str,tackle_cycles,str) &&
	 strskip(str,")(tackle_dist",str) && str2val(str,tackle_dist,str) &&
       strskip(str,")(tackle_exponent",str) && str2val(str,tackle_exponent,str) &&
       strskip(str,")(tackle_power_rate",str) && str2val(str,tackle_power_rate,str) &&
	 strskip(str,")(tackle_width",str) && str2val(str,tackle_width,str) &&
	 strskip(str,")(team_actuator_noise",str) && str2val(str,team_actuator_noise,str) &&
       strskip(str,")(text_log_compression",str) && strfind(str,')',str) &&// str2val(str,text_log_compression,str) &&
       	        //    strskip(str,")(replay",str) && str2val(str,replay,str) && read_replay.set_ok() &&
	 strskip(str,")(text_log_dated",str) && strfind(str,')',str) &&// str2val(str,text_log_dated,str) &&	 
       strskip(str,")(text_log_dir",str) && strfind(str,')',str) &&// str2val(str,text_log_dir,str) &&
	strskip(str,")(text_log_fixed",str) && strfind(str,')',str) &&// str2val(str,text_log_fixed,str) && 
       strskip(str,")(text_log_fixed_name",str) && strfind(str,')',str) &&// str2val(str,text_log_fixed_name,str) &&
	 strskip(str,")(text_logging",str) && strfind(str,')',str) &&// str2val(str,text_logging,str) &&
	 strskip(str,")(use_offside",str) && str2val(str,use_offside,str) &&
       strskip(str,")(verbose",str) && str2val(str,verbose,str) &&
	 strskip(str,")(visible_angle",str) && str2val(str,visible_angle,str) &&
       strskip(str,")(visible_distance",str) && str2val(str,visible_distance,str) &&
       strskip(str,")(wind_ang",str) && str2val(str,wind_ang,str) &&
       strskip(str,")(wind_dir",str) && str2val(str,wind_dir,str) &&
       strskip(str,")(wind_force",str) && str2val(str,wind_force,str) &&
	 strskip(str,")(wind_none",str) && str2val(str,wind_none,str) && 	 
       strskip(str,")(wind_rand",str) && str2val(str,wind_rand,str) &&
	 strskip(str,")(wind_random",str) && str2val(str,wind_random,str) && 	 
       strskip(str,"))",str));

  }
  //strskip(str,") (",str) && str2val(str,,str) &&
  if(!res) {
    cout << "ERROR! Parsed so far:\n";
    for(;origstr<str;origstr++) {
      cout << origstr[0];
    }
    cout << endl << flush;
    return false;
  }
  maxneckang = DEG2RAD(maxneckang);
  minneckang = DEG2RAD(minneckang);
  maxmoment = DEG2RAD(maxmoment);
  minmoment = DEG2RAD(minmoment);
  maxneckmoment = DEG2RAD(maxneckmoment);
  minneckmoment = DEG2RAD(minneckmoment);
  inertia_moment = DEG2RAD(inertia_moment);
  visible_angle = DEG2RAD(visible_angle);
  wind_dir = DEG2RAD(wind_dir);

  return true;
}

Value ServerParam::goal_width;
Value ServerParam::inertia_moment;
Value ServerParam::player_size;
Value ServerParam::player_decay;
Value ServerParam::player_rand;
Value ServerParam::player_weight;
Value ServerParam::player_speed_max;
Value ServerParam::player_accel_max;
Value ServerParam::stamina_max;
Value ServerParam::stamina_inc_max;
Value ServerParam::recover_init;
Value ServerParam::recover_dec_thr;
Value ServerParam::recover_min;
Value ServerParam::recover_dec;
Value ServerParam::effort_init;
Value ServerParam::effort_dec_thr;
Value ServerParam::effort_min;
Value ServerParam::effort_dec;
Value ServerParam::effort_inc_thr;
Value ServerParam::effort_inc;
Value ServerParam::kick_rand;
bool ServerParam::team_actuator_noise;
Value ServerParam::prand_factor_l;
Value ServerParam::prand_factor_r;
Value ServerParam::kick_rand_factor_l;
Value ServerParam::kick_rand_factor_r;
Value ServerParam::ball_size;
Value ServerParam::ball_decay;
Value ServerParam::ball_rand;
Value ServerParam::ball_weight;
Value ServerParam::ball_speed_max;
Value ServerParam::ball_accel_max;
Value ServerParam::dash_power_rate;
Value ServerParam::kick_power_rate;
Value ServerParam::kickable_margin;
Value ServerParam::control_radius;
Value ServerParam::control_radius_width;
Value ServerParam::maxpower;
Value ServerParam::minpower;
Value ServerParam::maxmoment;
Value ServerParam::minmoment;
Value ServerParam::maxneckmoment;
Value ServerParam::minneckmoment;
Value ServerParam::maxneckang;
Value ServerParam::minneckang;
Value ServerParam::visible_angle;
Value ServerParam::visible_distance;
Value ServerParam::wind_dir;
Value ServerParam::wind_force;
Value ServerParam::wind_ang;
Value ServerParam::wind_rand;
Value ServerParam::kickable_area;
Value ServerParam::catchable_area_l;
Value ServerParam::catchable_area_w;
Value ServerParam::catch_probability;
int ServerParam::goalie_max_moves;
Value ServerParam::ckick_margin;
Value ServerParam::offside_active_area_size;
bool ServerParam::wind_none;
bool ServerParam::wind_random;
int ServerParam::say_coach_cnt_max;
int ServerParam::say_coach_msg_size;
int ServerParam::clang_win_size;
int ServerParam::clang_define_win;
int ServerParam::clang_meta_win;
int ServerParam::clang_advice_win;
int ServerParam::clang_info_win;
int ServerParam::clang_mess_delay;
int ServerParam::clang_mess_per_cycle;
int ServerParam::clang_del_win;
int ServerParam::clang_rule_win;
int ServerParam::freeform_send_period;
int ServerParam::freeform_wait_period;
int ServerParam::half_time;
int ServerParam::simulator_step;
int ServerParam::send_step;
int ServerParam::recv_step;
int ServerParam::lcm_step;
int ServerParam::sense_body_step;
int ServerParam::say_msg_size;
int ServerParam::hear_max;
int ServerParam::hear_inc;
int ServerParam::hear_decay;
int ServerParam::catch_ban_cycle;
int ServerParam::slow_down_factor;
bool ServerParam::use_offside;
bool ServerParam::forbid_kick_off_offside;
Value ServerParam::offside_kick_margin;
Value ServerParam::audio_cut_dist;
Value ServerParam::quantize_step;
Value ServerParam::quantize_step_l;
Value ServerParam::quantize_step_dir;
Value ServerParam::quantize_step_dist_team_l;
Value ServerParam::quantize_step_dist_team_r;
Value ServerParam::quantize_step_dist_l_team_l;
Value ServerParam::quantize_step_dist_l_team_r;
Value ServerParam::quantize_step_dir_team_l;
Value ServerParam::quantize_step_dir_team_r;
bool ServerParam::coach;
bool ServerParam::coach_w_referee;
bool ServerParam::old_coach_hear;
int ServerParam::send_vi_step;
int ServerParam::start_goal_l;
int ServerParam::start_goal_r;
bool ServerParam::fullstate_l;
bool ServerParam::fullstate_r;
int ServerParam::drop_ball_time;

int ServerParam::port;
int ServerParam::coach_port;
int ServerParam::olcoach_port;
int ServerParam::verbose;
int ServerParam::replay;
int ServerParam::synch_mode;
int ServerParam::synch_offset;
int ServerParam::synch_micro_sleep;

int ServerParam::max_goal_kicks;
int ServerParam::point_to_ban;
int ServerParam::point_to_duration;
int ServerParam::tackle_cycles;
int ServerParam::back_passes;
int ServerParam::free_kick_faults;
int ServerParam::proper_goal_kicks;
int ServerParam::record_messages;
int ServerParam::send_comms;
Value ServerParam::stopped_ball_vel;
Value ServerParam::tackle_back_dist;
Value ServerParam::tackle_dist;
Value ServerParam::tackle_exponent;
Value ServerParam::tackle_power_rate;
Value ServerParam::tackle_width;
