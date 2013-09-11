static const char rcsId[] = "$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/race/rc.c,v 1.14 2003/07/07 17:42:13 sailer Exp $";

#define _POSIX_C_SOURCE 199309L

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>

#include <stdlib.h>
#include <syslog.h>

#include <lvme.h>

#include "rc.h"

int Rc_invariant(Rc *my)
{
	int retVal;

	retVal = 1;
	if ((my->memRegion == NULL) ||
		(my->busRegion == NULL) || (my->regRegion == NULL) || (my->xcRegion == NULL) || (my->dprRegion == NULL)
		) {
		retVal = 0;
	}
	return retVal;
}

int conRc(Rc *my, const char *name, unsigned long vmeBase)
{
	int retVal = 0;
	if (NULL == (my->memRegion = malloc(sizeof(LVme)))) {
		syslog(LOG_ERR, "%s: Malloc failed for conRc", name);
		retVal = -1;
	} else if (NULL == (my->busRegion = malloc(sizeof(LVme)))) {
		syslog(LOG_ERR, "%s: Malloc failed for conRc", name);
		retVal = -1;
		free(my->memRegion);
	} else if (NULL == (my->regRegion = malloc(sizeof(LVme)))) {
		syslog(LOG_ERR, "%s: Malloc failed for conRc", name);
		retVal = -1;
		free(my->memRegion);
		free(my->busRegion);
	} else if (NULL == (my->xcRegion = malloc(sizeof(LVme)))) {
		syslog(LOG_ERR, "%s: Malloc failed for conRc", name);
		retVal = -1;
		free(my->memRegion);
		free(my->busRegion);
		free(my->regRegion);
	} else if (NULL == (my->dprRegion = malloc(sizeof(LVme)))) {
		syslog(LOG_ERR, "%s: Malloc failed for conRc", name);
		retVal = -1;
		free(my->memRegion);
		free(my->busRegion);
		free(my->regRegion);
		free(my->xcRegion);
	} else {

		if ((NULL == (my->memRegion = newLVme(vmeBase + memRegion, memSpace, 0x09)))
			|| (NULL == (my->busRegion = newLVme(vmeBase + busRegion, busSpace, 0x09)))
			|| (NULL == (my->regRegion = newLVme(vmeBase + regRegion, regSpace, 0x09)))
			|| (NULL == (my->xcRegion = newLVme(vmeBase + xcRegion, xcSpace, 0x09)))
			|| (NULL == (my->dprRegion = newLVme(vmeBase + dprRegion, dprSpace, 0x09)))
			) {
			free(my->memRegion);
			free(my->busRegion);
			free(my->regRegion);
			free(my->xcRegion);
			free(my->dprRegion);
			syslog(LOG_ERR, "%s: creation of LVme instance failed", name);

			retVal = -1;
		}
	}

	return retVal;
}

void desRc(Rc *my)
{
	delLVme(my->memRegion);
	free(my->memRegion);

	delLVme(my->busRegion);
	free(my->busRegion);

	delLVme(my->regRegion);
	free(my->regRegion);

	delLVme(my->xcRegion);
	free(my->xcRegion);

	delLVme(my->dprRegion);
	free(my->dprRegion);
}
