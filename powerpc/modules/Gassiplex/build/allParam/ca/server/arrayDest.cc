static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/arrayDest.cc,v 1.3 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <aitTypes.h>

#include "arrayDest.h"

void ArrayDestructor::run(void *todelete) {
	aitUint32 *pd = (aitUint32 *) todelete;
	delete [] pd;
}

void ArrayIntDestructor::run(void *todelete) {
	aitUint32 *pd = (aitUint32 *) todelete;
	delete [] pd;
}

void ArrayStringDestructor::run(void *todelete) {
	aitString *pd = (aitString *) todelete;
	delete [] pd;
}

