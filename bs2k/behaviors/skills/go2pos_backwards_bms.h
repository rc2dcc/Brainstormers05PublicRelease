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

/** \file move_go2pos_backwards.h
    We define a very simple move to go backwards...
*/

#include "../base_bm.h"
#include "Vector.h"
#include "cmd.h"
#include "basic_cmd_bms.h"

#ifndef _GO2POSBACKWARDS_BMS_H_
#define _GO2POSBACKWARDS_BMS_H_

class Go2PosBackwards : public BaseBehavior {
  static bool initialized;

  BasicCmd *basic_cmd;

  /** target position */
  Vector target;
  
  /** accuracy at which we want to reach the position */
  double accuracy;

  /** accuracy for the angle to the target */
  double angle_accuracy;

  Value dash_power_needed_for_distance(Value dist);

public:
  /** constructor */
  void set_params(double p_x=-52, double p_y=0, double p_accuracy=1.0, double p_angle_accuracy=0.05);

  bool get_cmd(Cmd &cmd);
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if ( initialized ) 
      return true;
    initialized = true;

    return BasicCmd::init(conf_file, argc, argv);
  }

  Go2PosBackwards();
  virtual ~Go2PosBackwards();

};

#endif //_GO2POSBACKWARDS_BMS_H_







