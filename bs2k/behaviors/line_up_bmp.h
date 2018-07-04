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

#ifndef _LINE_UP_H_
#define _LINE_UP_H_

#include "base_bm.h"
#include "skills/basic_cmd_bms.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/face_ball_bms.h"

class LineUp : public BaseBehavior {
  static bool initialized;
  
  NeuroGo2Pos *go2pos;
  BasicCmd *basic_cmd;
  FaceBall *face_ball;

 protected:

  //float home_kick_off[12][2];

 public:

  LineUp();
  virtual ~LineUp();
  ANGLE ivSectorAngle;

  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if ( initialized )
      return true;
    initialized= true;
    return (
	    NeuroGo2Pos::init(conf_file, argc, argv) &&
	    BasicCmd::init(conf_file, argc, argv) &&
	    FaceBall::init(conf_file, argc, argv)
	    );
  }

  bool get_cmd(Cmd & cmd);
  bool get_cmd(Cmd & cmd, bool sectorBased);

};

#endif //_LINE_UP_H_

