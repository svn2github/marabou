#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "i_no_index.h"

int i_no_index(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "bla";
	unsigned long int result;
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getInt: Non-existing idx.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getInt(param, name, idx, &rows, &result);
	if ((rows == 0) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#if VERBOSE
		printf("%s : Failed. Rows: %d, return Value: %d.\n", testid, rows, retVal);
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

