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

#ifndef _BS03_VIEW_BMV_H_
#define _BS03_VIEW_BMV_H_

/* This is a port of the BS02_View_Strategy (in advanced mode).
   This strategy has been used since Fukuoka '02 and tries to get
   maximum view efficency by changing the view angle to normal whenever possible
   without losing cycles.

   The original view strategy has been heavily improved while porting to the
   behavior structure; the agent now usually (i.e. as long as we have no network probs)
   only needs to sync directly after connecting to the server and never loses
   synchronisation with the server afterwards.
*/

#include "../base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"
#include <iostream>

class BS03View : public ViewBehavior {
  static bool initialized;

  int cur_view_width;
  int next_view_width;
  int init_state;
  long last_normal;
  long lowq_counter;
  long lowq_cycles;
  long missed_commands;
  long cyc_cnt;
  bool need_synch;

  int time_of_see();
  Value get_delay();
  bool can_view_normal();
  bool can_view_normal_strict();

  void change_view(Cmd &cmd, int width, int quality);
  
 public:
  bool get_cmd(Cmd &cmd);
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    std::cout << "\nBS03View behavior initialized.";
    return true;
  }

  BS03View();
};

#endif
