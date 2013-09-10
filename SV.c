#include "SV.h"
#include <assert.h>

SV* newSV(void) {
    SV *s = (SV*) malloc(sizeof(SV));
	if (!s) 
		return NULL;

    s->head = (HEAD*)malloc(sizeof(HEAD));
	if (!s->head) {
		free(s);
		return NULL;
	}
    s->body = (BODY*)malloc(sizeof(BODY));
	if (!s->body) {
		free(s);
		free(s->head);
		return NULL;
	}

    s->head->type      = TYPE_SV;
    s->head->ref_count = 1;

    s->body->sv.iv       = 0;
    s->body->sv.type     = -1;
    s->body->sv.nv       = 0;
    s->body->sv.pv       = NULL;
    s->body->sv.pv_len   = 0;
    s->body->sv.ref_type = -1;
    s->body->sv.rv       = 0;

    return s;
}

SV* newSViv(I32 iv) {
    SV *s = newSV();
    if (!s)
		return NULL;

    s->body->sv.iv   = iv;
    s->body->sv.type = TYPE_IV;

    return s;
}

SV* newSVnv(double nv){
    SV *s = newSV();
    if (!s)
		return NULL;

    s->body->sv.nv   = nv;
    s->body->sv.type = TYPE_NV;

    return s;
}

SV* newSVpv(char* str, int len){
    SV *s = newSV();
    if (!s)
		return NULL;

	s->body->sv.pv_len = len;
    s->body->sv.type   = TYPE_PV;
	s->body->sv.pv     = (char*) malloc(len+1);
	strcpy(s->body->sv.pv, str);

    return s;
}

SV* SvRV(SV* rv) {
    SV* sv = (SV*)rv->body->sv.rv;
    return sv;
}

SV* newSVsv(SV *sv) {
    SV* ret;
    if (sv == NULL || !SV_OK(sv))
        return NULL;

    if (SvIOK(sv)) {
        ret = newSViv(SVt_IV(sv));
    } else if (SvNOK(sv)) {
        ret = newSVnv(SVt_NV(sv));
    } else if (SvPOK(sv)) {
        ret = newSVpv(SVt_PV(sv), sv->body->sv.pv_len);
    } else {
        //it shooud be than RV
        ret = newRV_inc(SvRV(sv));
    }

    return ret;
}

SV* newRV_inc(SV* other){
    SV* rv = newRV_noinc(other);
    other->head->ref_count += 1;

    return rv;
}

SV* newRV_noinc(SV* other){
    SV *rv = newSV();
	if (!rv)
		return NULL;

    rv->body->sv.rv   = (PTR)other;
    rv->body->sv.type = TYPE_RV;

    if (SV_OK(other))
        rv->body->sv.ref_type = TYPE_SV;
    else if (AV_OK(other))
        rv->body->sv.ref_type = TYPE_AV;
    else if (HV_OK(other))
        rv->body->sv.ref_type = TYPE_HV;

    return rv;
}

int SvIOK(SV* sv) {
    if (sv->body->sv.type == TYPE_IV)
        return TRUE;
    else
        return FALSE;
}

int SvNOK(SV* sv) {
    if (sv->body->sv.type == TYPE_NV)
        return TRUE;
    else
        return FALSE;
}

int SvPOK(SV* sv){
    if (sv->body->sv.type == TYPE_PV)
        return TRUE;
    else
        return FALSE;
}

int SvROK(SV *sv){
    if (sv->body->sv.type == TYPE_RV)
        return TRUE;
    else
        return FALSE;
}

int SvOK(SV* sv) {
	return (g_undef != sv ? TRUE : FALSE);
}

int SvTRUE(SV* sv){
	/* if scalar */
    if (SV_OK(sv)) {  
        if (SvPOK(sv) && strlen(SVt_PV(sv)) > 0)
                return TRUE;
        if (SvNOK(sv) && SVt_NV(sv) != 0)
                return TRUE;
        if (SvIOK(sv) && SVt_IV(sv) != 0)
                return TRUE;

	/* if array */
    } else if (AV_OK((AV*) sv)) { 
           if (av_len((AV*) sv) > 0)
            return TRUE;

	/* if hash */
    } else if (HV_OK((HV*) sv)) { 
          if (hv_iterinit((HV*) sv) > 0)
            return TRUE;
    }
    return FALSE;
}

int SVt_IV (SV* sv) {
    I32 val = 0;
    if(SV_OK(sv)) {
        if (SvIOK(sv)) {
            val = sv->body->sv.iv;
        } else if (SvNOK(sv)) {
            val = (int) SVt_NV(sv);
        } else if (SvPOK(sv)) {
			/* 
			 *	It's not compatible with perl,
			 *	it should be 0.
			 */
            val = atoi(SVt_PV(sv));
        }
    } else if (AV_OK(sv) || HV_OK(sv)) {
        val = av_len(sv) + 1;
    }

    return val;
}

double SVt_NV (SV* sv){
    double val = 0;
    if (SV_OK(sv)) {
        if (SvIOK(sv)) {
            val = (double) SVt_IV(sv);
        } else if (SvNOK(sv)) {
            val = sv->body->sv.nv;
        } else if (SvPOK(sv)) {
			/* The same as in SVt_IV. */
            val = atof(SVt_PV(sv));
        }
    } else if (AV_OK(sv) || HV_OK(sv)) {
        val = av_len(sv) + 1;
    }

    return val;
}

/*
 * TODO should it return value or a pointer to value??
 */
char* SVt_PV (SV* sv){
    char *val;
    if(SV_OK(sv)) {
        if (SvIOK(sv)) {
            int iv = SVt_IV(sv);
            val    = (char*) malloc(sizeof(iv)+1);
            sprintf(val,"%d",iv);
        } else if (SvNOK(sv)) {
            double nv = SVt_NV(sv);
            val       = (char*) malloc(sizeof(nv)+1);
            sprintf(val,"%g",nv);
        } else if (SvPOK(sv)) {
            char* pv = sv->body->sv.pv;
            val      = (char*) malloc(strlen(pv)+1);
            sprintf(val,"%s",pv);
        } else if(SvROK(sv)) {
            SV* rv = SvRV(sv);
            val    = (char*) malloc( sizeof(PTR) + 1);
            sprintf(val, "%p", rv);
        }
    } else if (AV_OK(sv) || HV_OK(sv)) {
        val = (char*)malloc(sizeof(int));
        sprintf(val, "%d",av_len(sv) + 1);
    } else {
        //Not a scalar
        val = (char*) malloc( sizeof(PTR) + 1);
        sprintf(val, "%p", sv);
    }

    return val;
}

I32 SvIV(SV* sv) {
    I32 val = SVt_IV(sv);
    sv_setiv(sv, val);

    return val;
}

double SvNV(SV* sv) {
    double val = SVt_NV(sv);
    sv_setnv(sv, val);

    return val;
}

char* SvPV(SV* sv, int len){
    char *val = SVt_PV(sv);
    sv_setpv(sv, val);

    return val;
}

void sv_setiv(SV* sv, int val){
	/* 
	 * Since newSV implementatio (and API) has changed
	 * test are needed to verify this soulution.
	 */
    if (!SvIOK(sv)) {
        if (SvPOK(sv))
            free(sv->body->sv.pv);
        sv->body->sv.type = TYPE_IV;
    }
    sv->body->sv.iv = val;
}

void sv_setnv(SV* sv, double val){
    if (!SvNOK(sv)) {
        if (SvPOK(sv))
            free(sv->body->sv.pv);
        sv->body->sv.type = TYPE_NV;
    }
    sv->body->sv.nv = val;
}

void sv_setsv(SV* dest, SV* src){
    if (src == NULL) {
		/* TODO logging functions instead of printfs */
        printf("src NULL");
        exit(0);
    }

	assert(dest != NULL);
	assert(src  != NULL);
	assert(src  != dest);
    if (dest == src || dest == NULL) {
        printf("dest NULL or dest==src");
        exit(0);
    }

    if (SvIOK(src)) {
        sv_setiv(dest, SVt_IV(src));
    } else if (SvNOK(src)) {
        sv_setnv(dest, SVt_NV(src));
    } else if (SvPOK(src)) {
        sv_setpv(dest, SVt_PV(src));
    } else {
        /* TODO create proper sv_setrv function */
        SV *tmp = newRV_inc(SvRV(src));
		if (tmp) {
			*dest = *tmp;
			free(tmp);
		}
    }
}

void sv_setpv(SV* sv, char *str){
    sv_setpvn(sv, str, strlen(str));
}

void sv_setpvn(SV* sv, char *str, int len) {

/*
 * TODO variable meta data clear
 * If SV is in fact AV or HV memory leak will occur since 
 * av_clear/hv_clear isn't invoked.
 * The same situation happens in  setiv set nv, etc.
 */
    if (!SvPOK(sv)) {
        sv->body->sv.type = TYPE_PV;
    }

    sv->body->sv.pv_len = len;
    sv->body->sv.pv     = (char*) realloc(sv->body->sv.pv, len+1);
    strcpy(sv->body->sv.pv, str);
}

void sv_catpv(SV* sv, char* str) {
    int len = strlen(str);
    svcatpvn(sv, str, len);
}

void svcatpvn(SV* sv, char* str, int len) {
    int str_len = strlen(str);
    if (str_len < len)
        len = str_len;

    char *new_val = (char*) malloc(len+1);
    strncpy(new_val, str, len);
    new_val[len]  = '\0';   //strncpy doesn't append '\0'

    char *old_val = SvPV(sv, 0);
    char *val     = (char*) malloc(strlen(old_val) + len+1);
    sprintf(val, "%s%s", old_val, new_val);

    sv_setpv(sv, val);
    free(new_val);
    free(val);
}

void svcatsv(SV* A, SV* B) {
    SV *tmp = newSVsv(B);
    char *b = SvPV(tmp, 0);

	sv_catpv(A, b);

	free(b);
    sv_free(tmp);
}

void svREFCNT_dec(SV* sv) {
    sv->head->ref_count -= 1;
    if (sv->head->ref_count <= 0)
        sv_free(sv);
}

void sv_free(SV *sv) {
    if (SV_OK(sv)) {
        if(SvPOK(sv))
            free(SVt_PV(sv));
        if(SvROK(sv))
            svREFCNT_dec(SvRV(sv));
    } else if (AV_OK((AV*) sv)) {
        av_clear((AV*) sv);
    } else if (HV_OK((HV*) sv)) {
        hv_clear((HV*) sv);
    }

    free(sv->body);
    free(sv->head);
    free(sv);
}

SV* perl_get_sv(char *varname, int create) {
    SV* sv = NULL;

    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        SV** ptr = hv_fetch(stash, "$", 1, 0);
        if (ptr == NULL) {
            sv = newSViv(0);
            hv_store(stash, "$", 1, sv, 0);
        } else {
            sv = *ptr;
        }
    }

    return sv;
}

SV* perl_set_sv(char *varname, SV* val, int create) {
    SV *sv = NULL;
    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        hv_store(stash, "$", 1, val, 0);
        sv = val;
    }

    return sv;
}

void sv_dump(SV* sv) {
    if (sv == NULL)
        return;

	if(!SvOK(sv)) {
		printf("undef");
		return;
	}

    if (SV_OK(sv)) {
        if (SvIOK(sv)) {
            printf("%d", SVt_IV(sv));
        } else if (SvNOK(sv)) {
            printf("%g", SVt_NV(sv));
        } else if (SvPOK(sv)) {
            printf("\"%s\"", SVt_PV(sv));
        } else if (SvROK(sv)) {
            printf("\\");
            sv_dump(SvRV(sv));
        }
    } else if (AV_OK((AV*)sv)) {
        printf("[ ");
        for (int i = 0; i <= av_len((AV*)sv); i++) {
            if(i>0)
                printf(", ");
            sv_dump((SV*) *av_fetch((AV*)sv, i, 0));
        }
        printf(" ]");
    } else if (HV_OK((HV*)sv)) {
        int len = hv_iterinit((HV*) sv);
        char* key;
        I32 klen;

        printf("{ ");
        for (int i = 0; i <= len; i++) {
            if(i>0)
                printf(", ");

            SV *v = (SV*) hv_iternextsv((HV*)sv, &key, &klen);
            printf("%s => ", key);
            sv_dump(v);
        }
        printf(" }");
    } else {
        printf("%lu\n", (PTR) sv);
    }
}

