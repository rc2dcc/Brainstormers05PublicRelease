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

/* @ralf:
   This is a class to store a small history, like which player was the last at ball etc..
   MDPstate cannot give such information because it is a look at the current situation.
*/
#ifndef _BS99_MEMORY_
#define _BS99_MEMORY_

class MDPmemory {
 protected:
  static const int MAX_MOMENTUM=5;
  int momentum[MAX_MOMENTUM];
  int counter;

 public:
  MDPmemory();
  /** This function updats the history information. It must be called every cyrcle to be correct.
   */
  void update();

  int opponent_last_at_ball_number;
  int opponent_last_at_ball_time;
  int teammate_last_at_ball_number; // include myself!
  int teammate_last_at_ball_time;
  int team_last_at_ball;
  int team_in_attack; //gibt an, welches Team im Angriff ist
};

#endif
