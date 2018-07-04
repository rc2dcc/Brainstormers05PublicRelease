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
 * Copyright (c) 2002 - , Artur Merke <amerke@ira.uka.de> 
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _TXT_FILTER_BS_H_
#define _TXT_FILTER_BS_H_

#include "txt_log.h"
#include "str2val.h"
#include "macro_msg.h"

struct TextFilterCMUlike : public TextFilter {
  void show_cur_number() {
    *out << "[P=" << cur_number << "] ";
  }

  void show_cur_level() {
    for (int i= 0; i < cur_level; i++) {
      if (cur_type == TYPE_NORMAL)
	*out << "-";
      else if ( cur_type == TYPE_ERROR )
	*out << "#";
    }
  }

  int cur_number;
  int cur_time;
  int cur_type;
  int cur_level;

  static const int TYPE_NORMAL= 0;
  static const int TYPE_ERROR= 1;
  static const int TYPE_FRAMEVIEW= 2;
  static const int TYPE_MAX_NUM= 3;

  bool use[TYPE_MAX_NUM];

  int level;
  bool show_number;
  bool show_number_in_every_line;
  bool show_time;
  bool show_time_in_every_line;
  bool show_level;
  bool show_level_in_every_line;
  bool show_type;
  bool show_type_in_every_line;

  std::ostream * out;
public:
  TextFilterCMUlike() { cur_number= -1; reset(); } 

  void reset();
  void set_stream(std::ostream & o) {
    out= &o;
  }
  void set_number(int number) {
    cur_number= number;
  }

  void set_normal_mode(int lev);
  void set_error_mode(int lev);
  void set_frameview_mode(int lev);
  bool process_type_info(int time, const char * dum, char const* & next);
  void process_character(char chr) {
    if ( cur_level > level || !use[cur_type] ) {
      //cout << "\n cur_level=
      return;
    }

    *out << chr;

    if (chr != '\n') 
      return;

    if (show_number_in_every_line)
      show_cur_number();

    if (show_time_in_every_line)
      *out << cur_time << ".0 ";

    if (show_level_in_every_line) 
      show_cur_level();
  }
};

/******************************************************************************/
/******************************************************************************/

struct CmdCount {
  int move_count;
  int kick_count;
  int dash_count;
  int turn_count;
  int catch_count;
  CmdCount() { reset(); }
  void reset();
  int total_count() const { return move_count + kick_count + dash_count + turn_count + catch_count; }
  void show_greater_counts(std::ostream & out, const CmdCount & c) const;
  void show(std::ostream & out) const;

  void set_neg_counts_to_zero();

  void operator +=(const CmdCount & count);
  void operator -=(const CmdCount & count);
  bool operator >=(const CmdCount & count) const;
};

std::ostream & operator<< (std::ostream& o, const CmdCount & v);  
CmdCount operator-(const CmdCount & c1, const CmdCount & c2);
bool operator!=(const CmdCount & c1, const CmdCount & c2);

/******************************************************************************/
/******************************************************************************/

struct TextFilterCmdCounter : TextFilter {
  CmdCount count;
  bool got_such_time;
  TextFilterCmdCounter() { reset(); }
  void reset() { 
    got_such_time= false;
    count.reset();
  }
  bool process_type_info(int time, const char * dum, char const* & next) {
    //cout << "\n" << time;
    got_such_time= true;

    if ( ! strskip(dum,".0 -",next) ) 
      return true;
    
    dum= next;
    //cout << "\n######## dum1= "; for (int i=0; i<10 && dum[i] != '\0'; i++) cout << dum[i];

    while (*dum == '-')
      dum++;
      
    if ( ! strskip(dum,"sent_cmd ",next) ) 
      return true;

    //cout << "\n######## dumE= "; for (int i=0; i<20 && dum[i] != '\0'; i++) cout << dum[i];

    dum= next;
    if ( strskip(dum,"dash",next) ) {
      count.dash_count++;
      return true;
    }
    if ( strskip(dum,"turn",next) ) {
      count.turn_count++;
      return true;
    }
    if ( strskip(dum,"kick",next) ) {
      count.kick_count++;
      return true;
    }
    if ( strskip(dum,"catch",next) ) {
      count.catch_count++;
      return true;
    }
    if ( strskip(dum,"moveto",next) ) {
      count.move_count++;
      return true;
    }
    
    next= dum;
    return true;
  } 

  void process_character(char chr) {} //do nothing
};



#endif
