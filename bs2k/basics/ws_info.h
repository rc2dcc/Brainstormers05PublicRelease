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

#ifndef _WS_INFO_H_
#define _WS_INFO_H_

#include "ws.h"
#include "cmd.h"
#include "intercept.h"
#include <string.h>

/**
   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 
   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 
   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 

   Artur an alle:

   Der folgende Code soll der Nachfolger von mdpInfo werden. 
   Aus den Erfahrungen die mit mdpInfo gemacht wurden ist die Idee
   entstanden, dass es keinen Sinn macht, alle nur erdenklichen Routinen
   in mdpInfo zu kodieren, die mit Spielernummern parametrisiert werden.

   Als kleiner Auszug aus den alten mdpInfo Methoden soll dienen:

   static Vector teammate_pos_abs(int number);
   static Vector teammate_vel_abs(int number);
   static Value teammate_distance_to(int number, Vector taret);
   static Value teammate_distance_to_ball(int number);
   static Value teammate_distance_to_me(int number);

   [nur als Anmerkung am Rande: jede der obigen Routinen hat eine "for" Schleife 
   implementiert, um uberhaupt den Spieler mit der entsprechenden Nummer zu finden]

   alle obigen Routinen koennen jetzt ersetzt werden durch

   PPlayer p= WSinfo::get_teammate_by_number(number);
   p->pos.distance( pos );

   oder es wird sonstwie auf p->pos, p->vel, p->ang etc.zugegriffen.
   
   Meist wird man allerdings erst gar nicht Spieler durch Nummern referenzieren!
   Viel wichtiger ist es oftmals, Spieler mit bestimmten Eigenschaften oder 
   Positionen zu extrahieren. Oftmals werden Spielermengen als Ergebnistyp
   erwartet, was bisher unmoeglich war. Betrachten wir z.B. folgende Routinen,
   die aus dem alten mdpInfo entnommen wurden:

   static int teammate_closest_to(Vector target);
   static int teammate_closest_to_ball();
   static int teammate_closest_to_me();
   static int teammate_closest_to_ball_wme();
   static int teammate_closest_to_ball_wme_wogoalie();
   static int teammate_closest_to_wme(Vector target);
 
   Der Nachteil liegt darin, dass man jeweils nur einen Spieler bekommt, wo doch manchmal
   die ersten 2 oder 3 interessant waeren. Also war es bisher unmoeglich, Spieler zu
   extrahieren, die sich in einem bestimmten Bereich befinden. Dazu musste man selbst
   auf die untersten Codeebenen heruntersteigen, und sich mit mehrmals verschachtelten 
   "for" Schleifen herumschlagen. Hier einige Beispiele, wie obigen Routinen jetzt ersetzt 
   werden koennen:

   WSpset pset= WSinfo::valid_teammates; oder
   WSpset pset= WSinfo::valid_teammates_without_me; oder
   WSpset pset= WSinfo::valid_opponents; 
   [ pset+= WSinfo::valid_opponents waere auch moeglich, damit sowohl die teammates 
   als auch opponents in die Menge hineinkommen]
   
   pset.keep_and_sort_closest_players_to_point(3, pos);

   behaelt alle Spieler die am naechsten zur Position [pos] liegen, nach Entfernung sortiert.

   Man kann aber auch jederzeit Spieler eines Bereichs extrahieren, indem man z.B sagt

   pset.keep_players_in_rectangle( Vector(52.5,20), Vector(36.5,-20) ); 

   so dass alle Spieler im gegnerischen Strafraum erfasst werden (dabei ist es
   unerheblich wie die Menge pset zustande gekommen ist, so dass man alle
   Freiheitsgrade bei deren Zusammenstellung hat!)

   WICHTIG:
   Da der Code in dieser Datei nach bestimmten Design Kriterien erstellt worden ist,
   die fuer einen Benutzer nicht unbedingt auf Anhieb erkennbar sind (wie z.B. 
   deferred precomputation etc.), KONSULTIERT mich (=Artur) daher bevor Ihr nicht weiter 
   kommt oder meint Routinen hinzufuegen zu muessen, weil sie dringend gebraucht werden. 

   Der Code ist sicher noch nicht vollstaendig, so dass ich auf eure Inputs angewiesen bin!!!

   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 
   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 
   WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG WICHTIG 

 */

#include "ws_pset.h"


/**
   this class is the replacement for the old mdpInfo class. 
 */
class WSinfo {
#if 0
  void test() {
    WSpset pset= WSinfo::valid_teammates_without_me;
    pset+= WSinfo::valid_opponents;
    pset.keep_players_in_circle(HIS_GOAL_LEFT_CORNER, 10.0);
    pset.keep_and_sort_closest_players_to_point(2,HIS_GOAL_LEFT_CORNER);

    pset= WSinfo::valid_teammates_without_me;
    pset.keep_and_sort_closest_players_to_point(2, WSinfo::me->pos);
    //altenative if just one nearest player is needed
    PPlayer p= WSinfo::valid_teammates_without_me.closest_player_to_point(WSinfo::me->pos);

    //all opponents in the penalty area
    pset= WSinfo::valid_opponents;
    pset.keep_players_in_rectangle( Vector(52.5,20), Vector(36.5,-20) ); //keep oponents in penalty area

    //offside line computation
    pset= WSinfo::valid_opponents;
    pset.keep_and_sort_players_by_x_from_right(2); //keep oponents in penalty area
    if (pset.num > 0)
      cout << "\noffside line= " << pset[pset.num-1];
    else
      cout << "\nno offside line";
  }
#endif
 public: 
  static WS const* ws;
  static WS const* ws_full;

  //JK PASS_MSG_HACK begin
  static bool jk_pass_msg_set;
  static bool jk_pass_msg_rec;
  static char  jk_pass_msg[80];
  static long jk_pass_msg_rec_time;
  static float jk_pass_msg_x;
  static float jk_pass_msg_y;
  //JK PASS_MSG_HACK end

  static PPlayer me;       ///< convenient shorthand for the player itself
  static PPlayer me_full;  ///< convenient shorthand for the player itself in fullstate (me_full==0 iff ws_full==0)

  static PPlayer his_goalie;  ///< can be a zero pointer, if goalie is not valid or not known

  static WS::Ball const* ball;       ///< convenient shorthand for the ball
  static WS::Ball const* ball_full;  ///< convenient shorthand for the ball in fullstate (ball_full==0 iff ws_full==0)

  static Cmd *current_cmd;  ///< will be set in client.c
  
  static WSpset alive_teammates;
  static WSpset alive_teammates_without_me;
  static WSpset alive_opponents;

  static WSpset valid_teammates;
  static WSpset valid_teammates_without_me;
  static WSpset valid_opponents;

public:
  static bool get_teammate(int number, PPlayer & p);
  static int relevant_teammate[11];
  static int num_relevant_teammates;

  static bool init(const WS * worldstate, const WS * worldstate_full); ///< worldstate_full==0 is possible

public:
  static bool is_my_pos_valid() { return is_teammate_pos_valid(me); }
  static bool is_ball_pos_valid();
  static bool is_teammate_pos_valid(PPlayer);
  static bool is_opponent_pos_valid(PPlayer);

  static bool is_ball_kickable() { return is_ball_kickable_for(me); }
  static bool is_ball_kickable_for(PPlayer);

  /** the result can be 0, if no such valid palyer was found */
  static PPlayer get_teammate_by_number(int num) { 
    if ( num > NUM_PLAYERS || num < 1 ) {
      //ERROR_OUT << " wrong teammate player number " << num;
      return 0;
    }
    return numbered_valid_players[num]; 
  }

  /** the result can be 0, if no such valid palyer was found */
  static PPlayer get_opponent_by_number(int num) { 
    if ( num > NUM_PLAYERS || num < 1 ) {
      if ( num != 0 ) //just for the turnament in padova!
	//ERROR_OUT << " wrong opponent player number " << num;
      return 0;
    }
    return numbered_valid_players[num+NUM_PLAYERS]; 
  }

  static int num_teammates_within_circle(const Vector &centre, const Value radius);

  static Value my_team_pos_of_offside_line(); 
  static Value his_team_pos_of_offside_line();

  static PPlayer get_teammate_with_newest_pass_info();
  static PPlayer teammate_closest2ball();
  static void visualize_state();
  static void set_relevant_teammates_default();
  static void set_relevant_teammates(const int t1=0,const int t2=0,const int t3=0,const int t4=0,
				     const int t5=0,const int t6=0,const int t7=0,const int t8=0,
				     const int t9=0,const int t10=0,const int t11=0);

protected:
  /** 
      following values should never be accessed directly. Especially never change the above
      protected to public. Most of it is initialized in the init(...) method. Take as an example
      the computation of offside lines. These are not precomputed, but in the init(...) method
      *_cache_ok is set to false. After the first computation of an offside line
      the *_cache_ok is set to true, and the cache value can be used in following enquiries (until 
      the new cycle, when the init(...) method is called again)
   */
  static WSpset pset_tmp;

  static Value my_team_pos_of_offside_line_cache;
  static bool my_team_pos_of_offside_line_cache_ok;
  static Value his_team_pos_of_offside_line_cache;
  static bool his_team_pos_of_offside_line_cache_ok;

  /** cache for keeping all players by their numbers, use the  
      get_teammate_by_number or get_opponent_by_number methods to access the values
  */
  static PPlayer numbered_valid_players[2*NUM_PLAYERS+1];

  static PPlayer teammate_with_newest_pass_info;  
  static bool teammate_with_newest_pass_info_ok;
};

#endif
