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

/* Author: Manuel "Sputnick" Nickschas, 11/2001
 *
 * The class Field and the other classes declared in this file
 * hold all data describing the field for the coach.
 * (e.g. the position of objects on the field, current playmode
 * and similar stuff).
 *
 */

#ifndef _FIELD_H_
#define _FIELD_H_

#include "defs.h"
#include "param.h"
#include "angle.h"
#include "messages.h"

class Object {
 public:
  bool alive;
  Value pos_x,pos_y,vel_x,vel_y;
  
  Vector pos() const { return Vector(pos_x,pos_y);};
  Vector vel() const { return Vector(vel_x,vel_y);};
  void setPos(Value px,Value py) { pos_x=px;pos_y=py;};
  void setVel(Value vx,Value vy) { vel_x=vx;vel_y=vy;};

 protected:
  Object() {alive=false;}
};

class Player : public Object {
 public:
  ANGLE bodyAng,neckAng;
  bool goalie;
  int number,type;
  int possibleTypes[MAX_PLAYER_TYPES];

  Player(Value px=0,Value py=0,Value vx=0,Value vy=0,
	 ANGLE bodyAng=ANGLE(0),ANGLE neckAng=ANGLE(0),int number=0);
};
  
class Ball : public Object {

};

class Field {
 private:
  int currentTime;
  int currentPM;
  char myTeamName[40];
  char hisTeamName[40];
  friend class MsgTeamNames;
 public:
  PlayerType plType[MAX_PLAYER_TYPES];
  Player myTeam[TEAM_SIZE],hisTeam[TEAM_SIZE];
  
  Ball ball;
  
  Field();

  struct {
    int last[MAX_SAY_TYPES];
    int lastInPlayOn[MAX_SAY_TYPES];
    bool ack[MAX_SAY_TYPES];
  } sayInfo;
  
  long getTime();
  void setTime(long time);
  int getPM();
  void setPM(int PM);
  const char *getMyTeamName();
  const char *getHisTeamName();

  void visState();
};







#endif
