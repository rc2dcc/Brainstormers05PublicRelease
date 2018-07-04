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

#include "ws.h"

std::ostream& operator<< (std::ostream& o, const WS::Player & p) {
   o << "\nPlayer= " << p.number;
   if (!p.alive) {
      o << " NOT alive";
      return o;
   }
   o  << " age= " << p.age
      << " time= " << p.time 
      << ", Pos= ("<< p.pos.x <<"," << p.pos.y << ")"
      << ", angle= " << p.ang 
      << ", neck_angle= " << p.neck_ang.get_value()
      << ", vel= ("<< p.vel.x << "," << p.vel.y  << ")"
      << ", stamina= " << p.stamina;
   return o;
}

std::ostream& operator<< (std::ostream& o, const WS::Ball & b) {
   o << "\nBall "
     << " age= " << b.age
     << " time= " << b.time
     << ", Pos= ("<< b.pos.x <<"," << b.pos.y << ")"
     << ", Vel= (" << b.vel.x << ","<< b.vel.y << ")";
   return o;
}

std::ostream& operator<< (std::ostream& o, const WS & ws) {
   int i;
   o << "\nWSstate     --------------------------------"
     << "\ntime           = " << ws.time
     << "\nplay_mode      = " << play_mode_str(ws.play_mode)
     << "\n--"
     << "\nmy_team_score  = " << ws.my_team_score
     << "\nhis_team_score = " << ws.his_team_score
     << "\n--"
     << "\nview_angle     = ";
   switch (ws.view_angle) {
   case WIDE: o<< "WIDE"; break;
   case NARROW: o<< "NARROW"; break;
   case NORMAL: o<< "NORMAL"; break;
   default: o << "invalid value";
   }
   o << "\nview_quality   = ";
   switch (ws.view_quality) {
   case HIGH: o<< "HIGH"; break;
   case LOW: o<< "LOW"; break;
   default: o << "invalid value";

   }
   o << "\n--"
     << "\nball           = " << ws.ball;
   //   if (ws.me)
   //     o << "\nme             =   " << *ws.me;
   o << "\n--"
     << "\nmy_team (" << ws.my_team_num << ")        = ";
   for (i=0;i < ws.my_team_num; i++) 
     o << ws.my_team[i];
   o << "\n--"
     << "\nhis_team (" << ws.his_team_num << ")      = ";
   for (i=0;i < ws.his_team_num; i++) 
     o << ws.his_team[i];
   o << "\nWSstate end --------------------------------";
   return o;
}

WS::WS() {
  last_message.heart_at = -1;
  last_message.from = 0;
}


