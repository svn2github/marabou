#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <allParam.h>

#include "b_odd_size.h"

int b_odd_size(const char *testid) {
	Param *param;
	char *name = "file";
	char *idx = "rnameodd";
	FILE *result;
	FILE *store;
	size_t size;
	int retVal;
	int myRetVal;
	struct stat fileS, *file = &fileS;

#ifdef VERBOSE
	printf("%s : Param_getBlob: Lowercase input on correct statements.\n", testid);
#endif

	stat("blob/test2.blob", file);

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getBlob(param, name, idx, &size, &result);
	if ((size == file->st_size) && (retVal == 0)) {
		int buf;
		store = fopen("test2.blob", "w+");
		while((buf = fgetc(result)) != EOF) {
			putc(buf, store);
		}
		fclose(store);
		if (!system("diff test2.blob blob/test2.blob")) {
			myRetVal = 0;
			printf("%s : Passed.\n", testid);
		} else {
#ifdef VERBOSE
			printf("%s : Failed. Files \"test2.blob\" and \"blob/test2.blob\" differ.\n", testid);
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

