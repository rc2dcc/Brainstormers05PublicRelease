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

/** \file types.h 
This file contains and \b should contain all \b typedefs, \b defines, \b enumerations and such that affect more than one file.
*/

#ifndef _BS2K_TYPES_H_
#define _BS2K_TYPES_H_

#include <math.h>

#if 0 
/* an Alex von Artur: ich hab' all das auskommentiert, da es im Client nicht
gebraucht wird, da aehnliches bereits in globaldef.h definiert ist (insbesondere
View Quality und Width).

Das was Du im deinem Weltmodell brauchst, solltes Du auch dort defnieren (zB.
SIDE_LEFT, Kick_Para etc.

*/

typedef enum View_Width_Enum{NARROW, NORMAL, WIDE} View_Width;
typedef enum View_Quality_Enum{HIGH, LOW} View_Quality;

/* We define Parameter structs for the actions for sake of easier handling. */
typedef struct Kick_Para{
  double power;
  double direction;
} Kick_Parameter;
typedef struct Turn_Para{
  double moment;
} Turn_Parameter;
typedef struct Dash_Para{
  double power;
} Dash_Parameter;
typedef struct Catch_Para{
  double direction;
} Catch_Parameter;
typedef struct Move_Para{
  double x;
  double y;
} Move_Parameter;
typedef struct Turn_Neck_Para{
  double moment;
} Turn_Neck_Parameter;
typedef struct Change_View_Para{
  View_Width view_width;
  View_Quality view_quality;
} Change_View_Parameter;
typedef struct Say_Para{
  char *message;
} Say_Parameter;

#ifndef Bool
#define Bool bool
#endif



/* identifiers from which side we are playing. 
Needed in the new update !
*/

#define SIDE_LEFT 1
#define SIDE_RIGHT -1
#endif

#endif
