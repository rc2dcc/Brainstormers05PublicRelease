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

#include "cmd.h"
#include <string.h>
#include "ws_info.h"

//#include "mdp_info.h"
void Cmd_Base::check_cmd() const { 
  if (!cmd_set) {
    ERROR_OUT << "\n no command was set"; 
    //cout << *(mdpInfo::mdp);
  }
}

void Cmd_Main::check_lock() const
    { if (lock_set) ERROR_OUT << "\n time: " << WSinfo::ws->time << " player: " << WSinfo::me->number << "this command is locked, type "<< get_type() << "\n"; }


bool Cmd_Main::clone( Cmd_Main const& cmd) {
  check_lock();
  lock_set= cmd.lock_set;
  cmd_set= cmd.cmd_set;
  type= cmd.type;
  par_1 = cmd.par_1;
  par_2 = cmd.par_2;
  par_ang= cmd.par_ang;
  priority= cmd.priority;
  return true;
}

int Cmd_Main::get_type(Value &power, Angle &angle){
  power= par_1; 
  angle= par_ang;
  return type;
}

std::ostream& operator<< (std::ostream& o, const Cmd_Main& cmd ) {
  Value par1,par2;
  Angle ang;
#if 0
  o << "\n [ ";
  if (cmd.is_lock_set()) 
    o << " <locked>, ";
#endif
  if (!cmd.is_cmd_set())
    o << " (no command)";
  else 
    switch (cmd.type) {
    case cmd.TYPE_MOVETO:
      cmd.get_moveto(par1,par2);
      o << "(move " << par1 << " " << par2 << ")";
      break;
    case cmd.TYPE_TURN:
      cmd.get_turn(ang);
      o << "(turn " << ang << ")";
      break;
    case cmd.TYPE_DASH:
      cmd.get_dash(par1);
      o << "(dash " << par1 << ")";
      break;
    case cmd.TYPE_KICK: 
      cmd.get_kick(par1,ang);
      o << "(kick " << par1 << " " << ang << ")";
      break;
    case cmd.TYPE_CATCH:
      cmd.get_catch(ang);
      o << "(catch " << ang << ")";
      break;
    case cmd.TYPE_TACKLE:
      cmd.get_tackle(par1);
      o << "(tackle " << par1 << ")";
      break;
    default:
      o << "( ? )";
    }
  //  o << " ]";
  return o;
}

std::ostream& operator<< (std::ostream& o, const Cmd_Neck & cmd ) {
  Angle ang;
  o << "\n [ ";
  if (cmd.is_lock_set()) 
    o << " <locked>, ";
  if (!cmd.is_cmd_set())
    o << " (no command)";
  else {
    cmd.get_turn(ang);
    o << " (turn_neck " << ang << ")";
  }
  o << " ]";
  return o;
}

std::ostream& operator<< (std::ostream& o, const Cmd_View & cmd ) {
  int angle, quality;
  o << "\n [ ";
  if (cmd.is_lock_set()) 
    o << " <locked>, ";
  if (!cmd.is_cmd_set())
    o << " (no command)";
  else {
    cmd.get_angle_and_quality(angle,quality);
    o << " (change_view ";
    switch (angle) {
    case cmd.VIEW_ANGLE_WIDE   :
      o << "wide ";
      break;
    case cmd.VIEW_ANGLE_NORMAL :
      o << "normal ";
      break;
    case cmd.VIEW_ANGLE_NARROW :
      o << "narrow ";
    default: 
      o << " ? ";
    };
    switch (quality) {
    case cmd.VIEW_QUALITY_HIGH :
      o << "high)";
      break;
    case cmd.VIEW_QUALITY_LOW :
      o << "low)";
      break;
    default:
      o << " ? ";
    }
  }
  o << " ]";
  return o;
}

std::ostream& operator<< (std::ostream& o, const Cmd_Say & cmd ) {
  o << "\n [ ";
  if (cmd.is_lock_set()) 
    o << " <locked>, ";
  if (!cmd.is_cmd_set())
    o << " (no command)";
  else {
    //cmd.get_turn(ang);
    o << " (say <...> )";// << cmd.message << ")";
  }
  o << " ]";
  return o;
}

std::ostream& operator<< (std::ostream& o, const Cmd & cmd ) {
  o << "\n--\nCmd:"
    << "\ncmd_main" << cmd.cmd_main
    << "\ncmd_neck" << cmd.cmd_neck
    << "\ncmd_view" << cmd.cmd_view
    << "\ncmd_say" << cmd.cmd_say;
  return o;
}

void Cmd_Say::set_pass(Vector const& pos, Vector const& vel, int time) {
  check_lock(); cmd_set= true; lock_set= true;
  pass.valid= true;
  pass.ball_pos= pos;
  pass.ball_vel= vel;
  pass.time= time;
}

bool Cmd_Say::get_pass(Vector & pos, Vector & vel, int & time) const {
  if ( ! pass.valid )
    return false;

  pos= pass.ball_pos;
  vel= pass.ball_vel;
  time= pass.time;
  return true;
}

void Cmd_Say::set_me_as_ball_holder(Vector const& pos) {
  check_lock(); cmd_set= true; lock_set= true;
  ball_holder.valid= true;
  ball_holder.pos= pos;
}

bool Cmd_Say::get_ball_holder(Vector & pos) const {
  if ( ! ball_holder.valid )
    return false;

  pos= ball_holder.pos;
  return true;
}



void Cmd_Say::set_ball(Vector const& pos, Vector const& vel, int age_pos, int age_vel) {
  check_lock(); cmd_set= true; lock_set= true;
  ball.valid= true;
  ball.ball_pos= pos;
  ball.ball_vel= vel;
  ball.age_pos= age_pos;
  ball.age_vel= age_vel;
}

bool Cmd_Say::get_ball(Vector & pos, Vector & vel, int & age_pos, int & age_vel) const {
  if ( ! ball.valid )
    return false;

  pos= ball.ball_pos;
  vel= ball.ball_vel;
  age_pos= ball.age_pos;
  age_vel= ball.age_vel;
  return true;
}

void Cmd_Say::set_players( WSpset const & pset ) {
  players.num= pset.num;
  if ( players.num > players.max_num )
    players.num= players.max_num;

  for (int i=0; i<players.num; i++) {
    players.player[i].pos= pset[i]->pos;
    players.player[i].team= pset[i]->team;
    players.player[i].number= pset[i]->number;
  }
}

int Cmd_Say::get_players_num() const {
  return players.num;
}

bool Cmd_Say::get_player(int idx, Vector & pos, int & team, int & number) const {
  if ( idx >= players.num )
    return false;

  pos= players.player[idx].pos;
  team= players.player[idx].team;
  number= players.player[idx].number;
  return true;
}


void Cmd_Say::set_msg(unsigned char type, short p1, short p2) {
  check_lock(); cmd_set= true; lock_set= true;
  msg.valid= true;
  msg.param1= p1;
  msg.param2= p2;
  msg.type=type;  //hauke
}

bool Cmd_Say::get_msg(unsigned char & type, short & p1, short & p2) const {
  if ( ! msg.valid )
    return false;
  p1= msg.param1;
  p2= msg.param2;
  type=msg.type;  //hauke
  return true;
}

#if 0
void Cmd_Say::set_message(const char *str) {
  set_message(str,0);
}

void Cmd_Say::set_message(const char *str, const int p_priority) {
  check_lock(); cmd_set= true; lock_set= true;

  priority = p_priority;
  if (!str) {
    ERROR_OUT << "\n Cmd_Say::set_message(const char *str) : str== 0";
    message[0]= '\0';
    return;
  }

  int len = strlen(str);

  if (len > MAXLEN) { 
    len= MAXLEN;
    //cerr << "\nsay message to long, cutting to length " << MAXLEN;
  }

  strncpy(message,str,len);
  message[len]= '\0';  
}
#endif

/* TEST */
#if 0
int main () {

  Cmd c;
  c.cmd_action.set_turn(5.0);
  c.cmd_neck.set_turn(1.4);
  
  cout << c;
  return 1;
}
#endif


