#include "GV.h"


HV* gv_stash(char* name, U32 klen, int create) {
    SV** sv = hv_fetch(gv_symbol_table, name, klen, 0);
    HV* hv = NULL;

    if (sv != NULL) {
        hv = (HV*) *sv;
    } else if (create == 1) {
        hv = newHV();
        hv_store(gv_symbol_table, name, klen, hv, 0);
    }

    return hv;
}

HV *gv_stashpv(char* name, int create) {
    return gv_stash(name, strlen(name), create);
}

HV *gv_stashsv(SV *sv, int create) {
    char *name = SvPV(sv, 0);
    return gv_stash(name, strlen(name), create);
}

int gv_init(void) {
    gv_symbol_table = newHV();
	if (!gv_symbol_table)
		return FALSE;

    g_undef = newSV();
	g_undef->head->type = TYPE_UNDEF;
	if (!g_undef) {
		svREFCNT_dec(gv_symbol_table);
		return FALSE;
	}
	
	return TRUE;
}
