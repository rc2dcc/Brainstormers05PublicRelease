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
#include "coach.h"

#define LOG_TMP(WHICH,TIME,LLL,XXX) \
  if (LogOptions::max_level>= 0 && LLL <= LogOptions::max_level) { \
    if (LogOptions::opt_log[WHICH].log_cout) cout << "\n" << TIME << ".0 "  << LogOptions::indent[WHICH][LLL] << " " XXX << flush; \
    if (LogOptions::opt_log[WHICH].log_cerr) cerr << "\n" << TIME << ".0 " << LogOptions::indent[WHICH][LLL] << " "  XXX << flush; \
    if (LogOptions::opt_log[WHICH].log_file) LogOptions::file << "\n" << TIME << ".0 " << LogOptions::indent[WHICH][LLL] << " " XXX; } 

#if 0
#define LOG_DEF(LLL,XXX) LOG_TMP(LogOptions::xx_DEF,fld.getTime(),LLL,XXX)
#define LOG_FLD(LLL,XXX) LOG_TMP(LogOptions::xx_FLD,fld.getTime(),LLL,XXX)
#define LOG_ERR(LLL,XXX) LOG_TMP(LogOptions::xx_ERR,fld.getTime(),LLL,XXX)
#define LOG_VIS(LLL,XXX) LOG_TMP(LogOptions::xx_VIS,fld.getTime(),LLL,XXX)
#define LOG_MSG(LLL,XXX) LOG_TMP(LogOptions::xx_MSG,fld.getTime(),LLL,XXX)
#define LOG_INT(LLL,XXX) LOG_TMP(LogOptions::xx_INT,fld.getTime(),LLL,XXX)
#else
#define LOG_DEF(LLL,XXX) 
#define LOG_FLD(LLL,XXX) 
#define LOG_ERR(LLL,XXX) 
#define LOG_VIS(LLL,XXX) 
#define LOG_MSG(LLL,XXX) 
#define LOG_INT(LLL,XXX) 
#endif

//TOCHANGE_19Okt
//#define LOG_DD(LLL,XXX) //LOG_TMP(LogOptions::xx_DEF,mdpInfo::mdp->time_current,LLL,XXX)


//#define MSG_WITH_NUMBER(XXX)  " [ " << mdpInfo::mdp->me->player_number <<" ] " XXX
//#define MSG_WITH_NUMBER(XXX) " [ " << mdpInfo::mdp->me->number <<" ] " XXX

//#define LOG_ERR(LLL,XXX) LOG_TMP(LogOptions::xx_ERR,mdpInfo::mdp->time_current,LLL,MSG_WITH_NUMBER(XXX))
//#define LOG_DEB(LLL,XXX) LOG_TMP(LogOptions::xx_DEB,mdpInfo::mdp->time_current,LLL,XXX)

//#define LOG_WM_ERR(TIME,LLL,XXX) LOG_TMP(LogOptions::xx_ERR,TIME,LLL,XXX)
//#define LOG_WM(TIME,LLL,XXX) LOG_TMP(LogOptions::xx_WM,TIME,LLL,XXX);

#define _2D "_2D_ "
#define L2D(x1,y1,x2,y2,col)  " l " << (x1) << " " << (y1) << " " << (x2) << " " << (y2) << " " << (col)
#define C2D(x1,y1,r,col)  " c " << (x1) << " " << (y1) << " " << (r) << " " << (col) << ";" 
#define P2D(x1,y1,col)  " p " << (x1) << " " << (y1) << " " << (col) 
#define STRING2D(x1,y1,text,col) " STRING col=" << col << " (" << x1 << "," << y1 << ",\"" << text << "\");"

