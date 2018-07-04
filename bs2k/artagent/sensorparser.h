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
#ifndef _SENSORPARSER_H_
#define _SENSORPARSER_H_

#include "sensorbuffer.h"

class SensorParser {
public:
  static bool get_message_type_and_time(const char *, int & type, int & time);
  static bool manual_parse_fullstate(const char *, Msg_fullstate & fullstate);
  static bool manual_parse_fullstate(const char *, Msg_fullstate_v8 & fullstate);
  static bool manual_parse_see(const char *, Msg_see & see, char const* & next);
  static bool manual_parse_init(const char *, Msg_init & init);
  static bool manual_parse_sense_body(const char *, Msg_sense_body & sb, char const* & next);
  static bool manual_parse_hear(const char *, Msg_hear & hear, char const* & next, bool & reset_int);
  static bool manual_parse_teamcomm(const char *, Msg_teamcomm & tc, const char *& next);
  static bool manual_parse_teamcomm(const char *, Msg_teamcomm2 & tc, const char *& next);
  static bool manual_encode_teamcomm(char *, const Msg_teamcomm & tc, char *& end);
  static bool manual_encode_teamcomm(char *, const Msg_teamcomm2 & tc, char *& end);
  // static bool manual_parse_teamcomm_side(const char *, int & side);
  static bool manual_parse_server_param(const char *, Msg_server_param & sp);
  static bool manual_parse_player_param(const char *, Msg_player_param & pp);
  static bool manual_parse_player_type(const char *, Msg_player_type & pt);
  static bool manual_parse_change_player_type(const char *, Msg_change_player_type & cpt);

  static bool manual_parse_my_trainercomm(const char * str);

  static void show_parser_error_point(std::ostream & out, const char * origin, const char * parse_error_point);
 protected:
  static bool manual_parse_my_online_coachcomm(const char *, Msg_my_online_coachcomm & moc, const char * & next);
  static bool manual_parse_play_mode(const char *, PlayMode & pm, int & score, const char *& next);
  static bool manual_parse_view_mode(const char *, int & quality, int & width, const char *& next);

  static const double ENCODE_SCALE= 150.0;
 protected:
  struct ParseObject {
    static const int UNKNOWN= -1;
    static const int MARKER_LINE= 0;
    static const int MARKER= 1;
    static const int BALL_OBJECT= -2;
    static const int PLAYER_OBJECT= 2;
    static const int PLAYER_MY_TEAM= 2;
    static const int PLAYER_MY_TEAM_GOALIE= 3;
    static const int PLAYER_HIS_TEAM= 4;
    static const int PLAYER_HIS_TEAM_GOALIE= 5;
    static const int PLAYER_UNKNOWN= 6;
    int res;
    int number;
    double x;
    double y;
  };
  static bool manual_parse_see_object(const char *, ParseObject & pobj, const char *& next);

  public: //debug
  
  // pos_x_range * pos_y_range must be < 2^13
  static const int pos_x_range= 55;
  static const int pos_y_range= 36;
  static const int vel_range= 30;
  static const unsigned int invalid_id= 0;
  static const unsigned int ball_id= 1;
  static const unsigned int pass_info_id= 24;
  static const unsigned int ball_info_id= 25;
  static const unsigned int ball_holder_info_id= 26;
  static const unsigned int msg_id= 27;
  static const unsigned int max_id= 27;

  static const int max_bit_size= 60;
  static const int object_bit_size= 18;
  static const int ball_holder_info_bit_size= 18;
  static const int pass_info_bit_size= 37;
  static const int ball_info_bit_size= 36;

  static bool pos_in_range13bit(Vector pos) {
    if ( pos.x < -pos_x_range || pos.x > pos_x_range || pos.y < -pos_y_range || pos.y > pos_y_range )
      return false;
    return true;
  }

  static bool pos_to_range13bit(Vector pos, unsigned int & max_13_bit) {
    //partition -pos_x_range,...,pos_x_range in x dir  : 2*pos_x_range+1 values
    //partition -pos_y_range,...,pos_y_range in y dir  : 2*pos_y_range+1 values

    if ( ! pos_in_range13bit(pos) ) 
      return false;
    
    unsigned int x=  int( rint(pos.x + pos_x_range) );
    unsigned int y=  int( rint(pos.y + pos_y_range) );
    max_13_bit= x * (2*pos_y_range+1) + y;
    return true;
  }

  static bool range13bit_to_pos(unsigned int max_13_bit, Vector & pos) {
    if ( max_13_bit >= ( 2*pos_x_range+1 )* ( 2*pos_y_range+1) )
      return false;

    //partition -pos_x_range,...,pos_x_range in x dir  : 2*pos_x_range+1 values
    //partition -pos_y_range,...,pos_y_range in y dir  : 2*pos_y_range+1 values

    pos.y= max_13_bit % (2*pos_y_range+1);
    pos.y -= pos_y_range;
    pos.x= max_13_bit / (2*pos_y_range+1);
    pos.x -= pos_x_range;
    
    return true;
  }

  static bool vel_to_range12bit(Vector vel, unsigned int & max_12_bit) {
    vel.x *= 10.0;
    vel.y *= 10.0;

    if ( vel.x < -vel_range || vel.x > vel_range || vel.y < -vel_range || vel.y > vel_range )
      return false;
    
    unsigned int x=  int( rint(vel.x + vel_range) );
    unsigned int y=  int( rint(vel.y + vel_range) );
    max_12_bit= x * (2*vel_range+1) + y;
    return true;
  }

  static bool range12bit_to_vel(unsigned int max_12_bit, Vector & vel) {
    if ( max_12_bit >= ( 2*vel_range+1 )* ( 2*vel_range+1) )
      return false;

    vel.y= max_12_bit % (2*vel_range+1);
    vel.y -= vel_range;
    vel.y /= 10.0;
    vel.x= max_12_bit / (2*vel_range+1);
    vel.x -= vel_range;
    vel.x /= 10.0;
    
    return true;
  }

#if 0
  static bool pos2uints(Vector pos, unsigned int & x, unsigned int & y) {
    pos.x += PITCH_LENGTH * 0.5 + 2.0;
    pos.y += PITCH_WIDTH * 0.5 + 2.0;
    pos.y *= 0.9;
    int tmp_x= int(pos.x);
    int tmp_y= int(pos.y);
    if ( tmp_x < 0 || tmp_x > 109 )
      return false;
    if ( tmp_y < 0 || tmp_y > 63 )
      return false;
    x= tmp_x;
    y= tmp_y;
    return true;
  }
  
  static bool uints2pos(unsigned int x, unsigned int y, Vector & pos) {
    if ( x < 0 || x > 109 )
      return false;
    if ( y < 0 || y > 63 )
      return false;
    pos.x = double(x);
    pos.x -= PITCH_LENGTH * 0.5 + 2.0;
    pos.y = double(y);
    pos.y /= 0.9;
    pos.y -= PITCH_WIDTH * 0.5 + 2.0;
    return true;
  }
#endif
};

int str2val(const char * str, NOT_NEEDED & val, const char* & next) ;


#endif
