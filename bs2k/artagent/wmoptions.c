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

#include "wmoptions.h"
#include "stdlib.h"
#include "string.h"

#include "valueparser.h"
#include "macro_msg.h"
#include <fstream>
#include <ctype.h>
#include <iomanip>
#include "server_options.h"

//long WMoptions::ms_time_max_wait_after_sense_body= 20;
long WMoptions::ms_time_max_wait_after_sense_body_long= 70;
long WMoptions::ms_time_max_wait_after_sense_body_short= 50;
long WMoptions::ms_time_max_wait_select_interval= -1;//10;
long WMoptions::s_time_normal_select_interval= 5;

int WMoptions::DUMMY= 0;
int WMoptions::EUMMY= 0;

bool WMoptions::offline;
bool WMoptions::foresee_opponents_positions;

bool WMoptions::use_fullstate_instead_of_see;
bool WMoptions::behave_after_fullstate;
bool WMoptions::behave_after_think;
bool WMoptions::disconnect_if_idle;
bool WMoptions::send_teamcomm;
bool WMoptions::recv_teamcomm;
bool WMoptions::ignore_fullstate;
bool WMoptions::ignore_sense_body;  
bool WMoptions::ignore_see;
bool WMoptions::ignore_hear;
bool WMoptions::use_joystick;
char WMoptions::joystick_device[MAX_NAME_LEN];
bool WMoptions::use_pfilter;

int WMoptions::max_cycles_to_forget_my_player= 30;
int WMoptions::max_cycles_to_forget_his_player= 60;

bool WMoptions::save_msg_times= false;
int WMoptions::his_goalie_number= 0;
bool WMoptions::use_aserver_ver_7_for_sending_commands= false;
/******************************************************************************/
void WMoptions::init_options() {
  //set_mode_test();
  set_mode_aserver();

  offline= false;
  foresee_opponents_positions= true;
  
  use_joystick= false;
  strcpy(joystick_device,"/dev/input/js0");
  use_pfilter= true;
  use_aserver_ver_7_for_sending_commands= false;
}

void WMoptions::read_options(char const* file, int argc, char const* const* argv) 
{
  ValueParser vp(file,"World_Model");

  if (argc > 1) {
    /* skip command name */
    argv++;argc--;


    vp.append_from_command_line(argc,argv,"wm_");
  }
  bool dum;

  vp.get("DUMMY",DUMMY);
  vp.get("EUMMY",EUMMY);

  vp.get("offline",offline);
  vp.get("foresee_opponents_positions",foresee_opponents_positions);
  
  //vp.set_verbose(true);
  dum= false;
  vp.get("aserver",dum); 
  if (dum) set_mode_aserver();

  dum= false;
  vp.get("test",dum);
  if (dum) set_mode_test();

  dum= ! disconnect_if_idle;
  vp.get("asynch",dum);
  disconnect_if_idle= ! dum;

  vp.get("use_pfilter",use_pfilter);

  dum= false;
  vp.get("synch_mode",dum);
  if (dum) set_mode_synch_mode();

  dum= false;
  vp.get("synch_mode_full",dum);
  if (dum) set_mode_synch_mode_with_fullstate();
  
  dum= send_teamcomm;
  vp.get("send_teamcomm",dum);
  if ( dum != send_teamcomm ) {
    send_teamcomm= dum;
    cout << "\nwm_send_teamcomm set to: " << send_teamcomm;
  }
  dum= recv_teamcomm;
  vp.get("recv_teamcomm",dum);
  if ( dum != recv_teamcomm ) {
    recv_teamcomm= dum;
    cout << "\nrecv_teamcomm set to: " << recv_teamcomm;
  }

  vp.get("ms_wait",ms_time_max_wait_after_sense_body_long);
  vp.get("ms_wait_short", ms_time_max_wait_after_sense_body_short);
  vp.get("s_wait", s_time_normal_select_interval);
  if ( vp.get("mta",max_cycles_to_forget_my_player) > 0) 
    cout << "\nmax_cycles_to_forget_my_player= " << max_cycles_to_forget_my_player << flush;  

  if ( vp.get("hta",max_cycles_to_forget_his_player) > 0)
    cout << "\nmax_cycles_to_forget_his_player= " << max_cycles_to_forget_his_player << flush;

  vp.get("save",save_msg_times);

  vp.get("send_v7",use_aserver_ver_7_for_sending_commands);
 
  vp.get("his_goalie_number", his_goalie_number);

  if ( vp.get("joystick",joystick_device,MAX_NAME_LEN) > 0 )
    use_joystick= true;

  vp.get("ms_wait_si",ms_time_max_wait_select_interval);
  
  if ( vp.num_of_not_accessed_entries() ) {
    ERROR_OUT << "\nInput: not recognized world model options (prefix wm_*):";
    vp.show_not_accessed_entries(ERROR_STREAM);
    ERROR_STREAM << "\nexiting ...\n";
    exit(1); //for the moment
  }
}

void WMoptions::print_options() {

}

#if 0
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

char *  BehaviorSet::tab[BehaviorSet::MAX_SIZE];
int BehaviorSet::size= 0;

bool BehaviorSet::read_line(char const* line) {
  while ( *line != '\0' ) {
    //std::cout << "\nline=" << line << flush;
    while (isspace(*line))
      line++;
   
    char const* beg= line;
    while ( ! isspace(*line) && *line != '\0')
      line++;
    //std::cout << "\nline2=" << line << flush;
    if ( beg!=line) {
      if (size >= MAX_SIZE ) {
	ERROR_OUT << "\nto much behaviours";
	return false;
      }
      int len= line-beg;
      tab[size]= new char[len+1];
      strncpy(tab[size],beg,len);
      tab[size][len]='\0';
      size++;
    }
  }
  return true;
}

bool BehaviorSet::init(char const* file, int argc, char const* const* argv) {
  bool res= true;
  size= 0;
  const char * block= "Behaviors";

  ifstream is(file);
  if (!is) { 
    cerr << "\nCannot open control file" << file;
    return false;
    //return;
  }

  char line[maxLineLength+1];
  char* act;
  int lineNo=0;
  //cerr << "\nParsing file \"" << fname << "\" with block [" << block << "] ";
  bool in_active_block= false;
  int block_size= 0;
  if (block) block_size= strlen(block);

  if (block_size == 0) //
    in_active_block= true;

  while (is) {
    is.getline(line,maxLineLength); lineNo++;
    
    act=line;
    while (isspace(*act)) act++; //strip leading whitespace

    // Check if comment
    if (*act=='#') continue; // OK, comment
    if (*act=='[') { // OK, recognizes [block]
      in_active_block= false;
      act++;
      //cout << "\n act= " << act << "\n blockdata()= " << block.data() << "\n block_size= " << block_size;
      if ( 0==strncmp(act, block, block_size) ) {
	act+= block_size;
	if ( ']'== *act )
	  in_active_block= true;
      }
      continue; 
    }

    if (!in_active_block)
      continue;

    if ( ! read_line(act) ) {
      res= false;
      break;
    }
  }

  is.close();
  show(std::cout);
  return res;
}

void BehaviorSet::show(ostream & out) {
  for (int i=0; i<size; i++) {
    out << "\n" << setw(2) << i << ". behavior= " << tab[i]; 
  }
}
#endif
