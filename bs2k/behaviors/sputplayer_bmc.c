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

#include "sputplayer_bmc.h"

#include "ws_info.h"
#include "log_macros.h"
#include "tools.h"
#include "ws_memory.h"

bool SputPlayer::initialized=false;

bool SputPlayer::get_cmd(Cmd & cmd) {
  //LOG_POL(0,<<"goal last seen: "<<WSmemory::last_seen_to_point(HIS_GOAL_LEFT_CORNER));
  //LOG_POL(0,<<" dir last seen: "<<WSmemory::last_seen_in_dir(0));
  Vector dumvec;
  int dumint;
  Vector dest = HIS_GOAL_LEFT_CORNER - Vector(2,2);
  switch(WSinfo::ws->play_mode) {
  case PM_PlayOn:
#if 0
    if(!WSinfo::is_ball_kickable()) {
    //go2pos->set_target(WSinfo::ball->pos);
    //go2pos->get_cmd(cmd);
      std::cerr << "\n#"<<WSinfo::ws->time<<" Ball not kickable!";
      intercept->get_cmd(cmd);
    } else {
      Value dumspeed;
      int dumsteps,dumnr;
      Vector dumipos,dumop;
      if(selfpass->is_selfpass_safe(ANGLE(0),dumspeed,dumipos,dumsteps,dumop,dumnr)) {
	LOG_POL(0,<<"Passing to self");
	selfpass->get_cmd(cmd,ANGLE(0),dumspeed,dumipos);
      } else {	
	if(std::fabs(WSinfo::me->ang.get_value_mPI_pPI())>.1) {
	  LOG_POL(0,<<"Turning ["<<-WSinfo::me->ang.get_value()<<"]");
	  cmd.cmd_main.set_turn(-WSinfo::me->ang.get_value());
	} else {
	  if(dribblestraight->is_dribble_safe(1)) {
	    dribblestraight->get_cmd(cmd);
	  } else {
	    if(holdturn->is_holdturn_safe()) {
	      holdturn->get_cmd(cmd);
	    } else {
	      std::cerr << "\n#"<<WSinfo::ws->time<<"No move possible!";
	      cmd.cmd_main.set_turn(0);
	    }
	  }
	}
      }
    }
#else
    if(!WSinfo::is_ball_pos_valid()) {
      faceball->turn_to_ball();
      faceball->get_cmd(cmd);
      return true;
    }
    if(!WSinfo::is_ball_kickable()) {
      if(flg) {
	std::cerr << "\nCyc #"<<WSinfo::ws->time<<": Lost ball unexpectedly!";
	flg=false;
      }
      intercept->get_cmd(cmd);
      return true;
    }
    if(!holdturn->is_holdturn_safe()) {
      LOG_POL(0,<<"HoldTurn not safe, kicking ball away!");
      neurokick->kick_to_pos_with_initial_vel(.6,Vector(0,0));
      neurokick->get_cmd(cmd);
      flg=false;
      return true;
    }
    flg=true;
    LOG_POL(0,<<"HoldTurn SAFE!");
    holdturn->get_cmd(cmd);
    return true;
    

#endif
  break;
  default:
    ;
    //cmd.cmd_main.set_turn(1.57);
  }

  //LOG_DEB(0, << "now in cycle : " << WSinfo::ws->time);

  //LOG_DEB(0, << _2D << C2D(WSinfo::me->pos.x, WSinfo::me->pos.y, 2, "ff0000") );
  //if ( WSinfo::ws->time % 100 == 1 ) {
  //  cout << "\nnow in cycle " << WSinfo::ws->time << flush;
  //}

  if(!cmd.cmd_main.is_cmd_set()) cmd.cmd_main.set_turn(0);
  return true;

}
