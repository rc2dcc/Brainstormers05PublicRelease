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

#include "attack_move1_wb_bmp.h"
#include "ws_info.h"
#include "ws_memory.h"
#include "log_macros.h"
#include "mdp_info.h"
#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "valueparser.h"
#include "options.h"
#include "log_macros.h"
#include "geometry2d.h"

#if 1
#define DBLOG_POL(LLL,XXX) LOG_POL(LLL,<<"ATTACK_MOVE1_WB: "XXX)
#define DBLOG_DRAW(LLL,XXX) LOG_POL(LLL,<<_2D <<XXX)
#define DBLOG_ERR(LLL,XXX) LOG_ERR(LLL,XXX)
#define MYGETTIME (Tools::get_current_ms_time())
#else
#define DBLOG_POL(LLL,XXX)
#define DBLOG_DRAW(LLL,XXX)
#define DBLOG_ERR(LLL,XXX) 
#define MYGETTIME (0)
#endif


/* constructor method */
Attack_Move1_Wb::Attack_Move1_Wb() {
  ValueParser vp(CommandLineOptions::policy_conf,"attack_move1_wb");
}

Attack_Move1_Wb::~Attack_Move1_Wb() {
}


bool Attack_Move1_Wb::do_move(){
  DBLOG_POL(0, << "check attack move 1 ");
  return false;
}



bool Attack_Move1_Wb::get_cmd(Cmd & cmd) {
  DBLOG_POL(0, << "get_cmd");
  return false;
}


