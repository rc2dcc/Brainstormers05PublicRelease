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

#ifndef _JKWball03_BMC_H_
#define _JKWball03_BMC_H_
#include "base_bm.h"
#include "skills/score_bms.h"
#include "intention.h"
#include "neuro_wball.h"
/*class has been removed 05/05, only stubs remain to preserve compilability*/
class JKWball03: public BaseBehavior {
 public:
  JKWball03() {};
  virtual ~JKWball03() {};
  bool get_cmd(Cmd & cmd) {return false;};
  void reset_intention() {};
  Score *score;
  struct{
    Intention pass_or_dribble_intention;
    Intention intention;
    NeckRequest neckreq;
  } my_blackboard;
  void check_write2blackboard() {};
  bool check_previous_intention(Intention prev_intention, 
    Intention  &new_intention) {return false;};
  bool intention2cmd(Intention &intention, Cmd &cmd) {return false;};
};
#endif
