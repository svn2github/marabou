static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/ca/server/paramServer.cc,v 1.5 2003/01/31 12:25:32 hadaq Exp $";
#define _POSIX_C_SOURCE 199509L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
  #include <unistd.h>

  #include <ctype.h>
  #include <string.h>
  #include <syslog.h>
}

#include <fdManager.h>

#include "paramRecordSet.h"

int main(int argc, char *argv[]) {
	openlog(argv[0], LOG_PERROR | LOG_PID, LOG_LOCAL0);

	ParamRecordSet *cas;

	if (NULL == (cas = new ParamRecordSet(MAX_NUM_PV))) {
		syslog(LOG_ERR, "Cannot allocate memory for the RecordSet. Exiting.\n");
		exit(-1);
	}                                                                       

	cas->setDebugLevel(0u);

	while (aitTrue) {
		osiTime delay(1000u, 0u);
		fileDescriptorManager.process(delay);
	}

	delete cas;
	closelog();
	return 0;
}

