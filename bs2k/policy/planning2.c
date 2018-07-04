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

#include "planning2.h"
#include "planning.h"
#include "log_macros.h"
#include "options.h"
#include "valueparser.h"
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

// ridi: added Debug Logs which are deactivated for competition
#if 0
#define DBLOG_POL(LLL,XXX) LOG_POL(LLL,"Planning2:"<<XXX)
#define DBLOG_ERR(LLL,XXX) LOG_ERR(LLL,XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D <<XXX)
#else
#define DBLOG_POL(LLL,XXX)
#define DBLOG_ERR(LLL,XXX) 
#define DBLOG_DRAW(LLL,XXX)
#endif

//#define MIN_POSITION_NET -20
#define BASELEVEL 0
#define MAX_PASSES 100



int Planning2::generate_pactions(const AState &state, AAction *actions){
  int num_actions = 0;
  AAction candidate_action; 
  Vector test_pass[30];

  for(int recv_idx=0;recv_idx<11;recv_idx++){
    /* passes to other players */
    if(state.my_team[state.my_idx].number == WSinfo::ws->my_goalie_number )
      break; // the goalie can not play passes
    if(state.my_team[recv_idx].valid == false)
      continue;
    if(state.my_idx == recv_idx)
      continue;
    if(state.my_team[recv_idx].number == WSinfo::ws->my_goalie_number)
      continue;
    int number_passpos = 0;
    test_pass[number_passpos++] = Pass_Direct;

    for(int i=0;i<number_passpos;i++){
      if(Planning::check_action_pass(state,candidate_action,recv_idx, test_pass[i])){
	actions[num_actions++] = candidate_action; // insert action
	break;  // stop testing  
      }
    } // check all potential pass positions
    if(num_actions > MAX_PASSES)
      return num_actions;
  } // for all receivers
  return num_actions;
}



Vector Planning2::compute_potential_pos(const AState & state){   // compute potential of that state
  AAction solution_path[1];
  AAction actions[MAX_PASSES];

  int pactions_size = generate_pactions(state, actions);
  Value best_evaluation = -1000;
  int best_idx=-1;

  DBLOG_POL(BASELEVEL+1,"In evaluate(state): paction size = "<<pactions_size);

  for(int a=0; a<pactions_size;a++){
    // these are all available passes to players
    /*
    Value evaluation = Tools::evaluate_pos(actions[a].actual_resulting_position);
    DBLOG_DRAW(BASELEVEL+1,C2D( actions[a].actual_resulting_position.x, 
				  actions[a].actual_resulting_position.y,0.5,"magenta"));
    DBLOG_POL(1," After pass: resulting pos "<< actions[a].actual_resulting_position  
	      <<"evaluation: "<<evaluation<<" best eval "<<best_evaluation);
    if(evaluation > best_evaluation){
      best_evaluation = evaluation;
      best_idx = a;
    }
    */
    if(best_idx <0)
      best_idx = a;
    else if(Tools::is_pos1_better(actions[a].actual_resulting_position,actions[best_idx].actual_resulting_position))
      best_idx = a;
  }
  
  if(best_idx>=0){
    DBLOG_DRAW(BASELEVEL+1,C2D( actions[best_idx].actual_resulting_position.x, 
				  actions[best_idx].actual_resulting_position.y,0.8,"magenta"));
    DBLOG_DRAW(BASELEVEL, L2D(state.ball.pos.x,state.ball.pos.y,actions[best_idx].actual_resulting_position.x, 
			      actions[best_idx].actual_resulting_position.y,"magenta"));

    return actions[best_idx].actual_resulting_position;
  }
  
  
  return state.ball.pos;
}
