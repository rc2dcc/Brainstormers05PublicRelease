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

#ifndef _MYSTATE_H_
#define _MYSTATE_H_

#include "angle.h"
#include "Vector.h"
#include "cmd.h"
#include "ws_info.h"

class MyState {

 public:
  Vector my_vel;
  Vector my_pos;
  Vector ball_vel;
  Vector ball_pos;
  ANGLE  my_angle;
  Vector op_pos;
  ANGLE op_bodydir;
  int op_bodydir_age;
  PPlayer op;

  void get_from_WS() {
    my_vel = WSinfo::me->vel;
    my_pos = WSinfo::me->pos;
    ball_vel = WSinfo::ball->vel;
    ball_pos = WSinfo::ball->pos;
    my_angle = WSinfo::me->ang;
  }

  MyState() { op=0; }
};

#endif
