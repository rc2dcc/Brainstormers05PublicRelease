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

#ifndef _SERVER_OPTIONS_H_
#define _SERVER_OPTIONS_H_

#include "globaltypes.h"
#include "Vector.h"
#include "angle.h"

/* don't edit this file unless YOU know what changes have to be done in other parts 
   of the code, so that this options are set correctly (ask <art>) 

   at the moment there is still support for reading a server.conf, but this will
   be removed in the beginning of 2002, and options will we read in only by using
   the (server_param ... ) message <art>
*/
struct ServerOptions { 
 public:
  ServerOptions();
  ~ServerOptions();

  static void init();
  static bool read_from_file(char *config_file);
  static Value ball_accel_max;
  static Value ball_decay;                 /** Decay rate of speed of the ball. */
  static Value ball_rand;                  /** Amount of noise added in the movements of the ball. */
  static Value ball_size;                  /** Radius of the ball. */
  static Value ball_speed_max;  /** Maximum speed of the ball during one simulation cycle */
  //static Value ball_weight;                /** Weight of the ball. This parameter concerns the wind factor */
  static Value catchable_area_l;  /** Goalie catchable area length. */
  //static Value catchable_area_w;   /** Goalie catchable area width. */
  static int catch_ban_cycle;/** The number of cycles the goalie is banned from catching the ball after a successful catch. */
  //static Value catch_probability;   /** The probability for a goalie to catch the ball (if it is not during the catch ban static interval) */
  static Value dash_power_rate;  /** Rate by which Power argument in dash command is multiplied.*/
  static Value effort_dec;  /** Decrement step for player's effort capacity. */
  static Value effort_dec_thr;        /** Decrement treshold for player's effort capacity. */
  static Value effort_inc;           /** Increment step for player's effort capacity. */
  static Value effort_inc_thr;        /** Increment treshold for player's effort capacity. */
  static Value effort_min; /** Minimum value for player's effort capacity. */
  //  static int goalie_max_moves;
  static Value goal_width;                 /** Width of the goal. For acquiring higher scores 14.02 was used in most cases*/
  static int half_time;   /** The length of a half time of a match. Unit is simulation cycle. */
  static Value inertia_moment;   /** Inertia moment of a player. It affects it's moves*/
  static Value kickable_area; /** The area within which the ball is kickable. kickable_area = kickable_margin + ball_size + player_size */
  static Value kickable_margin;
  static Value kick_power_rate;   /** Rate by which Power argument in cick command is multiplied.*/
  static ANGLE maxneckang;
  static ANGLE maxneckmoment;
  static Value maxpower;  /** Maximum value of Power argument in dash and kick commands. */
  static ANGLE minneckang;
  static ANGLE minneckmoment;
  static Value minpower;   /** Minimum value of Power argument in dash and kick commands. */
  //static Value player_accel_max;
  static Value player_decay;               /** player decay */
  static Value player_rand;                /** Amount of noise added in player's movements and turns. */
  static Value player_size;                /** Radius of a player. */
  static Value player_speed_max;   /** Maximum speed of a player during one simulation cycle */
  //  static Value player_weight;            /** Weight of a player. This parameter concerns the wind factor */
  static Value recover_dec;          /** Decrement step for player's recovery. */
  static Value recover_dec_thr;   /** Decrement treshold for player's recovery. */
  static Value recover_min;      /** Minimum player recovery. */
  static int simulator_step;   /** Length of period of simulation cycle. */
  static int slow_down_factor;  /** Slow down factor used by the soccer server. */
  static Value stamina_max;          /** Maximum stamina of a player */
  static Value stamina_inc_max;          /** Amount of stamina that a player gains in a simulation cycle. */
  static bool use_offside;   /** Flag for using offside rule [on/off] */
  static Value visible_angle;   /** Angle of view cone of a player in the standard view mode.*/
  static Value visible_distance ;   /*< visible distance */
  static Value tackle_dist;
  static Value tackle_back_dist;
  static Value tackle_width;
  static int tackle_exponent;
  static int tackle_cycles;


  /* these are not from server.conf... */
  static Vector own_goal_pos;   /** the position of our goal. */
  static Value penalty_area_width;   /** the width of the penalty area. */
  static Value penalty_area_length;    /** the length of the penalty area. */
  static Value pitch_length;  /** the length of the field. */
  static Value pitch_width;   /** the width of the field. */
  static Vector their_goal_pos; /** the position of their goal. */
  static Vector their_left_goal_corner;
  static Vector their_right_goal_corner;
};

#endif

