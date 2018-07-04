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

#include "joystick.h"
#include <stdio.h>			
#include <stdlib.h>			
#include <string.h>	
#include <unistd.h> // wegen {u}sleep()

#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "macro_msg.h"

#ifndef ERROR_STREAM
#define ERROR_STREAM std::cerr
#endif

#ifndef ERROR_OUT
#define ERROR_OUT ERROR_STREAM << "\n\n*** ERROR file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#ifndef WARNING_STREAM
#define WARNING_STREAM std::cerr
#endif

#ifndef WARNING_OUT
#define WARNING_OUT WARNING_STREAM << "\n\n*** WARNING file=\"" << __FILE__ << "\" line=" << __LINE__
#endif


Joystick::Joystick() {
  joystick_fd= 0;
  num_axes= 0;
  num_buttons= 0;
  version= 0;
  button= 0;
  axis= 0;
  button_chg= 0;
  axis_chg= 0;
}

bool Joystick::init(const char * joydev) {
  if ((joystick_fd = open(joydev, O_RDONLY)) < 0) {
    ERROR_OUT << "\ncould't open joystic device " << joydev;
    return false;
  }
  fcntl(joystick_fd, F_SETFL, O_NONBLOCK); //don't block

  unsigned char num;
  ioctl(joystick_fd, JSIOCGVERSION, &version);
  ioctl(joystick_fd, JSIOCGAXES, &num);
  num_axes= num;
  ioctl(joystick_fd, JSIOCGBUTTONS, &num);
  num_buttons= num;
  ioctl(joystick_fd, JSIOCGNAME(MAX_LEN), name);
  INFO_OUT << "\nJoystick    = " << name
	   << "\nversion     = " << version
	   << "\nnum_axes    = " << num_axes
	   << "\nnum_buttons = " << num_buttons
	   << std::flush;

  axis= new int[num_axes];
  axis_chg= new bool[num_axes];
  button= new int[num_buttons];
  button_chg= new bool[num_buttons];

  return true;
}

int Joystick::read_all_events() {
  int got_data= 0;
  for (int i=0; i<num_buttons; i++)
    button_chg[i]= false;
  for (int i=0; i<num_axes; i++)
    axis_chg[i]= false;
  js_event js;

  while(1) { //read all joystick events
    int size= read(joystick_fd, &js, sizeof(struct js_event));
    if (size != sizeof(struct js_event)) 
      break;
      
    got_data++;
    switch(js.type & ~JS_EVENT_INIT) {
    case JS_EVENT_BUTTON:
      button[js.number]= js.value;
      button_chg[js.number]= true;
      break;
    case JS_EVENT_AXIS:
      axis[js.number] = js.value;
      axis_chg[js.number]= true;
      break;
    }
  }
  return got_data;
}


/* test */
#if 0
main() {
  Joystick js;
  if (!js.init("/dev/input/js0"))
    return 1;
  
#if 0 //polling
  while(1) {
    int num= js.read_all_events();
    cout << "\nread " << num << " joystick events";
    usleep(100000);
  }
#endif

  fd_set rfds;
  while(1) {
    FD_ZERO(&rfds);
    FD_SET(js.joystick_fd, &rfds);
    int retval = select(js.joystick_fd+1, &rfds, NULL, NULL, NULL);
    if (retval > 0 && FD_ISSET(js.joystick_fd,&rfds) ) {
      int num= js.read_all_events();
      cout << "\nread " << num << " joystick events";
    }
    else {
      WARNING_OUT << "\nstrange select event occurred";
    }
  }
  return 0;
}
#endif
