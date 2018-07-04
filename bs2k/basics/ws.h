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

#ifndef _WS_H_
#define _WS_H_

#include <iostream>
/** 

Die Schnittstelle verwendet bei allen Angaben und unabhaengig von der
wirklichen Spielseite des Spielers das unten angegeben Koordinatensystem. Der
Koordinatenursprung befindet sich in der Mitte des Feldes. Die tatsaechliche
Position der Tore und Linien koennen aus den  Angaben ueber Feldmasse abgeleitet
werden. Sie sind aber insofern irrelevant, als dass es bei dieser Schnittstelle
nur um Positionen der beweglichen Objekte geht, unabhaengig von der
tatsaechlichen Feldgroesse. Das gegnerische Tor befinden sich immer rechts, also
in Richtung der X Achse.

      /------------------------------------------------\
      |                       |                        |
      |                       |                        |
      |                       |                        |
      |-------\               |                /-------|
      |       |               |                |       |
      |       |               |                |       |
      |       |               O <- (0,0)       |       | Goal of the opponent
      |       |               |                |       |
      |       |               |                |       |
      |-------/               |                \-------|
      |                       |                        |
      |                       |                        |
      |                       |                        |
      \------------------------------------------------/


            ^ Y axis
           /|\
	    |
            |
     -------O-------->  X axis
	    |
  	    |
	    |



Winkel werden im Bogenmass angegeben, d.h. jeder Winkel ist aus dem Intervall
[0,2*Pi) 

            
            | 0.5 Pi
	    |
   Pi       |          0
     -------O--------> 
	    |
  	    |
	    | 1.5 Pi
*/

#include "globaldef.h"
#include "types.h"
#include "Vector.h"
#include "angle.h"
#include "comm_msg.h"

/** WS = World State
    this is the successor of MDPstate
 */
struct WS {
  struct Player {
    Player() { alive= false; age= 100; time= -age; age_vel= age; direct_opponent_number = -1;}
    bool alive;    ///< if alive==false all other members take deliberate values

    int number;    ///< can be <= 0 if the number is unknown
    int team;      ///< takes values in MY_TEAM, HIS_TEAM, UNKNOWN_TEAM

    int time;      ///< time of last update of the position 
    int age;       ///< current time minus own time, just for user convenience!
    int age_vel;  ///< time of last update of the velocity 
    int age_ang;  ///< time of last update of the angle

    Vector pos;
    Vector vel;

    ANGLE ang;
    ANGLE neck_ang;     ///< neck_ang is  the absolute neck angle, NOT relative to player angle
    ANGLE neck_ang_rel;
    
    double stamina;
    double effort;
    double recovery;  

    bool tackle_flag;  //true if the oppoent is tackling (it's a snapshop from time 'time')
    bool pointto_flag; //pointto_dir is only valid, if pointto_flag == true
    ANGLE pointto_dir; 

    int direct_opponent_number; //number of direct opponent as assigned by coach
    void set_direct_opponent_number(int nr) {direct_opponent_number=nr;}

    struct {
      bool valid;
      int age; //indicates how old this message is!
      Vector ball_pos;
      Vector ball_vel;
      int abs_time; //this is the absolute time when ball_pos and ball_vel will be valid!
      int rel_time;
    } pass_info;

    //player type information
    double radius;         ///< radius of the player's body
    double speed_max;
    double dash_power_rate;
    double decay;
    double stamina_inc_max;
    double inertia_moment;
    double stamina_demand_per_meter;

    double kick_radius; ///< kick radius is the max distance from the middle of the player, where the ball is still kickable
    double kick_rand_factor;
  };

  struct Ball {
    Ball() { age= 100; time= -age; age_vel= age; invalidated= 0; }
    int time;
    int age;
    int age_vel;

    Vector pos;
    Vector vel;
    int invalidated; // 1 means ball was invalidated in current cycle, 2 means (possibly much) more the one cycle!
  };

  struct JoystickInfo {
    JoystickInfo() { valid= false; }
    bool valid;
    Vector stick1;
    Vector stick2;
  };

  int time;
  int time_of_last_update; 
  long ms_time_of_see_delay;
  long ms_time_of_sb;
  long ms_time_of_see;    // -1 if see messages are being ignored
   
  //char StateInfoString[2000]; //compact representation of state for messaging
  
  int play_mode;
  int penalty_count;
  int my_team_score;
  int his_team_score;
   
  int my_goalie_number;
  int his_goalie_number;

  int my_attentionto; // <= 0 if no attention is set
#if 0
  struct{
    Value dir;
    Value dist;
    int number;
    int seen_at;
    bool tackle_flag;
    int number_of_sensed_opponents;
  } closest_attacker; // the closest player to me; If team is known, then only opponents are considered
#endif
  // ... more information values
  
  int view_angle;   //takes values WIDE,NORMAL,NARROW defined in types.h
  int view_quality; //tekes values HIGH,LOW defined in types.h

  struct{
    int heart_at;
    int sent_at;
    int from;
  } last_message;
  int last_successful_say_at;   // time, when my message was broadcast to others
  
  Ball ball;
  Player my_team[NUM_PLAYERS+1];
  Player his_team[NUM_PLAYERS+1];
  int my_team_num;
  int his_team_num;
  SayMsg msg;
  JoystickInfo joystick_info;
  WS();
};

std::ostream& operator<< (std::ostream& o, const WS & ws) ;
std::ostream& operator<< (std::ostream& o, const WS::Player & p) ;
std::ostream& operator<< (std::ostream& o, const WS::Ball & b) ;

#endif
