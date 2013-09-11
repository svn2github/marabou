static const char rcsId[] =
	"$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/support_print.c,v 1.8 2004/08/02 13:39:43 hadaq Exp $";
#define _POSIX_C_SOURCE 199309L

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>

#include <stdlib.h>
#include <string.h>

char Print_dummy[500];

char *print_open(char *name)
{

	strcpy(Print_dummy, basename(name));
	strcat(Print_dummy, "(");
	strcat(Print_dummy, getenv("DAQ_BEAMTIME"));
	strcat(Print_dummy, ")");

	/* openlog(Print_dummy,LOG_PERROR | LOG_PID, LOG_LOCAL0); */
	return Print_dummy;
}
