static const char rcsId[] =
	"$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/dtu/dtuctrl.c,v 1.17 2003/06/20 15:12:26 hadaq Exp $";
#define _POSIX_C_SOURCE 199309L

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>

#include <stdlib.h>
#include <allParam.h>

#include "../support/blubctrl.h"
#include "dtu_lib.h"

int main(int argc, char *argv[])
{
	static int (*funtab[FunTab_size]) ();

	funtab[init] = Dtu_init;
	funtab[reset] = Dtu_reset;
	funtab[start] = Dtu_start;
	funtab[stop] = Dtu_stop;
	funtab[loadThresholds] = Dtu_loadThresholds;
	funtab[initialized] = Dtu_initialized;
	funtab[running] = Dtu_running;
	funtab[lvl1Busy] = Dtu_lvl1Busy;
	funtab[lvl2Busy] = Dtu_lvl2Busy;
	funtab[error] = Dtu_error;
	funtab[statusRegister] = Dtu_statusRegister;
	funtab[status] = Dtu_status;

	if (-1 == blubctrl(argc, argv, __DATE__ " " __TIME__, funtab)) {
		exit(EXIT_FAILURE);
	} else {
		exit(EXIT_SUCCESS);
	}
}
