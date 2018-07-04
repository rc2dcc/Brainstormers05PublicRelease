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

#ifndef _WMSTATS_H_
#define _WMSTATS_H_

#include "globaldef.h"
#include "wmstate.h"

#include <iostream>

class WMcompare {
  static const int MY_POS_BEG= 0;
  static const int MY_POS_END= MY_POS_BEG+5;
  static const int BALL_POS_BEG= MY_POS_END;
  static const int BALL_POS_END= BALL_POS_BEG+5;
  static const int BALL_VEL_BEG= BALL_POS_END;
  static const int BALL_VEL_END= BALL_VEL_BEG+5;
  static const int BALL_AGE_BEG= BALL_VEL_END;
  static const int BALL_AGE_END= BALL_AGE_BEG+5;
  static const int BALL_INVALID= BALL_AGE_END;
  static const int ALL_END= BALL_INVALID+1;
  char * lab[ALL_END];
  //int upd[ALL_END];
  //double dum[ALL_END];
  double val[ALL_END];
  double tmp[ALL_END];
  int    sep[ALL_END];
 public:  
  double min[ALL_END];
  double max[ALL_END];
  double tab[ALL_END];
  double sqr_tab[ALL_END];
  double max_tab[ALL_END];
  int    max_tab_time[ALL_END];
  int cnt[ALL_END];
  void init();
  void add(const WMstate & approx, const WMstate & exact);
  void show(std::ostream &);
};

#endif
