#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "ia_string_available.h"

#define NUM1 8

int ia_string_available(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tstringa";
	unsigned long int result[NUM1];
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getIntArray: Int is wanted but String-like values found.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal = Param_getIntArray(param, name, idx, NUM1, &rows, result);
	if ((rows == 0) && ((retVal == -1) || (retVal == 0))) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, return Value: %d\n", testid, rows, retVal);
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

