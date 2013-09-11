#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "s_no_index.h"

int s_no_index(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "bla";
	char result[PARAM_MAX_VALUE_LEN];
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getString: Non-existing idx.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getString(param, name, idx, &rows, result);
	if ((rows == 0) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
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

