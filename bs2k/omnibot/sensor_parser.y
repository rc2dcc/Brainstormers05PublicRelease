%{

  /**
     \file sensor_parser.y
     This is the general parser for all information coming from the server.
     If you want to add additional functionality (e.g. communication between
     clients, simply add your grammar and actions. Maybe you will have 
     to modify sensor_lexer.lex.

     invoke bison at least with options : -b si -p si
     @author (of an earlier version) Alex Sinner
     @author Artur Merke
  */
#include <stdio.h>  
#include <stdlib.h>  
#include <iostream.h>

  /** This file is needed for information about variable flags */
#include "options.h"
  /** This file contains the SensorBuffer class */
#include "sensorbuffer.h"
  /** YYPARSE_PARAM is the argument passed to spparse(). */
#define YYPARSE_PARAM sensor_buffer
  //#define YYDEBUG 1
  
  int sierror(char* s){
    cerr << "\nParser: ERROR: >>" << s << "<<";
    char ch; cin >> ch;
    return 0;
  }

  extern int silex();
  //  extern YY_BUFFER_STATE si_scan_string(char *);

%}
%union {
  double dval;
  int ival;
  char *sval;
}

%start start

/* for error handling */
%token SI_ERROR UNKNOWN_COMMAND ILLEGAL_COMMAND_FORM

/* opening/closing bracket tokens */
%token OB CB

/* server initialisation message tokens */
%token SENSE_BODY SEE HEAR SI_INIT

/* extra directions for HEAR */
%token REFEREE SELF

/* SEE related tokens */
%token GOAL FLAG LINE BALL PLAYER
%token _L_ _R_ _T_ _B_ _C_ _P_ _G_

%token <dval> DOUBLE DIRECTION
%token <ival> INT UNUM SIDE PMODE VIEW_QUALITY VIEW_WIDTH WHICHTEAM
%token <sval> STRING 

/* sense_body tokens */ 
%token VIEW_MODE STAMINA SPEED HEAD_ANGLE SI_KICK SI_DASH SI_TURN SI_SAY SI_TURN_NECK GOALIE

/* mdpstate tokens (same are already defined in sense_body tokens) */
%token MDP SI_SCORE

/* here you might define your own message tokens, if you want to include communication between the players */
%%

start: /*empty*/ {;}
| OB server_message CB start {;}
;

server_message: see {;}
| hear {;}
| sense_body {;}
| error {;}
| init {;}
| mdpstate {;}
;

init: SI_INIT SIDE INT PMODE {
  ((SensorBuffer *)sensor_buffer)->init->side = $2;
  ((SensorBuffer *)sensor_buffer)->init->number = $3;
  ((SensorBuffer *)sensor_buffer)->init->play_mode = PlayMode($4);
}
;

mdpstate: MDP INT {
  ((SensorBuffer *)sensor_buffer)->mdpstate->time = $2;
  ((SensorBuffer *)sensor_buffer)->mdpstate->players_num = 0;
} mdp_objects
;

mdp_objects: OB mdp_object CB {;}
| OB mdp_object CB mdp_objects {;}
;

mdp_object: mdp_player {;}
| mdp_ball {;}
| mdp_vmode {;}
| mdp_pmode {;}
| mdp_score {;}
;

mdp_player: SIDE UNUM DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE {
  int & num= ((SensorBuffer *)sensor_buffer)->mdpstate->players_num;
  if ( num < Msg_mdpstate::players_MAX) {
    Msg_mdpstate::_mdp_player & p= ((SensorBuffer *)sensor_buffer)->mdpstate->players[num];
    p.team= $1;
    p.number= $2;
    p.x= $3;
    p.y= $4;
    p.vel_x= $5;
    p.vel_y= $6;
    p.angle= $7;
    p.neck_angle= $8;
    p.stamina= $9;
    p.effort= $10;
    p.recovery= $11;
    num++;
  }
  else
    cerr << "\nParser: to much (mdp)players";
}
;

mdp_ball: BALL DOUBLE DOUBLE DOUBLE DOUBLE {
  Msg_mdpstate::_mdp_ball & b= ((SensorBuffer *)sensor_buffer)->mdpstate->ball;
  b.x= $2;
  b.y= $3;
  b.vel_x= $4;
  b.vel_y= $5;
}

mdp_vmode: VIEW_MODE VIEW_QUALITY VIEW_WIDTH {
  ((SensorBuffer *)sensor_buffer)->mdpstate->view_quality = $2;
  ((SensorBuffer *)sensor_buffer)->mdpstate->view_width = $3;
}
;

mdp_pmode: PMODE {
  ((SensorBuffer *)sensor_buffer)->mdpstate->play_mode = PlayMode($1);
}
;

mdp_score: SI_SCORE INT INT {
  Msg_mdpstate & mdp= *(((SensorBuffer *)sensor_buffer)->mdpstate);
  if ( WM::my_side == left_SIDE) {
    mdp.my_score= $2;
    mdp.his_score= $3;
  }
  else {
    mdp.his_score= $2;
    mdp.my_score= $3;
  }
}
;

see: SEE INT {
  ((SensorBuffer *)sensor_buffer)->see->time = $2;
  ((SensorBuffer *)sensor_buffer)->see->markers_num = 0;
  ((SensorBuffer *)sensor_buffer)->see->players_num = 0;
  ((SensorBuffer *)sensor_buffer)->see->line_upd = false;
  ((SensorBuffer *)sensor_buffer)->see->ball_upd= false;
}  see_objects 
;

see_objects: /*empty */ {;}
| OB see_object CB see_objects{;}
;

see_object: see_marker {;} 
| see_player {;} 
| see_ball {;}
| see_line {;}
;

see_ball: OB BALL CB DOUBLE DOUBLE DOUBLE DOUBLE {
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.ball_upd) {
    see.ball.how_many= 4;
    see.ball.dist = $4;
    see.ball.dir = $5;
    see.ball.dist_change = $6;
    see.ball.dir_change = $7;
    see.ball_upd= true;
  }
  else
    cerr << "\nParser: more then one ball";
}
| OB BALL CB DOUBLE DOUBLE {
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.ball_upd) {
    see.ball.how_many= 2;
    see.ball.dist = $4;
    see.ball.dir = $5;
    see.ball.dist_change = UNDEF_INFORMATION;
    see.ball.dir_change = UNDEF_INFORMATION;
    see.ball_upd= true;
  }
  else
    cerr << "\nParser: more then one ball";
}
;

see_player: see_player_object DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE {
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 6;
    p.dist = $2;
    p.dir = $3;
    p.dist_change = $4;
    p.dir_change = $5;
    p.body_dir = $6;
    p.head_dir = $7;
    num++;
  }
  else
    cerr << "\nParser: to much players";
}
| see_player_object DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE {
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 5;
    p.dist = $2;
    p.dir = $3;
    p.dist_change = $4;
    p.dir_change = $5;
    p.body_dir = $6;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
}
| see_player_object DOUBLE DOUBLE DOUBLE DOUBLE  {
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 4;
    p.dist = $2;
    p.dir = $3;
    p.dist_change = $4;
    p.dir_change = $5;
    p.body_dir = UNDEF_INFORMATION;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
}
| see_player_object DOUBLE DOUBLE {
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 2;
    p.dist = $2;
    p.dir = $3;
    p.dist_change = UNDEF_INFORMATION;
    p.dir_change = UNDEF_INFORMATION;
    p.body_dir = UNDEF_INFORMATION;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
}
;

see_marker: OB see_marker_object CB DOUBLE DOUBLE DOUBLE DOUBLE {
  int & num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.how_many= 4;
    m.dist = $4;
    m.dir = $5;
    m.dist_change = $6;
    m.dir_change = $7;
    num++;
  }
  else
    cerr << "\nParser: to much markers";
}
| OB see_marker_object CB DOUBLE DOUBLE {
  int & num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.how_many= 2;
    m.dist = $4;
    m.dir = $5;
    m.dist_change = UNDEF_INFORMATION;
    m.dir_change = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much markers";
}
;

see_line: OB see_line_object CB DOUBLE DOUBLE DOUBLE DOUBLE {
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.line_upd) {
    see.line.how_many= 4;
    see.line.dist = $4;
    see.line.dir = $5;
    see.line.dist_change = $6;
    see.line.dir_change = $7;
    see.line_upd= true;
  }
  else
    cerr << "\nParser: more then one line";
}
| OB see_line_object CB DOUBLE DOUBLE {
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.line_upd) {
    see.line.how_many= 2;
    see.line.dist = $4;
    see.line.dir = $5;
    see.line.dist_change = UNDEF_INFORMATION;
    see.line.dir_change = UNDEF_INFORMATION;
    see.line_upd= true;
  }
  else
    cerr << "\nParser: more then one line";
}
;

hear: HEAR INT REFEREE PMODE {
  Msg_hear & hear= *(((SensorBuffer *)sensor_buffer)->hear);
  hear.time= $2;
  hear.play_mode_upd= true;
  hear.play_mode = PlayMode($4);
}
| HEAR INT REFEREE PMODE INT {
  Msg_hear & hear= *(((SensorBuffer *)sensor_buffer)->hear);
  hear.time= $2;
  hear.play_mode_upd= true;
  hear.play_mode = PlayMode($4);
  if (hear.play_mode == PM_goal_l)
    if ( WM::my_side == left_SIDE) {
      hear.my_score_upd= true;
      hear.my_score= $5;
    }
    else {
      hear.his_score_upd= true;
      hear.his_score= $5;
    }
}
| HEAR INT INT communication {
  ((SensorBuffer *)sensor_buffer)->hear->time= $2;
}
| HEAR INT SELF communication {
  ((SensorBuffer *)sensor_buffer)->hear->time= $2;
}
;

sense_body: SENSE_BODY INT sbi {
  ((SensorBuffer *)sensor_buffer)->sb->time = $2;
}
;

see_player_object: OB PLAYER WHICHTEAM UNUM GOALIE CB {
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= $3;
    p.number= $4;
    p.goalie= true;
  }
}
| OB PLAYER WHICHTEAM UNUM CB {
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= $3;
    p.number= $4;
    p.goalie= false;
  }
}
| OB PLAYER WHICHTEAM CB{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= $3;
    p.number= UNDEF_INFORMATION;
    p.goalie= false;
  }
}
| OB PLAYER CB{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= unknown_TEAM;
    p.number= UNDEF_INFORMATION;
    p.goalie= false;
  }
}
;

see_marker_object: GOAL _L_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = 0;
  }
} 
| GOAL _R_ {
  //cerr << "<goal r>";
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = 0;
  }
}
| FLAG _C_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0.0;
    m.y = 0.0;
  }
}
| FLAG _L_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = PITCH_WIDTH/2.0;
  }
}
| FLAG _R_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = PITCH_WIDTH/2.0;
  }
} 
| FLAG _C_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0;
    m.y = PITCH_WIDTH/2.0;
  }
}
| FLAG _L_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = -PITCH_WIDTH/2.0;
  }
}
| FLAG _R_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = -PITCH_WIDTH/2.0;
  }
}
| FLAG _C_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0;
    m.y = -PITCH_WIDTH/2.0;
  }
}
| FLAG _P_ _L_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = PENALTY_AREA_WIDTH/2.0;
  }
}
| FLAG _P_ _R_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = PENALTY_AREA_WIDTH/2.0;
  }
}
| FLAG _P_ _L_ _C_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = 0.0;
  }
}
| FLAG _P_ _R_ _C_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = 0.0;
  }
}
| FLAG _P_ _L_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = -PENALTY_AREA_WIDTH/2.0;
  }
}
| FLAG _P_ _R_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = -PENALTY_AREA_WIDTH/2.0;
  }
}
| FLAG _T_ _L_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(-$4);
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
}
| FLAG _T_ _R_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double($4);
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
}
| FLAG _B_ _L_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(-$4);
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
}
| FLAG _B_ _R_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double($4);
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
}
| FLAG _L_ _T_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = double($4);
  }
}
| FLAG _L_ _B_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = double(-$4);
  }
}
| FLAG _R_ _T_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = double($4);
  }
}
| FLAG _R_ _B_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = double(-$4);
  }
}
| FLAG _L_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = 0.0;
  }
}
| FLAG _R_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = 0.0;
  }
}
| FLAG _T_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = 0.0;
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
}
| FLAG _B_ INT {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = 0.0;
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
}
| FLAG _G_ _L_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2;
    m.y = ServerOptions::goal_width/2;
  }
}
| FLAG _G_ _R_ _T_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2;
    m.y = ServerOptions::goal_width/2;
  }
}
| FLAG _G_ _L_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = -PITCH_LENGTH/2;
    m.y = -ServerOptions::goal_width/2;
  }
}
| FLAG _G_ _R_ _B_ {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2;
    m.y = -ServerOptions::goal_width/2;
  }
}
| FLAG {
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= false; 
    m.x = UNDEF_INFORMATION;
    m.y = UNDEF_INFORMATION;
  }
}
| GOAL { 
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= false;
    m.x = UNDEF_INFORMATION;
    m.y = UNDEF_INFORMATION;
  }
}
;

see_line_object: LINE _L_ {
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = -PITCH_LENGTH/2;
    l.y = 0.0;
  }
}
| LINE _R_ {
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = PITCH_LENGTH/2;
    l.y = 0.0;
  }
}
| LINE _T_ {
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = 0.0;
    l.y = PITCH_WIDTH/2;
  }
}
| LINE _B_ {
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = 0.0;
    l.y = -PITCH_WIDTH/2;
  }
}
;

sbi: /*empty */{;}
| sbi OB VIEW_MODE VIEW_QUALITY VIEW_WIDTH CB {
  ((SensorBuffer *)sensor_buffer)->sb->view_quality = $4;
  ((SensorBuffer *)sensor_buffer)->sb->view_width = $5;
}
| sbi OB STAMINA DOUBLE DOUBLE CB {
  ((SensorBuffer *)sensor_buffer)->sb->stamina = $4;
  ((SensorBuffer *)sensor_buffer)->sb->effort = $5;
}
| sbi OB SPEED DOUBLE DOUBLE CB 
{
  ((SensorBuffer *)sensor_buffer)->sb->speed_value = $4;
  ((SensorBuffer *)sensor_buffer)->sb->speed_angle = $5;
}
| sbi OB HEAD_ANGLE DOUBLE CB 
{
  ((SensorBuffer *)sensor_buffer)->sb->neck_angle = $4;
}
| sbi OB SI_KICK INT CB 
{
  ((SensorBuffer *)sensor_buffer)->sb->kick_count = $4;
}
| sbi OB SI_DASH INT CB 
{
  ((SensorBuffer *)sensor_buffer)->sb->dash_count = $4;
}
| sbi OB SI_TURN INT CB 
{((SensorBuffer *)sensor_buffer)->sb->turn_count = $4;}
| sbi OB SI_SAY INT CB 
{((SensorBuffer *)sensor_buffer)->sb->say_count = $4;}
| sbi OB SI_TURN_NECK INT CB 
{((SensorBuffer *)sensor_buffer)->sb->turn_neck_count = $4;}
;

/* here you can integrate your own communication grammar */
communication: STRING {
  ((SensorBuffer *)sensor_buffer)->hear->communication_upd= true;
}
;

%%
#include "lex.si.c"

void parse_sensor_data(char* argv, void *sensor_buffer){
  si_scan_string(argv);
  if (siparse(sensor_buffer)){
    cerr << "\n parse_sensor_data error !";
  }
}


#if 0
int main(int argc,char ** argv) {
  WM::my_side= right_SIDE;
  strcpy(WM::my_team_name,"my team");
  SensorBuffer sb;
  cout << "\nbegin:";
  if (0) {
    char buf_1[]= "(see     1)";
    char buf_2[]= "(see 1)";
    char buf[] = "(sense_body 217 (view_mode high normal) (stamina 2500.22 1) (speed 0.7) (head_angle 45) (kick 10) (dash 11) (turn 12) (say 13) (turn_neck 14))";
    char buf2[]= "(see 498 ((goal r) 58.6 0) ((flag c) 6 0 -0 0) ((flag r t) 67.4 -30) ((flag r b) 67.4 30) ((flag p r t) 46.5 -25) ((flag p r c) 42.1 0) ((flag p r b) 46.5 25) ((flag g r t) 59.1 -6) ((flag g r b) 59.1 6) ((flag t r 40) 60.3 -40) ((flag t r 50) 68 -34) ((flag b r 40) 60.3 40) ((flag b r 50) 68 34) ((flag r t 30) 70.1 -25) ((flag r t 20) 66.7 -17) ((flag r t 10) 64.1 -8) ((flag r 0) 63.4 0) ((flag r b 10) 64.1 8) ((flag r b 20) 66.7 17) ((flag r b 30) 70.1 25) ((ball) 10 -2 0 0) ((line r) 58.6 -89))";
    char buf3[]= "(hear 3000 referee before_kick_off)";
    char buf4[]= "(init l 4 before_kick_off)";
    char buf5[]= "(mdpstate 17 (pmode before_kick_off) (vmode high normal) (score 7 8) (ball 6e-41 -0 0 -0) (l_1 -3 37 0 -0 6.28 6.28 3500 1 1) (r_1 1.55e-05 37 0 -0 6.28 6.28 3500 1 1))";
    char new1[]= "(see 0 ((g r) 75.2 15) ((f c t) 24.5 -34 0 0) ((f r t) 73.7 -10) ((f r b) 90 36) ((f p r t) 56.3 0) ((f p r c) 59.7 19) ((f p r b) 68.7 35) ((f g r t) 73.7 10) ((f g r b) 77.5 20) ((f t 0) 27.7 -43 0 0) ((f t r 10) 35.5 -32) ((f t r 20) 44.3 -25) ((f t r 30) 53.5 -20) ((f t r 40) 62.8 -17) ((f t r 50) 72.2 -15) ((f b r 40) 83.9 44) ((f b r 50) 91.8 40) ((f r t 30) 78.3 -7) ((f r t 20) 77.5 0) ((f r t 10) 78.3 7) ((f r 0) 79.8 14) ((f r b 10) 83.1 21) ((f r b 20) 87.4 27) ((f r b 30) 91.8 32) ((l r) 72.2 -89))";

    while (0) {
      double tmp= 1000.0;
      //delay time
      for (long i= 1; i < 1E5; i++) 
	if (i % 10 == 0) tmp= 1000.0;
	else tmp= sqrt(tmp);
      //end delay time
      cout << "x" << flush;
      sb.parse_info(new1);
    }
  }
  if (argc>1) {
    cout << "\nParsing:\n" << argv[1];
    sb.parse_info(argv[1]);
    cout << "\nParser found: " << *(sb.see) << endl;
  }
  else {
    char sb1[] = "(sense_body 0 (view_mode high normal) (stamina 3500 1) (speed 0 0) (head_angle 0) (kick 0) (dash 0) (turn 0) (say 0) (turn_neck 0))";
    char see1[] = "(see 0 ((g r) 66.7 33) ((f r t) 55.7 3) ((f p r t) 42.5 23) ((f p r c) 53.5 43) ((f g r t) 62.8 28) ((f g r b) 70.8 38) ((f t 0) 3.6 -33 0 0) ((f t r 10) 13.2 -8 0 0) ((f t r 20) 23.1 -4) ((f t r 30) 33.1 -3) ((f t r 40) 42.9 -2) ((f t r 50) 53 -2) ((f r t 30) 60.9 6) ((f r t 20) 62.8 15) ((f r t 10) 66 24) ((f r 0) 70.8 31) ((f r b 10) 76.7 37) ((f r b 20) 83.1 43) ((p \"my team\" 1) 6 0 0 0 0 0) ((p \"PYTHONS\" 10 goalie) 33.1 0) ((p \"PYTHONS\") 36.6 0))";
    //char see1[] = "(see 23 ((p \"my_))))(() \nteam\" 1) 6 0 ))";

    cout << "\n--\nparsing:\n" << sb1;
    sb.parse_info(sb1);
    cout << "\nParser found: " << *(sb.sb);

    cout << "\n--\nparsing:\n" << see1 << flush;
    sb.parse_info(see1);
    cout << "\nParser found: " << *(sb.see);

  }
  return 1;
}
#endif 
