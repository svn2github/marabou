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

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <utime.h>
#include <string.h>

#include "SetColor.h"

main(int argc, char *argv[]) {

	if (argc <= 1) {
		cerr	<< setred << endl
				<< "xrcm: ROOT macro missing -" << endl << endl
				<< "Usage: xrcm [+|++[g|O]] [-lX1 ... -lXn] mfile [arg1, ..., argN]" << endl << endl
				<< "       +|++ g|O   -- load macro using ACLiC method" << endl
				<< "       -lXi       -- load libraries \"libXi.so\" (ACLiC mode only)" << endl
				<< "       mfile      -- file containing ROOT macro (ext = .C)" << endl
				<< "       argX       -- opt. arguments"
				<< setblack << endl << endl;
		exit(1);
	}

	const char * ep = getenv("ROOTSYS");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "xrcm: Env variable ROOTSYS must be set properly"
				<< setblack << endl;
		exit(1);
	}

	ep = getenv("MARABOU");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "xrcm: Env variable MARABOU must be set properly"
				<< setblack << endl;
		exit(1);
	}

	char * rootArgv[3];

	rootArgv[0] = "root";
	int argIdx;

	if (*argv[1] == '+') {
		char aclic[50];
		strcpy(aclic, argv[1]);
		char * libp = aclic;
		while (!isspace(*libp)) libp++;
		*libp++ = '\0';
		argIdx = 2;
		ep = getenv("CPLUS_INCLUDE_PATH");
		char mbIcl[200];
		strcpy(mbIcl, getenv("MARABOU"));
		strcat(mbIcl, "/include");
		if (ep == NULL || sizeof(ep) == 0 || strstr(ep, mbIcl) == NULL) {
			cerr	<< setred
					<< "xrcm: Env variable CPLUS_INCLUDE_PATH must be set properly:" << endl
					<< "      must contain \"" << mbIcl << "\"" << setblack << endl;
			exit(1);
		}
		struct stat statBuf;
		if (stat(argv[argIdx], &statBuf) == -1) {
			cerr	<< setred
					<< "xrcm: No such file - " << argv[argIdx] << setblack << endl;
			exit(1);
		}
		char macroName[200];
		char dotName[200];
		char * sp = strrchr(argv[argIdx], '/');
		if (sp == NULL) sp = argv[argIdx]; else sp++;
		strcpy(macroName, sp);
		strcpy(dotName, ".");
		strcat(dotName, sp);
		sp = strrchr(macroName, '.');
		if (sp != NULL) *sp = '\0';
		char sedCmd[200];
		strcpy(sedCmd, "sed 's.^#!.//.' ");
		strcat(sedCmd, argv[argIdx]);
		strcat(sedCmd, " > ");
		strcat(sedCmd, dotName);
		system(sedCmd);
		utimbuf timBuf;
		timBuf.actime = statBuf.st_atime;
		timBuf.modtime = statBuf.st_mtime;
		utime(dotName, &timBuf);
		ofstream macro(".xrcm", ios::out);
		macro	<< "{" << endl
				<< "	gROOT->Reset();" << endl;
		while (libp = strstr(libp, "-l")) {
			*(libp - 1) = '\0';
			libp += 2;
			if (sp = strstr(libp, "-l")) *(sp - 1) = '\0';
			macro	<< "	gSystem->Load(\"lib" << libp << ".so\");" << endl;
			if (sp) *(sp - 1) = ' ';
		}
		macro	<< "	gROOT->LoadMacro(\"" << dotName << aclic << "\");" << endl;
		if (argc > argIdx + 1) {
			macro	<< "	" << macroName;
			for (int i = argIdx + 1; i < argc; i++) {
				macro	<< ((i == argIdx + 1) ? "(" : ",");
				macro	<< argv[i];
			}
			macro	<< ");" << endl;
		} else {
			macro	<< "	" << macroName << "();" << endl;
		}
		macro << "}" << endl;
		macro.close();
		rootArgv[1] = ".xrcm";
	} else {
		argIdx = 1;
		struct stat statBuf;
		if (stat(argv[argIdx], &statBuf) == -1) {
			cerr	<< setred
					<< "xrcm: No such file - " << argv[argIdx] << setblack << endl;
			exit(1);
		}
		if (argc > argIdx + 1) {
			rootArgv[1] = (char *) calloc(1, 1000);
			strcpy(rootArgv[1], argv[argIdx]);
			for (int i = argIdx + 1; i < argc; i++) {
				strcat(rootArgv[1], (i == argIdx + 1) ? "(" : ",");
				strcat(rootArgv[1], argv[i]);
			}
			strcat(rootArgv[1], ")");
		} else {
			rootArgv[1] = argv[1];
		}
	}
	rootArgv[2] = NULL;

	char * rootProg = (char *) calloc(1, sizeof(getenv("ROOTSYS")) + sizeof("/bin/root") + 1);
	strcpy(rootProg, getenv("ROOTSYS"));
	strcat(rootProg, "/bin/root");

	cout	<< setblue
			<< "[xrcm: executing ROOT macro \"" << argv[argIdx] << "\")]"
			<< setblack << endl;

	execvp(rootProg, rootArgv);
}
