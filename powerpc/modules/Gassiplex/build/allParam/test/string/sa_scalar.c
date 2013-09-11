#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "sa_scalar.h"

#define NUM1 8

int sa_scalar(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tstring";
	char *result[NUM1];
	int i;
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getStringArray: Request for a scalar value.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM1 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getStringArray(param, name, idx, NUM1, &rows, result);
	if ((rows == 1) && result && (strcmp(result[0], "test100") == 0) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result: \'%s\', return Value: %d.\n", testid, rows, result[0], retVal);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}

	for (i = 0 ; i < NUM1 ; i++) {
		free(result[i]);
	}

	desParam(param);
	free(param);

	return myRetVal;
}

