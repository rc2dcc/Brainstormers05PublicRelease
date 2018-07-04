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

#include "server_options.h"

#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include "globaldef.h"
#include "macro_msg.h"

ServerOptions::ServerOptions() {}
ServerOptions::~ServerOptions() {}

/* some kind of default values */
Value ServerOptions::ball_accel_max= 2.7;
Value ServerOptions::ball_decay= 0.8;
Value ServerOptions::ball_rand= 0.2;
Value ServerOptions::ball_size= 0.15;
Value ServerOptions::ball_speed_max= 32.0;
//Value ServerOptions::ball_weight= 0.2;
Value ServerOptions::catchable_area_l= 2.0;
//Value ServerOptions::catchable_area_w= 1.0;
int ServerOptions::catch_ban_cycle= 5;
//Value ServerOptions::catch_probability= 1.0;
Value ServerOptions::dash_power_rate= 0.1;
Value ServerOptions::effort_dec= 0.05;
Value ServerOptions::effort_dec_thr= 0.3;
Value ServerOptions::effort_inc= 0.05;
Value ServerOptions::effort_inc_thr= 0.9;
Value ServerOptions::effort_min= 0.1;
//int ServerOptions::goalie_max_moves= 2;
Value ServerOptions::goal_width=14.02;
int ServerOptions::half_time= 600;
Value ServerOptions::inertia_moment= 5.0;
//ServerOptions::kickable_area is defined at the end;
Value ServerOptions::kickable_margin= 1.0;
Value ServerOptions::kick_power_rate= 0.1;
ANGLE ServerOptions::maxneckang;
ANGLE ServerOptions::maxneckmoment;
Value ServerOptions::maxpower= 100.0;
ANGLE ServerOptions::minneckang;
ANGLE ServerOptions::minneckmoment;
Value ServerOptions::minpower= -30;
//Value ServerOptions::player_accel_max;
Value ServerOptions::player_decay= 0.5;
Value ServerOptions::player_rand= 0.1;
Value ServerOptions::player_size= 1.0;
Value ServerOptions::player_speed_max= 32.0;
//Value ServerOptions::player_weight;
Value ServerOptions::recover_dec= 0.05;
Value ServerOptions::recover_dec_thr= 0.4;
Value ServerOptions::recover_min= 0.1;
int ServerOptions::simulator_step= 100;
int ServerOptions::slow_down_factor=1;
Value ServerOptions::stamina_max= 2500.0;
Value ServerOptions::stamina_inc_max= 50.0;
bool ServerOptions::use_offside= true;
Value ServerOptions::visible_angle= 90.0;
Value ServerOptions::visible_distance= 3.0;

Value ServerOptions::tackle_dist= 2.5;
Value ServerOptions::tackle_back_dist= 0.5;
Value ServerOptions::tackle_width= 1.25;
int ServerOptions::tackle_exponent= 6;
int ServerOptions::tackle_cycles= 10;


Value ServerOptions::kickable_area= ServerOptions::kickable_margin + ServerOptions::ball_size + ServerOptions::player_size;
/* these are not from server.conf... */
Vector ServerOptions::own_goal_pos= Vector(-52.5,0.0);
Value ServerOptions::penalty_area_width= 40.32;
Value ServerOptions::penalty_area_length= 16.5;
Value ServerOptions::pitch_length=105.0;
Value ServerOptions::pitch_width= 68.0;
Vector ServerOptions::their_goal_pos= Vector(52.5,0.0);
Vector ServerOptions::their_left_goal_corner = Vector(52.5,7.01);
Vector ServerOptions::their_right_goal_corner = Vector(52.5,-7.01);

void ServerOptions::init() {
  //nothig to do
}

bool ServerOptions::read_from_file(char *config_file){
#if 1
  ERROR_OUT << "read from file no longer supported";
  exit(1);
#else
  FILE *server_conf;
  server_conf = fopen(config_file,"r");
  if (server_conf == NULL) {
    cerr << "\nCan't open file " << config_file 
	 << "\n>>>  This file should contain parameters of the current server!!!" << endl;
    return false;
  }
  //cout << "\nReading server's parameter from " << config_file << endl;
  char line[1000];
  char com[256];
  int n;
  while(fgets(line,1000,server_conf)!=NULL){
    n = sscanf(line,"%s", com) ;
    char *t = line ;
#define NULLCHAR        '\000'
    while(*t != NULLCHAR) {
      if (*t == ':') *t = ' ' ;
      t++ ;
    }
    double lf;
    if(*line=='\n'||*line=='#');  /* skip comments */
    else if (strncasecmp(line,"goal_width",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      goal_width= Value(lf);
    } 
    else if (strncasecmp(line,"penalty_area_width",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      penalty_area_width= Value(lf);
    } 
    else if (strncasecmp(line,"penalty_area_length",10)==0){
      n = sscanf(line, "%s %lf", com, &lf) ;
      penalty_area_length = Value(lf);
    } 
    else if (strncasecmp(line,"player_size",11)==0){
      n = sscanf(line, "%s %lf", com, &lf) ;
      player_size= Value(lf);
    } 
    else if (strncasecmp(line,"player_decay",12)==0){
      n = sscanf(line, "%s %lf", com, &lf) ;
      player_decay=  Value(lf);
    } 
    else if (strncasecmp(line,"player_rand",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      player_rand= Value(lf);
    } 
    else if (strncasecmp(line,"player_weight",13)==0){
      n = sscanf(line, "%s %lf", com, &lf) ;
      //player_weight = Value(lf);
    } 
    else if (strncasecmp(line,"player_speed_max",16)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      player_speed_max= Value(lf);
    }
    else if (strncasecmp(line,"player_accel_max",16)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //player_accel_max= Value(lf);
    }
    else if (strncasecmp(line,"stamina_max",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      stamina_max= Value(lf);
    } 
    else if (strncasecmp(line,"stamina_inc_max",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      stamina_inc_max= Value(lf);
    } 
    else if (strncasecmp(line,"recover_dec_thr",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      recover_dec_thr= Value(lf);
    } 
    else if (strncasecmp(line,"recover_dec",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      recover_dec= Value(lf);
    } 
    else if (strncasecmp(line,"recover_min",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      recover_min= Value(lf);
    } 
    else if (strncasecmp(line,"effort_dec_thr",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      effort_dec_thr= Value(lf);
    } 
    else if (strncasecmp(line,"effort_dec",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      effort_dec= Value(lf);
    } 
    else if (strncasecmp(line,"effort_inc_thr",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      effort_inc_thr= Value(lf);
    } 
    else if (strncasecmp(line,"effort_inc",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      effort_inc= Value(lf);
    } 
    else if (strncasecmp(line,"effort_min",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      effort_min= Value(lf);
    } 
    else if (strncasecmp(line,"hear_max",8)==0){
      //n = sscanf(line, "%s %d", com, &hear_max ) ;
    } 
    else if (strncasecmp(line,"hear_inc",8)==0){
      //n = sscanf(line, "%s %d", com, &hear_inc ) ;
    } 
    else if (strncasecmp(line,"hear_decay",10)==0){
      //n = sscanf(line, "%s %d", com, &hear_decay ) ;
    } 
    else if (strncasecmp(line,"inertia_moment",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      inertia_moment= Value(lf);
    } 
    else if (strncasecmp(line,"catchable_area_l",16)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      catchable_area_l= Value(lf);
    } 
    else if (strncasecmp(line,"catchable_area_w",16)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //catchable_area_w= Value(lf);
    } 
    else if (strncasecmp(line,"catch_probability",17)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //catch_probability= Value(lf);
    } 
    else if (strncasecmp(line,"catch_ban_cycle",15)==0){
      n = sscanf(line, "%s %d", com, &catch_ban_cycle ) ;
    }
    else if (strncasecmp(line,"goalie_max_moves",16)==0){
      //n = sscanf(line, "%s %d", com, &goalie_max_moves ) ;
    }
    else if (strncasecmp(line,"ball_size",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      ball_size= Value(lf);
    } 
    else if (strncasecmp(line,"ball_decay",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      ball_decay= Value(lf);
    } 
    else if (strncasecmp(line,"ball_rand",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      ball_rand= Value(lf);
    } 
    else if (strncasecmp(line,"ball_weight",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //ball_weight= Value(lf);
    } 
    else if (strncasecmp(line,"ball_speed_max",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      ball_speed_max= Value(lf);
    }
    else if (strncasecmp(line,"ball_accel_max",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      ball_accel_max= Value(lf);
    }
    else if (strncasecmp(line,"wind_force",10)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //wind_force= Value(lf);
    } 
    else if (strncasecmp(line,"wind_dir",8)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //wind_dir= Value(lf);
    } 
    else if (strncasecmp(line,"wind_rand",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //wind_rand= Value(lf);
    } 
    else if (strncasecmp(line,"kickable_margin",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      kickable_margin= Value(lf);
    } 
    else if (strncasecmp(line,"ckick_margin",12)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //ckick_margin= Value(lf);
    }
    else if (strncasecmp(line,"kick_rand",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //kick_rand= Value(lf);
    }
    else if (strncasecmp(line,"dash_power_rate",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      dash_power_rate= Value(lf);
    } 
    else if (strncasecmp(line,"kick_power_rate",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      kick_power_rate= Value(lf);
    } 
    else if (strncasecmp(line,"visible_angle",13)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      visible_angle= Value(lf);
    } 
    else if (strncasecmp(line,"audio_cut_dist",14)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //audio_cut_dist= Value(lf);
    } 
    else if (strncasecmp(line,"quantize_step",13)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //quantize_step= Value(lf);
    } 
    else if (strncasecmp(line,"quantize_step_l",15)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //quantize_step_l= Value(lf);
    } 
    else if (strncasecmp(line,"maxpower",8)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      maxpower= Value(lf);
    } 
    else if (strncasecmp(line,"minpower",8)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      minpower= Value(lf);
    } 
    else if (strncasecmp(line,"maxmoment",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //maxmoment= Value(lf);
    } 
    else if (strncasecmp(line,"minmoment",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //minmoment= Value(lf);
    } 
    else if (strncasecmp(line,"port",4)==0){
      //n = sscanf(line, "%s %d", com, &port ) ;
    } 
    else if (strncasecmp(line,"coach_port",10)==0){
      //n = sscanf(line, "%s %d", com, &coach_port ) ;
    } 
    else if (strncasecmp(line,"simulator_step",14)==0){
      n = sscanf(line, "%s %d", com, &simulator_step ) ;
    } 
    else if (strncasecmp(line,"send_step",9)==0){
      //n = sscanf(line, "%s %d", com, &send_step ) ;
    } 
    else if (strncasecmp(line,"recv_step",9)==0){
      //n = sscanf(line, "%s %d", com, &recv_step ) ;
    } 
    else if (strncasecmp(line,"half_time",9)==0){
      n = sscanf(line, "%s %d", com, &half_time ) ;
    } 
    else if (strncasecmp(line,"say_msg_size",12)==0){
      //n = sscanf(line, "%s %d", com, &say_msg_size ) ;
    } 
    else if (strncasecmp(line,"use_offside",11)==0){
      char use_offside_string[128];
      n = sscanf(line, "%s %s", com, use_offside_string ) ;
      use_offside = (!strncasecmp(use_offside_string, "on",2)) ? true : false ;
    } 
    else if (strncasecmp(line,"offside_active_area_size",24)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //offside_active_area_size= Value(lf);
    } 
    else if (strncasecmp(line,"forbid_kick_off_offside",23)==0){
      char forbid_kick_off_offside_string[128];
      n = sscanf(line, "%s %s", com, forbid_kick_off_offside_string ) ;
      //forbid_kick_off_offside = (!strncasecmp(forbid_kick_off_offside_string, "on",2)) ? true : false ;
      // cout << forbid_kick_off_offside << endl;
    } 
    else if (strncasecmp(line,"verbose",7)==0){
      char verbose_string[128];
      n = sscanf(line, "%s %s", com, verbose_string ) ;
      //verbose = (!strncasecmp(verbose_string, "on",2)) ? true : false ;
    } 
    else if (strncasecmp(line,"maxneckmoment",13)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      maxneckmoment = (Value)lf;
    } 
    else if (strncasecmp(line,"minneckmoment",13)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      minneckmoment = (Value)lf;
    } 
    else if (strncasecmp(line,"maxneckan",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      maxneckang = DEG2RAD((Value)lf);
    } 
    else if (strncasecmp(line,"minneckan",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //minneckang = DEG2RAD((Value)lf);
    } 
    else if (strncasecmp(line,"offside_kick_margin",9)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      //offside_kick_margin = (Value)lf;
    } 
    else if (strncasecmp(line,"record_version",14)==0){
      // not needed
    } 
    else if (strncasecmp(line,"send_log",8)==0){
      // not needed
    } 
    else if (strncasecmp(line,"log_file",8)==0){
      // not needed
    } 
    else if (strncasecmp(line,"record_log",10)==0){
      // not needed
    }
    else if (strncasecmp(line,"log_times",9)==0){
      char log_times_string[128];
      n = sscanf(line, "%s %s", com, log_times_string ) ;
      //log_times = (!strncasecmp(log_times_string, "on",2)) ? true : false ;
    }
    else if (strncasecmp(line,"send_vi_step",8)==0){
      // not needed
    } 
    else if (strncasecmp(line,"say_coach_msg_size",16)==0){
      // not needed
    } 
    else if (strncasecmp(line,"say_coach_cnt_max",16)==0){
      // not needed
    } 
    else if (strncasecmp(line,"sense_body_step",10)==0){
      // not needed
    }
    else if (strncasecmp(line,"clang_win_size",14)==0){
      //n = sscanf(line, "%s %d", com, &clang_win_size ) ;
    }
    else if (strncasecmp(line,"clang_define_win",16)==0){
      //n = sscanf(line, "%s %d", com, &clang_define_win ) ;
    }
    else if (strncasecmp(line,"clang_meta_win",14)==0){
      //n = sscanf(line, "%s %d", com, &clang_meta_win ) ;
    }
    else if (strncasecmp(line,"clang_advice_win",16)==0){
      //n = sscanf(line, "%s %d", com, &clang_advice_win ) ;
    }
    else if (strncasecmp(line,"clang_info_win",14)==0){
      //n = sscanf(line, "%s %d", com, &clang_info_win ) ;
    }
    else if (strncasecmp(line,"clang_mess_delay",16)==0){
      //n = sscanf(line, "%s %d", com, &clang_mess_delay ) ;
    }
    else if (strncasecmp(line,"clang_mess_per_cycle",20)==0){
      //n = sscanf(line, "%s %d", com, &clang_mess_per_cycle ) ;
    }
    else if (strncasecmp(line,"tackle_dist",11)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      tackle_dist= Value(lf);
    } 
    else if (strncasecmp(line,"tackle_back_dist",16)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      tackle_back_dist= Value(lf);
    } 
    else if (strncasecmp(line,"tackle_width",12)==0){
      n = sscanf(line, "%s %lf", com, &lf ) ;
      tackle_width= Value(lf);
    } 
    else if (strncasecmp(line,"tackle_exponent",15)==0){
      n = sscanf(line, "%s %d", com, &tackle_exponent ) ;
    }     
    else if (strncasecmp(line,"tackle_cycles",13)==0){
      n = sscanf(line, "%s %d", com, &tackle_cycles ) ;
    } 
    else {
      cerr << "\n Unkown option in server.conf! \n" << line << endl;
    } 
    // cout << " bei file einlesen " << line << endl;
   
  }
  // cout << " nach file einlesen " << endl;
  kickable_area = kickable_margin + ball_size + player_size;
#endif
  return true;
}

