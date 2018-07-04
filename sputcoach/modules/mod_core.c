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

/* Author: Manuel "Sputnick" Nickschas, 02/2002
 *
 * This is the main coach module, offering core functionalities.
 *
 */

#include "coach.h"
#include "mod_defs.h"
#include "mod_core.h"
#include "messages.h"
//#include "modules.h"

ModCore::ModCore() {};

bool ModCore::init(int argc,char **argv) {
  //cout << "\nModCore init!";

  return true;
}

bool ModCore::destroy() {
  cout << "\nModCore destroy!";

  return true;
}

bool ModCore::behave() {
  //cout << "\nModCore behave!";

  return true;
}

bool ModCore::onRefereeMessage(bool PMChange) {
  //cout << "\nModCore referee! " << PMChange;

  return true;
}

bool ModCore::onHearMessage(const char *str) {
  //cout << "\nHEAR: "<<str;
  return true;
}

bool ModCore::onKeyboardInput(const char *str) {
  if(str[0]=='(') {
    RUN::sock.send_msg(str,strlen(str)+1);    
    return true;
  }
  
  return false;
}

const char ModCore::modName[]="ModCore";
