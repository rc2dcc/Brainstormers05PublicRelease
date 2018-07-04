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

#ifndef _TRAIN_BEHAVIORS_BMC_H_
#define _TRAIN_BEHAVIORS_BMC_H_

#include "base_bm.h"
#include "noball_demo_bmp.h"
#include "learn_wball_bmp.h"
#include "goal_kick_bmp.h"
#include "standard_situation_bmp.h"
#include "goalie_bs03_bmc.h"
#include "skills/neuro_go2pos_bms.h"
#include "line_up_bmp.h"

#define TRAINING

class TrainBehaviors: public BaseBehavior {
  static bool initialized;
  NoballDemo *noball_demo;
  LearnWball *learn_wball;
  GoalKick *goalkick;
  StandardSituation * standardSit;
  FaceBall *faceball;
  Goalie_Bs03 *goalie_bs03;
  NeuroGo2Pos *go2pos;
  LineUp *line_up;
  
  bool do_standard_kick;
  bool do_goal_kick;
public:
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if ( initialized )
      return true;
    initialized= true;
    return (
	    NoballDemo::init(conf_file,argc,argv) &&
	    LearnWball::init(conf_file,argc,argv) &&
	    GoalKick::init(conf_file,argc,argv) &&
	    FaceBall::init(conf_file,argc,argv) &&
	    StandardSituation::init(conf_file,argc,argv) &&
	    Goalie_Bs03::init(conf_file, argc, argv) &&
	    NeuroGo2Pos::init(conf_file, argc, argv) &&
	    LineUp::init(conf_file, argc, argv)
	    );
  }
  void reset_intention();
  TrainBehaviors() {
    noball_demo = new NoballDemo;
    learn_wball = new LearnWball;
    goalkick = new GoalKick;
    faceball = new FaceBall;
    standardSit = new StandardSituation;
    goalie_bs03 = new Goalie_Bs03;
    go2pos = new NeuroGo2Pos;
    line_up = new LineUp();

    do_standard_kick=do_goal_kick=false;
  }
  virtual ~TrainBehaviors() {
    delete noball_demo;
    delete learn_wball;
    delete goalkick;
    delete faceball;
    delete standardSit;
    delete goalie_bs03;
    delete go2pos;
    delete line_up;
  }
  bool get_cmd(Cmd & cmd);
};


#endif
