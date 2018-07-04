%{

  //#include "yy.tab.h"

#define siwrap() 1 /* we use the si-prefix instead of the standard yy. */
#undef YY_MAIN
%}
%option prefix="si"
/* we need a case-insensitive lexer, because the server gives sensor information in upper case if an object is very close to the player (i.e. he senses it) */
%option case-insensitive

/* Following are the state definitions for the lexer.
*/
%x HEAR_MODE 
%s HEAR_REFEREE
%x SENSE_MODE
%x SENSE_INFO
%x ERROR_MODE
%x SEE_MODE
%x SEE_OBJECT_MODE
%x SEE_OBJECT_ATTR
%x SEE_PLAYER
%x SEE_TEAMNAME
%x SEE_OBJECT_PARAMETER
%x MDP_MODE
%x MDP_OBJECT_MODE
%x MDP_PLAYER
%x MDP_OBJECT_PARAMETER
%s MDP_REFEREE
%x MDP_SCORE
%x SB_READ_DOUBLE
%x INIT_MODE

DIGIT [0-9]
STRING [^ \t\n\(\)]+
/* spaces are allowed in team names, but no quote marks*/
TEAMSTRING [^\"]+ 
INT    [+\-]?{DIGIT}+
/* an INT ist also an DOUBLE, but not the other way */
DOUBLE [+\-]?{DIGIT}+(\.?{DIGIT}*(e[+\-]?{DIGIT}+)?)?  
OPENING_BRACKET \(
CLOSING_BRACKET \)
QUOTE_MARK \"
%%

[\t ]+ /* ignore tabs and spaces*/ ;


mdpstate { BEGIN MDP_MODE; return MDP;}

<MDP_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_MODE>{INT} {silval.ival = atoi(sitext); return INT;}
<MDP_MODE>{OPENING_BRACKET} {BEGIN MDP_OBJECT_MODE; return OB;} 
<MDP_MODE>{CLOSING_BRACKET} {BEGIN 0;return CB;}

<MDP_OBJECT_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_OBJECT_MODE>vmode { return VIEW_MODE;}
<MDP_OBJECT_MODE>high { silval.ival= HIGH; return VIEW_QUALITY;}
<MDP_OBJECT_MODE>low { silval.ival= LOW; return VIEW_QUALITY; }
<MDP_OBJECT_MODE>narrow { silval.ival= NARROW; return VIEW_WIDTH;}
<MDP_OBJECT_MODE>normal { silval.ival= NORMAL; return VIEW_WIDTH;}
<MDP_OBJECT_MODE>wide { silval.ival= WIDE; return VIEW_WIDTH;}
<MDP_OBJECT_MODE>pmode {BEGIN MDP_REFEREE;}
<MDP_OBJECT_MODE>score {BEGIN MDP_SCORE; return SI_SCORE; }
<MDP_OBJECT_MODE>l_ {BEGIN MDP_PLAYER; if (WM::my_side == left_SIDE) silval.ival= my_TEAM; else silval.ival= his_TEAM; return SIDE; } 
<MDP_OBJECT_MODE>r_ {BEGIN MDP_PLAYER; if (WM::my_side == right_SIDE) silval.ival= my_TEAM; else silval.ival= his_TEAM; return SIDE; } 
<MDP_OBJECT_MODE>ball { BEGIN MDP_OBJECT_PARAMETER; return BALL;}
<MDP_OBJECT_MODE>{CLOSING_BRACKET} {BEGIN MDP_MODE; return CB;}

<MDP_REFEREE>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_REFEREE>{CLOSING_BRACKET} {BEGIN MDP_MODE; return CB;}

<MDP_SCORE>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_SCORE>{INT} {silval.ival = atoi(sitext); return INT;}
<MDP_SCORE>{CLOSING_BRACKET} {BEGIN MDP_MODE; return CB;}

<MDP_PLAYER>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_PLAYER>{INT} {BEGIN MDP_OBJECT_PARAMETER; silval.ival = atoi(sitext); return UNUM;}

<MDP_OBJECT_PARAMETER>[\t ]+ /* ignore tabs and spaces*/ ;
<MDP_OBJECT_PARAMETER>{INT}    {silval.dval = double(atoi(sitext)); return DOUBLE;}
<MDP_OBJECT_PARAMETER>{DOUBLE} {silval.dval = atof(sitext); return DOUBLE;}
<MDP_OBJECT_PARAMETER>{CLOSING_BRACKET} {BEGIN MDP_MODE; return CB;}

see {BEGIN SEE_MODE; return SEE;}

<SEE_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<SEE_MODE>{INT} {silval.ival = atoi(sitext); return INT;}
<SEE_MODE>{OPENING_BRACKET} {BEGIN SEE_OBJECT_MODE; return OB;} 
<SEE_MODE>{CLOSING_BRACKET} {BEGIN 0;return CB;}

<SEE_OBJECT_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<SEE_OBJECT_MODE>p {BEGIN SEE_PLAYER; return PLAYER;} /* player (ver. 6.00) */
<SEE_OBJECT_MODE>g {BEGIN SEE_OBJECT_ATTR; return GOAL;} /* goal (ver. 6.00) */
<SEE_OBJECT_MODE>b {BEGIN SEE_OBJECT_ATTR; return BALL;} /* ball (ver. 6.00) */
<SEE_OBJECT_MODE>f {BEGIN SEE_OBJECT_ATTR; return FLAG;} /* flag (ver. 6.00) */
<SEE_OBJECT_MODE>l {BEGIN SEE_OBJECT_ATTR; return LINE;} /* line (ver. 6.00) */
<SEE_OBJECT_MODE>{OPENING_BRACKET} { return OB;} 
 /*<SEE_OBJECT_MODE>{CLOSING_BRACKET} {BEGIN SEE_OBJECT_PARAMETER; return CB;}*/

<SEE_PLAYER>[\t ]+ /* ignore tabs and spaces*/ ;
<SEE_PLAYER>{INT} {silval.ival = atoi(sitext); return UNUM;}
<SEE_PLAYER>goalie {return GOALIE;}
<SEE_PLAYER>{CLOSING_BRACKET} {BEGIN SEE_OBJECT_PARAMETER; return CB;}
<SEE_PLAYER>{QUOTE_MARK} {BEGIN SEE_TEAMNAME;} 
<SEE_TEAMNAME>{TEAMSTRING} {
 if (strcmp( WM::my_team_name, sitext) == 0 )
   silval.ival= my_TEAM;
 else
   silval.ival= his_TEAM;
 //cout << "\nWM::my_team_name= [" << WM::my_team_name << "] arg3= [" << sitext << "]  (pteam == MY_TEAM)= " << (strcmp( WM::my_team_name, sitext) == 0);
 return WHICHTEAM;
}
<SEE_TEAMNAME>{QUOTE_MARK} {BEGIN SEE_PLAYER;}

<SEE_OBJECT_PARAMETER>[\t ]+ /* ignore tabs and spaces*/ ;
<SEE_OBJECT_PARAMETER>{OPENING_BRACKET} {;return OB;} 
<SEE_OBJECT_PARAMETER>{INT}    {silval.dval = double(atoi(sitext)); return DOUBLE;}
<SEE_OBJECT_PARAMETER>{DOUBLE} {silval.dval = atof(sitext); return DOUBLE;}
<SEE_OBJECT_PARAMETER>{CLOSING_BRACKET} {BEGIN SEE_MODE; return CB;}

<SEE_OBJECT_ATTR>[\t ]+ /* ignore tabs and spaces*/ ;
<SEE_OBJECT_ATTR>l {return _L_;}
<SEE_OBJECT_ATTR>r {return _R_;}
<SEE_OBJECT_ATTR>c {return _C_;}
<SEE_OBJECT_ATTR>t {return _T_;}
<SEE_OBJECT_ATTR>b {return _B_;}
<SEE_OBJECT_ATTR>p {return _P_;}
<SEE_OBJECT_ATTR>g {return _G_;}
<SEE_OBJECT_ATTR>{INT} {silval.ival = atoi(sitext); return INT;}
<SEE_OBJECT_ATTR>{CLOSING_BRACKET} {BEGIN SEE_OBJECT_PARAMETER; return CB;}


hear { BEGIN HEAR_MODE; return HEAR;}

<HEAR_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<HEAR_MODE>self {return SELF;}
<HEAR_MODE>referee {BEGIN HEAR_REFEREE; return REFEREE;}
<HEAR_MODE>{INT} {silval.ival = atoi(sitext); return INT;}
<HEAR_MODE>{STRING} { silval.sval = 0;/*strdup(sitext)*/; return STRING;}
<HEAR_MODE>{CLOSING_BRACKET} {BEGIN 0; return CB;}

<HEAR_REFEREE>[\t ]+ /* ignore tabs and spaces*/ ;
<HEAR_REFEREE>{CLOSING_BRACKET} {BEGIN 0; return CB; }

before_kick_off { silval.ival= PM_before_kick_off; return PMODE;}
time_over { silval.ival= PM_time_over; return PMODE;}
play_on { silval.ival= PM_play_on; return PMODE;}
kick_off_l { silval.ival= PM_kick_off_l; return PMODE;}
kick_off_r { silval.ival= PM_kick_off_r; return PMODE;}
kick_in_l { silval.ival= PM_kick_in_l; return PMODE;}
kick_in_r { silval.ival= PM_kick_in_r; return PMODE;}
free_kick_l { silval.ival= PM_free_kick_l; return PMODE;}
free_kick_r { silval.ival= PM_free_kick_r; return PMODE;}
corner_kick_l { silval.ival= PM_corner_kick_l; return PMODE;}
corner_kick_r { silval.ival= PM_corner_kick_r; return PMODE;}
goal_kick_l { silval.ival= PM_goal_kick_l; return PMODE;}
goal_kick_r { silval.ival= PM_goal_kick_r; return PMODE;}
goal_l { silval.ival= PM_goal_l; return PMODE;}
goal_r { silval.ival= PM_goal_r; return PMODE;}
drop_ball { silval.ival= PM_drop_ball; return PMODE;}
offside_l { silval.ival= PM_offside_l; return PMODE;}
offside_r { silval.ival= PM_offside_r; return PMODE;}
goalie_catch_ball_l { silval.ival= PM_goalie_catch_ball_l; return PMODE;}
goalie_catch_ball_r { silval.ival= PM_goalie_catch_ball_r; return PMODE;}


sense_body {BEGIN SENSE_MODE;  return SENSE_BODY ;}

<SENSE_MODE>[\t ] /* ignore tabs and spaces. */
<SENSE_MODE>{INT} {silval.ival = atoi(sitext); return INT;}
<SENSE_MODE>{OPENING_BRACKET} {BEGIN SENSE_INFO; return OB;}
<SENSE_MODE>{CLOSING_BRACKET} {BEGIN 0; return CB;}

<SENSE_INFO>[\t ] /* ignore tabs and spaces. */
<SENSE_INFO>{CLOSING_BRACKET} {BEGIN SENSE_MODE; return CB;}  
<SENSE_INFO>view_mode {return VIEW_MODE;}
<SENSE_INFO>high { silval.ival= HIGH; return VIEW_QUALITY;}
<SENSE_INFO>low { silval.ival= LOW; return VIEW_QUALITY; }
<SENSE_INFO>narrow { silval.ival= NARROW; return VIEW_WIDTH;}
<SENSE_INFO>normal { silval.ival= NORMAL; return VIEW_WIDTH;}
<SENSE_INFO>wide { silval.ival= WIDE; return VIEW_WIDTH;}
<SENSE_INFO>stamina {BEGIN SB_READ_DOUBLE; return STAMINA;}
<SENSE_INFO>speed {BEGIN SB_READ_DOUBLE; return SPEED;}
<SENSE_INFO>head_angle {BEGIN SB_READ_DOUBLE; return HEAD_ANGLE;}
<SENSE_INFO>kick {return SI_KICK;}
<SENSE_INFO>dash {return SI_DASH;}
<SENSE_INFO>turn {return SI_TURN;}
<SENSE_INFO>say {return SI_SAY;}
<SENSE_INFO>turn_neck {return SI_TURN_NECK;}
<SENSE_INFO>{INT} {silval.ival = atoi(sitext); return INT;}

<SB_READ_DOUBLE>[\t ]+ /* ignore tabs and spaces*/ ;
<SB_READ_DOUBLE>{CLOSING_BRACKET} {BEGIN SENSE_MODE; return CB;}  
<SB_READ_DOUBLE>{DOUBLE} {silval.dval = atof(sitext); return DOUBLE;}

error {BEGIN ERROR_MODE; return SI_ERROR ;}

<ERROR_MODE>[\t ] /* ignore tabs and spaces. */
<ERROR_MODE>unknown_command {return UNKNOWN_COMMAND;}
<ERROR_MODE>illegal_command_form {return ILLEGAL_COMMAND_FORM;}
<ERROR_MODE>{CLOSING_BRACKET} {BEGIN 0; return CB;}

init {BEGIN INIT_MODE; return SI_INIT ;}

<INIT_MODE>[\t ]+ /* ignore tabs and spaces*/ ;
<INIT_MODE>l {silval.ival = left_SIDE; return SIDE;}
<INIT_MODE>r {silval.ival = right_SIDE; return SIDE;}
<INIT_MODE>{INT} {BEGIN HEAR_REFEREE; silval.ival = atoi(sitext); return INT;}

{OPENING_BRACKET} { return OB;} 
{CLOSING_BRACKET} { return CB;}
{INT} {silval.ival = atoi(sitext); return INT;}

.|\n ; 
%%


