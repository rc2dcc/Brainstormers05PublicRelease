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

#ifndef _TRAIN_SKILLS_BMC_H_
#define _TRAIN_SKILLS_BMC_H_

#include "base_bm.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/neuro_kick_bms.h"

class TrainSkillsPlayer: public BaseBehavior {
  static bool initialized;
  static Vector target;
  static int num_cycles_in_trial;
  NeuroGo2Pos * go2pos;
  NeuroIntercept * neurointercept;
  NeuroKick *neurokick;

  bool playon(Cmd & cmd);

public:
  static bool TrainSkillsPlayer::init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    bool result = NeuroGo2Pos::init(conf_file,argc,argv) 
      && NeuroKick::init(conf_file,argc,argv) && NeuroIntercept::init(conf_file,argc,argv);
    return result;
  }

  TrainSkillsPlayer();
  bool get_cmd(Cmd & cmd);
};


#endif
