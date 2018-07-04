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

#ifndef _ootrap_nb_H_
#define _ootrap_nb_H_

#include "base_bm.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/dribble_straight_bms.h"
#include "skills/neuro_kick05_bms.h"
#include "skills/face_ball_bms.h"
#include "skills/onetwo_holdturn_bms.h"
#include "intention.h"
#include "neuro_wball.h"


#define MAX_STEPS2INTERCEPT 40



  
class OvercomeOffsideTrap: public BaseBehavior{
  
 
  NeuroGo2Pos * go2pos;
  NeuroIntercept * neuroIntercept;
  NeuroKick05 * neuroKick;
  FaceBall *faceBall;
  OneTwoHoldTurn *  onetwoholdturn;

  
  //:begin BOTH
  int mode;
  PPlayer player;
  Vector playerpos;
  PPlayer taropp;
  Vector pos1;
  Vector pos2;
  Vector pos3;
  Value speed;
  int start_intercept_time;
  
  Vector ball_pos;
  
  int say_time;
  
  
  
  Value dist_ol;
  bool got_already;
  bool go_left;
  int run_time;
  

//:begin PLAYER 1
bool p1_hold(Cmd & cmd);
bool p1_pass(Cmd & cmd);
bool p1_run(Cmd & cmd);
bool p1_intercept(Cmd & cmd);
//PLAYER 1    :end



//:begin PLAYER 2
bool p2_go2pos2(Cmd & cmd);
bool p2_wait(Cmd & cmd);
bool p2_intercept(Cmd & cmd);
bool p2_pass(Cmd & cmd);
//PLAYER 2    :end


public:
  virtual ~OvercomeOffsideTrap() {
    delete faceBall;
    delete go2pos;
    delete neuroKick;
    delete neuroIntercept;
    delete onetwoholdturn;

  }
  static bool init(char const * conf_file, int argc, char const* const* argv);
  OvercomeOffsideTrap();
  bool test_nb(Cmd & cmd);
  bool test_wb(Cmd & cmd);
  bool get_cmd(Cmd & cmd);
  //void reset_intention();
};


#endif
