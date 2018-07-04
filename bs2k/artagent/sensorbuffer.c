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
#include "sensorbuffer.h"
#include <iostream>

char WM::my_team_name[20];
int  WM::my_team_name_len;
int  WM::my_side;
int  WM::my_number;

double WM::my_radius;
double WM::my_kick_radius;
double WM::my_kick_margin;
double WM::my_inertia_moment;
double WM::my_dash_power_rate;
double WM::my_decay;

double WM::last_export_KONV;
int  WM::time;

std::ostream& operator<< (std::ostream& o, const Msg_sense_body & sb) {
  o << "\nMsg_sense_body "
    << "\n  time         " << sb.time
    << "\n  view_quality ";
  switch (sb.view_quality) {
  case HIGH: o<< "HIGH"; break;
  case LOW: o<< "LOW"; break;
  default: o << "invalid value";
  }
  o << "\n  view_width   ";
  switch (sb.view_width) {
  case WIDE: o<< "WIDE"; break;
  case NARROW: o<< "NARROW"; break;
  case NORMAL: o<< "NORMAL"; break;
   default: o << "invalid value";
  }
  o << "\n  stamina      " << sb.stamina
    << "\n  effort       " << sb.effort
    << "\n  speed_value  " << sb.speed_value
    << "\n  speed_angle  " << sb.speed_angle
    << "\n  neck_angle   " << sb.neck_angle
    << "\n  #k= " << sb.kick_count
    << " #d= " << sb.dash_count
    << " #t= " << sb.turn_count
    << " #s= " << sb.say_count
    << " #t_neck= " << sb.turn_neck_count
    << " #c= " << sb.catch_count
    << " #m= " << sb.move_count
    << " #c_view= " << sb.change_view_count;

    
  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_see & see) {
  o << "\nMsg_see "
    << "\n  time= " << see.time;

  if (see.markers_num>0) {
    o << "\n  Markers (" << see.markers_num << ")";
    for (int i= 0; i < see.markers_num; i++) {
      o << "\n  ( " << see.markers[i].x <<"," << see.markers[i].y << ")"
	<< " #= " << see.markers[i].how_many
	<< " dist= " << see.markers[i].dist
	<< " dir= " << see.markers[i].dir
	<< " dist_change= " << see.markers[i].dist_change
	<< " dir_change= " << see.markers[i].dir_change
	<< ")";
    }
  }

  if (see.players_num>0) {
    o << "\n  Players (" << see.players_num << ")";
    for (int i= 0; i < see.players_num; i++) {
      o << "\n team= ";
      switch (see.players[i].team) {
      case my_TEAM: o <<  "MY  "; break;
      case his_TEAM: o << "HIS "; break;
      default: o << "< ? >";
      }
      o << " number= " << see.players[i].number 
	<< " goalie= " << see.players[i].goalie 
	<< " #= " << see.players[i].how_many
	<< " dist= " << see.players[i].dist
	<< " dir= " << see.players[i].dir
	<< " dist_change= " << see.players[i].dist_change
	<< " dir_change= " << see.players[i].dir_change
	<< " body_dir= " << see.players[i].body_dir
	<< " head_dir= " << see.players[i].head_dir;
    }
  }

  if ( see.line_upd )
    o << "\n  Line " 
      << "\n  ( " << see.line.x <<"," << see.line.y << ")"
      << " #= " << see.line.how_many
      << " dist= " << see.line.dist
      << " dir= " << see.line.dir
      << " dist_change= " << see.line.dist_change
      << " dir_change= " << see.line.dir_change;
  
  if ( see.ball_upd)
    o << "\n  Ball " 
      << "\n " 
      << " #= " << see.ball.how_many
      << " dist= " << see.ball.dist
      << " dir= " << see.ball.dir
      << " dist_change= " << see.ball.dist_change
      << " dir_change= " << see.ball.dir_change;

  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_hear & hear) {
  o << "\nMsg_hear "
    << "\n  time= " << hear.time;

  if ( hear.play_mode_upd )
    o << "\n  play_mode= " << show_play_mode(hear.play_mode);

  if ( hear.my_score_upd )
    o << "\n  my_score= " << hear.my_score;

  if ( hear.his_score_upd )
    o << "\n  his_score= " << hear.his_score;

  if ( hear.teamcomm_upd )
    o << "\n  heard communication: "
      << hear.teamcomm;
  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_init & init) {
  o << "\nMsg_init "
    << "\n  side  = "; 
  switch (init.side) {
  case left_SIDE: o << "left"; break;
  case right_SIDE: o << "right"; break;
  default: o << "< ? >";
  }
  o << "\n  number=    " << init.number
    << "\n  play_mode= " << show_play_mode(init.play_mode);

  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_fullstate & fs) {
  o << "\n Msg_fullstate "
    << "\n  time         = " << fs.time
    << "\n  play_mode    = "   << show_play_mode(fs.play_mode)
    << "\n  view_quality ";
  switch (fs.view_quality) {
  case HIGH: o<< "HIGH"; break;
  case LOW: o<< "LOW"; break;
  default: o << "invalid value";
  }
 
  o << "\n  view_width   ";
  switch (fs.view_width) {
  case WIDE: o<< "WIDE"; break;
  case NARROW: o<< "NARROW"; break;
  case NORMAL: o<< "NORMAL"; break;
  default: o << "invalid value";
  }
  o << "\n  my_score     = " << fs.my_score
    << "\n  his_score    = " << fs.his_score;
	 
  if (fs.players_num>0) {
    o << "\n  Players (" << fs.players_num << ")";
    for (int i= 0; i < fs.players_num; i++) {
      o << "\n  team= ";
      switch (fs.players[i].team) {
      case my_TEAM: o <<  "MY  "; break;
      case his_TEAM: o << "HIS "; break;
      default: o << "< ? >";
      }
      o << " number= " << fs.players[i].number
	<< " pos= (" << fs.players[i].x <<"," << fs.players[i].y << ")"
	<< " vel= (" << fs.players[i].vel_x <<"," << fs.players[i].vel_y << ")"
	<< " angle= " << fs.players[i].angle
	<< " neck_angle= " << fs.players[i].neck_angle
	<< " stamina " << fs.players[i].stamina
	<< " effort " << fs.players[i].effort
	<< " recovery " << fs.players[i].recovery;
    }
  }

  o << "\n  Ball " 
    << " pos= (" << fs.ball.x <<"," << fs.ball.y << ")"
    << " vel= (" << fs.ball.vel_x <<"," << fs.ball.vel_y << ")";
  
  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_fullstate_v8 & fs) {
  o << "\n Msg_fullstate "
    << "\n  time         = " << fs.time
    << "\n  play_mode    = "   << show_play_mode(fs.play_mode)
    << "\n  view_quality ";
  switch (fs.view_quality) {
  case HIGH: o<< "HIGH"; break;
  case LOW: o<< "LOW"; break;
  default: o << "invalid value";
  }
 
  o << "\n  view_width   ";
  switch (fs.view_width) {
  case WIDE: o<< "WIDE"; break;
  case NARROW: o<< "NARROW"; break;
  case NORMAL: o<< "NORMAL"; break;
  default: o << "invalid value";
  }
  o << "\n  count        = " 
    << " k:" << fs.count_kick
    << " d:" << fs.count_dash
    << " t:" << fs.count_turn
    << " c:" << fs.count_catch
    << " m:" << fs.count_move
    << " tn:" << fs.count_turn_neck
    << " cv:" << fs.count_change_view
    << " s:" << fs.count_say;

  o << "\n  my_score     = " << fs.my_score
    << "\n  his_score    = " << fs.his_score;

  
  o << "\n  Ball " 
    << " pos= (" << fs.ball.x <<"," << fs.ball.y << ")"
    << " vel= (" << fs.ball.vel_x <<"," << fs.ball.vel_y << ")";
	 
  if (fs.players_num>0) {
    o << "\n  Players (" << fs.players_num << ")";
    for (int i= 0; i < fs.players_num; i++) {
      o << "\n  team= ";
      switch (fs.players[i].team) {
      case my_TEAM: o <<  "MY  "; break;
      case his_TEAM: o << "HIS "; break;
      default: o << "< ? >";
      }
      if (fs.players[i].goalie)
	o << " (goalie)";

      o << " number= " << fs.players[i].number
	<< " pos= (" << fs.players[i].x <<"," << fs.players[i].y << ")"
	<< " vel= (" << fs.players[i].vel_x <<"," << fs.players[i].vel_y << ")"
	<< " angle= " << fs.players[i].angle
	<< " neck_angle= " << fs.players[i].neck_angle
	<< " stamina " << fs.players[i].stamina
	<< " effort " << fs.players[i].effort
	<< " recovery " << fs.players[i].recovery;
    }
  }

  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_teamcomm & tc) {
  o << "\n Msg_teamcomm "
    << "\n time       = " << tc.time
    << "\n time_cycle = " << tc.time_cycle
    << "\n from       = " << tc.from;
	
  if (tc.his_goalie_number_upd) 
    o << "\n his_goalie_number= " << tc.his_goalie_number;
  else
    o << "\n no his_goalie_number";

  if (tc.ball_upd) {
    o << "\n ball"
      << "\n how_old= " << tc.ball.how_old <<" x= " << tc.ball.x << " y= " << tc.ball.y << " vel_x= " << tc.ball.vel_x << " vel_y= " << tc.ball.vel_y;
  }
  else
    o << "\n no ball";

  if (tc.players_num>0) {
    o << "\n Players (" << tc.players_num << ")";
    for (int i= 0; i < tc.players_num; i++) {
      o << "\n how_old= " << tc.players[i].how_old
	<< " team= ";
      switch (tc.players[i].team) {
      case my_TEAM: o <<  "MY  "; break;
      case his_TEAM: o << "HIS "; break;
      default: o << "< ? >";
      }
      o << " number= " << tc.players[i].number
	<< " pos= (" << tc.players[i].x <<"," << tc.players[i].y << ")";
    }
  }
  return o;
}

std::ostream& operator<< (std::ostream& o, const Msg_teamcomm2 & tc) {
  o << " Msg_teamcomm2 ";

  o << "\n";
  if (tc.msg.valid) {
    o << " | msg" << " from= " << tc.msg.from << " p1= " << tc.msg.param1 << " p2= " << tc.msg.param2;
  }
  else
    o << " | no msg";
  o << "\n";
  if (tc.ball.valid) {
    o << " | ball"
      << "  x= " << tc.ball.pos.x << " y= " << tc.ball.pos.y;
  }
  else
    o << " | no ball";

  o << "\n";
  if (tc.pass_info.valid) {
    o << " | pass "
      << "  x= " << tc.pass_info.ball_pos.x << " y= " << tc.pass_info.ball_pos.y
      << "  vx= " << tc.pass_info.ball_vel.x << " vy= " << tc.pass_info.ball_vel.y
      << " t= " << tc.pass_info.time;
  }
  else
    o << " | no pass info";

  o << "\n";
  if (tc.ball_info.valid) {
    o << " | ball_info "
      << "  x= " << tc.ball_info.ball_pos.x << " y= " << tc.ball_info.ball_pos.y
      << "  vx= " << tc.ball_info.ball_vel.x << " vy= " << tc.ball_info.ball_vel.y
      << "  ap= " << tc.ball_info.age_pos << " av= " << tc.ball_info.age_vel;
  }
  else
    o << " | no ball info";

  if (tc.ball_holder_info.valid) {
    o << " | ball_holder "
      << "  x= " << tc.ball_holder_info.pos.x << " y= " << tc.ball_holder_info.pos.y;
  }
  else
    o << " |no ball_holder";


  o << "\n"; 
  if (tc.players_num == 0) {
    o << " | no players";
    return o;
  }

  o << " | Players (" << tc.players_num << ")";
  for (int i= 0; i < tc.players_num; i++) {
    o << " | team= ";
    switch (tc.players[i].team) {
    case my_TEAM: o <<  "MY  "; break;
    case his_TEAM: o << "HIS "; break;
    default: o << "< ? >";
    }
    o << " number= " << tc.players[i].number
      << " pos= (" << tc.players[i].pos.x <<"," << tc.players[i].pos.y << ")";
  }
  return o;
}


std::ostream& operator<< (std::ostream& o, const Msg_my_online_coachcomm & moc) {
  o << "\n Msg_my_online_coachcomm "
    << "\n time= " << moc.time;
  
  if ( ! moc.his_player_types_upd ) 
    o << "\n his_player_types_upd= false";
  else {
    o << "\n his_player_types=";
    for (int i=0; i<NUM_PLAYERS; i++)
      o << " p_" << i+1 << ":" << moc.his_player_types[i];
  }
  
  
  return o;
}
