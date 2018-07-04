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

#ifndef INTERCEPT_BALL_BMS_H_
#define INTERCEPT_BALL_BMS_H_

/* This behavior implements a hand-coded intercept.
   The original code was written by Alexander Sung, and imported
   into our old move layout by Martin Riedmiller, who also
   added some improvements. It has finally been converted to
   our new concept of behaviors by Manuel Nickschas in 2003.

   Just call get_cmd(Cmd&); no further function calls are necessary.
   Return value will always be true.
*/


#include "../base_bm.h"
#include "types.h"
#include "cmd.h"
#include "log_macros.h"

class InterceptBall : public BaseBehavior {
  static bool initialized;

  bool final_state();
  static Angle calculate_deviation_threshold(Value distance);
  static bool is_destination_reachable(const Vector& destination, Vector my_pos, Vector my_vel, 
				       ANGLE my_ang, int turns);
  static bool is_destination_reachable2(const Vector& destination, Vector my_pos, Vector my_vel, 
				       ANGLE my_ang, int maxcycles);
  
  ANGLE my_angle_to(const Vector & my_pos, const ANGLE &my_angle, 
				   Vector target);
 public:
  InterceptBall();
  bool get_cmd(Cmd & cmd, const Vector & my_pos,const Vector & my_vel, 
	       const ANGLE my_ang, 
	       Vector ball_pos, Vector ball_vel, int &num_cycles);
  bool get_cmd(Cmd & cmd, const Vector & my_pos,const Vector & my_vel, 
	       const ANGLE my_ang, 
	       Vector ball_pos, Vector ball_vel);
  bool get_cmd(Cmd &cmd);
  bool get_cmd(Cmd &cmd, int &num_cycles);
  
  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if(initialized) return true;
    initialized = true;
    //cout << "\nInterceptBall behavior initialized.";
    return true;
  }

  private:
    Angle ivRequestedTurnAngle;
    long  ivValidityOfRequestedTurnAngle;
  public:
    bool  get_cmd_arbitraryPlayer( PPlayer player,
                                        Cmd & cmd, 
                                        const Vector & my_pos,
                                        const Vector & my_vel, 
                                        const ANGLE my_ang, 
                                        Vector ball_pos, 
                                        Vector ball_vel, 
                                        int &num_cycles,
                                        int maxcycles = 30);
    bool is_destination_reachable2_arbitraryPlayer
                               (PPlayer player,
                                const Vector& destination, 
                                Vector my_pos,
                                Vector my_vel, 
                                ANGLE my_ang, 
                                int maxcycles);
    bool checkForOptimzed1StepIntercept( PPlayer player,
                                         Cmd & cmd,
                                         Vector ballDestination,
                                         Vector myPos,
                                         Vector myVel,
                                         ANGLE  myAng );
    void setRequestForTurnAngle(Angle turnAngle, long timeOfValidity);
};

#endif
