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

#ifndef _INTERCEPT2_H_
#define _INTERCEPT2_H_

#include "Vector.h"
#include "ws_pset.h"
#include "cmd.h"

/*****************************************************************************/

struct InterceptResult2 {
  int time;
  Vector pos;
};
  
class Intercept2 {
  //static Value ball_vel_decay;
  //static Value log_ball_decay;


  struct NoneDashTurn {
    /** if type is 'turn', then val is the real angle we are going to turn, 
	and not the moment which will be applied
	if type is 'dash' the  val is the real acceleration applied 
	(do not need to multiply with dash_power_rate)
    */
	
    NoneDashTurn() { type= none; }
    enum Type { none, dash, turn };
    Type type;
    Value val;
    void set_turn(Value v) { type= turn; val= v; }
    void set_dash(Value v) { type= dash; val= v; }
    void set_none() { type= none; }
  };


  struct InterceptPositions {
    InterceptPositions() { reset(); }
    void reset() { num_pos= 0; }
    bool is_empty() { return !num_pos; }
#if 0
    int fastest_opponent_time; 
    PPlayer fastest_opponent;
    int fastest_teammate_time;
    PPlayer fastest_teammate;
#endif

    struct ResPos {
      int ball_time;
      int my_time;

      Vector ball_pos;
      Vector ball_vel;
      Value  ball_speed;
    };

    static const int MAX_RES_POS= 100;
    int num_pos;
    ResPos pos[MAX_RES_POS];


    bool add( ResPos const& res ) {
      if (num_pos >= MAX_RES_POS) {
	ERROR_OUT << "to much positions " << num_pos;
	return false;
      }
      pos[num_pos]= res;
      num_pos++;
      return true;
    }

    void show2d();
  };

  struct Object {
    Object(PPlayer p) { pos= p->pos; vel= p->vel; ang= p->ang; param= p; }
    PPlayer param;
    Vector pos;
    Vector vel;
    ANGLE ang;
#if 0
    int pos_age;
    int vel_age;
    int ang_age;
#endif
    void apply (NoneDashTurn const & ndt) {
      if ( ndt.type == ndt.dash ) {
	Vector acc(ang);
	vel= vel + ndt.val * acc;
	if ( vel.sqr_norm() > SQUARE(param->speed_max) ) {
	  vel.normalize( param->speed_max );
	  WARNING_OUT << " I'm wasting dashes";
	}
	pos += vel;
	vel *= param->decay;
	return;
      }
      pos += vel;
      vel *= param->decay;
      if ( ndt.type == ndt.turn ) 
	ang += ANGLE(ndt.val);
    }
  };

  InterceptPositions intercept_positions;

 public:

  struct {
    //time offset for the ball to start (if >= 0), or time ball is already moving (if < 0)
    int ball_start_time;
    Vector  ball_pos;
    Vector ball_vel;
    WSpset pset;
  } input;

  struct {
    WSpset pset;
  } output;

  void time_to_target( Vector const & target, Value tolerance, PPlayer player, int & time) const {
    Cmd_Main tmp;
    time_to_target(target, tolerance, player, time, tmp);
  }
  void time_to_target( Vector const & target, Value tolerance, PPlayer player, int & time, Cmd_Main & cmd ) const;

  void intercept_old( Vector const & ball_pos, Vector const & ball_vel, PPlayer player, int & time, Cmd_Main & cmd ) const;
  void intercept( Vector const & ball_pos, Vector const & ball_vel, PPlayer player, int & time, Cmd_Main & cmd );
  void compute_possible_intercept_positions( Vector const & ball_pos, Vector const & ball_vel, PPlayer player );
  void compute_fastest_opponent(Vector const & ball_pos, Vector const & ball_vel, int & time);
  bool can_reach_ball_with_dashing(Vector const & ball_pos, Vector const & ball_vel, Value tolerance, int & time, Vector & pos);
 public: //debug
  
  /** in canonical for the target is at the origin i.e. (0,0), and the players position is on the 
      negative x-axis!
  */
  void conv_to_canonical_form( Vector & target, Vector & pos, Vector & vel, ANGLE & ang) const; 

  void dash_to_minimize_vel(Object const & obj, Value & dash, Vector & new_vel ) const;
  void turn_to_pos( Vector & target, Object & obj, int & time, Value tolerance, NoneDashTurn & ndt ) const;
 public:
  /** if the needed time to intercept ball is > 2**(MAX_STEPS-2)  then 
      2**(MAX_STEPS-2) is the returned time and pos ist the ball position  
      after 2**(MAX_STEPS-2) steps */
  void minimal_time_player_reaches_ball_at_pos( int & time, Vector & pos );
  void minimal_time_player_reaches_ball_at_pos( InterceptResult & res );

  static bool init(char const * conf_file, int argc, char const* const* argv);
};


#endif
