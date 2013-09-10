/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CONCAT_ASSIGN = 258,
     MODULO_ASSIGN = 259,
     DIV_ASSIGN = 260,
     MUL_ASSIGN = 261,
     MINUS_ASSIGN = 262,
     PLUS_ASSIGN = 263,
     ASSIGN = 264,
     RANGE = 265,
     HASH_REF = 266,
     STR_NE = 267,
     STR_GE = 268,
     STR_LE = 269,
     STR_EQ = 270,
     NUM_NE = 271,
     NUM_GE = 272,
     NUM_LE = 273,
     NUM_EQ = 274,
     CONCAT = 275,
     NAMED_PREFIX = 276,
     MINUS = 277,
     RIGHT_DEC = 278,
     RIGHT_INC = 279,
     DEC = 280,
     INC = 281,
     UNARY_PERCANTAGE = 282,
     FOR = 283,
     INT = 284,
     FAKE1 = 285,
     DOUBLE = 286,
     FAKE2 = 287,
     UNLESS = 288,
     IF = 289,
     ELSE = 290,
     ELSIF = 291,
     STR = 292,
     BARE_WORD = 293
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 35 "syntax.y"
 int i; double d; char* s; 


/* Line 2068 of yacc.c  */
#line 92 "syntax.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


