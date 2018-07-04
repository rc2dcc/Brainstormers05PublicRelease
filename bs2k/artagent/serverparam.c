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

#include "serverparam.h"
#include "sensorparser.h"
#include "macro_msg.h"
#include "wm.h"

Msg_server_param * ServerParam::server_param= 0;
Msg_player_param * ServerParam::player_param= 0;
Msg_player_type  ** ServerParam::player_types= 0;
Msg_player_type  ServerParam::worst_case_opponent_type;

int ServerParam::number_of_player_types() {
  if ( ! player_param)
    return -1;
  return player_param->player_types;
}

bool ServerParam::all_params_ok() {
  if ( ! server_param)
    return false;
  if ( ! player_param)
    return false;
  for (int i=0; i< player_param->player_types; i++)
    if ( ! player_types[i])
      return false;
  
  return true;
}

bool ServerParam::incorporate_server_param_string(const char * buf) {
  if (server_param) {
    ERROR_OUT << ID << "\nstrange, server_param should be == 0";
    return false;
  }
  
  server_param= new Msg_server_param;

  return SensorParser::manual_parse_server_param(buf, * server_param);
}

bool ServerParam::incorporate_player_param_string(const char * buf) {
  if (player_param) {
    ERROR_OUT << ID << "\nstrange, player_param should be == 0";
    return false;
  }
  player_param= new Msg_player_param;

  bool res= SensorParser::manual_parse_player_param(buf, * player_param);
  if (!res)
    return false;

  if (player_types) {
    ERROR_OUT << ID << "\nstrange, player_types should be == 0";
    player_types= 0;
    return false;
  }

  player_types= new Msg_player_type*[ player_param->player_types];
  for (int i=0; i < player_param->player_types; i++)
    player_types[i]= 0; //init to 0

  return res;
}

bool ServerParam::incorporate_player_type_string(const char * buf) {
  if ( ! player_types) {
    ERROR_OUT << ID << "\nstrange, player_types should be != 0, ignoring player types";
    return false;
  }
  
  Msg_player_type * pt= new Msg_player_type;
  bool res= SensorParser::manual_parse_player_type(buf, * pt);
  if (!res)
    return false;

  if ( player_types[ pt->id ] ) {
    ERROR_OUT << ID << "\nstrange player type " << pt->id << " already exists";
    return false;
  }


  {
    double speed= 0;
    double old_speed= speed;
    while (true) {
      std::cout << "*" << std::flush;
      old_speed= speed;
      speed *= pt->player_decay;
      speed += 100*(pt->dash_power_rate * pt->effort_max);
      if ( speed > pt->player_speed_max ) {
	speed= pt->player_speed_max;
	break;
      }
      if ( speed - old_speed < 0.001 )
	break;
    }
    pt->real_player_speed_max= speed * 1.01; // 1 percent tolerance!
    if ( pt->real_player_speed_max > pt->player_speed_max ) 
      pt->real_player_speed_max= pt->player_speed_max;

    /* experimental
    if ( ! player_param ) 
      ERROR_OUT << ID << " player param should be initialized at this moment!";
    else 
      pt->real_player_speed_max += player_param->player_speed_max_delta_max;
    */
    double dash_to_keep_max_speed= (pt->real_player_speed_max - pt->real_player_speed_max * pt->player_decay) / (pt->dash_power_rate * pt->effort_max);
    if ( dash_to_keep_max_speed > 100.0 ) {
      //INFO_OUT << "\nreducing dash_to_keep_max_speed= " << dash_to_keep_max_speed << " to 100.0";
      dash_to_keep_max_speed= 100.0;
    }
    pt->stamina_demand_per_meter= (dash_to_keep_max_speed - pt->stamina_inc_max)/ pt->real_player_speed_max;
  }


  player_types[ pt->id ]= pt;
  if ( pt->id == 0 ) {
    worst_case_opponent_type= *pt;
    worst_case_opponent_type.id= -1;
  }
  else
    worst_case_opponent_type.adapt_better_entries( *pt );

  //INFO_OUT << "player_param= " << player_param->player_types;
#if 1
  if ( pt->id + 1 == player_param->player_types ) {
    INFO_OUT << ID << "Player types: ";
    for (int i=0; i < player_param->player_types; i++) 
      player_types[ i ]->show( INFO_STREAM );
    worst_case_opponent_type.show( INFO_STREAM );
  }
#endif

  return res;
}

//#define TAKE_PARAMETER(XXX) ServerOptions::XXX = server_param->XXX; res= res && server_param->read_ XXX .ok 

bool ServerParam::export_server_options() {
  if (!server_param)
    return false;

  //bool res= true;
  ServerOptions::ball_accel_max= server_param->ball_accel_max;
  //res = res && server_param->read_ball_accel_max.ok;
  ServerOptions::ball_decay= server_param->ball_decay;
  ServerOptions::ball_rand= server_param->ball_rand;
  ServerOptions::ball_size= server_param->ball_size;
  ServerOptions::ball_speed_max= server_param->ball_speed_max;
  //ServerOptions::ball_weight= server_param->ball_weight;
  ServerOptions::catchable_area_l= server_param->catchable_area_l;
  //ServerOptions::catchable_area_w= server_param->catchable_area_w;
  ServerOptions::catch_ban_cycle= server_param->catch_ban_cycle;
  //ServerOptions::catch_probability= server_param->catch_probability;
  ServerOptions::dash_power_rate= server_param->dash_power_rate;
  ServerOptions::effort_dec= server_param->effort_dec;
  ServerOptions::effort_dec_thr= server_param->effort_dec_thr;
  ServerOptions::effort_inc= server_param->effort_inc;
  ServerOptions::effort_inc_thr= server_param->effort_inc_thr;
  ServerOptions::effort_min= server_param->effort_min;
  //int ServerOptions::goalie_max_moves= server_param->goalie_max_moves;
  ServerOptions::goal_width= server_param->goal_width;
  ServerOptions::half_time= server_param-> half_time;
  ServerOptions::inertia_moment= server_param->inertia_moment;
  //kickable_area is defined at the end
  ServerOptions::kickable_margin= server_param->kickable_margin;
  ServerOptions::kick_power_rate= server_param->kick_power_rate;
  ServerOptions::maxneckang= ANGLE( DEG2RAD(server_param->maxneckang) );
  ServerOptions::maxneckmoment= ANGLE( DEG2RAD(server_param->maxneckmoment) );
  ServerOptions::maxpower= server_param->maxpower;
  ServerOptions::minneckang= ANGLE( DEG2RAD(server_param->minneckang) );
  ServerOptions::minneckmoment= ANGLE( DEG2RAD(server_param->minneckmoment) );
  ServerOptions::minpower= server_param->minpower;
  //ServerOptions::player_accel_max= server_param->player_accel_max;
  ServerOptions::player_decay= server_param->player_decay;
  ServerOptions::player_rand= server_param->player_rand;
  ServerOptions::player_size= server_param->player_size;
  ServerOptions::player_speed_max= server_param->player_speed_max;
  //ServerOptions::player_weight= server_param->player_weight;
  ServerOptions::recover_dec= server_param->recover_dec;
  ServerOptions::recover_dec_thr= server_param->recover_dec_thr;
  ServerOptions::recover_min= server_param->recover_min;
  ServerOptions::simulator_step= server_param->simulator_step;
  ServerOptions::slow_down_factor = server_param->slow_down_factor;
  ServerOptions::stamina_max= server_param->stamina_max;
  ServerOptions::stamina_inc_max= server_param->stamina_inc_max;
  ServerOptions::use_offside= server_param->use_offside;
  ServerOptions::visible_angle= server_param->visible_angle;
  ServerOptions::visible_distance= server_param->visible_distance;
  ServerOptions::tackle_dist= server_param->tackle_dist;
  ServerOptions::tackle_back_dist= server_param->tackle_back_dist;
  ServerOptions::tackle_width= server_param->tackle_width;
  ServerOptions:: tackle_exponent= server_param->tackle_exponent;
  ServerOptions:: tackle_cycles= server_param->tackle_cycles;

  ServerOptions::kickable_area= ServerOptions::kickable_margin + ServerOptions::ball_size + ServerOptions::player_size;
  return true;
}

Msg_player_type const* ServerParam::get_player_type(int type) {
  if ( player_types == 0 ) {
    ERROR_OUT << ID << "\nplayer types do not exist";
    return 0;
  }

  if (type >= player_param->player_types || type < 0 ) {
    ERROR_OUT << ID << "\nwrong type request " << type;//debug
    return 0;
  }

  if ( player_types[type]->id != type ) 
    ERROR_OUT << ID << "\nwrong type info";//debug

  return player_types[type];
}
