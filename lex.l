%{
	#include "syntax.tab.h"
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>

	char* cpystr(char*);
	int i = 0;
	int v = 0;
	char* tmp;
    int line_no = 1;

    int p_cnt = 0;  //parentheses count
    int sb_cnt = 0; //square brakets count
    int cb_cnt = 0; //curly brackets count
    int go = 0;
    int block = 0;

    int is_good_to_parse();

%}

%option interactive
%s ERROR

%%

for			{if(v) printf(" FOR "); yylval.i=i++; block++; return FOR;}
\(			{if(v) printf(" ( "); p_cnt++; return '(';}
\)			{
                if(v) 
                    printf(" ) "); 
                p_cnt--; go=1;
                return ')';
            }
\{			{if(v) printf(" { "); block++; cb_cnt++; return '{';}
\}			{
                if(v) 
                    printf(" } "); 
                cb_cnt--; block--; go=1;
                return '}';
            }
\[			{if(v) printf(" [ "); sb_cnt++; return '[';}
\]			{
                if(v) 
                    printf(" ] "); 
                sb_cnt--; go=1;
                return ']';
            }
if			{if(v) printf(" IF "); return IF;}
else		{if(v) printf(" ELSE "); return ELSE;}
elsif		{if(v) printf(" ELSIF "); return ELSIF;}

"-"			{if(v) printf(" - "); return '-';}
"+"			{if(v) printf(" + "); return '+';}
"*"			{if(v) printf(" * "); return '*';}
"/"			{if(v) printf(" / "); return '/';}
"+="		{if(v) printf(" PLUS_ASSIGN "); return PLUS_ASSIGN;}
"-="		{if(v) printf(" MINUS_ASSIGN "); return MINUS_ASSIGN;}
"*="		{if(v) printf(" MUL_ASSIGN "); return MUL_ASSIGN;}
"/="		{if(v) printf(" DIV_ASSIGN "); return DIV_ASSIGN;}
".="		{if(v) printf(" CONCAT_ASSIGN "); return CONCAT_ASSIGN;}
"%="		{if(v) printf(" MODULO_ASSIGN "); return MODULO_ASSIGN;}
"++"		{if(v) printf(" INC "); return INC;}
"--"		{if(v) printf(" DEC "); return DEC;}
".."		{if(v) printf(" RANGE "); return RANGE;}

"&"			{if(v) printf(" & "); return '&';}
"|"			{if(v) printf(" | "); return '|';}
"!"			{if(v) printf(" ! "); return '!';}
"=>"		{if(v) printf(" HASH_REF "); return HASH_REF;}
"="/[^=]	{if(v) printf(" ASSIGN "); return ASSIGN;}
">="		{if(v) printf(" NUM_GE "); return NUM_GE;}
"<="		{if(v) printf(" NUM_LE "); return NUM_LE;}
"!="		{if(v) printf(" NUM_NE "); return NUM_NE;}
"=="		{if(v) printf(" NUM_EQ "); return NUM_EQ;}
le		    {if(v) printf(" STR_LE "); return STR_LE;}
ge			{if(v) printf(" STR_GE "); return STR_GE;}
eq			{if(v) printf(" STR_EQ "); return STR_EQ;}
ne			{if(v) printf(" STR_NE "); return STR_NE;}
";"			{
                if(v) 
                    printf(" ; "); 
                go=1;
                return ';';}
#.*\n			;
'			{if(v) printf(" ' "); return '\'';}
,			{if(v) printf(" , "); return ',';}
@			{if(v) printf(" @ "); return '@';}
\$			{if(v) printf(" $ "); return '$';}
%			{if(v) printf(" PERCENT "); return '%';}
\.			{if(v) printf(" CONCAT "); return CONCAT;}

[0-9]+		{if(v) printf(" INT "); yylval.i=atoi(yytext); return INT;}
[0-9]+\.[0-9]+	{if(v) printf(" DOUBLE "); yylval.d=atof(yytext); return DOUBLE;}

[a-zA-Z_][a-zA-Z0-9_-]*[a-zA-Z]		{
					if(v) printf(" BARE_WORD ");
					yylval.s = cpystr(yytext);
					return BARE_WORD;
					}
[a-zA-Z_]				{
					if(v) printf(" BARE_WORD "); 
					yylval.s = cpystr(yytext);
					return BARE_WORD;
					}

\"[^\"]*\"		{
			if(v) printf(" STR "); 
            yytext[strlen(yytext)-1] = '\0';
			yylval.s = cpystr(yytext+1);
			return STR;
			}
'[^\']*'		{
			if(v) printf(" STR ");  
            yytext[strlen(yytext)-1] = '\0';
			yylval.s = cpystr(yytext+1);
			return STR;
			}

[ \t]		;
\n			{
                line_no++;
                if(go && block == 0 && is_good_to_parse())
                {
                    go=0;
                    yyterminate();
                }
            }
<<EOF>>     {go=0;yyterminate();}

<ERROR>[a-zA-Z0-9:_-]	{
				printf("Unknown token %s%s.\n", tmp, yytext);
				exit(1);
			}

<ERROR>.		{
				printf("Unknown token\n", tmp, yytext);
				exit(1);
			}
.			{tmp = cpystr(yytext); BEGIN(ERROR);}


%%

char* cpystr (char* src) {
	char* dest = (char*) malloc(strlen(src)+1);
	strcpy(dest,src);
	return dest;
}

int is_good_to_parse()
{
    if(p_cnt == 0 &&
       sb_cnt == 0 &&
       cb_cnt == 0)
    {
        return 1;
    }
   
   return 0;
}

