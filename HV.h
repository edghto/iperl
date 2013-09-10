#ifndef HV_H
#define HV_H

#include "kernel.h"
#include "utils.h"
#include "AV.h"
#include "GV.h"


/* Creates a new, empty HV. */
HV* newHV(void);

/* Stores the pointer to SV against the key.
You must supply the length of the key in
klen. The value of hashval is the hash
value used for storage. If it is 0, then the
value is computed for you. The return
value is a pointer to the location of the
new value within the hash. */
SV** hv_store(HV* , char* key, U32 klen, SV* val, U32 hashval);

/* Fetches an entry from an HV, returning a
pointer to the value. */
SV** hv_fetch(HV* , char* key, U32 klen, I32 lval);

/* Empties the contents of the hash, but does
not delete the hash itself. */
void hv_clear(HV* );

/* Empties the contents of the hash and then
deletes it. */
void hv_undef(HV* hash);

/* Deletes the element specified by key,
returning a mortal copy of the value that
was stored at that position. If you don’t
want this value, then specify
G_DISCARD in the flags element. */
SV* hv_delete(HV* , char* key, U32 klen, I32 flags);

/* Returns the number of elements in the
HV, and prepares for iteration through
the hash. */
I32 hv_iterinit(HV* );

/* Returns the value for the next element in
the hash. The key is placed into the
variable pointed to by key, the physical
length of which is placed into the variable
pointed to by pkeylen. This is the function
used with the keys, values, and each
functions within Perl. */
SV* hv_iternextsv(HV* , char** key, I32* pkeylen);

/* Gets the hash named varname, which
should be a fully qualified package name.
If create is 1, the hash is created.
 */
HV* perl_get_hv(char* , int );

HV* perl_set_hv(char* , HV* , int );

#endif //HV_H
