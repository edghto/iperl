#include "HV.h"

HV* newHV(void){
    HV *hv   = (HV*) malloc(sizeof(HV));
	if (!hv)
		return NULL;

    hv->head = (HEAD*) malloc(sizeof(HEAD));
	if (!hv->head) {
		free(hv);
		return NULL;
	}
    hv->body = (BODY*) malloc(sizeof(BODY));
	if (!hv->body) {
		free(hv->head);
		free(hv);
		return NULL;
	}

    hv->head->type       = TYPE_HV;
    hv->head->ref_count  = 1;
    hv->body->hv.entries = NULL;
	hv->body->hv.index   = -1;
	
    hv_clear(hv);

    return hv;
}

SV** hv_store(HV* hv, char* key, U32 klen, SV* val, U32 hashval){
    SV** ptr_sv = hv_fetch(hv, key, klen, 0);

    HE *he = newHE(key, klen, val, hashval);

    if (ptr_sv == NULL) {
        av_push((AV*) hv, (SV*)he);
        ptr_sv = hv_fetch(hv, key, klen, 0);
    } else {
        *ptr_sv = val;
    }

    return ptr_sv;
}

SV** hv_fetch(HV* hv, char* key, U32 klen, I32 lval){
    int i = hv_find(hv, key);
    if (i == -1) {
        return NULL;
    } else {
        HE *he = (HE*) hv->body->hv.entries[i];
        return (SV**) (&he->hent_val);
    }
}

void hv_clear(HV* hv){
    for (int i = av_len(hv); i >= 0 ; i--) {
		HE *he = (HE*) hv->body->hv.entries[i];
		svREFCNT_dec((SV*) he->hent_val);
		free(he);
    }
    free(hv->body->hv.entries);

    hv->body->hv.entries = (PTR*)malloc(sizeof(PTR) * 10);
    hv->body->hv.size    = 10;
    hv->body->hv.index   = -1;
    hv->body->hv.iter    = -1;
}

void hv_undef(HV* hv){
    sv_free((SV*) hv);
}

SV* hv_delete(HV* hv, char* key, U32 klen, I32 flags) {
    int index = hv_find(hv, key);
    if (index == -1)
        return NULL;

    HE *he = (HE*) hv->body->hv.entries[index];
    SV *sv = (SV*) he->hent_val;

    for (int i = index; i < av_len((AV*)hv); i++) {
        hv->body->hv.entries[i] =
                hv->body->hv.entries[i+1];
    }
    hv->body->hv.entries[av_len((AV*)hv)] = (PTR)NULL;

    hv->body->hv.index -= 1;

    return sv;
}

I32 hv_iterinit(HV* hv){
    hv->body->hv.iter = -1;
    return av_len((AV*)hv);
}

SV* hv_iternextsv(HV* hv, char** key, I32* pkeylen){

    hv->body->hv.iter += 1; //next item

    HE *he   = (HE*) hv->body->hv.entries[hv->body->hv.iter];
    *key     = he->hent_hek->hek_key;
    *pkeylen = he->hent_hek->hek_len;

    return (SV*) he->hent_val;
}

HV* perl_get_hv(char* varname, int create){
    HV* hv = NULL;

    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        SV** ptr = hv_fetch(stash, "%", 1, 0);
        if (ptr == NULL) {
            hv = newHV();
            hv_store(stash, "%", 1, hv, 0);
        } else {
            hv = *ptr;
        }
    }

    return hv;
}

HV* perl_set_hv(char *varname, HV* val, int create) {
    HV *hv    = NULL;
    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        hv_store(stash, "%", 1, val, 0);
        hv = val;
    }

    return hv;
}
