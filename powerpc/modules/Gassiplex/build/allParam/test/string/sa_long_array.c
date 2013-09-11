#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "sa_long_array.h"

#define NUM1 8
#define NUM5 4

int sa_long_array(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tstringa";
	char *result[NUM5];
	int rows;
	int retVal;
	int myRetVal;
	int i;

#ifdef VERBOSE
	printf("%s : Param_getStringArray: To many array members available.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM5 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getStringArray(param, name, idx, NUM5, &rows, result);
	if ((rows == NUM5) &&
		result[0] && (strcmp(result[0], "test1") == 0) &&
		result[1] && (strcmp(result[1], "test0") == 0) &&
		result[2] && (strcmp(result[2], "test3") == 0) &&
		result[3] && (strcmp(result[3], "test2") == 0) &&
		(retVal == 0)) {
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
	for (i = 0 ; i < NUM5 ; i++) {
		free(result[i]);
	}

	desParam(param);
	free(param);

	return myRetVal;
}

