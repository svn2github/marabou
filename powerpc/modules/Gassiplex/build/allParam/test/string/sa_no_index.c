#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "sa_no_index.h"

#define NUM1 8

int sa_no_index(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "bla";
	char *result[NUM1];
	int rows;
	int retVal;
	int myRetVal;
	int i;

#ifdef VERBOSE
	printf("%s : Param_getStringArray: Non-existing idx.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM1 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getStringArray(param, name, idx, NUM1, &rows, result);
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

	for (i = 0 ; i < NUM1 ; i++) {
		free(result[i]);
	}

	desParam(param);
	free(param);

	return myRetVal;
}

