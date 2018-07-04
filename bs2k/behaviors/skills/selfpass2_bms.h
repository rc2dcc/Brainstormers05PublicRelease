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

#ifndef _SELFPASS2_BMS_H_
#define _SELFPASS2_BMS_H_

#include "../base_bm.h"
#include "Vector.h"
#include "angle.h"
#include "cmd.h"
#include "basic_cmd_bms.h"
#include "oneortwo_step_kick_bms.h"
#include "options.h"
#include "ws_info.h"
#include "log_macros.h"


#define MAX_STEPS 15

class Selfpass2: public BaseBehavior {
  static bool initialized;
  BasicCmd *basic_cmd;
  OneOrTwoStepKick *onetwostepkick;
  

 private:
  typedef struct {
    int valid_at;
    Cmd cmd;
    Vector my_pos;
    Vector my_vel;
    ANGLE my_bodydir;
    bool I_have_ball;
    Vector ball_pos;
    Vector ball_vel;
    bool have_ball;
    Vector op_pos;
    int op_num;
    int op_steps2pos;
  } Simtable;

  Simtable simulation_table[MAX_STEPS];

  void simulate_my_movement(const ANGLE targetdir, const int max_steps, Simtable *simulation_table, 
			    const Vector mypos, const Vector myvel, const ANGLE myang,
			    const Value mystamina,
			    const Vector ballpos, const Vector ballvel,
			    const bool with_kick, const bool turn2dir_only = false);
  void get_cmd_to_go2dir(Cmd &tmp_cmd,const ANGLE targetdir,const Vector pos, const Vector vel, 
			 const ANGLE bodydir, const int stamina,
			 const Value inertia_moment = ServerOptions::inertia_moment, 
			 const Value stamina_inc_max = ServerOptions::stamina_inc_max);

  void get_cmd_to_go2pos(Cmd &tmp_cmd,const Vector targetpos,const Vector pos, const Vector vel, 
			 const ANGLE bodydir, const int stamina,
			 const PPlayer player);

  void simulate_ops_movement(Simtable *simulation_table, const bool target_is_ball= false);
  int get_min_cycles2_pos(const Vector targetpos, const PPlayer player, const int max_steps, Vector &resulting_pos);

  void print_table(Simtable *simulation_table);
  bool determine_kick(Simtable *simulation_table, const ANGLE targetdir, 
		      Vector & targetpos, Value & targetspeed, int & steps, Vector &attacking_op, 
		      int & attacking_number, 
		      const Vector mypos, const Vector myvel, const ANGLE myang,
		      const Value mystamina,
		      const Vector ballpos, const Vector ballvel,
		      const int reduce_dashes=0 );
  bool check_nokick_selfpass(Simtable *simulation_table, Vector & targetpos, int & steps, Vector &attacking_op,
			     int & attacking_number, const Vector ballpos);
  bool at_position(const Vector playerpos, const ANGLE bodydir, const Value kick_radius, const Vector targetpos);
  bool are_intermediate_ballpositions_safe(Vector ballpos, Vector ballvel, const int num_steps);
  void reset_simulation_table(Simtable *simulation_table);

 public:
  bool is_turn2dir_safe(const ANGLE targetdir, Value &speed, Vector &ipos, int &steps,
			Vector &attacking_op, int & op_number, const bool check_nokick_only = false, 
			const int max_dashes=10);
  // checks, if I can turn into target direction, with the ball lying in front of me


  bool is_turn2dir_safe(const ANGLE targetdir, Value &speed, Vector &ipos, int &steps,
			Vector &attacking_op, int & op_number, 
			const Vector mypos, const Vector myvel, const ANGLE myang,
			const Value mystamina,
			const Vector ballpos, const Vector ballvel,
			const bool check_nokick_only = false, 
			const int max_dashes=10);


  bool is_selfpass_safe(const ANGLE targetdir, Value &speed, Vector &ipos, int &steps,
			Vector &attacking_op, int & op_number, const bool check_nokick_only = false, 
			const int reduce_dashes=0);

  bool is_selfpass_safe(const ANGLE targetdir, Value &speed, Vector &ipos, int &steps,
			Vector &attacking_op, int & op_number, 
			const Vector mypos, const Vector myvel, const ANGLE myang,
			const Value mystamina,
			const Vector ballpos, const Vector ballvel,
			const bool check_nokick_only = false, 
			const int reduce_dashes=0);




  bool is_selfpass_safe(const ANGLE targetdir);
  bool is_selfpass_still_safe(const ANGLE targetdir, Value & kickspeed, int &op_num);

  bool get_cmd(Cmd &cmd);
  bool get_cmd(Cmd & cmd, const ANGLE targetdir, const Vector targetpos, const Value kickspeed);

  static bool init(char const * conf_file, int argc, char const* const* argv);
  Selfpass2();
  virtual ~Selfpass2();
};

#endif
