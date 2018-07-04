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

/*
 *  Author:   Artur Merke 
 */

#include <sys/time.h>
#include <unistd.h>
#include <errno.h>   //for return values of select

#include <iostream>
#include <iomanip>
#include <strstream>
//#include <sstream>

#include "udpsocket.h"
#include "joystick.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mdpstate.h"
#include "mdp_info.h"
#include "mdp_memory.h"
#include "ws_info.h"
#include "ws_memory.h"
#include "cmd.h"
#include "options.h"

#include "sensorparser.h"
#include "wmstate.h"
#include "wmoptions.h"
#include "wmstats.h"
#include "log_macros.h"

//#include "bs2k_agent.h"
//#include "strategy_factory.h"
//#include "policy_factory.h"
//#include "move_factory.h"
#include "planning.h"
#include "positioning.h"
#include "serverparam.h"
#include "macro_msg.h"
#include "messages_times.h"
//#include "log_intention.h"
#include "default_server_param.h"

#include "main_behaviors.h"
#include "blackboard.h"

//dietrich
//#include "policy_one_vs_goalie.h"

//#define TEST_SAY_MECHANISM

namespace RUN
{
  const int MaxStringSize= UDPsocket::MAXMESG;

  /* ms_time means time mesured in milliseconds */
  long get_current_ms_time()
  { //returns time in ms since first call to this routine
    timeval tval;
    static long s_time_at_start= 0;
    if (gettimeofday(&tval,NULL))
      ERROR_OUT << ID << "\n something wrong with time mesurement";

    if ( 0 == s_time_at_start )
      s_time_at_start= tval.tv_sec;

    return (tval.tv_sec - s_time_at_start) * 1000 + tval.tv_usec / 1000;
  }

  bool server_alive= false;
  UDPsocket sock;

  Joystick joystick;

  BaseBehavior * behavior_controller;
  NeckBehavior * neck_controller;
  ViewBehavior * view_controller;
  AttentionToBehavior * attentionto_controller;
  //Bs2kAgent * controller;

  MDPmemory *mdp_memory;

  WMcompare wm_compare;
};

char buffer[RUN::MaxStringSize];

struct CommandsInfo
{
  int last_sb_time;
  int cmd_counter[CMD_MAX];
  int cmd_send_time[CMD_MAX];

  void reset()
  {
    last_sb_time= 0;
    for (int i=0; i< CMD_MAX; i++)
    {
      cmd_counter[i]= 0;
      cmd_send_time[i]= -10;
    }
  }

  void use_msg_sense_body( const Msg_sense_body & sb )
  {
    int dum[CMD_MAX];
    dum[CMD_MAIN_MOVETO]= sb.move_count;
    dum[CMD_MAIN_TURN]  = sb.turn_count;
    dum[CMD_MAIN_DASH]  = sb.dash_count;
    dum[CMD_MAIN_KICK]  = sb.kick_count;
    dum[CMD_MAIN_CATCH] = sb.catch_count;
    dum[CMD_NECK_TURN]  = sb.turn_neck_count;
    dum[CMD_SAY]        = sb.say_count;
    dum[CMD_VIEW_CHANGE]= sb.change_view_count;
    for (int i=0; i< CMD_MAX; i++)
    {
      if ( last_sb_time + 1 == sb.time
           && cmd_send_time[i] == last_sb_time
           && dum[i] != cmd_counter[i]+1 )
      {
        MessagesTimes::add_lost_cmd(sb.time,i);
      }
      cmd_counter[i]= dum[i];
    }

    last_sb_time= sb.time;
  }

  void set_command(int time, const Cmd & cmd)
  {
    if ( cmd.cmd_main.is_cmd_set() )
    {
      int cmd_type= CMD_INVALID;
      switch ( cmd.cmd_main.get_type() )
      {
      case cmd.cmd_main.TYPE_MOVETO : cmd_type= CMD_MAIN_MOVETO; break;
      case cmd.cmd_main.TYPE_TURN 	: cmd_type= CMD_MAIN_TURN; break;
      case cmd.cmd_main.TYPE_DASH 	: cmd_type= CMD_MAIN_DASH; break;
      case cmd.cmd_main.TYPE_KICK 	: cmd_type= CMD_MAIN_KICK; break;
      case cmd.cmd_main.TYPE_CATCH  : cmd_type= CMD_MAIN_CATCH; break;
      case cmd.cmd_main.TYPE_TACKLE  : cmd_type= CMD_MAIN_TACKLE; break;
      default: ERROR_OUT << ID << "\nwrong type";
      }
      cmd_send_time[cmd_type]= time;
      MessagesTimes::add_sent_cmd(time, cmd_type);
    }
    if ( cmd.cmd_neck.is_cmd_set() )
      cmd_send_time[CMD_NECK_TURN]= time;

    if ( cmd.cmd_say.is_cmd_set() )
      cmd_send_time[CMD_SAY]= time;

    if ( cmd.cmd_view.is_cmd_set() )
    {
      cmd_send_time[CMD_VIEW_CHANGE]= time;
      MessagesTimes::add_sent_cmd(time, CMD_VIEW_CHANGE);
    }
  }
};

struct MessagesInfo
{
  struct _msg
  {
    int received;
    int cycle;
    bool processed;
    long ms_time;
  };

  _msg msg[MESSAGE_MAX];

  void reset()
  {
    for (int i=0; i< MESSAGE_MAX; i++)
    {
      msg[i].received= 0;
      msg[i].processed= false;
    }
  }

  void set_cycle(int c)
  {
    for (int i=0; i< MESSAGE_MAX; i++)
      msg[i].cycle= c;
  }

  void set_ms_time(long ms_time)
  {
    for (int i=0; i< MESSAGE_MAX; i++)
      msg[i].ms_time= ms_time;
  }
};

WMstate wm,wmfull;
Cmd last_cmd;

#if 1
int idle(int ms_time)
{
  if (ms_time <= 0)
    return 0;

  struct timeval tv;

  tv.tv_usec= 1000 * ms_time;
  tv.tv_sec = 0;

  return  select(0, NULL, NULL, NULL, &tv);
}
#endif
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* Send initialize message */
void send_initialize_message(UDPsocket & sock, const char* teamname,bool goalie= false)
{
  if ( WMoptions::offline )
    return;

  static char buf[100] ;
  std::ostrstream o_buf(buf,100);
  //ostringstream o_buf(buf,100);
  //o_buf <<  "(init " << teamname << " (version 7.02)";
  o_buf <<  "(init " << teamname << " (version " << ClientOptions::server_version << ")";
  if (goalie)
    o_buf <<  " (goalie)";
  o_buf << ")" << std::ends;

  sock.send_msg(o_buf.str(), o_buf.pcount());
  //std::cout << "\nsending init message: " << buf;

}


/****************************************************************************************/

/* Parse initialize message */
bool recv_initialize_message(UDPsocket & sock, PlayMode & pm)
{
  if ( WMoptions::offline )
  {
    WM::my_side= left_SIDE;
    WM::my_number= 2;
    return true;
  }

  Msg_init init;
  int num_bytes;
  if ( sock.recv_msg(buffer,num_bytes,true) )
  {    //true= does redirect to new port address (differs from 6000)
    //if ( sock.recv_msg(buffer,num_bytes,false) ){
    buffer[num_bytes]= '\000';
    //std::cout << "\nreceived: " << buffer << std::flush;
  }

  int type,time;
  bool res= SensorParser::get_message_type_and_time(buffer,type,time);
  if ( type != MESSAGE_INIT)
    res= false;
  if (res)
    res= SensorParser::manual_parse_init(buffer,init);
  if (!res)
  {
    ERROR_OUT << ID << "\nWRONG INITIALIZATION STRING " << buffer << std::endl;
    return false;
  }
  //wm.play_mode= init.play_mode; //important to get the initial play_mode
  pm= init.play_mode; //important to get the initial play_mode
  //cout << "\nwm.play_mode= " << PLAYMODE_STRINGS[wm.play_mode];

  WM::my_side= init.side;
  WM::my_number= init.number;

  RUN::server_alive = true;
  {
    //const char * msg="(ear (off opp))(ear (off our partial))(ear (on our complete))"; //don't want to hear opponent's messages, don't want to hear partial messages !!!
    //const char * msg="(ear (off opp))(ear (on our complete))"; //don't want to hear opponent's messages !!!
    const char * msg="(ear (off opp))";
    sock.send_msg(msg, strlen(msg)+1);
  }
  return true;
}

bool recv_parameter_messages(UDPsocket & sock)
{
  if ( WMoptions::offline )
  {
    bool res= true;
    res= res && ServerParam::incorporate_server_param_string( DEFAULT_MESSAGE_SERVER_PARAM );
    res= res && ServerParam::incorporate_player_param_string( DEFAULT_MESSAGE_PLAYER_PARAM);
    for (int i=0; i<DEFAULT_NUM_MESSAGE_PLAYER_TYPE; i++)
      res= res && ServerParam::incorporate_player_type_string(DEFAULT_MESSAGE_PLAYER_TYPE[i]);
    return res;
  }

  //the socket should be blocking in this routine
  bool res= false;
  int message_count_down= 20;
  while (true)
  {
    message_count_down--;
    if (message_count_down <= 0)
    {
      res= false;
      break;
    }
    int num_bytes;
    if ( sock.recv_msg(buffer,num_bytes,true) )
    {    //true= does redirect to new port address (differs from 6000)
      buffer[num_bytes]= '\000';
      //cout << "\nreceived: " << buffer;
    }
    else
      break;

    int type,time;
    res= SensorParser::get_message_type_and_time(buffer,type,time);

    if (!res)
      break;

    if ( type == MESSAGE_SERVER_PARAM )
    {
      res= ServerParam::incorporate_server_param_string(buffer);
    }
    else if ( type == MESSAGE_PLAYER_PARAM )
    {
      res= ServerParam::incorporate_player_param_string(buffer);
    }
    else if ( type == MESSAGE_PLAYER_TYPE )
    {
      res= ServerParam::incorporate_player_type_string(buffer);
    }
    else if ( type == MESSAGE_ERROR )
    {
      ERROR_OUT << ID << "\nreceived error message: " << buffer << std::flush;
      res= false;
    }
    else
    {
      ERROR_OUT << ID << "\nunknown message: " << buffer << std::flush;
      res= false;
    }
    if (!res)
      break;

    if ( ServerParam::all_params_ok() )
      break;
  }

  return res;
}


bool produce_parameter_messages(UDPsocket & sock, ostream & out)
{
#if 1
  //the socket should be blocking in this routine
  bool res= false;
  int number_of_player_types= 0;
  int message_count_down= 20;

  out << "/*"
  << "\n  The following server parameters were automatically generated by"
  << "\n\n    bool produce_parameter_messages(UDPsocket & sock, ostream & out)"
  << "\n\n  in artagent/client.c"
  << "\n*/";

  while (true)
  {
    message_count_down--;
    if (message_count_down <= 0)
    {
      res= false;
      break;
    }
    int num_bytes;
    if ( sock.recv_msg(buffer,num_bytes,true) )
    {    //true= does redirect to new port address (differs from 6000)
      buffer[num_bytes]= '\000';
      //cout << "\nreceived: " << buffer;
    }
    else
      break;

    int type,time;
    res= SensorParser::get_message_type_and_time(buffer,type,time);

    if (!res)
      break;

    if ( type == MESSAGE_SERVER_PARAM )
    {
      res= ServerParam::incorporate_server_param_string(buffer);
      if (res)
      {
        char const* dum= buffer;
        out << "\n\nconst char DEFAULT_MESSAGE_SERVER_PARAM[]=\"";
        while ( *dum != '\0' )
        {
          if ( *dum == '"' )
            out << '\\';
          out << *dum;
          dum++;
        }
        out << "\";";
      }
    }
    else if ( type == MESSAGE_PLAYER_PARAM )
    {
      res= ServerParam::incorporate_player_param_string(buffer);
      if (res)
        out << "\n\nconst char DEFAULT_MESSAGE_PLAYER_PARAM[]=\"" << buffer << "\";"
        << "\n\nconst int DEFAULT_NUM_MESSAGE_PLAYER_TYPE= " << ServerParam::number_of_player_types() << ";"
        << "\nconst char * const DEFAULT_MESSAGE_PLAYER_TYPE[DEFAULT_NUM_MESSAGE_PLAYER_TYPE]= { ";
    }
    else if ( type == MESSAGE_PLAYER_TYPE )
    {
      res= ServerParam::incorporate_player_type_string(buffer);
      number_of_player_types++;
      if (res)
      {
        if (number_of_player_types != 1)
          out << " ,\n      ";
        else
          out << "\n      ";

        out << "\"" << buffer << "\"";
        if (number_of_player_types ==  ServerParam::number_of_player_types() )
          out << "};";
      }
    }
    else if ( type == MESSAGE_ERROR )
    {
      ERROR_OUT << ID << "\nreceived error message: " << buffer << std::flush;
      res= false;
    }
    else
    {
      ERROR_OUT << ID << "\nunknown message: " << buffer << std::flush;
      res= false;
    }
    if (!res)
      break;

    if ( ServerParam::all_params_ok() )
      break;
  }

  return res;
#endif
}

double x_LP_2_SRV(double x)
{
  return x;
}

double y_LP_2_SRV(double y)
{
  return -y;
}

Angle ang_LP_2_SRV_deg(Angle a)
{
  //if (a < 0 || a > 2*PI) cerr << "\a= " << a;
  ANGLE tmp(a); //normalize in 0..2PI
  a= tmp.get_value();
  if ( a == PI )
  {
    WARNING_OUT << " an angle with value PI was set, this is not defined (see PI_MINUS_EPS)";
  }
  if (a<PI) return - a*180.0/PI;
  return -(a-2*PI)*180.0/PI;
}

void send_cmd_aserver_ver_7(UDPsocket & sock, const Cmd & cmd);

void send_cmd(UDPsocket & sock, Cmd const& cmd, Msg_teamcomm2 const& tc)
{
  if (WMoptions::use_aserver_ver_7_for_sending_commands)
    return send_cmd_aserver_ver_7(sock,cmd);

  /// new servers support more the one command in a message (say must be last!!!)
  static char buf_main[RUN::MaxStringSize*2] ;
  std::ostrstream o_main(buf_main,RUN::MaxStringSize);

  double par1,par2;
  Angle ang;

  /***************************************************************************/
  //interpret main command, i.e. one of {moveto,turn,dash,kick,catch}
  if ( cmd.cmd_main.is_cmd_set() )
  {
    const Cmd_Main& action= cmd.cmd_main; //shortcut
    switch ( action.get_type())
    {
    case action.TYPE_MOVETO   :
      action.get_moveto(par1,par2);
      o_main << "(move " <<  x_LP_2_SRV( par1 ) << " " << y_LP_2_SRV( par2 ) << ")";
      LOG_WM(wm.time.time,0,"SENDING MOVE " << buf_main);
      break;
    case action.TYPE_TURN 	 :
      action.get_turn(ang);
      o_main << "(turn " << ang_LP_2_SRV_deg( ang ) << ")";
      LOG_WM(wm.time.time,0,"SENDING TURN " << buf_main);
      //cout << "\n got turn " << ang <<" and transformed in " << ang_LP_2_SRV_deg( ang );
      break;
    case action.TYPE_DASH 	 :
      action.get_dash(par1);
      o_main << "(dash " << par1 << ")";
      break;
    case action.TYPE_KICK 	 :
      action.get_kick(par1,ang);
      o_main << "(kick " << par1 << " " << ang_LP_2_SRV_deg( ang ) << ")";
      break;
    case action.TYPE_CATCH    :
      action.get_catch( ang );
      o_main << "(catch " << ang_LP_2_SRV_deg( ang ) << ")";
      break;
    case action.TYPE_TACKLE 	 :
      action.get_tackle(par1);
      o_main << "(tackle " << par1 << ")";
      break;
    default:
      ERROR_OUT << ID << "\nwrong command";
    }
  }
  else
  { //es wurde kein Kommando gesetzt, evtl. warnen!!!
    //o_main << "(turn 0)"; //als debug info verwendbar
    //ERROR_OUT << ID << "\nno command was specified, sending (turn 0)";
  }
  /***************************************************************************/
  //interpret neck command
  if ( cmd.cmd_neck.is_cmd_set())
  {
    cmd.cmd_neck.get_turn( ang );
    o_main << "(turn_neck " << ang_LP_2_SRV_deg( ang ) << ")";
  }
  /***************************************************************************/
  //interpret view command
  if (cmd.cmd_view.is_cmd_set())
  {
    if ( WMoptions::behave_after_think )
    {
      /* this is a hack, because out change view strategy doesn't function when the cycle length is != 100ms */
      o_main << "(change_view narrow high)";
    }
    else
    {
      const Cmd_View &view= cmd.cmd_view;
      int va,vq;
      o_main << "(change_view ";
      view.get_angle_and_quality(va,vq);
      if ( view.VIEW_ANGLE_WIDE == va )
        o_main << "wide";
      else if ( view.VIEW_ANGLE_NORMAL == va )
        o_main << "normal";
      else
        o_main << "narrow";
      o_main << " ";
      if ( view.VIEW_QUALITY_HIGH == vq)
        o_main << "high";
      else
        o_main << "low";
      o_main << ")";
    }
  }


#if 1
  //sending of say

  if ( tc.get_num_objects() > 0 )
  {
    //  return;
    char * dum;
    char str[20]; //20 is enough for tc version 2
    bool res= SensorParser::manual_encode_teamcomm(str, tc, dum);
    if ( !res || dum == str )
    {
      // Sput: get rid of err msg before game has started... */
      if(WSinfo::ws->time > 0)
      {
        ERROR_OUT << ID << "something wrong with teamcomm encoding\n" << tc;
      }
    }
    else
    {
      dum[0]= '\0';
      o_main << "(say \"" << str << "\")";
    }
  }
#endif
  /***************************************************************************/
  /*


  in server 9.0.3 there is a bug, where nothing can come after an (attentionto off)



  */
  //interpret attention command
  //if ( WMoptions::behave_after_think )  //DEBUG, never leave it in normal code!!! (synch_mode seems not to work with attentionto)
  if ( cmd.cmd_att.is_cmd_set())
  {
    int p= -1;
    cmd.cmd_att.get_attentionto(p);
    if ( p < 0 )
      o_main << "(attentionto off)";
    else if ( p > 0 && p <= NUM_PLAYERS )
      o_main << "(attentionto our " << p << ")";
    else
    {
      o_main << "(attentionto off)"; //will not do any harm and may help in this undefined situation
      ERROR_OUT << ID << "\nwrong attentionto parameter " << p;
    }
  }

  if ( WMoptions::behave_after_think )
    o_main << "(done)";

  o_main << std::ends;
  if (o_main.pcount() > 1)
  {
    //LOG_WM(wmfull.time.time,0,<< "SENDING COMMAND:" << o_main.str());
    sock.send_msg( o_main.str(), o_main.pcount() );
  }
}

#define CMD_VIEW_HACK

#ifdef CMD_VIEW_HACK
Cmd_View cmd_view_hack;
#endif

#ifdef CMD_VIEW_HACK
void send_cmd_view(UDPsocket & sock, const Cmd_View & cmd_view)
{
  static char buf_main[RUN::MaxStringSize*2] ;
  std::ostrstream o_main(buf_main,RUN::MaxStringSize);

  //interpret view command
  if ( cmd_view.is_cmd_set())
  {
    const Cmd_View &view= cmd_view;
    int va,vq;
    o_main << "(change_view ";
    view.get_angle_and_quality(va,vq);
    if ( view.VIEW_ANGLE_WIDE == va )
      o_main << "wide";
    else if ( view.VIEW_ANGLE_NORMAL == va )
      o_main << "normal";
    else
      o_main << "narrow";
    o_main << " ";
    if ( view.VIEW_QUALITY_HIGH == vq)
      o_main << "high";
    else
      o_main << "low";
    o_main << ")";
  }
  o_main << std::ends;

  if (o_main.pcount() > 0)
    sock.send_msg( o_main.str(), o_main.pcount() );
}
#endif

#if 0
void send_tc(UDPsocket & sock, const Msg_teamcomm2 & tc)
{
  //if ( tc.get_num_objects() <= 0 )
  //  return;
  char *dum;
  char str[20]; //20 is enough for tc version 2
  str[0]= '(';
  str[1]= 's';
  str[2]= 'a';
  str[3]= 'y';
  str[4]= ' ';
  SensorParser::manual_encode_teamcomm(str+5, tc, dum);
  dum[0]= ')';
  dum[1]= '\0';
  sock.send_msg( str, dum-str+2);
}
#endif

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/


/****************************************************************************************/

#if 0
#include "strconv.h"
void debug_1_Step_Kick(Cmd & cmd)
{
  cout << "\n" << mdpInfo::mdp->time_current << "(0,...360) >>>" << std::flush;
  cin.getline(buffer,1000);

  double angle= -1;
  //cout << "\nplease enter angle (in degree)";
  //cin >> angle;
  const char * dum;
  bool really_exec= true;
  bool res= substrconv(buffer,angle,dum);
  if ( strshift(dum,"n",dum) )
    really_exec= false;
  if (res && angle >=  0.0)
  {
    angle = angle * PI/180.0;
    double res= Move_1Step_Kick::get_max_vel_in_dir(angle);
    Move_1Step_Kick kick1(res,angle);
    if ( really_exec )
      kick1.execute(cmd);
    cout << "\n res_factor = " << res << std::flush;
    //cmd.cmd_main.set_turn(0.0);
  }
  if ( !res || !really_exec )
  {
    cmd.cmd_main.set_turn(0);
    cout << "\nsetting (turn 0) command";
  }
}
#endif

#if 0
void debug_1or2_Step_Kick(Cmd & cmd)
{
  cout << "\n" << mdpInfo::mdp->time_current << "(0,...360) >>>" << std::flush;
  //cin.getline(buffer,1000);

  double angle= -1;
  //cout << "\nplease enter angle (in degree)";
  //cin >> angle;
  const char * dum;
  bool really_exec= true;
  bool res= substrconv(buffer,angle,dum);
  if ( strshift(dum,"n",dum) )
    really_exec= false;
  if (res && angle >=  0.0)
  {
    angle = angle * PI/180.0;
    Value vel= 2.5;
    //Value vel= 1.8;
    Value res_vel_1step;
    Value res_vel_2step;
    Cmd_Main res_cmd_1step;
    Cmd_Main res_cmd_2step;
    double res1= Move_1or2_Step_Kick::get_vel_in_dir(vel, 0.0,
                 res_vel_1step, res_cmd_1step,
                 res_vel_2step, res_cmd_2step);

    cout << "\n vel   = " << res1 << " vel1= " << res_vel_1step << " vel2= " << res_vel_2step;

    int res_steps= 0;
    if ( fabs( res_vel_1step - vel ) < 0.1 )
    {
      res_steps= 1;
      cmd.cmd_main= res_cmd_1step;
    }
    else if ( fabs( res_vel_2step - vel) < 0.1 )
    {
      res_steps= 2;
      cmd.cmd_main= res_cmd_2step;
    }
    else
      cmd.cmd_main.set_turn(0);

    cout << "\n steps = " << res_steps
    << "\n cmd   = " << cmd.cmd_main
    << std::flush<
  }
  if ( !res || !really_exec )
  {
    cmd.cmd_main.set_turn(0);
    cout << "\nsetting (turn 0) command";
  }
}


void debug_moving(Cmd & cmd)
{

  double d= drand48();

  if (d < 0.2 )
  {
    cmd.cmd_main.set_turn( drand48() * 2*PI);
  }
  else
    cmd.cmd_main.set_dash(100);

}
#endif


void correct_wmstate_team(WMstate & state, const WMstate & state_full, int level)
{
  if (level >= 1)
    for (int i=0; i< NUM_PLAYERS+1; i++)
      state.my_team[i] = state_full.my_team[i];

  if (level >= 2)
  {
    for (int i=0; i< NUM_PLAYERS+1; i++)
      state.his_team[i] = state_full.his_team[i];
    state.his_team[0].alive= false;
  }
}

void correct_wmstate_ball(WMstate & state, const WMstate & state_full, int level)
{
  if ( level == 0)
    return;

  Vector rel_ball= state_full.ball.pos - state_full.my_team[WM::my_number].pos;
  double dist_ball= rel_ball.norm();

  if ( level <= -1 )
  {
    if ( dist_ball >= 1.085 && level <= -1
         || dist_ball >= 3.0 && level <= -2
         || dist_ball >= 5.0 && level <= -3 )
    {
      //replace just the relative ball pos!
      state.ball.pos= state.my_team[WM::my_number].pos + rel_ball;
      //and ball vel
      state.ball.vel = state_full.ball.vel;
    }
    return;
  }

  if ( dist_ball <= 1.085 && level >= 1
       || dist_ball <= 3.0 && level >= 2
       || dist_ball <= 5.0 && level >= 3
       || level >= 4 )
  {
    //replace just the relative ball pos!
    state.ball.pos= state.my_team[WM::my_number].pos + rel_ball;
    //and ball vel
    state.ball.vel = state_full.ball.vel;
  }

  if ( level >= 5 )
  {
    state.my_team[WM::my_number]= state_full.my_team[WM::my_number];
    state.ball = state_full.ball;
  }
  if ( level >= 6 )
  {
    state.my_team[WM::my_number]= state_full.my_team[WM::my_number];
    state.ball = state_full.ball;
  }
}


/** waits until the given time is over, or until something happens with the file descriptor
 
\param
*/
int idle(int ms_time, int s_time, int fd, bool & got_fd)
{
  fd_set rfds;
  struct timeval tv;
  int retval;
  int max_fd_plus_1=  fd + 1;
  got_fd= false;

  FD_ZERO(&rfds);

  //FD_SET(0, &rfds); //Standardeingabe
  FD_SET(fd, &rfds);

  tv.tv_usec= 1000 * ms_time;
  tv.tv_sec = s_time;

  //long ms_time_before= RUN::get_current_ms_time();//TEST, muss weg!!!!

  retval = select(max_fd_plus_1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

#if 0 //test if select waits longer the required
  long ms_time_after= RUN::get_current_ms_time(); //TEST, muss weg!!!!
  long tmp= 1000 - (tv.tv_usec / 1000);
  //cerr << "\n+++" << tv.tv_usec << ", " << tmp << " ms_time= " <<ms_time;
  if ( s_time <= 0 && ms_time_after - ms_time_before > ms_time + 10)
  {
    //cerr << "\n*** tv.tv_usec / 1000= " << tmp << " ms_time= " << ms_time << " diff= " << tmp-ms_time << std::flush;
    cerr << "\n*** tv.tv_usec / 1000= " << tmp << " ms_time= " << ms_time << " diff= " << ms_time_after - ms_time_before - ms_time;
  }
#endif
  if ( retval > 0 )
    if ( FD_ISSET(fd,&rfds) )
    {
      got_fd= true;
      return 1;
    }
    else
      ERROR_OUT << ID << "\nwarning: received something, but not on the fd= " << fd ;

  if (retval < 0)
  {
    switch (errno)
    {
    case EBADF:
      ERROR_OUT << ID << "\nselect error: an invalid file descriptor was given in one of the sets";
      break;
    case EINTR:
      //cerr << "\nselect error: a non blocked signal was caught";
      break;
    case EINVAL:
      ERROR_OUT << ID << "\nselect error: n is negative";
      break;
    case ENOMEM:
      ERROR_OUT << ID << "\nselect error: select was unable to allocate memory for internal tables";
      break;
    default:
      ERROR_OUT << ID << "\nselect error: error code " << errno;
    }
    return -1;
  }

  return 0;
}

int idle(int ms_time, int s_time, int fd1, bool & got_fd1, int fd2, bool & got_fd2)
{
  fd_set rfds;
  struct timeval tv;
  int retval;
  int max_fd_plus_1;
  if (fd2>fd1)
    max_fd_plus_1=  fd2 + 1;
  else
    max_fd_plus_1=  fd1 + 1;
  got_fd1= false;
  got_fd2= false;
  FD_ZERO(&rfds);

  //FD_SET(0, &rfds); //Standardeingabe
  FD_SET(fd1, &rfds);
  FD_SET(fd2, &rfds);

  tv.tv_usec= 1000 * ms_time;
  tv.tv_sec = s_time;

  //long ms_time_before= RUN::get_current_ms_time();//TEST, muss weg!!!!

  retval = select(max_fd_plus_1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

#if 0 //test if select waits longer the required
  long ms_time_after= RUN::get_current_ms_time(); //TEST, muss weg!!!!
  long tmp= 1000 - (tv.tv_usec / 1000);
  //cerr << "\n+++" << tv.tv_usec << ", " << tmp << " ms_time= " <<ms_time;
  if ( s_time <= 0 && ms_time_after - ms_time_before > ms_time + 10)
  {
    //cerr << "\n*** tv.tv_usec / 1000= " << tmp << " ms_time= " << ms_time << " diff= " << tmp-ms_time << std::flush;
    cerr << "\n*** tv.tv_usec / 1000= " << tmp << " ms_time= " << ms_time << " diff= " << ms_time_after - ms_time_before - ms_time;
  }
#endif
  if ( retval > 0 )
  {
    if ( FD_ISSET(fd1,&rfds) )
      got_fd1= true;
    if ( FD_ISSET(fd2,&rfds) )
    {
      got_fd2= true;
      return 1;
    }
    if (got_fd1)
      return 1;

    ERROR_OUT << ID << "\nwarning: received something, but not on the fd1= " << fd1 << " or fd2= " << fd2;
  }

  if (retval < 0)
  {
    switch (errno)
    {
    case EBADF:
      ERROR_OUT << ID << "\nselect error: an invalid file descriptor was given in one of the sets";
      break;
    case EINTR:
      //cerr << "\nselect error: a non blocked signal was caught";
      break;
    case EINVAL:
      ERROR_OUT << ID << "\nselect error: n is negative";
      break;
    case ENOMEM:
      ERROR_OUT << ID << "\nselect error: select was unable to allocate memory for internal tables";
      break;
    default:
      ERROR_OUT << ID << "\nselect error: error code " << errno;
    }
    return -1;
  }

  return 0;
}

/** returns the number of read messages */
int receive_and_incorporate_joystick_messages(Joystick & joystick, WMstate * state, WMstate * state_full)
{
  int retval= joystick.read_all_events();

  if (state)
    state->incorporate_joystick_info(joystick);

  if (state_full)
    state_full->incorporate_joystick_info(joystick);

  //cout << "\nreceive_and_incorporate_joystick_messages= " << retval;
  return retval;
}

/** returns the number of read messages */
int receive_and_incorporate_server_messages(UDPsocket & sock, MessagesInfo & msg_info, CommandsInfo & cmd_info, WMstate * state, WMstate * state_full)
{
  int num_messages= 0;
  int num_bytes;
  Msg_sense_body sense_body;
  Msg_see see;
  Msg_fullstate fullstate;
  Msg_fullstate_v8 fullstate_v8;
  Msg_hear hear;
  Msg_change_player_type change_player_type;
  //Msg_teamcomm2 tc;
  //Msg_server_param server_param;

  int result;

  long ms_time= RUN::get_current_ms_time();
  bool run= true;
  while ( run && RUN::sock.recv_msg( buffer,num_bytes) )
  { //read all messages from the socket and incorporate them!
    buffer[num_bytes] = '\000';
    // cout << "\ngot=[" << buffer<< "]"<<endl<<flush;
    num_messages++;

    ms_time= RUN::get_current_ms_time(); //seems to be better if time is measured every time

    int type, time;
    bool res= SensorParser::get_message_type_and_time(buffer,type,time);

    if ( type == MESSAGE_THINK && time < 0 ) //MESSAGE_THINK doesn't have own time stamp
      time= msg_info.msg[MESSAGE_SENSE_BODY].cycle;

    //cout << "\nmsg= " << MESSAGE_STRINGS[type];
#if 0 //////////////////////////////////////// DEBUG
    if (type == MESSAGE_SEE)
      LOG_WM(time,0, << buffer);
#endif

    if (!res)
    {
      ERROR_OUT << ID << "\nwrong message type " << buffer;
      return -1;
    }

    MessagesTimes::add(time,ms_time,type);

    msg_info.msg[type].received++;
    msg_info.msg[type].cycle= time;
    msg_info.msg[type].ms_time= ms_time;

    const char * error_point= 0;
    switch ( type )
    {
    case MESSAGE_SENSE_BODY:
      WM::time= time;
      if (WMoptions::ignore_sense_body)
        break;
      msg_info.msg[type].processed= true;

      res= SensorParser::manual_parse_sense_body(buffer,sense_body,error_point);
      if (!res)
        break;
      //LOG_WM(sense_body.time,0, << "MSG SENSE:\n" << buffer); //debug
      cmd_info.use_msg_sense_body(sense_body);
      if (state) state->incorporate_cmd_and_msg_sense_body(last_cmd,sense_body);
      break;
    case MESSAGE_SEE:
      if (WMoptions::ignore_see)
        break;
      msg_info.msg[type].processed= true;

      res= SensorParser::manual_parse_see(buffer,see,error_point);
      if (!res)
        break;

      LOG_WM(see.time,0, << "MSG SEE: " << msg_info.msg[MESSAGE_SEE].ms_time - msg_info.msg[MESSAGE_SENSE_BODY].ms_time << " ms after sense body  now=" << ms_time);// << "\n" << buffer);
      if (state)
      {
        //	bool tc_was_sent= state->time == state->time_of_last_msg_see;
        state->incorporate_msg_see(see,msg_info.msg[MESSAGE_SEE].ms_time,
                                   msg_info.msg[MESSAGE_SEE].ms_time -
                                   msg_info.msg[MESSAGE_SENSE_BODY].ms_time);

#if 0 //team comm is no longer sent directly after see
        if ( !tc_was_sent && WMoptions::send_teamcomm )
        {
          state->export_msg_teamcomm(tc);
          //tc.ball_upd= true; tc.ball_pos= Vector(10,10); //test
          if ( tc.get_num_objects() > 0 )
            send_tc(RUN::sock,tc);
        }
#endif

#ifdef CMD_VIEW_HACK
        if ( ! WMoptions::behave_after_think ) //do not use view mode synchronization in synch_mode
          if ( cmd_view_hack.is_cmd_set() )
          {
            send_cmd_view(RUN::sock,cmd_view_hack);
            cmd_view_hack.unset_lock();
            cmd_view_hack.unset_cmd();
            if ( cmd_view_hack.is_cmd_set() )
            {
              ERROR_OUT << ID << "this should never be true";
            }
          }
#endif

      }
      break;
    case MESSAGE_FULLSTATE:
      if (WMoptions::ignore_fullstate)
        break;
      msg_info.msg[type].processed= true;

      if ( ClientOptions::server_version >= 8.0)
      {
        res= SensorParser::manual_parse_fullstate(buffer,fullstate_v8);
        if (!res)
          break;
        if (state_full)
        {
          state_full->import_msg_fullstate(fullstate_v8);
#if 0     //just a test
          state_full->export_msg_teamcomm(tc);
          if ( tc.get_num_objects() > 0 )
            send_tc(RUN::sock,tc);
#endif

        }
        LOG_WM(fullstate_v8.time,0, << " $$$$$ FULL my vel= " << state_full->my_team[WM::my_number].vel << " value= " << state_full->my_team[WM::my_number].vel.norm() << " ang= " << state_full->my_team[WM::my_number].vel.arg() );
      }
      else
      {
        res= SensorParser::manual_parse_fullstate(buffer,fullstate);
        if (!res)
          break;

        if (state_full) state_full->import_msg_fullstate(fullstate);
        LOG_WM(fullstate.time,0, << " $$$$$ FULL my vel= " << state_full->my_team[WM::my_number].vel << " value= " << state_full->my_team[WM::my_number].vel.norm() << " ang= " << state_full->my_team[WM::my_number].vel.arg() );
      }

      break;
    case MESSAGE_THINK:
      //cout << "\nMESSAGE_THINK";
      msg_info.msg[type].processed= true;
      //run= false;
      break;
    case MESSAGE_HEAR:{
      if (WMoptions::ignore_hear)
        break;

      msg_info.msg[type].processed= true;
      bool reset_int=false;
      result = SensorParser::manual_parse_hear(buffer,hear,error_point,reset_int);
      //res= SensorParser::manual_parse_hear(buffer,hear,error_point);

      if (reset_int)
      {
        RUN::behavior_controller->reset_intention();
        WSmemory::init();
        Blackboard::init();
      }

      if (!res)
        break;

#ifdef TEST_SAY_MECHANISM
      if ( hear.teamcomm_upd )
        MessagesTimes::add(hear.time, ms_time, MESSAGE_DUMMY2, hear.teamcomm.from);
#endif
      //if ( hear.teamcomm_upd ) LOG_WM(hear.time, 0,<< "got teamcomm [" << buffer << "]"); //debug
      if (state) state->incorporate_msg_hear(hear);
      if (state_full) state_full->incorporate_msg_hear(hear);
      break;}
    case MESSAGE_CHANGE_PLAYER_TYPE:
      msg_info.msg[type].processed= true;
      res= SensorParser::manual_parse_change_player_type(buffer,change_player_type);

      if (!res)
        break;

      if (state) state->incorporate_msg_change_player_type(change_player_type);
      break;
    case MESSAGE_INIT:
      ERROR_OUT << ID << "\nreceived init message: " << buffer << std::flush;
      break;
    case MESSAGE_SERVER_PARAM:
      //cerr << "\nreceived server_param message (not yet supported)" << std::flush;
      break;
    case MESSAGE_PLAYER_PARAM:
      //cerr << "\nreceived player_param message (not yet supported)" << std::flush;
      break;
    case MESSAGE_PLAYER_TYPE:
      //cerr << "\nreceived player_type message (not yet supported)" << std::flush;
      break;
    case MESSAGE_OK:
      INFO_OUT << ID << "\nreceived ok message: " << buffer << std::flush;
      break;
    case MESSAGE_ERROR:
      INFO_OUT << ID << "\nreceived error message [p="<< WM::my_number << " t= " << state->time.time << "]: "<< buffer << std::flush;
      break;
    default:
      ERROR_OUT << ID << "\nunknown message: " << buffer << std::flush;
      break;
    }

    if (!res)
    {
      ERROR_OUT << ID << "\nproblems with message: ";
      SensorParser::show_parser_error_point(ERROR_STREAM,buffer,error_point);
    }
  }

  return num_messages;
}

void export_mdpstate( MDPstate & mdp_state, MDPstate & mdp_state_full, const WMstate * state, const WMstate * state_full)
{
  if (state)
  {
    if (state_full)
    {
      state_full->export_mdpstate(mdp_state_full);
      mdpInfo::server_state= &mdp_state_full;
    }
    else
      mdpInfo::server_state= 0;

    state->export_mdpstate(mdp_state);
    mdpInfo::update(mdp_state);
    return;
  }

  if (state_full)
  {
    state_full->export_mdpstate(mdp_state_full);
    mdpInfo::server_state= &mdp_state_full;
    mdpInfo::update(mdp_state_full);
    return;
  }

  ERROR_OUT << ID << "\nshould never reach this point";
}

void export_ws( WS * ws_state, WS * ws_state_full, const WMstate * state, const WMstate * state_full)
{
  if (ws_state)
  {
    if (ws_state_full)
      state_full->export_ws(*ws_state_full);

    state->export_ws(*ws_state);
    WSinfo::init(ws_state,ws_state_full);
    return;
  }

  if (ws_state_full)
  {
    state_full->export_ws(*ws_state_full);
    WSinfo::init(ws_state_full,ws_state_full);
    return;
  }

  ERROR_OUT << ID << "\nshould never reach this point";
}


void check_reduce_dash_power(Cmd &cmd_form)
{
  //modify dash-command according to available stamina
  if (cmd_form.cmd_main.get_type() == cmd_form.cmd_main.TYPE_DASH)
  {
    Value power = 0;
    cmd_form.cmd_main.get_dash(power);
    LOG_POL(0, << "client.c:check_reduce_dash_power: Cmd Dash with power " << power);
    //LOG_DEB(0, << " Actual stamina " << mdpInfo::mdp->me->stamina.v);
    //LOG_DEB(0, << "stamina4steps_if_dashing " << mdpInfo::stamina4steps_if_dashing(power));
    //LOG_DEB(0, << "stamina_left " << mdpInfo::stamina_left());
    /*
    if (mdpInfo::mdp->me->effort.v < 0.99) {
      LOG_ERR(0, << "PROBLEM with stamina starts! Effort = " << mdpInfo::mdp->me->effort.v);
      }*/

    //if ((mdpInfo::stamina4steps_if_dashing(power) <= 0) || //VERY EXPENSIVE

    if ((ClientOptions::consider_goalie) && (cmd_form.cmd_main.get_priority() == 1))
    {
      LOG_ERR(0, << "Goalie dashes below stamina limit!");
      LOG_POL(0, << "Goalie dashes below stamina limit!");
    }
    else
      if ((WSinfo::me->stamina <= ServerOptions::recover_dec_thr * ServerOptions::stamina_max + 12.0 - 2.0 * power) &&
          (power < 0.0) ||
          (WSinfo::me->stamina <= ServerOptions::recover_dec_thr * ServerOptions::stamina_max + 12.0 + power) &&
          (power >= 0.0))
      {
        if (power < 0.0)
        {
          power = -0.5 * mdpInfo::stamina_left();
        }
        else
        {
          power = mdpInfo::stamina_left();
        }
        LOG_ERR(0, << "Warning (client.c): No stamina left: "
                << WSinfo::me->stamina <<"  Reducing dash to " << power);
        LOG_POL(0,<<"Warning (client.c): No stamina left: "
                << WSinfo::me->stamina << " Reducing dash to " << power);

        cmd_form.cmd_main.unset_lock();
        cmd_form.cmd_main.set_dash(power);
      }
  }
}


int main_loop_via_select()
{
  MessagesInfo msg_info;
  CommandsInfo cmd_info;

  msg_info.set_cycle(-1);
  msg_info.set_ms_time(0);

  cmd_info.reset();

  WS wstate, wstate_full;

  bool got_sock_data;
  bool got_joystick_data= false;

  Msg_teamcomm2 tc;

  while (RUN::server_alive)
  {
    int retval;
    if ( WMoptions::use_joystick )
      retval= idle( 0, 
                    (WMoptions::s_time_normal_select_interval*ServerOptions::slow_down_factor), 
                    RUN::sock.socket_fd, 
                    got_sock_data, 
                    RUN::joystick.joystick_fd, 
                    got_joystick_data);
    else
      retval= idle( 0, 
                    WMoptions::s_time_normal_select_interval*ServerOptions::slow_down_factor, 
                    RUN::sock.socket_fd, 
                    got_sock_data);
    msg_info.reset();

    if ( retval == 0 && WMoptions::disconnect_if_idle )
    {
      std::cout << "\nServer " //<< udpName(RUN::server)
      << " is not responding since more then " 
      << (WMoptions::s_time_normal_select_interval*ServerOptions::slow_down_factor) 
      << " seconds";
      RUN::server_alive= false;
      continue;
    }

    if ( got_joystick_data )
    {
      receive_and_incorporate_joystick_messages(RUN::joystick, &wm, &wmfull);

      if ( ! got_sock_data)
        continue;
    }

    if ( got_sock_data )
      receive_and_incorporate_server_messages(RUN::sock, msg_info, cmd_info, &wm, &wmfull);

    MDPstate mdp, mdpfull;
    Cmd cmd;
    WSinfo::current_cmd = &cmd;
    mdpInfo::my_current_cmd = &cmd;

    //cout << "*" << flush;
    if ( !msg_info.msg[MESSAGE_SENSE_BODY].processed &&
         !msg_info.msg[MESSAGE_FULLSTATE].processed &&
         !msg_info.msg[MESSAGE_THINK].processed )
      continue;
    //cout << msg_info.msg[MESSAGE_THINK].processed  << flush;

    if ( WMoptions::behave_after_think )
    {
      if ( ! msg_info.msg[MESSAGE_THINK].processed )
      {
        //cout << "@" << flush;
        continue;
      }

      //cout << "\ngot think " << flush;

      bool have_fullstate= ( msg_info.msg[MESSAGE_FULLSTATE].cycle == msg_info.msg[MESSAGE_THINK].cycle );

      if ( WMoptions::use_fullstate_instead_of_see )
      {
        //if ( 1 || WMoptions::use_fullstate_instead_of_see ) { //never leave it in the code
        if ( ! have_fullstate )
          WARNING_OUT << ID << "no fullstate information for this cycle, using old info";
        export_mdpstate(mdp,mdpfull,0,&wmfull);
        export_ws(0,&wstate_full,0,&wmfull);
      }
      else if ( have_fullstate )
      { //just using fullstate for comparisson!
        export_mdpstate(mdp,mdpfull,&wm,&wmfull);
        export_ws(&wstate,&wstate_full,&wm,&wmfull);
      }
      else
      { //this is the most usual case
        export_mdpstate(mdp,mdpfull,&wm,0);
        export_ws(&wstate,0,&wm,0);
      }
    }
    else if ( WMoptions::behave_after_fullstate )
    { //
      if ( ! msg_info.msg[MESSAGE_FULLSTATE].processed )
        continue;
      if ( ! WMoptions::use_fullstate_instead_of_see )
        WARNING_OUT << ID << "\nuse_fullstate_instead_of_see==false doesn't make sense here";
      export_mdpstate(mdp,mdpfull,0,&wmfull);
      export_ws(0,&wstate_full,0,&wmfull);
    }
    else if ( msg_info.msg[MESSAGE_SENSE_BODY].processed )
    {
      while (true)
      {
        double my_distance_to_ball= wm.my_distance_to_ball();
        long ms_time= RUN::get_current_ms_time();
        long ms_time_we_can_still_wait= (msg_info.msg[MESSAGE_SENSE_BODY].ms_time - ms_time)
                                        * ServerOptions::slow_down_factor;
        if ( my_distance_to_ball < 5.0 )
          ms_time_we_can_still_wait += WMoptions::ms_time_max_wait_after_sense_body_long
                                       * ServerOptions::slow_down_factor;
        else
          ms_time_we_can_still_wait += WMoptions::ms_time_max_wait_after_sense_body_short
                                       * ServerOptions::slow_down_factor;

        long ms_time_till_next_see
             =   msg_info.msg[MESSAGE_SEE].ms_time 
               + (wm.ms_time_between_see_messages() * ServerOptions::slow_down_factor )
               - ms_time;

        if ( ms_time_we_can_still_wait <= 0)
          break;

        bool wait= msg_info.msg[MESSAGE_FULLSTATE].cycle >= 0   //the fullstate messages are activated
                   && msg_info.msg[MESSAGE_FULLSTATE].cycle < msg_info.msg[MESSAGE_SENSE_BODY].cycle;

        if (!wait)
        {
#if 1 //original code, everybody waits
          wait= true
#else
          wait= ( my_distance_to_ball() <  15.0
                  || ClientOptions::consider_goalie && wm.my_distance_to_ball() < 35.0
                )
#endif
                && (  ms_time_till_next_see 
                    < (ms_time_we_can_still_wait + (15*ServerOptions::slow_down_factor)) )
                   // + X*slowdownfactor as tolerance, if messages come earlier then expected
                //&& msg_info.msg[MESSAGE_SEE].cycle < msg_info.msg[MESSAGE_SENSE_BODY].cycle;
                && msg_info.msg[MESSAGE_SEE].ms_time < msg_info.msg[MESSAGE_SENSE_BODY].ms_time;

          if (!wait)
            break;
        }

        long ms_time_dum= ms_time_we_can_still_wait;
        if (    (WMoptions::ms_time_max_wait_select_interval
                 * ServerOptions::slow_down_factor) > 0 
             &&
                ms_time_dum > (WMoptions::ms_time_max_wait_select_interval
                *ServerOptions::slow_down_factor)) //just make the time slots smaller (because of system load)
          ms_time_dum= WMoptions::ms_time_max_wait_select_interval
                       * ServerOptions::slow_down_factor;

        retval= idle( ms_time_dum, 0, RUN::sock.socket_fd, got_sock_data);
        //retval= idle( ms_time_we_can_still_wait, 0, RUN::sock.socket_fd);
#if 0
        ms_time_dum= RUN::get_current_ms_time();
        if (ms_time_dum - ms_time > ms_time_we_can_still_wait )
        {
          cerr << "\nbefore idle= " << ms_time << " after= " << ms_time_dum
          << " diff= " << ms_time_dum - ms_time
          << " > " << ms_time_we_can_still_wait << "= ms_time_we_can_still_wait"
          << " retval= " << retval << std::flush;
          if (retval <= 0)
            cerr << "-------------------------";
        }
#endif
        msg_info.reset();
        if ( got_sock_data )
          receive_and_incorporate_server_messages(RUN::sock, msg_info, cmd_info, &wm, &wmfull);

        if (msg_info.msg[MESSAGE_SENSE_BODY].processed)
        {
          //never use LOG_ERR here, because mdp could be not initialized
          LOG_WM_ERR(msg_info.msg[MESSAGE_SENSE_BODY].received, 0, << "[" << WM::my_number << "] ERROR, got 2 SENSE BODY WITHOUT BEHAVE");
          std::cerr << "\nERROR, got 2 SENSE BODY WITHOUT BEHAVE" << std::flush;
          break;
        }
      } //end of while loop

      //exporting the mdpstate
      if ( msg_info.msg[MESSAGE_FULLSTATE].cycle == msg_info.msg[MESSAGE_SENSE_BODY].cycle )
      {
        //correct_wmstate_team(wm,wmfull,WMoptions::EUMMY); //debug
        //correct_wmstate_ball(wm,wmfull,WMoptions::DUMMY); //debug
        if ( wm.my_distance_to_ball() <= 3.0)
          if (msg_info.msg[MESSAGE_SEE].cycle == msg_info.msg[MESSAGE_SENSE_BODY].cycle)
          {
            LOG_WM(wm.time.time,0, << "GOT_SEE 1");
          }
          else
            LOG_WM(wm.time.time,0, << "GOT_SEE 0");
        wm.compare_with_wmstate(wmfull);
#if 1   //this is the correct setting!!!
        export_mdpstate(mdp,mdpfull,&wm,&wmfull);
        export_ws(&wstate,&wstate_full,&wm,&wmfull);
#else   //just DEBUGGING, never leave it so to the repository
        if ( true || msg_info.msg[MESSAGE_SEE].cycle == msg_info.msg[MESSAGE_SENSE_BODY].cycle )
        {
          export_mdpstate(mdp,mdpfull,&wmfull,&wmfull);
          export_ws(&wstate,&wstate_full,&wmfull,&wmfull);
        }
        else
        {
          export_mdpstate(mdp,mdpfull,&wm,&wmfull);
          export_ws(&wstate,&wstate_full,&wm,&wmfull);
        }
#endif

      }
      else
      {
        export_mdpstate(mdp,mdpfull,&wm,0);
        export_ws(&wstate,0,&wm,0);
      }
    }
    else
      continue;

    //the rest of the loop performs a behave of the agent
    wm.show_object_info();
    long ms_time= RUN::get_current_ms_time();	//actualize current time
    MessagesTimes::add_before_behave(WSinfo::ws->time,ms_time);

    //some extra time information for the behave (a hack, should happen in the export methods)
    long dum= msg_info.msg[MESSAGE_SENSE_BODY].ms_time;
    mdp.ms_time_of_sb= dum;
    mdpfull.ms_time_of_sb= dum;
    wstate.ms_time_of_sb= dum;
    wstate_full.ms_time_of_sb= dum;

    //cout << "\nmdp.time " << mdp.time_current << "  play_mode= " << play_mode_str(mdp.play_mode);

    WSmemory::update();

    /* we still need MDPmemory... */
    RUN::mdp_memory->update();

    /* Sput03: we need to call view strategy first! */
    /* Because the old view strategy is too deeply nested with Bs2kAgent, it is better
       to convert it into a behavior structure. Since the later-to-be-used mechanism for view
       and neck behaviors has not yet been determined, this may only be a hack for now.
    */
    /* Some old policies reset the cmd, so we need to preserve its state...
       We should be able to remove this hack after switching completely to behaviors. */
    Cmd view_cmd;
    if( RUN::view_controller)
    {
      RUN::view_controller->get_cmd(view_cmd);
    }
    int view_ang,view_qual;
    bool view_set = view_cmd.cmd_view.is_cmd_set();
    if(view_set) view_cmd.cmd_view.get_angle_and_quality(view_ang,view_qual);
    //if(ClientOptions::consider_goalie != true){
    // ridi03: currently this does not work for the goalie
    // sput03: no longer true... at least for Bs03
    if ( RUN::behavior_controller)
      RUN::behavior_controller->get_cmd(cmd); // test if behavior feels responsible for this situation
    //}
    /* D
    if ( RUN::controller) {
    RUN::controller->behave(mdp,cmd); // compute command
    }*/


    check_reduce_dash_power(cmd);


    /* see above */
    if(view_set) cmd.cmd_view.set_angle_and_quality(view_ang,view_qual);

    /* Sput03: Now we need to call our neck behavior.
       
    */

    if( RUN::neck_controller )
    {
      RUN::neck_controller->get_cmd(cmd);
    }

    if( RUN::attentionto_controller )
    {
      RUN::attentionto_controller->get_cmd(cmd);
    }

    ms_time= RUN::get_current_ms_time();  //actualize current time
    MessagesTimes::add_after_behave(WSinfo::ws->time,ms_time);
    if ( msg_info.msg[MESSAGE_FULLSTATE].cycle == msg_info.msg[MESSAGE_SENSE_BODY].cycle )
      RUN::wm_compare.add(wm,wmfull);
#if 0
    cout << "\n$$$$$$$$$$$$$$$$$$$$\n[  " << WSinfo::ws->time
    << "  m_pos= " << mdpInfo::my_pos_abs()
    << "  m_vel= " << mdpInfo::my_vel_abs()
    << "  m_ang= " << mdpInfo::mdp->me->ang.v
    << "  b_pos= " << mdpInfo::mdp->ball.pos()
    << "  b_vel= " << mdpInfo::mdp->ball.vel()
    << "\n  b_pos_rel= " << mdpInfo::mdp->ball.pos()-mdpInfo::my_pos_abs()
    << " b_dist= " << (mdpInfo::mdp->ball.pos()-mdpInfo::my_pos_abs()).norm()
    << " |b_vel|= " << (mdpInfo::mdp->ball.vel()).norm()
    << " ]";
    debug_1or2_Step_Kick(cmd);
#endif
    last_cmd= cmd;

    long ms_time_dum= RUN::get_current_ms_time();

#if 0
    long ms_time_left
      =   (100*ServerOptions::slow_down_factor) 
        - (10 *ServerOptions::slow_down_factor) 
        - (ms_time_dum - msg_info.msg[MESSAGE_SENSE_BODY].ms_time);
    if ( ms_time_left < 0)
      ms_time_left= 0;
#endif

#ifdef CMD_VIEW_HACK
    if ( ! WMoptions::behave_after_think ) //do not use view mode synchronization in synch_mode
      if ( cmd.cmd_view.is_cmd_set()
           // && msg_info.msg[MESSAGE_SEE].cycle != msg_info.msg[MESSAGE_SENSE_BODY].cycle ) {
           && msg_info.msg[MESSAGE_SEE].ms_time < msg_info.msg[MESSAGE_SENSE_BODY].ms_time)
      {
        cmd_view_hack= cmd.cmd_view;
        cmd.cmd_view.unset_lock();
        cmd.cmd_view.unset_cmd();
      }
#endif
    //cmd.cmd_say.set_pass(Vector(10,10), Vector(2.1,1.22), wm.time.time+100); //test!!!!
    wm.export_msg_teamcomm(tc, cmd.cmd_say );

    //////send_cmd(RUN::sock,cmd, ms_time_left, msg_info.msg[MESSAGE_SENSE_BODY].ms_time+ 95);
    send_cmd(RUN::sock,cmd,tc);//, tc, 0, msg_info.msg[MESSAGE_SENSE_BODY].ms_time+ 95); //never wait with say messages

    cmd_info.set_command(WSinfo::ws->time, cmd);
    wm.reduce_dash_power_if_possible(cmd.cmd_main);
    wm.incorporate_cmd(cmd);


    //idle(100); /////////////////TEST
    //send_cmd(RUN::sock,cmd, ms_time_left);	  //execute command

    if ( ms_time_dum - ms_time > (40*ServerOptions::slow_down_factor))
      LOG_ERR(0,<< " Player " << WM::my_number << ": ms_time in behave = " << ms_time_dum- ms_time);

    if (    !WMoptions::ignore_sense_body 
         && ms_time_dum - msg_info.msg[MESSAGE_SENSE_BODY].ms_time 
            > (80*ServerOptions::slow_down_factor))
      LOG_ERR(0,<< " Player " << WM::my_number << ": ms_time since sense_body " << ms_time_dum - msg_info.msg[MESSAGE_SENSE_BODY].ms_time);

    if (    WMoptions::behave_after_fullstate 
         && ms_time_dum - msg_info.msg[MESSAGE_FULLSTATE].ms_time 
            > (80*ServerOptions::slow_down_factor))
      LOG_ERR(0,<< " Player " << WM::my_number << ": ms_time since fullstate " << ms_time_dum - msg_info.msg[MESSAGE_FULLSTATE].ms_time);
  }

  return 0;
}


#if 1
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

// time based version

#include <signal.h>
#include <fcntl.h>

namespace RUN
{
  sigset_t sigiomask, sigalrmask;
  long ms_timer_cycle= 10;
  long ms_time_of_last_message= 0;
  int server_time= -10;
  int timer_step= 0;
}

int receive_messages(UDPsocket & sock, long ms_time)
{
  int num_messages= 0;
  int num_bytes;

  if ( ms_time < 0)
    ms_time= RUN::get_current_ms_time();

  while ( RUN::sock.recv_msg( buffer,num_bytes) )
  { //read all messages from the socket and incorporate them!

    buffer[num_bytes] = '\000';
    //cout << "\ngot=[" << buffer<< "]";
    num_messages++;

    //ms_time= RUN::get_current_ms_time(); //seems to be better if time is measured every time

    int type, time;
    bool res= SensorParser::get_message_type_and_time(buffer,type,time);

    if (!res)
    {
      ERROR_OUT << ID << "\nwrong message type " << buffer;
      return -1;
    }
    if ( type == MESSAGE_SENSE_BODY )
    {
      RUN::timer_step= 0;
      RUN::server_time= time;
    }
    RUN::ms_time_of_last_message= ms_time;
    MessagesTimes::add(time,ms_time,type);
  }
  return num_messages;
}

/* set time interval between the sensor receiving and command sending */
inline void set_timer()
{
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = RUN::ms_timer_cycle * 1000;
  setitimer(ITIMER_REAL, &itv, NULL);
}

inline void set_timer(int usec)
{
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = usec;
  setitimer(ITIMER_REAL, &itv, NULL);
}

/* suspend the process until one of the signals comes through */
/* could check for situation to kill client, return false     */
/* i.e. too many actions with no sensory input coming in      */
bool wait_for_signals(sigset_t *mask)
{
  sigsuspend(mask);
  return true;
}

/* SIGIO handler: receive and parse messages from server */
void sigio_handler()
{
  //sigprocmask(SIG_BLOCK, &RUN::sigalrmask, NULL);

  //no longer used, we now use the select mechanism

  //sigprocmask(SIG_UNBLOCK, &RUN::sigalrmask, NULL);
}



/* SIGALRM handler: extract and send first command in commandlist */
void sigalrm_handler()
{
  //sigprocmask(SIG_BLOCK, &RUN::sigiomask, NULL);

  RUN::timer_step++;
  long ms_time= RUN::get_current_ms_time();
  MessagesTimes::add(RUN::server_time,ms_time,MESSAGE_DUMMY1,RUN::timer_step);

  receive_messages(RUN::sock,ms_time);

  if ( RUN::timer_step == 5 )
  {
    MessagesTimes::add_before_behave(RUN::server_time,ms_time);

    if (1)
      for (int i=0; i<10000; i++)
      {
        Vector v(i,i);
        double a= v.arg();
        v.init_polar(1,a);
      }
    ms_time= RUN::get_current_ms_time();
    MessagesTimes::add_after_behave(RUN::server_time,ms_time);
  }
  if (  ms_time - RUN::ms_time_of_last_message > 5000 )
  {
    std::cout << "\nServer " //<< udpName(RUN::server)
    << " is not responding since more then 5 sec";
    RUN::server_alive= false;
  }

  //sigprocmask(SIG_UNBLOCK, &RUN::sigiomask, NULL);
}
/****************************************************************************************/




sigset_t init_handler()
{
  sigemptyset(&RUN::sigalrmask);
  sigaddset(&RUN::sigalrmask, SIGALRM);
  sigemptyset(&RUN::sigiomask);
  sigaddset(&RUN::sigiomask, SIGIO);

  ////////////////////////////
  struct sigaction sigact;
  sigact.sa_flags = 0;

  ////////////////////////////
  sigact.sa_mask = RUN::sigiomask; //RUN::sigiomask will be blocked during execution of SIGALRM signals
  sigact.sa_handler = (void (*)(int))sigalrm_handler;
  sigaction(SIGALRM, &sigact, NULL);

  ////////////////////////////
  sigact.sa_mask = RUN::sigalrmask;  //RUN::sigalrmask will be blocked during execution of SIGIO signals
  sigact.sa_handler = (void (*)(int))sigio_handler;
  sigaction(SIGIO, &sigact, NULL);

  ////////////////////////////
  set_timer();


  sigprocmask(SIG_UNBLOCK, &RUN::sigiomask, NULL);
  sigprocmask(SIG_UNBLOCK, &RUN::sigalrmask, NULL);

  sigset_t sigsetmask;
  sigprocmask(SIG_BLOCK, NULL, &sigsetmask);   /* Get's the currently unblocked signals */

  //

  return sigsetmask;
}


void usage()
{
  //...

  RUN::sock.set_fd_nonblock();

  RUN::sock.set_fd_sigio();  //  bind sigio to file descriptor
  sigset_t sigfullmask = init_handler();
  while ( RUN::server_alive && wait_for_signals(&sigfullmask) )
    ; //noop
}
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/
#endif


int main(int argc, char *argv[])
{
#if 0
  //const char * const msg[3]= { "N5XN5XN5XS","W56M86ASRK","" };
  const int num= 4;
  char *  msg[num]= { "N5XN5XN5XS","W56M86ASRK","Io91073331","Ip92073231" };
  for (int i=0; i<num;i++ )
  {
    std::cout << "\n=======\nmsg=[" << msg[i] << "]";
    Msg_teamcomm2 tc;
    const char * dum;
    tc.reset();
    bool res= SensorParser::manual_parse_teamcomm(msg[i],tc,dum);
    std::cout << "\nres= " << res
    << "\n   tc=" << tc;
  }
  return 1;
#endif

  std::cout << std::endl << "[CommandLineOptions";
  CommandLineOptions::init_options();
  CommandLineOptions::read_from_command_line(argc,argv);
  std::cout << "|WMoptions";
  WMoptions::init_options();
  //WMoptions::read_from_command_line(argc,argv);
  WMoptions::read_options(CommandLineOptions::agent_conf,argc,argv);

  // init default values for agent
  std::cout << "|ClientOptions";
  ClientOptions::init();
  // read agent's default parameter file
  ClientOptions::read_from_file( CommandLineOptions::agent_conf );
  ClientOptions::read_from_command_line(argc,argv);
  strncpy(WM::my_team_name,ClientOptions::teamname,20);
  WM::my_team_name_len= strlen(WM::my_team_name);

  MessagesTimes::init(WMoptions::save_msg_times);


  // init default values for server

  ServerOptions::init();
  // read server configuration from file
  if (ClientOptions::server_version >= 8.00)
    std::cout << "]";
  else
  {
    ServerOptions::read_from_file( CommandLineOptions::server_conf);
    std::cout << "|ServerOptions]";
  }
  std::cout << " ok!" << std::endl;

  CommandLineOptions::print_options();

  if (WMoptions::use_joystick)
    if ( ! RUN::joystick.init(WMoptions::joystick_device) )
    {
      ERROR_OUT << ID << "\ncould not initialize joystick device: " << WMoptions::joystick_device << std::endl;
      return 1;
    }
    else
      std::cout << "\n[Joystick] ok";

  bool res= true;
  res= res && RUN::sock.init_socket_fd();
  res= res && RUN::sock.init_serv_addr(CommandLineOptions::host,CommandLineOptions::port);
  if (!res)
  {
    ERROR_OUT << ID << "\nProblems with initialization connection to  " << CommandLineOptions::host << " at port " << CommandLineOptions::port;
    exit(1);
  }

  if ( WMoptions::DUMMY || WMoptions::EUMMY)
  {
    std::cout << "\n******** wm_DUMMY= " << WMoptions::DUMMY;
    std::cout << "\n******** wm_EUMMY= " << WMoptions::EUMMY << std::flush;
  }
#if 0
  if ( WMoptions::ms_time_max_wait_after_sense_body_long > 0 )
  {
    std::cout << "\n******** ms_time_max_wait_after_sense_body_long = " << WMoptions::ms_time_max_wait_after_sense_body_long
    << "\n******** ms_time_max_wait_after_sense_body_short= " << WMoptions::ms_time_max_wait_after_sense_body_short << std::flush;
  }
#endif
  send_initialize_message(RUN::sock,ClientOptions::teamname,ClientOptions::consider_goalie);
  PlayMode pm;
  res= recv_initialize_message(RUN::sock, pm);
  if (!res)
  {
    std::cerr << "\nProblems with initialization connection  to " << CommandLineOptions::host;
    exit(1);
  }
  ClientOptions::player_no = WM::my_number;


  if (WMoptions::his_goalie_number > 0 && WMoptions::his_goalie_number <= 11)
  {
    wm.his_goalie_number= WMoptions::his_goalie_number;
    wmfull.his_goalie_number= WMoptions::his_goalie_number;
    std::cout << "\nset his_goalie_number to " << WMoptions::his_goalie_number << std::flush;
  }

  if (ClientOptions::server_version >= 8.00)
  {
    res= recv_parameter_messages(RUN::sock);
    //res= produce_parameter_messages(RUN::sock,std::cout);
    //std::cout << "\nres= " << res;
    //exit(0);
    if (!res)
    {
      ERROR_OUT << ID << "\nProblems with initialization of server parameters";
      exit(1);
    }
    res= ServerParam::export_server_options();

    if (!res)
    {
      ERROR_OUT << ID << "\nProblems with exporting server options";
      exit(1);
    }
    std::cout << "\n[ServerOptions] ok!";
  }
  //std::cout << "\nServerOptions::player_size= " << ServerOptions::player_size;

  //RUN::wm_compare.init();
  wmfull.init();
  wm.init(); //call this afer the WM::my_number is known!
  wm.play_mode= pm;



  LogOptions::init();
  LogOptions::read_from_command_line(argc,argv);
  char side_chr= 'l';
  if (left_SIDE != WM::my_side)
    side_chr= 'r';

  sprintf(LogOptions::log_fname,"%s/%s%d-%c-actions.log",LogOptions::log_dir, ClientOptions::teamname, ClientOptions::player_no,side_chr);
  LogOptions::open();

  std::cout << std::endl << "[mdpInfo";
  mdpInfo::init();
  std::cout << "|WSmemory";
  WSmemory::init();
  std::cout << "|Blackboard";
  Blackboard::init();
  /* D
  std::cout << "|Move_Factory";
  Move_Factory::init();
  std::cout << "|Policy_Factory";
  Policy_Factory::init();
  std::cout << "|Strategy_Factory";
  Strategy_Factory::init();
  */
  std::cout << "|Policy_Tools";
  Policy_Tools::init_params();
  std::cout << "|Planning";
  Planning::init_params();
  std::cout << "|AbstractMDP]";
  AbstractMDP::init_params();
  std::cout << "|DeltaPositioning]";
  DeltaPositioning::init_formations();
  std::cout << " ok!" << std::endl;

  // important: init the agent after reading client options
  //res= BehaviorSet::init(CommandLineOptions::agent_conf,argc,argv);
  //if ( ! res ) return -1;

  /* D
  if ( ClientOptions::strategy_type < 0 )
    RUN::controller= 0;
  else
    RUN::controller = new Bs2kAgent(); 
  */

  /* Sput: We still need MDPmemory, and we need it in client.c... */
  RUN::mdp_memory = new MDPmemory();

  res= init_behavior(ClientOptions::behavior, RUN::behavior_controller,
                     CommandLineOptions::agent_conf,argc,argv);
  if ( ! res )
    return -1;

  res= init_neck_behavior(RUN::neck_controller,
                          CommandLineOptions::agent_conf,argc,argv);
  if(!res) return -1;
  res= init_view_behavior(RUN::view_controller,
                          CommandLineOptions::agent_conf,argc,argv);
  if(!res) return -1;

  res= init_attentionto_behavior(RUN::attentionto_controller,
                                 CommandLineOptions::agent_conf,argc,argv);
  if(!res) return -1;


  if ( WMoptions::offline )
  {
    if ( ! RUN::behavior_controller )
    {
      INFO_OUT << ID << "\nneed a behavior in offline mode";
      return -1;
    }
    WMstate wm;
    WS wstate;
    export_ws(&wstate,0,&wm,0);
    Cmd cmd;
    bool res= RUN::behavior_controller->get_cmd(cmd);
    INFO_OUT << ID << "\nresult of offline mode execution " << res << "\n";
    return 0;
  }


  //D if ( ! RUN::controller && ! RUN::behavior_controller ) {
  if ( ! RUN::behavior_controller )
  {
    INFO_OUT << ID << "\nno behavior specified";
    return -1;
  }



  RUN::sock.set_fd_nonblock();

  {
    const char msg[]= "(clang (ver 7 8))";  //this is important to get coach clang messages (from server ver. 8.04 rel 5)
    RUN::sock.send_msg(msg,strlen(msg)+1);
  }


  //new version, uses select and no alarm/io handlers
  if ( WMoptions::DUMMY == 0 )
  {
    if (RUN::server_alive)
      main_loop_via_select(); //<-----------
  }
  /****************************************************************************************/
  else
  { //timer version!!!
    //RUN::sock.set_fd_nonblock();

    RUN::sock.set_fd_sigio();  //  bind sigio to file descriptor
    sigset_t sigfullmask = init_handler();
    while ( RUN::server_alive && wait_for_signals(&sigfullmask) )
      ;//noop
  }
  /****************************************************************************************/


  RUN::sock.send_msg("(bye)",6);
  //std::cout << "\nShutting down..." << std::endl; // player " << WM::my_number << std::endl;
  mdpInfo::print_mdp_statistics();

#if 0
  // dietrich
  One_vs_Goalie_Policy2* tempPolicy = (One_vs_Goalie_Policy2*) Policy_Factory::get_One_vs_Goalie_Policy2();
  tempPolicy->shutdown();
#endif

  if (WMoptions::save_msg_times)
  {
    char buf[256];
    sprintf(buf,"%s2",LogOptions::log_fname);
    //sprintf(buf,"%s%d_%d.msg",WM::my_number, WM::my_side);
    MessagesTimes::save(buf);
  }

  /*
  if(LogOptions::log_int) {
    IntentionLogger::save_all_until_now();
  }
  */
  LogOptions::close();
  std::cout << "\nShutting down player " << WM::my_number << "\n" << std::endl << std::flush;
  //RUN::wm_compare.show(std::cout);
}



#if 1
void send_cmd_aserver_ver_7(UDPsocket & sock, const Cmd & cmd)
{ //, long ms_time_left= 0, long ms_time_latest_limit= -1) {
  static char buf_main[RUN::MaxStringSize] ;
  static char buf_neck[RUN::MaxStringSize] ;
  static char buf_view[RUN::MaxStringSize] ;
  static char buf_say[RUN::MaxStringSize] ;
  std::ostrstream o_main(buf_main,RUN::MaxStringSize);
  std::ostrstream o_neck(buf_neck,RUN::MaxStringSize);
  std::ostrstream o_view(buf_view,RUN::MaxStringSize);
  std::ostrstream o_say(buf_say,RUN::MaxStringSize);

  double par1,par2;
  Angle ang;

  /***************************************************************************/
  //interpret main command, i.e. one of {moveto,turn,dash,kick,catch}
  if ( cmd.cmd_main.is_cmd_set() )
  {
    const Cmd_Main& action= cmd.cmd_main; //shortcut
    switch ( action.get_type())
    {
    case action.TYPE_MOVETO   :
      action.get_moveto(par1,par2);
      o_main << "(move " <<  x_LP_2_SRV( par1 ) << " " << y_LP_2_SRV( par2 ) << ")" << std::ends;
      break;
    case action.TYPE_TURN 	 :
      action.get_turn(ang);
      o_main << "(turn " << ang_LP_2_SRV_deg( ang ) << ")" << std::ends;
      //LOG_WM(wm.time.time,0,"SENDING TURN " << buf_main);
      //std::cout << "\n got turn " << ang <<" and transformed in " << ang_LP_2_SRV_deg( ang );
      break;
    case action.TYPE_DASH 	 :
      action.get_dash(par1);
      o_main << "(dash " << par1 << ")" << std::ends;
      break;
    case action.TYPE_KICK 	 :
      action.get_kick(par1,ang);
      o_main << "(kick " << par1 << " " << ang_LP_2_SRV_deg( ang ) << ")" << std::ends;
      break;
    case action.TYPE_CATCH    :
      action.get_catch( ang );
      o_main << "(catch " << ang_LP_2_SRV_deg( ang ) << ")" << std::ends;
      break;
    case action.TYPE_TACKLE 	 :
      action.get_tackle(par1);
      o_main << "(tackle " << par1 << ")";
      break;
    default:
      ERROR_OUT << ID << "\nwrong command";
    }
  }
  else
  { //es wurde kein Kommando gesetzt, evtl. warnen!!!
    o_main << "(turn 0)" << std::ends; //als debug info verwendbar
    //ERROR_OUT << ID << "\nno command was specified, sending (turn 0)";
  }
  /***************************************************************************/
  //interpret neck command
  if ( cmd.cmd_neck.is_cmd_set())
  {
    cmd.cmd_neck.get_turn( ang );
    o_neck << "(turn_neck " << ang_LP_2_SRV_deg( ang ) << ")" << std::ends;
  }
  /***************************************************************************/
  //interpret view command
  if (cmd.cmd_view.is_cmd_set())
  {
    const Cmd_View &view= cmd.cmd_view;
    int va,vq;
    o_view << "(change_view ";
    view.get_angle_and_quality(va,vq);
    if ( view.VIEW_ANGLE_WIDE == va )
      o_view << "wide";
    else if ( view.VIEW_ANGLE_NORMAL == va )
      o_view << "normal";
    else
      o_view << "narrow";
    o_view << " ";
    if ( view.VIEW_QUALITY_HIGH == vq)
      o_view << "high";
    else
      o_view << "low";
    o_view << ")" << std::ends;
  }

  /***************************************************************************/
  //interpret say command
  if ( cmd.cmd_say.is_cmd_set() )
  {
  }
  /***************************************************************************/
  // send  the data
  //sock.send_msg("(turn 90)", strlen("(turn 90)"+1));
  //return;

  if (o_main.pcount() > 0)
  {
    //LOG_WM(wmfull.time.time,0,<< "SENDING COMMAND:" << o_main.str());
    sock.send_msg( o_main.str(), o_main.pcount() );
  }
  if (o_neck.pcount() > 0)
  {
    sock.send_msg( o_neck.str(), o_neck.pcount());
    //cerr << "\ntest: don't send turn neck"; *************************************
  }
  if (o_view.pcount() > 0)
  {
    sock.send_msg( o_view.str(), o_view.pcount());
  }
  if (o_say.pcount() > 0)
  {
#if 0
    bool send= true;
    if ( cmd.cmd_say.get_priority() != 0)
    {
      idle(ms_time_left);
      long ms_time= RUN::get_current_ms_time();
      if ( ms_time > ms_time_latest_limit)
        send= false;
    }
    if (send)
      sock.send_msg( o_say.str(), o_say.pcount());

#ifdef TEST_SAY_MECHANISM
    long ms_time= RUN::get_current_ms_time();
    int param= cmd.cmd_say.get_priority();
    if (!send)
      param+= 4;
    MessagesTimes::add(wm.time.time,ms_time,MESSAGE_DUMMY1, param);
#endif
#endif

  }
}
#endif


#if 0
/*******************************************************************************
the code below is the old code when 2 timers were used to control the run time 
behavior of the agent. This is no longer used, but interesting on its own
 
*******************************************************************************/
#include <signal.h>
#include <fcntl.h>

namespace RUN
{
  sigset_t sigiomask, sigalrmask;
  long ms_timer_cycle= 10;
  long ms_time_of_last_message= 0;
}

/* set time interval between the sensor receiving and command sending */
inline void set_timer()
{
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = RUN::ms_timer_cycle * 1000;
  setitimer(ITIMER_REAL, &itv, NULL);
}

inline void set_timer(int usec)
{
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = usec;
  setitimer(ITIMER_REAL, &itv, NULL);
}

/* suspend the process until one of the signals comes through */
/* could check for situation to kill client, return false     */
/* i.e. too many actions with no sensory input coming in      */
bool wait_for_signals(sigset_t *mask)
{
  sigsuspend(mask);
  return true;
}

/* SIGIO handler: receive and parse messages from server */
void sigio_handler()
{
  //sigprocmask(SIG_BLOCK, &RUN::sigalrmask, NULL);

  //no longer used, we now use the select mechanism

  //sigprocmask(SIG_UNBLOCK, &RUN::sigalrmask, NULL);
}


/* SIGALRM handler: extract and send first command in commandlist */
void sigalrm_handler()
{
  //sigprocmask(SIG_BLOCK, &RUN::sigiomask, NULL);

  long ms_time= RUN::get_current_ms_time();
  MessagesTimes::add_
  receive_messages(RUN::sock,ms_time);


  if (  ms_time - RUN::ms_time_of_last_message > 5000 )
  {
    std::cout << "\nServer " //<< udpName(RUN::server)
    << " is not responding since more then 5 sec";
    RUN::server_alive= false;
  }

  //sigprocmask(SIG_UNBLOCK, &RUN::sigiomask, NULL);
}
/****************************************************************************************/


int receive_messages(UDPsocket & sock, long ms_time)
{
  int num_messages= 0;
  int num_bytes;

  if ( ms_time < 0)
    ms_time= RUN::get_current_ms_time();

  while ( RUN::sock.recv_msg( buffer,num_bytes) )
  { //read all messages from the socket and incorporate them!
    buffer[num_bytes] = '\000';
    //std::cout << "\ngot=[" << buffer<< "]";
    num_messages++;

    //ms_time= RUN::get_current_ms_time(); //seems to be better if time is measured every time

    int type, time;
    bool res= SensorParser::get_message_type_and_time(buffer,type,time);

    if (!res)
    {
      ERROR_OUT << ID << "\nwrong message type " << buffer;
      return -1;
    }

    MessagesTimes::add(time,ms_time,type);
  }
}


sigset_t init_handler()
{
  sigemptyset(&RUN::sigalrmask);
  sigaddset(&RUN::sigalrmask, SIGALRM);
  sigemptyset(&RUN::sigiomask);
  sigaddset(&RUN::sigiomask, SIGIO);

  ////////////////////////////
  struct sigaction sigact;
  sigact.sa_flags = 0;

  ////////////////////////////
  sigact.sa_mask = RUN::sigiomask; //RUN::sigiomask will be blocked during execution of SIGALRM signals
  sigact.sa_handler = (void (*)(int))sigalrm_handler;
  sigaction(SIGALRM, &sigact, NULL);

  ////////////////////////////
  sigact.sa_mask = RUN::sigalrmask;  //RUN::sigalrmask will be blocked during execution of SIGIO signals
  sigact.sa_handler = (void (*)(int))sigio_handler;
  sigaction(SIGIO, &sigact, NULL);

  ////////////////////////////
  set_timer();


  sigprocmask(SIG_UNBLOCK, &RUN::sigiomask, NULL);
  sigprocmask(SIG_UNBLOCK, &RUN::sigalrmask, NULL);

  sigset_t sigsetmask;
  sigprocmask(SIG_BLOCK, NULL, &sigsetmask);   /* Get's the currently unblocked signals */

  //

  return sigsetmask;
}


void usage()
{
  //...

  RUN::sock.set_fd_nonblock();

  RUN::sock.set_fd_sigio();  //  bind sigio to file descriptor
  sigset_t sigfullmask = init_handler();
  while ( RUN::server_alive && wait_for_signals(&sigfullmask) )
    ; //noop
}
/****************************************************************************************/
#endif
