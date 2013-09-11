#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "s_store_multiple.h"

int s_store_multiple(const char *testid) {
	Param *param;
	const char *name = "tname";
	const char *idx = "tnewstr";
	const char *old = "toldvaluex";
	const char *new = "tnewvalue";
	char result[PARAM_MAX_VALUE_LEN];
	int rows;
	int retVal1;
	int retVal2;
	int retVal3;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_storeString: Storing a value multiple.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, testid);

	retVal1 = Param_storeString(param, name, idx, old);
	retVal2 = Param_storeString(param, name, idx, new);
	retVal3 = Param_getString(param, name, idx, &rows, result);

	desParam(param);
	free(param);

	if ((rows == 1) && result && (strcmp(result, new) == 0) && (retVal1 == 0) && (retVal2 == 0) && (retVal3 == 0)) {
		if(system("diff s_store_multiple_s.tcl tcl/s_store_multiple.tcl")) {
#ifdef VERBOSE
			printf("%s : Failed: Files \"%s_s.tcl\" and \"tcl/%s.tcl\" differ.\n", testid, testid, testid);
			printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
			printf("%s : Failed.\n", testid);
#endif
			myRetVal = -1;
		} else {
			myRetVal = 0;
			printf("%s : Passed.\n", testid);
		}
	} else {
#ifdef VERBOSE
		printf("%s : Failed. Rows: %d, result: \'%s\'\nreturn Value of first store call: %d\nreturn Value of second store call: %d\nreturn Value of get call: %d.\n", testid, rows, result, retVal1, retVal2, retVal3);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}

	return myRetVal;
}

