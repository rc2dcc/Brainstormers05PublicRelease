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

#include "ws_pset.h"
#include "options.h"
#include "tools.h"
//#include "log_macros.h" //test
#include "macro_msg.h"
#include "log_macros.h"
#include "cmd.h"
#include "../behaviors/skills/intercept_ball_bms.h"

#define USE_GEOMETRY2D

////////////////////////////////////////////////////////////////////////////////
//static variables
InterceptBall                WSpset::cvInterceptBallBehavior;
WSpset::InterceptBallResult  WSpset::cvInterceptBallResult;


void WSpset::append(const WSpset & pset) {
  int size= pset.num;
  if (size + num > MAX_NUM) {
    //ERROR_OUT << "\nsize of WSpset would get larger then " << MAX_NUM << "(skipping tail elements)";
    ERROR_OUT << "\nsize of WSpset would get larger then " << MAX_NUM << "(skipping tail elements)";
    size= MAX_NUM- size -num;
  }
  memcpy(p+num,pset.p, size * sizeof( PPlayer * ));
  num+=size;
}

bool WSpset::append(PPlayer player) {
  if (num >= MAX_NUM) {
    ERROR_OUT << "\nsize of WSpset would get larger then " << MAX_NUM << "(skipping tail elements)";
    return false;
  }
  p[num]= player;
  num++;
  return true;
}

bool WSpset::prepend(PPlayer player) {
  if (num >= MAX_NUM) {
    ERROR_OUT << "\nsize of WSpset would get larger then " << MAX_NUM << "(skipping tail elements)";
    return false;
  }
  for (int i= num; i > 0; i--)
    p[i]= p[i-1];
  p[0]= player;
  num++;
  return true;
}

void WSpset::join(const WSpset & pset) {
  for (int i=0; i< pset.num; i++) {
    bool include= true;
    for (int k=0; k < num; k++) 
      if ( p[k] == pset.p[i] ) {
	include= false;
	break;
      }
    if (include) {
      bool tmp= append(pset.p[i]);
      if (!tmp)
	return;
    }
  }
}

bool WSpset::join(PPlayer player) {
  bool include= true;
  for (int k=0; k < num; k++) 
    if ( p[k] == player ) {
      include= false;
      break;
    }
  if (include) 
    return append(player);
  return true;
}

void WSpset::meet(const WSpset & pset) {
  int current_spare_idx= 0;
  for (int i=0; i< num; i++) {
    bool keep= false;
    for (int k=0; k < pset.num; k++) 
      if ( p[i] == pset.p[k] ) {
	keep= true;
	break;
      }
    if (keep) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  }
  num= current_spare_idx;
}

void WSpset::remove(const WSpset & pset) {
  int current_spare_idx= 0;
  for (int i=0; i< num; i++) {
    bool keep= true;
    for (int k=0; k < pset.num; k++) 
      if ( p[i] == pset.p[k] ) {
	keep= false;
	break;
      }
    if (keep) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  }
  num= current_spare_idx;
}

void WSpset::remove(PPlayer player) {
  int current_spare_idx= 0;
  for (int i=0; i< num; i++) 
    if ( p[i] != player ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  
  num= current_spare_idx;
}

PPlayer WSpset::closest_player_to_point(Vector pos) const {
  if (num <= 0)
    return 0;

  PPlayer result= p[0];
  Value min_sqr_dist= result->pos.sqr_distance(pos);

  for (int i=1; i<num; i++) {
    Value tmp= p[i]->pos.sqr_distance(pos);
    if (tmp < min_sqr_dist) {
      min_sqr_dist= tmp;
      result= p[i];
    }
  }
  return result;
}

PPlayer WSpset::get_player_by_number(int number) const {
  for (int i=0; i<num; i++)
    if ( p[i]->number == number )
      return p[i];
  return 0;
}

PPlayer WSpset::get_player_by_team_and_number(int team, int number) const {
  for (int i=0; i<num; i++)
    if ( p[i]->number == number && p[i]->team == team )
      return p[i];
  return 0;
}

PPlayer WSpset::get_player_with_newest_pass_info() const {
  PPlayer res= 0;
  for (int i=0; i<num; i++)
    if ( p[i]->pass_info.valid ) 
      if ( ! res )
	res= p[i];
      else 
	if ( p[i]->pass_info.age < res->pass_info.age )
	  res= p[i];
  return res;
}


void WSpset::keep_and_sort_players_by_x_from_right(int how_many) {
  Value xxx[num];
  for (int i=0; i< num; i++)
    xxx[i]= - p[i]->pos.x;

  keep_and_sort(how_many, xxx);
}

void WSpset::keep_and_sort_players_by_x_from_left(int how_many) {
  Value xxx[num];
  for (int i=0; i< num; i++)
    xxx[i]= p[i]->pos.x;

  keep_and_sort(how_many, xxx);
}

void WSpset::keep_and_sort_players_by_y_from_right(int how_many)
{
  Value xxx[num];
  for (int i = 0; i < num; i++) 
    xxx[i] = - p[i]->pos.y;
  //INFO: Aufsteigende Sortierung wird vorgenommen!
  keep_and_sort( how_many, xxx);
}

void WSpset::keep_and_sort_players_by_y_from_left(int how_many)
{
  Value xxx[num];
  for (int i = 0; i < num; i++) 
    xxx[i] = p[i]->pos.y;
  //INFO: Aufsteigende Sortierung wird vorgenommen!
  keep_and_sort( how_many, xxx);
}

void WSpset::keep_and_sort_closest_players_to_point(int how_many, Vector pos) {
  Value sqr_dist[num];
  for (int i=0; i< num; i++)
    sqr_dist[i]= p[i]->pos.sqr_distance(pos);

  keep_and_sort(how_many, sqr_dist);
}

void WSpset::keep_and_sort_best_interceptors(int how_many, Vector ball_pos, Vector ball_vel, InterceptResult * intercept_res) {
  InterceptResult ires[num];
  Intercept I;
  I.ball_pos = ball_pos;
  I.ball_vel = ball_vel;
  I.ball_vel_decay=  ServerOptions::ball_decay;

  for (int i=0; i<num; i++) {
    I.player_pos =  p[i]->pos;
    I.player_radius = p[i]->kick_radius;
    I.player_vel_max= 0.9 * p[i]->speed_max; //0.9 was also used in PolicyTools
    I.minimal_time_player_reaches_ball_at_pos( ires[i] );
LOG_POL(0,<<"i="<<i<<": team="<<p[i]->team<<"  num="<<p[i]->number<<"  ppos="<<p[i]->pos.x<<","<<p[i]->pos.y<<" ==>"<<ires[i].time<<"  ipos="<<ires[i].pos.x<<","<<ires[i].pos.y);
  }

  /* now keep the best of them */

  if (how_many > num)
    how_many= num;
  
  for (int i=0; i< how_many; i++) {
    int min_idx= i;
    for (int j=i+1; j< num; j++) 
      if ( ires[j].time <= ires[min_idx].time ) // <= results in favoring opponents to be considered fastest
        min_idx= j;

    intercept_res[i]= ires[min_idx];

    if (min_idx != i) { //toggle the elements at i and min_idx
      PPlayer obj_tmp= p[i];
      p[i]= p[min_idx];
      p[min_idx]= obj_tmp;
    }
  }
  num= how_many;
}

void 
WSpset::keep_and_sort_best_interceptors_with_intercept_behavior_to_WSinfoBallPos
                                       ( int               how_many, 
                                         InterceptResult * intercept_res) 
{
  if (cvInterceptBallResult.validAtTime == WSinfo::ws->time)
  {
    LOG_POL(3,"WSpset: COOL, I can reuse already computed intercept results (wspset.num="<<num<<")."<<std::flush);
    InterceptResult ires[num];
    for (int i=0; i<num; i++)
    {
      int playerNumber = p[i]->number;
      if (p[i]->team == MY_TEAM)
      {
        ires[i].time = cvInterceptBallResult.stepsToInterceptMyTeam[playerNumber];
        ires[i].pos  = cvInterceptBallResult.icptPositionsMyTeam   [playerNumber];
      }
      else
      {
        ires[i].time = cvInterceptBallResult.stepsToInterceptHisTeam[playerNumber];
        ires[i].pos  = cvInterceptBallResult.icptPositionsHisTeam   [playerNumber];
      }
      //LOG_POL(0,<<"ires["<<i<<"]="<<ires[i].time<<" is for player number "<<playerNumber<<std::flush);
    }
    /* now keep the best of them */
    if (how_many > num) how_many= num;
    //LOG_POL(0,<<"how_many==num=="<<how_many<<" "<<num<<std::flush);    

    for (int i=0; i< how_many; i++) 
    {
      int min_idx= i;
      for (int j=i+1; j< num; j++) 
      {
        if ( ires[j].time <= ires[min_idx].time ) // <= results in favoring opponents to be considered fastest
          min_idx= j;
        //LOG_POL(0,<<"LOOP: i="<<i<<" j="<<j<<" minidx="<<min_idx<<" ires[j].time="<<ires[j].time
        //<<" ires[min_idx].time="<<ires[min_idx].time);
      }
      intercept_res[i]= ires[min_idx];
      if (min_idx != i)  //toggle the elements at i and min_idx
      {
        PPlayer obj_tmp= p[i];
        p[i]= p[min_idx];
        p[min_idx]= obj_tmp;
        InterceptResult icptResDummy = ires[i];
        ires[i] = ires[min_idx];
        ires[min_idx] = icptResDummy;
      }
    }
    num= how_many;
  }
  else
  {
    LOG_POL(3,<<"WSpset: Ok, I must compute interception results now."<<std::flush);
    int innerHowMany = num; //num is instance variable
    InterceptResult innerInterceptResult[num];
    this->keep_and_sort_best_interceptors_with_intercept_behavior
                                       ( innerHowMany,
                                         WSinfo::ball->pos,
                                         WSinfo::ball->vel,
                                         innerInterceptResult );
    //for (int i=0; i<innerHowMany; i++)
    //  LOG_POL(0,"i="<<i<<" p[i]->number="<<p[i]->number<<" t="<<innerInterceptResult[i].time);
    cvInterceptBallResult.reset();
    for (int i=0; i<num; i++)
    {
      int playerNumber = p[i]->number;
      int playerTeam   = p[i]->team;
      if (playerTeam == MY_TEAM)
      {
        cvInterceptBallResult.stepsToInterceptMyTeam[playerNumber] = innerInterceptResult[i].time;
        cvInterceptBallResult.icptPositionsMyTeam   [playerNumber] = innerInterceptResult[i].pos;
      }
      else
      {
        cvInterceptBallResult.stepsToInterceptHisTeam[playerNumber] = innerInterceptResult[i].time;
        cvInterceptBallResult.icptPositionsHisTeam   [playerNumber] = innerInterceptResult[i].pos;
      }
    }
    if (num >= how_many) num = how_many;
    for (int i=0; i<num; i++)
      intercept_res[i] = innerInterceptResult[i];
    cvInterceptBallResult.validAtTime = WSinfo::ws->time;
  }
}

void 
WSpset::keep_and_sort_best_interceptors_with_intercept_behavior
                                       ( int               how_many, 
                                         Vector            ball_pos, 
                                         Vector            ball_vel, 
                                         InterceptResult * intercept_res) 
{
  InterceptResult ires[num];
  Intercept I;
  I.ball_pos = ball_pos;
  I.ball_vel = ball_vel;
  I.ball_vel_decay=  ServerOptions::ball_decay;

  Vector ballVelocity, resultingBallPos;
  int bestPlayerStepsForMyTeam   = 1000,
      bestPlayerStepsForHisTeam  = 1000,
      bestPlayerStepsForMyGoalie = 1000;
  int playerStepsToIntercept;
  
  //first: compute with simple intercept for all players
  for (int i=0; i<num; i++) 
  {
    I.player_pos =  p[i]->pos;
    I.player_radius = p[i]->kick_radius;
    I.player_vel_max= 0.9 * p[i]->speed_max; //0.9 was also used in PolicyTools
    I.minimal_time_player_reaches_ball_at_pos( ires[i] );
    LOG_POL(4, "WSpset: Initial estimate for team="<<p[i]->team<<" nr="<<p[i]->number<<": "<<ires[i].time<<" steps"<<std::flush);

    if ( p[i]->team == MY_TEAM && ires[i].time < bestPlayerStepsForMyTeam)
      bestPlayerStepsForMyTeam = ires[i].time;
    if ( p[i]->team == HIS_TEAM && ires[i].time < bestPlayerStepsForHisTeam)
      bestPlayerStepsForHisTeam = ires[i].time;
    if (    p[i]->team == MY_TEAM && p[i]->number == WSinfo::ws->my_goalie_number
         && ires[i].time < bestPlayerStepsForMyGoalie)
      bestPlayerStepsForMyGoalie = ires[i].time;
  }    
    
  //second: compute it again with the more correct interceptball-behavior
  //        for the best interceptors
  int interceptDeviationThreshold = 4;
  for (int i=0; i<num; i++) 
  {
    int currentBestPlayerSteps;
    if ( p[i]->team == MY_TEAM && p[i]->number != WSinfo::ws->my_goalie_number)
      currentBestPlayerSteps = bestPlayerStepsForMyTeam;
    if ( p[i]->team == HIS_TEAM )
      currentBestPlayerSteps = bestPlayerStepsForHisTeam;
    if ( p[i]->team == MY_TEAM && p[i]->number == WSinfo::ws->my_goalie_number)
      currentBestPlayerSteps = bestPlayerStepsForMyGoalie;
    int currentDiffFromBestIcptTime = ires[i].time - currentBestPlayerSteps;
    if (currentDiffFromBestIcptTime <= interceptDeviationThreshold)
    {
      LOG_POL(4, <<"WSpset: Try to correct estimate for team="<<p[i]->team<<" nr="<<p[i]->number<<std::flush);
      Cmd dummyCmd;
      //note, that the last parameter in the following call depicts the
      //number of steps that are checked maximally. if the number of steps
      //for interception exceeds that number, the intercept behavior
      //returns false
      int maxCyclesToCheck = ires[i].time + interceptDeviationThreshold;
      if (maxCyclesToCheck > 30) maxCyclesToCheck = 30;
      if (ires[i].time <= 5) maxCyclesToCheck = 40;
      if ( cvInterceptBallBehavior.get_cmd_arbitraryPlayer
                                          ( p[i],
                                            dummyCmd,
                                            p[i]->pos,
                                            p[i]->vel,
                                            p[i]->ang,
                                            ball_pos,
                                            ball_vel,
                                            playerStepsToIntercept,
                                            maxCyclesToCheck) )
      {
        //all right :-)
        ires[i].time = playerStepsToIntercept;
        resultingBallPos = ball_pos;
        ballVelocity = ball_vel;
        while (playerStepsToIntercept > 0)
        {
          resultingBallPos += ballVelocity;
          ballVelocity *= ServerOptions::ball_decay;
          playerStepsToIntercept --;
        }
        ires[i].pos = resultingBallPos;
      }
      else
      {
        LOG_POL(4, <<"WSpset: No correction made (maxcycles2check="<<ires[i].time + interceptDeviationThreshold<<")."<<std::flush);
      }
    }
    LOG_POL(1,<<"i="<<i<<": team="<<p[i]->team<<"  num="<<p[i]->number<<"  ppos="<<
    p[i]->pos.x<<","<<p[i]->pos.y<<" ==>"<<ires[i].time<<"  ipos="<<
    ires[i].pos.x<<","<<ires[i].pos.y<<std::flush);
  }

  /* now keep the best of them */
  if (how_many > num)
    how_many= num;
  
  for (int i=0; i< how_many; i++) 
  {
    int min_idx= i;
    for (int j=i+1; j< num; j++) 
      if ( ires[j].time <= ires[min_idx].time ) // <= results in favoring opponents to be considered fastest
        min_idx= j;
    intercept_res[i]= ires[min_idx];
    if (min_idx != i)  //toggle the elements at i and min_idx
    {
      PPlayer obj_tmp= p[i];
      p[i]= p[min_idx];
      p[min_idx]= obj_tmp;
      InterceptResult icptResDummy = ires[i];
      ires[i] = ires[min_idx];
      ires[min_idx] = icptResDummy;
    }
  }
  //for (int i=0; i<how_many; i++)
  //  LOG_POL(0,"kurz vor return: i="<<i<<" p[i]->number="<<p[i]->number<<" t="<<intercept_res[i].time);
  num= how_many;
}

void WSpset::keep_players_with_max_age(int age) {
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( p[i]->age <= age ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
}

void WSpset::keep_players_in( Set2d const & set ) {
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( set.inside(p[i]->pos) ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
}


void WSpset::keep_players_in_circle(Vector pos, double radius) {
  Value sqr_radius= radius*radius;
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( p[i]->pos.sqr_distance(pos) <= sqr_radius ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
}

void WSpset::keep_players_in_rectangle(Vector center, Value size_x, Value size_y) {
#ifdef USE_GEOMETRY2D
  keep_players_in(XYRectangle2d(center,size_x,size_y));
#else
  Value max_x= center.x + 0.5*size_x;
  Value min_x= center.x - 0.5*size_x;
  Value max_y= center.y + 0.5*size_y;
  Value min_y= center.y - 0.5*size_y;
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) {
    Vector pos= p[i]->pos;
    if ( pos.x <= max_x && pos.x >= min_x && pos.y <= max_y && pos.y >= min_y) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  }
  num= current_spare_idx;
#endif
}

void WSpset::keep_players_in_rectangle(Vector p1, Vector p2) {
#ifdef USE_GEOMETRY2D
  keep_players_in(XYRectangle2d(p1,p2));
#else
  Vector tmp= 0.5*p1 + 0.5*p2;
  keep_players_in_rectangle(tmp, fabs(p1.x - p2.x), fabs(p1.y - p2.y));
#endif
}

void WSpset::keep_players_in_triangle(Vector p1, Vector p2, Vector p3) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Triangle2d(p1,p2,p3));
#else
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( Tools::point_in_triangle(p[i]->pos, p1,p2,p3) ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
#endif
}

/**
   p1 and p3 must be connected by a diagonal of the quadrangle, or equivalently: 
   the points p1,p2,p3,p4 must follow the circumference of the rectangle

    p2           p1                           p3           p1
      +---------+        		        +---------+  
      |         |     <--- OK		        |         |  <--- NOT OK
      |         | 			        |         |  
      +---------+			        +---------+  
    p3           p4			      p2           p4
       
*/
void WSpset::keep_players_in_quadrangle(Vector p1, Vector p2, Vector p3, Vector p4) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Quadrangle2d(p1,p2,p3,p4));
#else
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( Tools::point_in_triangle(p[i]->pos, p1,p2,p3) ||
	 Tools::point_in_triangle(p[i]->pos, p1,p3,p4) ) { 
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
#endif
}

/**
   quadrangle is like a rectangle, but vertices are not required to be parallel to
   the x or the y axes!!!

   a                           b
   +-------------------------+          	
   |                         |
   |                         |
p1 +                         + p2
   |                         |
   |                         |
   +-------------------------+ 
  c	                      d
 
   the distance between  (a and c) is width
   the distance between  (b and d) is width

   the vectors a-c and b-d are parallel and orthogonal to the vector p2-p1

   but p2-p1 doesn't need to be parallel to the x or the y axes
*/
void WSpset::keep_players_in_quadrangle(Vector p1, Vector p2, Value width) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Quadrangle2d(p1,p2,width));
#else
  Vector tmp= p2-p1;
  Vector norm;
  norm.x= -tmp.y;
  norm.y= tmp.x;
  norm.normalize(0.5*width);
  Vector g1= p1+ norm;
  Vector g2= p1- norm;
  Vector g3= p2- norm;
  Vector g4= p2+ norm;

  //the order of the g* points is important!!!
  keep_players_in_quadrangle(g1,g2,g3,g4);
#endif
}

void WSpset::keep_players_in_quadrangle(Vector p1, Vector p2, Value width, Value width2) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Quadrangle2d(p1,p2,width,width2));
#else
  Vector norm2= p2-p1;
  Vector norm;
  norm.x= -norm2.y;
  norm.y= norm2.x;
  norm2= norm;
  norm.normalize(0.5*width);
  norm2.normalize(0.5*width2);

  Vector g1= p1+ norm;
  Vector g2= p1- norm;
  Vector g3= p2- norm2;
  Vector g4= p2+ norm2;

  //the order of the g* points is important!!!
  keep_players_in_quadrangle(g1,g2,g3,g4);
#endif
}

void WSpset::keep_players_in_halfplane(Vector pos, Vector normal_vec) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Halfplane2d(pos,normal_vec));
#else
  // A * x + B * y - C >= 0   is the representation of the halfplane
  Value A= normal_vec.x;
  Value B= normal_vec.y;
  Value C= A*pos.x + B*pos.y;

  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if (  A * p[i]->pos.x + B * p[i]->pos.y >= C ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
#endif
}
void WSpset::keep_players_in_halfplane(Vector pos, ANGLE ang1) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Halfplane2d(pos,ang1));
#else
  //we will implement this without the usage of the quite costly Vector::arg() 
  //method for the players
  Vector tmp;
  ang1+= 0.5*M_PI;
  tmp.init_polar(1.0, ang1.get_value() ); 
  keep_players_in_halfplane(pos,tmp);
#endif
}

void WSpset::keep_players_in_cone(Vector pos, ANGLE ang1, ANGLE ang2) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Cone2d(pos,ang1,ang2));
#else
  keep_players_in_halfplane(pos,ang1);
  keep_players_in_halfplane(pos, ang2+ M_PI);
#endif
}

void WSpset::keep_players_in_cone(Vector pos, Vector dir1, Vector dir2) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Cone2d(pos,dir1,dir2));
#else
  Vector tmp;
  tmp.x= -dir1.y;
  tmp.y= dir1.x;
  keep_players_in_halfplane(pos,tmp);
  tmp.x= dir2.y;
  tmp.y= -dir2.x;
  keep_players_in_halfplane(pos,tmp);
#endif
}

void WSpset::keep_players_in_cone(Vector pos, Vector dir, ANGLE ang) {
#ifdef USE_GEOMETRY2D
  keep_players_in(Cone2d(pos,dir,ang));
#else
  Value a= 0.5*ang.get_value_0_p2PI(); //here a half of the angle is taken
  Value c= cos( a );
  Value s= sin( a );

  /* Rotation Matrix (counterclockwise)    Rotation Matrix (clockwise) 
     [ cos   -sin]                         [  cos    sin]                         
     [ sin    cos]                         [ -sin    cos]                         
  */                                                                           

  Vector dir1= Vector( c*dir.x + s*dir.y, c*dir.y - s*dir.x); //rotate clockwise  
  Vector dir2= Vector( c*dir.x - s*dir.y, s*dir.x + c*dir.y); //rotate counterclockwise

  keep_players_in_cone(pos,dir1,dir2);
#endif
}

void WSpset::keep_players_with_pass_info() {
  int current_spare_idx= 0;
  for (int i=0; i<num; i++) 
    if ( p[i]->pass_info.valid ) {
      p[current_spare_idx]= p[i];
      current_spare_idx++;
    }
  num= current_spare_idx;
}


/******************************************************************************/
/* protected methods of WSpset                                                */
/******************************************************************************/
void WSpset::keep_and_sort(int how_many, Value * measured_data) {
  if (how_many > num)
    how_many= num;
  
  for (int i=0; i< how_many; i++) {
    int min_idx= i;
    for (int j=i+1; j< num; j++) 
      if ( measured_data[j] < measured_data[min_idx] ) 
        min_idx= j;

    if (min_idx != i) { //toggle the elements at i and min_idx
      Value int_tmp= measured_data[i];
      measured_data[i]= measured_data[min_idx];
      measured_data[min_idx]= int_tmp;

      PPlayer obj_tmp= p[i];
      p[i]= p[min_idx];
      p[min_idx]= obj_tmp;
    }
  }
  num= how_many;
}

