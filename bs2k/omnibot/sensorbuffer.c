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

#include "sensorbuffer.h"
#include <math.h>
#include <iostream.h>
//#include "Vector.h"
//extern void parse_sensor_data(char *argv, void *sensor_buffer);

char WM::my_team_name[20];
int  WM::my_side;
int  WM::my_number;
#if 0
int  WM::time;
int  WM::my_score;
int  WM::his_score;
PlayMode WM::play_mode;
#endif

ostream& operator<< (ostream& o, const Msg_sense_body & sb) {
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
    << " #t_neck= " << sb.turn_neck_count;
  return o;
}

ostream& operator<< (ostream& o, const Msg_see & see) {
  o << "\nMsg_see "
    << "\n  time= " << see.time;

  if (see.markers_num>0) {
    o << "\n  Markers (" << see.markers_num << ")";
    for (int i= 0; i < see.markers_num; i++) {
      cout << "\n  ( " << see.markers[i].x <<"," << see.markers[i].y << ")"
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
      default: o << "<???>";
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

ostream& operator<< (ostream& o, const Msg_hear & hear) {
  static const char *PlayModeString[] = PLAYMODE_STRINGS;
  o << "\nMsg_hear "
    << "\n  time= " << hear.time;

  if ( hear.play_mode_upd )
    o << "\n  play_mode= " << PlayModeString[hear.play_mode];

  if ( hear.my_score_upd )
    o << "\n  my_score= " << hear.my_score;

  if ( hear.his_score_upd )
    o << "\n  his_score= " << hear.his_score;

  if ( hear.communication_upd )
    o << "\n  heard communication (intrepretation not yet implemented)";
  return o;
}

ostream& operator<< (ostream& o, const Msg_init & init) {
  static const char *PlayModeString[] = PLAYMODE_STRINGS;
  o << "\nMsg_init "
    << "\n  side  = "; 
  switch (init.side) {
  case left_SIDE: o << "left"; break;
  case right_SIDE: o << "right"; break;
  default: o << "<???>";
  }
  o << "\n  number=    " << init.number
    << "\n  play_mode= " << PlayModeString[init.play_mode];

  return o;
}

ostream& operator<< (ostream& o, const Msg_mdpstate & mdp) {
  static const char *PlayModeString[] = PLAYMODE_STRINGS;
  o << "\n Msg_mdpstate "
    << "\n  time         = " << mdp.time
    << "\n  play_mode    = "   << PlayModeString[mdp.play_mode]
    << "\n  view_quality ";
  switch (mdp.view_quality) {
  case HIGH: o<< "HIGH"; break;
  case LOW: o<< "LOW"; break;
  default: o << "invalid value";
  }
 
  o << "\n  view_width   ";
  switch (mdp.view_width) {
  case WIDE: o<< "WIDE"; break;
  case NARROW: o<< "NARROW"; break;
  case NORMAL: o<< "NORMAL"; break;
  default: o << "invalid value";
  }
  o << "\n  my_score     = " << mdp.my_score
    << "\n  his_score    = " << mdp.his_score;
	 
  if (mdp.players_num>0) {
    o << "\n  Players (" << mdp.players_num << ")";
    for (int i= 0; i < mdp.players_num; i++) {
      o << "\n  team= ";
      switch (mdp.players[i].team) {
      case my_TEAM: o <<  "MY  "; break;
      case his_TEAM: o << "HIS "; break;
      default: o << "<???>";
      }
      o << " number= " << mdp.players[i].number
	<< " pos= (" << mdp.players[i].x <<"," << mdp.players[i].y << ")"
	<< " vel= (" << mdp.players[i].vel_x <<"," << mdp.players[i].vel_y << ")"
	<< " angle= " << mdp.players[i].angle
	<< " neck_angle= " << mdp.players[i].neck_angle
	<< " stamina " << mdp.players[i].stamina
	<< " effort " << mdp.players[i].effort
	<< " recovery " << mdp.players[i].recovery;
    }
  }

  o << "\n  Ball " 
    << " pos= (" << mdp.ball.x <<"," << mdp.ball.y << ")"
    << " vel= (" << mdp.ball.vel_x <<"," << mdp.ball.vel_y << ")";
  
  return o;
}

//////////////////////////////////////////////////
/* initialize everything */
SensorBuffer::SensorBuffer(){
  cout << "\nInit SensorBuffer" << endl << flush;
  sb = new Msg_sense_body();
  see= new Msg_see();
  hear= new Msg_hear();
  init= new Msg_init();
  mdpstate= new Msg_mdpstate();
}

//////////////////////////////////////////////////////////////////////
SensorBuffer::~SensorBuffer(){
  delete sb;
  delete see;
  delete hear;
  delete init;
}

//////////////////////////////////////////////////////////////////////
void SensorBuffer::parse_info(char *argv){
  /* defined in sensor_parser.y */
  parse_sensor_data(argv,(SensorBuffer *)this);
}


