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

/* Author: FluffyBunny
 *
 * This is a module template
 *
 */

#ifndef _MOD_TRAIN_H_
#define _MOD_TRAIN_H_

#include "defs.h"
#include "coach.h"
#include "param.h"
#include "field.h"
#include "options.h"
#include "modules.h"
#include "messages.h"
#include <iomanip.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <strstream>
#include <string>

#define CONTINUE 0
#define TEAM_HAS_BALL 1
#define OP_HAS_BALL 2
#define TIME_OVER 3
#define GAME_INTERRUPTED 4
#define BALL_LEFT_KICKRANGE 5
#define PASS_SUCCESS 6
#define BALL_INTERCEPTED 7
#define NEAR_GOAL 101
#define GOAL 102
#define GOALIE_CATCHED 103

//////////////////////////////////////////////////////////////////////////////////////
//////////////////// Supporting Classes //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

class Table {
friend ostream& operator<< (ostream& o,const Table& t);
private:
  int num_rows;
  int num_cols;
  int max_rows;
  float **tab;
  void set_max_rows(int max);
public:
  Table () { num_rows= 0; num_cols= 0; max_rows=0; }

  float operator()(int row,int col) const;
  const float * operator()(int row) const;
  void set(int row, int row, float value);
  int get_num_rows() const {return num_rows;}

  char* float_to_str(int num);

  bool save(const char*) const;
  bool load(int col,const char*);
};

class Situations {
 public:
  static const int NUM_OBJECTS= 23;
 protected:
  Table tab;
  bool active[NUM_OBJECTS];
  int num_sit;
  int cur_sit;
 public:
  Situations();

  int get_num_sit() const { return num_sit; }
  void set_cur_sit(int i) { if (i>=0 && i< num_sit) cur_sit= i; }
  void load_table(const char* fname);
  void set_active_obj(int i) { if (i>=0 && i<NUM_OBJECTS) active[i]= true; }
  void set_inactive_obj(int i) { if (i>=0 && i<NUM_OBJECTS) active[i]= false; }
  void send_situation(int);
  void send_cur_situation() {
    send_situation(cur_sit);
     cur_sit+=  1;
    if ( cur_sit >= num_sit ) cur_sit= 0;
  }

}; 

///////////////////////////////////////////////////////////////////////////////////

class ModTrain : public AbstractModule {
  static const int MAX_STR_LEN= 512;
  Situations sit;
  char train_file[MAX_STR_LEN];
  char conf_file[MAX_STR_LEN];
  char prot_file[MAX_STR_LEN];
  bool protocol_on;
  FILE *protf;

  bool sit_loop;
  void protocol_sit(const int terminated=0);
  float get_abs_angle(float angle);
  void start_new_sequence();
  int check_stop_sequence();
  void print_statistics();
  int player_type;

 public:

  bool init(int argc,char **argv);             /** init the module */
  bool destroy();                              /** tidy up         */
  
  bool behave();                               /** called once per cycle, after visual update   */
  bool onRefereeMessage(bool PMChange);        /** called on playmode change or referee message */
  bool onHearMessage(const char *str);         /** called on every hear message from any player */
  bool onKeyboardInput(const char *str);       /** called on keyboard input                     */
  bool onChangePlayerType(bool,int,int=-1);    /** SEE mod_template.c!                          */
  
  static const char modName[];                 /** module name, should be same as class name    */
  const char *getModName() {return modName;}   /** do not change this!                          */
};


#endif
