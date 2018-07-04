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

#ifndef _BASIC_CMD_BMS_H_
#define _BASIC_CMD_BMS_H_

/* This contains the interface to the base commands of soccer server.
   
*/

#include "../base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"

class BasicCmd : public BaseBehavior {
  static bool initialized;

  enum bm_types {KICK,DASH,TURN,TACKLE,CATCH,MOVE,TURN_INERTIA};
  
  int type;
  long type_set;

  Value power,moment;
  ANGLE direction;
  Vector pos;
  int priority;

  void do_turn_inertia(Cmd_Main &cmd, Value moment);
  
 public:
  void set_kick(Value power, ANGLE direction);
  void set_turn(Value moment);  // soccer server format: -180 ~ 180 degrees
  void set_dash(Value power, int priority=0);
  void set_tackle(Value power);
  void set_catch(ANGLE direction);
  void set_move(Vector pos);
  void set_turn_inertia(Value moment); // soccer server format: -180 ~ 180 degrees 
  
  bool get_cmd(Cmd &cmd);
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    //cout << "\nBasicCmd behavior initialized.";
    return true;
  }
  BasicCmd() { type_set=-1;}
};

#endif
