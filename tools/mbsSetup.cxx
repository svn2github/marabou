//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbsSetup
// Purpose:        Install MBS setup files
// Arguments:      ppc    -- name of PPC
//                 path   -- where to write files
//                 ctime  -- conversion time in usecs
// Results:        
// Exceptions:     
// Description:    Generates *.usf and *.scom files needed to start MBS
// @(#)Author:     R. Lutter
// @(#)Revision:   $Id: mbsSetup.cxx,v 1.1 2008-11-24 14:55:33 Rudolf.Lutter Exp $
// @(#)Date:       $date$
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TString.h"
#include "TSystem.h"
#include "TInetAddress.h"

#include "TMbsSetup.h"

#include "SetColor.h"

main(int argc, char *argv[]) {

	if (argc <= 1) {
		cerr	<< setred << endl
				<< "mbsSetup: Wrong number of arguments" << endl
				<< "Usage: mbsSetup ppc path" << endl
				<< "       ppc        -- name of PPC to be used" << endl
				<< "       path       -- path where to write MBS files" << endl
				<< setblack << endl << endl;
		exit(1);
	}

	const char * ep = getenv("ROOTSYS");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "mbsSetup: Env variable ROOTSYS must be set properly"
				<< setblack << endl;
		exit(1);
	}

	ep = getenv("MARABOU");
	if (ep == NULL || sizeof(ep) == 0) {
		cerr	<< setred
				<< "mbsSetup: Env variable MARABOU must be set properly"
				<< setblack << endl;
		exit(1);
	}

	TString ppc = argv[1];
	TInetAddress * ia = new TInetAddress(gSystem->GetHostByName(ppc.Data()));
	TString addr = ia->GetHostAddress();
	if (addr.CompareTo("0.0.0.0") == 0) {
		cerr	<< setred
				<< "mbsSetup: No such PPC - " << ppc
				<< setblack << endl;
		exit(1);
	}

	TString path = argv[2];
	if (!gSystem->IsAbsoluteFileName(path.Data())) path = Form("%s/%s", gSystem->WorkingDirectory(), path.Data());
	if (gSystem->AccessPathName(path.Data())) {
		cerr	<< setred
				<< "mbsSetup: No such directory - " << path << endl
				<< setblack << endl;
		exit(1);
	}

	Int_t ctime = 50;
	if (argc == 4) ctime = atoi(argv[3]);

	TMbsSetup * s = new TMbsSetup();
  	if (s) {
		s->EvtBuilder()->SetProcName(ppc.Data());
		s->SetNofReadouts(1);
		s->SetPath(path.Data());
		s->ReadoutProc(0)->SetProcName(ppc.Data());
		s->SetMode(kModeSingleProc);
		s->ReadoutProc(0)->TriggerModule()->SetType(kTriggerModuleVME);	
		s->ReadoutProc(0)->TriggerModule()->SetConversionTime(ctime);	
		s->MakeSetupFiles();
	}
}
