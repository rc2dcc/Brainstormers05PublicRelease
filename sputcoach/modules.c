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
 * See messages.h for description..
 *
 */

#include <string.h>

#include "coach.h"
#include "modules.h"
//#include "mod_defs.h"
#include "logger.h"
#include "str2val.h"


using namespace MOD;

int MOD::numModules;
AbstractModule *MOD::coachModule[MAX_MODULES];

void MOD::initModules(int argc, char **argv) {
  cout << "\nInitialising modules...";
  
  for(int i=0;i<numModules;i++) {
    coachModule[i]->init(argc,argv);
  }
}

void MOD::destroyModules() {
  cout << "\nShutting down modules...";

  for(int i=0;i<numModules;i++) {
    coachModule[i]->destroy();
  }
}

void MOD::loadModules() {
  char buf[2048];
  char name[100];
  const char *str,*pnt;
  str=buf;
  numModules=0;
  cout << "\n------------------------------------------------------------------"
       << "\nLoading modules...\n";
  ValueParser vp(Options::coach_conf,"Modules");
  vp.get("LoadModules",buf,2048,"ModCore");
  AbstractModule *mod;
  bool flg=false;int cnt=0;
  do {
    if(!strfind(str,',',pnt)) {
      mod=getModule(str);
      if(!mod) cout <<"<"<<str<<"> ";
      flg=true;cnt++;
    } else {
      strncpy(name,str,pnt-str);
      name[pnt-str]='\000';
      mod=getModule(name);
      if(!mod) cout <<"<"<<name<<"> ";
      str=pnt+1;cnt++;
    }
    if(mod!=NULL) {
      coachModule[numModules++]=mod;
      cout << "[" << mod->getModName() << "] ";
    }
  } while (!flg);
  if(cnt!=numModules) cout << "\nWARNING: One or more modules could not be found!";
}


void MOD::unloadModules() {
  for(int i=0;i<numModules;i++) delete coachModule[i];
}
