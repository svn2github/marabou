#define C_STYLE_PROTOS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "byte_order.h"
#include "mbsio.h"
#include "mbsio_protos.h"

void stop(int signo);
	
int main(int argc, char *argv[])

{
	MBSDataIO *mbs;
	int i;
	int nevts;
	unsigned int etype;
	
	signal(SIGINT, stop);
	
	if (argc != 4) {
		fprintf(stderr, "\nmbswrite: Wrong number of args\n\n");
		fprintf(stderr, "        Usage: mbswrite ppc file nevts\n\n");
		fprintf(stderr, "               ppc   :  powerpc to connect to\n");
		fprintf(stderr, "               file  :  file to write data to\n");
		fprintf(stderr, "                        ext = .lmd -> standard lmd format\n");
		fprintf(stderr, "                        ext = .med -> marabou special format\n");
		fprintf(stderr, "               nevts :  number of events (0 = infinity)\n");
		exit(1);
	}

	mbs = mbs_open_file(argv[1], "S", 0, stdout);	// open connection to ppc
	if (mbs == NULL) exit(1);

	mbs_open_lmd(argv[2]);
	
	nevts = atoi(argv[3]);
	if (nevts == 0) nevts = 1000000000;
	for (i = 0; i < nevts; i++) {			// decode events
		etype = mbs_next_event(mbs);
		if (etype == MBS_ETYPE_EOF) {
			printf("\nmbstst: [EOF] End of file\n");
			break;
		}
	}
	mbs_close_lmd();
	
	exit(0);
}

void stop(int signo) {
	fputs("\nclosing lmd file.\n", stderr);
	mbs_close_lmd();
	exit(0);
}

