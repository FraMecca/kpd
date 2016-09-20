#ifdef KPD_GC_UTIL
#define KPD_GC_UTIL

#include <gc.h>

/* kpd will use boehm's garbage collector
 * these macros will redefine malloc behaviour
 * free will just point to null, 
 * will be free'd on next sweep
 */

#define malloc(x) GC_malloc(x)
#define calloc(n,x) GC_malloc((n)*(x))
#define realloc(p,x) GC_realloc((p),(x))
#define free(x) (x) = NULL

#endif
