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

#include "line_up_bmp.h"
//#include "mdp_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "valueparser.h"
#include "options.h"
#include "../policy/positioning.h"
#include "../basics/tools.h"

extern Cmd last_cmd;
bool LineUp::initialized = false;

LineUp::LineUp() {
  //char key_kick_off[50];

  // open config file
  //ValueParser parser( CommandLineOptions::policy_conf, "Line_Up_Policy" );
  //parser.set_verbose(true);

  /*
  // read home positions
  for(int i=1;i<=11;i++){
    sprintf(key_kick_off, "home_kick_off_%d",i);

    parser.get(key_kick_off, home_kick_off[i], 2);
  }
  */

  basic_cmd = new BasicCmd();
  go2pos = new NeuroGo2Pos();
  face_ball = new FaceBall();

  ivSectorAngle = ANGLE(0);

}

LineUp::~LineUp() {
  delete basic_cmd;
  delete go2pos;
  delete face_ball;
}

bool LineUp::get_cmd(Cmd &cmd){
  
  if ( !ClientOptions::consider_goalie ) {
    /* Daniel
    int temp_matching = DeltaPositioning::get_matching();
    DeltaPositioning::select_matching(0);
    DeltaPositioning::select_formation(1);
    DeltaPositioning::update();
    */
    DashPosition my_form = DeltaPositioning::get_position(WSinfo::me->number);
    if (   my_form.x > 0.0
        || my_form.x + WSinfo::me->vel.x > 0.0) my_form.x = -1.0; //added by TG
    if (   last_cmd.cmd_main.get_type() == last_cmd.cmd_main.TYPE_DASH
        && fabs(WSinfo::me->ang.get_value_mPI_pPI()) < PI*(60.0/180.0) )
      my_form.x = -2.0;
    //D DeltaPositioning::select_matching(temp_matching);
    if ( (WSinfo::me->pos - my_form).sqr_norm() < 1.0 ) {
      
      face_ball->turn_to_ball();
      return face_ball->get_cmd(cmd);
    } else {
      basic_cmd->set_move(my_form);
      return basic_cmd->get_cmd(cmd);
    }
    //return Move_Factory::get_Move(my_form.x, my_form.y);
  }
  //line up goalie 
  Vector home_pos = Vector(-50.5, 0.0);//Vector(home_kick_off[WSinfo::me->number][0], home_kick_off[WSinfo::me->number][1]);
  Vector pos;
  pos.x = WSinfo::me->pos.x;
  pos.y = WSinfo::me->pos.y;

  double sqr_target_accuracy = 1.0;

  if ( !WSinfo::is_my_pos_valid() ) {
    double moment = DEG2RAD(40.0);//mdpInfo::view_angle_width_rad();
    INFO_OUT << "\nPlayer_no= " << ClientOptions::player_no << " my pos is not VALID";
    basic_cmd->set_turn(moment);
    return basic_cmd->get_cmd(cmd);
  }
  
  if( (pos - home_pos).sqr_norm() <= sqr_target_accuracy ) {
    face_ball->turn_to_ball();
    return face_ball->get_cmd(cmd);
    //return Move_Factory::get_Look_Around();
  }
  else { 
    switch (WSinfo::ws->play_mode) {
    case PM_my_BeforeKickOff:  
    case PM_his_BeforeKickOff: 
    case PM_my_AfterGoal:	   
    case PM_his_AfterGoal:	   
    case PM_Half_Time:
      basic_cmd->set_move(home_pos);
      return basic_cmd->get_cmd(cmd);
      //return Move_Factory::get_Move(home_pos.x, home_pos.y);
      break;
    default:
      go2pos->set_target(home_pos, 0.7, 0);
      return go2pos->get_cmd(cmd);
      //return Move_Factory::get_Neuro_Go2Pos_stamina(home_pos.x, home_pos.y, 20, 10, 20, 5);
    }
  }
  //return Move_Factory::get_Main_Move(); //never reaches this line
}

bool LineUp::get_cmd(Cmd &cmd, bool sectorBased)
{
  DashPosition my_form = DeltaPositioning::get_position(WSinfo::me->number);
  if (my_form.x > 0.0) my_form.x = -0.5; //added by TG
  if (    (WSinfo::me->pos - my_form).sqr_norm() > 1.0 
       || WSinfo::ball->age > 2 ) 
    return get_cmd(cmd);
  if (!sectorBased)
    return get_cmd(cmd);
  ANGLE nextWidth = Tools::next_view_angle_width();
  if (WSinfo::ws->time_of_last_update == WSinfo::ws->time)
    ivSectorAngle += nextWidth;
  ANGLE turnAngle = ivSectorAngle - WSinfo::me->ang;
  basic_cmd->set_turn_inertia( turnAngle.get_value_0_p2PI() );
  return basic_cmd->get_cmd(cmd);
}

