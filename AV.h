#ifndef AV_H
#define AV_H

#include "kernel.h"
#include "GV.h"

/* Creates a new, empty AV. */
AV* newAV(void);

/* Creates a new AV populated with the SVs
contained in **ptr. */
AV* av_make(int num, SV **ptr);

/* Returns the highest index of the array. Note that
this is not the number of elements in the array;
indexes start at 0, so the value is equivalent to
scalar(@array)–1, or $#array. */
I32 av_len(AV*);

/* Returns a pointer to the SV at location index
from an AV. Because a pointer is returned, you
can use the reference to update as well as access
the value stored. If lval is non-zero, then it
replaces the value at index with undef. */
SV** av_fetch(AV *, I32 index, I32 lval);

/* Stores an SV val at index within an AV and
returns the pointer to the new element. */
SV** av_store(AV*, I32 index, SV* val);

/* Deletes the references to all the SVs in an AV,
but does not delete the AV. */
void av_clear(AV*);

/* Deletes the AV. */
void av_undef(AV*);

/* Increases the size of the AV to num elements. If
num is less than the current number of elements,
it does nothing. */
void av_extend(AV*, int num);

/* Pushes an SV onto the end of the AV. This is
identical to the Perl push function. */
void av_push(AV*, SV*);

/* Pops an SV off the end of an AV. This is identical
to the Perl pop function. */
SV* av_pop(AV*);

/* Returns the first SV from an AV, deleting the
first element. This is identical to the Perl shift
function. */
SV* av_shift(AV*);

/* Adds num elements to the end of an AV, but
does not actually store any values. Use av_store
to actually add the SVs onto the end of the AV.
This is not quite identical to the operation of the
unshift function.*/
void av_unshift(AV*, I32 num);

/* Gets the AV called varname, which should be a
fully qualified name, without the leading @ sign.
Creates an AV with the specified name if create
is 1. */
AV* perl_get_av(char* , int );

AV* perl_set_av(char* , AV* , int );

#endif //AV_H
