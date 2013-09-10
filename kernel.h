#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define I32 int
#define U32 unsigned int
#define PTR uintptr_t

#define TRUE 1
#define FALSE 0

/*
 * Types macro
 */
#define TYPE_SV    0
#define TYPE_AV    1
#define TYPE_HV    2
#define TYPE_UNDEF 3

typedef uintptr_t PTR;

/*
 * Determines if V is SV, HV or AV
 */
typedef struct {
    int type;
    int ref_count;
//    enum {TYPE_SV, TYPE_AV, TYPE_HV} type;
} HEAD;


/*
 * The body of variable depends of type
 */
typedef union {
    struct {
        int iv;        //int value
        double nv;     //double value
        char* pv;      //string value
        int pv_len;    //string length
        PTR rv;        //reference to variable
        int type;      //variable type int, double, string, ref
        int ref_type;  //type of referenced value if is reference
    } sv;
    struct {
        PTR *entries;  //table
        int size;      //size of table
        int index;     //current largest index
    } av;
    struct {
        PTR *entries;  //table
        int size;      //size of table
        int index;     //current largest index
        int iter;      //iterator
    } hv;
} BODY;

/*
 * Basic variable SV, AV, HV inherit from it
 */
typedef struct {
    BODY* body;
    HEAD* head;
} PerlVariable;

/*
 * Types definitions
 */

typedef PerlVariable SV;

typedef PerlVariable AV;

typedef PerlVariable HV;


/*
 *  Hash key
 */
typedef struct {
    char* hek_key;
    U32 hek_hash;
    I32 hek_len;
} HKEY;

/*
 * Hash entry
 */
typedef struct {
    HKEY* hent_next; /* next entry in chain */
    HKEY* hent_hek; /* hash key */
    PTR hent_val; /* scalar value that was hashed */
} HE;

HV *gv_symbol_table;
SV *g_undef;

#endif //KERNEL_H
