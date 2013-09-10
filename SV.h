#ifndef SV_H
#define SV_H

#include <string.h>

#include "kernel.h"
#include "GV.h"

#define TYPE_IV 0
#define TYPE_NV 1
#define TYPE_PV 2
#define TYPE_RV 3

//for sv_dump only
#include <stdio.h>

/*
 * SV is defined in kernel.h
 */

SV* newSV(void);

/* Creates a new SV from an integer value. */
SV* newSViv(I32);

/* Creates a new SV from a float or double value. */
SV* newSVnv(double);

/* Creates a new SV from a string of length len.
The length will be calculated if len is 0. */
SV* newSVpv(char *str, int len);

/* Duplicates the scalar value. Creating a new
empty scalar value requires the use of the
special sv_undef global scalar.*/
SV* newSVsv(SV *);

/* Creates a new SV and points rv to it. If
pkgname is not null, then it blesses rv into
that package. This is the method used to
create references to scalars. */
SV* newSVrv(SV* rv, char* pkgname);

/* Creates a reference pointing to any type of
value, specified by other, incrementing the
reference count of the entity referred to. Can
be used to create references to any type of
value. */
SV* newRV_inc(SV* other);

/* Creates a reference pointing to any type of
value, specified by other, without
incrementing the reference count of the entity
referred to. Can be used to create references
to any type of value. */
SV* newRV_noinc(SV* other);

/* Returns true if the SV is an IV. */
int SvIOK(SV*);

/* Returns true if the SV is an NV. */
int SvNOK(SV*);

/* Returns true if the SV is a PV. */
int SvPOK(SV*);

/* Returns true if the SV is a reference (RV). */
int SvROK(SV*);

/* Returns true if the SV is not undef. */
int SvOK(SV*);

/* Returns true if the SV is true. */
int SvTRUE(SV*);

/* Returns a value */
int SVt_IV (SV* sv);

/* Returns a value */
double SVt_NV (SV* sv);

/* Returns a value */
char* SVt_PV (SV* sv);

/* Converts an SV to an IV. Returns 0 if the SV
contains a non-numeric string. */
I32 SvIV(SV*);

/* Converts an SV to a double. */
double SvNV(SV*);

/* Converts an SV to a pointer to a string and
updates len with the string’s length. */
char* SvPV(SV*, int len);

/* Dereferences a reference, returning an SV.
This can then be cast to an AV or HV as
appropriate. */
SV* SvRV(SV*);

/* Gives SV an integer value, converting SV to
an IV if necessary.*/
void sv_setiv(SV*, int);

/* Gives SV a double value, converting SV to an
NV if necessary. */
void sv_setnv(SV*, double);

/* Copies dest to src, ensuring that pointers dest != src. */
void sv_setsv(SV* dest, SV* src);

/* Gives SV a string value (assuming a
null-terminated string), converting SV to a
string if necessary. */
void sv_setpv(SV*, char *);

/* Gives SV a string value of length len,
converting SV to a string if necessary. */
void sv_setpvn(SV*, char *, int len);

/* Concatenates the string to the SV. */
void sv_catpv(SV*, char*);

/* Copies len characters from the string,
appending them to the SV. */
void svcatpvn(SV*, char*, int len);

/* Concatenates the SV B to the end of SV A. */
void svcatsv(SV* A, SV* B);

/* Creates a new IV with the value of value, and
points rv to it. If classname is non-null, then
it blesses rv into that package. */
void sv_setref_iv(SV* rv, char *classname, int value);

/* Creates a new NV with the value of value,
and points rv to it. If classname is non-null,
then it blesses rv into that package. */
void sv_setref_nv(SV* rv, char *classname, double value);

/* Creates a new PV with the value of value,
and points rv to it. If classname is non-null,
then it blesses rv into that package. */
void sv_setref_pv(SV* rv, char *classname, char* value);

/* Decrements the reference count for SV,
calling sv_free if the count is 0. */
void svREFCNT_dec(SV*);

/* Blesses rv within the package represented by
stash.*/
SV* sv_bless(SV *rv, HV* stash);

/* Returns 1 if the SV inherits from the class
pkgname. */
int sv_isa(SV*, char *pkgname);

/* Returns 1 if the SV is an object. */
int sv_isobject(SV*);

/* Creates a new blank mortal SV. */
SV* sv_newmortal();

/* Marks an existing SV as mortal. */
SV* sv_2mortal(SV*);

/* Duplicates an existing SV and makes the
duplicate mortal. */
SV* sv_mortalcopy(SV*);

/* free SV */
void sv_free(SV*);

/* Gets the variable name within a Perl script
specified by varname, which should be a
fully qualified reference. If create is set to 1,
then it creates a new scalar variable of that
name.*/
SV* perl_get_sv( char*, int );

SV* perl_set_sv(char*, SV* , int );

/* Pretty-prints a Perl variable (SV, AV, HV, etc.). */
void sv_dump(SV*);

#define say(name)\
    printf("%s: ", #name);sv_dump(name);printf("\n");


#endif //SV_H
