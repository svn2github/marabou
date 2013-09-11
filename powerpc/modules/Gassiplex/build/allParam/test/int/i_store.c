#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allParam.h>

#include "i_store.h"

int i_store(const char *testid) {
	Param *param;
	const char *name = "tname";
	const char *idx = "tnewint";
	unsigned long int tostore = 42;
	unsigned long int result;
	int rows;
	int retVal1;
	int retVal2;
	int myRetVal;

#ifdef VERBOSE
	printf("%s : Param_storeInt: Simple.\n", testid);
#endif

	param = malloc(sizeof(Param));
	conSetupParam(param, "i_store");

	retVal1 = Param_storeInt(param, name, idx, tostore);
	retVal2 = Param_getInt(param, name, idx, &rows, &result);

	desParam(param);
	free(param);

	if ((rows == 1) && result && (result == tostore) && (retVal1 == 0) && (retVal2 == 0)) {
		if(system("diff i_store_s.tcl tcl/i_store.tcl")) {
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
		printf("%s : Failed. Rows: %d, result: \'%ud\'\nreturn Value of store call: %d\nreturn Value of get call: %d.\n", testid, rows, result, retVal1, retVal2);
		printf("%s : Error string: %s\n", testid, Param_getErrStr(param));
#else
		printf("%s : Failed.\n", testid);
#endif
		myRetVal = -1;
	}

	return myRetVal;
}

