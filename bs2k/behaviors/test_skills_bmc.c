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

#include "test_skills_bmc.h"

#include "ws_info.h"
#include "log_macros.h"

bool TestSkillsPlayer::initialized=false;

bool TestSkillsPlayer::get_cmd(Cmd & cmd) {
  switch(WSinfo::ws->play_mode){
  case PM_PlayOn:
    //cmd.cmd_main.set_dash(100);
    go2pos->set_target( Vector(0,0) );
    return go2pos->get_cmd(cmd);
    break;
  default:
    cmd.cmd_main.set_turn(1.57);
  }

  LOG_DEB(0, << "now in cycle : " << WSinfo::ws->time);

  LOG_DEB(0, << _2D << C2D(WSinfo::me->pos.x, WSinfo::me->pos.y, 2, "ff0000") );
  if ( WSinfo::ws->time % 100 == 1 ) {
    std::cout << "\nnow in cycle " << WSinfo::ws->time << std::flush;
  }

  return true;
}
