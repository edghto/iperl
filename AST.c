#include "AST.h"

AV* newIF(AV* cond, AV* if_block) {

    AV* av = newAV();

    SV *sv = newSViv(IF);

    av_push(av, sv);
    av_push(av, newRV_noinc( cond ) );
    av_push(av, newRV_noinc( if_block ) );

    return av;
}

void setElseBlock(AV* av, AV* else_block) {
    av_push(av, newRV_noinc( else_block ) );
}

AV* newFOR(AV* var, AV* list, AV* for_block) {
    AV* av = newAV();

    SV *sv = newSViv(FOR);

    av_push(av, sv);
    av_push(av, newRV_noinc( var ) );
    av_push(av, newRV_noinc( list ) );
    av_push(av, newRV_noinc( for_block ) );

    return av;
}

AV* newBIOP(int op, AV* arg1, AV* arg2) {
    AV* av = newAV();

    SV *sv_type = newSViv(BIOP);
    SV *sv_op = newSViv(op);

    av_push(av, sv_type);
    av_push(av, sv_op);
    av_push(av, newRV_noinc( arg1 ) );
    av_push(av, newRV_noinc( arg2 ) );

    return av;

}

AV* newUOP(int op, AV* arg){
    AV* av = newAV();

    SV *sv_type = newSViv(UOP);
    SV *sv_op = newSViv(op);

    av_push(av, sv_type);
    av_push(av, sv_op);
    av_push(av, newRV_noinc( arg ) );

    return av;
}

AV* newSUB(char* name, U32 len, AV* list){
    AV* av = newAV();

    SV *sv_type = newSViv(SUB);
    SV *sv_sub = newSVpv(name, len);

    av_push(av, sv_type);
    av_push(av, sv_sub);
    av_push(av, newRV_noinc( list ) );

    return av;
}


AV* newVAR(int type, char* name, U32 len){
    AV* av = newAV();

    SV *sv_type = newSViv(VAR);
    SV *var_type = newSViv(type);
    SV *var_name = newSVpv(name, len);

    av_push(av, sv_type);
    av_push(av, var_type);
    av_push(av, var_name);

    return av;
}

AV* newREF(int ref_type, AV* v) {
    AV* av = newAV();

    SV *sv_type = newSViv(REF);
    SV *sv_ref_type = newSViv(ref_type);

    av_push(av, sv_type);
    av_push(av, sv_ref_type);
    av_push(av, newRV_noinc( v ) );

    return av;
}

AV* newARR(int a_type, int v_type, char* name, U32 len, AV* expr) {
    AV* av = newAV();

    SV *sv_type = newSViv(ARR);
    SV *array_type = newSViv(a_type);
    SV *var_type = newSViv(v_type);
    SV *var_name = newSVpv(name, len);

    av_push(av, sv_type);
    av_push(av, array_type);
    av_push(av, var_type);
    av_push(av, var_name);
    av_push(av, newRV_noinc( expr ) );

    return av;
}

AV* newINST(AV* inst) {
    AV* av = newAV();

    SV* sv_type = newSViv(INST);

    av_push(av, sv_type);
    av_push(av, newRV_noinc( inst ) );

    return av;
}

AV* newSCALAR(SV* sv) {
    AV* av = newAV();

    SV* sv_type = newSViv(SCALAR);

    av_push(av, sv_type);
    av_push(av, sv);

    return av;
}

AV* newLIST(AV* list) {
    AV* av = newAV();

    SV* sv_type = newSViv(LIST);

    av_push(av, sv_type);
    av_push(av, newRV_noinc( list ) );

    return av;
}

void dispose(AV* av) {
    //TODO maybe svREFCNT_dec on main node will do the trick
    int m = 0;
    int len = av_len(av);
    switch(SVt_IV(*av_fetch(av, 0, 0))) {
        case BIOP:
        case SUB:
        case REF:
            svREFCNT_dec(*av_fetch(av, 0, 0));
            svREFCNT_dec(*av_fetch(av, 1, 0));
            m = 2;
            break;
        case UOP:
            svREFCNT_dec(*av_fetch(av, 0, 0));
            m = 1;
            break;
        case VAR:
            for (int i = 0; i <= len; i++, svREFCNT_dec(*av_fetch(av, 0, 0)));
            m = len+1;
            break;
        case INST:
        case SCALAR:
        case LIST:
        case FOR:
        case IF:
            svREFCNT_dec(*av_fetch(av, 0, 0));
            m = 1;
            break;
        case ARR:
            svREFCNT_dec(*av_fetch(av, 0, 0));
            svREFCNT_dec(*av_fetch(av, 1, 0));
            svREFCNT_dec(*av_fetch(av, 2, 0));
            m = 3;
            break;
    };

    for (int i = m; i <= len; i++) {
        AV *item = *av_fetch(av, i, 0);
        dispose(item);
        svREFCNT_dec(item);
    }
}
