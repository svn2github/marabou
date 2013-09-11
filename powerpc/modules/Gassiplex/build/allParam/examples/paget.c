static const char rcsId[] = "$Header: /u/hadaq/CVS/allParam/examples/paget.c,v 1.2 2003/01/31 16:40:18 hadaq Exp $";
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
	int i;
	char name[PARAM_MAX_NAME_LEN];
	char idx[PARAM_MAX_NAME_LEN];
	char scount[PARAM_MAX_NAME_LEN];
	int count;
	int rows;
	char svalue[PARAM_MAX_VALUE_LEN];
	unsigned long int ivalue;
	char *savalue[PARAM_MAX_ARRAY_LEN];
	unsigned long int iavalue[PARAM_MAX_ARRAY_LEN];

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
		if (strcmp(mode, "s") == 0) {
			if (Param_getString(param, name, idx, &rows, svalue) || (rows != 1)) {
				fprintf(stderr, "%s: Parameter name=%s, idx=%s not found\n", argv[0], name, idx);
			} else {
				fprintf(stdout, "%s\n", svalue);
			}
		} else if (strcmp(mode, "i") == 0) {
			if (Param_getInt(param, name, idx, &rows, &ivalue) || (rows != 1)) {
				fprintf(stderr, "%s: Parameter name=%s, idx=%s not found\n", argv[0], name, idx);
			} else {
				fprintf(stdout, "%lu\n", ivalue);
			}
		} else if (strcmp(mode, "sa") == 0) {
			fputs("Count: ", stdout);
			if(NULL == fgets(scount, PARAM_MAX_NAME_LEN, stdin)) {
				cleanup(param);
				exit(EXIT_SUCCESS);
			}
			count = strtoul(scount, &endptr, 0);
			if(*endptr != '\n') {
				fprintf(stderr, "%s: %s is not a valid number\n", argv[0], scount);
			} else {
				for (i = 0 ; i < count ; i++) {
					savalue[i] = malloc(PARAM_MAX_VALUE_LEN);
				}
				if (Param_getStringArray(param, name, idx, count, &rows, savalue)) {
					fprintf(stderr, "%s: Parameter name=%s, idx=%s not found\n", argv[0], name, idx);
				} else {
					if (rows != count) {
						fprintf(stderr, "%s: %d, not %d items found\n", argv[0], rows, count);
					}
					for (i = 0 ; i < rows ; i++) {
						fprintf(stdout, "%d: %s\n", i, savalue[i]);
					}
				}
				for (i = 0 ; i < count ; i++) {
					free(savalue[i]);
				}
			}
		} else if (strcmp(mode, "ia") == 0) {
			fputs("Count: ", stdout);
			if(NULL == fgets(scount, PARAM_MAX_NAME_LEN, stdin)) {
				cleanup(param);
				exit(EXIT_SUCCESS);
			}
			count = strtoul(scount, &endptr, 0);
			if(*endptr != '\n') {
				fprintf(stderr, "%s: %s is not a valid number\n", argv[0], scount);
			} else {
				if (Param_getIntArray(param, name, idx, count, &rows, iavalue)) {
					fprintf(stderr, "%s: Parameter name=%s, idx=%s not found\n", argv[0], name, idx);
				} else {
					if (rows != count) {
						fprintf(stderr, "%s: %d, not %d items found\n", argv[0], rows, count);
					}
					for (i = 0 ; i < rows ; i++) {
						fprintf(stdout, "%d: %lu\n", i, iavalue[i]);
					}
				}
			}
		} else {
			fprintf(stderr, "%s: Mode must be one of 's', 'i', 'sa', 'ia', not %s\n", argv[0], mode);
		}
		fputs("Mode: ", stdout);
	}
	cleanup(param);
	return 0;
}
