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

#ifndef _ATTACK_MOVE1_WB_BMC_H_
#define _ATTACK_MOVE1_WB_BMC_H_

#include "base_bm.h"
#include "skills/basic_cmd_bms.h"
#include "skills/neuro_kick_bms.h"
#include "skills/selfpass_bms.h"
#include "skills/dribble_straight_bms.h"
#include "skills/one_step_kick_bms.h"
#include "skills/onetwo_holdturn_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/score_bms.h"
#include "../policy/abstract_mdp.h"
#include "intention.h"
#include "neuro_wball.h"

class Attack_Move1_Wb: public BaseBehavior {
 public:
  Attack_Move1_Wb();
  virtual ~Attack_Move1_Wb();
  bool get_cmd(Cmd & cmd);
  static bool do_move();
};


#endif
