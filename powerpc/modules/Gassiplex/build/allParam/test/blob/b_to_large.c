#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <allParam.h>

#include "b_to_large.h"

#define EPICS_MAX_INDEX 4092

int b_to_large(const char *testid) {
	Param *param;
	char *name = "file";
	char *idx = "rnamelarge";
	FILE *result;
	FILE *store;
	size_t size;
	int retVal;
	int myRetVal;
	struct stat fileS, *file = &fileS;

#ifdef VERBOSE
	printf("%s : Param_getBlob: File larger than %d bytes (EPICS_MAX_INDEX-limitation).\n", testid, (EPICS_MAX_INDEX - 1) * sizeof(unsigned long int));
#endif

	stat("blob/test1.blob", file);

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getBlob(param, name, idx, &size, &result);
	if ((size == file->st_size) && (retVal == 0)) {
		int buf;
		store = fopen("test1.blob", "w+");
		while((buf = fgetc(result)) != EOF) {
			putc(buf, store);
		}
		fclose(store);
		if (!system("diff test1.blob blob/test1.blob")) {
			myRetVal = 0;
			printf("%s : Passed.\n", testid);
		} else {
#ifdef VERBOSE
			printf("%s : Failed. Files \"test1.blob\" and \"blob/test1.blob\" differ.\n", testid);
#else
			printf("%s : Failed.\n", testid);
#endif
			myRetVal = -1;
		}
	fclose(result);
	} else if ((size == 0) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Size: %d, return Value: %d.\n", testid, size, retVal);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}

	desParam(param);
	free(param);

	return myRetVal;
}

