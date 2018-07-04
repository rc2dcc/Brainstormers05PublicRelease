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

#ifndef _NEURO_WBALL_H_
#define _NEURO_WBALL_H_

#include "../policy/abstract_mdp.h"
#include "intention.h"
#include "skills/selfpass_bms.h"
#include "skills/dribble_straight_bms.h"

class NeuroWball {

  Selfpass *selfpass;
  DribbleStraight *dribblestraight;

 private:

  //NetDescription P1net;

  bool is_better( const float V,const float Vbest, const AState & state, const AAction *actions, const int a, const int abest );

  void refine_laufpass( const AState state, AAction &action, const float Vaction );

  void print_action_data( const AState current_state, const AAction action, const int idx, const float V, const float display_value, const int log_level=2 );

  float do_evaluation( const AState current_state, const AAction action );

  float select_best_aaction( const AState current_state, const AAction *action_set, int action_set_size, AAction & best_action );

  void generate_safe_passes( const AState & state, AAction *actions, int & num_actions );

  void generate_risky_passes( const AState & state, AAction *actions, int & num_actions );

  void generate_passes( const AState & state, AAction *actions, int & num_actions, const int save_time );

  void generate_laufpasses( const AState & state, AAction *actions, int & num_actions, const int save_time );

  void generate_penaltyarea_passes(AAction *actions, int &num_actions);

  int generate_action_set( const AState & state, AAction *actions );

  bool selfpass_dir_ok( const AState & state, const ANGLE dir );

  bool check_selfpass( const AState & state, const ANGLE targetdir, Value &ballspeed, Vector &target, int &steps, Vector &op_pos, int &op_num );

  void generate_selfpasses( const AState & state, AAction *actions,int &num_actions);

  bool is_dribblestraight_possible( const AState & state );


  // new (everything that is needed for new version)
  Vector compute_potential_pos(const AState current_state,const AAction action);
  bool select_best_aaction2(const AState current_state,  AAction *action_set, int action_set_size, AAction &best_action);
  void check_to_improve_action(AAction &action);
  void check_for_best(const AAction *actions, const int idx, int & best_safe,  int & best_risky );
  bool is_safe(const AAction action);

 public:

  Value exploration;

  int exploration_mode;
  int evaluation_mode;

  NeuroWball();
  
  virtual ~NeuroWball();


  bool evaluate_passes_and_dribblings( AAction & best_aaction, AState & current_state );

  Value adjust_speed( const Vector ballpos, const Value dir, const Value speed );


  // new for OSAKA (everything that is needed for new version):
  bool evaluate_passes( AAction & best_aaction, AState & current_state );


};

#endif // _NEURO_WBALL_H_
