//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           xrcm
// Purpose:        Execute ROOTCint macro
// Arguments:      cfile     -- file containing ROOTCint macro
//                 rootArgv[1],...  -- opt arguments
// Results:        
// Exceptions:     
// Description:    Executes given file by call of the ROOT interpreter
// @(#)Author:     R. Lutter
// @(#)Revision:   %I%
// @(#)Date:       %G%
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream.h>
#include "SetColor.h"

main(int argc, char *argv[]) {

	char * rootArgv[100];
	char * rootProg;

	if (argc <= 1) {
		cerr	<< setred << endl
				<< "xrcm: ROOT macro missing -" << endl << endl
				<< "Usage: xrcm mfile [arg1, ..., argN]" << endl << endl
				<< "       mfile  -- file containing ROOT macro (ext = .C)" << endl
				<< "       argX   -- opt. arguments"
				<< setblack << endl << endl;
		exit(1);
	}

	const char * ep = getenv("ROOTSYS");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "xrcm: env variable ROOTSYS must be set properly"
				<< setblack << endl;
		exit(1);
	}

	ep = getenv("MARABOU");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "xrcm: env variable MARABOU must be set properly"
				<< setblack << endl;
		exit(1);
	}

	rootArgv[0] = "root";
	if (argc > 2) {
		rootArgv[1] = (char *) calloc(1, 1000);
		strcpy(rootArgv[1], argv[1]);
		for (int i = 2; i < argc; i++) {
			strcat(rootArgv[1], (i == 2) ? "(" : ",");
			strcat(rootArgv[1], argv[i]);
		}
		strcat(rootArgv[1], ")");
	} else {
		rootArgv[1] = argv[1];
	}
	rootArgv[2] = NULL;

	rootProg = (char *) calloc(1, sizeof(getenv("ROOTSYS")) + sizeof("/bin/root") + 1);
	strcpy(rootProg, getenv("ROOTSYS"));
	strcat(rootProg, "/bin/root");

	cout	<< setblue
			<< "[xrcm: executing ROOT macro \"" << argv[1] << "\")]"
			<< setblack << endl;

	execvp(rootProg, rootArgv);
}
