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


#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>


#include <iostream>
#include <iomanip>
#include <strstream>
#include <string>
#include "udpsocket.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mdpstate.h"
#include "mdp_info.h"
#include "cmd.h"
#include "options.h"

#include "sensorbuffer.h"
#include "wmstate.h"
#define BS2K

#ifdef BS2K 
#include "bs2k_agent.h"
#include "strategy_factory.h"
#include "policy_factory.h"
#include "move_factory.h"
#include "logger.h"
#include "positioning.h"
#endif

/* Global variables -- don't want to reallocate buffers each time */

DebugOutput *GlobalLogger, *DirectLogger, *InfoLogger;
DebugOutput *ArtLogger;

SensorBuffer SB;

namespace RUN {
sigset_t sigiomask, sigalrmask;
const int ms_timer_cycle= 1000;//10000;   // ms_ mesured in milliseconds
const int MaxStringSize= UDPsocket::MAXMESG;

/* ms_time means time mesured in milliseconds */
const int ms_time_since_last_receive_MAX= 5000;


long ms_time_of_last_timer= -1;
long ms_time_of_last_see= -1;
long ms_time_of_last_sense_body= -1;
long ms_time_of_last_mdpstate= -1;
long ms_time_of_last_receive= -1;

long get_current_ms_time() { //returns time in ms since first call to this routine
    timeval tval;
    static long s_time_at_start= 0;
    if (gettimeofday(&tval,NULL))
        cerr << "\n something wrong with time mesurement";

    if ( 0 == s_time_at_start )
        s_time_at_start= tval.tv_sec;

    return (tval.tv_sec - s_time_at_start) * 1000 + tval.tv_usec / 1000;
}

bool server_alive= false;
UDPsocket sock;

#ifdef BS2K
Bs2kAgent *controller;
#endif
};
/*  */


bool interpret_servers_mdpstate(const char* str, MDPstate & mdp) {
    if ( strncmp("(mdpstate ",str,10)== 0 ) {
        char* next;
        int KONV= 1; //KONV bewirkt Transformation von Koordinaten, falls der Spieler auf der rechten Seite spielt

        /* vorlaeufig > */
        mdp.StateInfoString[0] = '\0'; // wird irgendwann verschwinden, da StateInfoString nur ein Hack von Martin ist
        /* < vorlaeufig */

        if (WM::my_side == right_SIDE)
            KONV= -1;
        str += 10;
        mdp.time_current= strtol(str,&next,10);
        str= next;
        mdp.time_of_last_update= mdp.time_current;

        for (;;) {
            while(str[0]==' ') str++;
            if ( str[0]== '(')
                str ++;
            else
                return false;

            if ( (str[0]== 'l' || str[0]== 'r') && str[1]== '_' ) {
                FPlayer *team;
                int num;
                bool is_myteam= false;
                if ( str[0]== 'l' && WM::my_side== left_SIDE
                     || str[0]== 'r' && WM::my_side== right_SIDE) {
                    is_myteam= true;
                    team= mdp.my_team;
                }
                else
                    team= mdp.his_team;

                str += 2;
                num= strtol(str,&next,10);
                str = next;
                FVal pos_x = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal pos_y = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal vel_x = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal vel_y = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal angle = FVal( strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal neck_angle = FVal( strtod(str,&next),1.0,mdp.time_current );
                str = next;
                if ( WM::my_side == right_SIDE ) { //Anpassung der Winkel fuer den rechten Spieler
                    angle.v+= PI;
                    if (angle.v > 2*PI) angle.v -= 2*PI;
                    neck_angle.v+= PI;
                    if ( neck_angle.v > 2*PI) neck_angle.v -= 2*PI;
                }
                FVal stamina = FVal( strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal effort = FVal( strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal recovery = FVal( strtod(str,&next),1.0,mdp.time_current );
                str = next;
                team[num-1].init(num,pos_x,pos_y,vel_x,vel_y,angle,stamina,effort,recovery,neck_angle);
                if (is_myteam && num== WM::my_number)
                    mdp.me= &team[num-1];
                if (str[0]== ')')
                    str++;
                else
                    return false;
            }
            else if ( strncmp("ball ",str,5) == 0 ) {
                str += 5;
                FVal pos_x = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal pos_y = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal vel_x = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                FVal vel_y = FVal( KONV * strtod(str,&next),1.0,mdp.time_current );
                str = next;
                mdp.ball.init(pos_x,pos_y,vel_x,vel_y);
                if (str[0]== ')')
                    str++;
                else
                    return false;
            }
            else if ( strncmp("score ",str,6) == 0 ) {
                str += 6;
                mdp.my_team_score= strtol(str,&next,10);
                str = next;
                mdp.his_team_score= strtol(str,&next,10);
                str = next;
                if (WM::my_side == right_SIDE) {
                    int tmp = mdp.my_team_score;
                    mdp.my_team_score=  mdp.his_team_score;
                    mdp.his_team_score= tmp;
                }

                if (str[0]== ')')
                    str++;
                else
                    return false;
            }
            else if ( strncmp("vmode ",str,6) == 0 ) {
                str += 6;
                if (strncmp("high normal",str,strlen("high normal")) == 0 ) {
                    mdp.view_quality= HIGH;
                    mdp.view_angle= NORMAL;
                }
                else if (strncmp("high wide",str,strlen("high wide")) == 0 ) {
                    mdp.view_quality= HIGH;
                    mdp.view_angle= WIDE;
                }
                else if (strncmp("high narrow",str,strlen("high narrow")) == 0 ) {
                    mdp.view_quality= HIGH;
                    mdp.view_angle= NARROW;
                }
                else if (strncmp("low normal",str,strlen("low normal")) == 0 ) {
                    mdp.view_quality= LOW;
                    mdp.view_angle= NORMAL;
                }
                else if (strncmp("low wide",str,strlen("low wide")) == 0 ) {
                    mdp.view_quality= LOW;
                    mdp.view_angle= WIDE;
                }
                else if (strncmp("low narrow",str,strlen("low narrow")) == 0 ) {
                    mdp.view_quality= LOW;
                    mdp.view_angle= NARROW;
                }
                else
                    return false;

                while ( str[0] && str[0] != ')' ) {
                    str++;
                }
                if (str[0]== ')')
                    str++;
                else
                    return false;
            }
            else if ( strncmp("pmode ",str,6) == 0 ) {
                str += 6;
                if ( WM::my_side == left_SIDE ) {
                    if ( strncmp("before_kick_off",str,strlen("before_kick_off")) == 0) mdp.play_mode= mdp.PM_my_BeforeKickOff;
                    else if ( strncmp("time_over",str,strlen("time_over")) == 0)        mdp.play_mode= mdp.PM_TimeOver;
                    else if ( strncmp("play_on",str,strlen("play_on")) == 0)            mdp.play_mode= mdp.PM_PlayOn;
                    else if ( strncmp("kick_off_l",str,strlen("kick_off_l")) == 0)      mdp.play_mode= mdp.PM_my_KickOff;
                    else if ( strncmp("kick_off_r",str,strlen("kick_off_r")) == 0)      mdp.play_mode= mdp.PM_his_KickOff;
                    else if ( strncmp("kick_in_l",str,strlen("kick_in_l")) == 0)        mdp.play_mode= mdp.PM_my_KickIn;
                    else if ( strncmp("kick_in_r",str,strlen("kick_in_r")) == 0)        mdp.play_mode= mdp.PM_his_KickIn;
                    else if ( strncmp("free_kick_l",str,strlen("free_kick_l")) == 0)    mdp.play_mode= mdp.PM_my_FreeKick;
                    else if ( strncmp("free_kick_r",str,strlen("free_kick_r")) == 0)    mdp.play_mode= mdp.PM_his_FreeKick;
                    else if ( strncmp("corner_kick_l",str,strlen("corner_kick_l")) == 0)mdp.play_mode= mdp.PM_my_CornerKick;
                    else if ( strncmp("corner_kick_r",str,strlen("corner_kick_r")) == 0)mdp.play_mode= mdp.PM_his_CornerKick;
                    else if ( strncmp("goal_kick_l",str,strlen("goal_kick_l")) == 0)    mdp.play_mode= mdp.PM_my_GoalKick;
                    else if ( strncmp("goal_kick_r",str,strlen("goal_kick_r")) == 0)    mdp.play_mode= mdp.PM_his_GoalKick;
                    else if ( strncmp("goal_l",str,strlen("goal_l")) == 0)              mdp.play_mode= mdp.PM_my_AfterGoal;
                    else if ( strncmp("goal_r",str,strlen("goal_r")) == 0)              mdp.play_mode= mdp.PM_his_AfterGoal;
                    else if ( strncmp("drop_ball",str,strlen("drop_ball")) == 0)        mdp.play_mode= mdp.PM_Drop_Ball;
                    else if ( strncmp("offside_l",str,strlen("offside_l")) == 0)        mdp.play_mode= mdp.PM_his_OffSideKick;
                    else if ( strncmp("offside_r",str,strlen("offside_r")) == 0)        mdp.play_mode= mdp.PM_my_OffSideKick;

                    else if ( strncmp("goalie_catch_ball_l",str,strlen("goalie_catch_ball_l")) == 0)    mdp.play_mode= mdp.PM_my_GoalieFreeKick;
                    else if ( strncmp("goalie_catch_ball_r",str,strlen("goalie_catch_ball_r")) == 0)    mdp.play_mode= mdp.PM_his_GoalieFreeKick;

                    else mdp.play_mode= mdp.PM_Unknown;
                }
                else {
                    if ( strncmp("before_kick_off",str,strlen("before_kick_off")) == 0) mdp.play_mode= mdp.PM_his_BeforeKickOff;
                    else if ( strncmp("time_over",str,strlen("time_over")) == 0)        mdp.play_mode= mdp.PM_TimeOver;
                    else if ( strncmp("play_on",str,strlen("play_on")) == 0)            mdp.play_mode= mdp.PM_PlayOn;
                    else if ( strncmp("kick_off_l",str,strlen("kick_off_l")) == 0)      mdp.play_mode= mdp.PM_his_KickOff;
                    else if ( strncmp("kick_off_r",str,strlen("kick_off_r")) == 0)      mdp.play_mode= mdp.PM_my_KickOff;
                    else if ( strncmp("kick_in_l",str,strlen("kick_in_l")) == 0)        mdp.play_mode= mdp.PM_his_KickIn;
                    else if ( strncmp("kick_in_r",str,strlen("kick_in_r")) == 0)        mdp.play_mode= mdp.PM_my_KickIn;
                    else if ( strncmp("free_kick_l",str,strlen("free_kick_l")) == 0)    mdp.play_mode= mdp.PM_his_FreeKick;
                    else if ( strncmp("free_kick_r",str,strlen("free_kick_r")) == 0)    mdp.play_mode= mdp.PM_my_FreeKick;
                    else if ( strncmp("corner_kick_l",str,strlen("corner_kick_l")) == 0)mdp.play_mode= mdp.PM_his_CornerKick;
                    else if ( strncmp("corner_kick_r",str,strlen("corner_kick_r")) == 0)mdp.play_mode= mdp.PM_my_CornerKick;
                    else if ( strncmp("goal_kick_l",str,strlen("goal_kick_l")) == 0)    mdp.play_mode= mdp.PM_his_GoalKick;
                    else if ( strncmp("goal_kick_r",str,strlen("goal_kick_r")) == 0)    mdp.play_mode= mdp.PM_my_GoalKick;
                    else if ( strncmp("goal_l",str,strlen("goal_l")) == 0)              mdp.play_mode= mdp.PM_his_AfterGoal;
                    else if ( strncmp("goal_r",str,strlen("goal_r")) == 0)              mdp.play_mode= mdp.PM_my_AfterGoal;
                    else if ( strncmp("drop_ball",str,strlen("drop_ball")) == 0)        mdp.play_mode= mdp.PM_Drop_Ball;
                    else if ( strncmp("offside_l",str,strlen("offside_l")) == 0)        mdp.play_mode= mdp.PM_my_OffSideKick;
                    else if ( strncmp("offside_r",str,strlen("offside_r")) == 0)        mdp.play_mode= mdp.PM_his_OffSideKick;

                    else if ( strncmp("goalie_catch_ball_l",str,strlen("goalie_catch_ball_l")) == 0)    mdp.play_mode= mdp.PM_his_GoalieFreeKick;
                    else if ( strncmp("goalie_catch_ball_r",str,strlen("goalie_catch_ball_r")) == 0)    mdp.play_mode= mdp.PM_my_GoalieFreeKick;

                    else mdp.play_mode= mdp.PM_Unknown;
                }
                while ( str[0] && str[0] != ')' ) {
                    str++;
                }
                if (str[0]== ')')
                    str++;
                else
                    return false;
            }

            if (str[0] == ')') {
                //cout << "\n mdp \n" << mdp;
                return true;
            }
        }
    }
    return true;
}

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* Send initialize message */
void send_initialize_message(UDPsocket & sock, const char* teamname,bool goalie= false) {
    string tmp_str= "(init ";
    tmp_str += teamname;
    tmp_str += " (version 6.00)";
    if (goalie)
        tmp_str += "(goalie)";
    tmp_str += ")";

    const char *tmp= tmp_str.data();

    sock.send_msg(tmp,strlen(tmp));
}


/****************************************************************************************/

/* Parse initialize message */
void recv_initialize_message(UDPsocket & sock) {
    char buffer[RUN::MaxStringSize];
    int num_bytes;
    if ( sock.recv_msg(buffer,num_bytes,true) ){    //true= does redirect to new port address (differs from 6000)
        cout << "\nFrom  " << " received " << num_bytes << " Bytes:\n";
        buffer[num_bytes]= '\000';
        cout << buffer;
    }

    SB.parse_info(buffer);
    strncpy(WM::my_team_name,ClientOptions::teamname,20);
    WM::my_side= SB.init->side;
    WM::my_number= SB.init->number;

    RUN::server_alive = true;

    cout << *(SB.init);
#if 0
    char mode[100];
    if ( !(strncmp(buffer,"(init",4)) ) {
        /* It's an init msg */
        char s;
        sscanf(buffer,"(init %c %d %[^)]",&s, &RUN::player_no, mode);
        if ('l'== s)
            WM::my_side= left_SIDE;
        else
            WM::my_side= right_SIDE;
        RUN::server_alive = true;
    }
#endif
}

Value x_LP_2_SRV(Value x) {
    return x;
}

Value y_LP_2_SRV(Value y) {
    return -y;
}

Angle ang_LP_2_SRV_deg(Angle a) {
    if (a<PI) return - a*180.0/PI;
    return -(a-2*PI)*180.0/PI;
}

void send_cmd(UDPsocket & sock, const Cmd & cmd) {
    char buf_main[RUN::MaxStringSize] ;
    char buf_neck[RUN::MaxStringSize] ;
    char buf_view[RUN::MaxStringSize] ;
    char buf_say[RUN::MaxStringSize] ;
    ostrstream o_main(buf_main,RUN::MaxStringSize);
    ostrstream o_neck(buf_neck,RUN::MaxStringSize);
    ostrstream o_view(buf_view,RUN::MaxStringSize);
    ostrstream o_say(buf_say,RUN::MaxStringSize);

    Value par1,par2;
    Angle ang;

    /***************************************************************************/
    //interpret main command, i.e. one of {moveto,turn,dash,kick,catch}
    if ( cmd.cmd_main.is_cmd_set() ) {
        const Cmd_Main& action= cmd.cmd_main; //shortcut
        switch ( action.get_type()) {
        case action.TYPE_MOVETO   :
            action.get_moveto(par1,par2);
            o_main << "(move " << x_LP_2_SRV( par1 ) << " " << y_LP_2_SRV( par2 ) << ")";
            break;
        case action.TYPE_TURN 	 :
            action.get_turn(ang);
            o_main << "(turn " << ang_LP_2_SRV_deg( ang ) << ")";
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
            o_main << "(catch " << ang_LP_2_SRV_deg( ang ) <<")";
            break;
        default:
            cerr << "\nwrong command";
        }
    }
    else { //es wurde kein Kommando gesetzt, evtl. warnen!!!
        o_main << "(turn 0)"; //als debug info verwendbar
        //cerr << "\nno command was specified, sending (turn 0)";
    }
    /***************************************************************************/
    //interpret neck command
    if ( cmd.cmd_neck.is_cmd_set()) {
        cmd.cmd_neck.get_turn( ang );
        o_neck << "(turn_neck " << ang_LP_2_SRV_deg( ang ) << ")";
    }
    /***************************************************************************/
    //interpret view command
    if (cmd.cmd_view.is_cmd_set()) {
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
        o_view << ")";
    }
    
    /***************************************************************************/
    //interpret say command
    if ( cmd.cmd_say.is_cmd_set() ) {
        o_say << "(say " << cmd.cmd_say.get_message() << ")";
        //o_say << "(say 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789)";
    }
    /***************************************************************************/
    // send  the data
    //sock.send_msg("(turn 90)", strlen("(turn 90)"+1));
    //return;

    if (o_main.pcount() > 0) {
        sock.send_msg( o_main.str(), o_main.pcount() );
    }
    if (o_neck.pcount() > 0) {
        sock.send_msg( o_neck.str(), o_neck.pcount());
    }
    if (o_view.pcount() > 0) {
        sock.send_msg( o_view.str(), o_view.pcount());
    }
    if (o_say.pcount() > 0) {
        sock.send_msg( o_say.str(), o_say.pcount());
    }
}



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* set time interval between the sensor receiving and command sending */ 
inline void set_timer() {
    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = RUN::ms_timer_cycle * 1000;
    setitimer(ITIMER_REAL, &itv, NULL);
}

inline void set_timer(int usec) {
    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = RUN::ms_timer_cycle * 1000;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = usec;
    setitimer(ITIMER_REAL, &itv, NULL);
}


/****************************************************************************************/

/* suspend the process until one of the signals comes through */
/* could check for situation to kill client, return false     */
/* i.e. too many actions with no sensory input coming in      */
bool wait_for_signals(sigset_t *mask){
    sigsuspend(mask);
    return true;
}


/****************************************************************************************/


WMstate wm;      
Cmd last_cmd;

/* SIGIO handler: receive and parse messages from server */
void sigio_handler() {
    //sigprocmask(SIG_BLOCK, &RUN::sigalrmask, NULL);

    char buffer[RUN::MaxStringSize];
    int num_bytes;

    while ( RUN::sock.recv_msg( buffer,num_bytes) ) {
        buffer[num_bytes] = '\000';
        long ms_time= RUN::get_current_ms_time();
        RUN::ms_time_of_last_receive= ms_time;
        RUN::ms_time_of_last_timer= ms_time;

        if ( strncmp("(see ",buffer,5)== 0 ) {
#if 0
            char *str = buffer + 5;
            long time= strtol(str,0,10);
            cout << "\n" << time << " see ";
            if (ms_time > RUN::ms_time_of_last_see)
                cout << ms_time << " " << ms_time - RUN::ms_time_of_last_see;
#endif       
            RUN::ms_time_of_last_see= ms_time;
            //cout << buffer;
            SB.parse_info(buffer);
            //LOG_ART(SB.see->time) << " see " << SB.see->time << " ms_time= " << ms_time << END_LOG;
            //ArtLogger->FileStream << SB.see->time << ".0 -- " << buffer;

            wm.incorporate_msg_see(*(SB.see));
        }

        if ( strncmp("(sense_body ",buffer,5)== 0 ) {
#if 0
            cout << "\n>>> sb " << ms_time << " " << ms_time - RUN::ms_time_of_last_sense_body
                 << *(SB.sb);
#endif       
            RUN::ms_time_of_last_sense_body= ms_time;
            //cout << buffer;
            SB.parse_info(buffer);
            LOG_ART(SB.sb->time, << " sense_body" );
            //LOG_ART(SB.sb->time, << " sense_body " );
            wm.incorporate_cmd_and_msg_sense_body(last_cmd,*(SB.sb));
        }
#if 1
        if ( strncmp("(mdpstate ",buffer,10)== 0 ) {
            //cerr << "\n>>" << buffer << "<<";
            SB.parse_info(buffer);

            RUN::ms_time_of_last_mdpstate= ms_time;
            MDPstate mdp;
            //WMstate wm;

            if ( 0 || SB.mdpstate->time % 10 == 0) {
                //wm.compare_msg_mdpstate( *(SB.mdpstate) );
                wm.import_msg_mdpstate( *(SB.mdpstate) );
            }
            else {
                //wm.ball.pos= Vector( SB.mdpstate->ball.x, SB.mdpstate->ball.y );
                //wm.ball.vel= Vector( SB.mdpstate->ball.vel_x, SB.mdpstate->ball.vel_y );
                wm.compare_msg_mdpstate( *(SB.mdpstate) );
            }
            wm.export_mdpstate( mdp );
            //interpret_servers_mdpstate(buffer,mdp);
            Cmd cmd;

            mdpInfo::update(mdp); // update the object mdpInfo with the current mdp_state
            mdpInfo::server_state= &mdp; //nur fuer debug Zwecke, falls das WM nicht vollstaendig ist (im artagent ist dies identisch mit dem gelieferten WM)
            //cout << "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" << buffer << mdp;
#ifdef BS2K
            //cout << mdp;
            cmd= RUN::controller->behave(mdp); // compute command and execute it
            last_cmd= cmd;

            ms_time= RUN::get_current_ms_time();
            if (RUN::ms_time_of_last_mdpstate- ms_time > 40)
                cout << "\nPlayer " << WM::my_number << ": ms_time in behave = " << RUN::ms_time_of_last_mdpstate- ms_time;
            //cout << cmd;
            send_cmd(RUN::sock,cmd);
#endif
        }
#endif
    }

    //sigprocmask(SIG_UNBLOCK, &RUN::sigalrmask, NULL);
}



/****************************************************************************************/

/* SIGALRM handler: extract and send first command in commandlist */
void sigalrm_handler() {
    //sigprocmask(SIG_BLOCK, &RUN::sigiomask, NULL);

    if (RUN::ms_time_of_last_timer < 0)
        RUN::ms_time_of_last_timer= RUN::get_current_ms_time();
    else
        RUN::ms_time_of_last_timer+= RUN::ms_timer_cycle; //just to save one system call
    //long ms_time_of_last_timer= RUN::get_current_ms_time();

    if (RUN::ms_time_of_last_receive < 0) { //no receives mesured yet
        RUN::ms_time_of_last_receive= RUN::ms_time_of_last_timer; // prevents from waiting for a 1st receive forever
        return;
    }
    long ms_time_since_last_receive  = RUN::ms_time_of_last_timer - RUN::ms_time_of_last_receive;
    if (  ms_time_since_last_receive >= RUN::ms_time_since_last_receive_MAX) {
        cout << "\nServer " //<< udpName(RUN::server)
             << " is not responding since more then "
             << double(RUN::ms_time_since_last_receive_MAX)/1000.0
             << " (" << setprecision(4) << double(ms_time_since_last_receive)/1000.0
             << ") seconds";
        RUN::server_alive= false;
    }

    //sigprocmask(SIG_UNBLOCK, &RUN::sigiomask, NULL);
}


/****************************************************************************************/

sigset_t init_handler() { 
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


/****************************************************************************************/

void init_loggers() {
    char side_chr= 'l';
    if (left_SIDE != WM::my_side)
        side_chr= 'r';

    char logfilename[200];
    char infologfilename[200];
    sprintf(logfilename,"%s/%s%d-%c-actions.log",CommandLineOptions::log_dir,
            ClientOptions::teamname,
            ClientOptions::player_no,side_chr);
    sprintf(infologfilename,"%s/%s%d-%c-info.log",CommandLineOptions::log_dir,
            ClientOptions::teamname,
            ClientOptions::player_no,side_chr);

    InfoLogger = new DebugOutput(infologfilename);
    InfoLogger->setOutputToStdOut(true );

    ArtLogger = new DebugOutput();
    ArtLogger->setOutputToStdOut(false);
    ArtLogger->setOutputToStdErr(false);
    ArtLogger->setOutputToLogFile( false );

    if (CommandLineOptions::do_logging == 1){
        GlobalLogger = new DebugOutput(logfilename);
        GlobalLogger->setOutputToStdOut(false );
        DirectLogger = new DebugOutput();
        DirectLogger->setOutputToStdOut(false);
    }
    else if (CommandLineOptions::do_logging == 2){
        GlobalLogger = new DebugOutput();
        GlobalLogger->setOutputToStdOut(false);
        GlobalLogger->setOutputToStdErr(true);
        DirectLogger = new DebugOutput();
        DirectLogger->setOutputToStdOut(true);
    }
    else if (CommandLineOptions::do_logging == 3){
        DirectLogger = new DebugOutput(logfilename);
        DirectLogger->setOutputToStdOut(false );
        GlobalLogger = new DebugOutput();
        GlobalLogger->setOutputToStdOut(false);
    }
    else{ /* init object, but do not log */
        ///ART
        if (CommandLineOptions::do_logging==4) {
            ArtLogger->setLogFileName( logfilename );
            ArtLogger->setOutputToLogFile( true );
        }
        ///
        GlobalLogger = new DebugOutput();
        //GlobalLogger->setOutputToStdOut(true);
        GlobalLogger->setOutputToStdOut(false);
        DirectLogger = new DebugOutput();
        DirectLogger->setOutputToStdOut(false);
    }
}

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

int main_loop_via_select() {
    fd_set rfds;
    struct timeval tv;
    int retval;
    int max_fd_plus_1=  RUN::sock.socket_fd + 1;

    while (RUN::server_alive) {
        FD_ZERO(&rfds);
        //FD_SET(0, &rfds); //Standardeingabe
        FD_SET(RUN::sock.socket_fd, &rfds);
        /* Wait up to five seconds. */
        tv.tv_sec = 5; tv.tv_usec = 0;

        retval = select(max_fd_plus_1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

        if (retval>0) {
            if ( FD_ISSET(0,&rfds) ) {
                //cout << "\nStart Standardeingabe " << flush;
                char buffer[1000];
                cin.getline(buffer,1000);

                if ( strcmp(buffer,"quit") == 0 ) {
                }
            }
            if ( FD_ISSET(RUN::sock.socket_fd,&rfds) ) {
                char buffer[RUN::MaxStringSize];
                int num_bytes;
                bool mdp_step= false;
                while ( RUN::sock.recv_msg( buffer,num_bytes) ) {
                    buffer[num_bytes] = '\000';
                    if ( strncmp("(see ",buffer,5)== 0 ) continue;
                    if ( strncmp("(sense_body ",buffer,5)== 0 ) {
                        SB.parse_info(buffer);
                        LOG_ART(SB.sb->time, << " sense_body" );
                        wm.incorporate_cmd_and_msg_sense_body(last_cmd,*(SB.sb));
                        continue;
                    }
                    if ( strncmp("(mdpstate ",buffer,10)== 0 ) {
                        mdp_step= true;
                        SB.parse_info(buffer);
                        cout << "\nmdpstate time =" << buffer[9] << buffer[10] << buffer[11] << buffer[12] << buffer[13];
                    }
                }
                if (mdp_step) {
                    MDPstate mdp;
                    Cmd cmd;

                    wm.import_msg_mdpstate( *(SB.mdpstate) );
                    wm.export_mdpstate( mdp );

                    mdpInfo::update(mdp); // update the object mdpInfo with the current mdp_state
                    mdpInfo::server_state= &mdp; //nur fuer debug Zwecke, falls das WM nicht vollstaendig ist (im artagent ist dies identisch mit dem gelieferten WM)

                    float angle;
                    cout << "\nplease enter angle (in degree)";
                    cin >> angle;
                    if (angle >=  0.0) {
                        angle = angle * PI/180.0;
                        Value res= Move_1Step_Kick::get_max_vel_in_dir(angle);
                        Move_1Step_Kick kick1(res,angle);
                        kick1.execute(cmd);
                        cout << "\n res_factor = " << res << flush;
                        //cmd.cmd_main.set_turn(0.0);
                        send_cmd(RUN::sock,cmd);
                    }

                }

            }
        }
        else if ( retval == 0)
            ;//cout << "No Data is available now." << endl;
        else
            cout << "\nselect: error occured";
    }
    return 0;
}


int main(int argc, char *argv[]) {
    CommandLineOptions::init_options();
    CommandLineOptions::read_from_command_line(argc,argv);
    CommandLineOptions::print_options();


    // init default values for agent
    ClientOptions::init();
    // read agent's default parameter file
    ClientOptions::read_from_file( CommandLineOptions::agent_conf );
    ClientOptions::read_from_command_line(argc,argv);
    { //precede teamname with a '_' to get mdpstate messages
        char tmp[20];
        strncpy(tmp,ClientOptions::teamname,20);
        snprintf(ClientOptions::teamname,20,"_%s",tmp);
    }

    // init default values for server
    ServerOptions::init();
    // read server configuration from file
    ServerOptions::read_from_file( CommandLineOptions::server_conf);

    bool res= true;
    res= res && RUN::sock.init_socket_fd();
    if (CommandLineOptions::host != "localhost") {
        cout << "\nlocal host init_serv_addr = " << RUN::sock.init_serv_addr(CommandLineOptions::host,6000);
    }
    else {
        cout << "\ninit_serv_addr = " << RUN::sock.init_serv_addr(CommandLineOptions::host,6000);
    }

    send_initialize_message(RUN::sock,ClientOptions::teamname,ClientOptions::consider_goalie);
    recv_initialize_message(RUN::sock);

    ClientOptions::player_no = WM::my_number;

#ifdef BS2K
    init_loggers();

    cout << "\n --------------- BS2K -----------------" << flush;
    cout << "\n--\nInitializing Move_Factory";
    Move_Factory::init();
    cout << "\n--\nInitializing Policy_Factory";
    Policy_Factory::init();
    cout << "\n--\nInitializing Strategy_Factory";
    Strategy_Factory::init();
    cout << "\n--\nInitializing Policy_Tools";
    Policy_Tools::init_params();
    cout << "\n--\nInitializing Planning";
    Planning::init_params();
    cout << "\n--\nInitializing Magnetic_Tools";
    Magnetic_Tools::init_params();
    cout << "\n--\nInitializing Formations";
    DeltaPositioning::init_formations();
    cout << "\n--\n" << flush;

    RUN::controller = new Bs2kAgent(); // important: init Agent after reading client options
#endif 

    RUN::sock.set_fd_nonblock();

#if 1
    RUN::sock.set_fd_sigio();  //  bind sigio to file descriptor

    sigset_t sigfullmask = init_handler();
    while ( RUN::server_alive && wait_for_signals(&sigfullmask) );
    RUN::sock.send_msg("(bye)",6);
#else
    main_loop_via_select();
#endif
    cout << "\nShutting down player " << WM::my_number << endl;

}



