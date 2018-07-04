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
#ifndef _WM_H_
#define _WM_H_

struct Msg_player_type;

class WM {
 public:
  static char my_team_name[];
  static int my_team_name_len;
  static int my_side;
  static int my_number;

  static double my_radius;
  static double my_kick_radius;
  static double my_kick_margin;
  static double my_inertia_moment;
  static double my_decay;
  static double my_dash_power_rate;

  static double last_export_KONV;
  static int time;

  static void set_my_type( Msg_player_type const* pt); 
};

#define ID " (#" << WM::my_number << ", time=" << WM::time << ") "

#endif
