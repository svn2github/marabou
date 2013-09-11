#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "fa_lowercase_no_basedir.h"

#define NUM1 8

int fa_lowercase_no_basedir(const char *testid) {
	Param *param;
	char *name = "file";
	char *idx = "namea";
	char *result[NUM1];
	int rows;
	int retVal;
	int myRetVal;
	int i;

#ifdef VERBOSE
	printf("%s : Param_getFilenameArray: Lowercase input on correct statements.\n", testid);
	printf("%s : No glob(basedir) specified in the parameter source.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	for (i = 0 ; i < NUM1 ; i++) {
		result[i] = malloc(PARAM_MAX_VALUE_LEN * sizeof(char));
	}

	retVal = Param_getFilenameArray(param, name, idx, NUM1, &rows, result);
	if ((rows == NUM1) &&
		result[0] && (strcmp(result[0], "test1.blob") == 0) &&
		result[1] && (strcmp(result[1], "/test0.blob") == 0) &&
		result[2] && (strcmp(result[2], "test3.blob") == 0) &&
		result[3] && (strcmp(result[3], "/test2.blob") == 0) &&
		result[4] && (strcmp(result[4], "test5.blob") == 0) &&
		result[5] && (strcmp(result[5], "/test4.blob") == 0) &&
		result[6] && (strcmp(result[6], "test7.blob") == 0) &&
		result[7] && (strcmp(result[7], "/test6.blob") == 0) &&
		(retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result array:\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\nreturn Value: %d.\n", testid, rows,
			result[0],
			result[1],
			result[2],
			result[3],
			result[4],
			result[5],
			result[6],
			result[7],
			retVal);
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

