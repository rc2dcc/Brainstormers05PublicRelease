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

#include "globaldef.h"

const char * play_mode_str(int play_mode) {
  switch (play_mode) {
  case PM_Unknown           :   return "PM_Unknown";
  case PM_my_BeforeKickOff  :   return "PM_my_BeforeKickOff";
  case PM_his_BeforeKickOff :   return "PM_his_BeforeKickOff";
  case PM_TimeOver          :   return "PM_TimeOver"; 
  case PM_PlayOn            :   return "PM_PlayOn"; 
  case PM_my_KickOff        :   return "PM_my_KickOff"; 
  case PM_his_KickOff       :   return "PM_his_KickOff "; 
  case PM_my_KickIn         :   return "PM_my_KickIn"; 
  case PM_his_KickIn        :   return "PM_his_KickIn"; 
  case PM_my_FreeKick       :   return "PM_my_FreeKick"; 
  case PM_his_FreeKick      :   return "PM_his_FreeKick"; 

  case PM_my_GoalieFreeKick :   return "PM_my_GoalieFreeKick";
  case PM_his_GoalieFreeKick:   return "PM_his_GoalieFreeKick";

  case PM_my_CornerKick     :   return "PM_my_CornerKick"; 
  case PM_his_CornerKick    :   return "PM_his_CornerKick"; 
  case PM_my_GoalKick       :   return "PM_my_GoalKick"; 
  case PM_his_GoalKick      :   return "PM_his_GoalKick"; 
  case PM_my_AfterGoal      :   return "PM_my_AfterGoal"; 
  case PM_his_AfterGoal     :   return "PM_his_AfterGoal"; 
  case PM_Drop_Ball         :   return "PM_Drop_Ball"; 
  case PM_my_OffSideKick    :   return "PM_my_OffSideKick"; 
  case PM_his_OffSideKick   :   return "PM_his_OffSideKick"; 
  case PM_Half_Time         :   return "PM_Half_Time"; 
  case PM_Extended_Time     :   return "PM_Extended_Time"; 
  case PM_his_BeforePenaltyKick :   return "PM_his_BeforePenaltyKick";
  case PM_my_BeforePenaltyKick  :   return "PM_my_BeforePenaltyKick";
  case PM_my_PenaltyKick    :   return "PM_my_PenaltyKick";
  case PM_his_PenaltyKick   :   return "PM_his_PenaltyKick";				
  default                   :   return "PM_XXXXXXXXXXXXXXXXX";
  }     
  return 0;
}
