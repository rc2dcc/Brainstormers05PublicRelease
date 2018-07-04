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

#ifndef _MAIN_BEHAVIORS_
#define _MAIN_BEHAVIORS_

#include <string.h>

#include "bs03_bmc.h"
#include "test_skills_bmc.h"
#include "artplayer_bmc.h"  /* Artur's test player */
#include "train_skills_bmc.h"
#include "sputplayer_bmc.h" /* Sputnick's test player... */
//#include "train_behaviors_bmc.h"

#include "view/bs03_neck_bmn.h"  /* turn_neck strategy #42 */
#include "view/goalie_neck_bmn.h" /* goalie_dynamic_neck */
#include "view/bs03_view_bmv.h"  /* view strategy #41 */
#include "view/attention_to_bma.h"

inline bool init_behavior(char const* name, BaseBehavior *& behavior, 
			  char const* fname, int argc, char const* const* argv) {

  //INFO_OUT << "\nlooking for behavior= [" << name << "]";

  behavior= 0;
  if ( name == 0 ) {
    return true;
  }

  if ( strcmp(name,"-1") == 0 ) //no behaviour wished
    return true;
  if ( strcmp(name,"Bs03") == 0 ) {
    if ( ! Bs03::init(fname,argc,argv) )
      return false;
    behavior= new Bs03();
    return true;
  }

  /*if ( strcmp(name,"TrainBehaviors") == 0 ) {
    if ( ! TrainBehaviors::init(fname,argc,argv) )
      return false;
    behavior= new TrainBehaviors();
    return true;
  }*/

  if ( strcmp(name,"ArtPlayer") == 0 ) {
    if ( ! ArtPlayer::init(fname,argc,argv) )
      return false;
    behavior= new ArtPlayer();
    return true;
  }

  if ( strcmp(name,"SputPlayer") == 0 ) {
    if ( ! SputPlayer::init(fname,argc,argv) )
      return false;
    behavior= new SputPlayer();
    return true;
  }

  if ( strcmp(name,"TestSkillsPlayer") == 0 ) {
    if ( ! TestSkillsPlayer::init(fname,argc,argv) )
      return false;
    behavior= new TestSkillsPlayer();
    return true;
  }

  if ( strcmp(name,"TrainSkills") == 0 ) {
    if ( ! TrainSkillsPlayer::init(fname,argc,argv) )
      return false;
    behavior= new TrainSkillsPlayer();
    return true;
  }

  ERROR_OUT << "\ncould not find behavior= [" << name << "]";
  return false;
}

inline bool init_neck_behavior(NeckBehavior *& behavior, 
			       char const* fname, int argc, char const* const* argv) {
  //behavior = NULL;  // No neck behavior so far... we need to use the old neck policy for now.
  //return true;
  if(!BS03Neck::init(fname,argc,argv))
    return false;
  behavior = new BS03Neck();
  return true;
}

inline bool init_view_behavior(ViewBehavior *& behavior, 
			       char const* fname, int argc, char const* const* argv) {
  if(!BS03View::init(fname,argc,argv))
    return false;
  behavior = new BS03View();
  return true;
}

inline bool init_attentionto_behavior(AttentionToBehavior *& behavior, 
			       char const* fname, int argc, char const* const* argv) {
  if(!AttentionTo::init(fname,argc,argv))
    return false;
  behavior = new AttentionTo();
  return true;
}

#endif
