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


#include "mdp_memory.h"
#include "mdp_info.h"
#include "options.h"
#include "mdpstate.h"
#include "log_macros.h"

#define TEAM_SIZE 11

MDPmemory::MDPmemory() {
  opponent_last_at_ball_number = 0;
  opponent_last_at_ball_time = 0;
  teammate_last_at_ball_number = 0;
  teammate_last_at_ball_time = 0;
  counter = 0;
}

void MDPmemory::update(){
  Vector ball, player;
  if (WSinfo::is_ball_pos_valid()) {
    //berechnet mit Gedaechtnis wer im Angriff ist (=schnellster Spieler zum Ball)
    int summe = 0;
    momentum[counter] = mdpInfo::fastest_team_to_ball();
    counter = (counter + 1 ) % 5;
    for(int i=0; i<MAX_MOMENTUM;i++){
      summe += momentum[i];
    }
    team_in_attack = summe / MAX_MOMENTUM;

    ball = WSinfo::ball->pos;
    for (int i = 1 ; i <= TEAM_SIZE ; i++) {
      if (mdpInfo::is_teammate_pos_valid(i)){
	player =  mdpInfo::teammate_pos_abs(i);
	if (fabs((player - ball).norm()) < ServerOptions::kickable_area) {
	  teammate_last_at_ball_number = i;
	  teammate_last_at_ball_time = mdpInfo::mdp->time_current;
	}
      }
      if (mdpInfo::is_opponent_pos_valid(i)) {
	player =  mdpInfo::opponent_pos_abs(i);
	if (fabs((player - ball).norm()) < ServerOptions::kickable_area) {
	  opponent_last_at_ball_number = i;
	  opponent_last_at_ball_time = mdpInfo::mdp->time_current;
	}
      }
    }
    if(teammate_last_at_ball_time >= opponent_last_at_ball_time)
      team_last_at_ball = 0;
    else
      team_last_at_ball = 1;
  }   
  mdpInfo::update_memory(*this);
}

  
