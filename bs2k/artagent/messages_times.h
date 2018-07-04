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
#ifndef _MESSAGES_TIMES_H_
#define _MESSAGES_TIMES_H_

#include "wmdef.h"
#include <iostream>

struct MessagesTimes {
  static void init(bool create_buffer);

  //static const int MAX_SIZE= 30000;
  static int MAX_SIZE;

  struct Entry {
    short ms_time_diff;
    short time;
    short type;
    short param;
  };

  static long  last_msg_ms_time[MESSAGE_MAX];
  static short last_msg_time[MESSAGE_MAX];
  static long ms_time_last;
  static int size;
  static Entry * entry;

  static void reset();

  static void warning(std::ostream & out, const short * msg_time, const long * msg_ms_time, int msg_idx, long ms_time);
  static void add(int time, long ms_time, int type, int param= 0);

  static void add_before_behave(int time, long ms_time) {
    add(time,ms_time,MESSAGE_BEFORE_BEHAVE);
  }

  static void add_after_behave(int time, long ms_time) {
    add(time,ms_time,MESSAGE_AFTER_BEHAVE);
  }

  static void add_lost_cmd(int time, int cmd_type) {
    add(time,ms_time_last,MESSAGE_CMD_LOST, cmd_type);
  }

  static void add_sent_cmd(int time, int cmd_type) {
    add(time,ms_time_last,MESSAGE_CMD_SENT, cmd_type);
  }

  static bool save(std::ostream & out);

  static bool save(const char * fname);
};

#endif
