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

#ifndef _WMDEF_H_
#define _WMDEF_H_

#include "globaldef.h"

#if 1
#include "DebugOut.h"
extern DebugOutput *ArtLogger;
#define LOG_ART(T,XXX) ((*ArtLogger)<< "" << (T) <<".0 "<< "-" XXX << "\n")
#define LOG_ART_2D(T,XXX) ((*ArtLogger)<< "" << (T) <<".0 "<< "-_2D_" XXX << "\n" )
#define LOG_ART_2DD(T,SSS,XXX) ((*ArtLogger)<< "" << (T) <<".0 "<< "-" << SSS << "_2D_" XXX << "\n")

#define L2D(x1,y1,x2,y2,col)  " l " << (x1) << " " << (y1) << " " << (x2) << " " << (y2) << " " << (col)
#define C2D(x1,y1,r,col)  " c " << (x1) << " " << (y1) << " " << (r) << " " << (col) 
#define P2D(x1,y1,col)  " p " << (x1) << " " << (y1) << " " << (col) 
#else
#define LOG_ART(T,XXX) 
#define LOG_ART_2D(T,XXX) 
#define LOG_ART_2DD(T,XXX) 
#endif

#define UNDEF_INFORMATION 5326
#define UNDEF_STRING "\0"
#define SELF_DIRECTION 512
/* copy from server: params.h 
   The parser needs it !
*/
#define PITCH_LENGTH 		105.0
#define PITCH_WIDTH 	 	 68.0
#define PITCH_MARGIN		  5.0
#define PENALTY_AREA_LENGTH	 16.5
#define PENALTY_AREA_WIDTH	 40.32

#define left_SIDE   0
#define right_SIDE   1

#define my_TEAM  0
#define his_TEAM  1
#define unknown_TEAM 2


/** playmodes
    note that these are not the same than in the MDPstate
*/


typedef enum _PlayMode {
  PM_Null,            
  PM_before_kick_off,
  PM_time_over,         
  PM_play_on,           
  PM_kick_off_l,        
  PM_kick_off_r,        
  PM_kick_in_l,         
  PM_kick_in_r,         
  PM_free_kick_l,       
  PM_free_kick_r,       
  PM_corner_kick_l,     
  PM_corner_kick_r,     
  PM_goal_kick_l,       
  PM_goal_kick_r,       
  PM_goal_l,            
  PM_goal_r,            
  PM_drop_ball,         
  PM_offside_l,         
  PM_offside_r,
  PM_goalie_catch_ball_l,
  PM_goalie_catch_ball_r,
  PM_MAX             
} PlayMode ;

#define PLAYMODE_STRINGS {"",\
                        "before_kick_off",\
                        "time_over",\
                        "play_on",\
                        "kick_off_l",\
                        "kick_off_r",\
                        "kick_in_l",\
                        "kick_in_r",\
                        "free_kick_l",\
                        "free_kick_r",\
                        "corner_kick_l",\
                        "corner_kick_r",\
                        "goal_kick_l",\
                        "goal_kick_r",\
                        "goal_l",\
                        "goal_r",\
                        "drop_ball",\
                        "offside_l",\
                        "offside_r",\
                        "goalie_catch_ball_l",\
                        "goalie_catch_ball_r"}


#endif
