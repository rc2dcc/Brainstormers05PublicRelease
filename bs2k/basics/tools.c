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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "ws_info.h"
#include "tools.h"
#include "options.h"
#include "log_macros.h"
#include "server_options.h"
#include "macro_msg.h"
#include "blackboard.h"
#include "mystate.h"
#include "geometry2d.h"
#include "policy_tools.h"

#if 1
#define DBLOG_POL(LLL,XXX) LOG_POL(LLL,XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D<<XXX)
#else
#define DBLOG_DRAW(LLL,XXX)
#define DBLOG_POL(LLL,XXX) 
#endif

#define MIN(X,Y) ((X<Y)?X:Y)

int Tools::num_powers= -1;
Value Tools::ball_decay_powers[MAX_NUM_POWERS];


std::ostream& operator<< (std::ostream& o, const MyState& s) 
{
  return o <<s.my_pos << s.my_vel << s.ball_pos<<s.ball_vel<<" "
	   <<RAD2DEG(s.my_angle.get_value())<<" "<<s.my_pos.distance(s.ball_pos);
}
Angle Tools::get_abs_angle(Angle angle){
  Angle result;
  result = get_angle_between_null_2PI(angle);
  if(result>PI)
    result -= 2*PI;
  return(fabs(result));
}

ANGLE Tools::my_angle_to(Vector target){
  ANGLE result;
  target -= WSinfo::me->pos;
  result = target.ARG() - WSinfo::me->ang;
  return result;
}

ANGLE Tools::my_abs_angle_to(Vector target) {
  ANGLE result;
  result = my_angle_to(target);
  return result + WSinfo::me->ang;
}

ANGLE Tools::my_neck_angle_to(Vector target) {
  ANGLE result;
  target -= WSinfo::me->pos;
  result = target.ARG() - WSinfo::me->neck_ang;
  return result;
}

/** returns expected relative body turn angle */
ANGLE Tools::my_expected_turn_angle() {
  if(!WSinfo::current_cmd->cmd_main.is_cmd_set()) return ANGLE(0);
  if(WSinfo::current_cmd->cmd_main.get_type() != Cmd_Main::TYPE_TURN) return ANGLE(0);
  Value turnangle=0;
  WSinfo::current_cmd->cmd_main.get_turn(turnangle);
  //cout << "turnangle: " << turnangle;
  if(turnangle>PI) turnangle-=2*PI; 
  Value newangle=turnangle/(1.0+WSinfo::me->inertia_moment*WSinfo::me->vel.norm());
  return ANGLE(newangle);
}

Value Tools::moment_to_turn_neck_to_abs(ANGLE abs_dir_ang) {

  Value abs_dir = abs_dir_ang.get_value();
  Angle minang=(my_minimum_abs_angle_to_see()+ANGLE(.5*next_view_angle_width().get_value())).get_value();
  Angle maxang=(my_maximum_abs_angle_to_see()-ANGLE(.5*next_view_angle_width().get_value())).get_value();
  if(minang<maxang) {
    if(abs_dir<minang) abs_dir=minang;
    if(abs_dir>maxang) abs_dir=maxang;
  } else {
    if(abs_dir>maxang && abs_dir<(minang-maxang)/2.+maxang) abs_dir=maxang;
    else if(abs_dir<minang && abs_dir>maxang) abs_dir=minang;
  }      
  return Tools::get_angle_between_null_2PI(abs_dir-WSinfo::me->neck_ang.get_value()
					   -my_expected_turn_angle().get_value());
}

/** gets the maximum angle I could see (abs, right) */
ANGLE Tools::my_maximum_abs_angle_to_see() {
  ANGLE max_angle=WSinfo::me->ang+my_expected_turn_angle();
  max_angle+=ServerOptions::maxneckang+ANGLE(.5*next_view_angle_width().get_value());
  return max_angle;
}

/** gets the minimum angle I could see (abs, left) */
ANGLE Tools::my_minimum_abs_angle_to_see() {
  ANGLE min_angle=WSinfo::me->ang+my_expected_turn_angle();
  min_angle+=ServerOptions::minneckang-ANGLE(.5*next_view_angle_width().get_value());
  return min_angle;
}

/** returns true if abs direction can be in view_angle with appropriate neck turn */
bool Tools::could_see_in_direction(ANGLE target_ang) {
  Angle minang=my_minimum_abs_angle_to_see().get_value();
  Angle maxang=my_maximum_abs_angle_to_see().get_value();
  Angle target=target_ang.get_value();
  if(minang>maxang && (target<minang && target>maxang)) return false;
  if(minang<maxang && (target<minang || target>maxang)) return false;
  return true;
}

bool Tools::could_see_in_direction(Angle target_ang) {
  return could_see_in_direction(ANGLE(target_ang));
}

Value Tools::get_tackle_success_probability(Vector my_pos, Vector ball_pos, Value my_angle) {
  Value ret;
  Vector player_2_ball = ball_pos - my_pos;
  player_2_ball.rotate(-my_angle);
  if (player_2_ball.x >= 0.0) {
    ret = pow(player_2_ball.x/ServerOptions::tackle_dist, ServerOptions::tackle_exponent) + 
      pow(fabs(player_2_ball.y)/ServerOptions::tackle_width, ServerOptions::tackle_exponent);
  } else {
    ret = pow(player_2_ball.x/ServerOptions::tackle_back_dist, ServerOptions::tackle_exponent) + 
      pow(fabs(player_2_ball.y)/ServerOptions::tackle_width, ServerOptions::tackle_exponent);
  }
#if 0
  LOG_POL(0,<<"Tools: tackle ret: "<<ret<<" tackle dist "<<ServerOptions::tackle_dist 
	  <<" tackle exp "<<ServerOptions::tackle_exponent 
	  <<" tackle width "<<ServerOptions::tackle_width 
	  <<" tackle back dist "<<ServerOptions::tackle_back_dist 
	  );

#endif

  if (ret >= 1.0) return 0.0;
  else if (ret < 0.0) return 1.0;
  else return (1.0 - ret);
}

Angle Tools::get_angle_between_mPI_pPI(Angle angle) {
  while (angle >= PI) angle -= 2*PI;
  while (angle < -PI) angle += 2*PI;
  return angle;
}

Angle Tools::get_angle_between_null_2PI(Angle angle) {
  while (angle >= 2*PI) angle -= 2*PI;
  while (angle < 0) angle += 2*PI;
  return angle;
}

Value Tools::max(Value a, Value b) {
  if (a > b) return a;
  else return b;
}

Value Tools::min(Value a, Value b) {
  if (a < b) return a;
  else return b;
}
int Tools::max(int a, int b) {
  if (a > b) return a;
  else return b;
}

int Tools::min(int a, int b) {
  if (a < b) return a;
  else return b;
}

int Tools::int_random(int n)
{
  static bool FirstTime = true;
  
  if ( FirstTime ){
    /* initialize the random number seed. */
    timeval tp;
    gettimeofday( &tp, NULL );
    srandom( (unsigned int) tp.tv_usec );
    FirstTime = false;
  }

  if ( n > 2 )
    return( random() % n );
  else if ( n == 2 )
    return( ( (random() % 112) >= 56 ) ? 0 : 1 );
  else if ( n == 1 )
    return(0);
  else
  {
    printf("int_random(%d) ?\n",n);
    printf( "You called int_random(<=0)\n" );
    return(0);
  }
}

Value Tools::range_random(Value lo, Value hi)
{
  int x1 = int_random(10000);
  int x2 = int_random(10000);
  float r = (((float) x1) + 10000.0 * ((float) x2))/(10000.0 * 10000.0);
  return( lo + (hi - lo) * r );
}

int Tools::very_random_int(int n)
{
  int result = (int) range_random(0.0,(float)n);  /* rounds down */
  if ( result == n ) result = n-1;
  return(result);
}

Vector Tools::get_Lotfuss(Vector x1, Vector x2, Vector p){
  Vector r = x1 - x2;
  Vector a = x1 - p;
  Value l = -(a.x*r.x+a.y*r.y)/(r.x*r.x+r.y*r.y);
  Vector ergebnis = x1 + l*r;
  return ergebnis;
}

Value Tools::get_dist2_line(Vector x1, Vector x2, Vector p){
  //return get_Lotfuss(x1,x2,p).norm(); //this was wrong: corrected in 06/05
  return ( get_Lotfuss(x1,x2,p) - p ).norm();
}

long Tools::get_current_ms_time() { //returns time in ms since first call to this routine
  timeval tval;
  static long s_time_at_start= 0;
  if (gettimeofday(&tval,NULL))
    std::cerr << "\n something wrong with time mesurement";

  if ( 0 == s_time_at_start ) 
    s_time_at_start= tval.tv_sec;

  return (tval.tv_sec - s_time_at_start) * 1000 + tval.tv_usec / 1000;
}

void Tools::model_cmd_main(const Vector & old_my_pos, const Vector & old_my_vel, 
			   const ANGLE & old_my_ang,
			   const Vector & old_ball_pos, const Vector & old_ball_vel,
			   const Cmd_Main & cmd,
			   Vector & new_my_pos, Vector & new_my_vel,
			   ANGLE & new_my_ang,
			   Vector & new_ball_pos, Vector & new_ball_vel, const bool do_random) {
  const Angle a = old_my_ang.get_value_0_p2PI();
  Angle na;
  model_cmd_main(old_my_pos,old_my_vel,a,old_ball_pos,old_ball_vel,cmd,
		 new_my_pos,new_my_vel,na,new_ball_pos,new_ball_vel,do_random);
  new_my_ang = ANGLE(na);
}

void Tools::cmd2infostring(const Cmd_Main & cmd, char *info){
  Value turn_angle = 0;
  Value dash_power = 0;
  Value kick_power = 0;
  Value kick_angle = 0;
  switch(cmd.get_type()){
  case Cmd_Main::TYPE_DASH:
    cmd.get_dash(dash_power);
    sprintf(info,"Cmd_type DASH. power %g ",dash_power);
    break;
  case Cmd_Main::TYPE_KICK:
    cmd.get_kick(kick_power, kick_angle);
    sprintf(info,"Cmd_type KICK. power  %g  angle  %g",kick_power,(Value)(RAD2DEG(kick_angle)));
  break;
  case Cmd_Main::TYPE_TURN:
    cmd.get_turn(turn_angle);
    sprintf(info,"Cmd_type TURN. angle %g",(Value)(RAD2DEG(turn_angle)));
    break;
  }
}

Value Tools::get_dash2stop(){
 Cmd_Main cmd_main;
  Value min_speed = WSinfo::me->vel.norm();
  Vector dummypos;
  Vector dummyvel;
  Angle dummyang;

  Value best_dash = 0;

  for(Value dash = -100.;dash<100.; dash += 10.){
    cmd_main.unset_lock();
    cmd_main.set_dash(dash);
    model_player_movement(WSinfo::me->pos, WSinfo::me->vel, WSinfo::me->ang.get_value(), cmd_main,
			  dummypos, dummyvel, dummyang, false);

    Value speed = fabs(dummyvel.norm());
    if(speed<min_speed){
      best_dash = dash;
      min_speed = speed;
    }
  }
  return best_dash;
}


void Tools::model_player_movement(const Vector & old_my_pos, const Vector & old_my_vel, 
				  const Angle & old_my_ang,
				  const Cmd_Main & cmd,
				  Vector & new_my_pos, Vector & new_my_vel,
				  Angle & new_my_ang,
				  const bool do_random) {

  Vector old_ball_pos, old_ball_vel, new_ball_pos, new_ball_vel;
  
  model_cmd_main(old_my_pos,old_my_vel,old_my_ang,old_ball_pos,old_ball_vel,cmd,
		 new_my_pos,new_my_vel,new_my_ang,new_ball_pos,new_ball_vel,do_random);

}


void Tools::model_cmd_main(const Vector & old_my_pos, const Vector & old_my_vel, 
			   const Angle & old_my_ang,
			   const Vector & old_ball_pos, const Vector & old_ball_vel,
			   const Cmd_Main & cmd,
			   Vector & new_my_pos, Vector & new_my_vel,
			   Angle & new_my_ang,
			   Vector & new_ball_pos, Vector & new_ball_vel, const bool do_random) {

  int old_stamina = 4000; // not used
  int new_stamina;
  model_cmd_main(old_my_pos,old_my_vel,old_my_ang,old_stamina,old_ball_pos,old_ball_vel,cmd,
		 new_my_pos,new_my_vel,new_my_ang,new_stamina, new_ball_pos,new_ball_vel,do_random);
}

void Tools::model_cmd_main(const Vector & old_my_pos, const Vector & old_my_vel, 
			   const Angle & old_my_ang, const int &old_my_stamina, 
			   const Vector & old_ball_pos, const Vector & old_ball_vel,
			   const Cmd_Main & cmd,
			   Vector & new_my_pos, Vector & new_my_vel,
			   Angle & new_my_ang, int &new_my_stamina,
			   Vector & new_ball_pos, Vector & new_ball_vel, const bool do_random) {

  Value rand1= 0.0;
  Value rand2= 0.0;
  Vector acc;
  Value tmp;

#define GET_RANDOM(X,Y)((X)+drand48() *((Y)-(X)))

  Value rmax_ball = ServerOptions::ball_rand * old_ball_vel.norm();
  Value rmax_player = ServerOptions::player_rand * old_my_vel.norm();

  if(do_random){
    rand1=GET_RANDOM(-rmax_player,rmax_player);
    rand2=GET_RANDOM(-rmax_player,rmax_player);
  }

  
  /* computation of predicted state for ball and me */
  Value turn_angle = 0;
  Value dash_power = 0;
  Value kick_power = 0;
  Value kick_angle = 0;
  new_my_stamina = old_my_stamina; // default: do not change

  switch(cmd.get_type()){
  case Cmd_Main::TYPE_DASH:
    cmd.get_dash(dash_power);
    if(dash_power>0)
      new_my_stamina += (int)(-dash_power + WSinfo::me->stamina_inc_max); 
    // assume that stamina increase is ok.
    else
      new_my_stamina += (int)(2*dash_power + WSinfo::me->stamina_inc_max); 
    dash_power = dash_power*(1.0+rand1);
    break;
  case Cmd_Main::TYPE_KICK:
    cmd.get_kick(kick_power, kick_angle);
    kick_power = kick_power*(1.0+rand1);
    kick_angle = Tools::get_angle_between_mPI_pPI(kick_angle);
    kick_angle = kick_angle*(1.0+rand2);
    kick_angle = Tools::get_angle_between_null_2PI(kick_angle);
  break;
  case Cmd_Main::TYPE_TURN:
    cmd.get_turn(turn_angle);
    turn_angle = Tools::get_angle_between_mPI_pPI(turn_angle);
    turn_angle = turn_angle*(1.0+rand1);
    turn_angle = Tools::get_angle_between_mPI_pPI(turn_angle);
    break;
  }

  //LOG_POL(2,<<"Tools: Model Dash: "<<dash_power);
  //LOG_POL(2,<<" old ball vel "<<old_ball_vel);

  // copying current state variables
  new_ball_vel = old_ball_vel;
  new_ball_pos = old_ball_pos;
  new_my_pos = old_my_pos;
  new_my_vel = old_my_vel;
  new_my_ang = old_my_ang;

  if(do_random){
    Value r=GET_RANDOM(-rmax_ball,rmax_ball);
    new_ball_vel.x += r;
    r=GET_RANDOM(-rmax_ball,rmax_ball);
    new_ball_vel.y += r;
    r=GET_RANDOM(-rmax_player,rmax_player);
    new_my_vel.x += r;
    r=GET_RANDOM(-rmax_player,rmax_player);
    new_my_vel.y += r;
  }

  //step 1 : accelerate objects

  // me
  if(dash_power != 0){    
    acc.x = dash_power * cos(new_my_ang) * WSinfo::me->dash_power_rate * WSinfo::me->effort;
    acc.y = dash_power * sin(new_my_ang) * WSinfo::me->dash_power_rate * WSinfo::me->effort;
    
    if (acc.x || acc.y) {
      new_my_vel += acc ;
      if ((tmp = new_my_vel.norm()) > ServerOptions::player_speed_max)
	new_my_vel *= ( ServerOptions::player_speed_max / tmp) ;
    }
  }

  // ball
  if(kick_power != 0){ 
    Vector ball_dist = new_ball_pos - new_my_pos;
    if(ball_dist.norm() <= WSinfo::me->kick_radius){
      
      Value ball_angle = ball_dist.arg()-new_my_ang;
      ball_angle = Tools::get_angle_between_mPI_pPI(ball_angle);

      Value ball_dist_netto = ball_dist.norm() - WSinfo::me->radius - ServerOptions::ball_size;
      
      /* THIS IS WRONG!!! 
      kick_power *= ServerOptions::kick_power_rate *
	(1 - 0.25*fabs(ball_angle)/PI - 0.25*ball_dist_netto/WSinfo::me->kick_radius);
      */

      kick_power *= ServerOptions::kick_power_rate *
	(1 - 0.25*fabs(ball_angle)/PI - 0.25*ball_dist_netto/(WSinfo::me->kick_radius - WSinfo::me->radius - ServerOptions::ball_size));
      
      acc.x = kick_power * cos(kick_angle+new_my_ang);
      acc.y = kick_power * sin(kick_angle+new_my_ang);
      
      if (acc.x || acc.y) {
	new_ball_vel += acc ;
	if ((tmp = new_ball_vel.norm()) > ServerOptions::ball_speed_max)
	  new_ball_vel *= (ServerOptions::ball_speed_max / tmp) ;
      }
    }
  }
  
  // turn me
  if(turn_angle != 0){
    new_my_ang += turn_angle/(1.0 + 1.0*new_my_vel.norm()* WSinfo::me->inertia_moment); 
    new_my_ang = Tools::get_angle_between_mPI_pPI(new_my_ang);
  }

  //LOG_POL(2,<<"my old pos "<<new_my_pos<<" old ball pos "<<new_ball_pos
	//  <<" old ball vel "<<new_ball_vel);

  //step 2 : move
  new_my_pos += new_my_vel;     //me
  new_ball_pos += new_ball_vel; //ball

  //LOG_POL(2,<<"my new pos "<<new_my_pos<<" new ball pos "<<new_ball_pos
	  //<<" new ball vel "<<new_ball_vel);

  
  //step 3 : decay speed
  new_my_vel *= WSinfo::me->decay; //me
  new_ball_vel *= ServerOptions::ball_decay; //ball

}


void Tools::simulate_player(const Vector & old_pos, const Vector & old_vel, 
			    const ANGLE & old_ang, const int &old_stamina, 
			    const Cmd_Main & cmd,
			    Vector & new_pos, Vector & new_vel,
			    ANGLE & new_ang, int &new_stamina,
			    const Value stamina_inc_max,
			    const Value inertia_moment,
			    const Value dash_power_rate, const Value effort, const Value decay){
  
  // simulate one step of an arbitrary player

  Vector acc;
  Value tmp;

  /* computation of predicted state for ball and me */
  Value turn_angle = 0;
  Value dash_power = 0;
  new_stamina = old_stamina; // default: do not change

  switch(cmd.get_type()){
  case Cmd_Main::TYPE_DASH:
    cmd.get_dash(dash_power);
    if(dash_power>0)
      new_stamina += (int)(-dash_power + stamina_inc_max); 
    // assume that stamina increase is ok.
    else
      new_stamina += (int)(2*dash_power + stamina_inc_max); 
    dash_power = dash_power;
    break;
  case Cmd_Main::TYPE_TURN:
    cmd.get_turn(turn_angle);
    turn_angle = Tools::get_angle_between_mPI_pPI(turn_angle);
    turn_angle = turn_angle;
    turn_angle = Tools::get_angle_between_mPI_pPI(turn_angle);
    break;
  }

  // copying current state variables
  new_pos = old_pos;
  new_vel = old_vel;
  new_ang = old_ang;

  //step 1 : accelerate objects

  // player
  if(dash_power != 0){    
    acc.x = dash_power * cos(new_ang) * dash_power_rate * effort;
    acc.y = dash_power * sin(new_ang) * dash_power_rate * effort;
    
    if (acc.x || acc.y) {
      new_vel += acc ;
      if ((tmp = new_vel.norm()) > ServerOptions::player_speed_max)
	new_vel *= ( ServerOptions::player_speed_max / tmp) ;
    }
  }
  // turn me
  if(turn_angle != 0){
    new_ang += ANGLE(turn_angle/(1.0 + 1.0*new_vel.norm()* inertia_moment)); 
    //    new_ang = new_ang.ANGLE(Tools::get_angle_between_mPI_pPI(new_ang)); // not needed here?
  }

  //step 2 : move
  new_pos += new_vel;     //me
  
  //step 3 : decay speed
  new_vel *= decay; //me
}









void Tools::get_successor_state(MyState const &state, Cmd_Main const &cmd, MyState &next_state, 
const bool do_random) {
  Angle na;
  Angle a= state.my_angle.get_value_0_p2PI(); 

  model_cmd_main( state.my_pos, state.my_vel, a,state.ball_pos,state.ball_vel, 
		  cmd ,next_state.my_pos,
		  next_state.my_vel,na,next_state.ball_pos, next_state.ball_vel, do_random );
  next_state.my_angle= ANGLE(na);
  next_state.op_pos= state.op_pos;
  next_state.op_bodydir= state.op_bodydir;
  next_state.op_bodydir_age= state.op_bodydir_age;
  next_state.op = state.op;
}

bool Tools::intersection(const Vector & r_center, double size_x, double size_y,
                  const Vector & l_start, const Vector & l_end) {

  double p1_x= l_start.x - r_center.x;
  double p1_y= l_start.y - r_center.y;
  double p2_x= l_end.x   - r_center.x;
  double p2_y= l_end.y   - r_center.y;

  size_x *= 0.5;
  size_y *= 0.5;

  //now the rectangle is centered at (0,0) 

  double diff_x= (p2_x - p1_x);
  double diff_y= (p2_y - p1_y);

  if ( fabs(diff_x) >= 0.0001) {
    double N= (size_x - p1_x) / diff_x;
    double Y= p1_y + N * diff_y;
    if (0.0 <=  N  && N <= 1 && Y <= size_y && Y >= -size_y) return true;

    N= (-size_x - p1_x) / diff_x;
    Y= p1_y + N * diff_y;
    if (0.0 <=  N  && N <= 1 && Y <= size_y && Y >= -size_y) return true;
  }

  if ( fabs(diff_y) >= 0.0001) {
    double N= (size_y - p1_y) / diff_y;
    double X= p1_x + N * diff_x;
    if (0.0 <=  N  && N <= 1 && X <= size_x && X >= -size_x) return true;

    N= (-size_y - p1_y) / diff_y;
    X= p1_x + N * diff_x;
    if (0.0 <=  N  && N <= 1 && X <= size_x && X >= -size_x) return true;
  }
  return false;
}      

Value Tools::ballspeed_for_dist(const Value dist){
  Value result;
  const Value final_vel = 0.0; // final velocity after dist dist

  result = (1-ServerOptions::ball_decay)*(dist + final_vel*ServerOptions::ball_decay)
    + ServerOptions::ball_decay * final_vel;
  
  return result;
}


Vector Tools::ip_with_right_penaltyarea(const Vector p, const float dir){
// assumes that p is outside right penalty area; returns 0 if ip is not on right penalty area
  Vector dirvec;
  dirvec.init_polar(1.0, dir);
  Value border_x, alpha;
  Vector ip;

  if(p.x > FIELD_BORDER_X - PENALTY_AREA_LENGTH) 
    return Vector(0);

  if(dirvec.x >0)
    border_x = FIELD_BORDER_X - PENALTY_AREA_LENGTH; // test for opponent goal line
  else
    return Vector(0);

  // first check for horizontal line
  alpha = (border_x - p.x) / dirvec.x;
  ip.y = p.y + alpha * dirvec.y;
  ip.x = border_x;
  if(fabs(ip.y) <= PENALTY_AREA_WIDTH/2. + 3.)
    return ip;  
  return Vector(0);
}



Vector Tools::ip_with_fieldborder(const Vector p, const float dir){
  Vector dirvec;
  dirvec.init_polar(1.0, dir);
  Value border_x, border_y, alpha;
  Vector ip;

  //  LOG_POL(0,"check ip w fieldborder , pos: "<<p<<" dir "<<RAD2DEG(dir));

  if(dirvec.x >0)
    border_x = FIELD_BORDER_X; // test for opponent goal line
  else
    border_x = - FIELD_BORDER_X;

  // first check for horizontal line
  alpha = (border_x - p.x) / dirvec.x;
  ip.y = p.y + alpha * dirvec.y;
  ip.x = border_x;
  if(fabs(ip.y) <= FIELD_BORDER_Y)
    return ip;  
  // then check for vertical line
    
  if(dirvec.y >0)
    border_y = FIELD_BORDER_Y; // test for left side
  else
    border_y = - FIELD_BORDER_Y;

  alpha = (border_y - p.y) / dirvec.y;
  ip.x = p.x + alpha * dirvec.x;
  ip.y = border_y;
  return ip;
}


/* Berechnet den Schnittpunkt zweier Geraden */
Vector Tools::intersection_point(Vector p1, Vector steigung1, 
				 Vector p2, Vector steigung2) {
  double x, y, m1, m2;
  if ((steigung1.x == 0) || (steigung2.x == 0)) {
    if (fabs(steigung1.x) < 0.00001) {
      return point_on_line(steigung2, p2, p1.x);
    } else if (fabs(steigung1.x) < 0.00001) {
      return point_on_line(steigung1, p1, p2.x);
    } 
  }
  m1 = steigung1.y/steigung1.x;
  m2 = steigung2.y/steigung2.x;
  if (m1 == m2) return Vector(-51.5, 0);
  x = (p2.y - p1.y + p1.x*m1 - p2.x*m2) / (m1-m2);
  y = (x-p1.x)*m1 + p1.y;
  return Vector (x, y);

}



/* Berechnet die y-Koordinate Punktes auf der Linie, der die x-Koordinate x hat
 */
Vector Tools::point_on_line(Vector steigung, Vector line_point, Value x) {
  //steigung.normalize();
  steigung = (1.0/steigung.x) * steigung;
  if (steigung.x > 0) {
    return (x - line_point.x) * steigung + line_point;  
  }
  if (steigung.x < 0) {
    return (line_point.x - x) * steigung + line_point;  
  }  // Zur Sicherheit, duerfte aber nie eintreten
  return line_point;
} /* point_on_line */


bool Tools::point_in_triangle(const Vector & p, const Vector & t1, const Vector & t2, const Vector & t3) {
  // look for  p= a* t1 + b * t2 + c * t3 with a+b+c=1 and a,b,c >= 0
  // if such a solution doesn't exits, then the point cannot be in the triangle;

  double A= t2.x - t1.x;
  double B= t3.x - t1.x;
  double C= t2.y - t1.y;
  double D= t3.y - t1.y;
  
  double det= A * D - C * B;
  if ( fabs(det) < 0.000001 ) {//consider matrix non regular (numerical stability)
    //cout << " false, det= " << det;
    return false;
  }

  double x= p.x - t1.x;
  double y= p.y - t1.y;
  

  double a= D * x - B * y;
  double b= -C * x + A * y;
  
  a/= det;
  b/= det;

  if (a < 0 || b < 0) {
    //cout << "\n false, a= " << a << " b= " << b;
    return false;
  }
  if ( a + b > 1.0) {
    //cout << "\n false, a= " << a << " b= " << b << " a+b= " << a+ b;
    return false;
  }

#if 0
  cout << "\n A= " << A << " B= " << B;
  cout << "\n C= " << C << " D= " << D;
  cout << "\n x= " << x << " y= " << y;
  cout << "\n true  a= " << a << " b= " << b << " c= " << 1- (a+ b) << " det= " << det;
#endif
  return true;
}

bool Tools::point_in_rectangle(const Vector & p, const Vector & r1, const Vector & r2, const Vector & r3, const Vector & r4) {
  if ( point_in_triangle(p,r1,r2,r3) )
    return true;
  if ( point_in_triangle(p,r1,r2,r4) )
    return true;
  if ( point_in_triangle(p,r1,r3,r4) )
    return true;
  if ( point_in_triangle(p,r2,r3,r4) )
    return true;
  return false;
}

bool Tools::point_in_field(const Vector & p, Value extra_margin) {
  Value border_x= FIELD_BORDER_X + extra_margin;
  Value border_y= FIELD_BORDER_Y + extra_margin;
  return 
    p.x <= border_x   && 
    p.x >= -border_x &&
    p.y <= border_y && 
    p.y >= -border_y;
}

Value Tools::get_ball_decay_to_the_power(int power) {
  if ( power <= 0 )
    return 1.0;

  if ( power < num_powers )
    return ball_decay_powers[power];

  int limit= MAX_NUM_POWERS;
  if ( power < MAX_NUM_POWERS ) //be as lazy as possible ;-)
    limit= power+1;

  if ( num_powers < 0) {
    ball_decay_powers[0]= 1.0;
    ball_decay_powers[1]= ServerOptions::ball_decay;
    num_powers= 2;
  }
  
  Value start_value= ball_decay_powers[num_powers-1];
  while (num_powers < limit) {
    start_value *= ServerOptions::ball_decay;
    ball_decay_powers[num_powers]= start_value;
    num_powers++;
  }

  if ( power < num_powers ) 
    return ball_decay_powers[power];
 
  //the value of power is bigger then the size of the cache, so compute it directly
  INFO_OUT << "power= " << power << " > " 
	   << "MAX_NUM_POWERS= " << MAX_NUM_POWERS 
	   << " consider increasing the cache size";
  for ( int i= num_powers; i<= power; i++ )
    start_value *= ServerOptions::ball_decay;

  return start_value;
}

/* view and neck stuff */

ANGLE Tools::get_view_angle_width(int vang) {
  Angle normal=ServerOptions::visible_angle * PI/180.0;
  if ( WIDE == vang )
    return ANGLE(normal*2.0);
  if ( NARROW == vang )
    return ANGLE(normal*0.5); 
  return ANGLE(normal);
}

ANGLE Tools::cur_view_angle_width() {
  return get_view_angle_width(WSinfo::ws->view_angle);
}

ANGLE Tools::next_view_angle_width() {
  return get_view_angle_width(get_next_view_angle());
}

int Tools::get_next_view_angle() {
  return Blackboard::get_next_view_angle();
}

int Tools::get_next_view_quality() {
  return Blackboard::get_next_view_quality();
}

long Tools::get_last_lowq_cycle() {
  return Blackboard::get_last_lowq_cycle();
}

Vector Tools::get_last_known_ball_pos() {
  return Blackboard::get_last_ball_pos();
}

void Tools::force_highq_view() {
  if(WSinfo::ws->play_mode!=PM_PlayOn) Blackboard::force_highq_view=true;
}

void Tools::set_neck_request(int req_type, Value param, bool force) 
{
  if(Blackboard::neckReq.is_set()){
    if(force == false){
      LOG_POL(0,"Tools Error: Cannot set Neck Request; already set!");
      LOG_ERR(0,"Tools Error: Cannot set Neck Request; already set!");
      return;
    }
    else{
      LOG_POL(0,"Tools WARNING: Neck request was already set, overwriting!");
    }
  }
  LOG_POL(0,"Tools NeckRequest: Success, Neck Request has been set ["<<req_type<<","<<param<<"]!");
  Blackboard::set_neck_request(req_type,param);
}

void Tools::set_neck_request(int req_type, ANGLE param, bool force) 
{
  Tools::set_neck_request(req_type, param.get_value(), force);
}

int Tools::get_neck_request(Value &param) {
  return Blackboard::get_neck_request(param);
}

int Tools::get_neck_request(ANGLE &param) {
  Value dum;
  int res = Blackboard::get_neck_request(dum);
  param = ANGLE(dum);
  return res;
}


bool Tools::is_ball_safe_and_kickable(const Vector &mypos, const Vector &oppos, const ANGLE &opbodydir,
				    const Vector &ballpos, int bodydir_age){
  if(mypos.distance(ballpos) > WSinfo::me->kick_radius)
    return false;
  return is_ballpos_safe(oppos,opbodydir,ballpos,bodydir_age);
}

/* extended version with hetero player support and tackles, taking kick_rand into account */
bool Tools::is_ball_safe_and_kickable(const Vector &mypos, const PPlayer opp, const Vector &ballpos,
				      bool consider_tackles) {

  
  Value ball_safety=1.41*(WSinfo::ball->vel.norm()*WSinfo::me->kick_rand_factor
			  +ServerOptions::ball_rand*ServerOptions::ball_speed_max);
  if(mypos.distance(ballpos) > WSinfo::me->kick_radius-ball_safety){ 
    //LOG_MOV(0,"Checking new ballpos safety: Next ball position not in kickrange of player");
    return false;
  }
  return is_ballpos_safe(opp,ballpos,consider_tackles);
}

bool Tools::is_ball_safe_and_kickable(const Vector &mypos, const WSpset &opps, const Vector &ballpos,
				      bool consider_tackles) {
  for(int p=0;p<opps.num;p++)
    if(!is_ball_safe_and_kickable(mypos,opps[p],ballpos,consider_tackles)) return false;
  return true;
}

bool Tools::is_ballpos_safe(const Vector &oppos, const ANGLE &opbodydir,
			    const Vector &ballpos, int bodydir_age){
  // returns true if ballpos is safe in next time step, if an opponent approaches in worst case
#define SAFETY .25

  if(bodydir_age >0){ // if I'm not sure about age, take worst case
#if 0
    LOG_MOV(0,"Age of body dir "<<bodydir_age<<" dist2 ball " <<oppos.distance(ballpos)
	    <<" critical range "<<ServerOptions::kickable_area + ServerOptions::player_speed_max + SAFETY);
#endif
    if(oppos.distance(ballpos) < ServerOptions::kickable_area + ServerOptions::player_speed_max + SAFETY)
      return false;
    else
      return true;
  }

  // idea: rotate the opponent so that it virtually accelerates along the x-Axis
  // rotate ballpos, so that the relative Position remains (new_center)
  Vector new_center = ballpos - oppos;
  new_center.rotate(-(opbodydir.get_value())); 

  float op_radius = ServerOptions::kickable_area; // ridi: could be refined by exact op data
  float op_speed = ServerOptions::player_speed_max; // ridi: could be refinedby exact op data

  float y_thresh = op_radius + SAFETY;
  float x_thresh = op_radius + op_speed + SAFETY;

  //#define DRAW

  if(new_center.y > y_thresh){
#if DRAW
    LOG_MOV(0,_2D<<L2D(-( op_speed + op_radius), op_radius,(op_speed + op_radius), op_radius, "#ffff00"));
    LOG_MOV(0,_2D<<C2D(new_center.x,new_center.y,.1,"#ffff00"));
    LOG_MOV(0,<<"ok, y larger");
#endif
  /* Sput: This won't work as expected, since here we assume the opp has a quadrangular kickrange...
           so this routine is more conservative than necessary.
	   On the other hand it is quite fast and we are dealing with estimated values anyway...

	   Note that another ballpos_safe(), dealing with heteros and tackles, can be found below!
  */
    return true;
  }
  if(new_center.y < - y_thresh){
#if DRAW
    LOG_MOV(0,_2D<<L2D(-( op_speed + op_radius), -op_radius,(op_speed + op_radius), -op_radius, "#ffff00"));
    LOG_MOV(0,_2D<<C2D(new_center.x,new_center.y,.1,"#ffff00"));
    LOG_MOV(0,<<"ok, y smaller");
#endif
    return true;
  }
  if(new_center.x  > x_thresh){
#if DRAW
    LOG_MOV(0,_2D<<L2D(( op_speed + op_radius), -op_radius,(op_speed + op_radius), op_radius, "#ffff00"));
    LOG_MOV(0,_2D<<C2D(new_center.x,new_center.y,.1,"#ffff00"));
    LOG_MOV(0,<<"ok, x larger");
#endif
    return true;
  }
  if(new_center.x  < - x_thresh){
#if DRAW
    LOG_MOV(0,_2D<<L2D(-( op_speed + op_radius), -op_radius,-(op_speed + op_radius), op_radius, "#ffff00"));
    LOG_MOV(0,_2D<<C2D(new_center.x,new_center.y,.1,"#ffff00"));
    LOG_MOV(0,<<"ok, x smaller");
#endif
    return true;
  }
  return false;
}

/* use real opp data (hetero players...) and if wanted, also consider tackles! */
/* Also considers the goalie.                                                  */
#define NEW_BALLPOS_SAFE
#define NEW_SAFETY .01
#define BALL_MAX_SAFETY .12
#define PLAYER_MAX_SAFETY .10

/* this one takes a complete player set and tests every opponent.
   You SHOULD take care that only necessary players are within the pset - this routine
   does NOT remove players from it!
*/
bool Tools::is_ballpos_safe(const WSpset &opps,const Vector &ballpos,bool consider_tackles) {
  for(int p=0;p<opps.num;p++)
    if(!is_ballpos_safe(opps[p],ballpos,consider_tackles)) return false;
  return true;
}

bool Tools::is_ballpos_safe(const PPlayer opp,const Vector &ballpos,bool consider_tackles) {
  Value tackle_dist_threshold = 1.5; // this corresponds to a tackle probabilty of less than 83 %
  Value tackle_back_dist_threshold = 0; // tackling backwards is rahter unlikely

  //  if(consider_tackles)
  //  LOG_MOV(0,<<"Enter NEW check ballpos! consider_tackles : "<<consider_tackles); 

  //bool is_goalie=(WSinfo::ws->his_goalie_number>0&&opp->number==WSinfo::ws->his_goalie_number)?
  //  true:false;
  bool is_goalie=(WSinfo::his_goalie && WSinfo::his_goalie==opp);
  //LOG_POL(0,<<"ballpos_safe: is_goalie="<<is_goalie);
  //LOG_POL(0,<<"my effort="<<WSinfo::me->effort);
  Value ball_safety=min(.5*1.41*(ServerOptions::ball_rand*ServerOptions::ball_speed_max
			  +WSinfo::me->kick_rand_factor*WSinfo::ball->vel.norm()),
			BALL_MAX_SAFETY);
  Value opp_maxspeed,opp_maxspeed_back;
  //LOG_POL(0,<<"age_vel="<<opp->age_vel<<", effort="<<opp->effort);
  if(opp->age_vel>0) { // we don't know opp vel -> worst case!
    opp_maxspeed=opp->speed_max;
    opp_maxspeed_back=opp->speed_max;
  } else {
    opp_maxspeed=min(opp->vel.norm()+(100.0*opp->dash_power_rate*opp->effort),
		     opp->speed_max);
    opp_maxspeed_back=min(-opp->vel.norm()+(100.0*opp->dash_power_rate*opp->effort),
			  opp->speed_max);
  }
  if(opp->age_ang>0) { // we don't know opp dir -> worst case!
    
    Value max_radius;
    if(is_goalie) max_radius=ServerOptions::catchable_area_l+opp->speed_max;
    else {
      if(consider_tackles) {
	max_radius=tackle_dist_threshold +opp_maxspeed;
      } else {
	max_radius=opp->kick_radius+opp_maxspeed;
      }
    }
    max_radius+= min(.5*1.41*ServerOptions::player_rand*opp->speed_max,PLAYER_MAX_SAFETY);
    if(opp->pos.distance(ballpos) < max_radius+ball_safety+NEW_SAFETY) {
      //      LOG_MOV(0,<<"NEW check ballpos: Not safe [worst case assumption, op bodydir not known!]");
      return false;
    }
    else{
      //  LOG_MOV(0,<<"NEW check ballpos: Save  [worst case assumption, op bodydir not known!]");
      return true;
    }
  } // ok, current body dir data available, so consider opp's body dir!
  Vector new_center = ballpos - opp->pos;
  new_center.rotate(-(opp->ang.get_value()));

  Value op_radius,op_speed;

  if(is_goalie) {
    op_radius=ServerOptions::catchable_area_l;
  } 
  else {
    op_radius=opp->kick_radius;
  }
  
  Value nodash_safety=min( .5*ServerOptions::player_rand*opp->vel.norm(),PLAYER_MAX_SAFETY);
  Value dash_safety=min( .5*1.41*ServerOptions::player_rand*opp_maxspeed,PLAYER_MAX_SAFETY);


  Vector pos=WSinfo::me->pos;
 
    if(consider_tackles) {
      if(new_center.x>=0){ // ballpos is in the direction of the opponent's bodydir
	if((new_center.x< opp_maxspeed+op_radius + tackle_dist_threshold +dash_safety+ball_safety+NEW_SAFETY) &&
	   (fabs(new_center.y < 0.7)))
	  return false;
      } // can be tackled by quickly moving
      if(new_center.norm() < tackle_dist_threshold){
	return false; // ball can be reached by turning !
      }
    }


  if(new_center.y> op_radius+nodash_safety+ball_safety+NEW_SAFETY) return true;
  if(new_center.y<-op_radius-nodash_safety-ball_safety-NEW_SAFETY) return true;
  if(new_center.x> opp_maxspeed+op_radius+dash_safety+ball_safety+NEW_SAFETY) return true;
  if(new_center.x<-opp_maxspeed_back-op_radius-dash_safety-ball_safety-NEW_SAFETY) return true;
  return false;
}


Bool Tools::is_position_in_pitch(Vector position,  const float safety_margin){
  if ( (position.x-safety_margin < -ServerOptions::pitch_length / 2.) 
       || (position.x+safety_margin > ServerOptions::pitch_length / 2.) ) return false;
  if ( (position.y-safety_margin < -ServerOptions::pitch_width / 2.) 
       || (position.y+safety_margin > ServerOptions::pitch_width / 2.) ) return false;
  return true;
}


Value Tools::min_distance_to_border(const Vector position){
  return MIN(ServerOptions::pitch_length / 2. - fabs(position.x), ServerOptions::pitch_width / 2. - fabs(position.y));
}



bool Tools::is_a_scoring_position(Vector pos){
  const XYRectangle2d hot_scoring_area( Vector(FIELD_BORDER_X - 8, -7),
					Vector(FIELD_BORDER_X, 7)); 

  Value dist2goal = (Vector(FIELD_BORDER_X,0) - pos).norm();
  if(dist2goal > 20.0){
    //DBLOG_POL(0,"CHECK SCORING POS "<<pos<<" Failure! dist2goal > 25: "<<dist2goal);
    return false;
  }
  
  WSpset pset = WSinfo::valid_opponents;
  Vector opgoalpos =Vector(FIELD_BORDER_X,0.);
  Value scanrange = 8.0;
  Quadrangle2d check_area = Quadrangle2d(pos,opgoalpos,scanrange/2., scanrange);
  //DBLOG_DRAW(0,check_area);
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    //DBLOG_POL(0,"CHECK SCORING POS: Area  -> SUCCESS: empty ");
    return true;
  }
  if(pset.num == 1 && pset[0] == WSinfo::his_goalie){
    //DBLOG_POL(0,"CHECK SCORING POS: Area  -> SUCCESS: only goalie before position ");
      return true;
  }
  if(dist2goal > 15.0){
    //DBLOG_POL(0,"CHECK SCORING POS: FAILURE too far away for direct shot ");
      return false;
  }
  pset = WSinfo::valid_opponents;
  scanrange = 4.0;
  check_area = Quadrangle2d(pos,opgoalpos,scanrange, scanrange);
  //DBLOG_DRAW(0,check_area);
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    //DBLOG_POL(0,"CHECK SCORING POS: Area  -> SUCCESS: can score to middle ");
    return true;
  }

  pset = WSinfo::valid_opponents;
  check_area = Quadrangle2d(pos,Vector(FIELD_BORDER_X,+6.),scanrange, scanrange);
  //DBLOG_DRAW(0,check_area);
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    //DBLOG_POL(0,"CHECK SCORING POS: Area  -> SUCCESS: can score to right ");
    return true;
  }

  pset = WSinfo::valid_opponents;
  check_area = Quadrangle2d(pos,Vector(FIELD_BORDER_X,-6.),scanrange, scanrange);
  //DBLOG_DRAW(0,check_area);
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    //DBLOG_POL(0,"CHECK SCORING POS: Area  -> SUCCESS: can score to left ");
    return true;
  }
  

  if(hot_scoring_area.inside(pos)){
    //DBLOG_POL(0,"CHECK SCORING POS "<<pos<<": inside scoring position");
    return true;
  }
  //DBLOG_POL(0,"CHECK SCORING POS "<<pos<<": FAILURE not a scoring position");
  return false;
}



bool Tools::shall_I_wait_for_ball(const Vector ballpos, const Vector ballvel, int &steps){
#define SAFETY_MARGIN .3
#define MAX_STEPS 20
  steps = -1; // init
  const XYRectangle2d hot_scoring_area( Vector(FIELD_BORDER_X - 14, -11),
					Vector(FIELD_BORDER_X, 11)); 


  if(ballpos.x > WSinfo::me->pos.x &&
     hot_scoring_area.inside(WSinfo::me->pos) == false) // ball comes from before me -> go against it
    return false;
  WSpset pset= WSinfo::valid_opponents;
  pset.keep_players_in_circle(WSinfo::me->pos, 3.);
  if(pset.num > 0){
    // LOG_POL(0,"Ball is coming right to me, and no op. close to me -> just face ball");
    return false;
  }

  Vector predicted_ballpos = ballpos;
  Vector predicted_ballvel = ballvel;
  for(int i=0;i<MAX_STEPS;i++){
    predicted_ballpos += predicted_ballvel;
    predicted_ballvel *= ServerOptions::ball_decay;
    if(WSinfo::me->pos.distance(predicted_ballpos)<WSinfo::me->kick_radius){
      steps = i;
      break;
    }
  }
  if(steps <= 2){ // Ball is close2player
    if(WSinfo::me->pos.distance(predicted_ballpos)<WSinfo::me->kick_radius - SAFETY_MARGIN){
      LOG_POL(0,<<"Ball needs 2 or less steps and I'll get it without moving");
      return true;
    }
    return false; // Ball is close but I might miss it -> start active intercepting
  }
  if(steps > 0) // I'll get the ball in less than maxsteps
    return true;

  return false;

}
  
Value Tools::speed_after_n_cycles(const int n, const Value dash_power_rate,
				  const Value effort, const Value decay){
  // returns actual speed after n cycles when starting w. speed 0, and dashing 100
  Value u = 0;
  Value v= 0;
  for(int i= 0; i<n; i++){
    u = v + 100. * dash_power_rate * effort;
    v = u * decay;
  }
  //  LOG_POL(0,"Tools speed after "<<n<<" cycles, power rate "<<dash_power_rate<<" decay "<<decay<<" speed: "<<u);
  return u;
}

Value Tools::eval_pos(const Vector & pos, const Value mindist2teammate ){
  // ridi 04: simple procedure to evaluate quality of position: 0 bad 1 good

  PPlayer tcp; // teammate closest to ball
  WSpset pset = WSinfo::valid_teammates_without_me;
  pset.keep_players_in_circle(WSinfo::ball->pos, 8.0); // consider only players in reasonable distance to the ball
  tcp = pset.closest_player_to_point(WSinfo::ball->pos);
  if(tcp == 0)
    return 1.0;  // no player found that is closest to ball; return position is ok.
  return eval_pos_wrt_position(pos,WSinfo::ball->pos, mindist2teammate);
}


Value Tools::eval_pos_wrt_position(const Vector & pos,const Vector & targetpos, const Value mindist2teammate){
  // ridi 04: simple procedure to evaluate quality of position: 0 bad 1 good
  Value result = 0.0;

  WSpset pset = WSinfo::valid_opponents;
  PPlayer closest_op= pset.closest_player_to_point(pos);
  Value closest_op_dist;

  if(closest_op != NULL)
    closest_op_dist= pos.distance(closest_op->pos);
  else
    closest_op_dist= 1000;

  if(closest_op_dist < 2.0)
    return 0.0;

  // do not be too close to teammates
  pset = WSinfo::valid_teammates_without_me;
  pset.keep_players_in_circle(pos, mindist2teammate);
  if(pset.num > 0){
    result = 0.0;
    return result;
  }

  // check broader passway
  float width1 =  .9 * 2* ((pos-targetpos).norm()/2.5);
  float width2 = 4; // at ball be  a little smaller
  Quadrangle2d check_area = Quadrangle2d(pos, targetpos , width1, width2);
  LOG_POL(0, <<_2D<<check_area );
  pset = WSinfo::valid_opponents;
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    result = 4.0 + Tools::min(closest_op_dist,3.0);
    return result;
  }
  // check smaller passway
  width1 =  .7 * 2* ((pos-targetpos).norm()/2.5);
  width2 = 3; // at ball be  a little smaller
  check_area = Quadrangle2d(pos, targetpos , width1, width2);
  LOG_POL(0, <<_2D<<check_area );
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    result = 1.0  + Tools::min(closest_op_dist,3.0);
  }
  else
    result = 0.0;
  return result;
}


Value Tools::evaluate_wrt_position(const Vector & pos,const Vector & targetpos){
  // ridi 04: simple procedure to evaluate quality of position: 0 bad 1 good 2 better
  Value result = 0.0;

  // check broader passway
  float width1 =  1.0 * 2* ((pos-targetpos).norm()/2.5);
  float width2 = 4; // at ball be  a little smaller
  Quadrangle2d check_area = Quadrangle2d(pos, targetpos , width1, width2);
  //LOG_POL(0, <<_2D<<check_area );
  WSpset pset = WSinfo::valid_opponents;
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    Value mindist2op = min((pos-targetpos).norm()/2.5, 3.0);
    if( get_closest_op_dist(pos) > mindist2op){
      return 2.0;
    }
    return 1.0; // passway is free, but pos is close 2 op
  }
  // check smaller passway
  width1 =  .7 * 2* ((pos-targetpos).norm()/2.5);
  width2 = 3; // at ball be  a little smaller
  check_area = Quadrangle2d(pos, targetpos , width1, width2);
  //LOG_POL(0, <<_2D<<check_area );
  pset.keep_players_in(check_area);
  if(pset.num == 0){
    result = 1.0 ;
  }
  else
    result = 0.0;
  return result;
}

bool Tools::is_pos_free(const Vector & pos){
  WSpset pset = WSinfo::valid_teammates_without_me;
  pset.keep_players_in_circle(WSinfo::ball->pos, 3.0); // consider only players in reasonable distance to the ball
  pset.keep_players_in_circle(pos, 8.0); // consider only players in reasonable distance to the ball
  if(pset.num > 0)
    return false;
  return true;
}



Value Tools::get_closest_op_dist(const Vector pos){
    WSpset pset = WSinfo::valid_opponents;
    PPlayer closest_op= pset.closest_player_to_point(pos);
    Value closest_op_dist;
    if(closest_op != NULL)
      closest_op_dist= pos.distance(closest_op->pos);
    else
      closest_op_dist= 1000.;
    return closest_op_dist;
 }

int Tools::num_teammates_in_circle(const Vector pos, const Value radius){
    WSpset pset = WSinfo::valid_teammates_without_me;
    pset.keep_players_in_circle(pos,radius);
    return pset.num;
 }


 Value Tools::get_closest_teammate_dist(const Vector pos){
    WSpset pset = WSinfo::valid_teammates_without_me;
    PPlayer closest_teammate= pset.closest_player_to_point(pos);
    Value closest_teammate_dist;
    if(closest_teammate != NULL)
      closest_teammate_dist= pos.distance(closest_teammate->pos);
    else
      closest_teammate_dist= 1000.;
    return closest_teammate_dist;
 }

Value Tools::get_optimal_position(Vector & result, Vector * testpos, 
				   const int num_testpos,const PPlayer &teammate){

   Value mindist2teammate = 4.0;

  PPlayer ballholder; // teammate closest to ball
  Vector ipos; // not used, but needed for query
  int time2ball = -1;
  ballholder = get_our_fastest_player2ball(ipos, time2ball);
  if(ballholder != NULL){ // ballholder found and cycles <3. check it isn't me!!
    //DBLOG_DRAW(0, C2D(ballholder->pos.x,ballholder->pos.y,1.5,"blue"));
  }
  if(time2ball<0 || time2ball >=6){ // critcical value!!! 5 works, less than 5 is worse!!!
    //    DBLOG_POL(0,"CHECK BALLHODLER; NO BALLHOLDER FOUND OR TOO FAR. cycles need: "<<time2ball);
    ballholder = NULL; // reset
  }
  else if(ballholder != NULL){ // ballholder found and cycles <3. check it isn't me!!
    if(ballholder ->number == WSinfo::me->number){
      // I will be the next ballholder
      ballholder = NULL;
    }
  }


  result = testpos[0]; // default: use corrected target position.

  // now check, if I should find a better position within my region if I could probably get a pass

  Value max_V=-1;
  int bestpos = 0; // default
  int bestpos_teamdist = 0;
  Value max_Vteamdist = -1;


  for(int i= 0; i< num_testpos;i++){
    if(testpos[i].y >= ServerOptions::pitch_width/2. -1. ) // too far left
      testpos[i].y = ServerOptions::pitch_width/2. -1. ;
    if(testpos[i].y <= -(ServerOptions::pitch_width/2. -1.) ) // too far right
      testpos[i].y = -(ServerOptions::pitch_width/2. -1.) ;
    //    DBLOG_DRAW(0, C2D(testpos[i].x,testpos[i].y,0.3,"red")); // draw in any case, overdraw probably later

    
    Value closest_op_dist = get_closest_op_dist(testpos[i]);
    Value closest_teammate_dist = get_closest_teammate_dist(testpos[i]);

    if(closest_op_dist < 2.0)
      continue;
    Value Vopdist = min(4.0,closest_op_dist);
    if(closest_teammate_dist < mindist2teammate)
      continue;
    Value Vteamdist = min(5.0,closest_teammate_dist);
    if(Vteamdist>max_Vteamdist){
      bestpos_teamdist = i;
      max_Vteamdist = Vteamdist;
    }

    Value Vball = 0.0; 
    if(ballholder != NULL){
      //      DBLOG_DRAW(0, C2D(ballholder->pos.x,ballholder->pos.y,2.0,"blue"));
      Vball = evaluate_wrt_position(testpos[i],ballholder->pos);
    }
    Value Vteammate= 0.0;
    if(ballholder !=NULL && teammate != NULL && Vball == 0 && max_V < 1000) {
      //someone is close 2 ball and teammate exists and I have not found a position2ball yet
      //      DBLOG_DRAW(0, C2D(teammate->pos.x,teammate->pos.y,2.0,"blue"));
      Vteammate = evaluate_wrt_position(testpos[i],teammate->pos);
    }
    
    //    Value V=Vopdist + 100 * Vteammate + 1000*Vball;
    Value V= 100 * Vteammate + 1000*Vball;
    if(V>max_V){
      bestpos = i;
      max_V = V;
    }
    if(V>=1000.){
      //      DBLOG_DRAW(0, C2D(testpos[i].x,testpos[i].y,0.3,"green"));
    }
    else if(V>=100.){
      // DBLOG_DRAW(0, C2D(testpos[i].x,testpos[i].y,0.3,"blue"));
    }
    else if(V> 0.){
      //DBLOG_DRAW(0, C2D(testpos[i].x,testpos[i].y,0.3,"orange"));
    }
    //DBLOG_POL(0,"Evaluation pos  "<<i<<" "<<testpos[i]<<"  : "<< V);
  } // for all positions


  if(max_V <10.){
    result = testpos[bestpos_teamdist];
    //DBLOG_POL(0,"only found a position to keep distance from my teammate:  "<< result);
  }
  else{
    result = testpos[bestpos];
    //DBLOG_POL(0,"Found a position to run free:  "<< result);
  }

  //DBLOG_DRAW(0, C2D(result.x, result.y ,1.5,"red")); 
  //DBLOG_DRAW(0, C2D(result.x, result.y ,1.4,"blue")); 
  //DBLOG_POL(0,"Search Best position:  "<< result);
  if(WSinfo::me->pos.distance(result) < 1.0){
    result = WSinfo::me->pos;
  }  
  return max_V;
}


#define INFINITE_STEPS 1000

PPlayer Tools::get_our_fastest_player2ball(Vector &intercept_pos, int & steps2go){
  Vector ballpos = WSinfo::ball->pos;
  Vector ballvel = WSinfo::ball->vel;
#if 0
  Value speed = WSinfo::ball->vel.norm();
  Value dir = WSinfo::ball->vel.arg();
#endif 

  int myteam_fastest = INFINITE_STEPS;
  Vector earliest_intercept = Vector(0);
  PPlayer teammate = NULL;

  // check my team

  steps2go = -1;

  WSpset pset = WSinfo::valid_teammates;
  for (int idx = 0; idx < pset.num; idx++) {
    int my_time2react = 0;
    Vector player_intercept_pos;
    if(pset[idx]->pos.distance(ballpos) < 2.0){
      steps2go = 0; // this teammate has the ball!
      intercept_pos = pset[idx] ->pos;
      return pset[idx];
    }
    int steps = Policy_Tools::get_time2intercept_hetero(player_intercept_pos, ballpos, ballvel,
							pset[idx],
							my_time2react,myteam_fastest); 
    if(steps <0) 
      steps =INFINITE_STEPS; // player doesnt get ball in reasonable time
    if(steps < myteam_fastest){ // the fastest own player yet
      myteam_fastest = steps;
      earliest_intercept = player_intercept_pos;
      teammate=pset[idx];
    }
  }
  intercept_pos = earliest_intercept;
  
  if(myteam_fastest>=INFINITE_STEPS){
    steps2go = -1;
    return NULL;
  }
  
  steps2go = myteam_fastest;
  return teammate;


}

bool Tools::is_pos_occupied_by_ballholder(const Vector& pos){
   int time2ball = -1;
   Vector ipos;
   PPlayer ballholder = get_our_fastest_player2ball(ipos, time2ball);
   if(ballholder == NULL)
     return false;
   if(time2ball < 4 && pos.distance(ipos) < 5.0)
     return true;
   return false;
   
 }


bool Tools::is_ball_kickable_next_cycle(const Cmd &cmd, Vector & mypos,Vector & myvel, ANGLE &newmyang,
					Vector & ballpos,Vector & ballvel){

   model_cmd_main(WSinfo::me->pos,WSinfo::me->vel,WSinfo::me->ang,WSinfo::ball->pos,
		  WSinfo::ball->vel, cmd.cmd_main, mypos,myvel,newmyang,ballpos,ballvel);
   
   if(ballpos.distance(mypos) <= WSinfo::me->kick_radius)
     return true;
   else
     return false;
 }


 /***************************************************************/

 /* ridi 05: evaluate positions

 /***************************************************************/


Vector Tools::check_potential_pos(const Vector pos, const Value max_advance){
  
  WSpset pset;
  Vector endofregion;
  Value check_length = 30.;
  Value endwidth = 1.66 * check_length;
  
  Value startwidth = 5.;
  
  ANGLE testdir[10];
  int num_dirs = 0;
  testdir[num_dirs ++] = ANGLE(0);
  testdir[num_dirs ++] = ANGLE(45/180.*PI);
  testdir[num_dirs ++] = ANGLE(-45/180.*PI);
  testdir[num_dirs ++] = ANGLE(90/180.*PI);
  testdir[num_dirs ++] = ANGLE(-90/180.*PI);
  
  Value max_evaluation = evaluate_pos(pos); // default: evaluate current position
  Vector best_potential_pos = pos; 

  for(int i=0; i<num_dirs; i++){
    pset = WSinfo::valid_opponents;
    endofregion.init_polar(check_length, testdir[i]);
    endofregion += pos;
    Quadrangle2d check_area = Quadrangle2d(pos, endofregion, startwidth, endwidth);
    //    LOG_POL(1,<<_2D<< check_area );
    pset.keep_players_in(check_area);
    Value can_advance = check_length;
    if(pset.num >0){
      if(pset.num > 1 || pset[0] != WSinfo::his_goalie){
	// there is a player, can_advance is restricted to the dist to that player.
	PPlayer closest_op = pset.closest_player_to_point(pos);
	can_advance = pos.distance(closest_op->pos)*0.5;  // 0.5: potential opponent runs towards me
      }
    }
    // from here on, we know that we can advance in this direction!
    Vector potential_pos;
    can_advance = MIN(max_advance,can_advance);
    potential_pos.init_polar(can_advance,testdir[i]);
    potential_pos += pos;
    if(potential_pos.x > FIELD_BORDER_X)
      potential_pos.x = FIELD_BORDER_X;
    if(potential_pos.y > FIELD_BORDER_Y)
      potential_pos.y = FIELD_BORDER_Y;
    if(potential_pos.y < -FIELD_BORDER_Y)
      potential_pos.y = -FIELD_BORDER_Y;
    //    LOG_POL(0,<<_2D<< C2D(potential_pos.x, potential_pos.y,2.0,"orange"));
    Value evaluation = evaluate_pos(potential_pos);
    //    LOG_POL(0,<<"test potential "<<potential_pos<<" Evaluation: "<<evaluation<<" max_eval "<<max_evaluation);
    if(evaluation >max_evaluation){
      max_evaluation = evaluation;
      best_potential_pos = potential_pos;
    }
  }

  LOG_POL(0,<<_2D<< C2D(best_potential_pos.x, best_potential_pos.y,1.7,"red"));
  return best_potential_pos;
}


Value Tools::evaluate_pos(const Vector query_pos){

  Value evaluation;

  if(query_pos.x <35.){
    evaluation = query_pos.x;
  }
  else{
    evaluation = query_pos.x + FIELD_BORDER_Y - fabs(query_pos.y);
  }
  return evaluation;
}


Value Tools::evaluate_potential_of_pos(const Vector pos){
    
  Vector query_pos;
  
  query_pos = check_potential_pos(pos);
  
  Value evaluation = evaluate_pos(query_pos);

  LOG_POL(0,"Evaluation of pos "<<pos<<" : potential pos "<<query_pos<<" evaluation "<<evaluation);
  return evaluation;
}


int Tools::compare_two_positions(Vector pos1, Vector pos2){
  if(evaluate_potential_of_pos(pos1) >= evaluate_potential_of_pos(pos2))
    return 1;
  return 2;
}

void Tools::display_direction(const Vector pos, const ANGLE dir, const Value length){
  Vector targetpos;
  targetpos.init_polar(length, dir);
  targetpos+=pos;
  DBLOG_DRAW(0,L2D(pos.x,pos.y, targetpos.x,targetpos.y, "brown"));
}

// predicates ued for evaluation

bool Tools::can_advance_behind_offsideline(const Vector pos){

  if (pos.x < 0.0)
    return false;

  if(pos.x > WSinfo::his_team_pos_of_offside_line())
    return true;  // already behind offside line

  // new 05: check, if open area in front

  Vector endofregion;
  double length = 100.;
  double width = 125.; //should be >= length
  endofregion.init_polar(length, 0);
  endofregion += pos;
  Quadrangle2d check_area = Quadrangle2d(pos, endofregion, 6.,width);
  //  LOG_POL(0,<<_2D<< check_area );
  WSpset pset = WSinfo::valid_opponents;
  pset.keep_players_in(check_area);

  if(pset.num == 0){
    return true;
  }
  if(pset.num == 1 && pset[0] == WSinfo::his_goalie){
    return true;
  }
  return false;
}

int Tools::potential_to_score(Vector pos){
  if(pos.x > FIELD_BORDER_X - 8 && fabs(pos.y) <8)
    return HIGH;
  if(pos.x > FIELD_BORDER_X - 12 && fabs(pos.y) <12)
    return MEDIUM;
  if(pos.x > FIELD_BORDER_X - 16 && fabs(pos.y) <18)
    return LOW;
  return NONE;
}

bool Tools::close2_goalline(const Vector pos){
  return (pos.x > FIELD_BORDER_X - 7);
}



int Tools::compare_positions(const Vector pos1, const Vector pos2, Value & difference){
  difference = 0;
  if(potential_to_score(pos1)>potential_to_score(pos2))
    return FIRST;
  if(potential_to_score(pos1)<potential_to_score(pos2))
    return SECOND;
  
  // the potential to score is the same for both positions.
  // check, if they're already advanced.
  if(close2_goalline(pos1) == true && close2_goalline(pos2) == false)
    return FIRST;
  if(close2_goalline(pos1) == false && close2_goalline(pos2) == true)
    return SECOND;
  if(close2_goalline(pos1) == true && close2_goalline(pos2) == true){
    // they're both very far advanced, so take their y -coordinate 
    //    LOG_POL(0,"pos 1 : "<<pos1<<" are close to borderlien pos2 "<<pos2);
    difference =  (FIELD_BORDER_Y - fabs(pos1.y))  - (FIELD_BORDER_Y - fabs(pos2.y));
    return EQUAL;
  }

  // the potential to score is the same for both positions. (probably None)
  // none of the positions is very close2 goalline
  // now, compare their ability to overcome offside
  if(pos1.x > WSinfo::his_team_pos_of_offside_line() && pos2.x < WSinfo::his_team_pos_of_offside_line())
    return FIRST;
  if(pos1.x < WSinfo::his_team_pos_of_offside_line() && pos2.x > WSinfo::his_team_pos_of_offside_line())
    return SECOND;

  if(can_advance_behind_offsideline(pos1) == true && can_advance_behind_offsideline(pos2) == false){
    //LOG_POL(0,"pos 1 : "<<pos1<<" can advance behind offside , but 1 cannot "<<pos2);
    return FIRST;
  }
  if(can_advance_behind_offsideline(pos1) == false && can_advance_behind_offsideline(pos2) == true){
    //LOG_POL(0,"pos 2 : "<<pos2<<" can advance behind offside , but 1 cannot "<<pos1);
    return SECOND;
  }

  // both positions can either overcome offside or both fail. 
  // now, check their principal evaluation.
  //  LOG_POL(0,"Equal predicates. Now evaluate: pos 1 : "<<pos1<<"  pos2 "<<pos2);
  difference = evaluate_pos(pos1) - evaluate_pos(pos2);
  return EQUAL;

}


bool Tools::is_pos1_better(const Vector pos1, const Vector pos2){
  Value evaluation_delta;
  if (compare_positions(pos1, pos2, evaluation_delta) == FIRST)
    return true;
  else if (compare_positions(pos1, pos2, evaluation_delta) == SECOND)
    return false;
  else if(evaluation_delta >0)
    return true;
  else
    return false;
  return false;
}

