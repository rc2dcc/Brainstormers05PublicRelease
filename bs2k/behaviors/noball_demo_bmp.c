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

#include "bs03_bmc.h"
#include "noball_demo_bmp.h"
#include "log_macros.h"
#include "../basics/blackboard.h"
#include "../basics/intercept.h"
#include "../basics/ws_memory.h"
#include "../../lib/angle.h"


//////////////////////////////////////////////////////////////////////////////
//Definition and Initialization of (Static) Class Variables
//////////////////////////////////////////////////////////////////////////////
bool 
  NoballDemo::initialized = false;

//////////////////////////////////////////////////////////////////////////////
// STATIC METHODS
//////////////////////////////////////////////////////////////////////////////

/**
 * Initialization of class NoballDemo
 */
bool 
NoballDemo::init(char const * confFile, int argc, char const* const* argv) 
{
      if ( initialized )
        return true;
      initialized = 
           NeuroIntercept ::init(confFile, argc, argv)
        && NeuroGo2Pos    ::init(confFile, argc, argv)
        && FaceBall       ::init(confFile, argc, argv);
      return initialized;
}

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor for class NoballDemo.
 */
NoballDemo::NoballDemo()
{
  ivpNeuroInterceptBehavior  = new NeuroIntercept();
  ivpInterceptBallBehavior   = new InterceptBall();
  ivpGo2PosBehavior          = new NeuroGo2Pos();
  ivpFaceBallBehavior        = new FaceBall();
  ivpGo2PosBackwardsBehavior = new Go2PosBackwards();
  ivpBasicCmdBehavior        = new BasicCmd();
  
  ivWeAreAttacking                              = false;
  ivIAmFastestInterceptor                       = false;
}

/**
 * The destructor for class NoballDemo.
 */
NoballDemo::~NoballDemo()
{
  if (ivpNeuroInterceptBehavior) delete ivpNeuroInterceptBehavior;
  if (ivpInterceptBallBehavior) delete ivpInterceptBallBehavior;
  if (ivpGo2PosBehavior) delete ivpGo2PosBehavior;
  if (ivpFaceBallBehavior) delete ivpFaceBallBehavior;
  if (ivpGo2PosBackwardsBehavior) delete ivpGo2PosBackwardsBehavior;
  if (ivpBasicCmdBehavior) delete ivpBasicCmdBehavior;
}

//////////////////////////////////////////////////////////////////////////////
// NON-STATIC METHODS
//////////////////////////////////////////////////////////////////////////////

//============================================================================
// amIAttacker()
//============================================================================
bool     
NoballDemo::amIAttacker()
{
  LOG_POL(0,<<"NoballDemo::amIAttacker(): START [time="<<WSinfo::ws->time<<"]"<<std::flush);
  this->initCycle(); //there, relevant instance variables (like ivWeAreAttacking)
                     //are set by calls to e.g. updateInterceptInformation
  int role = DeltaPositioning::get_role(WSinfo::me->number);
  if (role == PT_DEFENDER) return false;
  return ivWeAreAttacking;
}

//============================================================================
// createAbstractActionToDefend()
//============================================================================
bool
NoballDemo::createAbstractActionToDefend()
{
  Vector target = DeltaPositioning::get_position(WSinfo::me->number);
  Value  targetTolerance = 1.0;
  bool   considerObstacles = true;
  bool   useOldGo2Pos = true;
  this->setAActionGoto(  target, 
                         targetTolerance, 
                         considerObstacles, 
                         useOldGo2Pos);
  return true;
}

//============================================================================
// createAbstractActionToInterceptTheBall()
//============================================================================
/**
 * 
 */
bool
NoballDemo::createAbstractActionToInterceptTheBall()
{
  this->setAActionGo2Ball();
  return true;
}

//============================================================================
// createOrPrepareAbstractActionToTackle()
//============================================================================
bool
NoballDemo::createOrPrepareAbstractActionToTackle()
{
  this->setAActionTackle(100.0);
  return true;
}

//============================================================================
// createAbstractActionToSearchBall()
//============================================================================
bool
NoballDemo::createAbstractActionToSearchBall()
{
  this->setAActionFaceBall(false);
  return true;
}

//============================================================================
// createAbstractActionToSupportAttacking()
//============================================================================
bool
NoballDemo::createAbstractActionToSupportAttacking()
{
  Vector target = DeltaPositioning::get_position(WSinfo::me->number);
  target.x += 5.0;
  Value  targetTolerance = 1.0;
  bool   considerObstacles = true;
  bool   useOldGo2Pos = true;
  this->setAActionGoto(  target, 
                         targetTolerance, 
                         considerObstacles, 
                         useOldGo2Pos);
  return true;
}


//============================================================================
// decideForAnAbstractAction()
//============================================================================
bool
NoballDemo::decideForAnAbstractAction()
{
  if ( !WSinfo::is_ball_pos_valid())
  {
    createAbstractActionToSearchBall();
    return true;
  }
  
  if ( ivIAmFastestInterceptor )
  {
    createAbstractActionToInterceptTheBall();
    return true;
  }
  if ( ivWeAreAttacking )
  {
    createAbstractActionToSupportAttacking();
    return true;
  }
  else
  {
    createAbstractActionToDefend();
    return true;
  }
  return true;
}



//============================================================================
// get_cmd(Cmd & cmd)
//============================================================================
/**
 * get_cmd represents the main method of this behavior. 
 * @param The 'cmd' object is given as a reference parameter and will be
 * filled by this method.
 * @return This method returns true if a command could be set, otherwise
 * false.
 */
bool 
NoballDemo::get_cmd(Cmd & cmd)
{
  bool stillOk = true;
  if (stillOk) stillOk = this->initCycle();
  if (!stillOk) return false;
  if (stillOk) stillOk = this->decideForAnAbstractAction();
  if (!stillOk) return false;
  if (stillOk) stillOk = this->translateAbstractActionIntoCommand(cmd);
  if (!stillOk) return false;
  return stillOk;
}
    
//============================================================================
// initCycle()
//============================================================================
/**
 * initCycle() initialises this bevahior at the beginning of the current
 * cycle in which it has been called.
 */
bool 
NoballDemo::initCycle()
{
  if (    ivCurrentCycleTime != WSinfo::ws->time
       || WSinfo::ws->play_mode != PM_PlayOn )
  {
    ivCurrentAbstractAction.action_type = -1; //no action by default

    mdpInfo::clear_my_intention();
    
    this->updateInterceptInformation();

    ivCurrentCycleTime = WSinfo::ws->time;
  }
  return true;
}


//============================================================================
// reset_intention
//============================================================================
/**
 * Reset potential intentions from the previous cycle.
 */
void 
NoballDemo::reset_intention()
{
}

//============================================================================
// setAActionGoto()
//============================================================================
void 
NoballDemo::setAActionGoto(  Vector target, 
                           Value  targetTolerance, 
                           int    considerObstacles, 
                           int    useOldGo2Pos) 
{
    AbstractMDP::set_action(  ivCurrentAbstractAction, 
                              AACTION_TYPE_GOTO, 
                              WSinfo::me->number, 
                              useOldGo2Pos, 
                              target, 
                              targetTolerance, 
                              0.0, 
                              Vector(0,0), 
                              Vector(0,0), 
                              considerObstacles);
}

//============================================================================
// setAActionGo2Ball() 
//============================================================================
void 
NoballDemo::setAActionGo2Ball() 
{
  mdpInfo::set_my_intention(DECISION_TYPE_INTERCEPTBALL);//look to ball
  AbstractMDP::set_action( ivCurrentAbstractAction, 
                           AACTION_TYPE_GO2BALL, 
                           WSinfo::me->number, 
                           0, Vector(0), 0);
}

//============================================================================
// setAActionFaceBall()
//============================================================================
void 
NoballDemo::setAActionFaceBall(bool lockNeckToBody) 
{
  int receiver = -1;
  if (lockNeckToBody == true) 
    receiver = 0; //receiver == -1 entspricht "neck nicht gelockt"
                  //receiver != -1 entsrpicht "neck zum koerper gelockt"
  AbstractMDP::set_action(  ivCurrentAbstractAction, 
                            AACTION_TYPE_FACE_BALL, 
                            WSinfo::me->number, 
                            receiver, 
                            Vector(0), 0);
}

//============================================================================
// setAActionFaceBall()
//============================================================================
void 
NoballDemo::setAActionKick(Value power, Angle dir)
{
  AbstractMDP::set_action( ivCurrentAbstractAction, 
                           AACTION_TYPE_SCORE,
                           WSinfo::me->number, 
                           0,
                           Vector(0),
                           power,
                           dir    );
}

//============================================================================
// setAActionStop()
//============================================================================
void
NoballDemo::setAActionStop()
{
  AbstractMDP::set_action( ivCurrentAbstractAction,
                           AACTION_TYPE_STAY,
                           WSinfo::me->number,
                           0,
                           Vector(0),
                           Tools::get_dash2stop() );
}

//============================================================================
// setAActionTackle()
//============================================================================
void 
NoballDemo::setAActionTackle(Value power) 
{
  AbstractMDP::set_action( ivCurrentAbstractAction, 
                           AACTION_TYPE_TACKLE, 
                           WSinfo::me->number, 
                           0, 
                           Vector(0), 
                           power);
}


//============================================================================
// setAActionTurnInertia()
//============================================================================
void 
NoballDemo::setAActionTurnInertia( ANGLE ang) 
{
  AbstractMDP::set_action( ivCurrentAbstractAction, 
                           AACTION_TYPE_TURN_INERTIA, 
                           WSinfo::me->number, 
                           0, Vector(0), 0, 
                           ang.get_value_mPI_pPI());
}

//============================================================================
// translateAbstractActionIntoCommand()
//============================================================================
/**
 * This method translates the current abstract action into a command.
 */
bool 
NoballDemo::translateAbstractActionIntoCommand(Cmd &cmd)
{
  bool returnValue;
  //general information on the current abstract action
  Value   speed   = ivCurrentAbstractAction.kick_velocity;
  Vector  target  = ivCurrentAbstractAction.target_position;
  Value   dir     = ivCurrentAbstractAction.kick_dir;
  int     considerObstacles  = ivCurrentAbstractAction.advantage;
  int     receiver           = ivCurrentAbstractAction.target_player;
  Vector  virtualBallPos = ivCurrentAbstractAction.virtual_ballpos,
          virtualBallVel = ivCurrentAbstractAction.virtual_ballvel; 
  

  switch (ivCurrentAbstractAction.action_type)
  {
      //----------------------------------------------------------------------
      case AACTION_TYPE_GOTO:
      //----------------------------------------------------------------------
      {
        bool willIReachTargetByDoingNothing = false;
        Vector myNextPos = WSinfo::me->pos + WSinfo::me->vel;
        if (myNextPos.distance(target) <= 1.0)
          willIReachTargetByDoingNothing = true;

        if (   WSinfo::me->pos.distance(target) > 1.0
            && willIReachTargetByDoingNothing == false)
        {
            ivpGo2PosBehavior->set_target
              (target, 1.0, considerObstacles, receiver );
            returnValue = ivpGo2PosBehavior->get_cmd(cmd);
        }
        else
        {
          ivpFaceBallBehavior->turn_to_ball(false);
          returnValue = ivpFaceBallBehavior->get_cmd(cmd);
        }
        break;
      }

      //----------------------------------------------------------------------
      case AACTION_TYPE_GO2BALL:
      //----------------------------------------------------------------------
      {
        LOG_POL(0,<<  "  NeuroIntercept will be used ..."<<std::flush);
        returnValue = this->ivpNeuroInterceptBehavior->get_cmd(cmd);
        break;
      }

      //----------------------------------------------------------------------
      case AACTION_TYPE_GO4PASS:
      //----------------------------------------------------------------------
      {
        LOG_POL(0,<<  "  NeuroIntercept will be used ..."<<std::flush);
        ivpNeuroInterceptBehavior->set_virtual_state( virtualBallPos,
                                                      virtualBallVel );
        returnValue = this->ivpNeuroInterceptBehavior->get_cmd(cmd);
        break;
      }
      
      //----------------------------------------------------------------------
      case AACTION_TYPE_SCORE:
      //----------------------------------------------------------------------
      {
        LOG_POL(0, << "Chosen AAction: SCORE "<<std::flush);
        LOG_POL(0,<<  "  BasicCmd will be used ..."<<std::flush);
        this->ivpBasicCmdBehavior->set_kick(speed, ANGLE(dir) );
        returnValue = ivpBasicCmdBehavior->get_cmd(cmd);
        break;
      }

      //----------------------------------------------------------------------
      case AACTION_TYPE_STAY:
      //----------------------------------------------------------------------
      {
        LOG_POL(0, << "Chosen AAction: STAY (STOP) "<<std::flush);
        LOG_POL(0,<<  "  BasicCmd will be used ... dash with "<<speed<<std::flush);
        this->ivpBasicCmdBehavior->set_dash(speed);
        returnValue = ivpBasicCmdBehavior->get_cmd(cmd);
        break;
      }
      
      //----------------------------------------------------------------------
      case AACTION_TYPE_TURN_INERTIA:
      //----------------------------------------------------------------------
      {
        LOG_POL(0, << "Chosen AAction: TURN INERTIA "<<std::flush);
        ivpBasicCmdBehavior->set_turn_inertia(dir);
        LOG_POL(0,<<"  BasicCmd (turn inertia "<<dir<<") is used (turn to direct opponent) ..."<<std::flush);
        returnValue = ivpBasicCmdBehavior->get_cmd(cmd);
        break;
      }

      //----------------------------------------------------------------------
      case AACTION_TYPE_FACE_BALL:
      //----------------------------------------------------------------------
      {
        LOG_POL(0, << "Chosen AAction: FACE BALL "<<std::flush);
        if (receiver == -1) //receiver ==-1 ==> neck nicht gelockt
          ivpFaceBallBehavior->turn_to_ball(false);
        else                //receiver !=-1 ==> neck gelockt zum koerper
          ivpFaceBallBehavior->turn_to_ball(true);
        ivpFaceBallBehavior->get_cmd(cmd);
        returnValue = true;
        break;
      }

      //----------------------------------------------------------------------
      case AACTION_TYPE_TACKLE:
      //----------------------------------------------------------------------
      {
        LOG_POL(0, << "Chosen AAction: TACKLE "<<std::flush);
        cmd.cmd_main.unset_lock();
        cmd.cmd_main.unset_cmd();
        ivpBasicCmdBehavior->set_tackle(100.0);
        returnValue = ivpBasicCmdBehavior->get_cmd(cmd);
        break;
      }

      //----------------------------------------------------------------------
      default:
      //----------------------------------------------------------------------
      {
        LOG_ERR(0, <<"NoballDemo aaction2cmd: AActionType " 
                   << ivCurrentAbstractAction.action_type << " not known");
        returnValue = true;
      }
  }
  return returnValue;
}

//============================================================================
// updateInterceptInformation()
//============================================================================
/**
 * This method calculates the number of steps that would be necessary for 
 *   - me
 *   - the best-positioned opponent player
 *   - the best-positioned teammate player
 * to intercept the ball.
 * @return No return value, however, the results of the calculations are
 * stored within the data structure ivCurrentInterceptInformation.
 */
void 
NoballDemo::updateInterceptInformation()
{
  int teamInAttack = WSmemory::team_in_attack();
  //are we attacking?
  ivWeAreAttacking
    = ( WSinfo::is_ball_pos_valid() && teamInAttack == MY_TEAM );

  WSpset pset_tmp = WSinfo::valid_teammates;
  pset_tmp += WSinfo::valid_opponents;
  int numberOfConsideredInterceptors = pset_tmp.num;
  InterceptResult intercept_res[numberOfConsideredInterceptors];
  pset_tmp.keep_and_sort_best_interceptors_with_intercept_behavior_to_WSinfoBallPos
           (numberOfConsideredInterceptors, 
            /*WSinfo::ball->pos, WSinfo::ball->vel,*/ intercept_res);
  ivCurrentInterceptInformation.myStepsToGo       = 1000;
  ivCurrentInterceptInformation.opponentStepsToGo = 1000;
  ivCurrentInterceptInformation.opponentNumber    = -1;
  ivCurrentInterceptInformation.teammateStepsToGo = 1000;
  ivCurrentInterceptInformation.teammateNumber    = -1;
  ivCurrentInterceptInformation.myGoalieStepsToGo = 1000;
  
  //intercept considerations
  for (int i=0; i<numberOfConsideredInterceptors; i++)
  {
    if (    ivCurrentInterceptInformation.opponentNumber == -1
         && pset_tmp[i]->team == HIS_TEAM)
    {
      ivCurrentInterceptInformation.opponentNumber = pset_tmp[i]->number;
      ivCurrentInterceptInformation.opponentStepsToGo = intercept_res[i].time;
    }
    if (    ivCurrentInterceptInformation.teammateNumber == -1
         && pset_tmp[i]->team == MY_TEAM )
    {
      if (   pset_tmp[i]->number != WSinfo::me->number
          && pset_tmp[i]->number != WSinfo::ws->my_goalie_number
         )
      {
        ivCurrentInterceptInformation.teammateNumber = pset_tmp[i]->number;
        ivCurrentInterceptInformation.teammateStepsToGo = intercept_res[i].time;
      }
    }
    if (    pset_tmp[i]->team   == MY_TEAM
         && pset_tmp[i]->number == WSinfo::ws->my_goalie_number)
      ivCurrentInterceptInformation.myGoalieStepsToGo = intercept_res[i].time;
    if (    pset_tmp[i]->team   == MY_TEAM
         && pset_tmp[i]->number == WSinfo::me->number)
      ivCurrentInterceptInformation.myStepsToGo = intercept_res[i].time;
  }

  //set the position field in ivCurrentInterceptInformation
  PPlayer tmpPlayer 
    = WSinfo::get_opponent_by_number(
                               ivCurrentInterceptInformation.opponentNumber);
  if (tmpPlayer != NULL) 
  {
    ivCurrentInterceptInformation.opponentPosition = tmpPlayer->pos;
    ivCurrentInterceptInformation.isBallKickableForOpponent
       = WSinfo::is_ball_kickable_for(tmpPlayer);
  }
  else 
  {
    ivCurrentInterceptInformation.opponentPosition = Vector(0.0, 0.0);
    ivCurrentInterceptInformation.isBallKickableForOpponent = false;
  }

  tmpPlayer 
    = WSinfo::get_teammate_by_number(
                               ivCurrentInterceptInformation.teammateNumber);
  if (tmpPlayer != NULL) 
  {
    ivCurrentInterceptInformation.teammatePosition = tmpPlayer->pos;
    ivCurrentInterceptInformation.isBallKickableForTeammate
      = WSinfo::is_ball_kickable_for( tmpPlayer );
  }
  else 
  {
    ivCurrentInterceptInformation.teammatePosition = Vector(0.0, 0.0);
    ivCurrentInterceptInformation.isBallKickableForTeammate = false;
  }


  if (   WSinfo::is_ball_pos_valid()
      && ivCurrentInterceptInformation.myStepsToGo 
         <= ivCurrentInterceptInformation.teammateStepsToGo 
     )
  {
    ivIAmFastestInterceptor = true;
  }
  else 
  {
    ivIAmFastestInterceptor = false;
  }
}

