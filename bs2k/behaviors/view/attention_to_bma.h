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

#ifndef _ATTENTION_TO_BMA_H_
#define _ATTENTION_TO_BMA_H_

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

class AttentionTo : public AttentionToBehavior {
  static bool initialized;

  bool teammate_controls_ball(int &number, Vector & pos);
  void generate_players4communication(Cmd & cmd_form);
  void construct_say_message(Cmd & cmd);
  void set_attention_to(Cmd & cmd);

#if 0
  bool get_teammate(int number, PPlayer & p);
  int relevant_teammate[11];
  int num_relevant_teammates;

  void set_relevant_teammates(const int t1=0,const int t2=0,const int t3=0,const int t4=0,
			      const int t5=0,const int t6=0,const int t7=0,const int t8=0,
			      const int t9=0,const int t10=0,const int t11=0);
#endif

#if 0
  void check_communicate_ball_and_mypos(Cmd & cmd);
  void communicate_players(Cmd & cmd_form);
#endif

 public:
  bool get_cmd(Cmd &cmd);
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    std::cout << "\nAttentionTo behavior initialized.";
    return true;
  }
  AttentionTo();
};

#endif // _ATTENTION_TO_BMA_H_
