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

#include "basic_cmd_bms.h"
#include "tools.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"

#define BASELEVEL 3

bool BasicCmd::initialized=false;

void BasicCmd::set_kick(Value pwr, ANGLE dir) {
  power=pwr;direction=dir;
  type=KICK;type_set=WSinfo::ws->time;
}

void BasicCmd::set_turn(Value mmnt) {
  moment=mmnt;
  type=TURN;type_set=WSinfo::ws->time;
}

void BasicCmd::set_dash(Value pwr, int prio) {
  power = pwr; priority=prio;
  type=DASH;type_set=WSinfo::ws->time;
}

void BasicCmd::set_tackle(Value pwr) {
  power=pwr;
  type=TACKLE;type_set=WSinfo::ws->time;
}

void BasicCmd::set_catch(ANGLE dir) {
  direction=dir;
  type=CATCH;type_set=WSinfo::ws->time;
}

void BasicCmd::set_move(Vector position) {
  pos=position;
  type=MOVE;type_set=WSinfo::ws->time;
}

void BasicCmd::set_turn_inertia(Value mmnt) {
  moment=mmnt;
  type=TURN_INERTIA;type_set=WSinfo::ws->time;
}

bool BasicCmd::get_cmd(Cmd & cmd) {
  if(type_set!=WSinfo::ws->time) {
    ERROR_OUT<<"BasicCmd: Type not set before calling get_cmd()!";
    return false;
  }
  switch(type) {
  case KICK:
    cmd.cmd_main.set_kick(power,direction.get_value_mPI_pPI()); break;
  case DASH:
    cmd.cmd_main.set_dash(power,priority); break;
  case TACKLE:
    cmd.cmd_main.set_tackle(power); break;
  case CATCH:
    cmd.cmd_main.set_catch(direction.get_value_mPI_pPI()); break;
  case TURN:
    cmd.cmd_main.set_turn(moment); break;
  case MOVE:
    cmd.cmd_main.set_moveto(pos.x,pos.y); break;
  case TURN_INERTIA:
    do_turn_inertia(cmd.cmd_main,moment); break;
  default:
    ERROR_OUT<<"BasicCmd: Unknown command type!";
    return false;
  }
  return true;
}

void BasicCmd::do_turn_inertia(Cmd_Main &cmd, Value moment) {
  moment = Tools::get_angle_between_mPI_pPI(moment);
  
  moment = moment * (1.0 + (WSinfo::me->inertia_moment * (WSinfo::me->vel.norm())));
  /* this does not work, since PI == 3.141593 > pi 
     this means that an angle greater PI results in a turn with the opposite turn direction
  if (angle > +PI) angle = +PI;
  if (angle < -PI) angle = -PI;
  */  
  if (moment > 3.14) moment = 3.14;
  if (moment < -3.14) moment = -3.14;

  //if ((ClientOptions::doku_level)) printf("turn manipulated: %f \n ",(float)p_moment);
  moment = Tools::get_angle_between_null_2PI(moment);
  cmd.set_turn(moment);
}
