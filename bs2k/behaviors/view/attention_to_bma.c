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

#include "attention_to_bma.h"
#include "intentiontype.h"
#include "blackboard.h"
#include "ws_info.h"
#include "ws_memory.h"
#include "../policy/positioning.h"

#if 1
#define MYLOG_POL(LLL,XXX) LOG_POL(LLL,XXX)
//#define MYLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D<<XXX)
#else
#define MYLOG_POL(LLL,XXX) 
#define MYLOG_DRAW(LLL,XXX)
#endif

bool AttentionTo::initialized = false;

AttentionTo::AttentionTo() {
}

bool AttentionTo::get_cmd(Cmd &cmd) {


  construct_say_message(cmd);
  set_attention_to(cmd);

  return true;
}

void AttentionTo::set_attention_to(Cmd & cmd){
  if (cmd.cmd_att.is_cmd_set()){
    MYLOG_POL(0, << "ATTENTION TO:  CMD FORM ALREADY SET !!! ");
    return;
  }

  Vector target;
  Value speed;
  int target_player_number;

  // attention of PASS PLAYER
  if(Blackboard::get_pass_info(WSinfo::ws->time, speed, target, target_player_number) == true){
    if ( target_player_number > 0 ){
      MYLOG_POL(0, << "ATTENTION TO: Set attention to pass_receiver "<<target_player_number);
      cmd.cmd_att.set_attentionto(target_player_number);
    }
    return;
  } // pass player

  int  new_att= 0;
  Vector ballholder_pos;
  int ballholder;
  if(teammate_controls_ball(ballholder, ballholder_pos)){// I do not control the ball myself and a teammate has the ball
    if(ballholder >0 && ballholder != WSinfo::me->number)
      new_att = ballholder;
    MYLOG_POL(0, << "ATTENTION TO: Set attention to ball holder "<<ballholder);
  }

  // if someone plays a pass, keep attention to him!
  PPlayer p= WSinfo::get_teammate_with_newest_pass_info();
  if ( p ) {
    new_att = p->number;
    MYLOG_POL(0, << "ATTENTION TO: Set attention to pass giver "<<new_att);
  }

  MYLOG_POL(0,<<"ATTENTION TO: Current Attention to is set to player "<< WSinfo::ws->my_attentionto);

  if(new_att == 0){// not set yet

    PPlayer teammate;

#if 1 // debug information only
    for(int i=0; i<WSinfo::num_relevant_teammates;i++){
      MYLOG_POL(0,<<"ATTENTION 2: list of relevant teammates "<<WSinfo::relevant_teammate[i]);
    }
#endif

    for(int i=0; i<WSinfo::num_relevant_teammates;i++){
      if(WSinfo::get_teammate(WSinfo::relevant_teammate[i],teammate)){
	MYLOG_POL(0,<<"ATTENTION 2: Checking relevant teammate "
		<<teammate->number<<" age "<<teammate->age);
	if(new_att == 0 && teammate->age >1){ // not yet found and too old
	  new_att = teammate->number;
	}
	if(teammate->age >3){ // much too old -> emergency set attention
	  new_att = teammate->number;
	  break;
	}
      }
      else{ // strange: relevant teammate not found -> set attention to
	MYLOG_POL(0,<<"ATTENTION 2: Teammate "<<WSinfo::relevant_teammate[i]<<" not found. Set attention");
	new_att =WSinfo::relevant_teammate[i];
	break; // quit loop
      }
    }
  }



  int old_att= WSinfo::ws->my_attentionto;
  if (old_att <= 0)
    old_att= 0;
  if (new_att <= 0)
    new_att= 0;

  if ( old_att == new_att ) {
    MYLOG_POL(0,<<"ATTENTION TO: old attention = new attention: "<<new_att);
    return;
  }


  if ( new_att > 0 ) {
#if 0 // ridi: do not check this -> we want to listen to players, that we don' t see currently
    PPlayer p= WSinfo::get_teammate_by_number(new_att);
    if ( p && p->alive ){ //otherwise attentionto causes (wrong command form) messages
#endif
      cmd.cmd_att.set_attentionto(new_att);
      MYLOG_POL(0,<<"ATTENTION TO: Set Attention to player "<<new_att);
      //    }
  }
  else{ // new_att <= 0;  this should not happen
    cmd.cmd_att.set_attentionto_none();
    MYLOG_POL(0,<<"ATTENTION TO: set NO attention -> CHECK!!!");
  }
}

void AttentionTo::construct_say_message(Cmd & cmd){
  Vector target;
  Value speed,dir;
  int target_player_number;

  // Communication for GOALIE
  if (ClientOptions::consider_goalie) 
  {
    if ((WSinfo::ball->age <= 4) && (WSinfo::ws->play_mode == PM_PlayOn)) 
    {
      //ball information
      cmd.cmd_say.set_ball(WSinfo::ball->pos, WSinfo::ball->vel, WSinfo::ball->age, 4);
      return;
    } 
  } // Goalie


  // Communication for PASS PLAYER
  if(Blackboard::get_pass_info(WSinfo::ws->time, speed, target, target_player_number) == true){
    MYLOG_POL(0, << "COMMUNICATION: Blackboard intention is PASS: to "<<target_player_number<<" target pos "
	    <<target<<" w. speed "<<speed<<" -> Communicate");
    int cycles2go = 0; // communicate for one cycle only
    if (Blackboard::pass_intention.immediatePass == false) cycles2go = 1;
    Vector ballvel;
    ballvel.init_polar(speed,(target - WSinfo::me->pos).arg());
    cmd.cmd_say.set_pass(WSinfo::me->pos,ballvel,WSinfo::ws->time + cycles2go);
    return;
  } // pass info was set
  else
  {
    MYLOG_POL(0,<<"COMMUNICATE: Blackboard contains no pass info. So I do not communicate a pass."<<std::flush);
  }

  // Communication for PASS PLAYER, WHEN BALL JUST LEFT
  if(WSinfo::is_ball_kickable() == false && WSmemory::ball_was_kickable4me_at == WSinfo::ws->time -1){
    // I had the ball last time, so communicate where the ball is now
    MYLOG_POL(0, << "Communicate: I had the ball last cycle, now I say where it goes ["<<WSinfo::ball->pos+WSinfo::ball->vel<<","<<ServerOptions::ball_decay*WSinfo::ball->vel<<"]");
    cmd.cmd_say.set_ball(WSinfo::ball->pos, WSinfo::ball->vel, WSinfo::ball->age, WSinfo::ball->age );
  }
  
  // Communication for BALL HOLDER
  if(WSinfo::is_ball_pos_valid() && WSinfo::is_ball_kickable()){ // ball is kickable, but no pass info was set
    MYLOG_POL(0, << "COMMUNICATE: I have the ball and my Position. Use set me as Ballholder"<<std::flush);
    cmd.cmd_say.set_me_as_ball_holder(WSinfo::me->pos);
    return;
  }

  // Communication for PLAYER WO BALL
  generate_players4communication(cmd);
}

#define MAX_SAY_CAPACITY 4 // no more than 4 players

void AttentionTo::generate_players4communication(Cmd & cmd_form){

  WSpset players4communication;
  PPlayer ballholder = WSinfo::teammate_closest2ball();

  if(!WSinfo::is_ball_kickable()) {// I do not control the ball myself
    // compute my next position
    Vector my_next_pos, my_next_vel;
    Angle my_next_ang;
    Tools::model_player_movement(WSinfo::me->pos,WSinfo::me->vel,WSinfo::me->ang.get_value(),
				 cmd_form.cmd_main, my_next_pos, 
				 my_next_vel, my_next_ang);

    MYLOG_POL(0,<<"I am not holding the ball, so communicate my next position "
	    << my_next_pos<<" (current pos "<<WSinfo::me->pos);      
    //    MYLOG_POL(0,<<_2D<<C2D(my_next_pos.x, my_next_pos.y, 1.1, "grey"));

    // construct communication set :
    WS::Player next_me = *WSinfo::me;

    next_me.pos = my_next_pos;
    players4communication.append(&next_me);
      
    if(ballholder !=NULL){
      WSpset pset= WSinfo::valid_opponents;
      pset.keep_players_with_max_age(1);

      float width1 =  1.0 * 2* ((ballholder->pos-WSinfo::me->pos).norm()/2.5);
      float width2 = 4; // at ball be  a little smaller
      Quadrangle2d check_area = Quadrangle2d(ballholder->pos, WSinfo::me->pos , width1, width2);
      LOG_POL(0, <<_2D<<check_area );
      pset.keep_players_in(check_area);
      //pset.keep_players_in_quadrangle(WSinfo::me->pos, ballholder->pos, 10 ); // players between me and ballholder

      pset.keep_and_sort_closest_players_to_point(2,ballholder->pos);
    
      if(pset.num>0){
	MYLOG_POL(0,<<"COMMUNICATE: Found Opponents in passway: "<<pset.num<<std::flush);
	players4communication.append(pset);
      }
      pset= WSinfo::valid_opponents;
      pset.keep_players_with_max_age(0);
      pset.keep_players_in_circle(ballholder->pos,5.); // opponents close 2 me
      pset.keep_and_sort_closest_players_to_point(2,ballholder->pos);
      if(pset.num>0){
	MYLOG_POL(0,<<"COMMUNICATE: Found Opponents close2e ballholder: "<<pset.num<<std::flush);
	players4communication.join(pset);
      }
    }

    if(players4communication.num < MAX_SAY_CAPACITY){
      WSpset pset= WSinfo::valid_opponents;
      pset.keep_players_with_max_age(0);
      pset.keep_players_in_circle(my_next_pos,40.); // opponents close 2 me
      pset.keep_and_sort_closest_players_to_point(10,my_next_pos);
      if(pset.num>0){
	MYLOG_POL(0,<<"COMMUNICATE: Found Opponents close2e my next pos: "<<pset.num<<std::flush);
	players4communication.join(pset);
      }
    }

    if(players4communication.num < MAX_SAY_CAPACITY){
      WSpset pset= WSinfo::valid_teammates_without_me;
      pset.keep_players_with_max_age(0);
      pset.keep_players_in_circle(my_next_pos,40.); // opponents close 2 me
      pset.keep_and_sort_closest_players_to_point(10,my_next_pos);
      if(pset.num>0){
	MYLOG_POL(0,<<"COMMUNICATE: Found Teammates close2e my next pos: "<<pset.num<<std::flush);
	players4communication.join(pset);
      }
    }

  } // ball is not kickable for me and teammate controls the ball

  for(int i=0; i<players4communication.num;i++){
    MYLOG_POL(0,<<"My communication set "<<i<<" number "
	    <<players4communication[i]->number<<" pos "<<players4communication[i]->pos);
    
    MYLOG_POL(0,<<_2D<<C2D(players4communication[i]->pos.x, players4communication[i]->pos.y, 1.3, "magenta"));
  }
  cmd_form.cmd_say.set_players( players4communication );

}

bool AttentionTo::teammate_controls_ball(int &number, Vector & pos){
  number = 0; // default
  if(WSinfo::is_ball_kickable() != false){// I control the ball myself!
    return false;
  }
  int steps2go;
  Vector ipos;
    
  WSpset teammates = WSinfo::valid_teammates_without_me;
  InterceptResult ires[1];
  teammates.keep_and_sort_best_interceptors_with_intercept_behavior_to_WSinfoBallPos
            ( 1, ires );
  if (teammates.num > 0 && ires[0].time <= 2)
  {
    number = teammates[0]->number;
    pos    = teammates[0]->pos;
    MYLOG_POL(2,<<"AttentionTo: /EXPENSIVE INTERCEPT/ To my way of thinking, the fastest intercepting teammate, to whom I ought to pay attention, is #"<<number<<", standing at "<<pos);
    return true;
  }

  PPlayer teammate=Tools::get_our_fastest_player2ball(ipos, steps2go);
  if(steps2go <= 2 && teammate && teammate != WSinfo::me){ // check: steps2go <= 1 oder steps2go <= 2 besser?
    number=teammate->number;
    pos = teammate->pos;
    MYLOG_POL(2,<<"AttentionTo: /CHEAP INTERCEPT/ To my way of thinking, the fastest intercepting teammate, to whom I ought to pay attention, is #"<<number<<", standing at "<<pos);
    return true;
  }
  return false;
}




