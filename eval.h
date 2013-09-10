#ifndef IPERL_EVAL_H
#define IPERL_EVAL_H

#include "kernel.h"
#include "SV.h"
#include "AV.h"
#include "HV.h"
#include "GV.h"
#include "utils.h"
#include "AST.h"

AV *expr_stack;

/* main eval, invoke only this one */
SV* eval(AV* );

SV* evalINST(AV* );

void evalIF(AV* );

void evalFOR(AV* );

SV* evalBIOP(AV* );

SV* evalASSIGN(AV *);

SV* evalUOP(AV* );

SV* evalSUB(AV* );

SV* evalVAR(AV* );

SV** evalARRscalar(AV* );

SV* evalARRslice(AV* );

SV* evalARR(AV* );

SV* evalREF(AV* );

SV* evalSCALAR(AV* );

SV* evalLIST(AV* );

int isVarOrArr(AV* );

int isVar(AV* );

int isArr(AV* );

int isList(AV* );

char* getVarName(AV* );

int eval_init(void);

#endif //IPERL_EVAL_H
