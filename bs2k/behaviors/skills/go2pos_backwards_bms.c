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

#include "go2pos_backwards_bms.h"
#include "tools.h"
#include "ws_info.h"

bool Go2PosBackwards::initialized = false;

Go2PosBackwards::Go2PosBackwards() {
  basic_cmd = new BasicCmd;
}

Go2PosBackwards::~Go2PosBackwards() {
  delete basic_cmd;
}

void Go2PosBackwards::set_params(double p_x, double p_y, double p_accuracy, double p_angle_accuracy){
  target.x = p_x;
  target.y = p_y;
  accuracy = p_accuracy;
  angle_accuracy = p_angle_accuracy;
}

Value Go2PosBackwards::dash_power_needed_for_distance(Value dist) {
  Vector v = Vector(1.0,0.0);
  v.rotate(WSinfo::me->ang.get_value());

  Value vel = WSinfo::me->vel.x * v.x + WSinfo::me->vel.y * v.y;
  Value power = (dist - vel*2.0/3.0) * 100.0;
  if (power > 100.0) return 100.0;
  else if (power < -100.0) return -100.0;
  else return power;
}

bool Go2PosBackwards::get_cmd(Cmd & cmd){
  double moment = 0;
  Value dist = target.distance(WSinfo::me->pos);
  if (dist < accuracy) {
    basic_cmd->set_turn(0.0);
    basic_cmd->get_cmd(cmd);
    return true;
  }

  if (fabs(Tools::get_angle_between_mPI_pPI(Tools::my_angle_to(target).get_value() - M_PI)) <= 
      angle_accuracy){
    basic_cmd->set_dash(-dash_power_needed_for_distance(dist));
    basic_cmd->get_cmd(cmd);
    return true;
    //cmd.cmd_main.set_dash(-fabs(dash_power));
  } else {
    basic_cmd->set_turn_inertia(Tools::get_angle_between_null_2PI(Tools::my_angle_to(target).get_value() - M_PI));
    basic_cmd->get_cmd(cmd);
    return true;
  }
}









