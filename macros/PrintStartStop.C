//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         PrintStartStop.C
// @(#)Purpose:      Print Start & Stop Time Stamps
// Syntax:           .x PrintStartStop.C(const Char_t * File,
//                                        const Char_t * LoadLib)
// Arguments:        Char_t * File             -- File (*.root)
//                   Char_t * LoadLib          -- LoadLib
// Description:      Print Start & Stop Time Stamps
// @(#)Author:       erda
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Mar  6 18:08:34 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                PrintStartStop.C
//                   Title:               Print Start & Stop Time Stamps
//                   Width:               
//                   NofArgs:             1
//                   Arg1.Name:           File
//                   Arg1.Title:          File (*.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         RawData:*.root
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>
#include <time.h>

void PrintStartStop(const Char_t * File)
//>>_________________________________________________(do not change this line)
//
{
	Int_t runTime;
	TString timeString;
	TString fileSpec, fileName, searchPath;
	TObjArray lofFiles;
	TMrbSystem ux;
	Int_t nofFiles;

	gSystem->Load("$MARABOU/lib/libMutex.so");
	gSystem->Load("$MARABOU/lib/libTMrbAnalyze.so");

	timeString.Resize(100);

	lofFiles.Delete();												// clear list of files matching wild card
	fileSpec = File;
	if (fileSpec.Index("*", 0) != -1) { 							// test if wild card
		ux.GetDirName(searchPath, fileSpec.Data()); 				// yes - extract path and file name
		ux.GetBaseName(fileName, fileSpec.Data());
		nofFiles = ux.FindFile(lofFiles, fileName.Data(), searchPath.Data(), kTRUE);	// read files from given directory
	} else {
		lofFiles.Add(new TObjString(fileSpec.Data()));				// no - insert exactly 1 file
		nofFiles = 1;
	}

	for (Int_t i = 0; i < nofFiles; i++) {							// loop over file names
		fileName = ((TObjString *) lofFiles[i])->GetString();		// get file name from list
		TFile * f = new TFile(fileName.Data()); 					// open root file
		TUsrEvtStart * start = new TUsrEvtStart();					// create start and stop events
		TUsrEvtStop * stop = new TUsrEvtStop();
		if (start->InitializeTree(f) && stop->InitializeTree(f)) {	// try to read them from file
			cout << endl << "File " << fileName << ":" << endl;
			start->GetTreeIn()->GetEvent(0);						// extract start time
			stop->GetTreeIn()->GetEvent(0); 						// ... stop time
			runTime = stop->GetTime() - start->GetTime();			// calc run time
			// output
			strftime((Char_t *) timeString.Data(), timeString.Length(), "%H:%M:%S", gmtime((const long *) &runTime));
			cout << "   "; start->Print();
			cout << "   "; stop->Print();
			cout << "   Runtime             :   " << timeString << endl;
		}
		f->Close(); 									// close root file
		delete f;										// delete temp objects
		delete start;
		delete stop;
	}
}
