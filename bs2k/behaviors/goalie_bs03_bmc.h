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

/** \file strategy_goalie.h
    The rulebased strategy for the goalie.
*/

#ifndef _GOALIE_BS03_BMC_H_
#define _GOALIE_BS03_BMC_H_

#include "base_bm.h"
#include "goalie03_bmp.h"
#include "goal_kick_bmp.h"
#include "line_up_bmp.h"
#include "skills/neuro_go2pos_bms.h"

class Goalie_Bs03 : public BaseBehavior {
  static bool initialized;
  Goalie03 *goalie03;
  GoalKick *goalkick;
  LineUp *line_up;
  NeuroGo2Pos *go2pos;

 protected:
  bool use_clever_moves;
 /** select a policy */
 public:
  static int INTERCEPT_BAN_STEPS; 
  static bool i_have_ball;
  static int intercept_ban;
  static int catch_ban;
  /** Very important !
      Since the playon method might be called !twice! during one timestep, we the catchban is set in the first run and in the second we cannot catch !!!
So we save the time when we try to catch and if the current time equals time_catch we reset the catchbancycle... 
  */
  static int time_flag;  

 
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if ( initialized )
      return true;
    initialized= true;
    return (
            NeuroGo2Pos::init(conf_file,argc,argv) &&
	    Goalie03::init(conf_file, argc, argv) &&
	    GoalKick::init(conf_file, argc, argv) &&
	    LineUp::init(conf_file, argc, argv)
	    );
  }
  Goalie_Bs03();

  virtual ~Goalie_Bs03() {
    delete goalie03;
    delete goalkick;
    delete line_up;
    delete go2pos;
  }

  void reset_intention();
  bool get_cmd(Cmd & cmd);

};

#endif //_GOALIE_BS03_BMC_H__
