#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "i_conparam.h"

int i_conparam(const char *testid) {
	Param *param;
	char *name = "tname";
	char *idx = "tint";
	unsigned long int result;
	int rows;
	int retVal;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_getInt: Lowercase input on correct statements.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conParam(param);

	retVal = Param_getInt(param, name, idx, &rows, &result);
	if ((rows == 1) && (result == 100) && (retVal == 0)) {
		myRetVal = 0;
		printf("%s : Passed.\n", testid);
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result: \'%ld\', return Value: %d.\n", testid, rows, result, retVal);
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

