#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "s_array.h"

int s_array(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tstringa";
	char result[PARAM_MAX_VALUE_LEN];
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getString: Request for Array value.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getString(param, name, idx, &rows, result);
	if ((rows == 1) &&
		result && (strcmp(result, "test1") == 0) &&
		(retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result : \'%s\', return Value: %d.\n", testid, rows, result, retVal);
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

