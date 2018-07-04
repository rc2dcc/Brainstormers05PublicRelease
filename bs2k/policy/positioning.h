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

/** \file positioning.h
   To tackle the positioning problem we divide the position process into two steps:
   1. examin (all?) possible positions and select the n best ones for the whole team
   2. assign each available player a position according to a matching function
   It would be nice, if the matching would consider players that have special jobs like
   intercepting the ball.
   For learning purpose you should alter the function "assign_positions".
   The function "match_positions" is planned as a more or less static graph algorithm,
   thus no intelligence at all.
   For simplicity we agreed to select positions that are in between our own offside line
   and the offside line of the opponent. Our own offside line is defined by the 
   "test_position_moveup" function in class "Magnetic_Playon_Policy". 
*/

#ifndef _POSITIONING_H
#define _POSITIONING_H

#include "globaldef.h"
#include "Vector.h"

#define NEW433HACK //Art new formation

#ifdef NEW433HACK
#include "formation.h"
#include "formation433_attack.h"
#endif

#define EVALUATION_DISTANCE 0
#define EVALUATION_DISTANCE_WEIGHT 1
#define EVALUATION_DISTANCE_BALL 2

#define MATCHING_PLAYER 0
#define MATCHING_GREEDY 1
#define MATCHING_DIVIDE_THREE 2
#define MATCHING_DISTRIBUTION 3
#define MATCHING_MOVEUP 4

/* we create a special data structure that holds several informations about a position
 */

class AbstractPosition : public Vector{
 public:
};

class DashPosition : public AbstractPosition{
 public:
  DashPosition() {}
  DashPosition( const Vector & vec, Value dp, Value r ) { x= vec.x, y=vec.y, dash_power= dp, radius= r; }
  Value dash_power;
  Value radius;
};

class PrioPosition : public AbstractPosition{
 public:
  Value priority;
  Value radius;
};

class DeltaPosition : public DashPosition{
 public:
    int    role;
    Value  weight;
    int    player_assigned;
};


class AbstractPositioning{
 public:
  const static int NUM_CONSIDERED_POSITIONS = 10;
  static AbstractPosition pos[NUM_CONSIDERED_POSITIONS];

  static void select_positions();

  static void match_positions();

  /* returns the position for the calling player
     this function assumes that the matching orders the positions by player,
  */
  static inline AbstractPosition get_position(int player_number){return pos[player_number];}
  
};

class Formation{
 public:
    int    number;
    char   name[50];
    Value  pos[10][3];

    inline void print(){ std::cout << "\n(" << number << ")";}
};

#define MAX_NUM_FORMATIONS 20

class DeltaPositioning : public AbstractPositioning{
#ifdef NEW433HACK
  static Formation433 formation433;
  static bool use_new_formation433;
#endif
  DeltaPositioning();

 public:

  static Formation form[MAX_NUM_FORMATIONS];
  static int cnt_formations;
  static Value stretch;

  static int current_formation;
  static int current_matching;
  static int current_evaluation;

  static int four_chain_established;
  static bool use_BS02_gamestates;
  static Value before_goal_line;

  static Value max_defense_line;
  static Value min_defense_line_offset;
  static Value defense_line_ball_offset;

  static Value ball_weight_x_defense;
  static Value ball_weight_y_defense;
  static Value ball_weight_x_buildup;
  static Value ball_weight_y_buildup;
  static Value ball_weight_x_attack;
  static Value ball_weight_y_attack;
  static int cycles_after_catch;
  static double move_defense_line_to_penalty_area;
  
  static DeltaPosition pos[NUM_CONSIDERED_POSITIONS];
  static int recommended_attentionto[NUM_PLAYERS+1];
  static void show_recommended_attentionto() {
    std::cout << "\nAttention to recommendation:\n";
    for(int i=1; i < NUM_PLAYERS + 1; i++) 
      std::cout << "\nplayer_" << i << " " << recommended_attentionto[i];
  }

  // this array can be modified by the calling function to select the players
  static bool consider_player[11];

  // this function fills the array consider_players
  static void init_players(bool *players);
  static void init_formations();

  static DashPosition get_position(int player_number);
  static Vector get_position_base(int player_number);
  static int get_role(int player_number);
  static int get_num_players_with_role(int role);
  static inline int get_num_defenders(){return get_num_players_with_role(0);}
  static inline int get_num_offenders(){return get_num_players_with_role(2);}
  static inline int get_num_midfield(){return get_num_players_with_role(3);}

  static Value get_my_defence_line();
  static Value get_my_offence_line();

  static Value evaluate(int player, int position);

  static Value get_radius(int player_number);

  static bool is_position_valid(Vector p); 

  static Value get_defence_line_ball_offset();

  static Formation433Attack attack433;

};

class DashStamina{
 public:
    Value stamina_offence;
    Value dash_offence;
    Value stamina_defence;
    Value dash_defence;
};


class Stamina{
 public:

  static int stamina_management_type;
  static int state,last_update;
  static Value stamina_reserve_defenders;
  static Value stamina_reserve_midfielders;
  static Value stamina_reserve_attackers;
  static Value stamina_full_level;
  static Value stamina_min_reserve_level;
  static Value stamina_min_reserve;

  static DashStamina dashstamina[3];
  

  static void init();
  static Value economy_level();
  static int dash_power();
  static void update_state();
  static int get_state();

};



#endif //_POSITIONING_H_
