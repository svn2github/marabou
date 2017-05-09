#!/share/local_rh9/bin/xrcm
//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         PrintFile.C
// @(#)Purpose:      Output ROOT file info
// Description:      Prints tree names, number of entries, start & stop info.
// @(#)Author:       R. Lutter
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         19-Jun-2000
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	#include <time.h>
	#include "SetColor.h"

	TString rootFile;
	Int_t startTime, stopTime;
	TString timeString;

	Float_t totalDeadTime = 0;

// try to load user's local lib
	TString wd = gSystem->WorkingDirectory();
	void * dirp = gSystem->OpenDirectory(wd.Data());
	if (dirp != NULL) {
		TRegexp rexp("*LoadLibs.C", kTRUE);
		TString cf;
		Char_t * dirent;
		for (;;) {
			dirent = gSystem->GetDirEntry(dirp);
			if (dirent == NULL) {
				cf.Resize(0);
				break;
			} else {
				cf = dirent;
				if (cf.Index(rexp) >= 0) break;
			}
		}
		if (cf.Length() != 0) {
			gROOT->Macro(cf.Data());			// load user's libs + MARaBOU libs
		} else {
			gROOT->Macro("LoadLibs.C"); 		// load MARaBOU libs
		}
	} else {
		gROOT->Macro("LoadLibs.C"); 			// load MARaBOU libs
	}

// main loop
	while (1) {
		cout	<< endl << "Name of ROOT file (ext = .root, .q -> exit): " << flush;
		cin 	>> rootFile;
		rootFile = rootFile.Strip(TString::kBoth);
		if (rootFile.CompareTo(".q") == 0) gSystem->Exit(0);	// exit?

		if (rootFile.Index(".root", 0) != rootFile.Length() - 5) rootFile += ".root";

		TFile * rf = new TFile(rootFile.Data());
		if (rf->IsZombie()) {									// test if open successful
			cerr	<< setred
					<< "PrintFile: Can't open file " << rootFile << ", try again ..."
					<< setblack << endl;
			continue;
		}

		TTree * start = NULL;
		TTree * stop = NULL;
		TTree * dtime = NULL;

		cout	<< setblue
				<< "================================================================================" << endl
				<< "PrintFile: List of objects from file " << rootFile << ":" << endl
				<< "Class           Name                Title                         Entries" << endl
				<< "--------------------------------------------------------------------------------"
				<< setblack << endl;

		TList * lk = rf->GetListOfKeys();
		TKey * k = (TKey *) lk->First();
		TTree *	t = NULL;
		TList * ol = new TList();		// there may be multiple keys - so keep track of them
		while (k != NULL) {
			if (ol->FindObject(k->GetName()) != NULL) { k = lk->After(k); continue; }
			ol->Add(k);
			TNamed * o = rf->Get(k->GetName());
			if (strcmp(o->ClassName(), "TTree") == 0) t = (TTree *) o; else t = NULL;
			if (t) {
				if (strcmp(t->GetName(), "Start") == 0) start = t;
				if (strcmp(t->GetName(), "Stop") == 0) stop = t;
				if (strcmp(t->GetName(), "DeadTime") == 0) dtime = t;
			}
			cout	<< setblue
					<< setw(15) << setiosflags(ios::left) << o->ClassName() << " "
					<< setw(20) << setiosflags(ios::left) << o->GetName()
					<< setw(30) << setiosflags(ios::left) << o->GetTitle();
			if (t) cout << setiosflags(ios::left) << t->GetEntries();
			cout	<< resetiosflags(ios::left) << setblack << endl;
			k = (TKey *) lk->After(k);
		}
		if (start || stop) {
			cout	<< setblue
					<< "--------------------------------------------------------------------------------"
					<< setblack << endl;

			if (start) {
				timeString.Remove(0);
				timeString.Resize(100);
				start->SetBranchAddress("start", &startTime);
				start->GetEntry(0);
				strftime(timeString.Data(), timeString.Length(), "%e-%b-%Y %H:%M:%S", localtime((const long *) &startTime));
				cout	<< setblue
						<< "START Acquisition at " << timeString
						<< setblack << endl;
			}
			if (stop) {
				timeString.Remove(0);
				timeString.Resize(100);
				stop->SetBranchAddress("stop", &stopTime);
				stop->GetEntry(0);
				strftime(timeString.Data(), timeString.Length(), "%e-%b-%Y %H:%M:%S", localtime((const long *) &stopTime));
				cout	<< setblue
						<< "STOP  Acquisition at " << timeString
						<< setblack << endl;
			}
			Int_t runTime = stopTime - startTime;
			timeString.Remove(0);
			timeString.Resize(100);
			strftime(timeString.Data(), timeString.Length(), "%H:%M:%S", gmtime((const long *) &runTime));
			cout	<< setblue
					<< "Run time             " << timeString
					<< setblack << endl;

			cout	<< setblue
					<< "--------------------------------------------------------------------------------"
					<< setblack << endl;
		}
		if (dtime) {
			TUsrDeadTime * dt = new TUsrDeadTime();
			TBranch * dtb = dtime->GetBranch("dtData");
			dtb->SetAddress(&dt);
			Int_t nent = dtime->GetEntries();
			Int_t maxEnt = 20000;
			Bool_t trunc = kFALSE;
			if (nent > maxEnt) {
				trunc = kTRUE;
				nent = maxEnt;
			}
			for (Int_t i = 0; i < nent; i++) {
				dtime->GetEntry(i);
				totalDeadTime += dt->Get();
			}
			cout	<< setblue
					<< "Average Dead Time    " << setw(3) << setprecision(3) << totalDeadTime / nent << " %";
			if (trunc) cout << " (" << nent << " entries)";
			cout	<< setblack << endl;

			cout	<< setblue
					<< "--------------------------------------------------------------------------------"
					<< setblack << endl;
		}
	}
}
