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


extern YYSTYPE yylval;
