#ifndef IPERL_UTILS_H
#define IPERL_UTILS_H

#include "kernel.h"
#include "AV.h"
#include <string.h>



int SV_OK(SV* );

int AV_OK(AV* );

int HV_OK(HV* );

HE* newHE(char* , U32 , SV* , U32 );

int hv_find(HV*, char* key);

#endif //IPERL_UTILS_H
