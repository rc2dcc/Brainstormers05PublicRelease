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

#ifndef _SPUTPLAYER_BMC_H_
#define _SPUTPLAYER_BMC_H_

#include "base_bm.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_kick_bms.h"
#include "skills/one_step_kick_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/intercept_ball_bms.h"
#include "skills/basic_cmd_bms.h"
#include "skills/dribble_straight_bms.h"
#include "skills/face_ball_bms.h"
#include "skills/search_ball_bms.h"
#include "skills/onetwo_holdturn_bms.h"
#include "skills/selfpass_bms.h"
#include "skills/neuro_intercept_bms.h"

/** This is a test player, solely for Sputnick, so don't mess around here ;-) */

class SputPlayer: public BaseBehavior {
  static bool initialized;
  NeuroKick *neurokick;
  NeuroGo2Pos *go2pos;
  OneStepKick *onestepkick;
  OneOrTwoStepKick *oneortwo;
  NeuroIntercept *intercept;
  BasicCmd *basic;
  OneTwoHoldTurn *holdturn;
  DribbleStraight *dribblestraight;
  SearchBall *searchball;
  FaceBall *faceball;
  Selfpass *selfpass;

  bool flg;
  
public:
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    
    return (
	    NeuroKick::init(conf_file,argc,argv) &&
	    NeuroGo2Pos::init(conf_file,argc,argv) &&
	    OneStepKick::init(conf_file,argc,argv) &&
	    OneOrTwoStepKick::init(conf_file,argc,argv) &&
	    NeuroIntercept::init(conf_file,argc,argv) &&
	    BasicCmd::init(conf_file,argc,argv) &&
	    OneTwoHoldTurn::init(conf_file,argc,argv) &&
	    DribbleStraight::init(conf_file,argc,argv) &&
	    SearchBall::init(conf_file,argc,argv) &&
	    FaceBall::init(conf_file,argc,argv) &&
	    Selfpass::init(conf_file,argc,argv)
	    );
  }
  SputPlayer() {
    neurokick = new NeuroKick;
    go2pos = new NeuroGo2Pos;
    onestepkick = new OneStepKick;
    oneortwo = new OneOrTwoStepKick;
    intercept = new NeuroIntercept;
    basic = new BasicCmd;
    holdturn = new OneTwoHoldTurn;
    dribblestraight = new DribbleStraight;
    searchball = new SearchBall;
    faceball = new FaceBall;
    selfpass = new Selfpass;
  }
  virtual ~SputPlayer() {
    delete neurokick; delete go2pos; delete onestepkick; delete oneortwo; delete intercept;
    delete basic; 
    delete searchball; delete faceball; delete dribblestraight;
  }
  bool get_cmd(Cmd & cmd);
};


#endif
