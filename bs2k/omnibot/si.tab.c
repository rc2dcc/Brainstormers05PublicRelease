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


/*  A Bison parser, made from sensor_parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse siparse
#define yylex silex
#define yyerror sierror
#define yylval silval
#define yychar sichar
#define yydebug sidebug
#define yynerrs sinerrs
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

#line 1 "sensor_parser.y"


  /**
     \file sensor_parser.y
     This is the general parser for all information coming from the server.
     If you want to add additional functionality (e.g. communication between
     clients, simply add your grammar and actions. Maybe you will have 
     to modify sensor_lexer.lex.

     invoke bison at least with options : -b si -p si
     @author (of an earlier version) Alex Sinner
     @author Artur Merke
  */
#include <stdio.h>  
#include <stdlib.h>  
#include <iostream.h>

  /** This file is needed for information about variable flags */
#include "options.h"
  /** This file contains the SensorBuffer class */
#include "sensorbuffer.h"
  /** YYPARSE_PARAM is the argument passed to spparse(). */
#define YYPARSE_PARAM sensor_buffer
  //#define YYDEBUG 1
  
  int sierror(char* s){
    cerr << "\nParser: ERROR: >>" << s << "<<";
    char ch; cin >> ch;
    return 0;
  }

  extern int silex();
  //  extern YY_BUFFER_STATE si_scan_string(char *);


#line 36 "sensor_parser.y"
typedef union {
  double dval;
  int ival;
  char *sval;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		195
#define	YYFLAG		-32768
#define	YYNTBASE	48

#define YYTRANSLATE(x) ((unsigned)(x) <= 302 ? yytranslate[x] : 75)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     6,     8,    10,    12,    14,    16,    18,    23,
    24,    29,    33,    38,    40,    42,    44,    46,    48,    60,
    66,    70,    72,    76,    77,    82,    83,    88,    90,    92,
    94,    96,   104,   110,   118,   125,   131,   135,   143,   149,
   157,   163,   168,   174,   179,   184,   188,   195,   201,   206,
   210,   213,   216,   219,   223,   227,   231,   235,   239,   243,
   248,   253,   258,   263,   268,   273,   278,   283,   288,   293,
   298,   303,   308,   313,   317,   321,   325,   329,   334,   339,
   344,   349,   351,   353,   356,   359,   362,   365,   366,   373,
   380,   387,   393,   399,   405,   411,   417,   423
};

static const short yyrhs[] = {    -1,
     6,    49,     7,    48,     0,    60,     0,    68,     0,    69,
     0,     1,     0,    50,     0,    51,     0,    11,    30,    28,
    31,     0,     0,    46,    28,    52,    53,     0,     6,    54,
     7,     0,     6,    54,     7,    53,     0,    55,     0,    56,
     0,    57,     0,    58,     0,    59,     0,    30,    29,    26,
    26,    26,    26,    26,    26,    26,    26,    26,     0,    17,
    26,    26,    26,    26,     0,    36,    32,    33,     0,    31,
     0,    47,    28,    28,     0,     0,     9,    28,    61,    62,
     0,     0,     6,    63,     7,    62,     0,    66,     0,    65,
     0,    64,     0,    67,     0,     6,    17,     7,    26,    26,
    26,    26,     0,     6,    17,     7,    26,    26,     0,    70,
    26,    26,    26,    26,    26,    26,     0,    70,    26,    26,
    26,    26,    26,     0,    70,    26,    26,    26,    26,     0,
    70,    26,    26,     0,     6,    71,     7,    26,    26,    26,
    26,     0,     6,    71,     7,    26,    26,     0,     6,    72,
     7,    26,    26,    26,    26,     0,     6,    72,     7,    26,
    26,     0,    10,    28,    12,    31,     0,    10,    28,    12,
    31,    28,     0,    10,    28,    28,    74,     0,    10,    28,
    13,    74,     0,     8,    28,    73,     0,     6,    18,    34,
    29,    45,     7,     0,     6,    18,    34,    29,     7,     0,
     6,    18,    34,     7,     0,     6,    18,     7,     0,    14,
    19,     0,    14,    20,     0,    15,    23,     0,    15,    19,
    21,     0,    15,    20,    21,     0,    15,    23,    21,     0,
    15,    19,    22,     0,    15,    20,    22,     0,    15,    23,
    22,     0,    15,    24,    19,    21,     0,    15,    24,    20,
    21,     0,    15,    24,    19,    23,     0,    15,    24,    20,
    23,     0,    15,    24,    19,    22,     0,    15,    24,    20,
    22,     0,    15,    21,    19,    28,     0,    15,    21,    20,
    28,     0,    15,    22,    19,    28,     0,    15,    22,    20,
    28,     0,    15,    19,    21,    28,     0,    15,    19,    22,
    28,     0,    15,    20,    21,    28,     0,    15,    20,    22,
    28,     0,    15,    19,    28,     0,    15,    20,    28,     0,
    15,    21,    28,     0,    15,    22,    28,     0,    15,    25,
    19,    21,     0,    15,    25,    20,    21,     0,    15,    25,
    19,    22,     0,    15,    25,    20,    22,     0,    15,     0,
    14,     0,    16,    19,     0,    16,    20,     0,    16,    21,
     0,    16,    22,     0,     0,    73,     6,    36,    32,    33,
     7,     0,    73,     6,    37,    26,    26,     7,     0,    73,
     6,    38,    26,    26,     7,     0,    73,     6,    39,    26,
     7,     0,    73,     6,    40,    28,     7,     0,    73,     6,
    41,    28,     7,     0,    73,     6,    42,    28,     7,     0,
    73,     6,    43,    28,     7,     0,    73,     6,    44,    28,
     7,     0,    35,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    73,    74,    77,    78,    79,    80,    81,    82,    85,    92,
    96,    98,    99,   102,   103,   104,   105,   106,   109,   131,
   139,   145,   150,   163,   170,   172,   173,   176,   177,   178,
   179,   182,   195,   210,   226,   242,   258,   276,   290,   306,
   319,   334,   340,   355,   358,   363,   368,   377,   386,   395,
   406,   415,   425,   434,   443,   452,   461,   470,   479,   488,
   497,   506,   515,   524,   533,   542,   551,   560,   569,   578,
   587,   596,   605,   614,   623,   632,   641,   650,   659,   668,
   677,   686,   695,   706,   714,   722,   730,   740,   741,   745,
   749,   754,   758,   762,   766,   768,   770,   775
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SI_ERROR",
"UNKNOWN_COMMAND","ILLEGAL_COMMAND_FORM","OB","CB","SENSE_BODY","SEE","HEAR",
"SI_INIT","REFEREE","SELF","GOAL","FLAG","LINE","BALL","PLAYER","_L_","_R_",
"_T_","_B_","_C_","_P_","_G_","DOUBLE","DIRECTION","INT","UNUM","SIDE","PMODE",
"VIEW_QUALITY","VIEW_WIDTH","WHICHTEAM","STRING","VIEW_MODE","STAMINA","SPEED",
"HEAD_ANGLE","SI_KICK","SI_DASH","SI_TURN","SI_SAY","SI_TURN_NECK","GOALIE",
"MDP","SI_SCORE","start","server_message","init","mdpstate","@1","mdp_objects",
"mdp_object","mdp_player","mdp_ball","mdp_vmode","mdp_pmode","mdp_score","see",
"@2","see_objects","see_object","see_ball","see_player","see_marker","see_line",
"hear","sense_body","see_player_object","see_marker_object","see_line_object",
"sbi","communication", NULL
};
#endif

static const short yyr1[] = {     0,
    48,    48,    49,    49,    49,    49,    49,    49,    50,    52,
    51,    53,    53,    54,    54,    54,    54,    54,    55,    56,
    57,    58,    59,    61,    60,    62,    62,    63,    63,    63,
    63,    64,    64,    65,    65,    65,    65,    66,    66,    67,
    67,    68,    68,    68,    68,    69,    70,    70,    70,    70,
    71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    72,    72,    72,    72,    73,    73,    73,
    73,    73,    73,    73,    73,    73,    73,    74
};

static const short yyr2[] = {     0,
     0,     4,     1,     1,     1,     1,     1,     1,     4,     0,
     4,     3,     4,     1,     1,     1,     1,     1,    11,     5,
     3,     1,     3,     0,     4,     0,     4,     1,     1,     1,
     1,     7,     5,     7,     6,     5,     3,     7,     5,     7,
     5,     4,     5,     4,     4,     3,     6,     5,     4,     3,
     2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     3,     3,     3,     3,     4,     4,     4,
     4,     1,     1,     2,     2,     2,     2,     0,     6,     6,
     6,     5,     5,     5,     5,     5,     5,     1
};

static const short yydefact[] = {     1,
     0,     6,     0,     0,     0,     0,     0,     0,     7,     8,
     3,     4,     5,    88,    24,     0,     0,    10,     1,    46,
    26,     0,     0,     0,     0,     0,     2,     0,     0,    25,
    42,    98,    45,    44,     9,     0,    11,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    30,    29,
    28,    31,     0,    43,     0,     0,    22,     0,     0,     0,
    14,    15,    16,    17,    18,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    83,    82,     0,     0,     0,     0,
     0,    26,     0,     0,     0,     0,     0,    12,     0,     0,
     0,    92,    93,    94,    95,    96,    97,    51,    52,     0,
     0,     0,     0,    53,     0,     0,    84,    85,    86,    87,
     0,    50,     0,     0,     0,    27,    37,     0,     0,    21,
    23,    13,    89,    90,    91,    54,    57,    74,    55,    58,
    75,     0,     0,    76,     0,     0,    77,    56,    59,     0,
     0,     0,     0,     0,    49,     0,     0,     0,     0,     0,
     0,    70,    71,    72,    73,    66,    67,    68,    69,    60,
    64,    62,    61,    65,    63,    78,    80,    79,    81,    33,
    48,     0,    39,    41,    36,    20,     0,     0,    47,     0,
     0,    35,     0,    32,    38,    40,    34,     0,     0,     0,
     0,    19,     0,     0,     0
};

static const short yydefgoto[] = {    27,
     8,     9,    10,    26,    37,    60,    61,    62,    63,    64,
    65,    11,    21,    30,    48,    49,    50,    51,    52,    12,
    13,    53,    80,    81,    20,    33
};

static const short yypact[] = {    21,
    -1,-32768,   -17,     1,    13,    45,    52,    74,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,     0,    56,-32768,    21,    76,
    77,    58,    50,    50,    59,    80,-32768,    10,    81,-32768,
    60,-32768,-32768,-32768,-32768,   -15,-32768,    61,    65,    66,
    68,    67,    69,    70,    71,    72,    48,    89,-32768,-32768,
-32768,-32768,    75,-32768,    78,    73,-32768,    79,    82,    96,
-32768,-32768,-32768,-32768,-32768,    83,    86,    87,    98,    99,
   100,   101,   102,   107,     5,    36,    -2,   108,    -4,   110,
   111,    77,    93,    94,    95,    90,    97,    80,   115,   117,
   119,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    12,
    14,   -14,     3,    16,    24,    54,-32768,-32768,-32768,-32768,
   103,-32768,    -3,   104,   105,-32768,   106,   109,   112,-32768,
-32768,-32768,-32768,-32768,-32768,   113,   114,-32768,   116,   118,
-32768,   120,   121,-32768,   122,   123,-32768,-32768,-32768,    46,
    49,    55,    57,   126,-32768,    -6,   127,   128,   129,   130,
   131,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   132,
-32768,   133,   134,   135,   136,-32768,   137,   138,-32768,   139,
   140,   141,   142,-32768,-32768,-32768,-32768,   143,   144,   145,
   146,-32768,   147,   159,-32768
};

static const short yypgoto[] = {   173,
-32768,-32768,-32768,-32768,    39,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    51,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   150
};


#define	YYLAST		174


static const short yytable[] = {     2,
   171,    55,   112,   145,   132,   133,     3,     4,     5,     6,
    14,    22,    23,   134,    56,    57,   107,   108,   109,   110,
    58,   135,   136,    98,    99,   146,     1,    24,    15,   113,
   137,    59,   126,   127,   129,   130,   138,   139,   172,   128,
    16,   131,   140,   141,     7,    38,    39,    40,    41,    42,
    43,    44,    45,    46,   100,   101,   102,   103,   104,   105,
   106,    75,    76,    77,    78,    79,   160,   161,   162,   163,
   164,   165,   142,   143,    17,   166,   167,   168,   169,    18,
    19,    28,    29,    25,    32,    36,    47,    54,    31,    35,
    67,    68,    66,    69,    70,    82,    71,    72,    73,    74,
    83,    85,    88,    84,    92,    93,    94,    95,    96,    87,
    86,    90,    91,    97,   111,    89,   114,   115,   117,   118,
   119,   123,   120,   124,   121,   125,   122,     0,   144,   147,
   148,   149,   116,     0,   150,     0,     0,   151,     0,   179,
   152,   153,     0,   154,     0,   155,   194,   156,   157,   158,
   159,   170,   173,   174,   175,   176,   177,   178,   195,   180,
   181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
   191,   192,   193,    34
};

static const short yycheck[] = {     1,
     7,    17,     7,     7,    19,    20,     8,     9,    10,    11,
    28,    12,    13,    28,    30,    31,    19,    20,    21,    22,
    36,    19,    20,    19,    20,    29,     6,    28,    28,    34,
    28,    47,    21,    22,    21,    22,    21,    22,    45,    28,
    28,    28,    19,    20,    46,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    19,    20,    21,    22,    23,    24,
    25,    14,    15,    16,    17,    18,    21,    22,    23,    21,
    22,    23,    19,    20,    30,    21,    22,    21,    22,    28,
     7,     6,     6,    28,    35,     6,     6,    28,    31,    31,
    26,    26,    32,    26,    28,     7,    28,    28,    28,    28,
    26,    29,     7,    26,     7,     7,     7,     7,     7,    28,
    32,    26,    26,     7,     7,    33,     7,     7,    26,    26,
    26,     7,    33,     7,    28,     7,    88,    -1,    26,    26,
    26,    26,    82,    -1,    26,    -1,    -1,    26,    -1,     7,
    28,    28,    -1,    28,    -1,    28,     0,    28,    28,    28,
    28,    26,    26,    26,    26,    26,    26,    26,     0,    26,
    26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
    26,    26,     0,    24
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
YYPARSE_RETURN_TYPE yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

YYPARSE_RETURN_TYPE
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 73 "sensor_parser.y"
{;;
    break;}
case 2:
#line 74 "sensor_parser.y"
{;;
    break;}
case 3:
#line 77 "sensor_parser.y"
{;;
    break;}
case 4:
#line 78 "sensor_parser.y"
{;;
    break;}
case 5:
#line 79 "sensor_parser.y"
{;;
    break;}
case 6:
#line 80 "sensor_parser.y"
{;;
    break;}
case 7:
#line 81 "sensor_parser.y"
{;;
    break;}
case 8:
#line 82 "sensor_parser.y"
{;;
    break;}
case 9:
#line 85 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->init->side = yyvsp[-2].ival;
  ((SensorBuffer *)sensor_buffer)->init->number = yyvsp[-1].ival;
  ((SensorBuffer *)sensor_buffer)->init->play_mode = PlayMode(yyvsp[0].ival);
;
    break;}
case 10:
#line 92 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->mdpstate->time = yyvsp[0].ival;
  ((SensorBuffer *)sensor_buffer)->mdpstate->players_num = 0;
;
    break;}
case 12:
#line 98 "sensor_parser.y"
{;;
    break;}
case 13:
#line 99 "sensor_parser.y"
{;;
    break;}
case 14:
#line 102 "sensor_parser.y"
{;;
    break;}
case 15:
#line 103 "sensor_parser.y"
{;;
    break;}
case 16:
#line 104 "sensor_parser.y"
{;;
    break;}
case 17:
#line 105 "sensor_parser.y"
{;;
    break;}
case 18:
#line 106 "sensor_parser.y"
{;;
    break;}
case 19:
#line 109 "sensor_parser.y"
{
  int & num= ((SensorBuffer *)sensor_buffer)->mdpstate->players_num;
  if ( num < Msg_mdpstate::players_MAX) {
    Msg_mdpstate::_mdp_player & p= ((SensorBuffer *)sensor_buffer)->mdpstate->players[num];
    p.team= yyvsp[-10].ival;
    p.number= yyvsp[-9].ival;
    p.x= yyvsp[-8].dval;
    p.y= yyvsp[-7].dval;
    p.vel_x= yyvsp[-6].dval;
    p.vel_y= yyvsp[-5].dval;
    p.angle= yyvsp[-4].dval;
    p.neck_angle= yyvsp[-3].dval;
    p.stamina= yyvsp[-2].dval;
    p.effort= yyvsp[-1].dval;
    p.recovery= yyvsp[0].dval;
    num++;
  }
  else
    cerr << "\nParser: to much (mdp)players";
;
    break;}
case 20:
#line 131 "sensor_parser.y"
{
  Msg_mdpstate::_mdp_ball & b= ((SensorBuffer *)sensor_buffer)->mdpstate->ball;
  b.x= yyvsp[-3].dval;
  b.y= yyvsp[-2].dval;
  b.vel_x= yyvsp[-1].dval;
  b.vel_y= yyvsp[0].dval;
;
    break;}
case 21:
#line 139 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->mdpstate->view_quality = yyvsp[-1].ival;
  ((SensorBuffer *)sensor_buffer)->mdpstate->view_width = yyvsp[0].ival;
;
    break;}
case 22:
#line 145 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->mdpstate->play_mode = PlayMode(yyvsp[0].ival);
;
    break;}
case 23:
#line 150 "sensor_parser.y"
{
  Msg_mdpstate & mdp= *(((SensorBuffer *)sensor_buffer)->mdpstate);
  if ( WM::my_side == left_SIDE) {
    mdp.my_score= yyvsp[-1].ival;
    mdp.his_score= yyvsp[0].ival;
  }
  else {
    mdp.his_score= yyvsp[-1].ival;
    mdp.my_score= yyvsp[0].ival;
  }
;
    break;}
case 24:
#line 163 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->see->time = yyvsp[0].ival;
  ((SensorBuffer *)sensor_buffer)->see->markers_num = 0;
  ((SensorBuffer *)sensor_buffer)->see->players_num = 0;
  ((SensorBuffer *)sensor_buffer)->see->line_upd = false;
  ((SensorBuffer *)sensor_buffer)->see->ball_upd= false;
;
    break;}
case 26:
#line 172 "sensor_parser.y"
{;;
    break;}
case 27:
#line 173 "sensor_parser.y"
{;;
    break;}
case 28:
#line 176 "sensor_parser.y"
{;;
    break;}
case 29:
#line 177 "sensor_parser.y"
{;;
    break;}
case 30:
#line 178 "sensor_parser.y"
{;;
    break;}
case 31:
#line 179 "sensor_parser.y"
{;;
    break;}
case 32:
#line 182 "sensor_parser.y"
{
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.ball_upd) {
    see.ball.how_many= 4;
    see.ball.dist = yyvsp[-3].dval;
    see.ball.dir = yyvsp[-2].dval;
    see.ball.dist_change = yyvsp[-1].dval;
    see.ball.dir_change = yyvsp[0].dval;
    see.ball_upd= true;
  }
  else
    cerr << "\nParser: more then one ball";
;
    break;}
case 33:
#line 195 "sensor_parser.y"
{
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.ball_upd) {
    see.ball.how_many= 2;
    see.ball.dist = yyvsp[-1].dval;
    see.ball.dir = yyvsp[0].dval;
    see.ball.dist_change = UNDEF_INFORMATION;
    see.ball.dir_change = UNDEF_INFORMATION;
    see.ball_upd= true;
  }
  else
    cerr << "\nParser: more then one ball";
;
    break;}
case 34:
#line 210 "sensor_parser.y"
{
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 6;
    p.dist = yyvsp[-5].dval;
    p.dir = yyvsp[-4].dval;
    p.dist_change = yyvsp[-3].dval;
    p.dir_change = yyvsp[-2].dval;
    p.body_dir = yyvsp[-1].dval;
    p.head_dir = yyvsp[0].dval;
    num++;
  }
  else
    cerr << "\nParser: to much players";
;
    break;}
case 35:
#line 226 "sensor_parser.y"
{
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 5;
    p.dist = yyvsp[-4].dval;
    p.dir = yyvsp[-3].dval;
    p.dist_change = yyvsp[-2].dval;
    p.dir_change = yyvsp[-1].dval;
    p.body_dir = yyvsp[0].dval;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
;
    break;}
case 36:
#line 242 "sensor_parser.y"
{
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 4;
    p.dist = yyvsp[-3].dval;
    p.dir = yyvsp[-2].dval;
    p.dist_change = yyvsp[-1].dval;
    p.dir_change = yyvsp[0].dval;
    p.body_dir = UNDEF_INFORMATION;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
;
    break;}
case 37:
#line 258 "sensor_parser.y"
{
  int & num= ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.how_many= 2;
    p.dist = yyvsp[-1].dval;
    p.dir = yyvsp[0].dval;
    p.dist_change = UNDEF_INFORMATION;
    p.dir_change = UNDEF_INFORMATION;
    p.body_dir = UNDEF_INFORMATION;
    p.head_dir = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much players";
;
    break;}
case 38:
#line 276 "sensor_parser.y"
{
  int & num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.how_many= 4;
    m.dist = yyvsp[-3].dval;
    m.dir = yyvsp[-2].dval;
    m.dist_change = yyvsp[-1].dval;
    m.dir_change = yyvsp[0].dval;
    num++;
  }
  else
    cerr << "\nParser: to much markers";
;
    break;}
case 39:
#line 290 "sensor_parser.y"
{
  int & num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.how_many= 2;
    m.dist = yyvsp[-1].dval;
    m.dir = yyvsp[0].dval;
    m.dist_change = UNDEF_INFORMATION;
    m.dir_change = UNDEF_INFORMATION;
    num++;
  }
  else
    cerr << "\nParser: to much markers";
;
    break;}
case 40:
#line 306 "sensor_parser.y"
{
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.line_upd) {
    see.line.how_many= 4;
    see.line.dist = yyvsp[-3].dval;
    see.line.dir = yyvsp[-2].dval;
    see.line.dist_change = yyvsp[-1].dval;
    see.line.dir_change = yyvsp[0].dval;
    see.line_upd= true;
  }
  else
    cerr << "\nParser: more then one line";
;
    break;}
case 41:
#line 319 "sensor_parser.y"
{
  Msg_see & see= *(((SensorBuffer *)sensor_buffer)->see);
  if (!see.line_upd) {
    see.line.how_many= 2;
    see.line.dist = yyvsp[-1].dval;
    see.line.dir = yyvsp[0].dval;
    see.line.dist_change = UNDEF_INFORMATION;
    see.line.dir_change = UNDEF_INFORMATION;
    see.line_upd= true;
  }
  else
    cerr << "\nParser: more then one line";
;
    break;}
case 42:
#line 334 "sensor_parser.y"
{
  Msg_hear & hear= *(((SensorBuffer *)sensor_buffer)->hear);
  hear.time= yyvsp[-2].ival;
  hear.play_mode_upd= true;
  hear.play_mode = PlayMode(yyvsp[0].ival);
;
    break;}
case 43:
#line 340 "sensor_parser.y"
{
  Msg_hear & hear= *(((SensorBuffer *)sensor_buffer)->hear);
  hear.time= yyvsp[-3].ival;
  hear.play_mode_upd= true;
  hear.play_mode = PlayMode(yyvsp[-1].ival);
  if (hear.play_mode == PM_goal_l)
    if ( WM::my_side == left_SIDE) {
      hear.my_score_upd= true;
      hear.my_score= yyvsp[0].ival;
    }
    else {
      hear.his_score_upd= true;
      hear.his_score= yyvsp[0].ival;
    }
;
    break;}
case 44:
#line 355 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->hear->time= yyvsp[-2].ival;
;
    break;}
case 45:
#line 358 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->hear->time= yyvsp[-2].ival;
;
    break;}
case 46:
#line 363 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->time = yyvsp[-1].ival;
;
    break;}
case 47:
#line 368 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= yyvsp[-3].ival;
    p.number= yyvsp[-2].ival;
    p.goalie= true;
  }
;
    break;}
case 48:
#line 377 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= yyvsp[-2].ival;
    p.number= yyvsp[-1].ival;
    p.goalie= false;
  }
;
    break;}
case 49:
#line 386 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= yyvsp[-1].ival;
    p.number= UNDEF_INFORMATION;
    p.goalie= false;
  }
;
    break;}
case 50:
#line 395 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->players_num;
  if ( num < Msg_see::players_MAX) {
    Msg_see::_see_player & p= ((SensorBuffer *)sensor_buffer)->see->players[num];
    p.team= unknown_TEAM;
    p.number= UNDEF_INFORMATION;
    p.goalie= false;
  }
;
    break;}
case 51:
#line 406 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = 0;
  }
;
    break;}
case 52:
#line 415 "sensor_parser.y"
{
  //cerr << "<goal r>";
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = 0;
  }
;
    break;}
case 53:
#line 425 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0.0;
    m.y = 0.0;
  }
;
    break;}
case 54:
#line 434 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = PITCH_WIDTH/2.0;
  }
;
    break;}
case 55:
#line 443 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = PITCH_WIDTH/2.0;
  }
;
    break;}
case 56:
#line 452 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0;
    m.y = PITCH_WIDTH/2.0;
  }
;
    break;}
case 57:
#line 461 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0;
    m.y = -PITCH_WIDTH/2.0;
  }
;
    break;}
case 58:
#line 470 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0;
    m.y = -PITCH_WIDTH/2.0;
  }
;
    break;}
case 59:
#line 479 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = 0;
    m.y = -PITCH_WIDTH/2.0;
  }
;
    break;}
case 60:
#line 488 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = PENALTY_AREA_WIDTH/2.0;
  }
;
    break;}
case 61:
#line 497 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = PENALTY_AREA_WIDTH/2.0;
  }
;
    break;}
case 62:
#line 506 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = 0.0;
  }
;
    break;}
case 63:
#line 515 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = 0.0;
  }
;
    break;}
case 64:
#line 524 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2.0+PENALTY_AREA_LENGTH;
    m.y = -PENALTY_AREA_WIDTH/2.0;
  }
;
    break;}
case 65:
#line 533 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH;
    m.y = -PENALTY_AREA_WIDTH/2.0;
  }
;
    break;}
case 66:
#line 542 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(-yyvsp[0].ival);
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
;
    break;}
case 67:
#line 551 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(yyvsp[0].ival);
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
;
    break;}
case 68:
#line 560 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(-yyvsp[0].ival);
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
;
    break;}
case 69:
#line 569 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = double(yyvsp[0].ival);
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
;
    break;}
case 70:
#line 578 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = double(yyvsp[0].ival);
  }
;
    break;}
case 71:
#line 587 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = double(-yyvsp[0].ival);
  }
;
    break;}
case 72:
#line 596 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = double(yyvsp[0].ival);
  }
;
    break;}
case 73:
#line 605 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = double(-yyvsp[0].ival);
  }
;
    break;}
case 74:
#line 614 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = -PITCH_LENGTH/2 - PITCH_MARGIN;
    m.y = 0.0;
  }
;
    break;}
case 75:
#line 623 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2 + PITCH_MARGIN;
    m.y = 0.0;
  }
;
    break;}
case 76:
#line 632 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = 0.0;
    m.y = PITCH_WIDTH/2 + PITCH_MARGIN;
  }
;
    break;}
case 77:
#line 641 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = 0.0;
    m.y = -PITCH_WIDTH/2 - PITCH_MARGIN;
  }
;
    break;}
case 78:
#line 650 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true;
    m.x = -PITCH_LENGTH/2;
    m.y = ServerOptions::goal_width/2;
  }
;
    break;}
case 79:
#line 659 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2;
    m.y = ServerOptions::goal_width/2;
  }
;
    break;}
case 80:
#line 668 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = -PITCH_LENGTH/2;
    m.y = -ServerOptions::goal_width/2;
  }
;
    break;}
case 81:
#line 677 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= true; 
    m.x = PITCH_LENGTH/2;
    m.y = -ServerOptions::goal_width/2;
  }
;
    break;}
case 82:
#line 686 "sensor_parser.y"
{
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= false; 
    m.x = UNDEF_INFORMATION;
    m.y = UNDEF_INFORMATION;
  }
;
    break;}
case 83:
#line 695 "sensor_parser.y"
{ 
  int num = ((SensorBuffer *)sensor_buffer)->see->markers_num;
  if ( num < Msg_see::markers_MAX) {
    Msg_see::_see_marker & m= ((SensorBuffer *)sensor_buffer)->see->markers[num];
    m.see_position= false;
    m.x = UNDEF_INFORMATION;
    m.y = UNDEF_INFORMATION;
  }
;
    break;}
case 84:
#line 706 "sensor_parser.y"
{
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = -PITCH_LENGTH/2;
    l.y = 0.0;
  }
;
    break;}
case 85:
#line 714 "sensor_parser.y"
{
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = PITCH_LENGTH/2;
    l.y = 0.0;
  }
;
    break;}
case 86:
#line 722 "sensor_parser.y"
{
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = 0.0;
    l.y = PITCH_WIDTH/2;
  }
;
    break;}
case 87:
#line 730 "sensor_parser.y"
{
  bool upd = ((SensorBuffer *)sensor_buffer)->see->line_upd;
  if ( !upd ) {
    Msg_see::_see_line & l= ((SensorBuffer *)sensor_buffer)->see->line;
    l.x = 0.0;
    l.y = -PITCH_WIDTH/2;
  }
;
    break;}
case 88:
#line 740 "sensor_parser.y"
{;;
    break;}
case 89:
#line 741 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->view_quality = yyvsp[-2].ival;
  ((SensorBuffer *)sensor_buffer)->sb->view_width = yyvsp[-1].ival;
;
    break;}
case 90:
#line 745 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->stamina = yyvsp[-2].dval;
  ((SensorBuffer *)sensor_buffer)->sb->effort = yyvsp[-1].dval;
;
    break;}
case 91:
#line 750 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->speed_value = yyvsp[-2].dval;
  ((SensorBuffer *)sensor_buffer)->sb->speed_angle = yyvsp[-1].dval;
;
    break;}
case 92:
#line 755 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->neck_angle = yyvsp[-1].dval;
;
    break;}
case 93:
#line 759 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->kick_count = yyvsp[-1].ival;
;
    break;}
case 94:
#line 763 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->sb->dash_count = yyvsp[-1].ival;
;
    break;}
case 95:
#line 767 "sensor_parser.y"
{((SensorBuffer *)sensor_buffer)->sb->turn_count = yyvsp[-1].ival;;
    break;}
case 96:
#line 769 "sensor_parser.y"
{((SensorBuffer *)sensor_buffer)->sb->say_count = yyvsp[-1].ival;;
    break;}
case 97:
#line 771 "sensor_parser.y"
{((SensorBuffer *)sensor_buffer)->sb->turn_neck_count = yyvsp[-1].ival;;
    break;}
case 98:
#line 775 "sensor_parser.y"
{
  ((SensorBuffer *)sensor_buffer)->hear->communication_upd= true;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 780 "sensor_parser.y"

#include "lex.si.c"

void parse_sensor_data(char* argv, void *sensor_buffer){
  si_scan_string(argv);
  if (siparse(sensor_buffer)){
    cerr << "\n parse_sensor_data error !";
  }
}


#if 0
int main(int argc,char ** argv) {
  WM::my_side= right_SIDE;
  strcpy(WM::my_team_name,"my team");
  SensorBuffer sb;
  cout << "\nbegin:";
  if (0) {
    char buf_1[]= "(see     1)";
    char buf_2[]= "(see 1)";
    char buf[] = "(sense_body 217 (view_mode high normal) (stamina 2500.22 1) (speed 0.7) (head_angle 45) (kick 10) (dash 11) (turn 12) (say 13) (turn_neck 14))";
    char buf2[]= "(see 498 ((goal r) 58.6 0) ((flag c) 6 0 -0 0) ((flag r t) 67.4 -30) ((flag r b) 67.4 30) ((flag p r t) 46.5 -25) ((flag p r c) 42.1 0) ((flag p r b) 46.5 25) ((flag g r t) 59.1 -6) ((flag g r b) 59.1 6) ((flag t r 40) 60.3 -40) ((flag t r 50) 68 -34) ((flag b r 40) 60.3 40) ((flag b r 50) 68 34) ((flag r t 30) 70.1 -25) ((flag r t 20) 66.7 -17) ((flag r t 10) 64.1 -8) ((flag r 0) 63.4 0) ((flag r b 10) 64.1 8) ((flag r b 20) 66.7 17) ((flag r b 30) 70.1 25) ((ball) 10 -2 0 0) ((line r) 58.6 -89))";
    char buf3[]= "(hear 3000 referee before_kick_off)";
    char buf4[]= "(init l 4 before_kick_off)";
    char buf5[]= "(mdpstate 17 (pmode before_kick_off) (vmode high normal) (score 7 8) (ball 6e-41 -0 0 -0) (l_1 -3 37 0 -0 6.28 6.28 3500 1 1) (r_1 1.55e-05 37 0 -0 6.28 6.28 3500 1 1))";
    char new1[]= "(see 0 ((g r) 75.2 15) ((f c t) 24.5 -34 0 0) ((f r t) 73.7 -10) ((f r b) 90 36) ((f p r t) 56.3 0) ((f p r c) 59.7 19) ((f p r b) 68.7 35) ((f g r t) 73.7 10) ((f g r b) 77.5 20) ((f t 0) 27.7 -43 0 0) ((f t r 10) 35.5 -32) ((f t r 20) 44.3 -25) ((f t r 30) 53.5 -20) ((f t r 40) 62.8 -17) ((f t r 50) 72.2 -15) ((f b r 40) 83.9 44) ((f b r 50) 91.8 40) ((f r t 30) 78.3 -7) ((f r t 20) 77.5 0) ((f r t 10) 78.3 7) ((f r 0) 79.8 14) ((f r b 10) 83.1 21) ((f r b 20) 87.4 27) ((f r b 30) 91.8 32) ((l r) 72.2 -89))";

    while (0) {
      double tmp= 1000.0;
      //delay time
      for (long i= 1; i < 1E5; i++) 
	if (i % 10 == 0) tmp= 1000.0;
	else tmp= sqrt(tmp);
      //end delay time
      cout << "x" << flush;
      sb.parse_info(new1);
    }
  }
  if (argc>1) {
    cout << "\nParsing:\n" << argv[1];
    sb.parse_info(argv[1]);
    cout << "\nParser found: " << *(sb.see) << endl;
  }
  else {
    char sb1[] = "(sense_body 0 (view_mode high normal) (stamina 3500 1) (speed 0 0) (head_angle 0) (kick 0) (dash 0) (turn 0) (say 0) (turn_neck 0))";
    char see1[] = "(see 0 ((g r) 66.7 33) ((f r t) 55.7 3) ((f p r t) 42.5 23) ((f p r c) 53.5 43) ((f g r t) 62.8 28) ((f g r b) 70.8 38) ((f t 0) 3.6 -33 0 0) ((f t r 10) 13.2 -8 0 0) ((f t r 20) 23.1 -4) ((f t r 30) 33.1 -3) ((f t r 40) 42.9 -2) ((f t r 50) 53 -2) ((f r t 30) 60.9 6) ((f r t 20) 62.8 15) ((f r t 10) 66 24) ((f r 0) 70.8 31) ((f r b 10) 76.7 37) ((f r b 20) 83.1 43) ((p \"my team\" 1) 6 0 0 0 0 0) ((p \"PYTHONS\" 10 goalie) 33.1 0) ((p \"PYTHONS\") 36.6 0))";
    //char see1[] = "(see 23 ((p \"my_))))(() \nteam\" 1) 6 0 ))";

    cout << "\n--\nparsing:\n" << sb1;
    sb.parse_info(sb1);
    cout << "\nParser found: " << *(sb.sb);

    cout << "\n--\nparsing:\n" << see1 << flush;
    sb.parse_info(see1);
    cout << "\nParser found: " << *(sb.see);

  }
  return 1;
}
#endif 
