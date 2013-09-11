#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <allParam.h>

#include "b_lowercase.h"

int b_lowercase(const char *testid) {
	Param *param;
	char *name = "file";
	char *idx = "rname";
	FILE *result;
	FILE *store;
	size_t size;
	int retVal;
	int myRetVal;
	struct stat fileS, *file = &fileS;

#ifdef VERBOSE
	printf("%s : Param_getBlob: Lowercase input on correct statements.\n", testid);
#endif

	stat("blob/test0.blob", file);

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getBlob(param, name, idx, &size, &result);
	if ((size == file->st_size) && (retVal == 0)) {
		int buf;
		store = fopen("test0.blob", "w+");
		while((buf = fgetc(result)) != EOF) {
			putc(buf, store);
		}
		fclose(store);
		if (!system("diff test0.blob blob/test0.blob")) {
			myRetVal = 0;
			printf("%s : Passed.\n", testid);
		} else {
#ifdef VERBOSE
			printf("%s : Failed. Files \"test0.blob\" and \"blob/test0.blob\" differ.\n", testid);
#else
			printf("%s : Failed.\n", testid);
#endif
			myRetVal = -1;
		}
	fclose(result);
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

