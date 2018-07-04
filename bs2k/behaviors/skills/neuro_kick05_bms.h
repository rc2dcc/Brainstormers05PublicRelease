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

#ifndef _NEURO_KICK05_BMS_H_
#define _NEURO_KICK05_BMS_H_

/* This behavior is the re-learnt RL-based kick behavior, written in
 * April 2005. It features improved performance, in particular for
 * high-speed kicks, compared to the NeuroKick-Behavior, while showing
 * identical performance for slower kicks.
 * 
 * It currently makes use of 4 neural nets which where trained for
 * different speeds (the decision which net to use for which desired
 * kick velocity is made in method chooseCurrentNet()).
 * 
 * The OneStep- and OneTwoStepKick-Behaviors are fully integrated;
 * they were also incorporated during learning already.
 * 
 * The behavior is capable of reading relevant parameters from a
 * config file
 *  - maximal_kick_velocity_deviation [0.1]
 *  - maximal_kick_direction_deviation [10degrees]
 * however, the behavior at hand has been optimized for the default
 * values.
 * Furthermore, the behavior has been optimized for the current settings
 * of constant class variables
 *  - cvcMinimalDesiredBallPlayerDistanceRelative  = 0.1
 *  - cvcMaximalDesiredBallPlayerDistanceRelative  = 1.0
 * which determine allowed ball regions within the player's kickable
 * area.
 * 
 * Note, that the learning process has always assumed relative ball-player
 * distances (within [0.0; 1.0], corresponding to [0.0m, kick_radius]) so
 * that an abstraction from heterogeneous player types (with varying kick
 * radii from 0.7 to 0.9) could be introduced.
 * 
 * The 'interface' to this class (public methods) has intentionally been
 * made identical to NeuroKick's interface.
 * 
 * Thomas Gabel, 2005
 */

#include "../base_bm.h"
#include "angle.h"
#include "Vector.h"
#include "tools.h"
#include "cmd.h"
#include "n++.h"
#include "macro_msg.h"
#include "valueparser.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"
#include "oneortwo_step_kick_bms.h"
#include "one_step_kick_bms.h"
#include "mystate.h"

// MACRO DEFINITIONS
#define NUMBER_OF_NEURAL_NETS  4

//============================================================================
// CLASS NeuroKick05ItrActions 
//============================================================================
class NeuroKick05ItrActions 
{
  /* set parameterss */
  static const Value ivKickPowerMin           = 5;
  static const Value ivKickPowerInc           = 5;
  static const Value ivKickPowerSteps         = 20;
  static const Value ivFinetuneKickPowerInc   = 1;
  static const Value ivFinetuneKickPowerSteps = 10;
  static const Value ivKickAngleMin           = 0;
  static const Value ivKickAngleInc           = 2*PI/62;//==360/62=5
  static const Value ivKickAngleSteps         = 62;
  static const Value ivFinetuneKickAngleInc   = 2*PI/360;
  static const Value ivFinetuneKickAngleSteps = 90;

  Value ivPowerMin,
        ivPowerInc,
        ivPowerSteps,
        ivAngleSteps;
  ANGLE ivAngleMin,
        ivAngleInc;
  
  Cmd_Main ivAction;
  int      ivAngleDone,
           ivPowerDone;
  Value    ivPower;
  ANGLE    ivAngle;
  
  //--------------------------------------------------------------------------
  public:
  //--------------------------------------------------------------------------
  void reset (bool   finetune=false,
              Value  orig_pwr=0,
              ANGLE  orig_ang=ANGLE(0)) 
  {
    if (!finetune) 
    {
      ivAngleMin   = ANGLE(ivKickAngleMin);
      ivAngleInc   = ANGLE(ivKickAngleInc);
      ivAngleSteps = ivKickAngleSteps;
      ivPowerMin   = ivKickPowerMin;
      ivPowerInc   = ivKickPowerInc;
      ivPowerSteps = ivKickPowerSteps;
    } 
    else 
    {
      ivAngleMin   =   orig_ang
                     - ANGLE(0.5*ivFinetuneKickAngleSteps*ivFinetuneKickAngleInc);
      ivAngleInc   = ANGLE(ivFinetuneKickAngleInc);
      ivAngleSteps = ivFinetuneKickAngleSteps + 1;
      ivPowerMin   =   orig_pwr
                     - (0.5*ivFinetuneKickPowerSteps*ivFinetuneKickPowerInc);
      ivPowerInc   = ivFinetuneKickPowerInc;
      ivPowerSteps = ivFinetuneKickPowerSteps + 1;
    }
    ivAngleDone = 0;
    ivPowerDone = 0;
    ivAngle = ivAngleMin; 
    ivPower = ivPowerMin;
  }
  
  Cmd_Main *next() 
  {
    if ( ivPowerDone<ivPowerSteps && ivAngleDone<ivAngleSteps ) 
    {
      ivAction.unset_lock();
      ivAction.unset_cmd();
      ivAction.set_kick(ivPower,ivAngle.get_value_mPI_pPI());
      ivAngle+=ivAngleInc;
      if(++ivAngleDone>=ivAngleSteps) 
      {
        ivAngle     = ivAngleMin;
        ivAngleDone = 0;
        ivPower += ivPowerInc;
        ivPowerDone++;
      }
      return &ivAction;
    }
    return NULL;
  }
};

//============================================================================
// CLASS NeuroKick05State
//============================================================================
class NeuroKick05State
{
  private:
  public:
    //pre-processed state information (to be used as neural net input)
    Value  ivBallPositionRelativeAngle; //relative angle of current ball pos
                                        //to my body orientation
    Value  ivRelativeBallDistance; //relative means relative to my kick raidus
    Vector ivRelativeBallVelocity;
    Value  ivRelativeBodyToKickAngle; //relative angle of kick direction
                                      //to my current body orientation
    //world information
    Vector ivMyWorldPosition;
    Vector ivMyWorldVelocity;
    ANGLE  ivMyWorldANGLE;
    Vector ivBallWorldPosition;
    Vector ivBallWorldVelocity;
    //opponent information
    Vector ivOpponentPosition;
    ANGLE  ivOpponentBodyAngle;
    int    ivOpponentBodyAngleAge;
    
    NeuroKick05State()
    { 
      ivBallPositionRelativeAngle = 0.0;        
      ivRelativeBallDistance = 0.0;
      ivRelativeBallVelocity = Vector(0.0,0.0); 
      ivRelativeBodyToKickAngle = 0.0;
      ivMyWorldPosition = Vector(0.0,0.0);
      ivMyWorldVelocity = Vector(0.0,0.0);
      ivMyWorldANGLE = ANGLE(0.0);
      ivBallWorldPosition = Vector(0.0,0.0);
      ivBallWorldVelocity = Vector(0.0,0.0);
      ivOpponentPosition = Vector(1000.0,1000.0);
      ivOpponentBodyAngle = ANGLE(0.0);
      ivOpponentBodyAngleAge = 0;
    }
};

//============================================================================
// CLASS NeuroKick05
//============================================================================
class NeuroKick05: public BaseBehavior 
{
#if 0
  struct State {
    Vector my_vel;
    Vector my_pos;
    ANGLE  my_ang;
    Vector ball_pos;
    Vector ball_vel;
  };
#endif

  //--------------------------------------------------------------------------
  private:
  //--------------------------------------------------------------------------

  //variables
  
  static bool    cvInitialized;
  static Value   cvMaximalKickVelocityDeviation;
  static Value   cvMaximalKickDirectionDeviation;
  
  static const Value cvcMinimalDesiredBallPlayerDistanceRelative  = 0.1;
  static const Value cvcMaximalDesiredBallPlayerDistanceRelative  = 1.0;

  static Net   * cvpNetArray[NUMBER_OF_NEURAL_NETS];
  Net          * ivpCurrentNet;
  
  NeuroKick05ItrActions   ivActionInterator;
  OneStepKick           * ivpOneStepKickBehavior;
  OneOrTwoStepKick      * ivpOneOrTwoStepKickBehavior;
  
  long           ivInitInCycle;
  Value          ivTargetKickVelocity;
  ANGLE          ivTargetKickAngle;
  Vector         ivTargetKickPosition;
  bool           ivDoTargetTracking; 
  bool           ivDoFineTuning;
  
  NeuroKick05State   ivFakeState;
  long               ivFakeStateTime;
  
  //methods
  Net            * chooseCurrentNet();
  void             completeState(NeuroKick05State &state);
  Value            computeInitialVelocityForFinalVelocity(Value  distance,
                                                          Value  finalVelocity);
  bool             decideForAnAction(Cmd &cmd);
  Value            evaluateState(NeuroKick05State & state);
  NeuroKick05State getCurrentState();  
  void             getWSState(NeuroKick05State &s);
  bool             isFailureState(const NeuroKick05State & state);
  void             setNeuralNetInput( NeuroKick05State & state,
                                      Value                    targetVelocity,
                                      ANGLE                    targetDir,
                                      float                  * net_in );

  //--------------------------------------------------------------------------
  public:
  //--------------------------------------------------------------------------
  //constructor
  NeuroKick05();
  //destructor
  virtual ~NeuroKick05();
  //static methods
  static 
    bool init(char const *confFile, int argc, char const* const* argv);
  //non-static methods
  /** This makes it possible to "fake" WS information.
   *  This must be called _BEFORE_ any of the kick functions, and is valid for 
   *  the current cycle only.
   */
  void set_state( const Vector &myPos, 
                 const Vector &myVel,
                 const ANGLE  &myAng,
                 const Vector &ballPos,
                 const Vector &ballVel);
  /** Resets the current state to that found in WS.
   *  This must be called _BEFORE_ any of the kick functions.
   */
  void reset_state();
  //kick initializing methods
  void kick_to_pos_with_initial_vel( Value         vel,
                                     const Vector &pos );
  void kick_to_pos_with_final_vel  ( Value         vel, 
                                     const Vector &pos );
  void kick_to_pos_with_max_vel    ( const Vector &pos ); 
  void kick_in_dir_with_initial_vel( Value         vel, 
                                     const ANGLE  &dir );
  void kick_in_dir_with_max_vel    ( const ANGLE  &dir );
  //'main' method: get command
  bool get_cmd(Cmd & cmd);
};
    
#endif
