#include "eval.h"

int eval_init(void) {
    expr_stack = newAV();
	return expr_stack ? TRUE : FALSE;
}

SV* eval(AV* av) {
    say(av);
    AV* ret;

    switch(SVt_IV(*av_fetch(av, 0, 0))) {
        case FOR:
            evalFOR(av);
            ret = newSViv(TRUE);
            break;
        case IF:
            evalIF(av);
            ret = newSViv(TRUE);
            break;
        case BIOP:
            ret = (SVt_IV(*av_fetch(av, 1, 0))==ASSIGN?evalASSIGN(av):evalBIOP(av));
            break;
        case UOP:
            ret = evalUOP(av);
            break;
        case SUB:
            ret = evalSUB(av);
            break;
        case VAR:
            ret = evalVAR(av);
            break;
        case REF:
            ret = evalREF(av);
            break;
        case INST:
            ret = evalINST(av);
            break;
        case SCALAR:
            ret = evalSCALAR(av);
            break;
        case LIST:
            ret = evalLIST(av);
            break;
        case ARR:
            ret = evalARR(av);
            break;
        default:
            printf( "eval - Not yet implemented\n%d\n", av_len(av));
            exit(1);
    };

    return ret;
}

SV* evalINST(AV* av) {
    SV *ret;
    for (int i = 1; i <= av_len(av); i++) {
        ret = eval( SvRV( *av_fetch(av, i, 0) ) );
    }

    return ret;
}

void evalIF(AV* av) {
    SV *cond = eval( SvRV( *av_fetch(av, 1, 0) ) );
    if (SvTRUE(cond)) {
        eval( SvRV( *av_fetch(av, 2, 0) ) );
    } else if (av_len( SvRV( *av_fetch(av, 3, 0) ) ) != -1) {
        eval( SvRV( *av_fetch(av, 3, 0) ) );
    }
}

void evalFOR(AV* av) {
    AV *block = SvRV( av_pop(av) );
    AV *list = (AV*)eval( SvRV( av_pop(av) ) );
    AV *var = SvRV( av_pop(av) );
    SV *v = NULL;
    SV *underscore = perl_get_sv("_", 1);
    int len = av_len(list);

    if (av_len(var) != -1)
        v = evalVAR(var);

    for (int i = 0 ; i <= len; i++) {
        if (v != NULL)
            sv_setsv(v, (SV*) *av_fetch(list, i, 0));

        sv_setsv(underscore, (SV*) *av_fetch(list, i, 0));

        eval(block);
    }
}

SV* evalBIOP(AV* av) {
    SV* sv_op = *av_fetch(av, 1, 0);
    SV* arg1 = eval(SvRV( *av_fetch(av, 2, 0) ) );
    SV* arg2 = eval(SvRV( *av_fetch(av, 3, 0) ) );

    double arg1_num = SVt_NV(arg1);
    double arg2_num = SVt_NV(arg2);
    char *arg1_str = SVt_PV(arg1);
    char *arg2_str = SVt_PV(arg2);

    SV* sv;
    double result;

    switch(SVt_IV(sv_op)) {
        case '<':
            if (arg1_num < arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case '>':
            if (arg1_num > arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case NUM_EQ:
            if (arg1_num == arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case NUM_LE:
            if (arg1_num <= arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case NUM_GE:
            if (arg1_num >= arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case NUM_NE:
            if (arg1_num != arg2_num)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case STR_EQ:
            if (strcmp(arg1_str, arg2_str) == 0)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case STR_LE:
            if (strcmp(arg1_str, arg2_str) < 0)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case STR_GE:
            if (strcmp(arg1_str, arg2_str) > 0)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case STR_NE:
            if (strcmp(arg1_str, arg2_str) != 0)
                sv = newSViv(TRUE);
            else
                sv = newSViv(FALSE);
            break;
        case CONCAT:
            sv = newSVsv(arg1);
            sv_catpv(sv, arg2_str);
            break;
        case '+':
            result = arg1_num + arg2_num;
            if (result != (int) result)
                sv = newSVnv(result);
            else
                sv = newSViv((int)result);
            break;
        case '-':
            result = arg1_num - arg2_num;
            if (result != (int) result)
                sv = newSVnv(result);
            else
                sv = newSViv((int)result);
            break;
        case '*':
            result = arg1_num * arg2_num;
            if (result != (int) result)
                sv = newSVnv(result);
            else
                sv = newSViv((int)result);
            break;
        case '/':
            result = arg1_num / arg2_num;
            if (result != (int) result)
                sv = newSVnv(result);
            else
                sv = newSViv((int)result);
            break;
        case RANGE:
            sv = newAV();

            for ( result = arg1_num; result <= arg2_num; result+=1 )
            {
                av_push( sv, newSViv(result) );
            }
            break;
        case '%':
            sv = newSViv((long  int)arg1_num % (long int)arg2_num);
        case ASSIGN:
            break;
        default:
            sv = newSViv(FALSE);
    };

    return sv;
}

int isVarOrArr(AV* av) {
    if (isVar(av) || isArr(av))
        return TRUE;
    else
        return FALSE;
}

int isVar(AV* av) {
    if (SVt_IV(*av_fetch(av, 0, 0)) == VAR)
        return TRUE;
    else
        return FALSE;
}

int isArr(AV* av) {
    if (SVt_IV(*av_fetch(av, 0, 0)) == ARR)
        return TRUE;
    else
        return FALSE;
}

int isList(AV* av) {
    if (SVt_IV(*av_fetch(av, 0, 0)) == LIST)
        return TRUE;
    else
        return FALSE;
}

//SV* evalASSIGN(AV *av) {
//    int var = isVarOrArr(*av_fetch(av, 2, 0));
//    SV* arg2 = eval(*av_fetch(av, 3, 0));
//
//    SV* sv;
//    if (!var) {
//        printf("Left side of '=' must be variable!\n");
//        exit(1);
//    }
//    char* name = getVarName(*av_fetch(av, 2, 0));
//
//    if(isVar(*av_fetch(av, 2, 0))) {
//        SV* arg1 = eval(*av_fetch(av, 2, 0));
//        if (SV_OK(arg1) && SV_OK(arg2)) {
//            perl_set_sv(name, arg2, 1);
//        } else if (AV_OK(arg1) && AV_OK(arg2)) {
//            perl_set_av(name, arg2, 1);
//        } else if (HV_OK(arg1) && HV_OK(arg2)) {
//            perl_set_hv(name, arg2, 1);
//        } else {
//            printf("Incompatible types in assignment!\n");
//            exit(1);
//        }
//    } else {
//        SV** ptr = evalARRscalar(*av_fetch(av, 2, 0));
//        *ptr = arg2;
//    }
//    sv = (SV*)arg2;
//
//    return sv;
//}

SV* evalASSIGN(AV *av) {
    SV* left = SvRV( *av_fetch(av, 2, 0) );
    SV* right = SvRV( *av_fetch(av, 3, 0) );
    char* name;
    SV* arg1;
    SV* arg2;
    SV** ptr;
    SV* sv;
    int incompatible = TRUE;

    if (!isVarOrArr(left)) {
        printf("Left side of '=' must be variable!\n");
        exit(1);
    }

    name = getVarName( left );
    arg2 = eval(right);

    if(isVar(left)) {
        arg1 = eval(left);
        if (SV_OK(arg1)) {
            svREFCNT_dec(arg1);
            if (SV_OK(arg2)) {
                perl_set_sv(name, newSVsv(arg2), 1);
                incompatible = FALSE;
            } else if (AV_OK(arg2) || HV_OK(arg2)) {
                perl_set_sv(name, newSViv(av_len(arg2)+1), 1);
                incompatible = FALSE;
            }
        } else if (AV_OK(arg1)) {
            svREFCNT_dec(arg1);
            if (AV_OK(arg2)) {
                arg1 = newAV();
                int i;
                for (i = 0; i <= av_len(arg2); i++)
                    av_push(arg1, newSVsv(*av_fetch(arg2, i, 0)));

                perl_set_av(name, arg1, 1);
                incompatible = FALSE;
            }
        } else if (HV_OK(arg1)) {
            if (AV_OK(arg2)) {
                svREFCNT_dec(arg1);
                arg1 = newHV();
                int i, len = av_len(arg2);
                if ( 0 != (len+1) % 2 ) {
                    SV *key = newSVsv(*av_fetch(arg2, len, 0));
                    hv_store(arg1, SVt_PV(key), strlen(SVt_PV(key)), newSViv(0), 0 );
                    len -= 1;
                }
//TO CHECK !!!!!!!
                for (i = 0; len != 0 && i <= len; i+=2) {
                    SV *key = newSVsv(*av_fetch(arg2, i, 0));
                    SV *val = newSVsv(*av_fetch(arg2, i+1, 0));
                    hv_store(arg1, SVt_PV(key), strlen(SVt_PV(key)), val, 0 );
                }
                perl_set_hv(name, arg1, 1);
                incompatible = FALSE;
            } else if (HV_OK(arg2)) {
                svREFCNT_dec(arg1);
                //foreach pair make copy
                incompatible = FALSE;
            }
        }
    } else if (isArr(left)){
        if (SVt_IV(*av_fetch(left, 2, 0)) == '$') {
            ptr = evalARRscalar(left);
            if (SV_OK(arg2)) {
                *ptr = newSVsv(arg2);
                incompatible = FALSE;
            } else if (AV_OK(arg2) || HV_OK(arg2)) {
                *ptr = newSViv(av_len(arg2)+1);
                incompatible = FALSE;
            }
        } else if(SVt_IV(*av_fetch(left, 2, 0)) == '@') {
            if (AV_OK(arg2)) {
                arg1 = evalARRslice(left);
                int i, min = (av_len(arg1) < av_len(arg2)?av_len(arg1): av_len(arg2));
                for (i = 0; i <= min; i++) {
                    PTR p = (PTR)SVt_IV(*av_fetch(arg1, i, 0));
                    ptr = (SV**) p;
                    *ptr = newSVsv(*av_fetch(arg2, i, 0));
                }
                incompatible = FALSE;
            }
        }
        //TODO hash slices
    }


    if (incompatible) {
        printf("Incompatible types in assignment!\n");
        exit(1);
    }
    sv = (SV*)arg2;

    return sv;
}

SV* evalUOP(AV* av) {
    SV* sv_op = *av_fetch(av, 1, 0);
    SV* arg1 = eval(SvRV( *av_fetch(av, 2, 0) ) );
    double old_val = SVt_NV(arg1);
    double result;

    SV* sv = NULL;

    switch(SVt_IV(sv_op)) {
        case '!':
            if (SvTRUE(arg1))
                sv = newSViv(FALSE);
            else
                sv = newSViv(TRUE);
            break;
        case '\\':
            sv = newRV_inc(arg1);
            break;
        case POST_INC:
            old_val += 1;
            sv = newSVsv(arg1);
            sv_setnv(arg1, old_val);
            break;
        case PRE_INC:
            old_val += 1;
            sv_setnv(arg1, old_val);
            sv = arg1;
            break;
        case POST_DEC:
            old_val -= 1;
            sv = newSVsv(arg1);
            sv_setnv(arg1, old_val);
            break;
        case PRE_DEC:
            old_val -= 1;
            sv_setnv(arg1, old_val);
            sv = arg1;
            break;
        case '-':
            result = - SVt_NV(arg1);
            if (result != (int) result)
                sv = newSVnv(result);
            else
                sv = newSViv((int)result);
            break;
        default:
            sv = newSViv(FALSE);
    };

    return sv;
}

AV* evalSUB(AV* av) {
    printf( "evalSUB - Not yet implemented\n" );
    exit(1);
}

char* getVarName(AV* av) {
    if (isVar(av)) {
        return SVt_PV(*av_fetch(av, 2, 0));
    } else {
        return SVt_PV(*av_fetch(av, 3, 0));
    }
}

SV* evalVAR(AV* av) {
    int type = SVt_IV(*av_fetch(av, 1, 0));
    char* varname = getVarName(av);
    SV *v;

    switch(type) {
        case '$':
            v = perl_get_sv(varname, 1);
            break;
        case '@':
            v = perl_get_av(varname, 1);
            break;
        case '%':
            v = perl_get_hv(varname, 1);
            break;
        default:
            printf( "evalVAR - Not yet implemented\n" );
            exit(1);
    }

    return (SV*) v;
}

SV** evalARRscalar(AV* av) {
    int array_type = SVt_IV(*av_fetch(av, 1, 0));
    char* varname = getVarName(av);
    SV *index = eval(SvRV( *av_fetch(av, 4, 0) ) );

    char *key;
    SV** ptr;
    AV *vv;

    switch(array_type) {
        case '@':
			vv = perl_get_av(varname, 1);
            ptr = av_fetch(vv, SVt_IV(index), 0);
			if (!ptr)
				ptr = &g_undef;
            break;
        case '%':
            vv = perl_get_hv(varname, 1);
            key = SVt_PV(index);
            ptr = hv_fetch(vv, key, strlen(key), 0);
            if (!ptr)
                ptr = &g_undef;
            break;
        default:
            printf( "evalARRscalar - Not yet implemented\n" );
            exit(1);
    }

    return (SV**) ptr;
}

SV* evalARRslice(AV* av) {
    int array_type = SVt_IV(*av_fetch(av, 1, 0));
    char* varname = getVarName(av);
    SV *index = eval(SvRV( *av_fetch(av, 4, 0) ) );

    AV *items;
    AV *vv;
    int i;

    switch(array_type) {
        case '@':
            vv = perl_get_av(varname, 1);
            items = newAV();
            for (i = 0; i <= av_len(index); i++) {
                av_push(items,
                        newSViv((PTR)av_fetch(vv, SVt_IV(*av_fetch(index, i, 0)),0))
                        );
            }
            break;
        default:
            printf( "evalARRslice - Not yet implemented\n" );
            exit(1);
    }

    return (SV*) items;
}

SV* evalARR(AV* av) {
    int var_type = SVt_IV(*av_fetch(av, 2, 0));
    SV *sv;
    AV *slice;
    AV *new_slice;
    PTR item;
    int i;

    switch(var_type) {
        case '$':
            sv = (SV*) *evalARRscalar(av);
            break;
        case '@':
        case '%':
            slice = evalARRslice(av);
            new_slice = newAV();
            for (i = 0; i <= av_len(slice); i++) {
                item = (PTR) SVt_IV(*av_fetch(slice, i, 0));
                av_push(new_slice, *((SV**)item));
            }
            sv = new_slice;
            break;
        default:
            printf( "evalAAR - Not yet implemented\n" );
            exit(1);
    }

    return sv;
}

SV* evalREF(AV* av) {
     return newRV_inc( eval( SvRV( *av_fetch(av, 2, 0) ) ) );
}

SV* evalSCALAR(AV* av) {
    return *av_fetch(av, 1, 0);
}

SV* evalLIST(AV* av) {
    AV *list = newAV();
    AV *old_list = SvRV( av_pop(av) );
    int len = av_len(old_list);

    for (int i = 0; i <= len; i++) {
        av_push( list, eval( SvRV( av_pop( old_list ) ) ) );
    }

    return (SV*)list;
}
