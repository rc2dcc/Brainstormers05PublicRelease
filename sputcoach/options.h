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

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "defs.h"
#include "Vector.h"
#include "valueparser.h"
#include <iostream>
#include <fstream>

struct Options {
  static const int MAX_LEN= 150;
  static char host[MAX_LEN];
  static int port;
  static bool isTrainer;
  static bool interactive;
  static bool waitForOurTeam;
  static bool waitForHisTeam;
  static bool synch_mode;
  static char coach_conf[MAX_LEN];

  static bool server_94;
  
  static char teamName[30];
  //static bool wm_fukuoka;   // no longer needed
  
  static void init();
  static void print_options();
  static bool read_config_name(int argc,char **argv);
  static bool read_from_file(char *config_file);
  static bool read_from_command_line(int argc, char **argv);

private:
  static const int MAX_LINE_LENGTH = 500;

};

/*****************************************************************************/
/*****************************************************************************/
/* LogOptions */

class LogOptions{
  struct OutOpt {
    void init(bool a, bool b, bool c) {
      log_cout= a;
      log_cerr= b;
      log_file= c;
    }
    void init(int num, const char * val);
    OutOpt() { init(true,false,true);}
    OutOpt(bool a, bool b, bool c) { init(a,b,c); }
    bool log_cout;
    bool log_cerr;
    bool log_file;
  };
  static const int MAX_LEN= 512;
 public:
  static bool use_file() { return opt_log[xx_DEF].log_file || opt_log[xx_ERR].log_file
			     || opt_log[xx_VIS].log_file || opt_log[xx_MSG].log_file
			     || opt_log[xx_FLD].log_file || opt_log[xx_INT].log_file; }
  
  static char log_dir[MAX_LEN];
  static char log_fname[MAX_LEN];
  static void init();
  static void open();
  static void close();
  static bool read_from_command_line(int argc, char **argv);
  static bool read_from_file(char*);
  static void readOptions(ValueParser *vp);

  static const int num_level= 3;
  static const int xx_DEF= 0;
  static const int xx_ERR= 1;
  static const int xx_VIS= 2;
  static const int xx_MSG= 3;
  static const int xx_FLD= 4;
  static const int xx_INT= 5;
  static const int xx_NUM= 6;
  static const char indent[xx_NUM][num_level][num_level+6]; 
  static int max_level;
  static OutOpt opt_log[xx_NUM];

  static ofstream file;
};

#endif

