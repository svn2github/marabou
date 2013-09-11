#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "sa_short_array.h"

#define NUM1 8
#define NUM3 16

int sa_short_array(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tstringa";
	char *result[NUM3];
	int rows;
	int retVal;
	int myRetVal;
	int i;

#ifdef VERBOSE
	printf("%s : Param_getStringArray: To few array members available.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM3 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getStringArray(param, name, idx, NUM3, &rows, result);
	if ((rows == NUM1) &&
		result[0] && (strcmp(result[0], "test1") == 0) &&
		result[1] && (strcmp(result[1], "test0") == 0) &&
		result[2] && (strcmp(result[2], "test3") == 0) &&
		result[3] && (strcmp(result[3], "test2") == 0) &&
		result[4] && (strcmp(result[4], "test5") == 0) &&
		result[5] && (strcmp(result[5], "test4") == 0) &&
		result[6] && (strcmp(result[6], "test7") == 0) &&
		result[7] && (strcmp(result[7], "test6") == 0) &&
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

	for (i = 0 ; i < NUM3 ; i++) {
		free(result[i]);
	}

	desParam(param);
	free(param);

	return myRetVal;
}

