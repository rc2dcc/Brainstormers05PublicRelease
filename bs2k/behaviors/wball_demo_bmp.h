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

#ifndef _WBALL_DEMO_BMC_H_
#define _WBALL_DEMO_BMC_H_

#include "base_bm.h"
#include "skills/basic_cmd_bms.h"
#include "skills/neuro_kick05_bms.h"
#include "skills/dribble_straight_bms.h"
#include "skills/one_step_kick_bms.h"
#include "skills/onetwo_holdturn_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/score_bms.h"
#include "../policy/abstract_mdp.h"
#include "intention.h"
#include "neuro_wball.h"

class WballDemo: public BaseBehavior {
  NeuroKick05 *neurokick;
  DribbleStraight *dribblestraight;
  BasicCmd *basiccmd;
  OneStepKick *onestepkick;
  OneOrTwoStepKick *oneortwo;
  OneTwoHoldTurn *onetwoholdturn;
  Score *score;
  NeuroWball *neuro_wball;

 private:
  bool in_penalty_mode;
  long last_at_ball;
  int at_ball_for_cycles;
  int cycles_in_waitandsee;
  long last_waitandsee_at;  
  long last_heavy_attack_at;
  PPlayer closest_opponent; // warning: might be 0!
  int my_role;

  struct{
    Intention pass_or_dribble_intention;
    Intention intention;
    NeckRequest neckreq;
  } my_blackboard;

  bool intention2cmd(Intention &intention, Cmd &cmd);

  bool test_default(Intention &intention);
  bool test_holdturn(Intention &intention);
  bool test_dribble(Intention &intention);
  bool test_pass_or_dribble(Intention &intention);

  bool is_dribble_possible();
  int I_am_heavily_attacked_since();

  bool get_pass_or_dribble_intention(Intention &intention);
  bool get_pass_or_dribble_intention(Intention &intention, AState &state);
  
  void aaction2intention(const AAction& aaction, Intention &intention);

  // auxillary functions for offensive_move:
  void get_onestepkick_params(Value &speed, Value &dir);
  void get_opening_pass_params(Value &speed, Value &dir, Vector &ipos, int &advantage, 
			       int & closest_teammate);
  bool am_I_attacked();

 protected:
 public:
  bool get_intention(Intention &intention);
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    return (
	    NeuroKick05::init(conf_file,argc,argv) &&
	    DribbleStraight::init(conf_file,argc,argv) &&
	    BasicCmd::init(conf_file,argc,argv) &&
	    OneStepKick::init(conf_file,argc,argv) &&
	    OneOrTwoStepKick::init(conf_file,argc,argv) &&
	    OneTwoHoldTurn::init(conf_file,argc,argv)
	    );
  }
  WballDemo();
  virtual ~WballDemo();
  bool get_cmd(Cmd & cmd);
  void reset_intention();
};

#endif
