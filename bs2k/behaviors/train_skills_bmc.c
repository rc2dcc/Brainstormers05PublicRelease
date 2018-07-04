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

#include "train_skills_bmc.h"

#include "ws_info.h"
#include "log_macros.h"
#include "math.h"

#include "../artagent/wmoptions.h"

#define TOLERANCE 0.3
#define TRAINING_RANGE 20
#define MAX_CYCLES_PER_TRIAL 10

bool TrainSkillsPlayer::initialized=false;
Vector TrainSkillsPlayer::target;
int TrainSkillsPlayer::num_cycles_in_trial;

TrainSkillsPlayer::TrainSkillsPlayer() { 
  go2pos= new NeuroGo2Pos; 
  neurointercept= new NeuroIntercept;     
  neurokick = new NeuroKick;
}


bool TrainSkillsPlayer::playon(Cmd & cmd) {
  num_cycles_in_trial++;

#if 0  // train neurogo2pos
  LOG_POL(0, << _2D << C2D(target.x, target.y, 1, "ff0000") );
  go2pos->set_target(target);
  if((go2pos->get_cmd(cmd) == false) || //no command was set -> target reached!
     (num_cycles_in_trial > MAX_CYCLES_PER_TRIAL)){
    num_cycles_in_trial = 0;
    do{
      target = WSinfo::me->pos + Vector((drand48() -.5)*2.0 * TRAINING_RANGE, 
					(drand48() -.5)*2.0 * TRAINING_RANGE);
    } while (WSinfo::me->pos.sqr_distance(target) < SQUARE(1.0));
  }       
  return true;
#endif

  if(!WSinfo::is_ball_kickable()) {
    if(neurointercept->get_cmd(cmd) == false){// at ball ?
    }
  } 
  else {
    Vector target;
    do{
      target = WSinfo::me->pos + Vector((drand48() -.5)*2.0 * TRAINING_RANGE, 
					(drand48() -.5)*2.0 * TRAINING_RANGE);
    } while (target.sqr_distance(Vector(0,0)) > SQUARE(30.0));
    neurokick->kick_to_pos_with_final_vel(0,target);
    //    neurokick->get_cmd(cmd);
    cmd.cmd_main.set_turn(0.);
  }

  return true;  
}


bool TrainSkillsPlayer::get_cmd(Cmd & cmd) {
  if(WMoptions::offline){ // no server connected -> go directly to training  
    neurointercept->get_cmd(cmd);
    return true;
  }

  switch(WSinfo::ws->play_mode){
  case PM_PlayOn:
    return playon(cmd);
    break;
  default:
#if 1
    if(WSinfo::me->pos.sqr_distance(Vector(-5.,0)) > SQUARE(1.)){
      cmd.cmd_main.set_moveto(-5.,0.);
      return true;
    }
#endif
    return playon(cmd);
    return true;
  }

  LOG_POL(0, << "TrainSkills: now in cycle : " << WSinfo::ws->time);

  //LOG_DEB(0, << _2D << C2D(WSinfo::me->pos.x, WSinfo::me->pos.y, 2, "ff0000") );

  return false;
}
