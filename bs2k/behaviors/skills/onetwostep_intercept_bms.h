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

#ifndef ONEOR2STEP_INTERCEPT_BMS_H_
#define ONEOR2STEP_INTERCEPT_BMS_H_

#include "../base_bm.h"
#include "mystate.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"

class OneTwoStepInterceptItrActions {
  /* set params */
  
  /* neuro_intercept uses dashs */
  static const Value dash_power_min = -100;
  static const Value dash_power_inc = 20;
  static const Value dash_power_max= 100;
  static const Value dash_power_steps = (int)((dash_power_max - dash_power_min)/dash_power_inc) +1;
  
  /* neuro_intercept uses turns */
  static const Value turn_min= -M_PI;  // important: start with strongest turn to the right
  static const Value turn_max= M_PI;  // and end with strongest turn to the left!
  //  static const Value turn_inc= 2*M_PI/16.0; // this gives 16 increments
  //static const Value turn_inc= 2*M_PI/72.0;
  static const Value turn_inc= 2*M_PI/72.0;
  //static const Value turn_inc= 2*M_PI/180.0;
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



class OneTwoStep_Intercept : public BaseBehavior {
  static bool initialized;
  ANGLE my_angle_to(const Vector & my_pos, const ANGLE &my_angle, Vector target);
 private:
  bool get_1step_cmd(Cmd & cmd, Value &sqrdist, const MyState &state);
  bool get_2step_cmd(Cmd & cmd, Value &sqrdist, int &steps, const MyState &state);
  bool get_1step_cmd_virtual(Value &step2sqrdist, Value &step3sqrdist, const MyState &state);


  Value step1success_sqrdist;
  Value step2success_sqrdist;

 public:
  bool get_cmd(Cmd & cmd, const MyState &state);
  bool get_cmd(Cmd & cmd, const MyState &state, int &steps);
  bool get_cmd(Cmd & cmd, int &steps);
  bool get_cmd(Cmd &cmd);
 
  OneTwoStep_Intercept::OneTwoStep_Intercept();
  static bool init(char const * conf_file, int argc, char const* const* argv); 
};

#endif
