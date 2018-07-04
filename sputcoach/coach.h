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

/* Author: Manuel "Sputnick" Nickschas, 11/2001
 *
 * This file declares main components of SputCoach, including most of the network
 * communication.
 *
 */

#ifndef _COACH_H_
#define _COACH_H_

#include "defs.h"
#include "param.h"
#include "field.h"
#include "options.h"
#include "udpsocket.h"

extern Field fld;
extern bool onlineCoach;

namespace RUN {
  
    const int bufferMaxSize = UDPsocket::MAXMESG;
    extern UDPsocket sock;

    //const int side_NONE= 0;
    const int side_LEFT= 0;
    const int side_RIGHT= 1;
    extern int side;
    
    extern bool serverAlive;
    extern bool initDone;
    extern bool quit;

    bool init(int argc,char**argv);
    bool initPostConnect(int argc,char **argv);
    void cleanUp();
    long getCurrentMsTime(); // returns time in ms since first call to this routine
    void initOptions(int argc,char **argv);
    void initLogger(int argc,char **argv);
    bool initNetwork();
    void printPlayerTypes();
    void printPTCharacteristics();
    void announcePlayerChange(bool ownTeam,int unum,int type=-1);
    void mainLoop(int argc, char **argv);
};


#endif
