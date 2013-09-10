
#include "AV.h"

AV* newAV(void) {
    AV *av   = (AV*) malloc(sizeof(AV));
	if (!av)
		return NULL;

    av->head = (HEAD*) malloc(sizeof(HEAD));
	if (!av->head) {
		free(av);
		return NULL;
	}
    av->body = (BODY*) malloc(sizeof(BODY));
	if ( !av->body) {
		free(av->head);
		free(av);
		return NULL;
	}
    av->head->type       = TYPE_AV;
    av->head->ref_count  = 1;
    av->body->av.entries = NULL;
	av->body->av.index   = -1;

    av_clear(av);

    return av;
}

void av_extend(AV* av, int length) {
    if (av->body->av.size >= length)
        return;

    PTR *tmp = (PTR*) malloc(sizeof(PTR)*length);

    for (int i = 0; i < av->body->av.size; i++){
        tmp[i] = av->body->av.entries[i];
    }
    for (int i = av->body->av.size; i < length; i++) {
        tmp[i] = (PTR) NULL;
    }


    free(av->body->av.entries);
    av->body->av.entries = tmp;
    av->body->av.size = length;
}

AV* av_make(int num, SV **ptr) {
    AV *av = newAV();
    if (num > av->body->av.size) {
        av_extend(av, num);
    }

    for (int i = 0; i < num; i++){
        av->body->av.entries[i] = (PTR)&ptr;
    }

    av->body->av.index = num - 1;

    return av;
}

I32 av_len(AV* av) {
	if (AV_OK(av))
    	return av->body->av.index;
	else if (HV_OK((HV*) av))
		return av->body->hv.index;
	else
		return 0;
}

SV** av_fetch(AV* av, I32 index, I32 lval) {
    if (av_len(av) < index) {
        return NULL;
    }

    if (lval != 0) {
        av->body->av.entries[index] = (PTR)NULL;
    }

    return (SV**)&(av->body->av.entries[index]);
}

SV** av_store(AV* av, I32 index, SV* val) {
    if (index >= av->body->av.size) {
        av_extend(av, index);
    }

    for (int i = av_len(av); i >= index ; i--) {
        av->body->av.entries[i+1] =
                av->body->av.entries[i];
    }
    av->body->av.entries[index] = (PTR) val;

    if (av->body->av.index < index)
        av->body->av.index = index;
    else
        av->body->av.index += 1;

    return (SV**) &(av->body->av.entries[index]);
}

void av_clear(AV* av) {
    for (int i = av_len(av); i >= 0 ; i--) {
		svREFCNT_dec((SV*)av->body->av.entries[i]);
	}
    free(av->body->av.entries);
    av->body->av.entries = (PTR*)malloc(sizeof(PTR) * 10);
    av->body->av.size    = 10;
    av->body->av.index   = -1;

    for (int i = 0; i < av->body->av.size; i++) {
        av->body->av.entries[i] = (PTR) NULL;
    }
}

void av_undef(AV* av) {
    sv_free((SV*) av);
}

void av_push(AV* av, SV* sv){
    int new_index = av_len(av);

    if ( AV_OK(sv) ) new_index += av_len( sv ) + 1;
    else if( HV_OK(sv) ) printf("Not Yet implemented!\n");//new_index += 2 * ( av_len( sv ) + 1 ) + 1;
    else new_index += 1;

    if (new_index >= av->body->av.size) {
        av_extend(av, av->body->av.size + ( new_index + (10-new_index%10)) );
    }

    if ( AV_OK(sv) ) {
//        memcpy(av->body->av.entries+av->body->av.index+1,
//               sv->body->av.entries, av_len( sv ) + 1);
        for (int i = 0; i <= av_len(sv); i++ )
            av->body->av.entries[av->body->av.index + 1 + i] = 
										(PTR) sv->body->av.entries[i];
    }
    else if( HV_OK(sv) ) {
        printf("Not Yet implemented!\n");
        exit(0);
        char* key;
        I32 klen;
        for (int i = 0; i <= hv_iterinit((HV*) sv); i++) {
            SV *v = (SV*) hv_iternextsv((HV*)sv, &key, &klen);
            av->body->av.entries[av->body->av.index + 1 + 2*i] = (PTR) newSVpv( key, klen );
            av->body->av.entries[av->body->av.index + 1 + 2*i + 1] = (PTR) v;
        }

    }
    else {
        av->body->av.entries[new_index] = (PTR)sv;
    }

    av->body->av.index = new_index;
}

SV* av_pop(AV* av) {
    int last_index = av_len(av);
    if (last_index < 0)
        return NULL;

    SV* sv = (SV*) av->body->av.entries[last_index];
    av->body->av.entries[last_index] = (PTR)NULL;
    av->body->av.index -= 1;

    return sv;
}

SV* av_shift(AV* av) {
    int last_index = av_len(av);
    if (last_index < 0)
        return NULL;

    SV* sv = (SV*) av->body->av.entries[0];

    for (int i = 0; i < av_len(av) - 1; i++){
        av->body->av.entries[i] =
                av->body->av.entries[i+1];
    }
    av->body->av.entries[last_index] = (PTR)NULL;
    av->body->av.index -= 1;

    return sv;
}

void av_unshift(AV* av, I32 num) {
    int new_index = av_len(av) + 1;
    if (new_index == av->body->av.size) {
        av_extend(av, av->body->av.size + 10);
    }

    for (int i = av_len(av); i >= 0 ; i--) {
        av->body->av.entries[i+1] =
                av->body->av.entries[i];
    }

    av->body->av.entries[0] = (PTR) num;
    av->body->av.index = new_index;
}

AV* perl_get_av(char* varname, int create){
    AV* av = NULL;

    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        SV** ptr = hv_fetch(stash, "@", 1, 0);
        if (ptr == NULL) {
            av = newAV();
            hv_store(stash, "@", 1, av, 0);
        } else {
            av = *ptr;
        }
    }

    return av;
}

AV* perl_set_av(char *varname, AV* val, int create) {
    AV *av = NULL;
    HV *stash = gv_stashpv(varname, create);

    if (stash != NULL) {
        hv_store(stash, "@", 1, val, 0);
        av = val;
    }

    return av;
}
