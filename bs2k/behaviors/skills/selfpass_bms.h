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

#ifndef _SELFPASS_BMS_H_
#define _SELFPASS_BMS_H_

#include "../base_bm.h"
#include "Vector.h"
#include "angle.h"
#include "cmd.h"
#include "basic_cmd_bms.h"
#include "oneortwo_step_kick_bms.h"
#include "valueparser.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"
#include "../../policy/abstract_mdp.h"

class Selfpass: public BaseBehavior {
  static bool initialized;
  BasicCmd *basic_cmd;
  OneOrTwoStepKick *onetwostepkick;
  int optime2react[50];

 public:
  void determine_optime2react(const ANGLE targetdir, const int max_dashes);
  bool is_selfpass_safe(const ANGLE targetdir, Value &speed, Vector &ipos, int &steps, Vector &attacking_op,
			int & op_number, const int max_dashes = 10, Value op_time2react =0);
  //op_time2react is the time that is assumed the opponents need to react. 0 is worst case, that they
  // are maximally quick, 1 assumes
  // that they need 1 cycle to react. This is already pretty aggressive and (nearly) safe

  bool is_selfpass_safe( const AState & state, const ANGLE targetdir, Value &speed, Vector &ipos, int &steps,
			 Vector &attacking_op, int & op_number, const int max_dashes,
			 Value op_time2react );
  void determine_optime2react( const AState & state, const ANGLE targetdir, const int max_dashes );


  bool get_cmd(Cmd &cmd);
  bool get_cmd(Cmd & cmd, const ANGLE targetdir, const Value speed, const Vector target);
  bool get_cmd(Vector my_pos,Vector my_vel,ANGLE my_ang,Vector ball_pos,Vector ball_vel,
Vector opp_pos,ANGLE opp_ang,Cmd & cmd, const ANGLE targetdir, const Value speed, const Vector target);

  static bool init(char const * conf_file, int argc, char const* const* argv);
  Selfpass();
  virtual ~Selfpass();
};

#endif
