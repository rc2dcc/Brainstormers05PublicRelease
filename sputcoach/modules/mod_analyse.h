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
 * This module analyses the opponent team and tries to find out the opponent
 * player types.
 *
 */

#ifndef _MOD_ANALYSE_H_
#define _MOD_ANALYSE_H_

#include "defs.h"
#include "coach.h"
#include "param.h"
#include "field.h"
#include "options.h"
#include "modules.h"
#include "messages.h"

class ModAnalyse : public AbstractModule {
 private:
  Field oldfld;
  int oldpm;

  bool changeAnnounced;
  bool goalieAnnounced;
  
  Value maxKickRange;
  Value kickRanges[MAX_PLAYER_TYPES];
  bool successMentioned[TEAM_SIZE*2];
  bool onlyOneLeft[TEAM_SIZE*2];
  int possTypes[TEAM_SIZE*2][MAX_PLAYER_TYPES];
  int pltypes;
  bool ballKicked;
  int kickingPlayer;
  bool canKick[TEAM_SIZE*2];
  bool mayCollide[TEAM_SIZE*2];

  Player *getPlByIndex(int i,Field *field);

  void checkCollisions();
  bool checkKick();
  bool checkPlayerDecay();
  bool checkInertiaMoment();
  bool checkPlayerSpeedMax();
  
 public:

  bool init(int argc,char **argv);             /** init the module */
  bool destroy();                              /** tidy up         */
  
  bool behave();                               /** called once per cycle, after visual update   */
  bool onRefereeMessage(bool PMChange);        /** called on playmode change or referee message */
  bool onHearMessage(const char *str);         /** called on every hear message from any player */
  bool onKeyboardInput(const char *str);       /** called on keyboard input                     */
  bool onChangePlayerType(bool,int,int);       /** SEE mod_template.c!                          */
  
  static const char modName[];                 /** module name, should be same as class name    */
  const char *getModName() {return modName;}   /** do not change this!                          */
};

#endif
