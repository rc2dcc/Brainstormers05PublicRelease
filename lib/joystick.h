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
 * Copyright (c) 2002, Artur Merke <amerke@ira.uka.de> 
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

struct Joystick {
  static const int MAX_LEN= 512;
  static const int MAX_AXIS_VALUE= 32767;
  static const int MIN_AXIS_VALUE= -MAX_AXIS_VALUE;
  int joystick_fd;
  int num_axes;
  int num_buttons;
  int version;
  char name[MAX_LEN];
  int * button;
  int * axis;
  bool * button_chg;
  bool * axis_chg;

  Joystick();
  bool init(const char * joydev);
  int read_all_events();
};

#endif
