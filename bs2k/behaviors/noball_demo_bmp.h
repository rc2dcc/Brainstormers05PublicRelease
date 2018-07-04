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

#ifndef _NOBALL_DEMO_BMP_H_
#define _NOBALL_DEMO_BMP_H_

#include "../policy/abstract_mdp.h"
#include "../policy/policy_tools.h"
#include "../policy/positioning.h"
#include "skills/face_ball_bms.h"
#include "skills/go2pos_backwards_bms.h"
#include "skills/intercept_ball_bms.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"

//////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

class NoballDemo : public BaseBehavior 
{
  struct tInterceptInformation
  {
    int     myStepsToGo;
    int     myGoalieStepsToGo;
    int     teammateStepsToGo;
    int     teammateNumber;
    Vector  teammatePosition;
    bool    isBallKickableForTeammate;
    int     opponentStepsToGo;
    int     opponentNumber;
    Vector  opponentPosition;
    Bool    isBallKickableForOpponent;
  };
  
  ////////////////////////////////////////////////////////////////////////////
  // PRIVATE AREA
  ////////////////////////////////////////////////////////////////////////////
  private:
    //==VARIABLES=============================================================
    long                          ivCurrentCycleTime;
    bool                          ivIAmFastestInterceptor;
    bool                          ivWeAreAttacking;
    static 
      bool                        initialized;
    //other behaviors
    NeuroIntercept              * ivpNeuroInterceptBehavior;
    InterceptBall               * ivpInterceptBallBehavior;
    NeuroGo2Pos                 * ivpGo2PosBehavior;
    Go2PosBackwards             * ivpGo2PosBackwardsBehavior;
    FaceBall                    * ivpFaceBallBehavior;
    BasicCmd                    * ivpBasicCmdBehavior;
    //information holding variables
    tInterceptInformation         ivCurrentInterceptInformation;
    Go2Ball_Steps               * ivpGoToBallStepsList;
    //variables related to actions to be performed
    AAction                       ivCurrentAbstractAction;
    
    //==METHODS===============================================================
    bool      createAbstractActionToDefend();
    bool      createAbstractActionToInterceptTheBall();
    bool      createOrPrepareAbstractActionToTackle();
    bool      createAbstractActionToSearchBall();
    bool      createAbstractActionToSupportAttacking();

    bool      decideForAnAbstractAction();
    
    void      requestNeckRequest( int   neckRequestType, 
                                  ANGLE requestedAngle=ANGLE(0));
    void      setAActionGo2Ball();
    void      setAActionKick(Value power, Angle dir);
    void      setAActionTackle(Value power);

    void      setAActionFaceBall(bool lockNeckToBody);
    void      setAActionGoto(  Vector target, 
                               Value  targetTolerance, 
                               int    considerObstacles, 
                               int    useOldGo2Pos);
    void      setAActionStop();
    void      setAActionTurnInertia(ANGLE ang);

  ////////////////////////////////////////////////////////////////////////////
  // PUBLIC AREA
  ////////////////////////////////////////////////////////////////////////////
  public:
      //constructor and destructor
      NoballDemo();
      virtual 
        ~NoballDemo();
      //static methods
      static 
         bool  init(char const * conf_file, int argc, char const* const* argv);
      //non-static methods
      bool     amIAttacker();
      virtual  bool     initCycle();
      virtual  bool     get_cmd(Cmd &cmd);
      void     reset_intention();
      bool     translateAbstractActionIntoCommand(Cmd & cmd);
      void     updateInterceptInformation();
};

#endif //_NOBALLDEMO_BMP_H_
