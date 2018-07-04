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

#ifndef _GOALIE03_H_
#define _GOALIE03_H_

//#include "base_policies.h"
//#include "move_factory.h"
#include "../policy/policy_tools.h"
#include "Vector.h"
#include "mdp_info.h"
#include "base_bm.h"
#include "skills/neuro_go2pos_bms.h"
#include "skills/neuro_intercept_bms.h"
#include "skills/basic_cmd_bms.h"
#include "skills/face_ball_bms.h"
#include "skills/neuro_kick05_bms.h"
#include "skills/onetwo_holdturn_bms.h"
#include "skills/oneortwo_step_kick_bms.h"
#include "skills/go2pos_backwards_bms.h"

//#include "globals.h"

/**
 */
class Goalie03 : public BaseBehavior {
  static bool initialized;

  NeuroGo2Pos *go2pos;
  NeuroIntercept *intercept;
  BasicCmd *basic_cmd;
  FaceBall *face_ball;
  NeuroKick05 *neuro_kick;
  OneOrTwoStepKick *oneortwo;
  Go2PosBackwards *go2pos_backwards;
  OneTwoHoldTurn *hold_turn;

 protected:
  /* Daniel 09.04 2001 */
  Vector left_corner;
  Vector right_corner;
  Vector ball_vel;
  Vector ball_pos;
  double ball_angle;
  Vector my_pos;
  Vector my_vel;
  Value last_player_line;
  double dist_to_r_corner;
  double dist_to_l_corner;
  int direction;
  int direction_t;
  int use_trapez;
  Vector pos_trap;
  int break_through_nr;
  Vector id_pos_when_goalshot;
  Vector id_Punkt_auf_Tor;
  static const int TOP = 1;
  static const int BOTTOM = 0;
  static const int NORTH_WEST = 0;
  static const int NORTH_EAST = 1;
  static const int NORTH = 2;
  static const int SOUTH = 3;
  static const int SOUTH_WEST = 4;
  static const int SOUTH_EAST = 5;
  static const double DEG2RAD = M_PI/180;
  static const double RAD2DEG = 180/M_PI;
  static double TOP_TRAPEZ_ANGLE;
  static double BOTTOM_TRAPEZ_ANGLE;
  static Value xA;

  struct{
    float me;
    float my_goalie;
    float teammate;
    int teammate_number;
    Vector teammate_pos;
    Bool ball_kickable_for_teammate;
    float opponent;
    int opponent_number;
    Vector opponent_pos;
    Bool ball_kickable_for_opponent;
  } steps2go;


  //Main_Move* move;
  double ball_near_treshold;
  double goalside_tolerance;
  double normal_tolerance;
  bool is_ball_catchable;
  int last_catch_time;
  int last_intercept;
  bool intercept_out_of_penalty_area;
  int last_intercept_outside_p;
  int last_time_ball_pos_invalid;
  int last_go2pos;
  bool less_than_two_steps;
  Vector saved_go2pos;

  Go2Ball_Steps * go2ball_steps;

  void compute_steps2go();

  int was_last_move_intercept();
  int was_last_move_intercept_p();
  int opponent_steps_to_ball();
  int ball_steps_to(Vector pos);
  int ball_steps_to_with_max_speed(Vector pos);
  int goalie_steps_to(Vector pos);
  double skalarprodukt(Vector v1, Vector v2);
  bool go_to_y(Cmd &cmd, Vector pos, int priority = 0);
  Vector positioniere(double gewichtung, int anticipate_ball_pos);
  bool test_2step_go2pos(Cmd &cmd);
  bool get_Move_Line(Cmd &cmd, int ball_near_goal);
  bool test_catch(Cmd &cmd);
  bool test_kick(Cmd &cmd);
  bool test_teammate_control(Cmd &cmd);
  bool test_goal_shot(Cmd &cmd);
  bool test_turn_up(Cmd &cmd);
  bool test_fastest_to_ball(Cmd &cmd);
  bool test_turn_around(Cmd &cmd);
  bool get_Neuro_Go2Pos_back_forw(Cmd &cmd, double x, double y, int steps);
  bool goalshot_avoid_intercept(Cmd &cmd, Vector pos);
  bool test_corner_goalshot(Cmd &cmd);
  bool go_to_pos(Cmd &cmd, Vector pos);
  Value power_needed_for_distance(Value dist);
  int goalie_early_intercept(int *resPtrA, Vector *target_pos);
  int teammate_steps_to_ball();
  Value dash_power_needed_for_distance(Value dist);

  void select_defaultgoalkick_from_mypos(Vector &bestpos, float &vel, float &dir, int &playernumber);
  int is_defender_in_rectangle(Value x, Value y);
  bool test_kick_trapez(Cmd &cmd);
  Vector positioniere_trapez();
  bool test_intercept_trapez(Cmd &cmd);
  bool test_goalshot_trapez(Cmd &cmd);
  int am_i_at_home_trapez(Value tolerance);
  bool test_turn_around_trapez(Cmd &cmd);
  bool test_turn_up_trapez(Cmd &cmd, double tol = 4.0);
  bool get_Move_Trapez(Cmd &cmd);
  bool go_to_y_trapez(Cmd &cmd, Vector pos, int priority = 0);
  void set_vars();
  Angle calculate_deviation_threshold(Value distance);
  bool is_destination_reachable(const Vector& destination, Vector my_pos, 
				Vector my_vel, Angle my_ang, int turns);
  bool intercept_goalie(Cmd &cmd, int priority = 0);
  
  Vector point_on_line(Vector steigung, Vector line_point, double x);
  void log_steps2go();
  int am_I_looking_straight(double tolerance);
  int is_turning_allowed();
  int am_I_at_home(double faktor);
  int is_ball_heading_for_goal();
  int is_turning_FCP_allowed();
  int ball_steps_to_turn();
  Vector expected_ball_pos(int steps);
  Vector intersection_point(Vector p1, Vector steigung1, Vector p2, Vector steigung2);
  /* */
 public:

  static bool init(char const * conf_file, int argc, char const* const* argv) {
    if ( initialized )
      return true;
    initialized= true;
    
    return (
	    BasicCmd::init(conf_file, argc, argv) &&
	    NeuroGo2Pos::init(conf_file,argc,argv) &&
	    NeuroIntercept::init(conf_file,argc,argv) &&
	    FaceBall::init(conf_file,argc,argv) &&
	    NeuroKick05::init(conf_file,argc,argv) &&
	    OneTwoHoldTurn::init(conf_file,argc,argv) &&
	    OneOrTwoStepKick::init(conf_file,argc,argv) &&
	    Go2PosBackwards::init(conf_file, argc, argv)
	    );
  }

  Goalie03();
  virtual ~Goalie03();
  void reset_intention();
  bool get_cmd(Cmd &cmd);
};

#endif //_GOALIE03_H_

