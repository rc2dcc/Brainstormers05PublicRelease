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

#ifndef _NECK_CMD_BMS_H_
#define _NECK_CMD_BMS_H_

/* This contains the interface to the base commands of soccer server.
   
*/

#include "../base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"
#include "mdp_info.h"

class NeckCmd : public BaseBehavior {
  static bool initialized;

  Value moment;
  
 public:
  void set_turn_neck_abs(Value abs_dir_P) { 
    moment = mdpInfo::moment_to_turn_neck_to_abs( abs_dir_P );
  }
  
  void set_turn_neck_rel(Value rel_dir_P) { 
    moment = mdpInfo::moment_to_turn_neck_to_rel( rel_dir_P );
  }

  void set_turn_neck(Value moment_P) {
    moment = moment_P;
  }

  bool get_cmd(Cmd &cmd) {
    if(!cmd.cmd_neck.is_lock_set()) {    
      cmd.cmd_neck.set_turn( moment );
    } else {
      ERROR_OUT << " ERROR IN NECK_CMD BEHAVIOR: NECK CMD WAS ALREADY SET!!!";
    }
    return true;
  }
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    return true;
  }
  NeckCmd() {}
  virtual ~NeckCmd() {}
};


#endif // _NECK_CMD_BMS_H_
