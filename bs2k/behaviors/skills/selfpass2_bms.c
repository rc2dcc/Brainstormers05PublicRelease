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

#include "selfpass2_bms.h"
#include "ws_memory.h"
#include "tools.h"

bool Selfpass2::initialized= false;

#if 1
#define DBLOG_MOV(LLL,XXX) LOG_POL(LLL,<<"Selfpass2: "<<XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D<<XXX)
//#define DBLOG_DRAW(LLL,XXX)
#else
#define DBLOG_MOV(LLL,XXX)
#define DBLOG_DRAW(LLL,XXX)
#endif


#define BODYDIR_TOLERANCE 5.0   // allowed tolerance before turning into target direction
#define MIN_DIST_2_BORDER 2.  // keep away from border
#define SAFETY_MARGIN 0.2  // want to have the ball safe!


/*************************************************************************/

/* Initialization */

bool Selfpass2::init(char const * conf_file, int argc, char const* const* argv) {
  if(initialized) return true; // only initialize once...
  initialized= true;
  cout<<"\nSelfpass2 behavior initialized.";
  return (BasicCmd::init(conf_file, argc, argv)
	  && OneOrTwoStepKick::init(conf_file,argc,argv)
	  );
}

Selfpass2::Selfpass2(){
  basic_cmd = new BasicCmd;
  onetwostepkick = new OneOrTwoStepKick;
}

Selfpass2::~Selfpass2() {
  delete basic_cmd;
  delete onetwostepkick;
}

bool Selfpass2::get_cmd(Cmd & cmd){

  DBLOG_MOV(0,"get_cmd: No precomputation available! Call is_safe(targetdir) first!!!");
  return false;
}

/*
bool Selfpass2::get_cmd(Cmd & cmd){

  if(simulation_table[0].valid_at != WSinfo::ws->time){
    DBLOG_MOV(0,"get_cmd: No precomputation available! Call is_safe(targetdir) first!!!");
    return false;
  }
  cmd = simulation_table[0].cmd;
  return true;
}
*/

bool Selfpass2::get_cmd(Cmd & cmd, const ANGLE targetdir, const Vector targetpos, const Value kickspeed){

  DBLOG_MOV(0,"get_cmd: targetdir: "<<RAD2DEG(targetdir.get_value())<<" targetpos "<<targetpos
	    <<" kickspeed "<<kickspeed);

	    
  if(kickspeed >0){ // want to kick -> compute appropriate onestep kick
    Value speed1, speed2;
    onetwostepkick->reset_state(); // use current ws-state
    onetwostepkick->kick_to_pos_with_initial_vel(kickspeed,targetpos);
    onetwostepkick->get_vel(speed1,speed2);
    if(fabs(speed1-kickspeed)<=0.1){
      // everything's fine!
      onetwostepkick->get_cmd(cmd);
      return true;
    }
    DBLOG_MOV(0,"get_cmd: PROBLEM. Should kick, but kick does not work. CALL ridi!!!");
    return false;
  }

  // have to turn or dash to targetpos
  get_cmd_to_go2dir(cmd, targetdir, WSinfo::me->pos, WSinfo::me->vel,WSinfo::me->ang,(int)( WSinfo::me->stamina), WSinfo::me->inertia_moment, WSinfo::me->stamina_inc_max);
  return true;
}



bool Selfpass2::is_selfpass_safe(const ANGLE targetdir){

  Vector ipos,op;
  Value speed;
  int steps, op_num;

  return is_selfpass_safe(targetdir, speed,ipos, steps, op, op_num);
}

bool Selfpass2::is_selfpass_still_safe(const ANGLE targetdir, Value & kickspeed, int &op_num){
  // used to recheck previous intention. checks nokickc only!

  Vector ipos,op;
  int steps;

  return is_selfpass_safe(targetdir, kickspeed,ipos, steps, op, op_num, true);
}

bool Selfpass2::is_turn2dir_safe(const ANGLE targetdir, Value &kickspeed, Vector &targetpos, int &actual_steps,
				 Vector &attacking_op, int & op_number, 
				 const bool check_nokick_only, const int max_dashes){
  return is_turn2dir_safe(targetdir, kickspeed, targetpos, actual_steps, attacking_op, op_number,
			  WSinfo::me->pos, WSinfo::me->vel, WSinfo::me->ang, WSinfo::me->stamina,
			  WSinfo::ball->pos, WSinfo::ball->vel,
			  check_nokick_only, max_dashes);
}

bool Selfpass2::is_turn2dir_safe(const ANGLE targetdir, Value &kickspeed, Vector &targetpos, int &actual_steps,
				 Vector &attacking_op, int & op_number, 
				 const Vector mypos, const Vector myvel, const ANGLE myang,
				 const Value mystamina,
				 const Vector ballpos, const Vector ballvel,
				 const bool check_nokick_only, const int max_dashes){

  //  int max_steps = 15;  // default
  int max_steps = MAX_STEPS;  // default

  max_steps = 5;  // rough estimation: 1 kick, 2 turns, then dash

  bool with_kick = false;
  bool target_is_ball = true;
  bool stop_if_turned = true;
  kickspeed = 0.0;

  if((targetdir.diff(myang) <=BODYDIR_TOLERANCE/180.*PI)){
    DBLOG_MOV(0,"Hey, I already turned in that direction! targetdir "<<RAD2DEG(targetdir.get_value()));
    max_steps = 2; // try a dash
    stop_if_turned = 0; // don't stop if direction ok -> this is already true
  }

  
  //first, check, without kick movement
  static const float minDistToBall = 1.25*(ServerOptions::player_size + ServerOptions::ball_size);
  simulate_my_movement(targetdir,max_steps,simulation_table,mypos, myvel, myang, mystamina, ballpos, ballvel,
		       with_kick, stop_if_turned); 
  bool tooCloseToBall = simulation_table[1].my_pos.distance(simulation_table[1].ball_pos)<minDistToBall;
  if(!tooCloseToBall){
    simulate_ops_movement(simulation_table, target_is_ball);
    print_table(simulation_table);
    if(check_nokick_selfpass(simulation_table, targetpos, actual_steps, attacking_op, op_number,ballpos) == true){
      if((targetdir.diff(simulation_table[actual_steps].my_bodydir) <=BODYDIR_TOLERANCE/180.*PI)){
	DBLOG_MOV(0,"Hey, I can turn2dir safely without kick. targetdir "<<RAD2DEG(targetdir.get_value()));
	return true;
      }
    }
  } // not too Close to Ball

  // now check with kick
  with_kick = true;
  target_is_ball = false;
  simulate_my_movement(targetdir,max_steps,simulation_table, mypos, myvel, myang, mystamina, ballpos, ballvel,
		       with_kick, stop_if_turned); 
  simulate_ops_movement(simulation_table, target_is_ball);

  print_table(simulation_table);


  if(  determine_kick(simulation_table, targetdir, targetpos, kickspeed, actual_steps, attacking_op, op_number,
		      mypos, myvel, myang, mystamina, ballpos, ballvel) == true){
    if(targetdir.diff(simulation_table[actual_steps].my_bodydir) <=BODYDIR_TOLERANCE/180.*PI){ 
      // at final position, I do look into target direction 
      DBLOG_MOV(0,"Turn 2 Dir Is Safe: Hey, I found a correct kick command to dir "<<RAD2DEG(targetdir.get_value())
		<<" mypos "<<mypos<<" targetpos "<<targetpos<<" kickspeed "<<kickspeed);
      return true;
    }
    else{ // turn not possible
      DBLOG_MOV(0,"Turn 2 dir: I found a correct kick command, but turning not possible in time");
      return false;
    }
  }
  else{ // no kick found
    DBLOG_MOV(0,"Turn2dir Is Safe: Couldn't find an appropriate one step kick to dir "<<RAD2DEG(targetdir.get_value()));
    return false;
  }
  return false;
}


bool Selfpass2::is_selfpass_safe(const ANGLE targetdir, Value &kickspeed, Vector &targetpos, int &actual_steps,
				 Vector &attacking_op, int & op_number, 
				 const bool check_nokick_only, const int reduce_dashes){
  return is_selfpass_safe(targetdir, kickspeed, targetpos, actual_steps, attacking_op, op_number,
			  WSinfo::me->pos, WSinfo::me->vel, WSinfo::me->ang, WSinfo::me->stamina,
			  WSinfo::ball->pos, WSinfo::ball->vel, check_nokick_only, reduce_dashes);

}



bool Selfpass2::is_selfpass_safe(const ANGLE targetdir, Value &kickspeed, Vector &targetpos, int &actual_steps,
				 Vector &attacking_op, int & op_number, 
				 const Vector mypos, const Vector myvel, const ANGLE myang,
				 const Value mystamina,
				 const Vector ballpos, const Vector ballvel,
				 const bool check_nokick_only, const int reduce_dashes){

  //  int max_steps = 15;  // default
  int max_steps = MAX_STEPS;  // default

  bool with_kick = false;
  bool target_is_ball = true;
  kickspeed = 0.0;

  static const float minDistToBall = 1.25*(ServerOptions::player_size + ServerOptions::ball_size);

  //first, check, without kick movement
  simulate_my_movement(targetdir,max_steps,simulation_table,mypos, myvel, myang, mystamina, 
		       ballpos, ballvel, with_kick); 
	
  bool tooCloseToBall = simulation_table[1].my_pos.distance(simulation_table[1].ball_pos)<minDistToBall;

  if(!tooCloseToBall){
    simulate_ops_movement(simulation_table, target_is_ball);
    print_table(simulation_table);
    
    if(check_nokick_selfpass(simulation_table, targetpos, actual_steps,  attacking_op, op_number, ballpos) == true){
      if((targetdir.diff(simulation_table[actual_steps].my_bodydir) 
	  <=BODYDIR_TOLERANCE/180.*PI)){
	// I am looking to the right direction
	if (mypos.distance(targetpos) > 3.0){
	  // I can advance very nicely
	  DBLOG_MOV(0,"Hey, I can considerably move without kick in dir "<<RAD2DEG(targetdir.get_value()));
	  return true;
	}
	if(check_nokick_only == true){
	  DBLOG_MOV(0,"Hey, I only test withoutkick, and at least can turn 2 dir "<<RAD2DEG(targetdir.get_value()));
	  return true;
	}
      } // after all, I am looking in targetdir
    } // check nokick selfpass is true
  } // not too close too ball


  if(check_nokick_only == true){
    DBLOG_MOV(0,"Hey, Check with nokick only not successful, return false");
    // hmm, withoutkick seems not to be a success!!!
    return false;
  }

  DBLOG_MOV(0,"Is Safe: not possible without kick in dir "<<RAD2DEG(targetdir.get_value()));

  // now check with kick
  with_kick = true;
  target_is_ball = false;
  simulate_my_movement(targetdir,max_steps,simulation_table,mypos, myvel, myang, mystamina, ballpos, ballvel, with_kick); 
  simulate_ops_movement(simulation_table, target_is_ball);

  print_table(simulation_table);


  if(  determine_kick(simulation_table, targetdir, targetpos, kickspeed, actual_steps, attacking_op, op_number, 
		      mypos, myvel, myang, mystamina, ballpos, ballvel,reduce_dashes) == true){
    if(targetdir.diff(simulation_table[actual_steps].my_bodydir) <=BODYDIR_TOLERANCE/180.*PI){ 
      // at final position, I do look into target direction 
      DBLOG_MOV(0,"Is Safe: Hey, I found a correct kick command to dir "<<RAD2DEG(targetdir.get_value())
		<<" targetpos "<<targetpos<<" kickspeed "<<kickspeed);
      return true;
    }
    else{ // turn not possible
      DBLOG_MOV(0,"I found a correct kick command, but turning not possible in time");
      return false;
    }
  }
  else{ // no kick found
    DBLOG_MOV(0,"Is Safe: Couldn't find an appropriate one step kick to dir "<<RAD2DEG(targetdir.get_value()));
    return false;
  }
  return false;
}

void Selfpass2::get_cmd_to_go2pos(Cmd &tmp_cmd,const Vector targetpos,const Vector pos, const Vector vel, 
				  const ANGLE bodydir, const int stamina,
				  const PPlayer player){

  if(pos.distance(targetpos) < 2*ServerOptions::player_speed_max){
    // potentially could reach the targetpos by simply dashing; so try this first!
    Vector next_pos, next_vel;
    ANGLE next_bodydir;
    int next_stamina;

    tmp_cmd.cmd_main.unset_lock();
    tmp_cmd.cmd_main.unset_cmd();
    basic_cmd->set_dash(100);
    basic_cmd->get_cmd(tmp_cmd);	
    Tools::simulate_player(pos,vel,bodydir,stamina,
			   tmp_cmd.cmd_main, 
			   next_pos,next_vel,next_bodydir,next_stamina,
			   player->stamina_inc_max, player->inertia_moment,
			   player->dash_power_rate,
			   player->effort, player->decay);
    if(at_position(next_pos, next_bodydir, player->kick_radius, targetpos)){
      // hey, a dash will do this!!
      return;
    }
    tmp_cmd.cmd_main.unset_lock();
    tmp_cmd.cmd_main.unset_cmd();
  }

  // normal computation
  ANGLE targetdir;
  targetdir = (targetpos - pos).ARG();
  get_cmd_to_go2dir(tmp_cmd, targetdir, pos,vel,bodydir, stamina, player->inertia_moment, 
		    player->stamina_inc_max);
}


void Selfpass2::get_cmd_to_go2dir(Cmd &tmp_cmd,const ANGLE targetdir,const Vector pos, const Vector vel, 
				  const ANGLE bodydir, const int stamina,
				  const Value inertia_moment, const Value stamina_inc_max){
  // compute right command to dash in targetdir
  Value moment;

  if(targetdir.diff(bodydir) >BODYDIR_TOLERANCE/180.*PI){ // have to turn
    Value moment = (targetdir-bodydir).get_value_mPI_pPI() * 
      (1.0 + (inertia_moment * (vel.norm())));
    if (moment > 3.14) moment = 3.14;
    if (moment < -3.14) moment = -3.14;
    basic_cmd->set_turn(moment);
    basic_cmd->get_cmd(tmp_cmd);
    return;
  }
  
  // turned, now dashing
  int dash_power = 100;
  if(stamina <= ServerOptions::recover_dec_thr*ServerOptions::stamina_max + 100.){
    dash_power = (int)stamina_inc_max;
  }
  basic_cmd->set_dash(dash_power);
  basic_cmd->get_cmd(tmp_cmd);	
  return;
}

void Selfpass2::reset_simulation_table(Simtable *simulation_table){
  for(int i=0; i<MAX_STEPS; i++){
    simulation_table[i].valid_at = -1; // invalidate
  }
}


void Selfpass2::simulate_my_movement(const ANGLE targetdir, const int max_steps, Simtable *simulation_table, 
				 const Vector mypos, const Vector myvel, const ANGLE myang,
				 const Value mystamina,
				 const Vector ballpos, const Vector ballvel,
				     const bool with_kick, const bool turn2dir_only){
  // creates a table with possible positions, when first cmd is a kick
  Cmd tmp_cmd;

  Vector tmp_pos = mypos;
  Vector tmp_vel = myvel;
  Vector tmp_ballvel = ballvel;  
  Vector tmp_ballpos = ballpos;  
  Vector next_pos, next_vel, next_ballpos, next_ballvel;
  ANGLE tmp_bodydir = myang;
  Angle next_bodydir;
  int tmp_stamina = (int) mystamina;
  int next_stamina;

  reset_simulation_table(simulation_table);

  for(int i=0; i<=max_steps && i<MAX_STEPS; i++){
    tmp_cmd.cmd_main.unset_lock();
    tmp_cmd.cmd_main.unset_cmd();
    if(i==0 && with_kick == true){
      // do nothing here; simulate a kick command, which is currently not known.
      basic_cmd->set_turn(0);
      basic_cmd->get_cmd(tmp_cmd); // copy it to tmp_cmd;
    }
    else 
      get_cmd_to_go2dir(tmp_cmd, targetdir,tmp_pos,tmp_vel,tmp_bodydir, tmp_stamina, WSinfo::me->inertia_moment, WSinfo::me->stamina_inc_max);

    // write to table:
    simulation_table[i].valid_at = WSinfo::ws->time; // currently valid
    simulation_table[i].my_pos = tmp_pos;
    simulation_table[i].my_vel = tmp_vel;
    simulation_table[i].my_bodydir = tmp_bodydir;
    simulation_table[i].ball_pos = tmp_ballpos;
    simulation_table[i].ball_vel = tmp_ballvel;
    simulation_table[i].cmd = tmp_cmd;
    if(tmp_pos.distance(tmp_ballpos) <= WSinfo::me->kick_radius - SAFETY_MARGIN)
      simulation_table[i].I_have_ball = true;
    else
      simulation_table[i].I_have_ball = false;
    if(turn2dir_only == true){
      if((targetdir.diff(simulation_table[i].my_bodydir) <=BODYDIR_TOLERANCE/180.*PI)){
	// only do steps, until direction is ok.
	return;
      }
    }

    Tools::model_cmd_main(tmp_pos,tmp_vel,tmp_bodydir.get_value(),tmp_stamina, tmp_ballpos, tmp_ballvel,
			  tmp_cmd.cmd_main, 
			  next_pos,next_vel,next_bodydir,next_stamina, next_ballpos, next_ballvel);
      
    tmp_pos = next_pos;
    tmp_vel = next_vel;
    tmp_ballpos = next_ballpos;
    tmp_ballvel = next_ballvel;
    tmp_bodydir = ANGLE(next_bodydir);
    tmp_stamina = next_stamina;
  };  // while
}

bool Selfpass2::at_position(const Vector playerpos, const ANGLE bodydir, const Value kick_radius, const Vector targetpos){
   if(playerpos.distance(targetpos) <=  kick_radius) // at position!
     return true;
   Vector translation, new_center;
   translation.init_polar(0.5, bodydir);
   new_center = playerpos + translation;
   if(new_center.distance(targetpos) <= kick_radius) // at position!
     return true;
   return false;
}


int Selfpass2::get_min_cycles2_pos(const Vector targetpos, const PPlayer player, const int max_steps, Vector &resulting_pos){

  Cmd tmp_cmd;
  Vector tmp_pos = player->pos;
  Vector tmp_vel = player->vel;
  Vector next_pos, next_vel;
  ANGLE tmp_bodydir = player->ang;
  ANGLE next_bodydir;
  int tmp_stamina = (int) player->stamina;
  int next_stamina;

  // ridi: make it more safe:
  if (player->age_ang >0){
    tmp_bodydir = (targetpos - tmp_pos).ARG();
  }
  // probably too cautious...
  /*
  if (player->age_vel >0){
    Vector vel;
    vel.init_polar(ServerOptions::player_speed_max, (targetpos - tmp_pos).ARG());
    tmp_vel = vel;
  }
  */
  resulting_pos = tmp_pos;

  int i;
  if(player->number == 0){
    DBLOG_MOV(0,"checking op player "<<player->number<<" kick radius "<<player->kick_radius
	      <<" inertia_moment "<<player->inertia_moment<<" power rate "<<player->dash_power_rate
	      <<" effort "<<player->effort<<" decay "<<player->decay);
  }

  if(tmp_pos.distance(targetpos) > (max_steps+2) * 1.25 * ServerOptions::player_speed_max + player->kick_radius){
    // no chance to get to position in time
    //    DBLOG_DRAW(0,C2D(tmp_pos.x,tmp_pos.y,1.3,"black"));          
    return -1;
  }

  for(i=0; i<=max_steps && i<=MAX_STEPS; i++){
    // test only: remove or deactivate
    /*
    char infoString[10];
    sprintf(infoString,"%d",max_steps); // this denotes the trajecetory number
    DBLOG_DRAW(0,STRING2D(tmp_pos.x-0.2,tmp_pos.y-0.4,infoString,"ffff00"));
    DBLOG_DRAW(0,C2D(next_pos.x,next_pos.y,player->kick_radius,"ffff00"));      
    */
    // test only: end

    resulting_pos = tmp_pos;
    Value radius = player->kick_radius;
    if(player == WSinfo::his_goalie)
      radius = ServerOptions::catchable_area_l;

    if(at_position(tmp_pos, tmp_bodydir, radius ,targetpos) == true){
      // hey, I am already at position
      return i;
    }
    tmp_cmd.cmd_main.unset_lock();
    tmp_cmd.cmd_main.unset_cmd();

    get_cmd_to_go2pos(tmp_cmd, targetpos,tmp_pos,tmp_vel,tmp_bodydir, tmp_stamina, player);

    Tools::simulate_player(tmp_pos,tmp_vel,tmp_bodydir,tmp_stamina,
			   tmp_cmd.cmd_main, 
			   next_pos,next_vel,next_bodydir,next_stamina,
			   player->stamina_inc_max, player->inertia_moment,
			   player->dash_power_rate,
			   player->effort, player->decay);

    tmp_pos = next_pos;
    tmp_vel = next_vel;
    tmp_bodydir = ANGLE(next_bodydir);
    tmp_stamina = next_stamina;
  };  // while
  return -1;
}

void Selfpass2::simulate_ops_movement(Simtable *simulation_table, const bool target_is_ball){
  // for all steps, for all ops: check time to playerpos
  
  // check opponents;
  WSpset pset = WSinfo::valid_opponents; // enough, if I do it once.
  Vector closest_op;
  Value closest_dist = 1000;

  for(int t=0; t<MAX_STEPS; t++){
    if(simulation_table[t].valid_at != WSinfo::ws->time){
      // entry not valid
      break;
    }
    Vector targetpos = simulation_table[t].my_pos;
    if(target_is_ball == true){
      targetpos = simulation_table[t].ball_pos;
    }
    int best_op_steps = 1000;
    Vector best_op_resulting_pos;
    int closest_op_num;
    int best_idx = -1;

    for (int idx = 0; idx < pset.num; idx++) {
      int max_steps = t; // check for t steps only
      Vector resulting_pos;
      int op_steps = get_min_cycles2_pos(targetpos, pset[idx], max_steps, resulting_pos);

      if(op_steps >=0 && op_steps < best_op_steps){ // I found an opponent that gets to position in time
	best_op_steps = op_steps;
	best_op_resulting_pos = resulting_pos;
	best_idx = idx;
	closest_dist = 0;
      }
      if(targetpos.distance(resulting_pos) < closest_dist){
	closest_dist = targetpos.distance(resulting_pos);
	closest_op = pset[idx]->pos;
	closest_op_num = pset[idx]->number;
      }

    } // for all ops idx


    if(best_idx >=0){ // found an intercepting opponent
      simulation_table[t].op_pos = pset[best_idx]->pos;;
      simulation_table[t].op_steps2pos = best_op_steps;
      simulation_table[t].op_num = pset[best_idx]->number;
    }
    else{
      simulation_table[t].op_pos = closest_op;
      simulation_table[t].op_num = closest_op_num;
      simulation_table[t].op_steps2pos = -1;
    }
  }// for all t
}


void Selfpass2::print_table(Simtable *simulation_table){
  for(int t=0; t<MAX_STEPS; t++){
    if(simulation_table[t].valid_at != WSinfo::ws->time){
      // entry not valid
      break;
    }
    DBLOG_MOV(1,"time "<<t<<" mypos: "<<simulation_table[t].my_pos
	      <<" closest op: "<<simulation_table[t].op_pos
	      <<" number "<<simulation_table[t].op_num
	      <<" gets me in "<<simulation_table[t].op_steps2pos<<" steps"<<" have ball: "<<simulation_table[t].I_have_ball);

    // oppos
    char infoString[10];
    sprintf(infoString,"%d",simulation_table[t].op_steps2pos);
    /*
    if(simulation_table[t].op_steps2pos >0){
      DBLOG_DRAW(0,STRING2D(simulation_table[t].op_pos.x-0.2,simulation_table[t].op_pos.y-0.4,infoString,"ff0000")
		 <<C2D(simulation_table[t].op_pos.x,simulation_table[t].op_pos.y,1.1,"ff0000"));
    }
    */

    // mypos
    /*
    sprintf(infoString,"%d",t);

    DBLOG_DRAW(0,C2D(simulation_table[t].my_pos.x,simulation_table[t].my_pos.y,0.1,"0000ff")
	       <<STRING2D(simulation_table[t].my_pos.x-0.2,simulation_table[t].my_pos.y-0.4,infoString,"0000ff"));
    */

  }

  Value turn_angle = 0;
  Value dash_power = 0;
  Value kick_power = 0;
  Value kick_angle = 0;

  switch(simulation_table[0].cmd.cmd_main.get_type()){
  case Cmd_Main::TYPE_DASH:
    simulation_table[0].cmd.cmd_main.get_dash(dash_power);
    DBLOG_MOV(1,"DASH "<<dash_power);
    break;
  case Cmd_Main::TYPE_KICK:
    simulation_table[0].cmd.cmd_main.get_kick(kick_power, kick_angle);
    DBLOG_MOV(1,"KICK power "<<kick_power<<" kick dir "<<RAD2DEG(kick_angle));
  break;
  case Cmd_Main::TYPE_TURN:
    simulation_table[0].cmd.cmd_main.get_turn(turn_angle);
    DBLOG_MOV(1,"TURN "<<turn_angle);
    break;
  }
}


bool Selfpass2::determine_kick(Simtable *simulation_table, const ANGLE targetdir,
			       Vector & targetpos, Value & targetspeed, int & steps, Vector & attacking_op, 
			       int & attacking_num, 
			       const Vector mypos, const Vector myvel, const ANGLE myang,
			       const Value mystamina,
			       const Vector ballpos, const Vector ballvel,
			       const int reduce_dashes ){
  int best_t = -1;

  Value current_dist2border = Tools::min_distance_to_border(mypos);

  for(int t=1; t<MAX_STEPS; t++){  // start with t=1, since I know already that I have the ball at t= 0 (NOW!)
    if(simulation_table[t].valid_at != WSinfo::ws->time){
      // entry not valid
      break;
    }
    if(simulation_table[t].op_steps2pos >= 0 && simulation_table[t].op_steps2pos <= t){ 
      // if opponent gets me at all and he is faster or equally fast at position, stop search.
      // critical point: break or no break:
      // break;  // using break here is the more safe version; probably uses spectacular dribblings
    }
    else{ // opponent does not get to position in time, now check if its inside pitch
      if((Tools::min_distance_to_border(simulation_table[t].my_pos) > MIN_DIST_2_BORDER) ||
	 (current_dist2border < MIN_DIST_2_BORDER && 
	  Tools::min_distance_to_border(simulation_table[t].my_pos) > current_dist2border)){
	// position is either in pitch, or improves my current situation
	best_t = t;
      }
    }
  }
  

  best_t -= reduce_dashes;  // for a safer life!


  if (best_t <0){ // no position is safe
    steps = -1;
    return false;
  } 

  attacking_op = simulation_table[best_t].op_pos;
  attacking_num = simulation_table[best_t].op_num;
  //  DBLOG_MOV(0,"check with kick selfpass: Attackerpos: "<<attacking_op);

  // found a safe position
  Value summed_decay = 0.0;
  Value decay = 1.0;
  for(int t = 0; t<=best_t;t++){ // compute decay; not elegant, but explicit and clear
    summed_decay += decay;
    decay *= ServerOptions::ball_decay;
  };  

  targetpos = simulation_table[best_t].my_pos;
  /*
  Vector a_bit_forward;
  a_bit_forward.init_polar(0.5, targetdir);
  targetpos += a_bit_forward;
  */

  targetspeed = (ballpos - targetpos).norm()/summed_decay;

  //  DBLOG_MOV(0,"Should kick to "<< targetpos<<" kickspeed "<<targetspeed);
  Value speed1, speed2;

  // compute closest opponent to compute (virtual) state for kick command.
  Vector oppos;
  ANGLE opdir;
  int opdirage;
  WSpset pset= WSinfo::valid_opponents;
  pset.keep_and_sort_closest_players_to_point(1, ballpos);
  if ( pset.num ){
    oppos = pset[0]->pos;
    opdir = pset[0]->ang;
    opdirage = pset[0]->age_ang;
  }
  else{
    oppos = Vector(1000,1000); // outside pitch
    opdir = ANGLE(0);
    opdirage = 1000;
  }

  onetwostepkick->reset_state(); // reset state
  onetwostepkick->set_state(mypos,myvel,myang,ballpos,ballvel,oppos,opdir,opdirage);
  onetwostepkick->kick_to_pos_with_initial_vel(targetspeed,targetpos);
  onetwostepkick->get_vel(speed1,speed2);
  if(fabs(speed1-targetspeed)<=0.1){
    // everything's fine!
    onetwostepkick->get_cmd(simulation_table[0].cmd);
    steps = best_t;
    DBLOG_DRAW(0,C2D(targetpos.x,targetpos.y,.3,"orange"));          
    Vector tmp_ballpos =simulation_table[0].ball_pos;
    Vector tmp_ballvel;
    ANGLE kickdir = (targetpos -  tmp_ballpos).ARG();
    tmp_ballvel.init_polar(targetspeed, kickdir);

    if(are_intermediate_ballpositions_safe(tmp_ballpos, tmp_ballvel, steps) == true){
      DBLOG_MOV(0,"check KICK selfpass: SUCCESS: have the ball after "<<best_t<<" cycles, and  ballpos. safe!");
      return true;
    }
    else{
      DBLOG_MOV(0,"check kick selfpass: NO success: have the ball after "<<best_t<<" cycles, but   ballpos. NOT safe!");
      return false;
    }
  }
  //DBLOG_MOV(0,"determine correct kick: No one-step kick found! ");
  steps = best_t;
  return false;
}


bool Selfpass2::check_nokick_selfpass(Simtable *simulation_table, Vector & targetpos, int & steps, Vector &attacking_op, int & attacking_num, const Vector ballpos){
  int best_t =  -1;

  Value current_dist2border = Tools::min_distance_to_border(ballpos);

  int max_steps = 0;
  for(int t= MAX_STEPS-1; t>0; t--){  // start with t=1, since I know already that I have the ball at t= 0 (NOW!)
    if(simulation_table[t].I_have_ball== true){
      max_steps = t;
      break;
    }
  }

  //  DBLOG_MOV(0,"check nokick selfpass: Number of steps I have the ball without kicking: "<<max_steps);
    
  for(int t=1; t<=max_steps; t++){  // start with t=1, since I know already that I have the ball at t= 0 (NOW!)
    if(simulation_table[t].valid_at != WSinfo::ws->time){
      // entry not valid
      break;
    }
    if(simulation_table[t].op_steps2pos >= 0 && simulation_table[t].op_steps2pos <= t){ 
      // if opponent gets ball and he is faster or equally fast at position
      //DBLOG_MOV(0,"check nokick selfpass: Attackerpos: "<<attacking_op);
      break;
    }
    //    if((simulation_table[t].my_pos - simulation_table[t].ball_pos).norm() <= WSinfo::me->kick_radius - SAFETY_MARGIN){
    if(simulation_table[t].I_have_ball== true){
      // I have the ball at that time.
      if((Tools::min_distance_to_border(simulation_table[t].ball_pos) > MIN_DIST_2_BORDER) ||
	 (current_dist2border < MIN_DIST_2_BORDER && 
	  Tools::min_distance_to_border(simulation_table[t].ball_pos) > current_dist2border)){
	// position is either in pitch, or improves my current situation
	best_t = t;
      }  // ball is in pitch
    } // ball is kickable
  } // for all t


  if (best_t <0){ // no position is safe
    steps = best_t;
    return false;
  } 

  attacking_op = simulation_table[best_t].op_pos;
  attacking_num = simulation_table[best_t].op_num;
  //  DBLOG_MOV(0,"check with nokick: Attackerpos: "<<attacking_op);
  targetpos = simulation_table[best_t].my_pos;
  steps = best_t;
  DBLOG_MOV(0,"check nokick selfpass: SUCCESS: have the ball after "<<best_t<<" cycles, and  no op between!");
  return true;
}

bool Selfpass2::are_intermediate_ballpositions_safe(Vector tmp_ballpos, Vector tmp_ballvel, const int num_steps){
  // for all steps check, if ballposition is safe
  
  // check opponents;
  WSpset pset = WSinfo::valid_opponents; // enough, if I do it once.
  Vector resulting_pos;

  for(int t=0; t<=num_steps; t++){ 
    DBLOG_DRAW(0,C2D(tmp_ballpos.x,tmp_ballpos.y,.2,"grey"));          
    for (int idx = 0; idx < pset.num; idx++) {
      int op_steps = get_min_cycles2_pos(tmp_ballpos, pset[idx], t, resulting_pos);

      if(op_steps >=0){ // I found an opponent that gets to position in time
	DBLOG_MOV(0,"check intermediate ballpositions. opponent "<<pset[idx]->number<<" gets ball"
		  <<" in step "<<op_steps);
	DBLOG_DRAW(0,C2D(tmp_ballpos.x,tmp_ballpos.y,1.3,"black"));          
	return false;
      }
    } // for all ops idx
    tmp_ballpos += tmp_ballvel;
    tmp_ballvel *= ServerOptions::ball_decay;
  } // for all timesteps
  return true;
}
