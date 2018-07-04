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
 * This file contains stuff to deal with modules.
 *
 */

#ifndef _MODULES_H_
#define _MODULES_H_

#include "defs.h"
#include "field.h"
#include "options.h"

class AbstractModule {

 public:

  virtual bool init(int argc,char **argv)=0;// {return false;}
  virtual bool destroy() {return false;}
  
  virtual bool behave() {return false;}           // called every cycle right after see_global message
  virtual bool onRefereeMessage(bool PMChange) {return false;} // called on every referee message
  virtual bool onKeyboardInput(const char*) {return false;}    // return true if processed!
  virtual bool onHearMessage(const char *) {return false;}     // called on hear messages (no referee)
  virtual bool onChangePlayerType(bool ownTeam,int unum,int type=-1) {return false;}
                                                               // called on player change
  
  static const char modName[];//="Abstract";
  virtual const char *getModName()=0;

};

//const char AbstractModule::modName[];

namespace MOD {

  const int MAX_MODULES=20;
  
  extern int numModules;
  extern AbstractModule *coachModule[MAX_MODULES];

  extern AbstractModule *getModule(const char *str); // this one is implemented in mod_defs.h!
  void loadModules();
  void initModules(int argc,char **argv);
  void destroyModules();
  
  void unloadModules();
  
};

#endif
