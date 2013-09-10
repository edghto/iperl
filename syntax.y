%{
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "kernel.h"
	#include "AST.h"
	#include "AV.h"
	#include "SV.h"
	#include "HV.h"
	#include "eval.h"

	#define push(x)       av_push(stack, newRV_noinc( x ))
	#define pop()         SvRV( av_pop(stack) )
	#define is_empty()    (av_len(stack) < 0)
	#define clear_stack() av_clear(stack)
	AV* stack;

    extern int line_no;
%}

%left ASSIGN PLUS_ASSIGN MINUS_ASSIGN MUL_ASSIGN DIV_ASSIGN MODULO_ASSIGN CONCAT_ASSIGN
%left RANGE
%left HASH_REF
%left '\\'
%left '<' '>' NUM_EQ NUM_LE NUM_GE NUM_NE STR_EQ STR_LE STR_GE STR_NE
%left CONCAT
%left NAMED_PREFIX
%left '+' '-'
%left '*' '/' '%'
%left '!' MINUS
%right RIGHT_INC RIGHT_DEC
%left INC DEC
%left UNARY_PERCANTAGE

%union { int i; double d; char* s; };
%token FOR<i> INT<i> FAKE1<d> DOUBLE<d> FAKE2<s> UNLESS IF ELSE ELSIF ASSIGN STR<s> BARE_WORD<s> NUM_EQ NUM_LE NUM_GE NUM_NE STR_EQ STR_LE STR_GE STR_NE CONCAT INC DEC PLUS_ASSIGN MINUS_ASSIGN MUL_ASSIGN DIV_ASSIGN CONCAT_ASSIGN MODULO_ASSIGN HASH_REF RANGE

%%

program : instruction
	{
		if (!is_empty()) {
			AV *av = (AV*) pop();
			SV* ret = eval(av);
			printf("\t");sv_dump(ret);printf("\n");
		}
		clear_stack();
	}
	|
	;

instructions : instructions instruction
		{
			AV *inst = (AV*) pop();
			AV *insts = (AV*) pop();
			av_push(insts, newRV_noinc( inst ) );
			push(insts);
		}
             | instruction 
		{
			AV *inst =  (AV*) pop();
			AV *av = newINST(inst);
			push(av);
		}
             ;

instruction : colon_instruction ';'
            | none_colon_instruction
            ;

colon_instruction : eexpr
                  | assign
                  | short_loop
                  | short_cond
		  ;

none_colon_instruction : loop
                       | condition
                       ;

loop : loop_keyword '(' expr ')' '{' instructions '}'
	{
		AV *for_block =  (AV*) pop();
		AV *list =  (AV*) pop();
		AV *var = newAV(); //TODO currently by only available variable is $_
		AV *av = newFOR(var, list, for_block);
		push(av);
	}
     ;

condition : if elsifs else
		{
			AV *else_block =  (AV*) pop();
			AV *elsifs =  (AV*) pop();
			AV *if_block =  (AV*) pop();
			setElseBlock(if_block, elsifs);
			setElseBlock(elsifs, else_block);
			push(if_block);
		}
          | if else
		{
			AV *else_block =  (AV*) pop();
			AV *if_block =  (AV*) pop();
			setElseBlock(if_block, else_block);
			push(if_block);
		}
          ;

if : cond_keyword '(' expr ')' '{' instructions '}'
	{
		AV *if_block =  (AV*) pop();
		AV *cond =  (AV*) pop();
		AV *av = newIF(cond, if_block);
		push(av);
	}
   ;

elsifs : elsif elsifs
	{
		AV *elsifs = (AV*) pop();
		AV *elsif =  (AV*) pop();
		setElseBlock(elsif, elsifs);
		push(elsif);
	}
       | elsif
       ;

elsif : ELSIF '(' expr ')' '{' instructions '}'
	{
		AV *if_block =  (AV*) pop();
		AV *cond =  (AV*) pop();
		AV *av = newIF(cond, if_block);
		push(av);
	}
      ;

else : ELSE '{' instructions '}'
	{
		AV *else_block =  (AV*) pop();
		push(else_block); 
	}
     |
     {
        AV *empty_else = newAV();
        push(empty_else);
     }
     ;

short_loop : expr loop_keyword '(' expr ')' 
		{
			AV *list =  (AV*) pop();
			AV *for_block =  (AV*) pop();
			AV *var = newAV(); //TODO
			AV *av = newFOR(var, list, for_block);
			push(av);
		}
           | short_cond loop_keyword '(' expr ')'
		{
			AV *list =  (AV*) pop();
			AV *for_block =  (AV*) pop();
			AV *var = newAV(); //TODO
			AV *av = newFOR(var, list, for_block);
			push(av);
		}
           ;

loop_keyword : FOR
             ;

short_cond : expr cond_keyword '(' expr ')'
		{
			AV *cond =  (AV*) pop();
			AV *if_block =  (AV*) pop();
			AV *av = newIF(cond, if_block);
			push(av);
		}
           ;

cond_keyword : IF
             | UNLESS
             ;

assign : lvalue ASSIGN expr
	{
		AV *arg2 =  (AV*) pop();
		AV *arg1 =  (AV*) pop();
		AV *av = newBIOP(ASSIGN, arg1, arg2);
		push(av);
	}
       ;

lvalue : var
       ; 

eexpr : expr
      |
      ;

expr : STR
	{
		SV* sv = newSVpv($1, strlen($1));
		AV* av = newSCALAR(sv);
		push(av);
	}
     | INT
	{
		SV* sv = newSViv($1);
		AV* av = newSCALAR(sv);
		push(av);
	}
     | DOUBLE
	{
		SV* sv = newSVnv($1);
		AV* av = newSCALAR(sv);
		push(av);
	}
     | var
     | '[' list ']'
	{
		AV *rv = newLIST( (AV*) pop() );
		AV *av = newREF(TYPE_AV, rv);
		push(av);
	}
     | '(' list ')'
	{
		AV *list =  (AV*) pop();
		AV *av = newLIST(list);
		push(av);
	}
     | '{' hash_def '}'
	{
		AV *rv =  (AV*) pop();
		AV *av = newREF(TYPE_HV, rv);
		push(av);
	}
     | bi_operator
     | u_operator
     ;

bi_operator : expr '+' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('+', arg1, arg2);
			push(av);
		}
         | expr '-' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('-', arg1, arg2);
			push(av);
		}
         | expr '*' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('*', arg1, arg2);
			push(av);
		}
         | expr '/' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('/', arg1, arg2);
			push(av);
		}
         | expr '>' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('>', arg1, arg2);
			push(av);
		}
         | expr '<' expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('<', arg1, arg2);
			push(av);
		}
         | expr NUM_LE expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(NUM_LE, arg1, arg2);
			push(av);
		}
         | expr NUM_GE expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(NUM_GE, arg1, arg2);
			push(av);
		}
         | expr NUM_EQ expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(NUM_EQ, arg1, arg2);
			push(av);
		}
         | expr NUM_NE expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(NUM_NE, arg1, arg2);
			push(av);
		}
         | expr STR_EQ expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(STR_EQ, arg1, arg2);
			push(av);
		}
         | expr STR_LE expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(STR_LE, arg1, arg2);
			push(av);
		}
         | expr STR_GE expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(STR_GE, arg1, arg2);
			push(av);
		}
         | expr STR_NE expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(STR_NE, arg1, arg2);
			push(av);
		}
         | expr CONCAT expr
 		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(CONCAT, arg1, arg2);
			push(av);
		}
          | expr PLUS_ASSIGN expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *op = newBIOP('+', arg1, arg2);
			AV *av = newBIOP(ASSIGN, arg1, op);
			push(av);
		}
          | expr MINUS_ASSIGN expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *op = newBIOP('-', arg1, arg2);
			AV *av = newBIOP(ASSIGN, arg1, op);
			push(av);
		}
           | expr MUL_ASSIGN expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *op = newBIOP('*', arg1, arg2);
			AV *av = newBIOP(ASSIGN, arg1, op);
			push(av);
		}
           | expr DIV_ASSIGN expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *op = newBIOP('/', arg1, arg2);
			AV *av = newBIOP(ASSIGN, arg1, op);
			push(av);
		}
          | expr CONCAT_ASSIGN expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *op = newBIOP(CONCAT, arg1, arg2);
			AV *av = newBIOP(ASSIGN, arg1, op);
			push(av);
		}
          | expr RANGE expr
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP(RANGE, arg1, arg2);
			push(av);
		}
		   | expr '%' expr 
		{
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			AV *av = newBIOP('%', arg1, arg2);
			push(av);
		}

       ;

u_operator : '!' expr
		{
			AV *arg =  (AV*) pop();
			AV *av = newUOP('!', arg);
			push(av);
		}
           | INC expr
 		{
			AV *arg =  (AV*) pop();
			AV *av = newUOP(PRE_INC, arg);
			push(av);
		}
           | expr INC	%prec RIGHT_INC
 		{
			AV *arg =  (AV*) pop();
			AV *av = newUOP(POST_INC, arg);
			push(av);
		}
           | DEC expr
        {
			AV *arg =  (AV*) pop();
			AV *av = newUOP(PRE_DEC, arg);
			push(av);
		}
           | expr DEC 	%prec RIGHT_DEC
        {
			AV *arg =  (AV*) pop();
			AV *av = newUOP(POST_DEC, arg);
			push(av);
		}
           | '\\' expr
      		{
			AV *arg =  (AV*) pop();
			AV *av = newUOP('\\', arg);
			push(av);
		}
           | '-' expr   %prec MINUS
 		{
			AV *arg =  (AV*) pop();
			AV *av = newUOP('-', arg);
			push(av);
		}
           ;

hash_def : hash_entry ',' hash_def
		{
			HV *hv =  (HV*) pop();
			AV *arg2 =  (AV*) pop();
			AV *arg1 =  (AV*) pop();
			char *key = SVt_PV(arg1);
			hv_store(hv, key, strlen(key), (SV*) arg2, 0);
			push(hv);
		}
         | hash_entry
         ;

hash_entry : expr HASH_REF expr 
           ;

var : '$' BARE_WORD
	{
		AV *av = newVAR('$', $2, strlen($2));
		push(av);
	}
    | '@' BARE_WORD
	{
		AV *av = newVAR('@', $2, strlen($2));
		push(av);
	}
    | '%' BARE_WORD		%prec UNARY_PERCANTAGE
	{
		AV *av = newVAR('%', $2, strlen($2));
		push(av);
	}
    | '@' BARE_WORD '[' list ']'
	{
		AV* l = (AV*) pop();
		AV *av = newARR('@', '@', $2, strlen($2), newLIST(l));
		push(av);	
	}
    | '$' BARE_WORD '{' expr '}'
	{
		AV* e =  (AV*) pop();
		AV *av = newARR('%', '$', $2, strlen($2), e);
		push(av);
	}
    | '$' BARE_WORD '[' expr ']'
	{
		AV* e = (AV*) pop();
		AV *av = newARR('@', '$', $2, strlen($2), e);
		push(av);
	}
    ;

list : list_item ',' list
	{
		AV *av = (AV*) pop();
		SV *sv = pop();
		av_push(av, newRV_noinc( sv ) );
		push(av);
	}
     | list_item
	{
		AV *item = (AV*) pop();
		AV *av = newAV();
		av_push(av, newRV_noinc( item ) );
		push(av);
	}
     ;

list_item : eexpr
          ;

%%

yyerror(char* s) {
	printf("%s, line: %d\n",s, line_no);
    exit(1);
}

int main() {
	if (!gv_init()) {
		printf("gv_init [failed]\n");
		return 0;
	}

	if (!eval_init()) {
		printf("eval_init [failed]\n");
		return 0;
	}

	stack = newAV();
	if (!stack) {
		printf("stack_init [failed]\n");
		return 0;
	}

	int r = 0;
	while(TRUE){
		if(r == 0){ printf("%d > ", line_no); }
		r = yyparse();
	}
	return 0;
}

