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

#include "wmstats.h"
#include <iomanip.h>
#include <stdio.h>

/******************************************************************************/
void WMcompare::init() {
  const double infinity= 100000.0;
  for (int i=0; i<ALL_END; i++) {
    tab[i]= 0.0;
    sqr_tab[i]= 0.0;
    //max_tab[i]= -1.0;
    max_tab_time[i]= -1;
    cnt[i]= 0;
    lab[i]= 0;
    min[i]= 0; max[i]= -1;
    sep[i]= 0;
  }
  lab[MY_POS_BEG]    = "my_pos           ";
  min[MY_POS_BEG]    = -infinity;
  max[MY_POS_BEG]    = infinity;
  lab[MY_POS_BEG+1]  = "my_pos |x| <  35 ";
  min[MY_POS_BEG+1]  = 0.0;
  max[MY_POS_BEG+1]  = 35.0;
  lab[MY_POS_BEG+2]  = "my_pos |x| >= 35 ";
  min[MY_POS_BEG+2]  = 35;
  max[MY_POS_BEG+2]  = infinity;
  lab[MY_POS_BEG+3]  = "my_pos |y| <  20 ";
  min[MY_POS_BEG+3]  = 0.0;
  max[MY_POS_BEG+3]  = 20.0;
  lab[MY_POS_BEG+4]  = "my_pos |y| >= 20 ";
  min[MY_POS_BEG+4]  = 20.0;
  max[MY_POS_BEG+4]  = infinity;
  lab[BALL_POS_BEG+0]= "ball_pos [ 0, 1] ";
  min[BALL_POS_BEG+0]= 0.0;
  max[BALL_POS_BEG+0]= 1.0;
  lab[BALL_POS_BEG+1]= "ball_pos [ 1, 3] ";
  min[BALL_POS_BEG+1]= 1.0;
  max[BALL_POS_BEG+1]= 3.0;
  lab[BALL_POS_BEG+2]= "ball_pos [ 3,10] ";
  min[BALL_POS_BEG+2]= 3.0;
  max[BALL_POS_BEG+2]= 10.0;
  lab[BALL_POS_BEG+3]= "ball_pos [10,25] ";
  min[BALL_POS_BEG+3]= 10.0;
  max[BALL_POS_BEG+3]= 25.0;
  lab[BALL_POS_BEG+4]= "ball_pos [25,->] ";
  min[BALL_POS_BEG+4]= 25.0;
  max[BALL_POS_BEG+4]= infinity;
  lab[BALL_VEL_BEG+0]= "ball_vel [ 0, 1] ";
  min[BALL_VEL_BEG+0]= 0.0;
  max[BALL_VEL_BEG+0]= 1.0;
  lab[BALL_VEL_BEG+1]= "ball_vel [ 1, 3] ";
  min[BALL_VEL_BEG+1]= 1.0;
  max[BALL_VEL_BEG+1]= 3.0;
  lab[BALL_VEL_BEG+2]= "ball_vel [ 3,10] ";
  min[BALL_VEL_BEG+2]= 3.0;
  max[BALL_VEL_BEG+2]= 10.0;
  lab[BALL_VEL_BEG+3]= "ball_vel [10,25] ";
  min[BALL_VEL_BEG+3]= 10.0;
  max[BALL_VEL_BEG+3]= 25.0;
  lab[BALL_VEL_BEG+4]= "ball_vel [25,->] ";
  min[BALL_VEL_BEG+4]= 25.0;
  max[BALL_VEL_BEG+4]= infinity;
  lab[BALL_AGE_BEG+0]= "ball_age [ 0, 1] ";
  min[BALL_AGE_BEG+0]= 0.0;
  max[BALL_AGE_BEG+0]= 1.0;
  lab[BALL_AGE_BEG+1]= "ball_age [ 1, 3] ";
  min[BALL_AGE_BEG+1]= 1.0;
  max[BALL_AGE_BEG+1]= 3.0;
  lab[BALL_AGE_BEG+2]= "ball_age [ 3,10] ";
  min[BALL_AGE_BEG+2]= 3.0;
  max[BALL_AGE_BEG+2]= 10.0;
  lab[BALL_AGE_BEG+3]= "ball_age [10,25] ";
  min[BALL_AGE_BEG+3]= 10.0;
  max[BALL_AGE_BEG+3]= 25.0;
  lab[BALL_AGE_BEG+4]= "ball_age [25,->] ";
  min[BALL_AGE_BEG+4]= 25.0;
  max[BALL_AGE_BEG+4]= infinity;
  lab[BALL_INVALID]  = "ball_invalid     ";
  min[BALL_INVALID]  = -infinity;
  max[BALL_INVALID]  = 0.0;

  sep[BALL_POS_BEG]  = 20;
  sep[BALL_VEL_BEG]  = 10;
  sep[BALL_AGE_BEG]  = 20;
}

void WMcompare::add(const WMstate & approx, const WMstate & exact) {
  if ( exact.play_mode != PM_play_on )
    return;

  double ttt, vvv;
  const WMstate::_wm_player & exact_me= exact.my_team[WM::my_number];
  const WMstate::_wm_player & approx_me= approx.my_team[WM::my_number];
  
  ttt= fabs(exact_me.pos.x);
  vvv= exact_me.pos.distance(approx_me.pos);
  tmp[MY_POS_BEG]= ttt;  
  val[MY_POS_BEG]= vvv;
  tmp[MY_POS_BEG+1]= ttt;
  val[MY_POS_BEG+1]= vvv;
  tmp[MY_POS_BEG+2]= ttt;
  val[MY_POS_BEG+2]= vvv;
  ttt= fabs(exact_me.pos.y);
  tmp[MY_POS_BEG+3]= ttt;
  val[MY_POS_BEG+3]= vvv;
  tmp[MY_POS_BEG+4]= ttt;
  val[MY_POS_BEG+4]= vvv;
  ttt= exact_me.pos.distance( exact.ball.pos );
  vvv= (exact.ball.pos - exact_me.pos).distance( approx.ball.pos - approx_me.pos );
  for (int i=BALL_POS_BEG; i<BALL_POS_END; i++) {
    tmp[i]= ttt;
    val[i]= vvv;
  }
  vvv= exact.ball.vel.distance(approx.ball.vel);
  for (int i=BALL_VEL_BEG; i<BALL_VEL_END; i++) {
    tmp[i]= ttt;
    val[i]= vvv;
  }

  if ( approx.ball.time_pos.time < 0 ) { //just count cases with not neg. ball time
    ttt= -1.0;
    vvv= 1.0;
  }
  else
    vvv= approx.time.time - approx.ball.time_pos.time;

  for (int i=BALL_AGE_BEG; i<BALL_AGE_END; i++) {
    tmp[i]= ttt;
    val[i]= vvv;
  }
  tmp[BALL_INVALID]= ttt;
  val[BALL_INVALID]= vvv;

  for (int i=0; i< ALL_END; i++) 
    if ( tmp[i] >= min[i] && tmp[i] < max[i] ) {
      vvv= val[i];
      cnt[i]++;
      double step= 1.0/double(cnt[i]);
#if 0
      if ( i == BALL_POS_BEG ) {
	cout << "\nt= " << approx.time.time << ": " << tab[i] << " c=" << cnt[i] << " v= " << vvv 
	     << " eb= " << exact.ball.pos << " em= " << exact_me.pos
	     << " ab= " << approx.ball.pos << " em= " << approx_me.pos
	     << " abt= " << approx.ball.time_pos.time;
      }
#endif
      tab[i] += step*( vvv - tab[i] );
      sqr_tab[i] += step*( vvv*vvv - sqr_tab[i] );
      if ( max_tab_time[i] < 0 || fabs(vvv) > max_tab[i] ) {
	max_tab[i]= fabs(vvv);
	max_tab_time[i]= exact.time.time;
      }
    }
}

      
void format(std::ostream & out, int len, int prec, double v) {
  char dum[10];
  //sprintf(dum,"\%%dlf",prec);
  sprintf(dum,"%%%d.%dlf",len,prec);
  //cout << "\n DUM= " << dum;
  char buf[30];
  sprintf(buf,dum,v);
  //sprintf(buf,"%5.3lf",v);
  out << buf;
}

void WMcompare::show(std::ostream & out) {
  out << "\nWMcompare::show";
  for (int i=0; i<ALL_END; i++) {
    if ( sep[i] ) {
      cout << "\n";
      for (int j=0; j< sep[i]; j++)
	cout << "-";
    }

    if ( cnt[i] == 0 )
      continue;

    out << "\n";

    if ( lab[i] ) 
      out << lab[i];
    else
      out << "################";
    
    out << "exp= ";
    format(out,6,3,tab[i]);
    out << ", dev= "; //Standardabweichung = sqrt(Varianz)
    format(out,6,3, sqrt(sqr_tab[i] - tab[i]*tab[i]));
    out << ", max= ";
    format(out,6,3,max_tab[i]);
    out << "(" << setw(4) << max_tab_time[i] << ")"
	<< ", cnt= " << setw(4) << cnt[i];  
  }
}
