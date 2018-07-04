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

#ifndef _BS03_BMC_H_
#define _BS03_BMC_H_

#include "base_bm.h"
#include "noball_demo_bmp.h"
#include "wball_demo_bmp.h"
#include "attack_move1_wb_bmp.h"
#include "goal_kick_bmp.h"
#include "score04_bmp.h"
#include "standard_situation_bmp.h"
#include "goalie_bs03_bmc.h"
#include "skills/neuro_go2pos_bms.h"
#include "line_up_bmp.h"
#include "penalty_bmp.h"
#include "ootrap_bmp.h"  //hauke

//#define TRAINING

class Bs03: public BaseBehavior {
  static bool initialized;
  NoballDemo *noball_demo;
  Attack_Move1_Wb *attack_move1_wb;
  WballDemo *wball_demo;
  Penalty *penalty;
  Score04 *score04;
  GoalKick *goalkick;
  StandardSituation * standardSit;
  FaceBall *faceball;
  Goalie_Bs03 *goalie_bs03;
  NeuroGo2Pos *go2pos;
  LineUp *line_up;
  OvercomeOffsideTrap *ootrap; //hauke

  bool do_standard_kick;
  bool do_goal_kick;
  long ivLastOffSideTime;
public:
  static int cvHisOffsideCounter;
  static bool init(char const * conf_file, int argc, char const* const* argv);
  void select_relevant_teammates();
  void reset_intention();
  Bs03() {
    ivLastOffSideTime = -999;
    score04=new Score04;
    attack_move1_wb = new Attack_Move1_Wb;
    noball_demo = new NoballDemo;
    wball_demo = new WballDemo;
    penalty = new Penalty;
    goalkick = new GoalKick;
    faceball = new FaceBall;
    standardSit = new StandardSituation;
    goalie_bs03 = new Goalie_Bs03;
    go2pos = new NeuroGo2Pos;
    line_up = new LineUp();
    ootrap = new OvercomeOffsideTrap();   //hauke

    do_standard_kick=do_goal_kick=false;
  }
  virtual ~Bs03() {
    delete score04;
    delete noball_demo;
    delete attack_move1_wb;
    delete wball_demo;
    delete goalkick;
    delete faceball;
    delete standardSit;
    delete goalie_bs03;
    delete go2pos;
    delete line_up;
    delete ootrap;  //hauke 
    delete penalty;
  }
  bool get_cmd(Cmd & cmd);
};


#endif
