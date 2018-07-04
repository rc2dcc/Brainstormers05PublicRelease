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

typedef union {
  double dval;
  int ival;
  char *sval;
} YYSTYPE;
#define	SI_ERROR	258
#define	UNKNOWN_COMMAND	259
#define	ILLEGAL_COMMAND_FORM	260
#define	OB	261
#define	CB	262
#define	SENSE_BODY	263
#define	SEE	264
#define	HEAR	265
#define	SI_INIT	266
#define	REFEREE	267
#define	SELF	268
#define	GOAL	269
#define	FLAG	270
#define	LINE	271
#define	BALL	272
#define	PLAYER	273
#define	_L_	274
#define	_R_	275
#define	_T_	276
#define	_B_	277
#define	_C_	278
#define	_P_	279
#define	_G_	280
#define	DOUBLE	281
#define	DIRECTION	282
#define	INT	283
#define	UNUM	284
#define	SIDE	285
#define	PMODE	286
#define	VIEW_QUALITY	287
#define	VIEW_WIDTH	288
#define	WHICHTEAM	289
#define	STRING	290
#define	VIEW_MODE	291
#define	STAMINA	292
#define	SPEED	293
#define	HEAD_ANGLE	294
#define	SI_KICK	295
#define	SI_DASH	296
#define	SI_TURN	297
#define	SI_SAY	298
#define	SI_TURN_NECK	299
#define	GOALIE	300
#define	MDP	301
#define	SI_SCORE	302


extern YYSTYPE silval;
