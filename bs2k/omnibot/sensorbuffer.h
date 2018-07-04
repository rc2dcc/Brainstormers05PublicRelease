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

 #ifndef _SENSOR_BUFFER_H_
 #define _SENSOR_BUFFER_H_

 /** \file sensor_buffer.h
     This file contains everything needed to store the temporary sensor information
     \author Alex Sinner
 */

#include <string.h>

#include "globaldef.h"
#include "wmdef.h"

extern "C" void parse_sensor_data(char *argv, void *sensor_buffer);

class WM {
 public:
  static char my_team_name[];
  static int my_side;
  static int my_number;
#if 0
  static int time;
  static int my_score;
  static int his_score;
  /** playmode
      note that this is not the same than in the MDPstate
  */
  static PlayMode play_mode;
#endif
};

//////////////////////////////////////////////////
/** This class can contain am see message       */
struct Msg_see {
  struct _see_marker {
    /** default constructor */
    _see_marker(){}
    /** in special cases as (Goal) or (Flag) there is no information about the Goal or Flag id,
	in such a situation see_position is set to false
     */
    bool see_position;
    /** the marker's x coordinate. */
    double x;
    /** the marker's x coordinate. */
    double y;
    /** information amount */
    int how_many;
    /** distance to the player*/
    double dist;
    /** direction from the player*/
    double dir;
    /** change of distance. 
	Note that it can be 0 (UNDEF_INFORMATION) when the marker is too far away */
    double dist_change;
    /** change of direction.
	Note that it can be 0 (UNDEF_INFORMATION) when the marker is too far away  */
    double dir_change;
  };

  typedef  _see_marker _see_line;

  struct _see_player {
    _see_player() {}
    /** which values for team: my_TEAM, his_TEAM, unknown_TEAM */
    int team;
    /** Uniform number */
    int number;
    /** is seen as goalie */
    bool goalie;
    /** information amount */
    int how_many;
    /** distance to the player*/  
    double dist;
    /** direction from the player*/
    double dir;
    /** change of distance.*/
    double dist_change;
    /** change of direction.*/
    double dir_change;
    /** direction of the body. */
    double body_dir;
    /** direction of the head. */
    double head_dir;
  };

  struct _see_ball{
    /** default constructor. */
    _see_ball(){}
    /** information amount */
    int how_many;
    /** distance to the player*/  
    double dist;
    /** direction from the player*/
    double dir;
    /** change of distance.*/
    double dist_change;
    /** change of direction.*/
    double dir_change;
  };

  Msg_see() { time= -1; markers_num= 0; players_num= 0; line_upd= false; ball_upd= false; }
  int time;
  static const int markers_MAX= 50;
  static const int players_MAX= 22;
  int markers_num;  //how many markers were updated
  int players_num;  //how many markers were updated
  bool line_upd;  //was there an update ot the line field
  bool ball_upd;  //was there an update ot the line field
  
  _see_marker markers[markers_MAX]; 
  _see_player players[players_MAX];
  _see_line   line;
  _see_ball   ball;
};

//////////////////////////////////////////////////
/** This class can contain am hear message       */
struct Msg_hear {
  Msg_hear() { time= -1; play_mode= PM_Null; 
               my_score= -1; my_score_upd= false; 
	       his_score= -1; his_score_upd= false;
               communication_upd= false;
  }
  int time;
  PlayMode play_mode;
  bool     play_mode_upd;
  int      my_score;
  bool     my_score_upd;
  int      his_score;
  bool     his_score_upd;
  //Comm   communication;
  bool     communication_upd;
};

struct Msg_init {
  Msg_init() { side= -9; number= -1; play_mode= PM_Null; }
  int side;
  int number;
  PlayMode play_mode;
};

struct Msg_mdpstate {
  Msg_mdpstate() { time= -1; players_num= 0; }
  static const int players_MAX= 22;
  int players_num;
  struct _mdp_player {
    int team;
    int number;
    double x;
    double y;
    double vel_x;
    double vel_y;
    double angle;
    double neck_angle;
    double stamina;
    double effort;
    double recovery;
  };
  struct _mdp_ball {
    double x;
    double y;
    double vel_x;
    double vel_y;
  };
  int time;  
  _mdp_player players[players_MAX];
  _mdp_ball ball;
  PlayMode play_mode;
  /** view quality */
  int /*View_Quality*/ view_quality;
  /** view width */
  int /*View_Width*/ view_width;
  int my_score;
  int his_score;
};

//////////////////////////////////////////////////
/** Sense-Body information. */
class Msg_sense_body {
 public:
  /** default constructor. */
  Msg_sense_body(){}
  /** timestep of last update. */
  int time;
  /** view quality */
  int /*View_Quality*/ view_quality;
  /** view width */
  int /*View_Width*/ view_width;
  /** stamina */
  double stamina;
  /** effort */
  double effort;
  /** speed */
  double speed_value;
  double speed_angle;
  /** head angle */
  double neck_angle;
  /** kick count */
  int kick_count;
  /** dash count */
  int dash_count;
  /** turn count */
  int turn_count;
  /** say count */
  int say_count;
  /** count of turn_neck commands */
  int turn_neck_count;
};

//////////////////////////////////////////////////

class SensorBuffer {

  friend void parse_sensor_data(char *argv, void *sensor_buffer);
  friend int siparse(void *);
  
  //private:
 public:
  /** contains information about sense_body information */
  Msg_sense_body *sb;
  /** contains information about see information */
  Msg_see *see;
  /** contains information about hear information */
  Msg_hear *hear;
  /** contains information about initialization information */
  Msg_init *init;
  /** contains full sever information (if supported by the server) */
  Msg_mdpstate *mdpstate;
  
 public:
  /** Default constructor */
  SensorBuffer();
  /** Default destructor */
  ~SensorBuffer();
  /** Parses Server Sensor Information.
      @param argv string to be parsed
  */
  void parse_info(char *);
};


ostream& operator<< (ostream& o, const Msg_sense_body& sb) ;
ostream& operator<< (ostream& o, const Msg_see& see) ;
ostream& operator<< (ostream& o, const Msg_hear& hear) ;
ostream& operator<< (ostream& o, const Msg_init& init) ;
ostream& operator<< (ostream& o, const Msg_mdpstate& mdpstate) ;
#endif 
