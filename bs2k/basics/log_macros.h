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

#include "options.h"
#include "ws_info.h"

#if 0 // activate in competitions only
#define LOG_MDP(LLL,XXX) LOG_TMP(LogOptions::xx_MDP,WSinfo::ws->time,LLL,XXX)
#define LOG_DEC(LLL,XXX) LOG_TMP(LogOptions::xx_DEF,WSinfo::ws->time,LLL,XXX)
#define LOG_MOV(LLL,XXX) LOG_TMP(LogOptions::xx_DEF,WSinfo::ws->time,LLL,XXX)
#define LOG_POL(LLL,XXX) LOG_TMP(LogOptions::xx_DEF,WSinfo::ws->time,LLL,XXX)
#define LOG_STR(LLL,XXX) LOG_TMP(LogOptions::xx_DEF,WSinfo::ws->time,LLL,XXX)
#else
#define LOG_MDP(LLL,XXX)
#define LOG_DEC(LLL,XXX)
#define LOG_MOV(LLL,XXX)
#define LOG_POL(LLL,XXX)
#define LOG_STR(LLL,XXX)
#endif

#define LOG_ERR(LLL,XXX) LOG_TMP(LogOptions::xx_ERR,WSinfo::ws->time,LLL,MSG_WITH_NUMBER(XXX))

#define LOG_TMP(WHICH,TIME,LLL,XXX) \
  if (LogOptions::max_level>= 0 && LLL <= LogOptions::max_level) { \
    if (LogOptions::opt_log[WHICH].log_cout) std::cout << "\n" << TIME << ".0 "  << LogOptions::indent[WHICH][LLL] << " " XXX ; \
    if (LogOptions::opt_log[WHICH].log_cerr) std::cerr << "\n" << TIME << ".0 " << LogOptions::indent[WHICH][LLL] << " "  XXX ; \
    if (LogOptions::opt_log[WHICH].log_file) LogOptions::file << "\n" << TIME << ".0 " << LogOptions::indent[WHICH][LLL] << " " XXX; } 

//don't use new line, just append text (useful in loops; always use at least one LOG_TMP before)!
#define LOG_TMP_APPEND(WHICH,LLL,XXX) \
  if (LogOptions::max_level>= 0 && LLL <= LogOptions::max_level) { \
    if (LogOptions::opt_log[WHICH].log_cout) std::cout << XXX ; \
    if (LogOptions::opt_log[WHICH].log_cerr) std::cerr << XXX ; \
    if (LogOptions::opt_log[WHICH].log_file) LogOptions::file << XXX; } 

//TOCHANGE_19Okt
#define LOG_DD(LLL,XXX) //LOG_TMP(LogOptions::xx_DEF,WSinfo::ws->time,LLL,XXX)


#define MSG_WITH_NUMBER(XXX) " [ " << ClientOptions::player_no <<" ] " XXX

#define LOG_DEB(LLL,XXX) LOG_TMP(LogOptions::xx_DEB,WSinfo::ws->time,LLL,XXX)
//#define LOG_DEB(LLL,XXX) LOG_TMP(LogOptions::xx_DEB,WSinfo::ws->time << ".0 - (" << __FILE__ << __LINE__ << ")",LLL,XXX)

#define LOG_WM_ERR(TIME,LLL,XXX) LOG_TMP(LogOptions::xx_ERR,TIME,LLL,XXX)
#if 0
#define LOG_WM(TIME,LLL,XXX) LOG_TMP(LogOptions::xx_WM,TIME,LLL,XXX);
#else
#define LOG_WM(TIME,LLL,XXX)
#endif
#define LOG_WM_APPEND(LLL,XXX) LOG_TMP_APPEND(LogOptions::xx_WM,LLL,XXX);


#define _2D "_2D_ "
#define L2D(x1,y1,x2,y2,col)  " l " << (x1) << " " << (y1) << " " << (x2) << " " << (y2) << " " << (col) << ";"
//#define L2D(p1,p2,col)  " l " << (p1).x << " " << (p1).y << " " << (p2).x << " " << (p2).y << " " << (col) << ";"
#define C2D(x1,y1,r,col)  " c " << (x1) << " " << (y1) << " " << (r) << " " << (col) << ";"
//#define C2D(p,r,col)  " c " << (p).x << " " << (p).y << " " << (r) << " " << (col) << ";"
#define P2D(x1,y1,col)  " p " << (x1) << " " << (y1) << " " << (col) << ";"
//#define VP2D(p,col)  " p " << (p).x << " " << (p).y << " " << (col) << ";"
#define STRING2D(x1,y1,text,col) " STRING col=" << col << " (" << x1 << "," << y1 << ",\"" << text << "\");"
//#define STRING2D(p,text,col) " STRING col=" << col << " (" << (p).x << "," << (p).y << ",\"" << text << "\");"
