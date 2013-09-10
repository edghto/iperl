#include "utils.h"

int SV_OK(SV* sv) {
    if (sv->head->type == TYPE_SV)
        return TRUE;
    else
        return FALSE;
}

int AV_OK(AV* av) {
    if (av->head->type == TYPE_AV)
        return TRUE;
    else
        return FALSE;
}

int HV_OK(HV* hv) {
    if (hv->head->type == TYPE_HV)
        return TRUE;
    else
        return FALSE;
}

HE* newHE(char* key, U32 klen, SV* val, U32 hashval) {
    HE he;
    HKEY hkey;

    hkey.hek_key = (char*) malloc(klen+1);
    strcpy(hkey.hek_key, key);
    hkey.hek_hash = hashval;
    hkey.hek_len = klen;

    he.hent_hek = (HKEY*) malloc(sizeof(HKEY));
    *(he.hent_hek) = hkey;
    he.hent_val = (PTR) val;

    HE *ret = (HE*)malloc(sizeof(HE));
    *ret = he;

    return ret;
}

int hv_find(HV* hv, char* key) {
    for(int i = 0; i<= av_len((AV*) hv); i++) {
        HE *he = (HE*)hv->body->hv.entries[i];
        if (strcmp(key, he->hent_hek->hek_key) == 0) {
            return i;
        }
    }
    return -1;
}
