#ifndef GV_H
#define GV_H

#include "kernel.h"
#include "SV.h"
#include "HV.h"

/* helper - dont use it directly */
HV* gv_stash(char* name, U32 klen, int create);

/* Gets the corresponding HV for the glob named
name, and creates it if create is equal to 1. */
HV *gv_stashpv(char* name, int create);

/* Gets the corresponding HV for the glob named
by the SV, and creates it if create is equal to 1. */
HV *gv_stashsv(SV *, int create);

/* inits stash (symbol table hash) */
int gv_init(void);

#endif //GV_H
