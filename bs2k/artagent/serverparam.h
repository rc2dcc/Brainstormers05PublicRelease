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
#ifndef _SERVERPARAM_H_
#define _SERVERPARAM_H_

#include "sensorbuffer.h"
#include "server_options.h"

class ServerParam {
  static Msg_server_param * server_param;
  static Msg_player_param * player_param;
  static Msg_player_type  ** player_types;
  static Msg_player_type  worst_case_opponent_type;
 public:
  static int number_of_player_types();
  static bool all_params_ok();
  static bool incorporate_server_param_string(const char * buf);
  static bool incorporate_player_param_string(const char * buf);
  static bool incorporate_player_type_string(const char * buf);
  static bool export_server_options();
  static Msg_player_type const* get_player_type(int type);
  static Msg_player_type const* get_worst_case_opponent_type() {
    return &worst_case_opponent_type;
  }
};
#endif
