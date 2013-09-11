#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "f_lowercase_abs.h"

int f_lowercase_abs(const char *testid) {
	Param *param;
	char *name = "file";
	char *idx = "aname";
	char result[PARAM_MAX_VALUE_LEN];
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getFilename: Lowercase input on correct statements.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getFilename(param, name, idx, &rows, result);
	if ((rows == 1) && result && (strcmp(result, "/test0.blob") == 0) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result: \'%s\', return Value: %d.\n", testid, rows, result, retVal);
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

