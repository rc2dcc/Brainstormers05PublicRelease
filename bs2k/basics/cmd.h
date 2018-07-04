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

#ifndef _CMD_H_
#define _CMD_H_

#include "globaldef.h"
#include "Vector.h"
#include "angle.h"
#include "macro_msg.h"
#include "comm_msg.h"
#include <iostream> 
#include "ws_pset.h"

/** 
Die Klasse Cmd_Base dient als Basisklasse fuer die Klassen:
  
Cmd_main, Cmd_Neck, Cmd_View, Cmd_Say

Cmd_Base implementiert einen Standardmechanismus fuer diese
Kommando-Klassen. Dieser besteht darin, dass man ein Kommando sperren und
setzen kann. 

Fuer den Anwender ist die Klasse Cmd von Bedeutung. Sie enthaelt im wesentlichen 
vier Felder, die den entsprechnden Kommandoklassen entsprechen.

Struktur:

                      Cmd_Base
               
                         |
      __________________/ \_________________
     |             |           |            |

  Cmd_Main    Cmd_Neck    Cmd_View     Cmd_Say


und 

                        Cmd
 
              enthaelt je ein Objekt vom Typ:
              
              Cmd_Main    
              Cmd_Neck    
              Cmd_View     
              Cmd_Say

 
Autor: ART!
*/

struct Cmd_Base {
  ///Initialzustand ist immer : (nicht gesperrt) und (Kommando nicht gesetzt)
  Cmd_Base() { unset_lock(); unset_cmd(); }
  ///Pruefe ob Objekt gesperrt ist
  bool is_lock_set() const { return lock_set; }
  ///Pruefe ob ein Kommando gesetzt ist, nur bei Ergebnis true ist in den
  ///abgeleitete Klassen das Auslesen der Werte sinnvoll.
  bool is_cmd_set()  const { return cmd_set; }
  /// setze Sperre, muss meinstens nicht explizit aufgerufen werden, da das
  /// Setzen von Kommandos in den abgeleiteten Klassen automatisch eine Sperre erzeugt
  void set_lock()   { lock_set= true; }
  /// entferne Sperre, wird evtl. nicht gebraucht
  void unset_lock() { lock_set= false; }
  /// Falls ein gesetztes Kommando doch nicht beruecksichtigt werden soll, kann 
  /// explizit mit dieser Methode tun
  void unset_cmd()  { check_lock(); cmd_set= false; } 
protected:
  /// Interner Schalter: Sperre (ein/aus)
  bool lock_set;
  /// Interner Schalter: Kommando (gesetzt/nicht gesetzt)
  bool cmd_set;
  /// Fehlermeldung falls auf ein gesperrtes Objekt geschrieben wird
  void check_lock() const
    { if (lock_set) ERROR_OUT << "\n this command is locked"; }
  /// Fehlermeldung falls kein Kommando gesetzt wurde
  void check_cmd() const;/* { 
    if (!cmd_set) {
      cerr << "\n no command was set"; 
    }
  } 
  */
};

/** 

Kommandos die vom Spieler im Spiel abgesetzt werden koennen. Es darf jeweils
nur ein Kommando gesetzt sein. Nach dem Setzen eines Kommandos wird automatisch 
eine Sperre gesetzt.

Winkel und Koordinaten werden entsprechend den Konventionen aus

mdpstate.h   <- siehe Doku. in dieser Datei

behandelt.
*/
struct Cmd_Main: public Cmd_Base{
  Cmd_Main(): Cmd_Base() { type= TYPE_NONE; lock_set=false; cmd_set=false; priority=0;}
  ///Typen von Kommandos
  const static int TYPE_NONE     = 0;
  const static int TYPE_MOVETO   = 1;
  const static int TYPE_TURN 	 = 2;
  const static int TYPE_DASH 	 = 3;
  const static int TYPE_KICK 	 = 4;
  const static int TYPE_CATCH    = 5;    
  const static int TYPE_TACKLE   = 6;

  int get_type() const { return type; }
  int get_type(Value &power, Angle &angle);
  int get_priority()  const {return priority;};

  bool clone( Cmd_Main const& cmd);

  void check_lock() const;

  void set_moveto(Value x, Value y) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_MOVETO; par_1=x;par_2= y; priority = 0; }
  void set_turn(Angle angle) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_TURN; par_ang= angle; priority = 0; }
  void set_turn(ANGLE const& angle) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_TURN; par_ang= angle.get_value_0_p2PI(); priority = 0; }
  void set_dash(Value power, int p_priority = 0) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_DASH; par_1= power; priority = p_priority; }
  void set_kick(Value power, Angle angle) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_KICK; par_1= power; par_ang= angle; priority = 0; }
  void set_catch(Angle angle) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_CATCH; par_ang= angle; priority = 0; }
  void set_tackle(Value power) 
    { check_lock(); cmd_set= true; lock_set= true; type = TYPE_TACKLE; par_1= power; priority = 0; }

  void get_moveto(Value & x, Value & y) const
    { check_type(TYPE_MOVETO); x= par_1; y= par_2; }
  void get_turn(Angle & angle) const 
    { check_type(TYPE_TURN); angle= par_ang; }
  void get_dash(Value & power) const 
    { check_type(TYPE_DASH); power= par_1; }
  void get_kick(Value & power, Angle & angle) const 
    { check_type(TYPE_KICK); power= par_1; angle= par_ang; }
  void get_catch(Angle & angle) const 
    { check_type(TYPE_CATCH); angle= par_ang; }
  void get_tackle(Value & power) const 
    { check_type(TYPE_TACKLE); power= par_1; }

  friend std::ostream& operator<< (std::ostream& o, const Cmd_Main & cmd);
private:
  int type;
  Value par_1,par_2;
  Angle par_ang;
  int priority;
  void check_type (int t) const {
    check_cmd();
    if (type != t) ERROR_OUT << "wrong type"; 
  }
};

struct Cmd_Neck: public Cmd_Base {
  Cmd_Neck(): Cmd_Base() {}
  void set_turn(Angle angle) 
    { check_lock(); cmd_set= true; lock_set= true; par_ang= angle; }
  void get_turn(Angle & angle) const
    { check_cmd();  angle= par_ang; }

  friend std::ostream& operator<< (std::ostream& o, const Cmd_Neck & cmd);
private:
  Angle par_ang;
};

struct Cmd_View: public Cmd_Base {
  Cmd_View(): Cmd_Base() {}
  const static int VIEW_ANGLE_WIDE     = WIDE;  //WIDE etc. define in globaldef.h
  const static int VIEW_ANGLE_NORMAL   = NORMAL;
  const static int VIEW_ANGLE_NARROW   = NARROW;
  const static int VIEW_QUALITY_HIGH   = HIGH;
  const static int VIEW_QUALITY_LOW    = LOW;
  void set_angle_and_quality(int ang, int quality) 
    { check_lock(); cmd_set= true; lock_set= true; view_angle= ang; view_quality= quality; }
  void get_angle_and_quality(int & ang, int & quality) const
    { check_cmd(); ang= view_angle; quality= view_quality; }

  void check_lock() const
    { if (lock_set) ERROR_OUT << "\n this command is locked, type view"; }

  friend std::ostream& operator<< (std::ostream& o, const Cmd_View & cmd);
private:
  int view_angle;
  int view_quality;
};

struct Cmd_Say: public Cmd_Base {
  Cmd_Say(): Cmd_Base() { pass.valid= false; ball.valid= false; ball_holder.valid= false; players.num= 0; msg.valid= false;}
  ~Cmd_Say() {}
  //const static int MAXLEN= 0;
  //void set_message(const char *str); 
  //void set_message(const char *str, const int p_priority);
  //int get_priority()  const {return priority;};
  //const char* get_message() const { 
  //  return message;
  //}
  /* sets the ball position and velocity. This information can be
     about future ball position and velocity, so the absolute time MUST 
     be given.
  */

  void check_lock() const
    { if (lock_set) ERROR_OUT << "\n this command is locked, type say"; }


  void set_pass(Vector const& pos, Vector const &vel, int time);
  bool pass_valid() const { return pass.valid; }
  bool get_pass(Vector & pos, Vector & vel, int & time) const;

  void set_me_as_ball_holder(Vector const& pos);
  bool ball_holder_valid() const { return ball_holder.valid; }
  bool get_ball_holder(Vector & pos) const;

  void set_ball(Vector const& pos, Vector const &vel, int age_pos, int age_vel); //age must be < 8
  bool ball_valid() const { return ball.valid; }
  bool get_ball(Vector & pos, Vector & vel, int & age_pos, int & age_vel) const;

  void set_players( WSpset const & pset );
  int get_players_num() const;
  bool get_player(int idx, Vector & pos, int & team, int & number) const;
  
  void set_msg(unsigned char type, short p1, short p2);
  bool get_msg(unsigned char & type, short & p1, short & p2) const;
  bool get_msg(SayMsg & m) const { m= msg; return true; }
  bool msg_valid() const { return msg.valid; } 
  friend std::ostream& operator<< (std::ostream& o, const Cmd_Say & cmd);
private:
  struct {
    bool valid;
    Vector ball_pos;
    Vector ball_vel;
    int time;
  } pass;

  struct {
    bool valid;
    Vector pos;
  } ball_holder;

  struct {
    bool valid;
    Vector ball_pos;
    Vector ball_vel;
    int age_pos;
    int age_vel;
  } ball;

  struct {
    struct _player {
      int number;
      int team;
      Vector pos;
    };
    static const int max_num= 3;
    _player player[max_num];
    int num;
  } players;

  SayMsg msg;
  //char message[MAXLEN+1];
  //int priority;
};

struct Cmd_Attention: public Cmd_Base {
  Cmd_Attention(): Cmd_Base() {}

  void check_lock() const
    { if (lock_set) ERROR_OUT << "\n this command is locked, type attention"; }
  
  void set_attentionto(int p)
    { check_lock(); cmd_set= true; lock_set= true; player= p; }
  void set_attentionto_none()
    { check_lock(); cmd_set= true; lock_set= true; player= -1; }
  void get_attentionto(int & p) const
    { check_cmd(); p= player; }

  friend std::ostream& operator<< (std::ostream& o, const Cmd_Attention & cmd);
private:
  int player;
};

/** Die Klasse Cmd ist die Standardschnittstelle zum Weltmodell
    bzw. Laufzeitumgebung des Agenten. 
    Alles was ein Agent in einem Zeitschritt bestimmen kann, wird in dieser
    Klasse festgehalten.
    Sie ist das Gegestueck zur Klasse WS bzw. WSinfo

                                       |
                  Weltmodell       |WSinfo>
                                       |       Taktikebene
               Laufzeitumgebung        |
                                     <Cmd|
                                       |
				       
				       
   Alle Konventionen bezueglich Koordinaten und Winkelangaben, die in der
   Datei   
   
   ws.h 

   festgehalten sind, gelten damit auch fuer die Klasse Cmd.
*/
struct Cmd {
  Cmd_Main   cmd_main;
  Cmd_Neck   cmd_neck;
  Cmd_View   cmd_view;
  Cmd_Say    cmd_say;
  Cmd_Attention cmd_att;
  friend std::ostream& operator<< (std::ostream& o, const Cmd & cmd);
};

#endif
