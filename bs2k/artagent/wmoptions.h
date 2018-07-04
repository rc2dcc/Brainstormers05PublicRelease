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

/*
 *  Author:   Artur Merke 
 */

#ifndef _WMOPTIONS_H_
#define _WMOPTIONS_H_

#include "globaldef.h"
//#include <iostream>


class WMoptions {
 public:  
  static long ms_time_max_wait_after_sense_body_long;
  static long ms_time_max_wait_after_sense_body_short;
  static long ms_time_max_wait_select_interval;
    
  static long s_time_normal_select_interval;
  //static const long s_time_normal_select_interval= 15;

  static int  DUMMY; //just for testing
  static int  EUMMY; //just for testing

  static bool foresee_opponents_positions;
  static bool offline;  //don't connect to the server, just start the first behavior

  static bool use_fullstate_instead_of_see;
  static bool behave_after_fullstate;
  static bool behave_after_think;
  static bool disconnect_if_idle;
  static bool send_teamcomm;
  static bool recv_teamcomm;
  static bool ignore_fullstate;
  static bool ignore_sense_body;  
  static bool ignore_see;
  static bool ignore_hear;
  static bool use_joystick;
  static char joystick_device[MAX_NAME_LEN];
  static bool use_pfilter;

  static int max_cycles_to_forget_my_player; //test
  static int max_cycles_to_forget_his_player; //test

  static bool save_msg_times;
  static int his_goalie_number; //this is especially usefull for pure fullstate agents, where no goalie info is sended
  static bool use_aserver_ver_7_for_sending_commands;

  static void set_mode_competition() {
    use_fullstate_instead_of_see= false;
    send_teamcomm= true;
    recv_teamcomm= true;
    disconnect_if_idle= true;
    behave_after_fullstate= false;
    behave_after_think= false;
    ignore_fullstate= true;
    ignore_sense_body= false;
    ignore_see= false;
    ignore_hear= false;
  }

  static void set_mode_test() {
    use_fullstate_instead_of_see= false;
    send_teamcomm= true;
    recv_teamcomm= true;
    disconnect_if_idle= true;
    behave_after_fullstate= false;
    behave_after_think= false;
    ignore_fullstate= false;
    ignore_sense_body= false;
    ignore_see= false;
    ignore_hear= false;
  }

  static void set_mode_aserver() {
    use_fullstate_instead_of_see= true;
    send_teamcomm= false;
    recv_teamcomm= false;
    disconnect_if_idle= true;
    behave_after_fullstate= true;
    behave_after_think= false;
    ignore_fullstate= false;
    ignore_sense_body= false;//true; // <- can be changed to false (then missed commands are recognized) 
    ignore_see= true;
    //ignore_hear= true;
    ignore_hear= false;
  }

  static void set_mode_synch_mode() {
    use_fullstate_instead_of_see= false;
    send_teamcomm= true;
    recv_teamcomm= true;
    disconnect_if_idle= true;
    behave_after_fullstate= false;
    behave_after_think= true;
    ignore_fullstate= false;
    ignore_sense_body= false;
    ignore_see= false;
    ignore_hear= false;
  }

  static void set_mode_synch_mode_with_fullstate() {
    use_fullstate_instead_of_see= true;
    send_teamcomm= true;
    recv_teamcomm= true;
    disconnect_if_idle= true;
    behave_after_fullstate= false;
    behave_after_think= true;
    ignore_fullstate= false;
    ignore_sense_body= false;
    ignore_see= false;
    ignore_hear= false;
  }

  static void init_options();
  static void read_options(char const* file, int argc, char const* const* argv);
  static void print_options();
};


#if 0
class BehaviorSet {
  static const int maxLineLength=1024;
  static const int MAX_SIZE=100;
  static char * tab[MAX_SIZE];
  static int size;
    
  static bool read_line(char const* line);
 public:

  static bool init(char const* file, int argc, char const* const* argv);
  static void show(ostream & out);
};
#endif

#endif
