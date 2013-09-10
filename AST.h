#ifndef IPERL_AST_H
#define IPERL_AST_H

#include "syntax.tab.h"
#include "kernel.h"
#include "AV.h"
#include "SV.h"

#define BIOP     1
#define UOP      2
#define SUB      3
#define VAR      4
#define REF      5
#define ARR      6
#define INST     7
#define SCALAR   8
#define LIST     9
#define POST_INC 10
#define PRE_INC  11
#define POST_DEC 12
#define PRE_DEC  13



AV* newIF(AV* , AV* );

void setElseBlock(AV* , AV* );

AV* newFOR(AV* , AV* , AV* );

AV* newBIOP(int , AV* , AV* );

AV* newUOP(int , AV* );

AV* newSUB(char* , U32 , AV* );

AV* newVAR(int , char* , U32 );

AV* newREF(int , AV* );

AV* newARR(int , int , char* , U32 , AV*);

AV* newINST(AV* );

AV* newSCALAR(SV* );

AV* newLIST(SV* );

void dispose(AV* );

#endif //IPERL_AST_H
