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

#ifndef _TOOLS_H
#define _TOOLS_H

#include "globaldef.h"
#include "options.h"
#include "cmd.h"
#include "mystate.h"
#include "ws_pset.h"

#define NONE 0
#define LOW 1
#define MEDIUM 2
#define HIGH 3

#define EQUAL 0
#define FIRST 1
#define SECOND 2



class Tools {
  static const int MAX_NUM_POWERS= 100;
  static int num_powers;
  static Value ball_decay_powers[MAX_NUM_POWERS];
 public:
  static Vector Tools::ip_with_fieldborder(const Vector p, const float dir);
  static Vector Tools::ip_with_right_penaltyarea(const Vector p, const float dir);
  static Value Tools::ballspeed_for_dist(const Value dist);

  /** Return the angle in [-Pi,+Pi) */
  static Angle get_angle_between_mPI_pPI(Angle angle);
  /** Return the angle in [0,+2*Pi) */
  static Angle get_angle_between_null_2PI(Angle angle);

  static Value get_tackle_success_probability(Vector my_pos, Vector ball_pos, Value my_angle);

  /** Return the absolut size of angle */
  static Angle get_abs_angle(Angle angle);

  /** calculates my angle to another object. */
  static ANGLE my_angle_to(Vector target);

  static ANGLE my_abs_angle_to(Vector target); 
  
  /** calculates my neck angle to another object. */
  static ANGLE my_neck_angle_to(Vector target);

  // >>>>>>>>>>>>>>>>>>>>
  // These functions work only if Cmd_Main has already been set!!
  
  /** returns expected relative body turn angle */
  static ANGLE my_expected_turn_angle();

  static Value moment_to_turn_neck_to_abs(ANGLE abs_dir);

  /** gets the maximum angle I could see (abs, right) */
  static ANGLE my_maximum_abs_angle_to_see();

  /** gets the minimum angle I could see (abs, left) */
  static ANGLE my_minimum_abs_angle_to_see();

  /** returns true if abs direction can be in view_angle with appropriate neck turn */
  static bool could_see_in_direction(ANGLE target_ang);
  static bool could_see_in_direction(Angle target_ang);
  
  // <<<<<<<<<<<<<<<<<<<<<
  
  /** Return a random integer with 0 <= int < range. */
  static int int_random(int range);

  static Value range_random(Value lo, Value hi);
  
  static int very_random_int(int n);

  inline static bool equal(Value x,Value y,Value epsilon = 0.00001)
    {return(fabs(x-y)<epsilon);};

  /** Return the maximum */
  static Value max(Value a, Value b);
  static Value min(Value a, Value b);
  static int max(int a, int b);
  static int min(int a, int b);


  static Value Tools::get_dash2stop();
  static bool is_a_scoring_position(Vector pos);


  static Vector get_Lotfuss(Vector x1, Vector x2, Vector p);
  static Value get_dist2_line(Vector x1, Vector x2, Vector p);

  //art: returns time in ms since first call to this routine 
  static long get_current_ms_time();
  
  // returns info string. info[] must have length of 50 
  static void cmd2infostring(const Cmd_Main & cmd, char *info);

  // predicts player movement only. calls model_cmd_main.
  static void model_player_movement(const Vector & old_my_pos, const Vector & old_my_vel, 
				    const Angle & old_my_ang,
				    const Cmd_Main & cmd,
				    Vector & new_my_pos, Vector & new_my_vel,
				    Angle & new_my_ang,
				    const bool do_random=false);

  /** this method can model the soccerserver if you apply a basic command
      dash and turn concern always the my_* parameters
   */
  static void model_cmd_main(const Vector & my_pos, 
			     const Vector & my_vel, 
			     const Angle & my_ang,
			     const int &old_my_stamina,
			     const Vector & ball_pos,
			     const Vector & ball_vel,
			     const Cmd_Main & cmd, 
			     Vector & new_my_pos,
			     Vector & new_my_vel,
			     Angle & new_my_ang,
			     int &new_my_stamina,
			     Vector & new_ball_pos,
			     Vector & new_ball_vel, const bool do_random = false);

  static void model_cmd_main(const Vector & my_pos, 
			     const Vector & my_vel, 
			     const Angle & my_ang,
			     const Vector & ball_pos,
			     const Vector & ball_vel,
			     const Cmd_Main & cmd, 
			     Vector & new_my_pos,
			     Vector & new_my_vel,
			     Angle & new_my_ang,
			     Vector & new_ball_pos,
			     Vector & new_ball_vel, const bool do_random = false);
  
  /* same as above, but using ANGLE instead of Angle */
  static void model_cmd_main(const Vector & my_pos, 
			     const Vector & my_vel, 
			     const ANGLE & my_ang,
			     const Vector & ball_pos,
			     const Vector & ball_vel,
			     const Cmd_Main & cmd, 
			     Vector & new_my_pos,
			     Vector & new_my_vel,
			     ANGLE & new_my_ang,
			     Vector & new_ball_pos,
			     Vector & new_ball_vel, const bool do_random = false);


  /* same as model_... as above, uses mystate*/
  static void get_successor_state(MyState const &state, Cmd_Main const &cmd, MyState &next_state,
				  const bool do_random = false);


  /* Berechnet den Schnittpunkt zweier Geraden */
  static Vector intersection_point(Vector p1, Vector steigung1, 
				   Vector p2, Vector steigung2);

  static Vector point_on_line(Vector steigung, Vector line_point, Value x);

  static bool Tools::intersection(const Vector & r_center, double size_x, double size_y,
				  const Vector & l_start, const Vector & l_end);

  //a triagle is defined as the convex hull of the three points t1,..,t3, which can be chosen arbitrary
  static bool point_in_triangle(const Vector & p, const Vector & t1, const Vector & t2, const Vector & t3);

  //a rectangle is defined as the convex hull of the four points r1,..,r4, which can be chosen arbitrary
  static bool point_in_rectangle(const Vector & p, const Vector & r1, const Vector & r2, const Vector & r3, const Vector & r4);

  //extra_margin can be negative to make the field smaller
  static bool point_in_field(const Vector & p, Value extra_margin= 0.0);

  /** it's a cached and lazy method to computed powers of ball_decay, (questions: ask art!)
      if the power is bigger then the max cache size, a warning will inform you about it.
   */
  static Value get_ball_decay_to_the_power(int power);

  static int min_int_greater_eqal( Value val ) {
    return int( ceil(val) );
  }
  
  static int max_int_smaller_equal( Value val ) {
    return int( floor(val) );
  }

  inline static Vector opponent_goalpos(){return Vector(ServerOptions::pitch_length/2.,0);};

  /* here comes stuff used by neck and view behaviors   */

  /* calculate ANGLE from NARROW, NORMAL or WIDE */
  static ANGLE get_view_angle_width(int view_ang);

  /* These use the blackboard to get their information. */
  static ANGLE cur_view_angle_width();
  static ANGLE next_view_angle_width();
  static int get_next_view_angle();
  static int get_next_view_quality();
  static long get_last_lowq_cycle();
  static Vector get_last_known_ball_pos();
  static void force_highq_view();
  
  static void set_neck_request(int req_type, Value param = 0, bool force = false);
  static void set_neck_request(int req_type, ANGLE param, bool force = false);
  static int get_neck_request(Value &param);    // returns NECK_REQ_NONE if not set
  static int get_neck_request(ANGLE &param);    // returns NECK_REQ_NONE if not set

  static bool is_ball_kickable_next_cycle(const Cmd &cmd, Vector & mypos,Vector & myvel, ANGLE &newmyang,
					  Vector & ballpos,Vector & ballvel);

  static bool is_ballpos_safe(const Vector &oppos, const ANGLE &opbodydir,
			      const Vector &ballpos, int bodydir_age);
  static bool is_ballpos_safe(const PPlayer opp,const Vector &ballpos,bool consider_tackles=false);
  static bool is_ballpos_safe(const WSpset &opps,const Vector &ballpos,bool cons_tackles=false);
  static bool is_ball_safe_and_kickable(const Vector &mypos, const Vector &oppos, const ANGLE &opbodydir,
					const Vector &ballpos, int bodydir_age);
  static bool is_ball_safe_and_kickable(const Vector &mypos, const PPlayer opp,const Vector &ballpos,
					bool consider_tackles=false);
  static bool is_ball_safe_and_kickable(const Vector &mypos, const WSpset &opps,const Vector &ball,
					bool consider_tackles=false);
  static bool is_position_in_pitch(Vector position,  const float safety_margin = 1.0 );

  static bool shall_I_wait_for_ball(const Vector ballpos, const Vector ballvel, int &steps);
  
  static Value speed_after_n_cycles(const int n, const Value dash_power_rate,
				    const Value effort, const Value decay);

  static bool is_pos_free(const Vector & pos);
  static Value eval_pos_wrt_position(const Vector & pos,const Vector & targetpos, const Value mindist2teammate = 2.0);
  static Value eval_pos(const Vector & pos,const Value mindist2teammate = 3.0);

  static Value evaluate_wrt_position(const Vector & pos,const Vector & targetpos);
  static Value get_closest_op_dist(const Vector pos);
  static Value get_closest_teammate_dist(const Vector pos);
  static Value get_optimal_position(Vector & result, Vector * testpos, 
				    const int num_testpos,const PPlayer &teammate);

  static PPlayer get_our_fastest_player2ball(Vector &intercept_pos, int & steps2go);
  static bool is_pos_occupied_by_ballholder(const Vector &pos);
  static int num_teammates_in_circle(const Vector pos, const Value radius);
  static Vector check_potential_pos(const Vector pos, const Value max_advance= 10);
  static Value Tools::evaluate_pos(const Vector query_pos);
  static Value Tools::evaluate_potential_of_pos(const Vector pos);
  static int compare_two_positions(Vector pos1, Vector pos2);
  static Value Tools::min_distance_to_border(const Vector position);
  static void display_direction(const Vector pos, const ANGLE dir, const Value length);
  static bool can_advance_behind_offsideline(const Vector pos);
  static int potential_to_score(Vector pos);

  static int compare_positions(const Vector pos1, const Vector pos2, Value & difference);

  static bool close2_goalline(const Vector pos);

  static bool is_pos1_better(const Vector pos1, const Vector pos2);

  static void simulate_player(const Vector & old_pos, const Vector & old_vel, 
			      const ANGLE & old_ang, const int &old_stamina, 
			      const Cmd_Main & cmd,
			      Vector & new_pos, Vector & new_vel,
			      ANGLE & new_ang, int &new_stamina,
			      const Value stamina_inc_max,
			      const Value inertia_moment,
			      const Value dash_power_rate, const Value effort, const Value decay);
};

extern std::ostream& operator<< (std::ostream& o, const MyState& s);

#endif
