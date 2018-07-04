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
 * Author:   Artur Merke 
 */
#include "sensorparser.h"
#include "wmtools.h"
#include "wmoptions.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "str2val.h"
#include "server_options.h"
#include "macro_msg.h"
#include "bit_fifo.h"

//#include "log_macros.h" //debug

bool SensorParser::get_message_type_and_time(const char * str, int & type, int & time) {
  const char * dum;

  if (strncmp(str,"(sense_body ", 12) == 0) {
    str+= 12;
    type= MESSAGE_SENSE_BODY;
    if (! str2val(str,time,dum) )
      return false;
    return true;
  }

  if (strncmp(str,"(see ", 4) == 0) {
    str+= 4;
    type= MESSAGE_SEE;
    if (! str2val(str,time,dum) )
      return false;
    return true;
  }

  if (strncmp(str,"(fullstate ", 11) == 0) {
    str+= 11;
    type= MESSAGE_FULLSTATE;
    if (! str2val(str,time,dum) )
      return false;
    return true;
  }

  if (strncmp(str,"(think)", 7) == 0) {
    str+= 11;
    type= MESSAGE_THINK;
    time= -1; //no time is provided with the think message
    return true;
  }

  if (strncmp(str,"(hear ", 6) == 0) {
    str+= 6;
    type= MESSAGE_HEAR;
    if (! str2val(str,time,dum) )
      return false;
    return true;
  }

  if (strncmp(str,"(change_player_type ", 20) == 0) {
    type= MESSAGE_CHANGE_PLAYER_TYPE;
    time= -1;
    return true;
  }

  if (strncmp(str,"(init ", 6) == 0) {
    type= MESSAGE_INIT;
    time= -1;
    return true;
  }

  if (strncmp(str,"(server_param ", 14) == 0) {
    type= MESSAGE_SERVER_PARAM;
    time= -1;
    return true;
  }

  if (strncmp(str,"(player_param ", 14) == 0) {
    type= MESSAGE_PLAYER_PARAM;
    time= -1;
    return true;
  }

  if (strncmp(str,"(player_type ", 13) == 0) {
    type= MESSAGE_PLAYER_TYPE;
    time= -1;
    return true;
  }

  if (strncmp(str,"(ok ", 3) == 0) {
    type= MESSAGE_OK;
    time= -1;
    return true;
  }

  if (strncmp(str,"(error ", 7) == 0) {
    type= MESSAGE_ERROR;
    time= -1;
    return true;
  }

  type= MESSAGE_UNKNOWN;
  return false;
}

//////////////////////////////////////////////////////////////////////
bool SensorParser::manual_parse_fullstate(const char* str, Msg_fullstate & fullstate) {
  fullstate.reset();

  if ( strncmp("(fullstate",str,10)!= 0 ) 
    return false;

  char* next;
  str += 10;  
  fullstate.time= strtol(str,&next,10);
  str= next;

  fullstate.players_num= 0;
  while(true) {
    while(str[0]==' ') 
      str++;
    if ( str[0] == ')' )
      return true;
    if ( str[0] != '(') 
      return false;
    str ++;
      
    if ( (str[0]== 'l' || str[0]== 'r') && str[1]== '_' ) {
      Msg_fullstate::_fs_player & player= fullstate.players[fullstate.players_num];
      fullstate.players_num++;
      if ( str[0]== 'l' && WM::my_side== left_SIDE 
	   || str[0]== 'r' && WM::my_side== right_SIDE) {
	player.team= my_TEAM;
      }
      else
	player.team= his_TEAM;

      str += 2;
      player.number= strtol(str,&next,10);
      str = next;
      player.x= strtod(str,&next);
      str = next;
      player.y= strtod(str,&next);
      str = next;
      player.vel_x= strtod(str,&next);
      str = next;
      player.vel_y = strtod(str,&next);
      str = next;
      player.angle = strtod(str,&next);
      str = next;
      player.neck_angle = strtod(str,&next);
      str = next;
      player.stamina = strtod(str,&next);
      str = next;
      player.effort = strtod(str,&next);
      str = next;
      player.recovery = strtod(str,&next);
      str = next;
      if (str[0]== ')')
	str++;
      else
	return false;
    }
    else if ( strncmp("ball ",str,5) == 0 ) {
      str += 5;
      fullstate.ball.x = strtod(str,&next);
      str = next;
      fullstate.ball.y = strtod(str,&next);
      str = next;
      fullstate.ball.vel_x = strtod(str,&next);
      str = next;
      fullstate.ball.vel_y = strtod(str,&next);
      str = next;
      if (str[0]== ')')
	str++;
      else
	return false;
    }
    else if ( strncmp("score ",str,6) == 0 ) {
      str += 6;
      fullstate.my_score= strtol(str,&next,10);
      str = next;
      fullstate.his_score= strtol(str,&next,10);
      str = next;
      if (WM::my_side == right_SIDE) {
	int tmp = fullstate.my_score;
	fullstate.my_score=  fullstate.his_score;
	fullstate.his_score= tmp;
      }
	  
      if (str[0]== ')')
	str++;
      else
	return false;
    }
    else if ( strncmp("vmode ",str,6) == 0 ) {
      str += 6;
      const char * dum;
      if ( !manual_parse_view_mode(str,fullstate.view_quality, fullstate.view_width, dum) ) {
	ERROR_OUT << ID << "\nparse error " << str;
	return false;
      }
      str= dum;

      while ( str[0] == ' ' ) 
	str++;

      if (str[0]== ')')
	str++;
      else
	return false;
    }
    else if ( strncmp("pmode ",str,6) == 0 ) {
      str += 6;      /* 1234567890123456789 */
      PlayMode pm;
      const char * dum;
      int dum_int;
      if ( !manual_parse_play_mode(str,pm,dum_int,dum) ) {
	ERROR_OUT << ID << "\nparse error " << str;
	return false;
      }
      str= dum;
      fullstate.play_mode= pm;

      while ( str[0] == ' ' ) 
	str++;

      if (str[0]== ')')
	str++;
      else
	return false;
    }
  }  
  // never reaches this point
  return false;
}

//////////////////////////////////////////////////////////////////////
bool SensorParser::manual_parse_fullstate(const char* str, Msg_fullstate_v8 & fullstate) {
  fullstate.reset();
  
  bool res= true;
  if ( strncmp("(fullstate",str,10)!= 0 ) 
    return false;

  str += 10;  
  const char * dum= str;
  const char * dum2;
  int dum_int;

  res = res && 
    str2val(dum,fullstate.time,dum) &&
    //recognizing play mode
    strskip(dum,"(pmode",dum) &&
    manual_parse_play_mode(dum,fullstate.play_mode,dum_int,dum) &&
    strskip(dum,')',dum) &&
    //recognizing visual mode
    strskip(dum,"(vmode",dum) &&
    manual_parse_view_mode(dum,fullstate.view_quality, fullstate.view_width, dum) &&
    strskip(dum,')',dum) &&
#if 0
    //skipping stamina
    strskip(dum,"(stamina",dum) &&
    str2val(dum,dum_double,dum) &&
    str2val(dum,dum_double,dum) &&
    str2val(dum,dum_double,dum) &&
    strskip(dum,')',dum) &&
#endif
    //recognizing count
    strskip(dum,"(count",dum) &&
    str2val(dum,fullstate.count_kick,dum) &&
    str2val(dum,fullstate.count_dash,dum) &&
    str2val(dum,fullstate.count_turn,dum) &&
    str2val(dum,fullstate.count_catch,dum) &&
    str2val(dum,fullstate.count_kick,dum) &&
    str2val(dum,fullstate.count_turn_neck,dum) &&
    str2val(dum,fullstate.count_change_view,dum) &&
    str2val(dum,fullstate.count_say,dum) &&
    strskip(dum,')',dum);

  //some experimental arm stuff, will be skipped here "(arm (movable 0) (expires 0) (target 0 0) (count 0))"
  if ( res && strskip(dum,"(arm",dum2) ) {
    dum= dum2;
    res= strfind(dum,')',dum) && strfind(dum+1,')',dum) && strfind(dum+1,')',dum) && strfind(dum+1,')',dum) && strfind(dum+1,')',dum);
    dum++;
  }
    
  res= res &&
    //recognizing the score
    strskip(dum,"(score",dum) &&
    str2val(dum,fullstate.my_score,dum) &&
    str2val(dum,fullstate.his_score,dum); 

  if (WM::my_side == right_SIDE) {
    int tmp = fullstate.my_score;
    fullstate.my_score=  fullstate.his_score;
    fullstate.his_score= tmp;
  }

  res = res && strskip(dum,')',dum) && 
    //recognizing the ball
    strskip(dum,"((b)",dum) &&
    str2val(dum,fullstate.ball.x,dum) &&
    str2val(dum,fullstate.ball.y,dum) &&
    str2val(dum,fullstate.ball.vel_x,dum) &&
    str2val(dum,fullstate.ball.vel_y,dum) &&
    strskip(dum,')',dum); 

  //recognizing the players
  bool recognized_myself= false;

  fullstate.players_num= 0;
  while(res) {
    if ( ! strskip(dum,"((p",dum) ) 
      break;
    
    //recognizing player's header
    Msg_fullstate_v8::_fs_player & player= fullstate.players[fullstate.players_num];
    fullstate.players_num++;

    const char * dum2= dum;
    if ( strskip(dum,'l',dum2) ) {
      if (WM::my_side== left_SIDE)
	player.team= my_TEAM;
      else
	player.team= his_TEAM;
    }
    else if ( strskip(dum,'r',dum2) ) {
      if (WM::my_side== right_SIDE)
	player.team= my_TEAM;
      else
	player.team= his_TEAM;
    }
    else 
      res= false;

    dum= dum2;
    
    res = res && str2val(dum,player.number,dum);
    
    if ( strskip(dum,'g',dum2 ) ){
      player.goalie= true;
      player.type= 0;
    }
    else {
      player.goalie= false;
      res = res && str2val(dum,player.type,dum2);
    }
    dum= dum2;
    res = res && strskip(dum,')',dum) &&
      //recognizing player's values
      str2val(dum,player.x,dum) &&
      str2val(dum,player.y,dum) &&
      str2val(dum,player.vel_x,dum) &&
      str2val(dum,player.vel_y,dum) &&
      str2val(dum,player.angle,dum) &&
      str2val(dum,player.neck_angle,dum) &&
      //recognizing stamina values
      strskip(dum,"(stamina",dum) && 
      str2val(dum,player.stamina,dum) &&
      str2val(dum,player.effort,dum) &&
      str2val(dum,player.recovery,dum) &&
      strskip(dum,')',dum) &&
      //end of player's tuple
      strskip(dum,')',dum);
    
    if (player.team == my_TEAM && player.number == WM::my_number)
      recognized_myself= true;
  }

  res = res && strskip(dum,')',dum); 

  if (!res) {
    ERROR_OUT << ID << "\nparse error:\n";
    ERROR_STREAM << "\noriginal message: " << str;
    ERROR_STREAM << "\n\n>>>";
    show_parser_error_point(ERROR_STREAM,str,dum);
    return false;
  }
  else if ( ! recognized_myself  ) {
    ERROR_OUT << ID << "\ndidn't find myself in fullstate= " << str;
    return false;
  }

  return true;
}

bool SensorParser::manual_parse_see(const char* str, Msg_see & see, char const* & next) {
  const char * dum;
  //cout << "\nparsing=[" << str << "]";
  see.reset();
  next= str;

  if ( !strskip(next,"(see ",next) )
    return false;
  if ( !str2val(next, see.time, next) )
    return false;

  while (true) {
    if ( strskip(next,')',next) ) //ending braces -> end of message, otherwise eat up white space
      return true;

    if ( ! strskip(next,'(',next) )
      return false;

    ParseObject pobj;
    dum= next;
    if ( !manual_parse_see_object(next,pobj,next) ) {
      ERROR_OUT << ID << "\nunknown object" << next;
      return false;
    }

    if (pobj.res == pobj.UNKNOWN) {
      // *** IMPORTANT *** 
      // "(F)" will be treated as pobj.UNKNOWN, bu maybe we should consider such information useful in the future
      // "(G)" will be treated as pobj.UNKNOWN, bu maybe we should consider such information useful in the future

      if (dum[1] != 'F' && dum[1] != 'G' && dum[2] != ')') { //are there some other unknown objects?
	INFO_OUT << ID << "\nunknown object: ";
	for (int i=0; dum[i] != '\0' && i <20; i++)
	  INFO_STREAM << dum[i];
      }
    }
    double ppp[7];
    bool tackle_flag= false;
    int actual = str2val(next,7,ppp,next);  //read up to 7 values
    if (actual <= 0)
      return false;

    if ( strskip(next,'t',next) )
      tackle_flag= true;

    if ( ! strskip(next,')',next) )
      return false;

    //incorporate parsed data 

    if (pobj.res == pobj.MARKER) {
      if ( see.markers_num < Msg_see::markers_MAX) {
	Msg_see::_see_marker & m= see.markers[see.markers_num];
	m.see_position= true;
	m.how_many= actual;
	m.x= pobj.x;
	m.y= pobj.y;
	m.dist = ppp[0];
	m.dir = ppp[1];
	m.dist_change = ppp[2];
	m.dir_change = ppp[3];
	see.markers_num++;
      }
      else
	ERROR_OUT << ID << "\nParser: to much markers";
    }
    else if (pobj.res >= pobj.PLAYER_OBJECT ) {
      //cout << "\nplayer object: " << pobj.res;
      if ( see.players_num < Msg_see::players_MAX) {
	Msg_see::_see_player & p= see.players[see.players_num];
	p.goalie= false;
	p.tackle_flag= tackle_flag;
	p.pointto_flag= false;
	switch (pobj.res) {
	case pobj.PLAYER_MY_TEAM_GOALIE :
	  p.goalie= true;
	case pobj.PLAYER_MY_TEAM :
	  p.team= my_TEAM;
	  break;
	case pobj.PLAYER_HIS_TEAM_GOALIE :
	  p.goalie= true;
	case pobj.PLAYER_HIS_TEAM :
	  p.team= his_TEAM;
	  break;
	case pobj.PLAYER_UNKNOWN :
	  p.team= unknown_TEAM;
	}
	if (pobj.number > 0 && pobj.number < 12)
	  p.number= pobj.number;
	else
	  p.number= 0;
	    
	p.how_many= actual;
	switch ( actual ) {
	case 7: p.how_many= 6; p.pointto_flag= true; p.pointto_dir= ppp[6];  //no braek at case 7,6,4
	case 6: p.body_dir = ppp[4]; p.head_dir = ppp[5];
	case 4: p.dist_change = ppp[2]; p.dir_change = ppp[3];
	case 2: p.dist = ppp[0]; p.dir = ppp[1]; 
	  see.players_num++;
	  break;
	case 3: p.how_many= 2; p.dist = ppp[0]; p.dir = ppp[1]; p.pointto_flag= true; p.pointto_dir= ppp[2];
	  see.players_num++;
	  break;
	case 1: 
	  //ignore players with just one parameter (players_num is not increased here!)
	  if (p.number != 0) {
	    // probably the view quality is low -> problems with chang view strategy, if this message comes to often!!!
	    //WARNING_OUT << ID << "\nnumber of player parameters= " << actual << "\n --> the view quality is probably low (check change_view!!!)";
	  }
	  break;
	default: 
	  ERROR_OUT << ID << "\nwrong number of player parameters: " << actual 
		    << "\np.number= " << p.number << " p.team= " << p.team << "\nmsg=" << str;
	}
      }
      else
	ERROR_OUT << ID << "\nParser: to much players";
    }
    else if (pobj.res == pobj.BALL_OBJECT) {
      if (!see.ball_upd) {
	see.ball.how_many= actual;
	see.ball.dist = ppp[0];
	see.ball.dir = ppp[1];
	see.ball.dist_change = ppp[2];
	see.ball.dir_change = ppp[3];
	see.ball_upd= true;
      }
      else
	ERROR_OUT << ID << "\nParser: more then one ball";
    }
    else if (pobj.res == pobj.MARKER_LINE) {
      if (!see.line_upd) {
	see.line.see_position= true;
	see.line.x= pobj.x;
	see.line.y= pobj.y;
	see.line.how_many= actual;
	see.line.dist = ppp[0];
	see.line.dir = ppp[1];
	see.line.dist_change = ppp[2];
	see.line.dir_change = ppp[3];
	see.line_upd= true;
      }
      else
	;//ERROR_OUT << ID << "\nParser: more then one line";
    }
    else if ( pobj.res == pobj.UNKNOWN )
      ;
    else
      ERROR_OUT << ID << "\n-- UNKNOWN RESULT TYPE";
  }
  //never gets here
  return false;
}

bool SensorParser::manual_parse_init(const char* str, Msg_init & init) {
  init.reset();

  const char * dum;
  const char * origin= str;
  bool res= true;
  res= res && strskip(str,"(init ",str);

  dum= str;
  if ( strskip(str,'l',dum) )
    init.side= left_SIDE;
  else if ( strskip(str,'r',dum) )
    init.side= right_SIDE;
  else 
    return res= false;
  str= dum;

  int dum_int;
  res= res && str2val(str,init.number,str) && 
    manual_parse_play_mode(str,init.play_mode,dum_int,str) &&
    strskip(str,')',str);

  if (!res) { 
    ERROR_OUT << ID << "\nparse error:\n";
    show_parser_error_point(ERROR_STREAM,origin,str);
  }
  return res;
}

bool SensorParser::manual_parse_sense_body(const char * str, Msg_sense_body & sb, char const* & next) {
  sb.reset();
  const char * dum;
  next= str;
  //(sense_body 0 (view_mode high normal) (stamina 4000 1) (speed 0 0) (head_angle 0) (kick 0) (dash 0) (turn 0) (say 0) (turn_neck 0) (catch 0) (move 0) (change_view 0) (arm (movable 0) (expires 0) (target 0 0) (count 0)) (focus (target none) (count 0)) (tackle (expires 0) (count 0)))
  bool res= strskip(next,"(sense_body ",next) &&
    str2val(next,sb.time,next) &&
    strskip(next,"(view_mode ",next) &&
    manual_parse_view_mode(next, sb.view_quality, sb.view_width, next) &&
    strskip(next,')',next) &&
    strskip(next,"(stamina ",next) && str2val(next,sb.stamina,next) && str2val(next,sb.effort,next) && strskip(next,')',next) &&
    strskip(next,"(speed ",next) && str2val(next,sb.speed_value,next) && str2val(next,sb.speed_angle,next) && strskip(next,')',next) &&
    strskip(next,"(head_angle ",next) && str2val(next,sb.neck_angle,next) && strskip(next,')',next) &&
    strskip(next,"(kick ",next) && str2val(next,sb.kick_count,next) && strskip(next,')',next) &&
    strskip(next,"(dash ",next) && str2val(next,sb.dash_count,next) && strskip(next,')',next) &&
    strskip(next,"(turn ",next) && str2val(next,sb.turn_count,next) && strskip(next,')',next) &&
    strskip(next,"(say ",next) && str2val(next,sb.say_count,next) && strskip(next,')',next) &&
    strskip(next,"(turn_neck ",next) && str2val(next,sb.turn_neck_count,next) && strskip(next,')',next) &&
    strskip(next,"(catch ",next) && str2val(next,sb.catch_count,next) && strskip(next,')',next) &&
    strskip(next,"(move ",next) && str2val(next,sb.move_count,next) && strskip(next,')',next) &&
    strskip(next,"(change_view ",next) && str2val(next,sb.change_view_count,next) && strskip(next,')',next);

  //LOG_WM(sb.time,0,<<"MY STAMINA= " << sb.stamina);
#if 1 //extension for server version 8.04
  if ( strskip(next,')',dum ) )  //this is to support aserver version 7 (or version 7 servers in general)
    return true;

  /*(arm (movable 0) (expires 0) (target 0 0) (count 0)) 
    (focus (target none) (count 0)) 
    (tackle (expires 0) (count 0)))
  */
  res= res && strskip(next,"(arm (movable",next) && str2val(next,sb.arm_movable,next) && 
    strskip(next,") (expires",next) && str2val(next,sb.arm_expires,next) &&
    strskip(next,") (target",next) && str2val(next,sb.arm_target_x,next) && str2val(next,sb.arm_target_y,next) &&
    strskip(next,") (count",next) && str2val(next,sb.arm_count,next) && strskip(next,')',next) && 
    strskip(next,") (focus (target",next);

  if ( ! res)
    return false;

  if ( strskip(next,"none",dum) ) {
    sb.focus_target=  -1;
    next= dum;
  }
  else {
    strspace(next,next);
    if (next[0] == 'l' && WM::my_side == left_SIDE ||
	next[0] == 'r' && WM::my_side == right_SIDE  ) {
      next++;
      res= str2val(next,sb.focus_target,next);
    }
    else {
      next++;
      res= str2val(next,sb.focus_target,next);
      ERROR_OUT << ID << "\nI have attention to player " << sb.focus_target << " of opponent team";
      sb.focus_target= 0;
    }
  }
  res= res && strskip(next,") (count",next) && str2val(next,sb.focus_count,next) && strskip(next,')',next) &&
    strskip(next,") (tackle (expires",next) && str2val(next,sb.tackle_expires,next) &&
    strskip(next,") (count",next) && str2val(next,sb.tackle_count,next) && strskip(next,')',next) &&
    strskip(next,')',next);// insist on an ending brace
#endif  
  
  return res;
}


bool SensorParser::manual_parse_hear(const char * str, Msg_hear & hear, char const* & next, bool & reset_int) {
  const char * dum;
  hear.reset();
  //LOG_WM(hear.time,0, << "\nHearParsing0= " << str );
  if ( !strskip(str,"(hear ",next) )
    return false;
  
  //cout << "\nCOMM= " << next;

  int number;

  if ( !str2val(next,hear.time,next) )
    return false;


  if ( strskip(next,"referee",dum) ) {
    next= dum;
    int score_in_pm;
    if ( ! manual_parse_play_mode(next,hear.play_mode,score_in_pm, next) )
      return false;

    hear.play_mode_upd= true;
    if ( score_in_pm > 0 ) {
      if (hear.play_mode == PM_goal_l)
	if ( WM::my_side == left_SIDE) {
	  hear.my_score_upd= true;
	  hear.my_score= score_in_pm;
	}
	else {
	  hear.his_score_upd= true;
	  hear.his_score= score_in_pm;
	}
      else if (hear.play_mode == PM_goal_r)
	if ( WM::my_side == right_SIDE) {
	  hear.my_score_upd= true;
	  hear.my_score= score_in_pm;
	}
	else {
	  hear.his_score_upd= true;
	  hear.his_score= score_in_pm;
	}
    }
    if ( !strskip(next,")",next ) )
      return false;

    return true;
  }
  else if ( strskip(next,"self",dum) ) { //communication from myself
    next= dum;
    return true;
  }
  else if ( str2val(next,number,dum) ) { //communication from my teammate or from an opponent
    next= dum; 
    //LOG_WM(hear.time,0, << "\nHearParsing1= " << str << " now at " << next );
    
    //cout << "\ncomm= " << next;
    if ( strskip(next,"opp",dum) ) {
      //cout << "\n got opp message " << str;
      WARNING_OUT << ID << " I still get messages from opponents";
      return true;  //ignore opponents messages
    }
    
    if ( ! strskip(next,"our",next) )
      return false;
    
    if ( ! str2val(next,number,next) )  //this is the number of the communicating player
      return false;
    
    if ( ! strskip(next,'"',next) )
      return false;
    if ( ! manual_parse_teamcomm(next,hear.teamcomm,next) ) {
      WARNING_OUT << ID << "\nproblems with teamcomm";
      return false;
    }
    hear.teamcomm.from= number;

    if ( ! strskip(next,'"',next) )
      return false;

    //cout << "hear.teamcomm_upd= true;";
    hear.teamcomm_upd= true;

    //cout << "\nteamcomm ok " << hear.teamcomm.from << " at time " << hear.time << " = " << str;
    if ( ! strskip(next,')',next) )
      return false;

    //LOG_WM(hear.time,0, << "done with HearParsing");
    return true;
  }
  else if ( strskip(next,"online_coach_",dum) ) {
    next= dum;
    if ( WM::my_side == left_SIDE ) {
      if ( strskip(next,"right",dum) )
	return true; //don't read online coach messages of the opponent
      if ( ! strskip(next,"left",next) )
	return false; //wrong message type
    }
    else {
      if ( strskip(next,"left",dum) )
	return true; //don't read online coach messages of the opponent
      if ( ! strskip(next,"right",next) )
	return false; //wrong message type
    }
    bool res= manual_parse_my_online_coachcomm(next,hear.my_online_coachcomm,next);
    if (!res) 
      return false;
    hear.my_online_coachcomm_upd= true;
    hear.my_online_coachcomm.time= hear.time;

    //INFO_OUT << ID << "\nsuccessfully parsed coach message:" << str << "\nresult->" << hear;

    return true;
  }
  else if ( strskip(next,"coach",dum) ) {
    next= dum;
    bool res= manual_parse_my_trainercomm(next);
    if (!res) 
      return false;
    //INFO_OUT << ID << "\nsuccessfully parsed coach message:" << str << "\nresult->" << hear;
    reset_int=true;
    return true;
  }
  else if ( strskip(next,"our",dum) ) {
    hear.teamcomm_partial_upd= true;
    return true;
  }

  return false;
}

bool parse_score(const char * str, int & score, const char *& next) {
  if ( *str != '_' ) 
    return true;

  str++;
  if ( !str2val(str,score,next) )
    return false;
  return true;
}

bool SensorParser::manual_parse_play_mode(const char * str, PlayMode & pm, int & score, const char *& next) {
  score= -1;
  while (str[0] == ' ')
    str++;       
                 /* 1234567890123456789 */  
  if      (strncmp("before_kick_off", str, 15) == 0) { pm= PM_before_kick_off; str+= 15; } 
  else if (strncmp("time_over", str, 9) == 0)        { pm= PM_time_over; str+= 9; } 
  else if (strncmp("play_on", str, 7) == 0)          { pm= PM_play_on; str+= 7; } 
  else if (strncmp("kick_off_l", str, 10) == 0)      { pm= PM_kick_off_l; str+= 10; }
  else if (strncmp("kick_off_r", str, 10) == 0)      { pm= PM_kick_off_r; str+= 10; } 
  else if (strncmp("kick_in_l", str, 9) == 0)        { pm= PM_kick_in_l; str+= 9; } 
  else if (strncmp("foul_r", str, 6) == 0)           { pm= PM_kick_in_l; str+= 6; }  //match foul_r to kick_in_l
  else if (strncmp("kick_in_r", str, 9) == 0)        { pm= PM_kick_in_r; str+= 9; } 
  else if (strncmp("foul_l", str, 6) == 0)           { pm= PM_kick_in_r; str+= 6; }  //match foul_l to kick_in_r
  else if (strncmp("free_kick_l", str, 11) == 0)     { pm= PM_free_kick_l; str+= 11; } 
  else if (strncmp("free_kick_r", str, 11) == 0)     { pm= PM_free_kick_r; str+= 11; } 
  else if (strncmp("indirect_free_kick_l", str, 20) == 0)  { pm= PM_free_kick_l; str+= 20; } //no dinstiction to free_kick_* at the moment, go2003
  else if (strncmp("indirect_free_kick_r", str, 20) == 0)  { pm= PM_free_kick_r; str+= 20; } //no dinstiction to free_kick_* at the moment, go2003 
  else if (strncmp("corner_kick_l", str, 13) == 0)         { pm= PM_corner_kick_l; str+= 13; } 
  else if (strncmp("corner_kick_r", str, 13) == 0)         { pm= PM_corner_kick_r; str+= 13; }
  else if (strncmp("goal_kick_l", str, 11) == 0)           { pm= PM_goal_kick_l; str+= 11; } 
  else if (strncmp("goal_kick_r", str, 11) == 0)           { pm= PM_goal_kick_r; str+= 11; } 
  else if (strncmp("goal_l", str, 6) == 0) { 
    pm= PM_goal_l; str+= 6; 
    if ( str[0] == '_' ) {
      str++;
      const char * dum;
      if ( !str2val(str,score,dum) )
	return false;
      str= dum;
    }
  }
  else if (strncmp("goal_r", str, 6) == 0) { 
    pm= PM_goal_r; str+= 6; 
    if ( str[0] == '_' ) {
      str++;
      const char * dum;
      if ( !str2val(str,score,dum) )
	return false;
      str= dum;
    }
  }
  else if (strncmp("drop_ball", str, 9) == 0)              { pm= PM_drop_ball; str+= 9; }
  else if (strncmp("offside_l", str, 9) == 0)              { pm= PM_offside_l; str+= 9; }
  else if (strncmp("offside_r", str, 9) == 0)              { pm= PM_offside_r; str+= 9; }
  else if (strncmp("goalie_catch_ball_l", str, 19) == 0)   { pm= PM_goalie_catch_ball_l; str+= 19; } 
  else if (strncmp("goalie_catch_ball_r", str, 19) == 0)   { pm= PM_goalie_catch_ball_r; str+= 19; } 
  else if (strncmp("free_kick_fault_l",str, 17) == 0)      { pm= PM_free_kick_fault_l; str+= 17; }
  else if (strncmp("free_kick_fault_r",str, 17) == 0)      { pm= PM_free_kick_fault_r; str+= 17; }
  else if (strncmp("back_pass_l",str, 11) == 0)            { pm= PM_back_pass_l; str+= 11; }
  else if (strncmp("back_pass_r",str, 11) == 0)            { pm= PM_back_pass_r; str+= 11; }
                 /* 1234567890123456789 */
  else if (strncmp("penalty_kick_l",str,14) == 0)          { pm= PM_penalty_kick_l; str+= 14; }
  else if (strncmp("penalty_kick_r",str,14) == 0)          { pm= PM_penalty_kick_r; str+= 14; }
  else if (strncmp("catch_fault_l",str,13) == 0)           { pm= PM_catch_fault_l; str+= 13; }
  else if (strncmp("catch_fault_r",str,13) == 0)           { pm= PM_catch_fault_r; str+= 13; }
  else if (strncmp("indirect_free_kick_l",str,20) == 0)    { pm= PM_indirect_free_kick_l; str+= 20; }
  else if (strncmp("indirect_free_kick_r",str,20) == 0)    { pm= PM_indirect_free_kick_r; str+= 20; }
  else if (strncmp("penalty_setup_l",str,15) == 0)         { pm= PM_penalty_setup_l; str+= 15; }
  else if (strncmp("penalty_setup_r",str,15) == 0)         { pm= PM_penalty_setup_r; str+= 15; }
  else if (strncmp("penalty_ready_l",str,15) == 0)         { pm= PM_penalty_ready_l; str+= 15; }
  else if (strncmp("penalty_ready_r",str,15) == 0)         { pm= PM_penalty_ready_r; str+= 15; }
  else if (strncmp("penalty_taken_l",str,15) == 0)         { pm= PM_penalty_taken_l; str+= 15; }
  else if (strncmp("penalty_taken_r",str,15) == 0)         { pm= PM_penalty_taken_r; str+= 15; }
  else if (strncmp("penalty_miss_l",str,14) == 0)          { pm= PM_penalty_miss_l; str+= 14; }
  else if (strncmp("penalty_miss_r",str,14) == 0)          { pm= PM_penalty_miss_r; str+= 14; }
  else if (strncmp("penalty_score_l",str,15) == 0)         { pm= PM_penalty_score_l; str+= 15; }
  else if (strncmp("penalty_score_r",str,15) == 0)         { pm= PM_penalty_score_r; str+= 15; }

  else if (strncmp("penalty_onfield_l",str,17) == 0)       { pm= PM_penalty_onfield_l; str+= 17; }
  else if (strncmp("penalty_onfield_r",str,17) == 0)       { pm= PM_penalty_onfield_r; str+= 17; }
  else if (strncmp("penalty_foul_l",str,14) == 0)          { pm= PM_penalty_foul_l; str+= 14; }
  else if (strncmp("penalty_foul_r",str,14) == 0)          { pm= PM_penalty_foul_r; str+= 14; }
  else if (strncmp("penalty_winner_l",str,16) == 0)        { pm= PM_penalty_winner_l; str+= 16; }
  else if (strncmp("penalty_winner_r",str,16) == 0)        { pm= PM_penalty_winner_r; str+= 16; }
  else if (strncmp("penalty_draw",str,12) == 0)            { pm= PM_penalty_draw; str+= 16; }
                 /* 1234567890123456789 */
  else if (strncmp("half_time",str, 9) == 0) { pm= PM_half_time; str+= 9; }
  else if (strncmp("time_up",str, 7) == 0) { pm= PM_time_up; str+= 7; }
  else if (strncmp("time_extended",str, 13) == 0) { pm= PM_time_extended; str+= 13; }
                 /* 1234567890123456789 */
  else { 
    pm= PM_Null; 
    next= str;
    return false;
  }

  next= str;
  return true;
}

bool SensorParser::manual_parse_view_mode(const char * str, int & quality, int & width, const char *& next) {
  while (str[0] == ' ')
    str++;

  if (strncmp("high normal",str,11) == 0 ) {
    str+= 11;
    quality= HIGH; 
    width= NORMAL;
  }
  else if (strncmp("high wide",str,9) == 0 ) {
    str+= 9;
    quality= HIGH; 
    width= WIDE;
  }
  else if (strncmp("high narrow",str,11) == 0 ) {
    str+= 11;
    quality= HIGH; 
    width= NARROW;
  }
  else if (strncmp("low normal",str,10) == 0 ) {
    str+= 10;
    quality= LOW; 
    width= NORMAL;
  }
  else if (strncmp("low wide",str,8) == 0 ) {
    str+= 8;
    quality= LOW; 
    width= WIDE;
  }
  else if (strncmp("low narrow",str,10) == 0 ) {
    str+= 10;
    quality= LOW; 
    width= NARROW;
  }
  else 
    return false;

  next= str;
  return true;
}
#define ERROR { std::cout << "\nparse_error"; return false; }

bool SensorParser::manual_parse_see_object(const char * str, ParseObject & pobj, const char *& next) {
#if 1
  const char * dum;
  pobj.res= pobj.UNKNOWN;
  pobj.number= -1;
  pobj.x= -1000.0;
  pobj.y= -1000.0;

  int num= 0;
  const char XXX = -10;
  const char _F_ = -1;
  const char _P_ = -2;
  const char _G_ = -3;
  const char _C_ = -4;
  const char _L_ = -5; //line and left
  const char _R_ = -6;
  const char _T_ = -7;
  const char _B_ = -8;

  int obj[5];

  while ( str[0] == ' ')
    str++;
  if (str[0] != '(')
    ERROR;
  str++;
  while (num<4) {
    if (str[0] == 'f' || str[0] == 'F') {
      str++;
      obj[num]= _F_;
    }
    else if (str[0] == 'p' || str[0] == 'P') {
      str++;
      obj[num]= _P_;
    }
    else if (str[0] == 'g' || str[0] == 'G') {
      str++;
      obj[num]= _G_;
    }
    else if (str[0] == 'l' || str[0] == 'L') {
      str++;
      obj[num]= _L_;
    }
    else if (str[0] == 'r') {
      str++;
      obj[num]= _R_;
    }
    else if (str[0] == 't') {
      str++;
      obj[num]= _T_;
    }
    else if (str[0] == 'b' || str[0] == 'B') {
      str++;
      obj[num]= _B_;
    }
    else if (str[0] == 'c') {
      str++;
      obj[num]= _C_;
    }
    else if (str[0] == '0') {
      str+= 1;
      obj[num]= 0;
    }
    else if (str[0] == '1' && str[1] == '0') {
      str+= 2;
      obj[num]= 10;
    }
    else if (str[0] == '2' && str[1] == '0') {
      str+= 2;
      obj[num]= 20;
    }
    else if (str[0] == '3' && str[1] == '0') {
      str+= 2;
      obj[num]= 30;
    }
    else if (str[0] == '4' && str[1] == '0') {
      str+= 2;
      obj[num]= 40;
    }
    else if (str[0] == '5' && str[1] == '0') {
      str+= 2;
      obj[num]= 50;
    }
    else
      ERROR;
    num++;

    if (str[0] == '\0')
      ERROR;
    if (str[0] == ' ')
      str++;
    if (str[0] == ')' || str[0] == '"')
      break;
  }

  if (num<=0)
    ERROR;

  obj[num]= XXX;
  num++;

  const double LEFT_LINE= -PITCH_LENGTH/2.0;
  const double LEFT_OUTSIDE=  LEFT_LINE - PITCH_MARGIN;
  const double LEFT_PENALTY=  LEFT_LINE + PENALTY_AREA_LENGTH;
  const double RIGHT_LINE= PITCH_LENGTH/2.0;
  const double RIGHT_OUTSIDE=  RIGHT_LINE + PITCH_MARGIN;
  const double RIGHT_PENALTY=  RIGHT_LINE - PENALTY_AREA_LENGTH;
  const double TOP_LINE = PITCH_WIDTH/2.0;
  const double TOP_OUTSIDE = TOP_LINE + PITCH_MARGIN;
  const double TOP_PENALTY = PENALTY_AREA_WIDTH/2.0;
  const double TOP_GOAL = ServerOptions::goal_width/2;
  const double BOTTOM_LINE = -PITCH_WIDTH/2.0;
  const double BOTTOM_OUTSIDE = BOTTOM_LINE - PITCH_MARGIN;
  const double BOTTOM_PENALTY = -PENALTY_AREA_WIDTH/2.0;
  const double BOTTOM_GOAL = -ServerOptions::goal_width/2;

  if (obj[0] == _P_) { //the object was a player, read his attributes, the date is read later on
    if (str[0] == ')') {
      str++;
      pobj.res= pobj.PLAYER_UNKNOWN;
      next= str;
      return true;
    }

    if (str[0] == '"') {
      str++;
      pobj.res= pobj.PLAYER_HIS_TEAM;
      if ( strncmp(str,WM::my_team_name,WM::my_team_name_len) == 0 ) {
	str+= WM::my_team_name_len;
	if (str[0] == '"')  //this check is necessary, because oppenent's name can have my_team_name as prefix
	  pobj.res= pobj.PLAYER_MY_TEAM;
      }

      while ( str[0] != '\0' && str[0] != '"' )
	str++;

      if ( str[0] == '\0' ) 
	return false;

      str++;
      if (str[0] == ')') {
	str++;
	pobj.number= 0;
	next= str;
	return true;
      }
      
      if ( !str2val(str,pobj.number,dum) ) 
	return false;

      str= dum;
      if (str[0] == ')') {
	str++;
	next= str;
	return true;
      }
      if ( !strskip(str,"goalie",dum) ) 
	return false;
      str= dum;
      if (pobj.res == pobj.PLAYER_MY_TEAM)
	pobj.res= pobj.PLAYER_MY_TEAM_GOALIE;
      else if (pobj.res == pobj.PLAYER_HIS_TEAM)
	pobj.res= pobj.PLAYER_HIS_TEAM_GOALIE;
      if (str[0] != ')')
	return false;
      str++;
      next= str;
      return true;
    }
  }
  // 0 --------------------------------------------
  else if (obj[0] == _F_) {
    // 1 --------------------------------------------
    if (obj[1] == _P_) {
      // 2 --------------------------------------------
      if (obj[2] == _L_) {
	if (obj[3] == _T_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_PENALTY;
	  pobj.y = TOP_PENALTY;
	}
	else if (obj[3] == _C_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_PENALTY;
	  pobj.y = 0.0;
	}
	if (obj[3] == _B_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_PENALTY;
	  pobj.y = BOTTOM_PENALTY;
	}
      }
      // 2 --------------------------------------------
      else if (obj[2] == _R_) {
	if (obj[3] == _T_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_PENALTY;
	  pobj.y = TOP_PENALTY;
	}
	else if (obj[3] == _C_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_PENALTY;
	  pobj.y = 0.0;
	}
	if (obj[3] == _B_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_PENALTY;
	  pobj.y = BOTTOM_PENALTY;
	}
      }
    }
    // 1 --------------------------------------------
    if (obj[1] == _G_) {
      // 2 --------------------------------------------
      if (obj[2] == _L_) {
	if (obj[3] == _T_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_LINE;
	  pobj.y = TOP_GOAL;
	}
	else if (obj[3] == _B_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_LINE;
	  pobj.y = BOTTOM_GOAL;
	}
      }
      // 2 --------------------------------------------
      else if (obj[2] == _R_) {
	if (obj[3] == _T_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_LINE;
	  pobj.y = TOP_GOAL;
	}
	else if (obj[3] == _B_) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_LINE;
	  pobj.y = BOTTOM_GOAL;
	}
      }
    }
    // 1 --------------------------------------------
    else if (obj[1]== _L_) {
      // 2 --------------------------------------------
      if ( obj[2] == 0) {
	pobj.res= pobj.MARKER;
	pobj.x = LEFT_OUTSIDE;
	pobj.y = 0.0;
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _T_) {
	if ( obj[3] == XXX) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_LINE;
	  pobj.y = TOP_LINE;
	}
	else if ( obj[3] > 0 && obj[3] <= 30) {
	  pobj.res= pobj.MARKER;
	  pobj.x= LEFT_OUTSIDE;
	  pobj.y = double(obj[3]);
	}
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _B_) {
	if ( obj[3] == XXX) {
	  pobj.res= pobj.MARKER;
	  pobj.x =  LEFT_LINE;
	  pobj.y =  BOTTOM_LINE;
	}
	else if ( obj[3] > 0 && obj[3] <= 30) {
	  pobj.res= pobj.MARKER;
	  pobj.x = LEFT_OUTSIDE;
	  pobj.y = double(-obj[3]);
	}
      }
    }
    // 1 --------------------------------------------
    else if (obj[1] == _R_) {
      // 2 --------------------------------------------
      if ( obj[2] == 0) {
	pobj.res= pobj.MARKER;
	pobj.x = RIGHT_OUTSIDE;
	pobj.y = 0.0;
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _T_) {
	if ( obj[3] == XXX) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_LINE;
	  pobj.y = TOP_LINE;
	}
	else if ( obj[3] > 0 && obj[3] <= 30 ) {
	  pobj.res= pobj.MARKER;
	  pobj.x= RIGHT_OUTSIDE;
	  pobj.y = double(obj[3]);
	}
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _B_) {
	if ( obj[3] == XXX) {
	  pobj.res= pobj.MARKER;
	  pobj.x =  RIGHT_LINE;
	  pobj.y =  BOTTOM_LINE;
	}
	else if ( obj[3] > 0 && obj[3] <= 30) {
	  pobj.res= pobj.MARKER;
	  pobj.x = RIGHT_OUTSIDE;
	  pobj.y = double(-obj[3]);
	}
      }
    }
    // 1 --------------------------------------------
    else if (obj[1]== _C_) {
      // 2 --------------------------------------------
      if ( obj[2] == XXX) {
	pobj.res= pobj.MARKER;
	pobj.x = 0.0;
	pobj.y = 0.0;
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _T_) {
	pobj.res= pobj.MARKER;
	pobj.x = 0.0;
	pobj.y = TOP_LINE;
      }
      // 2 --------------------------------------------
      else if ( obj[2] == _B_) {
	pobj.res= pobj.MARKER;
	pobj.x = 0.0;
	pobj.y = BOTTOM_LINE;
      }
    }
    // 1 --------------------------------------------
    else if (obj[1]== _T_) {
      if (obj[2] == 0) {
	pobj.res= pobj.MARKER;
	pobj.x = 0.0;
	pobj.y= TOP_OUTSIDE;
      }
      else if (obj[2] == _L_ && obj[3] > 0 && obj[3] <= 50) {
	pobj.res= pobj.MARKER;
	pobj.x = double(-obj[3]);
	pobj.y= TOP_OUTSIDE;
      }
      else if (obj[2] == _R_ && obj[3] > 0 && obj[3] <= 50) {
	pobj.res= pobj.MARKER;
	pobj.x = double(obj[3]);
	pobj.y= TOP_OUTSIDE;
      }
    }
    // 1 --------------------------------------------
    else if (obj[1]== _B_) {
      if (obj[2] == 0) {
	pobj.res= pobj.MARKER;
	pobj.x = 0.0;
	pobj.y= BOTTOM_OUTSIDE;
      }
      else if (obj[2] == _L_ && obj[3] > 0 && obj[3] <= 50) {
	pobj.res= pobj.MARKER;
	pobj.x = double(-obj[3]);
	pobj.y= BOTTOM_OUTSIDE;
      }
      else if (obj[2] == _R_ && obj[3] > 0 && obj[3] <= 50) {
	pobj.res= pobj.MARKER;
	pobj.x = double(obj[3]);
	pobj.y= BOTTOM_OUTSIDE;
      }
    }
  }
  // 0 --------------------------------------------
  else if (obj[0] == _G_) {
    if (obj[1] == _L_) {
      pobj.res= pobj.MARKER;
      pobj.x = LEFT_LINE;
      pobj.y = 0.0;
    }
    else if (obj[1] == _R_) {
      pobj.res= pobj.MARKER;
      pobj.x = RIGHT_LINE;
      pobj.y = 0.0;
    }
  }
  // 0 --------------------------------------------
  else if (obj[0] == _B_) {
    pobj.res= pobj.BALL_OBJECT;
    if (str[0] != ')')
      return false;
    str++;
    next= str;
    return true;
  } 
  // 0 --------------------------------------------
  else if (obj[0] == _L_) {
    if (obj[1] == _L_) {
      pobj.res= pobj.MARKER_LINE;
      pobj.x = LEFT_LINE;
      pobj.y = 0.0;
    }
    else if (obj[1] == _R_) {
      pobj.res= pobj.MARKER_LINE;
      pobj.x = RIGHT_LINE;
      pobj.y = 0.0;
    }
    if (obj[1] == _T_) {
      pobj.res= pobj.MARKER_LINE;
      pobj.x = 0.0;
      pobj.y = TOP_LINE;
    }
    else if (obj[1] == _B_) {
      pobj.res= pobj.MARKER_LINE;
      pobj.x = 0.0;
      pobj.y = BOTTOM_LINE;
    }
  }
  if (str[0] != ')') 
    return false;

  str++;
  next= str;
  return true;
#endif
}

int str2val(const char * str, NOT_NEEDED & val, const char* & next) {
  return strfind(str,')',next);
}

#if 0
bool SensorParser::manual_parse_player_param(const char * str, Msg_player_param & pp) {
  const char * origin= str;

  bool res;

  /* automatically generated from a player_param message
     with entries separated by newlines and without the ending ')'
     gawk -f generate_player_param_parser.awk <player_param_message>
  */
  res= strskip(str,"(player_param ",str) &&
    strskip(str,"(player_types",str) && str2val(str,pp.player_types,str) && pp.read_player_types.set_ok() && strskip(str,')',str) &&
    strskip(str,"(pt_max",str) && str2val(str,pp.pt_max,str) && pp.read_pt_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(random_seed",str) && str2val(str,pp.random_seed,str) && pp.read_random_seed.set_ok() && strskip(str,')',str) &&
    strskip(str,"(subs_max",str) && str2val(str,pp.subs_max,str) && pp.read_subs_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(dash_power_rate_delta_max",str) && str2val(str,pp.dash_power_rate_delta_max,str) && pp.read_dash_power_rate_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(dash_power_rate_delta_min",str) && str2val(str,pp.dash_power_rate_delta_min,str) && pp.read_dash_power_rate_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(effort_max_delta_factor",str) && str2val(str,pp.effort_max_delta_factor,str) && pp.read_effort_max_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(effort_min_delta_factor",str) && str2val(str,pp.effort_min_delta_factor,str) && pp.read_effort_min_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(extra_stamina_delta_max",str) && str2val(str,pp.extra_stamina_delta_max,str) && pp.read_extra_stamina_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(extra_stamina_delta_min",str) && str2val(str,pp.extra_stamina_delta_min,str) && pp.read_extra_stamina_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(inertia_moment_delta_factor",str) && str2val(str,pp.inertia_moment_delta_factor,str) && pp.read_inertia_moment_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(kick_rand_delta_factor",str) && str2val(str,pp.kick_rand_delta_factor,str) && pp.read_kick_rand_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(kickable_margin_delta_max",str) && str2val(str,pp.kickable_margin_delta_max,str) && pp.read_kickable_margin_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(kickable_margin_delta_min",str) && str2val(str,pp.kickable_margin_delta_min,str) && pp.read_kickable_margin_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(new_dash_power_rate_delta_max",str) && str2val(str,pp.new_dash_power_rate_delta_max,str) && pp.read_new_dash_power_rate_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(new_dash_power_rate_delta_min",str) && str2val(str,pp.new_dash_power_rate_delta_min,str) && pp.read_new_dash_power_rate_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(new_stamina_inc_max_delta_factor",str) && str2val(str,pp.new_stamina_inc_max_delta_factor,str) && pp.read_new_stamina_inc_max_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(player_decay_delta_max",str) && str2val(str,pp.player_decay_delta_max,str) && pp.read_player_decay_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(player_decay_delta_min",str) && str2val(str,pp.player_decay_delta_min,str) && pp.read_player_decay_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(player_size_delta_factor",str) && str2val(str,pp.player_size_delta_factor,str) && pp.read_player_size_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,"(player_speed_max_delta_max",str) && str2val(str,pp.player_speed_max_delta_max,str) && pp.read_player_speed_max_delta_max.set_ok() && strskip(str,')',str) &&
    strskip(str,"(player_speed_max_delta_min",str) && str2val(str,pp.player_speed_max_delta_min,str) && pp.read_player_speed_max_delta_min.set_ok() && strskip(str,')',str) &&
    strskip(str,"(stamina_inc_max_delta_factor",str) && str2val(str,pp.stamina_inc_max_delta_factor,str) && pp.read_stamina_inc_max_delta_factor.set_ok() && strskip(str,')',str) &&
    strskip(str,')',str);

  if (!res) { 
    ERROR_OUT << ID << "\nparse error:\n";
    show_parser_error_point(ERROR_STREAM,origin,str);
  }
  return res;
}
#endif

bool SensorParser::manual_parse_player_type(const char * str, Msg_player_type & pt) {
  const char * origin= str;

  bool res= strskip(str,"(player_type ",str) &&
    strskip(str,"(id",str) && str2val(str,pt.id,str) && strskip(str,')',str) &&
    strskip(str,"(player_speed_max",str) && str2val(str,pt.player_speed_max,str) && strskip(str,')',str) &&
    strskip(str,"(stamina_inc_max",str) && str2val(str,pt.stamina_inc_max,str) && strskip(str,')',str) &&
    strskip(str,"(player_decay",str) && str2val(str,pt.player_decay,str) && strskip(str,')',str) &&
    strskip(str,"(inertia_moment",str) && str2val(str,pt.inertia_moment,str) && strskip(str,')',str) &&
    strskip(str,"(dash_power_rate",str) && str2val(str,pt.dash_power_rate,str) && strskip(str,')',str) &&
    strskip(str,"(player_size",str) && str2val(str,pt.player_size,str) && strskip(str,')',str) &&
    strskip(str,"(kickable_margin",str) && str2val(str,pt.kickable_margin,str) && strskip(str,')',str) &&
    strskip(str,"(kick_rand",str) && str2val(str,pt.kick_rand,str) && strskip(str,')',str) &&
    strskip(str,"(extra_stamina",str) && str2val(str,pt.extra_stamina,str) && strskip(str,')',str) &&
    strskip(str,"(effort_max",str) && str2val(str,pt.effort_max,str) && strskip(str,')',str) &&
    strskip(str,"(effort_min",str) && str2val(str,pt.effort_min,str) && strskip(str,')',str) &&
    strskip(str,')',str);

  if (!res) { 
    ERROR_OUT << ID << "\nparse error:\n";
    show_parser_error_point(ERROR_STREAM,origin,str);
  }
  return res;
}

bool SensorParser::manual_parse_change_player_type(const char * str, Msg_change_player_type & cpt) {
  cpt.reset();
  bool res= strskip(str,"(change_player_type ",str) && str2val(str,cpt.number,str);
  if (!res)
    return false;

  if ( strskip(str,')',str) )
    return true;

  res= str2val(str,cpt.type,str) && strskip(str,')',str);
  return res;
}

bool SensorParser::manual_parse_my_online_coachcomm(const char * str, 
                                     Msg_my_online_coachcomm & moc, 
                                     const char *& next) 
{
  //printf("MANUALPARSEHEAR: %s\n",str);
  const char * next1;
  moc.reset();
  int tmp;
  next= str;
  //if ( ! str2val(next,moc.time,next) )
  //  return false;
  str= next;
  if ( strskip(str,"(info",next) )
    ;
  else if ( strskip(str,"(advice",next) )
    ;
  else
    return false;

  if ( ! strskip(next,'(',next) )
    return false;
  if ( ! str2val(next,tmp,next) )
    return false;
  if ( ! strskip(next,"(true)",next) )
    return false;
  if ( ! strskip(next,'"',next) )
    return false;
  if ( strskip(next,"pt",next1) )
  {
    next = next1;

    for (int i=0; i<NUM_PLAYERS; i++) {
      switch (*next) {
      case '0' : moc.his_player_types[i]= 0; break;
      case '1' : moc.his_player_types[i]= 1; break;
      case '2' : moc.his_player_types[i]= 2; break;
      case '3' : moc.his_player_types[i]= 3; break;
      case '4' : moc.his_player_types[i]= 4; break;
      case '5' : moc.his_player_types[i]= 5; break;
      case '6' : moc.his_player_types[i]= 6; break;
      case '_' : moc.his_player_types[i]= -1; break;
      default: return false;
      }
      next++;
    }
    if ( ! strskip(next,'g',next) )
      return false;
  
    if ( strskip(next,'_', str ) ) {
      next= str;
      moc.his_goalie_number_upd= false;
    }
    else {     
      moc.his_goalie_number_upd= true;
      if ( ! str2val(next,moc.his_goalie_number, next) )
        return false;
    }
    moc.his_player_types_upd= true;
  }
    else return false;

  return true;
}


bool SensorParser::manual_parse_my_trainercomm(const char * str) {
  int tmp;
  const char * next;
  next= str;
  //if ( ! str2val(next,moc.time,next) )
  //  return false;
  //str= next;
  if ( ! strskip(next,'"',next) )
    return false;  
  //if ( ! strskip(next,"r",next) ) hack
  if (!next[9]=='r')
    return false;
  return true;

  if ( ! str2val(next,tmp,next) )
    return false;
  if ( ! strskip(next,"(true)",next) )
    return false;
  if ( ! strskip(next,'"',next) )
    return false;
  if ( ! strskip(next,"pt",next) )
    return false;

}


bool SensorParser::manual_parse_teamcomm(const char * str, Msg_teamcomm & tc, const char *& next) {
  const char * beg= str;
  tc.reset();
  const double SCALE= ENCODE_SCALE; //shortcut
  int idum;

  if ( str[0] != '*' || str[1] != '*' || str[2] != '*')  
    return false;
  str+= 3;
  if ( !WMTOOLS::a64_to_uint6(str,tc.side) )
    return false; 

  if ( tc.side != WM::my_side )
    return false;

  str++;
  if ( !WMTOOLS::a3x64_to_int18(str,tc.time) )
    return false;
  str+= 3;
  if ( !WMTOOLS::a3x64_to_int18(str,tc.time_cycle) )
    return false;
  str+= 3;
  if ( !WMTOOLS::a64_to_uint6(str,tc.from) )
    return false; 
  str++;

  //get his goalie number (if available)
  if ( !WMTOOLS::a64_to_uint6(str,idum) )
    return false;
  str++;
  tc.his_goalie_number_upd= idum;
  if ( tc.his_goalie_number_upd ) {
    if ( !WMTOOLS::a64_to_uint6(str,tc.his_goalie_number) ) {
      ERROR_OUT << ID << "SensorParser::manual_parse_teamcomm: wrong range for his_goalie_number (" << tc.his_goalie_number << ")";
      return false; 
    }
    str++;
  }

  //get ball information (if available)
  if ( !WMTOOLS::a64_to_uint6(str,idum) )
    return false;
  str++;
  tc.ball_upd= idum;

  if ( tc.ball_upd) {
    if ( !WMTOOLS::a64_to_uint6(str,tc.ball.how_old) )
      return false;
    str++;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.ball.x= double(idum)/SCALE;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.ball.y= double(idum)/SCALE;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.ball.vel_x= double(idum)/SCALE;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.ball.vel_y= double(idum)/SCALE;
  }

  if ( !WMTOOLS::a64_to_uint6(str,tc.players_num) )
    return false;
  str++;
  if ( tc.players_num < 0 || tc.players_num > tc.players_MAX) {
    ERROR_OUT << ID << "\nto many _tc_player entries";
    return false;
  }
  
  for (int i=0; i< tc.players_num; i++) {
    if ( !WMTOOLS::a64_to_uint6(str,tc.players[i].how_old) )
      return false;
    str++;
    if ( !WMTOOLS::a64_to_uint6(str,tc.players[i].team) )
      return false;
    str++;
    if ( !WMTOOLS::a64_to_uint6(str,tc.players[i].number) )
      return false;
    str++;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.players[i].x= double(idum)/SCALE;
    if ( !WMTOOLS::a3x64_to_int18(str, idum) )
      return false;
    str+= 3;
    tc.players[i].y= double(idum)/SCALE;
  }

  int check_sum= 0;
  while ( beg < str) {
    check_sum += *beg;
    beg++;
  }
  check_sum= abs(check_sum) % 63;
  int saved_check_sum;
  if ( !WMTOOLS::a64_to_uint6(str,saved_check_sum) )
    return false;
  str++;

  if ( saved_check_sum != check_sum ) {
    ERROR_OUT << ID << "\nwrong checksum";
    return false;
  }
  if ( str[0] != '*' ) {
    ERROR_OUT << ID << "\nwrong end character";
    return false;
  }
  str++;
  next= str;
  return true;
}

bool SensorParser::manual_parse_teamcomm(const char * str, Msg_teamcomm2 & tc, const char *& next) {
  next= str;
  tc.reset();
  BitFIFO bfifo;
  int dum;

  for (int i=0; i<10; i++) { //the message must be 10 bytes long, last char is the checksum!!!
    if ( ! WMTOOLS::a64_to_uint6(next, dum) )
      return false;

    bfifo.put(6,dum);
    next++;
  }
  unsigned int tmp;
  //cout << "\nrecv= "; bfifo.show(cout);
  
  //here in bfifo we have the right information, and the checksum of the information was right
  
  while ( bfifo.get_size() >= 5 ) {
    unsigned int obj_id;
    
    //cout << "\ndecoding ";bfifo.show(cout);
    if ( ! bfifo.get(5,obj_id) )
      return false;

    //cout << "\nnew obj_id= " << obj_id;
    
    if (obj_id == invalid_id ) 
      return true;

    if (obj_id > max_id)
      return false;
    
    if ( obj_id == msg_id ) {
      if ( ! bfifo.get(16,tmp) )
        return false;
      tc.msg.param1= (short)tmp;
      
      if ( ! bfifo.get(16,tmp) )
        return false;
      tc.msg.param2= (short)tmp;

      if ( ! bfifo.get(8,tmp) ) //hauke
        return false;           //hauke
      tc.msg.type= (unsigned char)tmp;  //hauke

      tc.msg.valid= true;
      tc.msg.from= tc.from;
      continue;  
    }

    //it must be an object, so read the positions of it 
    if ( ! bfifo.get(13,tmp) )
      return false;

    Vector pos;
    if ( ! range13bit_to_pos(tmp, pos) )
      return false;

    if ( obj_id == ball_id ) {
      tc.ball.valid= true;
      tc.ball.pos= pos;
      continue;
    }

    if ( obj_id == pass_info_id ) {
      tc.pass_info.ball_pos= pos;

      if ( ! bfifo.get(12,tmp) )
	return false;
      
      if ( ! range12bit_to_vel(tmp, tc.pass_info.ball_vel) )
	return false;

      if ( ! bfifo.get(7,tmp) )
	return false;
      
      tc.pass_info.time= tmp;

      tc.pass_info.valid= true;
      continue;
    }

    if ( obj_id == ball_info_id ) {
      tc.ball_info.ball_pos= pos;

      if ( ! bfifo.get(12,tmp) )
	return false;
      
      if ( ! range12bit_to_vel(tmp, tc.ball_info.ball_vel) )
	return false;

      if ( ! bfifo.get(3,tmp) )
	return false;
      
      tc.ball_info.age_pos= tmp;

      if ( ! bfifo.get(3,tmp) )
	return false;
      
      tc.ball_info.age_vel= tmp;
      tc.ball_info.valid= true;
      continue;
    }

    if ( obj_id == ball_holder_info_id ) {
      tc.ball_holder_info.valid= true;
      tc.ball_holder_info.pos= pos;
      continue;
    }

    Msg_teamcomm2::_player & p= tc.players[tc.players_num];
    p.pos= pos;
    p.number= (obj_id - 2) % 11 + 1;
    if ( obj_id <= 12 )
      p.team= my_TEAM;
    else 
      p.team= his_TEAM;
    tc.players_num++;
  }
  return true;
}

bool SensorParser::manual_encode_teamcomm(char * str, const Msg_teamcomm & tc, char *& end) {
  const char * beg= str;
  const double SCALE= ENCODE_SCALE; //shortcut

  str[0]= '*';   str[1]= '*';   str[2]= '*';
  str+= 3;
  WMTOOLS::uint6_to_a64(tc.side,str); 
  str++;
  WMTOOLS::int18_to_a3x64(tc.time,str); 
  str+= 3;
  WMTOOLS::int18_to_a3x64(tc.time_cycle,str); 
  str+= 3;
  WMTOOLS::uint6_to_a64(tc.from,str); 
  str++;

  if ( tc.his_goalie_number_upd) {
    WMTOOLS::uint6_to_a64(1,str); 
    str++;
    WMTOOLS::uint6_to_a64(tc.his_goalie_number,str); 
    str++;
  }
  else {
    WMTOOLS::uint6_to_a64(0,str); 
    str++;
  }

  if ( tc.ball_upd) {
    WMTOOLS::uint6_to_a64(1,str); 
    str++;
    WMTOOLS::uint6_to_a64(tc.ball.how_old,str); 
    str++;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.ball.x) ),str); 
    str+= 3;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.ball.y) ),str); 
    str+= 3;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.ball.vel_x) ),str); 
    str+= 3;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.ball.vel_y) ),str); 
    str+= 3;
  }
  else {
    WMTOOLS::uint6_to_a64(0,str); 
    str++;
  }

  WMTOOLS::uint6_to_a64(tc.players_num,str); 
  str++;
  for (int i=0; i< tc.players_num; i++) {
    WMTOOLS::uint6_to_a64(tc.players[i].how_old,str); 
    str++;
    WMTOOLS::uint6_to_a64(tc.players[i].team,str); 
    str++;
    WMTOOLS::uint6_to_a64(tc.players[i].number,str); 
    str++;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.players[i].x) ),str); 
    str+= 3;
    WMTOOLS::int18_to_a3x64( int( rint(SCALE * tc.players[i].y) ),str); 
    str+= 3;
  }

  int check_sum= 0;
  while ( beg < str) {
    check_sum += *beg;
    beg++;
  }
  check_sum= abs(check_sum) % 63;
  WMTOOLS::uint6_to_a64(check_sum,str);
  str++;
  str[0]= '*';
  str++;
  end= str;
  return true;
}


bool SensorParser::manual_encode_teamcomm(char * str, const Msg_teamcomm2 & tc, char *& next) {
  next= str;
  
  //we can encode 6 bits / character
  //in version 9 we have 10 characters ---> 60 bits of information !!!

  int bits_num= max_bit_size;
  int bits_left= max_bit_size;

  //const double SCALE= ENCODE_SCALE; //shortcut
  unsigned int tmp;
  unsigned int tmp2;

  BitFIFO bfifo;

  if ( tc.msg.valid ) {
    bfifo.put(5,msg_id);
    tmp= tc.msg.param1;
    bfifo.put(16, tmp);
    tmp= tc.msg.param2;
    bfifo.put(16, tmp);
    tmp= tc.msg.type; //hauke
    bfifo.put(8, tmp);  //hauke
    bits_left -= 45;     //hauke -37
  }
  
  if ( tc.pass_info.valid ) {
    if ( ! pos_to_range13bit(tc.pass_info.ball_pos, tmp) ||
	 ! vel_to_range12bit(tc.pass_info.ball_vel, tmp2) )
      return false;
    if ( bits_left < pass_info_bit_size ) {
      WARNING_OUT << ID << "\nto much info for " << bits_num << " bits, skipping pass_info";
    } else {
      bfifo.put(5,pass_info_id);
      bfifo.put(13,  tmp);
      bfifo.put(12,  tmp2);
      if ( tc.pass_info.time < 0 || tc.pass_info.time > 127 )
        return false;
      tmp2= tc.pass_info.time;
      bfifo.put(7,  tmp2);
      bits_left -= pass_info_bit_size; //later on + 2 bits for the time
    }
  }

  bool ball_encoded= false;
  if ( tc.ball_info.valid && bits_left >= ball_info_bit_size ) {
    if ( ! pos_to_range13bit(tc.ball_info.ball_pos, tmp) ||
	 ! vel_to_range12bit(tc.ball_info.ball_vel, tmp2) )
      return false;

    if ( bits_left < ball_info_bit_size ) {
      WARNING_OUT << ID << "\nto much info for " << bits_num << " bits, skipping ball_info";
    } else {
      bfifo.put(5,ball_info_id);
      bfifo.put(13,  tmp);
      bfifo.put(12,  tmp2);
      if ( tc.ball_info.age_pos < 0 || tc.ball_info.age_pos > 7 )
	return false;
      tmp2= tc.ball_info.age_pos;
      bfifo.put(3,  tmp2);
      if ( tc.ball_info.age_vel < 0 || tc.ball_info.age_vel > 7 )
	return false;
      tmp2= tc.ball_info.age_vel;
      bfifo.put(3,  tmp2);
      bits_left -= ball_info_bit_size; //later on + 2 bits for the time
      ball_encoded= true;
    }
  }
  
  if ( !ball_encoded && tc.ball.valid ) {
    if ( ! pos_to_range13bit(tc.ball.pos, tmp) )
      return false;

    if ( bits_left < object_bit_size ) {
      WARNING_OUT << ID << "\nto much info for " << bits_num << " bits, skipping ball object";
    } else {
      bfifo.put(5,ball_id);
      bfifo.put(13,  tmp);
      bits_left -= object_bit_size;
    }
  }
  
  if ( tc.ball_holder_info.valid ) {
    if ( ! pos_to_range13bit(tc.ball_holder_info.pos, tmp) )
      return false;

    if ( bits_left < ball_holder_info_bit_size ) {
      WARNING_OUT << ID << "\nto much info for " << bits_num << " bits_left= " << bits_left;
      //return false;
    }
    else {
      bfifo.put(5,ball_holder_info_id);
      bfifo.put(13,  tmp);
      bits_left -= ball_holder_info_bit_size;
    }
  }
  

  for (int i=0; i<tc.players_num; i++) {
    if ( bits_left < object_bit_size ) {
      WARNING_OUT << ID << "\nto much info for " << bits_num << " bits, skipping " << tc.players_num -i << " players";
      break;
    }
    const Msg_teamcomm2::_player & p = tc.players[i];
    if ( ! pos_to_range13bit(p.pos, tmp) ) {
      ERROR_OUT << ID << "\n ! pos_to_range13bit pos= " << p.pos; 
      return false;
    }

    int number= p.number;
    if ( p.team != my_TEAM )
      number += 11;

    bfifo.put(5, number+1);
    bfifo.put(13, tmp);
    bits_left -= object_bit_size;
  }

  if ( bits_left == bits_num ) { //nothing to be encoded
    //hauke:
    WARNING_OUT << ID << " no info in teamcomm" << tc;
    *next= '\0';
    return true;
  }

  if (  bits_left > 0 ) {
    //cout << "\nfilling up with " << bits_left << " bits";
    if ( ! bfifo.fill_with_zeros(bits_left) )
      return false;
  }

  //cout << "\ncode= ";bfifo.show(cout);
  
  //now read the bit sequence in 6 bit chunks, each chunk is then encoded by one character
  for (int i=0; i<10; i++) {
    bfifo.get(6,tmp);
    if ( ! WMTOOLS::uint6_to_a64(tmp,next) )
      return false;
    next++;
  }

  return true;
}

/**
   parse_error_point is allowed to be == 0, in that case just the message 'origin' is shown
 */
void SensorParser::show_parser_error_point(std::ostream & out, const char * origin, const char * parse_error_point) {
  if (parse_error_point)
    for (  ; origin<parse_error_point && *origin!= '\0';origin++)
      out << *origin;
  else {
    out << origin
	<< "\n[no parse error point provided]";
  }
  
  if (origin != parse_error_point) {
    out << "\n[something wrong with parse error point]";
    return;
  }
  out << "   <***parse error***>   ";
  int i=0;
  while (i<25 && *origin != '\0') {
    out << *origin;
    origin++;
    i++;
  }
  if (*origin != '\0')
    out << " .......";
}

/*********************** T E S T *********************************************/
#if 0
using namespace std;

void test_ascii_converter() {
  int a;
  char buf[4]= "   ";

  for (int i= -131073; i < 131074; i++) {
    WMTOOLS::int18_to_a3x64(i,buf);
    WMTOOLS::a3x64_to_int18(buf,a);
    if (i != a || (i > 0 && i < 10) ) {
      cout << "\n" << i << " " << buf << " " << a;
      if (i!= a)
	cout << " error";
    }
  }

  buf[1]= '\0';
  for (int i= 0; i < 64; i++) {
    WMTOOLS::uint6_to_a64(i,buf);
    WMTOOLS::a64_to_uint6(buf,a);
    
    cout << "\n" << i << " " << buf << " " << a;
  }
}

void test_teamcomm() {
  Msg_teamcomm tc, tc2;
  tc.side= WM::my_side;
  tc.time= 13;
  tc.time_cycle= 1;
  tc.from= 9;
  tc.his_goalie_number_upd= true;
  tc.his_goalie_number= 11;
  tc.ball_upd= true;
  tc.ball.x= 1.1; tc.ball.y= 1.2; 
  tc.ball.vel_x= 1.1; tc.ball.vel_y= 1.2; 
  tc.players_num= 25;
  for (int i=0; i< tc.players_num; i++) {
    tc.players[i].how_old= i;
    if (i > 0 && i < 12) {
      tc.players[i].number= i;
      tc.players[i].team= my_TEAM;
    }
    else if (i >= 12 && i < 22) {
      tc.players[i].number= i-11;
      tc.players[i].team= his_TEAM;
    }
    else if (i==0) {
      tc.players[i].number= 0;
      tc.players[i].team= his_TEAM;
    }
    else {
      tc.players[i].number= 0;
      tc.players[i].team= unknown_TEAM;
    }
    tc.players[i].x= -111.11;
    tc.players[i].y=  111.11;
  }

  char buf[512];
  char * dum;
  const char * dum2;
  bool res= SensorParser::manual_encode_teamcomm(buf,tc,dum);
  dum[0]= ']'; dum[1]= '\0';
  bool res2= SensorParser::manual_parse_teamcomm(buf,tc2,dum2);
  cout << "\ncoding  res= " << res; 
  cout << "\nparsing res= " << res2;
  cout << "\n-- tc" << tc;
  cout << "\n-- tc2" << tc2;
  cout << "\nencoded string len= " << strlen(buf);
  cout << "\nencoded string " << buf;
  
}

void test_teamcomm2() {
  Vector pos(-15.51,14.11);
  Vector pos2;
  unsigned int val;
  cout << "\n=== -1" << flush;
  SensorParser::pos_to_range13bit(pos,val);
  SensorParser::range13bit_to_pos(val,pos2);
  cout << "\npos= " << pos << " pos2= " << pos2;

  Vector vel(-2.76,1.14);
  Vector vel2;
  SensorParser::vel_to_range12bit(vel,val);
  SensorParser::range12bit_to_vel(val,vel2);
  cout << "\nvel= " << vel << " vel2= " << vel2;
  //////////////////////////////////////////////////////////
  Msg_teamcomm2 tc, tc2;
  
  tc.msg.valid= true;
  tc.msg.param1= 32000;
  tc.msg.param2= -32000;
  tc.ball.valid= true;
  tc.ball.pos= Vector(12.1, 21.0); 
  tc.ball_holder_info.valid= true;
  tc.ball_holder_info.pos=  Vector(13.4,-29.3);
#if 0
  tc.pass_info.valid= true;

  tc.pass_info.ball_pos= Vector(50.2,31.1);

  tc.pass_info.ball_vel= Vector(2.7,1.13);
  tc.pass_info.time= 127;
#endif
#if 0
  tc.ball_info.valid= true;

  tc.ball_info.ball_pos= Vector(50.2,31.1);

  tc.ball_info.ball_vel= Vector(2.7,1.13);
  tc.ball_info.age_pos= 3;
  tc.ball_info.age_vel= 7;
#endif
  tc.players_num= 2;
  for (int i= 0; i< tc.players_num; i++) {
    int n= i+11;
    if ( n < 12)
      tc.players[i].number= n;
    else
      tc.players[i].number= n - 11;

    if ( tc.players[i].number < 12 )
      tc.players[i].team= my_TEAM;
    else
      tc.players[i].team= his_TEAM;
    tc.players[i].pos.x= -15.11;
    tc.players[i].pos.y=  15.11;
  }

  char buf[512];
  char * dum;
  const char * dum2;
  bool res= SensorParser::manual_encode_teamcomm(buf,tc,dum);
  dum[0]= '\0';

  bool res2= SensorParser::manual_parse_teamcomm(buf,tc2,dum2);

  cout << "\ncoding  res= " << res; 
  cout << "\nparsing res= " << res2;
  cout << "\n-- tc " << tc;
  cout << "\n-- tc2" << tc2;
  cout << "\nencoded string len= " << strlen(buf);
  cout << "\nencoded string [" << buf << "]";

#if 0
  cout << "\n----------------";
  res2= SensorParser::manual_parse_teamcomm("iqPknP9qE6",tc2,dum2);
  cout << "\nparsing res= " << res2
       << "\nremainder: " << dum2;
  cout << "\n-- tc2" << tc2;
#endif
}

#include "default_server_param.h"
#include <iomanip>

bool produce_server_param_parser(char const* param_str) {
  /* generates a parser for the server_param string */
  int max_size=200;
  int size= 0;
  char ** tab= new char*[max_size];
  char const* str= 0;
  if ( strskip(param_str,"server_param")) {
    str= DEFAULT_MESSAGE_SERVER_PARAM;
    strskip(str,"(server_param ",str);
  }
  else if ( strskip(param_str,"player_param")) {
    str= DEFAULT_MESSAGE_PLAYER_PARAM;
    strskip(str,"(player_param ",str);
  }  
  else {
    ERROR_OUT << ID << " do not recognize " << param_str;
    return false;
  }
  char const* dum;
  char const* dum2;

  bool res= true;
  while (true) {
    res= strskip(str,'(',str)  && strfind(str,' ',dum) && strfind(dum,')',dum2);
    if ( ! res ) {
      res= strskip(str,')',str);
      break;
    }
    tab[size]= new char[dum-str+1];
    strncpy(tab[size],str,dum-str);
    tab[size][dum-str]= '\0';
#if 0
    cout << "\n" << size << "  [";
    while ( str < dum )
      cout << *str++;
    cout << "]";
    cout << " : " << tab[size]; 
#endif
    size++;
    str= dum2+1;
    //if ( size > 1) break;
  }
  if ( ! res ) {
    ERROR_OUT << ID << "something wrong with the " << param_str << " string";
    return false;
  }
  
  /* sort the server param decreasingly (very important, increasing order will 
     not work because of strskip(...) semantics 
  */
  for (int i=0; i<size; i++) 
    for (int j=i+1; j<size; j++) 
      if ( strcmp(tab[i],tab[j]) < 0 ) {
	char * dum= tab[i];
	tab[i]= tab[j];
	tab[j]= dum;
      }

  
  //for (int i=0; i<size; i++) { cout << "\n" << setw(3) << i << " : [" << tab[i] << "] , "; }
  
  //output 
  const char SPACE0[]="\n";
  const char SPACE1[]="\n  ";
  const char SPACE2[]="\n    ";
  const char SPACE3[]="\n      ";

  cout << SPACE0 << "/* automatically generated from a server_param message using"
       << SPACE0 << "   bool produce_server_param_parser()"
       << SPACE0 
       << SPACE0 << "                  DO NOT EDIT THIS FILE !!!"
       << SPACE0 << "*/"
       << SPACE0 << "#include \"sensorparser.h\""
       << SPACE0 << "#include \"str2val.h\""
       << SPACE0 << "#include \"macro_msg.h\""
       << SPACE0  
       << SPACE0 << "bool SensorParser::manual_parse_" << param_str << "(const char * str, Msg_" << param_str << " & param) {"
       << SPACE1 << "const char * origin= str;"
       << SPACE1
       << SPACE1 << "bool res;"
       << SPACE1 
       << SPACE1 << "char const* dum;"
       << SPACE1 << "res= strskip(str,\"(" << param_str << "\",str);";

  cout << SPACE1 << "while (res) {"
       << SPACE2  << "res= strskip(str,'(',str);"
       << SPACE2  << "if ( ! res ) {"
       << SPACE2  << "  res= strskip(str,')',str);"
       << SPACE2  << "  break;"
       << SPACE2  << "}"
       << SPACE2  << "bool unknown_option= false;"
       << SPACE2  << "switch( *str ) {";
    
  //for (int i=0; i<size; i+= 20) {
  for (int i=0; i<size; i++) {
    bool new_block= i==0 || tab[i][0] != tab[i-1][0];
    if (new_block) {
      if ( i != 0 )
	cout << SPACE3 << "unknown_option= true;"
	     << SPACE3 << "break;";
      cout << SPACE3 << "// ----------------------------------------------------------------"
	   << SPACE2  << "case '" << tab[i][0] << "': ";
    }
    if (new_block)
      cout << SPACE3 << "if ";
    else
      cout << SPACE3 << "else if ";
    
    cout << "( strskip(str,\"" << tab[i] << "\",dum) )  {"
	 << SPACE3 << "  str= dum;"
	 << SPACE3 << "  res= str2val(str,param."<< tab[i] << ",str) && param.read_" << tab[i] << ".set_ok() && strskip(str,')',str);"
	 << SPACE3 << "  break;"
	 << SPACE3 << "}";
  }
  cout << SPACE2  << "default: "
       << SPACE2  << "  unknown_option= true;"
       << SPACE2  << "}"
       << SPACE2  << "if ( unknown_option ) {"
       << SPACE2  << "  WARNING_OUT << \"\\nunkown server option [\";"
       << SPACE2  << "    while ( *str != '\\0' && *str != ')' )"
       << SPACE2  << "      WARNING_STREAM << *str++;"
       << SPACE2  << "    WARNING_STREAM << \"]\";"
       << SPACE2  << "    if ( *str == ')' )"
       << SPACE2  << "      str++;"
       << SPACE2  << "    else"
       << SPACE2  << "      res= false;"
       << SPACE2  << "}"
       << SPACE1  << "} //while";

  cout << SPACE1  
       << SPACE1 << "if (!res) {"
       << SPACE2 << "ERROR_OUT << \"\\nparse error:\\n\";"
       << SPACE2 << "show_parser_error_point(ERROR_STREAM,origin,str);"
       << SPACE1 << "}"
       << SPACE1 << "return res;"
       << SPACE0 << "}"
       << std::endl;

  return true;
}


int main(int argc, char ** argv) {
  bool res;
#if 0
  res= produce_server_param_parser("server_param");
  res= produce_server_param_parser("player_param");
  cerr << "\nRESULT= " << res;
  return 1;
#endif
  WM::my_side= right_SIDE;
  test_teamcomm2(); return 1;
#if 0
  //test_ascii_converter(); return 1;
  //WM::my_side= right_SIDE;
  WM::my_side= left_SIDE;
  strcpy(WM::my_team_name,"brain");
  WM::my_team_name_len= strlen(WM::my_team_name);
  //ServerOptions::goal_width= 11111.0;//just for test
  WM::my_number= 1;
  Msg_see see;
  Msg_fullstate fullstate;
  Msg_fullstate_v8 fullstate_v8;
  Msg_hear hear;
  Msg_init init;
  Msg_sense_body sb;
  Msg_server_param sp;
  Msg_player_param pp;
  Msg_player_type pt;
  
  //bool res;
  const bool show_result= false;
  char buf_sb1[] = "(sense_body 0 (view_mode high normal) (stamina 3500 1)";
  char buf_sb0[] = "(sense_body 16 (view_mode high normal) (stamina 4000.0 0.9) (speed 1.1 1.2) (head_angle 90.1) (kick 1) (dash 1) (turn 1) (say 1) (turn_neck 0) (catch 0) (move 0) (change_view 0))";
  char buf_see[] = "(see 17 ((B) 1.1 1.2 1.3 1.4) ((l l) 1.1 1.2) ((f c) 1.1 1.2 1.3 1.4) ((f c t) 1.1 1.2 1.3 1.4) ((f c b) 1.1 1.2 1.3 1.4) ((f l t) 1.1 1.2 1.3 1.4) ((f l b) 1.1 1.2 1.3 1.4) ((f r t) 1.1 1.2 1.3 1.4) ((f r t) 1.1 1.2 1.3 1.4) ((f p l t) 1.1 1.2 1.3 1.4) ((f p l c) 1.1 1.2 1.3 1.4) ((f p l b) 1.1 1.2 1.3 1.4)((f p r t) 1.1 1.2 1.3 1.4) ((f p r c) 1.1 1.2 1.3 1.4) ((f p r b) 1.1 1.2 1.3 1.4) ((f g l t) 1.1 1.2 1.3 1.4) ((g l) 1.1 1.2 1.3 1.4) ((f g l b) 1.1 1.2 1.3 1.4) ((f g r t) 1.1 1.2 1.3 1.4) ((g r) 1.1 1.2 1.3 1.4) ((f g r b) 1.1 1.2 1.3 1.4) ((f l t 30) 1.1 1.2 1.3 1.4) ((f l t 20) 1.1 1.2 1.3 1.4) ((f l t 10) 1.1 1.2 1.3 1.4) ((f l 0) 1.1 1.2 1.3 1.4) ((f l b 10) 1.1 1.2 1.3 1.4) ((f l b 20) 1.1 1.2 1.3 1.4) ((f l b 30) 1.1 1.2 1.3 1.4) ((f r t 30) 1.1 1.2 1.3 1.4) ((f r t 20) 1.1 1.2 1.3 1.4) ((f r t 10) 1.1 1.2 1.3 1.4) ((f r 0) 1.1 1.2 1.3 1.4) ((f r b 10) 1.1 1.2 1.3 1.4) ((f r b 20) 1.1 1.2 1.3 1.4) ((f r b 30) 1.1 1.2 1.3 1.4) ((f t l 50) 1.1 1.2 1.3 1.4) ((f t l 40) 1.1 1.2 1.3 1.4) ((f t l 30) 1.1 1.2 1.3 1.4) ((f t l 20) 1.1 1.2 1.3 1.4) ((f t l 10) 1.1 1.2 1.3 1.4) ((f t r 50) 1.1 1.2 1.3 1.4) ((f t r 40) 1.1 1.2 1.3 1.4) ((f t r 30) 1.1 1.2 1.3 1.4) ((f t r 20) 1.1 1.2 1.3 1.4) ((f t r 10) 1.1 1.2 1.3 1.4) ((f t 0) 1.1 1.2 1.3 1.4) ((f b l 50) 1.1 1.2 1.3 1.4) ((f b l 40) 1.1 1.2 1.3 1.4) ((f b l 30) 1.1 1.2 1.3 1.4) ((f b l 20) 1.1 1.2 1.3 1.4) ((f b l 10) 1.1 1.2 1.3 1.4) ((f b r 50) 1.1 1.2 1.3 1.4) ((f b r 40) 1.1 1.2 1.3 1.4) ((f b r 30) 1.1 1.2 1.3 1.4) ((f b r 20) 1.1 1.2 1.3 1.4) ((f b r 10) 1.1 1.2 1.3 1.4) ((f b 0) 1.1 1.2 1.3 1.4) ((p \"brain\" 1 goalie) 1.1 1.2 1.3 1.4 1.5 1.6) ((p \"brain\" 1) 1.1 1.2 1.3 1.4 1.5 1.6)((p \"brain\") 1.1 1.2 1.3 1.4 1.5 1.6)((p) 1.1 1.2 1.3 1.4 1.5 1.6)((p \"otherteam\" 1 goalie) 1.1 1.2 1.3 1.4 1.5 1.6)((p \"otherteam\" 1) 1.1 1.2 1.3 1.4 1.5 1.6)((p \"otherteam\") 1.1 1.2 1.3 1.4 1.5 1.6)((p) 1.1 1.2 1.3 1.4 1.5 1.6))";
  char buf_init[] = "(init r 2 kick_off_l)";
  char buf_full[] = "(fullstate 441 (pmode play_on) (vmode high normal) (score 12 1) (ball 7 -23.4 0 0) (l_1 -4 -28.9 0 0 0 0 4000 1 1) (l_2 6.06 -26.9 0 0 0 0 4000 1 1) (l_3 7.31 -27.1 0 0 0 0 4000 1 1) (l_4 8.63 -26.9 -0 -0 0 0 4000 1 1) (l_5 9.23 -26.9 -0 -0 0 0 4000 1 1) (l_6 9.83 -26.9 -0 -0 0 0 4000 1 1) (l_7 9.83 -26.9 -0 -0 0 0 4000 1 1) (l_8 9.83 -26.9 -0 -0 0 0 4000 1 1) (l_9 10.4 -26.8 -0 -0 0 0 4000 1 1) (l_10 9.23 -26.8 -0 -0 0 0 4000 1 1) (l_11 11 -26.8 -0 -0 0 0 4000 1 1) (r_1 4.66 -32.3 0 0 0 0 4000 1 1) (r_2 5.33 -32.4 0 0 0 0 4000 1 1) (r_3 6.04 -32.5 -0 -0 0 0 4000 1 1) (r_4 6.76 -32.6 -0 -0 0 0 4000 1 1) (r_5 7.48 -32.6 -0 -0 0 0 4000 1 1) (r_6 8.2 -32.5 0 0 0 0 4000 1 1) (r_7 8.89 -32.4 0 0 0 0 4000 1 1) (r_8 9.56 -32.2 0 0 0 0 4000 1 1) (r_9 6.04 -32.5 -0 -0 0 0 4000 1 1) (r_10 6.76 -32.6 -0 -0 0 0 4000 1 1) (r_11 7.48 -32.6 -0 -0 0 0 4000 1 1))";
  char buf_full_v8[]= "(fullstate 0 (pmode before_kick_off) (vmode high normal) (stamina 4e+03 1 1) (count 0 0 0 0 0 0 0 0) (score 0 0) ((b) 0 0 0 0) ((p l 1 g) -3 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p l 2 0) -6 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p l 3 0) -9 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p l 4 0) -12 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p r 1 0) 3 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p r 2 0) 6 -37 0 0 0 0 (stamina 4e+03 1 1)) ((p r 3 0) 9 -37 0 0 0 0 (stamina 4e+03 1 1)))";
  char buf_hear1[]= "(hear 3000 referee before_kick_off)";
  char buf_hear2[]= "(hear 34 referee goal_l_122)";
  char buf_hear3[]= "(hear 34 0 \"***1W0CW0091B1+W2aW2pVzQVzBP010Rxba4P101Rxba4P202Rxba4P303Rxba4P404Rxba4P505Rxba4P606Rxba4P707Rxba4P808Rxba4P909Rxba4PA0ARxba4PB0BRxba4PC11Rxba4PD12Rxba4PE13Rxba4PF14Rxba4PG15Rxba4PH16Rxba4PI17Rxba4PJ18Rxba4PK19Rxba4PL1ARxba4PM20Rxba4PN20Rxba4PO20Rxba4Pz*\")";
  char buf_sp[]= "(server_param (goal_width 14.02) (player_size 0.3) (player_decay 0.4) (player_rand 0.1) (player_weight 60) (player_speed_max 1) (player_accel_max 1) (stamina_max 4000) (stamina_inc_max 45) (recover_dec_thr 0.3) (recover_min 0.5) (recover_dec 0.002) (effort_dec_thr 0.3) (effort_min 0.6) (effort_dec 0.005) (effort_inc_thr 0.6) (effort_inc 0.01) (kick_rand 0) (team_actuator_noise 0) (prand_factor_l 1) (prand_factor_r 1) (kick_rand_factor_l 1) (kick_rand_factor_r 1) (ball_size 0.085) (ball_decay 0.94) (ball_rand 0.05) (ball_weight 0.2) (ball_speed_max 2.7) (ball_accel_max 2.7) (dash_power_rate 0.006) (kick_power_rate 0.027) (kickable_margin 0.7) (control_radius 2) (catch_probability 1) (catchable_area_l 2) (catchable_area_w 1) (goalie_max_moves 2) (maxpower 100) (minpower -100) (maxmoment 180) (minmoment -180) (maxneckmoment 180) (minneckmoment -180) (maxneckang 90) (minneckang -90) (visible_angle 1.5708) (visible_distance 3) (audio_cut_dist 50) (quantize_step 0.1) (quantize_step_l 0.01) (quantize_step_dir -1) (quantize_step_dist_team_l -1) (quantize_step_dist_team_r -1) (quantize_step_dist_l_team_l -1) (quantize_step_dist_l_team_r -1) (quantize_step_dir_team_l -1) (quantize_step_dir_team_r -1) (ckick_margin 1) (wind_dir 0) (wind_force 0) (wind_rand 0) (inertia_moment 5) (wind_none 0) (wind_random 0) (half_time 3000) (drop_ball_time 200) (port 6000) (coach_port 6001) (olcoach_port 6002) (say_coach_cnt_max 128) (say_coach_msg_size 128) (simulator_step 100) (send_step 150) (recv_step 10) (sense_body_step 100) (say_msg_size 512) (clang_win_size 300) (clang_define_win 1) (clang_meta_win 1) (clang_advice_win 1) (clang_info_win 1) (clang_mess_delay 50) (clang_mess_per_cycle 1) (hear_max 2) (hear_inc 1) (hear_decay 2) (catch_ban_cycle 5) (coach 0) (coach_w_referee 0) (old_coach_hear 0) (send_vi_step 100) (use_offside 1) (offside_active_area_size 5) (forbid_kick_off_offside 0) (verbose 0) (replay ) (offside_kick_margin 9.15) (slow_down_factor 1) (synch_mode 0) (synch_offset 60) (synch_micro_sleep 1) (start_goal_l 0) (start_goal_r 0) (fullstate_l 1) (fullstate_r 1) (slowness_on_top_for_left_team 1) (slowness_on_top_for_right_team 1) (send_comms 0) (text_logging 1) (game_logging 1) (game_log_version 3) (text_log_dir ./) (game_log_dir ./) (text_log_fixed_name rcssserver) (game_log_fixed_name rcssserver) (text_log_fixed 1) (game_log_fixed 1) (text_log_dated 0) (game_log_dated 0) (log_date_format %Y%m%d%H%M-) (log_times 0) (record_messages 0) (text_log_compression 0) (game_log_compression 0))";
  char buf_pp[]= "(player_param (player_types 7) (subs_max 3) (pt_max 3) (player_speed_max_delta_min 0) (player_speed_max_delta_max 0.2) (stamina_inc_max_delta_factor -100) (player_decay_delta_min 0) (player_decay_delta_max 0.2) (inertia_moment_delta_factor 25) (dash_power_rate_delta_min 0) (dash_power_rate_delta_max 0.002) (player_size_delta_factor -100) (kickable_margin_delta_min 0) (kickable_margin_delta_max 0.2) (kick_rand_delta_factor 0.5) (extra_stamina_delta_min 0) (extra_stamina_delta_max 100) (effort_max_delta_factor -0.002) (effort_min_delta_factor -0.002) (random_seed -1))";
  char buf_pt[]="(player_type (id 0) (player_speed_max 1) (stamina_inc_max 45) (player_decay 0.4) (inertia_moment 5) (dash_power_rate 0.006) (player_size 0.3) (kickable_margin 0.7) (kick_rand 0) (extra_stamina 0) (effort_max 1) (effort_min 0.6))";

  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing see message:\n";// << buf_see;
  res= SensorParser::manual_parse_see(buf_see,see);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << see;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_init;
  res= SensorParser::manual_parse_init(buf_init,init);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout <<  "\nfound: " << init;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing old fullstate format:\n";// << full;
  res= SensorParser::manual_parse_fullstate(buf_full,fullstate);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << fullstate;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing fullstate version 8:\n";// << buf_full_v8;
  res= SensorParser::manual_parse_fullstate(buf_full_v8,fullstate_v8);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << fullstate_v8;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_sb0;
  res= SensorParser::manual_parse_sense_body(buf_sb0,sb);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << sb;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_hear2;
  res= SensorParser::manual_parse_hear(buf_hear2,hear);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << hear;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_hear3;
  res= SensorParser::manual_parse_hear(buf_hear3,hear);
  cout << "\nParser res= " << res;
  if (show_result) 
    cout << "\nfound: " << hear;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing server parameter:\n";// << buf_sp;
  res= SensorParser::manual_parse_server_param(buf_sp,sp);
  cout << "\nParser res= " << res;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_pp;
  res= SensorParser::manual_parse_player_param(buf_pp,pp);
  cout << "\nParser res= " << res;
  cout << "\n----------------------------------------------------";
  cout << "\n--\nparsing:\n" << buf_pt;
  res= SensorParser::manual_parse_player_type(buf_pt,pt);
  cout << "\nParser res= " << res;
  //test_teamcomm();
  return 1;
#endif
}
#endif 

