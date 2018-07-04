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

/** @author  Martin Riedmiller
    @date    16.2.2000
    @version 1.0
    @short   Logger, derived from DebugOutput
*/

#ifndef LOGGER_H
#define LOGGER_H


#include <DebugOut.h>
#include <mdp_info.h>

#if 0
extern DebugOutput *GlobalLogger,*DirectLogger,*InfoLogger;
#define LOG_GENERAL ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "----" << " G ")
#define LOG_STRATEGY ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "----" << " S ")
#define LOG_POLICY ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "----" << " P ")
#define LOG_MOVE ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "----" << " M ")
#define LOG_SENSE_BODY ((*GlobalLogger)<< "\n" )
#define LOG_STATE ((*GlobalLogger)<< "\n" )
#define LOG_GENERAL_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------"<< " G ")
//#define LOG_WM_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------"<< " WM ")
#define LOG_WM_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------"<< " WM ")
#define LOG_WM ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "--"<< " WM ")
//#define LOG_WMDEB ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "--"<< " WM ")
#define LOG_STRATEGY_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------"<< " S ")
#define LOG_POLICY_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------" << " P ")
#define LOG_MOVE_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------" << " M ")
#define LOG_ERROR ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "-" << " E ")
#define LOG_SYSTEM_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "------"<< " C ")
#define LOG_SYSTEM ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "----"<< " C ")
#define LOG_DRAWLINE ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "--"<< "DrawLine ")
#define LOG_DRAWCIRCLE ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "--"<< "DrawCircle ")
#define LOG_DRAWCIRCLE_D ((*GlobalLogger)<< "\n" << mdpInfo::mdp->time_current <<".0 "<< "-------"<< "DrawCircle ")

//#define LOG_INFO(XXX) (cout<< mdpInfo::mdp->time_current <<" ("<< mdpInfo::mdp->me->number<<") " XXX)
#define LOG_INFO(XXX) ((*InfoLogger)<< mdpInfo::mdp->time_current <<" ("<< mdpInfo::mdp->me->number<<") "<< XXX<<"\n")
#define LOG_INFO_T(XXX) ((*InfoLogger) XXX<<"\n")

#endif

#if 0//1
#define LOG_DIRECT(XXX) ((*DirectLogger)<< mdpInfo::mdp->time_current <<".0 "<< "-"<< mdpInfo::mdp->me->number<<" " XXX << "\n")
#define LOG_DIRECT_T(XXX) ((*DirectLogger)  XXX << "\n")
#define LOG_DIRECT_2D(XXX) ((*DirectLogger)<< mdpInfo::mdp->time_current <<".0 "<< "-_2D_"  XXX << "\n")

#define L2D(x1,y1,x2,y2,col)  " l " << (x1) << " " << (y1) << " " << (x2) << " " << (y2) << " " << (col)
#define C2D(x1,y1,r,col)  " c " << (x1) << " " << (y1) << " " << (r) << " " << (col) 
#define P2D(x1,y1,col)  " p " << (x1) << " " << (y1) << " " << (col) 

#else // ignore output
#define LOG_DIRECT(XXX) 
#define LOG_DIRECT_T(XXX)
#define LOG_DIRECT_2D(XXX)

#endif 

#endif
