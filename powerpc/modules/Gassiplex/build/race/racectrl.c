static const char rcsId[] =
  "$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/race/racectrl.c,v 1.13 2003/06/20 15:12:37 hadaq Exp $";
#define _POSIX_C_SOURCE 199309L

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>

#include <stdlib.h>
#include <allParam.h>

#include "../support/blubctrl.h"
#include "race_lib.h"

int main(int argc, char *argv[])
{
  static int (*funtab[FunTab_size]) ();
  
  funtab[init] = Race_init;
  funtab[reset] = Race_reset;
  funtab[start] = Race_start;
  funtab[stop] = Race_stop;
  funtab[loadThresholds] = Race_loadThresholds;
  funtab[initialized] = Race_initialized;
  funtab[running] = Race_running;
  funtab[lvl1Busy] = Race_lvl1Busy;
  funtab[lvl2Busy] = Race_lvl2Busy;
  funtab[error] = Race_error;
  funtab[statusRegister] = Race_statusRegister;
  funtab[status] = Race_status;
  
  if (-1 == blubctrl(argc, argv, __DATE__ " " __TIME__, funtab)) {
    exit(EXIT_FAILURE);
  } else {
    exit(EXIT_SUCCESS);
  }
}
