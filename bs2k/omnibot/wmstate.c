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

/*
 *  Author:   Artur Merke 
 */

#include "wmstate.h"
#include "options.h"
#include "geometry2d.h"

ostream& operator<< (ostream& o, const WMtime & wmtime) {
  return o << wmtime.time << "[," << wmtime.cycle <<"]";
}

void WMstate::import_msg_mdpstate(const Msg_mdpstate & msg) {
  //cerr << ">>>>>>>>>" << msg;
  time= msg.time;
  //cout << "\nmsg.time= " << msg.time << "  ,  time.time= " << time.time;

  play_mode= msg.play_mode;
  my_score= msg.my_score;
  his_score= msg.his_score;
  view_quality= msg.view_quality;
  view_width= msg.view_width;

  for (int i= 1; i < NUM_PLAYERS+1; i++) {
    my_team[i].alive= false;
    his_team[i].alive= false;
  }

  ball.time= time;
  //cout << "\nmsg.time= " << msg.time << "  ,  ball.time.time= " << ball.time.time;
  ball.pos= Vector(msg.ball.x,msg.ball.y);
  ball.vel= Vector(msg.ball.vel_x,msg.ball.vel_y);

  for (int i= 0; i< msg.players_num; i++) {
    const Msg_mdpstate::_mdp_player & p = msg.players[i];
    _wm_player * wm_t;
    if ( p.team == my_TEAM )
      wm_t= my_team;
    else
      wm_t= his_team;

    _wm_player & wm_p = wm_t[ p.number ];
    wm_p.time= time;
    wm_p.alive= true;
    wm_p.pos= Vector(p.x,p.y);
    wm_p.vel= Vector(p.vel_x,p.vel_y);
    wm_p.stamina= p.stamina;

    if ( p.team == my_TEAM && p.number == WM::my_number ) {
      my_angle=  p.angle;
      my_neck_angle= p.neck_angle;
      my_neck_angle_rel = my_neck_angle - my_angle;
      my_effort= p.effort;
      my_recovery= p.recovery;

      my_speed_value= wm_p.vel.norm();
      //my_speed_angle= my_neck_angle - ANGLE(wm_p.vel.arg()); // !!!!!!!!!!!! to be tested
    }
  }
}

void WMstate::compare_msg_mdpstate(const Msg_mdpstate & msg) const {
  LOG_ART(msg.time, << "- WMstate::compare_msg_mdpstate -------" );
  static const char *PlayModeString[] = PLAYMODE_STRINGS;
  Vector tmpVec, tmpVec2;
  ANGLE tmpANG;
  Value tmp;
  //cerr << ">>>>>>>>>" << msg;
  if (time.time != msg.time)
    LOG_ART(msg.time, << "- time= " << time.time << "  [" << msg.time << "]" );
  if (play_mode != msg.play_mode) 
    LOG_ART(msg.time, << "- play_mode= " << PlayModeString[play_mode] << "  [" << PlayModeString[msg.play_mode] << "]" );

  if (my_score !=  msg.my_score)
    LOG_ART(msg.time, << "- my_score= " << my_score << "  [" << msg.my_score << "]" );
  if (his_score !=  msg.his_score)
    LOG_ART(msg.time, << "- his_score= " << his_score << "  [" << msg.his_score << "]" );

  if ( view_quality != msg.view_quality ) 
    LOG_ART(msg.time, << "- view_quality= " << view_quality << "  [" << msg.view_quality << "]" );

  if ( view_width != msg.view_width ) 
    LOG_ART(msg.time, << "- view_width= " << view_width << "  [" << msg.view_width << "]" );

  Value eps= 0.1;

  //cout << "\nmsg.time= " << msg.time << "  ,  ball.time.time= " << ball.time.time;
  tmpVec= Vector(msg.ball.x,msg.ball.y);
  if ( tmpVec.distance(ball.pos) > eps) 
    LOG_ART(msg.time, << "- ball.pos= " << ball.pos << "  [" << tmpVec << "]" << " diff= " << tmpVec.distance(ball.pos) );

  tmpVec= Vector(msg.ball.vel_x,msg.ball.vel_y);
  if ( tmpVec.distance(ball.vel) > eps) 
    LOG_ART(msg.time, << "- ball.vel= " << ball.vel << "  [" << tmpVec << "]" << " diff= " << tmpVec.distance(ball.vel) );

  for (int i= 0; i< msg.players_num; i++) {
    const Msg_mdpstate::_mdp_player & p = msg.players[i];
    const _wm_player * wm_t;
    if ( p.team == my_TEAM )
      wm_t= my_team;
    else
      wm_t= his_team;

    const _wm_player & wm_p = wm_t[ p.number ];

    if ( p.team == my_TEAM && p.number == WM::my_number ) {
      tmpVec.init_polar(5,my_neck_angle.get_value());
      tmpVec2.init_polar(5,p.neck_angle);
      Vector tmpVec3,tmpVec4;
      tmpVec3.init_polar(1,my_angle.get_value());
      tmpVec4.init_polar(1,p.angle);
      //draw neck angle
      LOG_ART_2D(msg.time, 
	<< L2D( p.x,p.y,p.x+tmpVec2.x,p.y + tmpVec2.y,"black") 
	//<< L2D( p.x,p.y,p.x+tmpVec4.x,p.y + tmpVec4.y,"black") 
	<< C2D(p.x,p.y,1.3,"black") 
	<< L2D(wm_p.pos.x,wm_p.pos.y,wm_p.pos.x+tmpVec.x,wm_p.pos.y + tmpVec.y,"magenta") 
	//<< L2D(wm_p.pos.x,wm_p.pos.y,wm_p.pos.x+tmpVec3.x,wm_p.pos.y + tmpVec3.y,"magenta") 
	<< C2D(wm_p.pos.x,wm_p.pos.y,1.5,"magenta") 
	<< C2D(ball.pos.x,ball.pos.y,0.4,"magenta") 
	<< C2D(wm_p.pos.x + msg.ball.x - p.x,wm_p.pos.y + msg.ball.y - p.y, 0.3,"red") 
	);


      tmpVec= Vector( msg.ball.x, msg.ball.y) - Vector( p.x, p.y );
      Vector tmpVec2= ball.pos - wm_p.pos;
      if (tmpVec.norm() <= ServerOptions::player_size + ServerOptions::ball_size + 0.05)
	LOG_ART(msg.time, << " Collision?  " << tmpVec.norm() - ServerOptions::player_size - ServerOptions::ball_size << " -------- !!!!!!!!!!!!!!" ); 
      if ( tmpVec.distance(tmpVec2) > eps) 
	LOG_ART(msg.time, << "- rel ball.pos= " << tmpVec2 << "  [" << tmpVec << "]" << " diff= " << tmpVec.distance(tmpVec2) );

      tmpVec= Vector( p.x, p.y);
      if ( tmpVec.distance( wm_p.pos) > eps) 
	LOG_ART(msg.time, << "- my_pos= " << wm_p.pos << "  [" << tmpVec << "]" << " diff= " << tmpVec.distance(wm_p.pos) );

      tmpVec= Vector( p.vel_x, p.vel_y);
      if ( tmpVec.distance( wm_p.vel) > eps) 
	LOG_ART(msg.time, << "- my_vel= " << wm_p.vel << "  [" << tmpVec << "]" << " diff= " << tmpVec.distance(wm_p.vel) );

      tmpANG = my_angle -  p.angle;
      //LOG_ART(msg.time, << "-    my_angle = " << my_angle << "   p.angle= " << p.angle << "  diffANG= " << tmpANG << "   diffANG (-PI,PI) = " << tmpANG.get_value_mPI_pPI() );
      if ( fabs( tmpANG.get_value_mPI_pPI() ) > eps*0.1 )
	LOG_ART(msg.time, << "- my_angle= " << my_angle.get_value() << "  [" << p.angle << "]" << " diff_deg= " << tmpANG.get_value_mPI_pPI()*180.0/PI );

      tmpANG = my_neck_angle -  p.neck_angle;
      if ( fabs( tmpANG.get_value_mPI_pPI() ) > eps * 0.1 )
	LOG_ART(msg.time, << "- my_neck_angle= " << my_neck_angle.get_value() << "  [" << p.neck_angle << "]" << " diff_deg= " << tmpANG.get_value_mPI_pPI()*180.0/PI );

      tmpVec= Vector( p.vel_x, p.vel_y);
      tmp= tmpVec.norm();
      if ( fabs ( my_speed_value - tmp ) > eps )
	LOG_ART(msg.time, << "- my_speed_value= " << my_speed_value << "  [" << tmp << "]" << " diff " << fabs( my_speed_value - tmp) );

    }
  }
}

void WMstate::export_mdpstate(MDPstate & mdp) const {
  int KONV= 1; //KONV bewirkt Transformation von Koordinaten, falls der Spieler auf der rechten Seite spielt
  if (WM::my_side == right_SIDE) KONV= -1;

  /* vorlaeufig > */
  mdp.StateInfoString[0] = '\0'; // wird irgendwann verschwinden, da StateInfoString nur ein Hack von Martin ist
  /* < vorlaeufig */

  mdp.time_current= time.time;
  mdp.time_of_last_update= mdp.time_current;

  mdp.my_team_score= my_score;
  mdp.his_team_score= his_score;

  mdp.view_quality = view_quality;
  mdp.view_angle= view_width;

  mdp.ball.pos_x= FVal( KONV * ball.pos.x, 1.0, ball.time.time );
  mdp.ball.pos_y= FVal( KONV * ball.pos.y, 1.0, ball.time.time );
  mdp.ball.vel_x= FVal( KONV * ball.vel.x, 1.0, ball.time.time );
  mdp.ball.vel_y= FVal( KONV * ball.vel.y, 1.0, ball.time.time );

  for (int k= 0; k<2; k++) {
    FPlayer * mdp_t = mdp.my_team;
    const _wm_player * wm_t = my_team;
    int which_team= my_TEAM;
    if (k>0) {
      mdp_t= mdp.his_team;
      wm_t= his_team;
      which_team= his_TEAM;
    }

    for (int i= 1; i< NUM_PLAYERS+1; i++) {
      FPlayer & mdp_p = mdp_t[i-1];
      const _wm_player & wm_p = wm_t[i];
    
      mdp_p.alive= wm_p.alive;
      mdp_p.number= i;
      
      mdp_p.pos_x= FVal( KONV * wm_p.pos.x, 1.0, wm_p.time.time );
      mdp_p.pos_y= FVal( KONV * wm_p.pos.y, 1.0, wm_p.time.time );
      mdp_p.vel_x= FVal( KONV * wm_p.vel.x, 1.0, wm_p.time.time );
      mdp_p.vel_y= FVal( KONV * wm_p.vel.y, 1.0, wm_p.time.time );
      mdp_p.stamina = FVal( wm_p.stamina,1.0,wm_p.time.time );
      
      if ( WM::my_number == i && my_TEAM == which_team ) {
	mdp.me= &mdp_p;
	mdp_p.ang= FVal( my_angle.get_value() , 1.0 , time.time );
	mdp_p.neck_angle= FVal( my_neck_angle.get_value() , 1.0, time.time );
	if ( WM::my_side == right_SIDE ) { //Anpassung der Winkel fuer den rechten Spieler
	  mdp_p.ang.v+= PI; if ( mdp_p.ang.v > 2*PI) mdp_p.ang.v -= 2*PI;
	  mdp_p.neck_angle.v+= PI; if ( mdp_p.neck_angle.v > 2*PI) mdp_p.neck_angle.v -= 2*PI;
	}
	mdp_p.effort = FVal( my_effort , 1.0, time.time );
	mdp_p.recovery = FVal( my_recovery , 1.0, time.time );

      }
      else {
	mdp_p.ang= FVal( 0.0 , 0.0, -10 );
	mdp_p.neck_angle= FVal( 0.0 , 0.0, -10 );
	mdp_p.effort = FVal( 0.0,0.0,-10 ); //information not in 
	mdp_p.recovery = FVal( 0.0,0.0,-10 );    
      }
    }
  }
  
  if ( WM::my_side == left_SIDE ) 
    switch ( play_mode) {
    case PM_before_kick_off: mdp.play_mode= mdp.PM_my_BeforeKickOff; break;
    case PM_time_over:       mdp.play_mode= mdp.PM_TimeOver; break;
    case PM_play_on:         mdp.play_mode= mdp.PM_PlayOn; break;
    case PM_kick_off_l:      mdp.play_mode= mdp.PM_my_KickOff; break;
    case PM_kick_off_r:      mdp.play_mode= mdp.PM_his_KickOff; break;
    case PM_kick_in_l:       mdp.play_mode= mdp.PM_my_KickIn; break;
    case PM_kick_in_r:       mdp.play_mode= mdp.PM_his_KickIn; break;
    case PM_free_kick_l:     mdp.play_mode= mdp.PM_my_FreeKick; break;
    case PM_free_kick_r:     mdp.play_mode= mdp.PM_his_FreeKick; break;
    case PM_corner_kick_l:   mdp.play_mode= mdp.PM_my_CornerKick; break;
    case PM_corner_kick_r:   mdp.play_mode= mdp.PM_his_CornerKick; break;
    case PM_goal_kick_l:     mdp.play_mode= mdp.PM_my_GoalKick; break;
    case PM_goal_kick_r:     mdp.play_mode= mdp.PM_his_GoalKick; break;
    case PM_goal_l:          mdp.play_mode= mdp.PM_my_AfterGoal; break;
    case PM_goal_r:          mdp.play_mode= mdp.PM_his_AfterGoal; break;
    case PM_drop_ball:       mdp.play_mode= mdp.PM_Drop_Ball; break;
    case PM_offside_l:       mdp.play_mode= mdp.PM_his_OffSideKick; break;
    case PM_offside_r:       mdp.play_mode= mdp.PM_my_OffSideKick; break;
    case PM_goalie_catch_ball_l:  mdp.play_mode= mdp.PM_my_GoalieFreeKick; break;
    case PM_goalie_catch_ball_r:  mdp.play_mode= mdp.PM_his_GoalieFreeKick; break;
    default:                 mdp.play_mode=  mdp.PM_Unknown;
    }
  else
    switch ( play_mode) {
    case PM_before_kick_off: mdp.play_mode= mdp.PM_his_BeforeKickOff; break;
    case PM_time_over:       mdp.play_mode= mdp.PM_TimeOver; break;
    case PM_play_on:         mdp.play_mode= mdp.PM_PlayOn; break;
    case PM_kick_off_l:      mdp.play_mode= mdp.PM_his_KickOff; break;
    case PM_kick_off_r:      mdp.play_mode= mdp.PM_my_KickOff; break;
    case PM_kick_in_l:       mdp.play_mode= mdp.PM_his_KickIn; break;
    case PM_kick_in_r:       mdp.play_mode= mdp.PM_my_KickIn; break;
    case PM_free_kick_l:     mdp.play_mode= mdp.PM_his_FreeKick; break;
    case PM_free_kick_r:     mdp.play_mode= mdp.PM_my_FreeKick; break;
    case PM_corner_kick_l:   mdp.play_mode= mdp.PM_his_CornerKick; break;
    case PM_corner_kick_r:   mdp.play_mode= mdp.PM_my_CornerKick; break;
    case PM_goal_kick_l:     mdp.play_mode= mdp.PM_his_GoalKick; break;
    case PM_goal_kick_r:     mdp.play_mode= mdp.PM_my_GoalKick; break;
    case PM_goal_l:          mdp.play_mode= mdp.PM_his_AfterGoal; break;
    case PM_goal_r:          mdp.play_mode= mdp.PM_my_AfterGoal; break;
    case PM_drop_ball:       mdp.play_mode= mdp.PM_Drop_Ball; break;
    case PM_offside_l:       mdp.play_mode= mdp.PM_my_OffSideKick; break;
    case PM_offside_r:       mdp.play_mode= mdp.PM_his_OffSideKick; break;
    case PM_goalie_catch_ball_l:  mdp.play_mode= mdp.PM_his_GoalieFreeKick; break;
    case PM_goalie_catch_ball_r:  mdp.play_mode= mdp.PM_my_GoalieFreeKick; break;
    default:                 mdp.play_mode= mdp.PM_Unknown;
    }
};


void WMstate::incorporate_cmd_and_msg_sense_body(const Cmd & cmd, const Msg_sense_body & sb) {
  Value tmp, tmp2;
  Vector tmpVec;
  ANGLE ang;
  _wm_player & me = my_team[WM::my_number];

  if ( cmd.cmd_main.is_cmd_set() ) {
    const Cmd_Main & cmd_main= cmd.cmd_main;
    switch ( cmd_main.get_type()) {
      case cmd_main.TYPE_MOVETO   :   
	//cmd_main.get_moveto(par1,par2);
	//o_main << "(move " << x_LP_2_SRV( par1 ) << " " << y_LP_2_SRV( par2 ) << ")";
	break;
      case cmd_main.TYPE_TURN 	 :   
	if (turn_count + 1 == sb.turn_count) {
	  cmd_main.get_turn(tmp);
	  ang= tmp;
	  my_angle += ANGLE( ang.get_value_mPI_pPI()/(1.0 + ServerOptions::inertia_moment * my_speed_value ));
	}
	else
	  cerr << "\n server missed a turn" 
	       << ",  player= " << WM::my_number 
	       << ",  time= [" << sb.time -1 << "," << sb.time << "]";
	break;  
      case cmd_main.TYPE_DASH 	 :   
	if (dash_count + 1 == sb.dash_count) {
	  cmd_main.get_dash(tmp);
	  if (tmp > ServerOptions::maxpower) tmp= ServerOptions::maxpower;
	  if (tmp < ServerOptions::minpower) tmp= ServerOptions::minpower;
	  if (tmp> 0 && tmp> me.stamina) tmp= me.stamina;
	  if (tmp< 0 && -2*tmp > me.stamina) tmp= -me.stamina;
	  tmp *= my_effort;
	  tmp *= ServerOptions::dash_power_rate;
	  tmpVec.init_polar(tmp, my_angle.get_value() );
	  me.vel += tmpVec;
	  tmp= me.vel.norm();
	  if (tmp > ServerOptions::player_speed_max)
	    me.vel.normalize(ServerOptions::player_speed_max);
	}
	else
	  cerr << "\n server missed a dash" 
	       << ",  player= " << WM::my_number 
	       << ",  time= [" << sb.time -1 << "," << sb.time << "]";
	break;
      case cmd_main.TYPE_KICK 	 :   
	if (kick_count + 1 == sb.kick_count) {	  
	  //cout << "\nKICK KICK KICK KICK KICK KICK";
	  cmd_main.get_kick(tmp,tmp2);
	  if (tmp > ServerOptions::maxpower) tmp= ServerOptions::maxpower;
	  if (tmp < ServerOptions::minpower) tmp= ServerOptions::minpower;
	  if ( me.pos.distance( ball.pos ) > ServerOptions::kickable_area )
	    break;

	  ang=  ANGLE( ball.pos.arg() ) - my_angle;
	  
	  tmp *= ServerOptions::kick_power_rate *
	    (
	     1.0 - 0.25 * fabs( ang.get_value_mPI_pPI() )/PI 
	         - 0.25 * (me.pos.distance(ball.pos) - ServerOptions::player_size - ServerOptions::ball_size)/ ServerOptions::kickable_margin
	    );

	  ang= my_angle + ANGLE(tmp2);
	  tmpVec.init_polar( tmp, ang.get_value() );
	  ball.vel += tmpVec;
	  tmp= ball.vel.norm();
	  if (tmp > ServerOptions::ball_speed_max)
	    ball.vel.normalize(ServerOptions::ball_speed_max);
	}
	else
	  cerr << "\n server missed a kick" 
	       << ",  player= " << WM::my_number 
	       << ",  time= [" << sb.time -1 << "," << sb.time << "]";
	break;
      case cmd_main.TYPE_CATCH    :   
	break;    
    default: 
      cerr << "\nwrong command";
    }    
  }

#if 0
  if ( cmd.cmd_neck.is_cmd_set() ) {
    const Cmd_Neck & cmd_neck= cmd.cmd_neck;
    cmd_neck.get_turn(tmp);
    ang= tmp;

    if ( ang.get_value_mPI_pPI() > DEG2RAD( ServerOptions::maxneckmoment) )
      ang= DEG2RAD( ServerOptions::maxneckmoment);
    else  if ( ang.get_value_mPI_pPI() < DEG2RAD( ServerOptions::minneckmoment) )
      ang= DEG2RAD( ServerOptions::minneckmoment);

    my_neck_angle += ang;

    ang = my_neck_angle - my_angle;
    if ( ang.get_value_mPI_pPI() > DEG2RAD(ServerOptions::maxneckang) )
      my_neck_angle= my_angle + ANGLE( DEG2RAD(ServerOptions::maxneckang) );
    else
    if ( ang.get_value_mPI_pPI() < DEG2RAD(ServerOptions::minneckang) )
      my_neck_angle= my_angle - ANGLE( DEG2RAD(ServerOptions::minneckang) );
  }
#endif


  me.pos += me.vel;
  me.vel *= ServerOptions::player_decay;

  ball.pos += ball.vel;
  ball.vel *= ServerOptions::ball_decay;

  if (time.time + 1 == sb.time) 
    time= sb.time;
  else if (time.time == sb.time)
    time.cycle++;
  else {
    cerr << "\n Last wmstate.time= " << time << " , sense_body.time= " << sb.time;
    time= sb.time;
  }

  my_neck_angle= my_angle + ANGLE( - DEG2RAD( sb.neck_angle) );
  my_speed_value= sb.speed_value;
  my_speed_angle= sb.speed_angle;
  me.vel.normalize(my_speed_value); // !!!
  my_neck_angle_rel= ANGLE( - DEG2RAD( sb.neck_angle) );
  kick_count= sb.kick_count;
  dash_count= sb.dash_count;
  turn_count= sb.turn_count;
  say_count=  sb.say_count;
  turn_neck_count= sb.turn_neck_count;
}

void WMstate::compute_pos_from_markers(Vector & pos, Vector & vel, ANGLE & n_ang, 
				       const Msg_see & see) const {
  Vector tmpVec;
  const _wm_player & me = my_team[ WM::my_number ];

  pos= me.pos; 
  vel= me.vel;
  n_ang= my_neck_angle;

  int best_five[5];
  bool can_be_taken[ see.markers_MAX ];
  int best_num= 0;
  for (int i=0; i< see.markers_num; i++) {
    if ( see.markers[i].see_position ) {
      can_be_taken[i]= true;
      if (best_num < 5) { //initial match
	best_five[best_num]= i;
	best_num++;
      }
    }
    else 
      can_be_taken[i]= false;
  }

  if (see.markers_num < best_num)
    best_num= see.markers_num;
  LOG_ART(time.time, << " #Markers= " << best_num );
    
  
  //select the neares five markers
  for (int i= 0; i < best_num; i++) {
    for (int j= 0; j < see.markers_num; j++) 
      if ( can_be_taken[j] && (see.markers[j].dist <  see.markers[ best_five[i] ].dist || !can_be_taken[ best_five[i] ] ) )
	best_five[i]= j;
    can_be_taken[ best_five[i] ] = false;
  }

  for (int i=0; i< best_num; i++) 
    LOG_ART(time.time, << " Best (" << best_five[i] << ") " << see.markers[ best_five[i] ].x << " " << see.markers[best_five[i]].y 
		       << " " << see.markers[best_five[i]].dist << " " << see.markers[best_five[i]].dir );

  //intersect the circles around 2 nearest markers
  if (best_num > 1) {
    const Msg_see::_see_marker & m1 = see.markers[ best_five[0] ];
    const Msg_see::_see_marker & m2 = see.markers[ best_five[1] ];
    //const Msg_see::_see_marker & m2 = see.markers[ best_five[best_num-1] ];
    
    if  (best_five[0] == best_five[1])
      LOG_ART(time.time, << " The markers coincide" );

    Circle2d c1( Point2d(m1.x, m1.y), m1.dist);
    Circle2d c2( Point2d(m2.x, m2.y), m2.dist);

    Point2d p1,p2;

    int res= Geometry2d::intersect_circle_with_circle(p1,p2,c1,c2);
    if (res == 1) {
      Vector v1= Vector(p1.x,p1.y);
      Vector v2= Vector(p2.x,p2.y);
      if ( pos.distance(v1) < pos.distance(v2) )
	pos= v1;
      else
	pos= v2;
    }
    else { //there was no intersection
      LOG_ART(time.time, << " There was no intersection " );
      Vector v1= Vector(m1.x, m1.y);
      Vector v2= Vector(m2.x, m2.y);
      pos= v1 + m1.dist * ( v2-v1 );
    }
    
    //compute now my_neck_angle
    ANGLE dir =  ANGLE(  DEG2RAD(m1.dir) );

    tmpVec = Vector(m1.x,m1.y) - pos;
    n_ang= ANGLE( tmpVec.arg() ) + dir;
    tmpVec.init_polar(10,my_neck_angle.get_value());
#if 1
    LOG_ART_2DD(time.time, "--" ,
      << C2D(m1.x,m1.y,m1.dist, "gray" ) 
      << C2D(m2.x,m2.y,m2.dist, "gray" )
      //<< L2D(pos.x, pos.y, m1.x, m1.y,"blue") 
      << C2D(pos.x,pos.y,1.7, "pink" ) 
      << L2D(pos.x,pos.y,pos.x+tmpVec.x,pos.y + tmpVec.y," pink") 
      );

    if (best_num>2)
      LOG_ART_2DD(time.time, "---" , << C2D(see.markers[ best_five[2] ].x,see.markers[ best_five[2] ].y,see.markers[ best_five[2] ].dist , "gray30" ) );

    if (best_num>3)
      LOG_ART_2DD(time.time, "---" , << C2D(see.markers[ best_five[3] ].x,see.markers[ best_five[3] ].y,see.markers[ best_five[3] ].dist , "gray30" ) );
#endif
    LOG_ART(time.time, << " m1.dir= " << m1.dir );
    LOG_ART(time.time, << " DEG2RAD(m1.dir)= " << DEG2RAD(m1.dir) );

    //computer now my velocity
    if (m1.how_many>= 4) {
      
    }
    else
      LOG_ART(time.time, << " Nearest marker has no distchg and dirchng information" );
  }
  else {
    LOG_ART(time.time, << " I see just " << see.markers_num << " markers" );
  }

}

void WMstate::incorporate_msg_see(const Msg_see & see) {
  if (time.time != see.time)
    cout << "\n Current wmstate.time= " << time << " , see.time= " << see.time;

  _wm_player & me = my_team[ WM::my_number ];
  Vector tmpVec;

  //Vector pos,vel; ANGLE n_ang;

  compute_pos_from_markers2(me.pos,me.vel,my_neck_angle, see);

  //compute now my_angle
  my_angle= my_neck_angle - my_neck_angle_rel;

  
  //compute now ball pos and velocity
  if (see.ball_upd) {
    ANGLE dir=  DEG2RAD( - see.ball.dir );
    dir += my_neck_angle;
    tmpVec.init_polar( see.ball.dist, dir.get_value() );
    ball.pos= me.pos + tmpVec;
    //LOG_ART_2D(time.time, << C2D(ball.pos.x,ball.pos.y,1,"black") );
    //ball.pos

  }
};

//berechnet die aktuelle Position mit Hilfe einer gewichteten Summe
void WMstate::compute_pos_from_markers2(Vector & pos, Vector & vel, ANGLE & n_ang, 
				       const Msg_see & see) const {

  Vector m1;
  Vector m2;
  Vector m3;
  Vector m4;
  Angle dir1, dir2, dir3, dir4;
  Vector a;
  Vector b;
  Angle alpha, beta;
  Value c1, c2, c3, c4;
  Value l1, l2;
  Vector y;

  int cnt = 0;
  for (int i=0; i< see.markers_num; i++) {
    if ( see.markers[i].see_position ) {
      if(cnt==0){
        m1 = Vector(see.markers[i].x, see.markers[i].y);
        dir1 = see.markers[i].dir;
      }
      if(cnt==1){
        m2 = Vector(see.markers[i].x, see.markers[i].y);
        dir2 = see.markers[i].dir;
      }
      if(cnt==2){
        m3 = Vector(see.markers[i].x, see.markers[i].y);
        dir3 = see.markers[i].dir;
      }
      if(cnt==3){
        m4 = Vector(see.markers[i].x, see.markers[i].y);
        dir4 = see.markers[i].dir;
      }
      if(cnt>3) break;
      cnt++;
    }
  }

  if (cnt<4){
    cout << "\nKann Position nicht bestimmen!";
    return;
  }

  LOG_ART(time.time, "Marker1: " << m1 << ", Winkel: " << dir1);
  LOG_ART(time.time, "Marker2: " << m2 << ", Winkel: " << dir2);
  LOG_ART(time.time, "Marker3: " << m3 << ", Winkel: " << dir3);
  LOG_ART(time.time, "Marker4: " << m4 << ", Winkel: " << dir4);

  a = m2 - m1;
  b = m3 - m1;
  //  LOG_ART(time.time, "A: " << a);
  //  LOG_ART(time.time, "B: " << b);
  alpha = (dir1-dir2)/180.0 * PI;
  beta  = (dir1-dir3)/180.0 * PI;
  //  LOG_ART(time.time, "Alpha: " << alpha << ", Beta: " << beta);

  c1 = b.x*cos(alpha) - b.y*sin(alpha);
  c2 = a.x*sin(beta)  + a.y*cos(beta);
  c3 = b.x*sin(alpha) + b.y*cos(alpha);
  c4 = a.x*cos(beta)  - a.y*sin(beta);
  //  LOG_ART(time.time, "C1: " << c1);
  //  LOG_ART(time.time, "C2: " << c2);
  //  LOG_ART(time.time, "C3: " << c3);
  //  LOG_ART(time.time, "C4: " << c4);

  l1 = (-a.x*c2 + a.y*c4 - b.y*c4 + b.x*c2) / (c1*c2 - c3*c4);
  l2 = (l1*c1-b.x+a.x) / c4;
  //  LOG_ART(time.time, "L1: " << l1);
  //  LOG_ART(time.time, "L2: " << l2);

  y.y = (a.y*(1-l1*cos(alpha)) + a.x*l1*sin(alpha)) / (l1*cos(alpha)*(1-l1*cos(alpha))-(1-l1*cos(alpha))-l1*l1*sin(alpha)*sin(alpha));
  y.x = (a.y+y.y-y.y*l1*cos(alpha))/(l1*sin(alpha));
  //  LOG_ART(time.time, "Y: " << y << " Länge: " << y.norm());
  pos = m1 - y;

  LOG_ART(time.time, "Position: " << pos);

  a = m3 - m2;
  b = m4 - m2;
  alpha = (dir2-dir3)/180.0 * PI;
  beta  = (dir2-dir4)/180.0 * PI;

  c1 = b.x*cos(alpha) - b.y*sin(alpha);
  c2 = a.x*sin(beta)  + a.y*cos(beta);
  c3 = b.x*sin(alpha) + b.y*cos(alpha);
  c4 = a.x*cos(beta)  - a.y*sin(beta);

  l1 = (-a.x*c2 + a.y*c4 - b.y*c4 + b.x*c2) / (c1*c2 - c3*c4);
  l2 = (l1*c1-b.x+a.x) / c4;

  y.y = (a.y*(1-l1*cos(alpha)) + a.x*l1*sin(alpha)) / (l1*cos(alpha)*(1-l1*cos(alpha))-(1-l1*cos(alpha))-l1*l1*sin(alpha)*sin(alpha));
  y.x = (a.y+y.y-y.y*l1*cos(alpha))/(l1*sin(alpha));
  pos = m2 - y;

  LOG_ART(time.time, "Position: " << pos);


}
