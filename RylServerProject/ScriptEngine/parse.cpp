
/*  A Bison parser, made from string.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define YYLSP_NEEDED

#define	TOKEN_ID	258
#define	TOKEN_INTEGERVALUE	259
#define	TOKEN_FLOATVALUE	260
#define	TOKEN_BOOLVALUE	261
#define	TOKEN_STRINGVALUE	262
#define	TOKEN_INT	263
#define	TOKEN_FLOAT	264
#define	TOKEN_BOOL	265
#define	TOKEN_STRING	266
#define	TOKEN_VOID	267
#define	TOKEN_FOR	268
#define	TOKEN_WHILE	269
#define	TOKEN_IF	270
#define	TOKEN_ELSE	271
#define	TOKEN_SWITCH	272
#define	TOKEN_CASE	273
#define	TOKEN_DEFAULT	274
#define	TOKEN_CONTINUE	275
#define	TOKEN_BREAK	276
#define	TOKEN_RETURN	277
#define	TOKEN_ENDSTATEMENT	278
#define	TOKEN_LEFTPARENTHESIS	279
#define	TOKEN_RIGHTPARENTHESIS	280
#define	TOKEN_LEFTBRACE	281
#define	TOKEN_RIGHTBRACE	282
#define	TOKEN_LEFTBRACKET	283
#define	TOKEN_RIGHTBRACKET	284
#define	TOKEN_ERROR	285
#define	TOKEN_COMMA	286
#define	TOKEN_COLON	287
#define	TOKEN_ASSIGNMENT	288
#define	TOKEN_COMPOUNDADDITION	289
#define	TOKEN_COMPOUNDSUBTRACTION	290
#define	TOKEN_COMPOUNDMULTIPLICATION	291
#define	TOKEN_COMPOUNDDIVISION	292
#define	TOKEN_COMPOUNDREMINDER	293
#define	TOKEN_AND	294
#define	TOKEN_OR	295
#define	TOKEN_LESSTHAN	296
#define	TOKEN_LESSTHANOREQUAL	297
#define	TOKEN_MORETHAN	298
#define	TOKEN_MORETHANOREQUAL	299
#define	TOKEN_EQUALITY	300
#define	TOKEN_NOTEQUAL	301
#define	TOKEN_ADDITION	302
#define	TOKEN_SUBTRACTION	303
#define	TOKEN_MULTIPLICATION	304
#define	TOKEN_DIVISION	305
#define	TOKEN_REMINDER	306
#define	TOKEN_NOT	307
#define	PREFIXINCREMENT	308
#define	PREFIXDECREMENT	309
#define	TOKEN_INCREMENT	310
#define	TOKEN_DECREMENT	311

#line 1 "string.y"


/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */

#include <malloc.h>  // _alloca is used by the parser
#include <string>  // strcpy
#include <assert.h>
#include <stdlib.h>
#include "lex.h"     // the lexer
#include "SyntaxTree.h"
#include "SymbolTable.h"
#include "BaseDef.h"
#include "Message.h"

// Some yacc (bison) defines
#define YYDEBUG 1	      // Generate debug code; needed for YYERROR_VERBOSE
#define YYERROR_VERBOSE // Give a more specific parse error message 

// Forward references
void yyerror (char *msg);

extern CSyntaxTree *	g_pSynTree;
extern CSymbolTable *	g_pSymbolTable;

SFuncType	GetFunctionType( int nodeID );
SFuncType	GetFunctionType( int nodeID1, int nodeID2 );
SFuncType	GetFuncCallType( int nodeID );
int			CountArrayElements( int nodeID );
int			GetConstantValue( int nodeID );



#line 56 "string.y"
typedef union {
   int		symbolID;	// entry from symbol table
   int		nodeID;		// node in the syntax tree
   char 	szIdentifier[4096];
   char		szConstant[4096];
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		191
#define	YYFLAG		-32768
#define	YYNTBASE	57

#define YYTRANSLATE(x) ((unsigned)(x) <= 311 ? yytranslate[x] : 97)

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
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,     8,    10,    12,    14,    16,    19,
    21,    23,    27,    29,    32,    38,    46,    55,    58,    60,
    65,    69,    70,    76,    82,    87,    89,    91,    93,    94,
    97,   100,   104,   108,   110,   112,   114,   116,   118,   120,
   122,   124,   128,   132,   134,   139,   145,   150,   155,   157,
   161,   163,   167,   168,   171,   173,   175,   177,   179,   188,
   195,   197,   199,   201,   203,   205,   209,   211,   215,   219,
   223,   227,   231,   235,   237,   241,   243,   247,   249,   253,
   257,   259,   263,   267,   271,   275,   277,   281,   285,   287,
   291,   295,   299,   301,   304,   307,   310,   312,   317,   322,
   326,   329,   332,   334,   338
};

static const short yyrhs[] = {    58,
     0,    58,    59,     0,     0,    60,     0,    61,     0,    62,
     0,    66,     0,    70,     0,     1,    23,     0,    71,     0,
    84,     0,    28,    58,    29,     0,    23,     0,    86,    23,
     0,    15,    24,    86,    25,    59,     0,    15,    24,    86,
    25,    59,    16,    59,     0,    17,    24,    86,    25,    28,
    63,    65,    29,     0,    64,    63,     0,    64,     0,    18,
    85,    32,    58,     0,    19,    32,    58,     0,     0,    13,
    24,    67,    25,    59,     0,    14,    24,    86,    25,    59,
     0,    68,    69,    23,    69,     0,    61,     0,    71,     0,
    86,     0,     0,    21,    23,     0,    20,    23,     0,    22,
    69,    23,     0,    72,    75,    23,     0,    12,     0,     8,
     0,     9,     0,    10,     0,    11,     0,     3,     0,     3,
     0,    76,     0,    75,    31,    76,     0,    74,    33,    87,
     0,    74,     0,     3,    26,    27,    77,     0,     3,    26,
    85,    27,    77,     0,     3,    26,    85,    27,     0,    33,
    28,    78,    29,     0,    87,     0,    87,    31,    78,     0,
    80,     0,    80,    31,    79,     0,     0,    72,    74,     0,
    72,     0,     3,     0,    23,     0,    28,     0,    72,    81,
    24,    79,    25,    83,    58,    29,     0,    72,    81,    24,
    79,    25,    82,     0,     4,     0,     5,     0,     6,     0,
     7,     0,    87,     0,    87,    31,    86,     0,    88,     0,
    73,    33,    87,     0,    73,    34,    87,     0,    73,    35,
    87,     0,    73,    36,    87,     0,    73,    37,    87,     0,
    73,    38,    87,     0,    89,     0,    88,    40,    89,     0,
    90,     0,    89,    39,    90,     0,    91,     0,    90,    45,
    91,     0,    90,    46,    91,     0,    92,     0,    91,    41,
    92,     0,    91,    43,    92,     0,    91,    42,    92,     0,
    91,    44,    92,     0,    93,     0,    92,    47,    93,     0,
    92,    48,    93,     0,    94,     0,    93,    49,    94,     0,
    93,    50,    94,     0,    93,    51,    94,     0,    95,     0,
    55,    73,     0,    56,    73,     0,    52,    94,     0,    96,
     0,    73,    26,    86,    27,     0,     3,    24,    86,    25,
     0,     3,    24,    25,     0,    73,    55,     0,    73,    56,
     0,    85,     0,    24,    86,    25,     0,    73,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   120,   125,   126,   130,   131,   132,   133,   134,   135,   136,
   137,   141,   146,   147,   151,   153,   155,   161,   162,   166,
   171,   172,   176,   178,   183,   188,   189,   193,   194,   198,
   199,   200,   205,   211,   212,   214,   216,   218,   223,   231,
   239,   240,   244,   245,   246,   248,   250,   255,   260,   261,
   266,   267,   269,   273,   274,   278,   283,   287,   291,   300,
   306,   307,   308,   309,   313,   314,   318,   319,   321,   323,
   325,   327,   329,   334,   335,   340,   341,   346,   347,   349,
   354,   355,   357,   359,   361,   366,   367,   369,   374,   375,
   377,   379,   384,   385,   387,   389,   393,   394,   396,   403,
   410,   411,   415,   416,   418
};

static const char * const yytname[] = {   "$","error","$undefined.","TOKEN_ID",
"TOKEN_INTEGERVALUE","TOKEN_FLOATVALUE","TOKEN_BOOLVALUE","TOKEN_STRINGVALUE",
"TOKEN_INT","TOKEN_FLOAT","TOKEN_BOOL","TOKEN_STRING","TOKEN_VOID","TOKEN_FOR",
"TOKEN_WHILE","TOKEN_IF","TOKEN_ELSE","TOKEN_SWITCH","TOKEN_CASE","TOKEN_DEFAULT",
"TOKEN_CONTINUE","TOKEN_BREAK","TOKEN_RETURN","TOKEN_ENDSTATEMENT","TOKEN_LEFTPARENTHESIS",
"TOKEN_RIGHTPARENTHESIS","TOKEN_LEFTBRACE","TOKEN_RIGHTBRACE","TOKEN_LEFTBRACKET",
"TOKEN_RIGHTBRACKET","TOKEN_ERROR","TOKEN_COMMA","TOKEN_COLON","TOKEN_ASSIGNMENT",
"TOKEN_COMPOUNDADDITION","TOKEN_COMPOUNDSUBTRACTION","TOKEN_COMPOUNDMULTIPLICATION",
"TOKEN_COMPOUNDDIVISION","TOKEN_COMPOUNDREMINDER","TOKEN_AND","TOKEN_OR","TOKEN_LESSTHAN",
"TOKEN_LESSTHANOREQUAL","TOKEN_MORETHAN","TOKEN_MORETHANOREQUAL","TOKEN_EQUALITY",
"TOKEN_NOTEQUAL","TOKEN_ADDITION","TOKEN_SUBTRACTION","TOKEN_MULTIPLICATION",
"TOKEN_DIVISION","TOKEN_REMINDER","TOKEN_NOT","PREFIXINCREMENT","PREFIXDECREMENT",
"TOKEN_INCREMENT","TOKEN_DECREMENT","program","statement_list","statement","compound_statement",
"expression_statement","selection_statement","cases","case_one","default","iteration_statement",
"for_expression","for_init_statement","optional_expression","jump_statement",
"declaration","decl_specifiers","variable","new_variable","declarator_list",
"init_declarator","array_initializer","initializer_list","argument_declaration_list",
"argument_declaration","function_name","function_decl_end","function_def_start",
"function_definition","constant_expression","expression","assignment_expression",
"logical_or_expression","logical_and_expression","equality_expression","relational_expression",
"additive_expression","multiplicative_expression","unary_expression","postfix_expression",
"primary_expression",""
};
#endif

static const short yyr1[] = {     0,
    57,    58,    58,    59,    59,    59,    59,    59,    59,    59,
    59,    60,    61,    61,    62,    62,    62,    63,    63,    64,
    65,    65,    66,    66,    67,    68,    68,    69,    69,    70,
    70,    70,    71,    72,    72,    72,    72,    72,    73,    74,
    75,    75,    76,    76,    76,    76,    76,    77,    78,    78,
    79,    79,    79,    80,    80,    81,    82,    83,    84,    84,
    85,    85,    85,    85,    86,    86,    87,    87,    87,    87,
    87,    87,    87,    88,    88,    89,    89,    90,    90,    90,
    91,    91,    91,    91,    91,    92,    92,    92,    93,    93,
    93,    93,    94,    94,    94,    94,    95,    95,    95,    95,
    95,    95,    96,    96,    96
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     1,     2,     1,
     1,     3,     1,     2,     5,     7,     8,     2,     1,     4,
     3,     0,     5,     5,     4,     1,     1,     1,     0,     2,
     2,     3,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     3,     3,     1,     4,     5,     4,     4,     1,     3,
     1,     3,     0,     2,     1,     1,     1,     1,     8,     6,
     1,     1,     1,     1,     1,     3,     1,     3,     3,     3,
     3,     3,     3,     1,     3,     1,     3,     1,     3,     3,
     1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
     3,     3,     1,     2,     2,     2,     1,     4,     4,     3,
     2,     2,     1,     3,     1
};

static const short yydefact[] = {     3,
     0,     0,    39,    61,    62,    63,    64,    35,    36,    37,
    38,    34,     0,     0,     0,     0,     0,     0,    29,    13,
     0,     3,     0,     0,     0,     2,     4,     5,     6,     7,
     8,    10,     0,   105,    11,   103,     0,    65,    67,    74,
    76,    78,    81,    86,    89,    93,    97,     9,     0,     0,
     0,     0,     0,    31,    30,     0,    28,     0,     0,   105,
    96,    39,    94,    95,    40,    44,     0,    41,     0,     0,
     0,     0,     0,     0,     0,     0,   101,   102,    14,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   100,     0,    26,     0,    29,    27,     0,
     0,     0,     0,    32,   104,    12,     0,     0,    33,     0,
    53,     0,    68,    69,    70,    71,    72,    73,    66,    75,
    77,    79,    80,    82,    84,    83,    85,    87,    88,    90,
    91,    92,    99,     0,     0,    40,     0,     0,     0,     0,
     0,    43,    42,    55,     0,    51,    98,    23,    29,    24,
    15,     0,     0,    45,    47,    40,    54,     0,    53,    25,
     0,     0,    22,    19,     0,    46,    57,    58,    60,     3,
    52,    16,     0,     0,     0,    18,     0,    49,     0,     3,
     3,    17,    48,     0,    59,     0,     0,    50,     0,     0,
     0
};

static const short yydefgoto[] = {   189,
     1,    26,    27,    28,    29,   163,   164,   175,    30,    97,
    98,    56,    31,    32,    33,    34,    66,    67,    68,   154,
   177,   145,   146,    69,   169,   170,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47
};

static const short yypact[] = {-32768,
   115,    -2,     8,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    27,    42,    49,    73,    78,    80,    50,-32768,
    50,-32768,    50,   101,   101,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   130,   400,-32768,-32768,   111,   109,   102,   110,
   -34,    17,    -1,    63,-32768,-32768,-32768,-32768,    12,   369,
    50,    50,    50,-32768,-32768,   118,-32768,   125,   227,   -13,
-32768,-32768,-32768,-32768,    14,   119,    -9,-32768,   127,    50,
    50,    50,    50,    50,    50,    50,-32768,-32768,-32768,    50,
    50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
    50,    50,    50,-32768,   128,-32768,   129,    50,-32768,   152,
   131,   132,   135,-32768,-32768,-32768,    23,    50,-32768,   152,
    67,   134,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   110,
   -34,    17,    17,    -1,    -1,    -1,    -1,    63,    63,-32768,
-32768,-32768,-32768,   395,   139,   137,   395,   395,   136,   133,
   138,-32768,-32768,   165,   144,   142,-32768,-32768,    50,-32768,
   180,   169,   170,-32768,   133,-32768,-32768,    16,    67,-32768,
   395,   103,   178,   169,    50,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   172,   173,   174,-32768,   177,   176,   283,-32768,
-32768,-32768,-32768,    50,-32768,   171,   339,-32768,   201,   202,
-32768
};

static const short yypgoto[] = {-32768,
   -22,   -89,-32768,   158,-32768,    45,-32768,-32768,-32768,-32768,
-32768,   -78,-32768,   160,   -48,     0,    68,-32768,   104,    56,
    29,    57,-32768,-32768,-32768,-32768,-32768,   -97,   -18,   -67,
-32768,   140,   143,   -14,    59,    10,     3,-32768,-32768
};


#define	YYLAST		456


static const short yytable[] = {    59,
    57,   100,    58,   113,   114,   115,   116,   117,   118,   141,
    83,    84,    70,   109,     3,     4,     5,     6,     7,   135,
    48,   110,    60,    63,    64,    61,     4,     5,     6,     7,
    95,    49,   101,   102,   103,    21,    94,   -56,   167,   107,
   142,    77,    78,   168,   148,    89,    90,   150,   151,   140,
    50,   112,     3,     4,     5,     6,     7,    85,    86,    87,
    88,   119,   144,    23,   173,    51,    24,    25,   122,   123,
   160,   172,    52,    21,     8,     9,    10,    11,    12,    57,
    60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
    60,    60,    60,   130,   131,   132,    53,   178,   128,   129,
    54,    23,    55,    62,    24,    25,     4,     5,     6,     7,
   144,    91,    92,    93,    -1,     2,   178,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    57,    16,    65,    79,    17,    18,    19,    20,    21,    80,
   104,    81,    22,   124,   125,   126,   127,   179,    82,   105,
   111,   108,   133,   134,   136,   137,   138,   186,   187,   139,
   147,   149,   107,   152,   155,   153,    23,   156,   158,    24,
    25,     2,   159,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,   162,    16,   -20,   -20,
    17,    18,    19,    20,    21,   161,   174,   165,    22,   -20,
   190,   191,   182,   180,   181,   183,   184,    96,   176,    99,
   166,   157,   188,   143,     0,   171,     0,     0,     0,     0,
   120,     0,    23,     0,   121,    24,    25,     2,     0,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,     0,    16,     0,     0,    17,    18,    19,    20,
    21,     0,     0,     0,    22,   106,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
     0,    24,    25,     2,     0,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,     0,    16,
     0,     0,    17,    18,    19,    20,    21,     0,     0,     0,
    22,   185,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    23,     0,     0,    24,    25,     2,
     0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,     0,    16,     0,     0,    17,    18,
    19,    20,    21,     0,     0,     0,    22,   -21,     0,     0,
     0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    23,    20,    21,    24,    25,     2,     0,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
     0,    16,     0,     0,    17,    18,    19,    20,    21,     0,
    23,     0,    22,    24,    25,    70,     0,     0,     0,     0,
     0,     0,    71,    72,    73,    74,    75,    76,     0,     0,
     0,     0,     0,     0,     0,     0,    23,     0,     0,    24,
    25,     0,     0,     0,    77,    78
};

static const short yycheck[] = {    22,
    19,    50,    21,    71,    72,    73,    74,    75,    76,   107,
    45,    46,    26,    23,     3,     4,     5,     6,     7,    98,
    23,    31,    23,    24,    25,    23,     4,     5,     6,     7,
    49,    24,    51,    52,    53,    24,    25,    24,    23,    26,
   108,    55,    56,    28,   134,    47,    48,   137,   138,    27,
    24,    70,     3,     4,     5,     6,     7,    41,    42,    43,
    44,    80,   111,    52,   162,    24,    55,    56,    83,    84,
   149,   161,    24,    24,     8,     9,    10,    11,    12,    98,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    91,    92,    93,    24,   165,    89,    90,
    23,    52,    23,     3,    55,    56,     4,     5,     6,     7,
   159,    49,    50,    51,     0,     1,   184,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
   149,    17,     3,    23,    20,    21,    22,    23,    24,    31,
    23,    40,    28,    85,    86,    87,    88,   170,    39,    25,
    24,    33,    25,    25,     3,    25,    25,   180,   181,    25,
    27,    23,    26,    28,    27,    33,    52,     3,    25,    55,
    56,     1,    31,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    18,    17,    18,    19,
    20,    21,    22,    23,    24,    16,    19,    28,    28,    29,
     0,     0,    29,    32,    32,    29,    31,    50,   164,    50,
   155,   144,   184,   110,    -1,   159,    -1,    -1,    -1,    -1,
    81,    -1,    52,    -1,    82,    55,    56,     1,    -1,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    -1,    17,    -1,    -1,    20,    21,    22,    23,
    24,    -1,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,
    -1,    55,    56,     1,    -1,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    -1,    17,
    -1,    -1,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,     1,
    -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    -1,    17,    -1,    -1,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    28,    29,    -1,    -1,
    -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    52,    23,    24,    55,    56,     1,    -1,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    -1,    17,    -1,    -1,    20,    21,    22,    23,    24,    -1,
    52,    -1,    28,    55,    56,    26,    -1,    -1,    -1,    -1,
    -1,    -1,    33,    34,    35,    36,    37,    38,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    55,
    56,    -1,    -1,    -1,    55,    56
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

#line 3 "bison.simple"



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



/* Prevent warning if -Wstrict-prototypes.  */

#ifdef __GNUC__

int yyparse (void);

#endif



#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */

#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)

#else				/* not GNU C or C++ */

#ifndef __cplusplus



/* This is the most reliable way to avoid incompatibilities

   in available built-in functions on various systems.  */

static void

__yy_memcpy (from, to, count)

     char *from;

     char *to;

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

__yy_memcpy (char *from, char *to, int count)

{

  register char *f = from;

  register char *t = to;

  register int i = count;



  while (i-- > 0)

    *t++ = *f++;

}



#endif

#endif



#line 192 "bison.simple"



/* The user can define YYPARSE_PARAM as the name of an argument to be passed

   into yyparse.  The argument should have type void *.

   It should actually point to an object.

   Grammar actions can access the variable by casting it

   to the proper pointer type.  */



#ifdef YYPARSE_PARAM

#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;

#else

#define YYPARSE_PARAM

#define YYPARSE_PARAM_DECL

#endif



int

yyparse(YYPARSE_PARAM)

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

      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));

      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));

      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));

#ifdef YYLSP_NEEDED

      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));

      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));

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
#line 120 "string.y"
{ assert( g_pSynTree != NULL );
																g_pSynTree->SetRoot( yyvsp[0].nodeID ); ;
    break;}
case 2:
#line 125 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_STATEMENT_LIST, yyvsp[-1].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 3:
#line 126 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[1].first_line, TYPE_EMPTY_STATEMENT ); ;
    break;}
case 4:
#line 130 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 5:
#line 131 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 6:
#line 132 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 7:
#line 133 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 8:
#line 134 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 9:
#line 135 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_ERROR_STATEMENT ); ;
    break;}
case 10:
#line 136 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 11:
#line 137 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 12:
#line 142 "string.y"
{ yyval.nodeID = yyvsp[-1].nodeID; ;
    break;}
case 13:
#line 146 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_EMPTY_STATEMENT ); ;
    break;}
case 14:
#line 147 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_EXPRESSION, yyvsp[-1].nodeID ); ;
    break;}
case 15:
#line 152 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-4].first_line, TYPE_IF_STATEMENT, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 16:
#line 154 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-6].first_line, TYPE_IF_ELSE_STATEMENT, yyvsp[-4].nodeID, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 17:
#line 157 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-7].first_line, TYPE_SWITCH_STATEMENT, yyvsp[-5].nodeID, yyvsp[-2].nodeID, yyvsp[-1].nodeID ); ;
    break;}
case 18:
#line 161 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_SWITCH_CASES, yyvsp[-1].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 19:
#line 162 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 20:
#line 167 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, TYPE_CASE_ONE, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 21:
#line 171 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_DEFAULT, yyvsp[0].nodeID ); ;
    break;}
case 22:
#line 172 "string.y"
{ yyval.nodeID = 0; ;
    break;}
case 23:
#line 177 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-4].first_line, TYPE_FOR_STATEMENT, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 24:
#line 179 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-4].first_line, TYPE_WHILE_STATEMENT, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 25:
#line 184 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, TYPE_FOR_EXPRESSION, yyvsp[-3].nodeID, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 26:
#line 188 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 27:
#line 189 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 28:
#line 193 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 29:
#line 194 "string.y"
{ yyval.nodeID = 0; ;
    break;}
case 30:
#line 198 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_BREAK_STATEMENT ); ;
    break;}
case 31:
#line 199 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_CONTINUE_STATEMENT ); ;
    break;}
case 32:
#line 201 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_RETURN_STATEMENT, yyvsp[-1].nodeID ); ;
    break;}
case 33:
#line 206 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_DECLARATION, yyvsp[-2].nodeID, yyvsp[-1].nodeID ); 
																g_pSymbolTable->SetCurrentType( T_VOID ); ;
    break;}
case 34:
#line 211 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_SPECIFIER_VOID ); ;
    break;}
case 35:
#line 212 "string.y"
{ g_pSymbolTable->SetCurrentType( T_INT );
																yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_SPECIFIER_INT ); ;
    break;}
case 36:
#line 214 "string.y"
{ g_pSymbolTable->SetCurrentType( T_FLOAT );
																yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_SPECIFIER_FLOAT ); ;
    break;}
case 37:
#line 216 "string.y"
{ g_pSymbolTable->SetCurrentType( T_BOOL );
																yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_SPECIFIER_BOOL ); ;
    break;}
case 38:
#line 218 "string.y"
{ g_pSymbolTable->SetCurrentType( T_STRING );
																yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, TYPE_SPECIFIER_STRING ); ;
    break;}
case 39:
#line 223 "string.y"
{ 
																yyval.symbolID = g_pSymbolTable->FindVar( yyvsp[0].szIdentifier ); 
																if( yyval.symbolID == 0 )
																	ErrorMessage2( yylsp[0].first_line, "undefined symbol : %s", yyvsp[0].szIdentifier );
															;
    break;}
case 40:
#line 231 "string.y"
{
																yyval.symbolID = g_pSymbolTable->AddVar( yyvsp[0].szIdentifier ); 
																if( yyval.symbolID == 0 )
																	ErrorMessage2( yylsp[0].first_line, "이미 정의 되어 있는 심볼입니다. : %s", yyvsp[0].szIdentifier );
															;
    break;}
case 41:
#line 239 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 42:
#line 240 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_DECLARATOR_LIST, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 43:
#line 244 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_INIT_DECLARATION, yyvsp[0].nodeID ); ;
    break;}
case 44:
#line 245 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, yyvsp[0].symbolID, TYPE_NORMAL_DECLARATION ); ;
    break;}
case 45:
#line 247 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, g_pSymbolTable->AddArrVar( yyvsp[-3].szIdentifier, CountArrayElements( yyvsp[0].nodeID ) ), TYPE_ARRAY_INITIALIZE, yyvsp[0].nodeID ); ;
    break;}
case 46:
#line 249 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-4].first_line, g_pSymbolTable->AddArrVar( yyvsp[-4].szIdentifier, GetConstantValue( yyvsp[-2].nodeID ) ), TYPE_ARRAY_INITIALIZE2, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 47:
#line 251 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, g_pSymbolTable->AddArrVar( yyvsp[-3].szIdentifier, GetConstantValue( yyvsp[-1].nodeID ) ), TYPE_ARRAY_DECLARATION, yyvsp[-1].nodeID ); ;
    break;}
case 48:
#line 256 "string.y"
{ yyval.nodeID = yyvsp[-1].nodeID; ;
    break;}
case 49:
#line 260 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 50:
#line 262 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_INITIALIZER_LIST, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 51:
#line 266 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 52:
#line 268 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_ARGUMENT_DECLARATION_LIST, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 53:
#line 269 "string.y"
{ yyval.nodeID = 0; ;
    break;}
case 54:
#line 273 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, yyvsp[0].symbolID, TYPE_ARGUMENT_DECLARATION, yyvsp[-1].nodeID ); ;
    break;}
case 55:
#line 274 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 56:
#line 278 "string.y"
{ strcpy( yyval.szIdentifier, yyvsp[0].szIdentifier ); 
																g_pSymbolTable->BeginLocalNameSpace();	;
    break;}
case 57:
#line 283 "string.y"
{ g_pSymbolTable->EndLocalNameSpace( 0 ); ;
    break;}
case 58:
#line 287 "string.y"
{ g_pSymbolTable->EndArgument(); ;
    break;}
case 59:
#line 292 "string.y"
{ 
																int symbID = g_pSymbolTable->AddFunc( true, yyvsp[-6].szIdentifier, GetFunctionType( yyvsp[-7].nodeID, yyvsp[-4].nodeID ) );
																if( symbID == 0 )
																	ErrorMessage( yylsp[-7].first_line, "같은 함수를 중복하여 정의했습니다." );
																yyval.nodeID = g_pSynTree->Insert( yylsp[-7].first_line, symbID, TYPE_FUNCTION_DEFINITION, yyvsp[-4].nodeID, yyvsp[-1].nodeID );
																	g_pSymbolTable->SetCurrentType( T_VOID );
																g_pSymbolTable->EndLocalNameSpace( symbID );
															;
    break;}
case 60:
#line 301 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-5].first_line, g_pSymbolTable->AddFunc( false, yyvsp[-4].szIdentifier, GetFunctionType( yyvsp[-5].nodeID, yyvsp[-2].nodeID ) ), TYPE_FUNCTION_DECLARATION, yyvsp[-2].nodeID );
																	g_pSymbolTable->SetCurrentType( T_VOID ); ;
    break;}
case 61:
#line 306 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, g_pSymbolTable->AddConst( yyvsp[0].szConstant, T_INT ), TYPE_CONSTANT_EXPRESSION ); ;
    break;}
case 62:
#line 307 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, g_pSymbolTable->AddConst( yyvsp[0].szConstant, T_FLOAT ), TYPE_CONSTANT_EXPRESSION ); ;
    break;}
case 63:
#line 308 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, g_pSymbolTable->AddConst( yyvsp[0].szConstant, T_BOOL ), TYPE_CONSTANT_EXPRESSION ); ;
    break;}
case 64:
#line 309 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, g_pSymbolTable->AddConst( yyvsp[0].szConstant, T_STRING ), TYPE_CONSTANT_EXPRESSION ); ;
    break;}
case 65:
#line 313 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 66:
#line 314 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_EXPRESSION_LIST, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 67:
#line 318 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 68:
#line 320 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_ASSIGNMENT_EXPRESSION, yyvsp[0].nodeID ); ;
    break;}
case 69:
#line 322 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_COMPOUND_ADDITION, yyvsp[0].nodeID ); ;
    break;}
case 70:
#line 324 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_COMPOUND_SUBTRACTION, yyvsp[0].nodeID ); ;
    break;}
case 71:
#line 326 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_COMPOUND_MULTIPLICATION, yyvsp[0].nodeID ); ;
    break;}
case 72:
#line 328 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_COMPOUND_DIVISION, yyvsp[0].nodeID ); ;
    break;}
case 73:
#line 330 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, yyvsp[-2].symbolID, TYPE_COMPOUND_REMINDER, yyvsp[0].nodeID ); ;
    break;}
case 74:
#line 334 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 75:
#line 336 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_OR_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 76:
#line 340 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 77:
#line 342 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_AND_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 78:
#line 346 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 79:
#line 348 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_EQUALITY_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 80:
#line 350 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_NOTEQAUL_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 81:
#line 354 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 82:
#line 356 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_LESSTHAN_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 83:
#line 358 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_MORETHAN_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 84:
#line 360 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_LESSTHANOREQUAL_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 85:
#line 362 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_MORETHANOREQUAL_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 86:
#line 366 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 87:
#line 368 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_ADDITION_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 88:
#line 370 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_SUBTRACTION_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 89:
#line 374 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 90:
#line 376 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_MULTIPLICATION_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 91:
#line 378 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_DIVISION_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 92:
#line 380 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_REMINDER_EXPRESSION, yyvsp[-2].nodeID, yyvsp[0].nodeID ); ;
    break;}
case 93:
#line 384 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 94:
#line 386 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, yyvsp[0].symbolID, TYPE_PREFIXINCREMENT ); ;
    break;}
case 95:
#line 388 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, yyvsp[0].symbolID, TYPE_PREFIXDECREMENT ); ;
    break;}
case 96:
#line 389 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, TYPE_NOT_EXPRESSION, yyvsp[0].nodeID ); ;
    break;}
case 97:
#line 393 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 98:
#line 395 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, yyvsp[-3].symbolID, TYPE_ARRAY_INDEXING, yyvsp[-1].nodeID ); ;
    break;}
case 99:
#line 397 "string.y"
{ 
																int symbolID = g_pSymbolTable->FindFunc( yyvsp[-3].szIdentifier, GetFuncCallType( yyvsp[-1].nodeID ) );
																if( symbolID == 0 )
																	ErrorMessage2( "Error(line %d) : 지정한 함수를 찾을 수 없습니다.\n", yylsp[-3].first_line );
																yyval.nodeID = g_pSynTree->Insert( yylsp[-3].first_line, symbolID, TYPE_FUNCTION_CALL, yyvsp[-1].nodeID ); 
															;
    break;}
case 100:
#line 404 "string.y"
{ 
																int symbolID = g_pSymbolTable->FindFunc( yyvsp[-2].szIdentifier, 0 );
																if( symbolID == 0 )
																	ErrorMessage2( "Error(line %d) : 지정한 함수를 찾을 수 없습니다.\n", yylsp[-2].first_line );
																yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, symbolID, TYPE_FUNCTION_CALL ); 
															;
    break;}
case 101:
#line 410 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, yyvsp[-1].symbolID, TYPE_POSTFIXINCREMENT ); ;
    break;}
case 102:
#line 411 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-1].first_line, yyvsp[-1].symbolID, TYPE_POSTFIXDECREMENT ); ;
    break;}
case 103:
#line 415 "string.y"
{ yyval.nodeID = yyvsp[0].nodeID; ;
    break;}
case 104:
#line 417 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[-2].first_line, TYPE_EXPRESSION, yyvsp[-1].nodeID ); ;
    break;}
case 105:
#line 418 "string.y"
{ yyval.nodeID = g_pSynTree->Insert( yylsp[0].first_line, yyvsp[0].symbolID, TYPE_VARIABLE ); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */

#line 487 "bison.simple"



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

#line 441 "string.y"

/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

eDataType	GetType( SNode * pNode )
{
	switch( pNode->m_eType )
	{
	case TYPE_ARGUMENT_DECLARATION :
			
			return GetType( pNode->m_ArrPtrChilds[0] );

	case TYPE_SPECIFIER_INT :
			
			return T_INT;

	case TYPE_SPECIFIER_FLOAT :
			
			return T_FLOAT;

	case TYPE_SPECIFIER_BOOL :

			return T_BOOL;

	case TYPE_SPECIFIER_STRING :

			return T_STRING;
	}
	return T_VOID;
}

SFuncType	GetFunctionType( eDataType retType, int nodeID )
{
	SNode * pNode = (SNode*)nodeID;

	SFuncType FuncType;
	
	int i = 0;

	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_ARGUMENT_DECLARATION_LIST )
		{
			FuncType.SetArgType( i++, GetType( pNode->m_ArrPtrChilds[0] ) );
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else																//TYPE_ARGUMENT_DECLARATION나 TYPE_SPECIFIER_XXX 계열일 때
		{																	
			FuncType.SetArgType( i++, GetType( pNode ) );
			break;
		}
	}

	assert( i <= 8 );

	FuncType.SetReturnType( retType );

	return FuncType;
}

SFuncType	GetFunctionType( int nodeID )
{
	return GetFunctionType( g_pSymbolTable->GetCurrentType(), nodeID );
}

SFuncType	GetFunctionType( int nodeID1, int nodeID2 )
{
	SNode * pNode = (SNode*)nodeID1;
	if( pNode != NULL )
	{
		return GetFunctionType( GetType( pNode ), nodeID2 );
	}
	return 0;
}

SFuncType	GetFuncCallType( int nodeID )
{
	SNode * pNode = (SNode*)nodeID;

	SFuncType FuncType;

	int i = 0;

	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_EXPRESSION_LIST )
		{
			FuncType.SetArgType( i++, pNode->m_ArrPtrChilds[0]->m_eReturnType );
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else
		{
			FuncType.SetArgType( i++, pNode->m_eReturnType );
			break;	
		}
	}

	assert( i <= 8 );

	FuncType.SetReturnType( T_VOID );

	return FuncType;
}

int			CountArrayElements( int nodeID )
{
	if( nodeID == 0 )
		return 0;

	SNode * pNode = (SNode*)nodeID;
	//initializer_list

	int i = 0;
	
	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_INITIALIZER_LIST )
		{
			i++;
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else
		{
			i++;
			break;
		}	
	}

	return i;
}

int			GetConstantValue( int nodeID )
{
	//constant expression
	SNode * pNode = (SNode*)nodeID;

	if( pNode->m_eType == TYPE_CONSTANT_EXPRESSION )
	{
		if( g_pSymbolTable->GetTypeOfConst( pNode->m_SymbolID ) == T_INT )
		{
			const char * pStr = g_pSymbolTable->GetNameOfConst( pNode->m_SymbolID );
			return strtol( pStr, NULL, 0 );
		}
	}

	return 0;
}
