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
 * This file contains information about available modules.
 *
 * To add a module, change information here and don't forget
 * to add a section for your module in coach.conf, too!
 *
 */

#ifndef _MOD_DEFS_H_
#define _MOD_DEFS_H_

#include "str2val.h"

/** Include module header files here */

#include "mod_core.h"
#include "mod_change.h"
#include "mod_analyse.h"
#include "mod_art.h"
#include "mod_train.h"
#include "mod_demo_train.h"
#include "mod_bs2k.h"
#include "mod_setsit.h"

/** Add a line for your module here */
AbstractModule *MOD::getModule(const char *name) {

  if(!strcmp(name,ModCore::modName)) {return new ModCore();}
  if(!strcmp(name,ModChange::modName)) {return new ModChange();}
  if(!strcmp(name,ModAnalyse::modName)) {return new ModAnalyse();}
  if(!strcmp(name,ModArt::modName)) {return new ModArt();}   
  if(!strcmp(name,ModTrain::modName)) {return new ModTrain();}
  if(!strcmp(name,ModDemoTrain::modName)) {return new ModDemoTrain();}
  if(!strcmp(name,ModBS2K::modName)) {return new ModBS2K();}   
  if(!strcmp(name,ModSetSit::modName)) {return new ModSetSit();}  
  
  return NULL;
}

#endif
