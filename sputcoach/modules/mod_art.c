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

/* Author: Art! , 02/2002
 *
 * This is a private module
 *
 */

#include "coach.h"
#include "mod_art.h"
#include "messages.h"
#include <string.h>

ModArt::ModArt() {};

bool ModArt::init(int argc,char **argv) {
  //cout << "\nModArt init!";

  return true;
}

bool ModArt::destroy() {
  cout << "\nModArt destroy!";

  return true;
}

bool ModArt::behave() {
  //cout << "\nModArt behave!";

  return true;
}

bool ModArt::onRefereeMessage(bool PMChange) {
  //cout << "\nModArt referee! " << PMChange;

  return true;
}

bool ModArt::onKeyboardInput(const char *str) {
  if(str[0]=='(') {
    RUN::sock.send_msg(str,strlen(str)+1);    
    return true;
  }
  if(!strcmp(str, "print_types")){
    RUN::printPlayerTypes();
    return true;
  }
  return false;
}

const char ModArt::modName[]="ModArt";
