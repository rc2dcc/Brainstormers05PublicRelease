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
 * See field.h for description.
 *
 */

#include "field.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <strstream>

/*********************************************
 * class Object
 ********************************************/



/********************************************
 * class Player
 *******************************************/

Player::Player(Value px,Value py,Value vx,Value vy,ANGLE bang,ANGLE nang,int nr) {
  pos_x=px;pos_y=py;vel_x=vx;vel_y=vy;
  bodyAng=bang;neckAng=nang;
  alive=false;type=0;
  for(int i=0;i<MAX_PLAYER_TYPES;i++) possibleTypes[i]=0;
}

/********************************************
 * class Field
 *******************************************/

Field::Field() {
  currentTime=0;
  currentPM=0;
  myTeamName[0]=0;hisTeamName[1]=0;
  for(int i=0;i<MAX_SAY_TYPES;i++) {
    sayInfo.last[i]=-1;sayInfo.lastInPlayOn[i]=-1;
    sayInfo.ack[i]=true;
  }  
}

void Field::setTime(long time) {currentTime=time;}

long Field::getTime() {return currentTime;}

void Field::setPM(int PM) {currentPM=PM;}

int Field::getPM() {return currentPM;}

const char *Field::getMyTeamName() {
  return myTeamName;
}

const char *Field::getHisTeamName() {
  return hisTeamName;
}
	      
void Field::visState() {
return;
  static char logbuf[2048];
  std::strstream str(logbuf,2048);
  Vector pos,vel;
  Value bang,nang;

  if(ball.alive) {
    str << C2D(ball.pos_x,ball.pos_y,0.9,"#550055");
    str << L2D(ball.pos_x,ball.pos_y,ball.pos_x+5*ball.vel_x,ball.pos_y+5*ball.vel_y,"#550055");
  }
  
  for(int i=0;i<11;i++) {
    if(myTeam[i].alive) {
      pos=myTeam[i].pos();vel=myTeam[i].vel();
      bang=myTeam[i].bodyAng.get_value();nang=myTeam[i].neckAng.get_value();
      str << C2D(pos.x,pos.y,.7,"#0000ff");
      str << L2D(pos.x,pos.y,pos.x+.7*cos(bang),pos.y+.7*sin(bang),"#0000ff");
      str << L2D(pos.x,pos.y,pos.x+5*vel.x,pos.y+5*vel.y,"#0000ff");
    }
    if(hisTeam[i].alive) {
      pos=hisTeam[i].pos();vel=hisTeam[i].vel();
      bang=hisTeam[i].bodyAng.get_value();nang=hisTeam[i].neckAng.get_value();
      str << C2D(pos.x,pos.y,.7,"#ff0000");
      str << L2D(pos.x,pos.y,pos.x+.7*cos(bang),pos.y+.7*sin(bang),"#ff0000");
      str << L2D(pos.x,pos.y,pos.x+5*vel.x,pos.y+5*vel.y,"#ff0000");
    }
    if(str.pcount()>1800) {
      str << '\0';
      LOG_VIS(0,<< _2D << logbuf);
      str.seekp(0);
    }
  }

  str << '\0';
  LOG_VIS(0,<< _2D << logbuf);
}
