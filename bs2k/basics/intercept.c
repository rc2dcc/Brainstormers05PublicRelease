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

#include "intercept.h"

/******************************************************************************
   class Intercept (for questions ask ART!)
*******************************************************************************/

bool Intercept::ball_in_player_radius_at_time( Value ball_vel_decay_to_the_power_of_time, int time ) {
  /* 1) valid values for time are: 0,1,2,3,...
     2) ball_vel_decay_to_the_power_of_time= ball_vel_decay ** time  (time 0 is a special case)
  */

  if ( time == 0 && player_radius >= 0) // special case, does not involve geometric series
    return (ball_pos - player_pos).sqr_norm() <= (player_radius * player_radius);

  // the vector C is the vector from player_pos (at time 0) to the ball_pos at time "time" */
  Vector C = ball_pos 
    + (1.0- ball_vel_decay_to_the_power_of_time)/(1.0 - ball_vel_decay)* ball_vel  
    - player_pos ;
  Value player_circle_radius= time * player_vel_max + player_radius;
#if 0
  cout << "\n  time=" << time 
       << "\n  distance: " << C.norm()
       << "\n  player_circle " << time * player_vel_max
       << "\n  ball_pos_at_time[" << time <<"]= " << C + player_pos;
#endif
  //return C.sqr_norm() <= (player_circle_radius * player_circle_radius);
  // ridi: above formula wrong for player_circle_radius < 0 use that instead
  return C.norm() <= player_circle_radius;
}

void Intercept::minimal_time_player_reaches_ball_at_pos( InterceptResult & res ) {
  minimal_time_player_reaches_ball_at_pos( res.time, res.pos );
}

void Intercept::minimal_time_player_reaches_ball_at_pos( int & res_time, Vector & res_pos ) {
  Value gamma[MAX_STEPS];
  int time[MAX_STEPS];
  gamma[0] = 1.0;              time[0]= 0;
  gamma[1]= ball_vel_decay;    time[1]= 1;
  
  if ( ball_in_player_radius_at_time(gamma[0], time[0]) ) {
    res_time= time[0];
    res_pos= ball_pos;
    return;
  } 

  if ( ball_in_player_radius_at_time(gamma[1], time[1]) ) {
    res_time= time[1];
    res_pos= ball_pos+ ball_vel;
    return;
  }

  int i= 2;
  for (;;) {
    gamma[i]= gamma[i-1] * gamma[i-1];
    time[i] = time[i-1] + time[i-1];
#if 0
    cout << "\ni= " << i 
	 << " gamma[" << i << "]= " << gamma[i]
	 << " time[" << i << "]= " << time[i];
#endif    
    if ( ball_in_player_radius_at_time( gamma[i], time[i] ) ) 
      break;
    i++;
    if ( i >= MAX_STEPS ) {
      //cout << "\nNeeds more then " << time[i-1] << " steps.";
      res_time= time[i-1] ;
      res_pos =  ball_pos + (1.0- gamma[i-1] )/(1.0 - ball_vel_decay)* ball_vel;
      return;
    }
  }
  i= i-1;
  
  /* invariant in the following loop are:
     ball_in_player_radius_at_time(max,max_t) = true;
     ball_in_player_radius_at_time(min,min_t) = false;
     min= ball_vel_decay ** min_t;
     mid= ball_vel_decay ** mid_t;
     max= ball_vel_decay ** max_t;
  */
  int min_t= time[i];
  int mid_t= time[i]+ time[i-1];
  int max_t= time[i+1];
  Value min= gamma[i];
  Value mid= gamma[i]*gamma[i-1];
  Value max= gamma[i+1];

  while ( i > 1) {
#if 0
    cout << "\n i= " << i << " ( " << min_t << " , " << mid_t << " , " << max_t << " )";
#endif
    if ( ball_in_player_radius_at_time(mid, mid_t) ) {
      max= mid;
      mid= min*gamma[i-2];
      max_t= mid_t;
      mid_t= min_t + time[i-2];
    }
    else {
      min= mid;
      mid= min*gamma[i-2];
      min_t= mid_t;
      mid_t= min_t + time[i-2];
    }
    i= i-1;
  }
#if 0
  cout << "\n i= " << i << " ( " << min_t << " , " << mid_t << " , " << max_t << " )"
       << "\nTime = " << max_t;
#endif
  res_time= max_t;
  res_pos = ball_pos + (1.0-max)/(1.0- ball_vel_decay)*ball_vel;
}
/******************************************************************************
  end class Intercept
*******************************************************************************/
