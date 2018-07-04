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

#ifndef _NEURO_GO2POS_BMS_H_
#define _NEURO_GO2POS_BMS_H_

#include "../base_bm.h"
#include "angle.h"
#include "Vector.h"
#include "cmd.h"
#include "n++.h"
#include "macro_msg.h"
#include "basic_cmd_bms.h"

class NeuroGo2PosItrActions {
  /* neurogo2pos uses dashs */
  static const Value dash_power_min = 40;
  static const Value dash_power_inc = 20;
  static const Value dash_power_max= 100;
  static const Value dash_power_steps = (int)((dash_power_max - dash_power_min)/dash_power_inc) +1;
  
  /* neurogo2pos uses turns */
  static const Value turn_min= -M_PI;  // important: start with strongest turn to the right
  static const Value turn_max= M_PI;  // and end with strongest turn to the left!
  static const Value turn_inc= 2*M_PI/72.0;
  static const int turn_steps=(int)((turn_max - turn_min)/turn_inc) +1;

  Cmd_Main action;
  Angle turn;
  int turn_counter;
  Value dash;
  int dash_counter;
  Value move;
 public:
  //ItrActions();
  static bool init();

  void reset() {
    turn= turn_min;
    dash= dash_power_min;
    turn_counter= 0;
    dash_counter= 0;
  }

  Cmd_Main * next() {
    if ( turn_counter < turn_steps ) {
      action.unset_lock();
      action.unset_cmd();
      // a bit tricky: make sure that maximum turn angles are contained in action set !!!!
      if(turn <= -(M_PI - 0.01))  // angle is smaller or approx. equal than max neg. turn 
	action.set_turn(-(M_PI - 0.00001/180. * PI));
	//action.set_turn(-M_PI);
      else if(turn >=(M_PI - 0.01))// angle is larger or approx. equal than max pos. turn 
	action.set_turn((M_PI - 0.00001/180. * PI));
	//action.set_turn(M_PI);
      else
	action.set_turn( turn );
      turn += turn_inc;
      turn_counter++;
      return &action;
    }
    
    if ( dash_counter < dash_power_steps ) {
      action.unset_lock();
      action.unset_cmd();
      if(dash>dash_power_max)
	dash=dash_power_max;
      action.set_dash( dash );
      dash += dash_power_inc;
      dash_counter++;
      return &action;
    }
    return 0;
  }
};





class NeuroGo2Pos: public BaseBehavior {

  BasicCmd *basic_cmd;
  int obstacle_found;

  static const Value MAX_GO2POS_DISTANCE= 5.0;
  static bool initialized;

  static int op_mode;
  static bool use_regular_states;

  struct State {
    Vector my_vel;
    Vector my_pos;
    ANGLE  my_angle;
    Vector opnt1_pos;
  };

  static const Value costs_failure = 1.0;
  static const Value costs_success = 0.0;
  static Value target_tolerance;
  static int consider_obstacles;
  static int use_old_go2pos;
  static Value costs_per_action;

  // important: do init first, since net loading might be aborted
  static Net * net;

  NeuroGo2PosItrActions itr_actions;
  Vector target_pos;

  void get_cur_state( State & state);
  void get_features( State const& state, Vector target, float * net_in);
  
  Value evaluate( State const& state, Vector const& target);
  bool is_failure( State const& state,  Vector const& target);
  bool is_success( State const& state,  Vector const& target);
  bool neuro_decide(Cmd & cmd);

  // learning stuff
  #define STATE_MEMORY_SIZE 5000
  #define NUM_FEATURES 8
  float uparams[10];


  struct PatternSet{
    long ctr;;
    float input[STATE_MEMORY_SIZE][NUM_FEATURES],target[STATE_MEMORY_SIZE];
  };

  static int num_stored;
  static int num_epochs;
  static int store_per_cycle;
  static int repeat_mainlearningloop;
  static int state_memory_ctr;

  typedef struct{
    State state;
    Vector target_pos;
  }   StateMemoryEntry;

  StateMemoryEntry state_memory[STATE_MEMORY_SIZE];
  PatternSet training_set;
   
  bool learn(Cmd &cmd);
  void check_cmd(Cmd &cmd);
  void store_state();
  void print_memory();
  void generate_training_patterns();
  void train_nn();
  Value get_value_of_best_successor(State const &state, Vector const& target);


 public:
  NeuroGo2Pos();
  static bool init(char const * conf_file, int argc, char const* const* argv);
  virtual ~NeuroGo2Pos();

  void set_target(Vector target, Value target_tolerance = 1.0, int cons_obstacles = 1, int use_old = 0);
  bool get_cmd(Cmd & cmd);
};


#endif
