static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/examples/pastore.c,v 1.2 2003/01/31 16:40:18 hadaq Exp $";
#define _POSIX_C_SOURCE 199506L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* !HAVE_CONFIG_H */

#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <allParam.h>

static void usage(const char *name) {
	fprintf(stderr, "%s [-s setup]\n", name);
}

static void cleanup(Param *param) {
	desParam(param);
	free(param);
}

int main(int argc, char * const argv[]) {
	Param *param;
	char mode[4];
	char *setup = NULL;
	char *endptr;
	int c;
	char name[PARAM_MAX_NAME_LEN];
	char idx[PARAM_MAX_NAME_LEN];
	char svalue[PARAM_MAX_VALUE_LEN];
	unsigned long int ivalue;

	while((c = getopt(argc, argv, "s:")) != -1) {
		switch(c) {
			case('s'):
				setup = optarg;
				break;
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	param = malloc(sizeof(Param));
	if(conSetupParam(param, setup)) {
		fprintf(stderr, "%s: Cannot create parameter source with setup %s\n", argv[0], setup);
		exit(EXIT_FAILURE);
	}

	fputs("Mode: ", stdout);
	while (NULL != fgets(mode, 4, stdin)) {
		mode[strlen(mode) - 1] = '\0';
		fputs("Name: ", stdout);
		if(NULL == fgets(name, PARAM_MAX_NAME_LEN, stdin)) {
			cleanup(param);
			exit(EXIT_SUCCESS);
		}
		name[strlen(name) - 1] = '\0';
		fputs("Idx: ", stdout);
		if(NULL == fgets(idx, PARAM_MAX_NAME_LEN, stdin)) {
			cleanup(param);
			exit(EXIT_SUCCESS);
		}
		idx[strlen(idx) - 1] = '\0';
		fputs("Value: ", stdout);
		if(NULL == fgets(svalue, PARAM_MAX_VALUE_LEN, stdin)) {
			cleanup(param);
			exit(EXIT_SUCCESS);
		}
		svalue[strlen(svalue) - 1] = '\0';
		if (strcmp(mode, "s") == 0) {
			if (Param_storeString(param, name, idx, svalue)) {
				fprintf(stderr, "%s: Failed\n", argv[0]);
			}
		} else if (strcmp(mode, "i") == 0) {
			ivalue = strtoul(svalue, &endptr, 0);
			if(*endptr != '\0') {
				fprintf(stderr, "%s: %s is not a valid number\n", argv[0], svalue);
			} else {
				if (Param_storeInt(param, name, idx, ivalue)) {
					fprintf(stderr, "%s: Failed\n", argv[0]);
				}
			}
		} else {
			fprintf(stderr, "%s: Mode must be either 's' or 'i', not %s\n", argv[0], mode);
		}
		fputs("Mode: ", stdout);
	}
	cleanup(param);
	return 0;
}
