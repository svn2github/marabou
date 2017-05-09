// setup scaler readout
{
	#include "SetColor.h"
	
	ULong_t lsb, msb, data;
	gROOT->Macro("LoadConfigLibs.C");
	gROOT->Macro("LoadEsoneLibs.C");

// search config file
	TString wd = gSystem->WorkingDirectory();
	void * dirp = gSystem->OpenDirectory(wd.Data());
	if (dirp == NULL) {
		cerr	<< setred
				<< "ssca: Can't open working directory - " << wd << endl;
				<< setblack << endl;
		gSystem->Exit(1);
	}
	TRegexp rexp("*Config.root", kTRUE);
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
	if (cf.Length() == 0) {
		cerr	<< setred
				<< "ssca: Can't find MARaBOU's config file"
				<< setblack << endl;
		gSystem->Exit(1);
	}

// read objects from config file
	Int_t nCnafs = 0;
	TMrbConfig * c = new TMrbConfig();
	c = c->ReadFromFile(cf.Data());

// get scaler list
// init up to 3 scalers
	TGMrbList * sl = c->GetLofScalers();
	TMrbCamacScaler * s1 = (TMrbCamacScaler *) sl->First();
	s1->Print();
	Int_t crate1 = s1->GetCNAF(TMrbCNAF::kCnafCrate);
	Int_t station1 = s1->GetCNAF(TMrbCNAF::kCnafStation);
	Int_t data1 = 0;
	TMrbCamacScaler * s2 = (TMrbCamacScaler *) sl->After(s1);
	if (s2) {
		s2->Print();
		Int_t crate2 = s2->GetCNAF(TMrbCNAF::kCnafCrate);
		Int_t station2 = s2->GetCNAF(TMrbCNAF::kCnafStation);
		Int_t data2 = 0;
	}
	TMrbCamacScaler * s3 = (TMrbCamacScaler *) sl->After(s2);
	if (s3) {
		s3->Print();
		Int_t crate3 = s3->GetCNAF(TMrbCNAF::kCnafCrate);
		Int_t station3 = s3->GetCNAF(TMrbCNAF::kCnafStation);
		Int_t data3 = 0;
	}

// get readout proc for CAMAC access
	TEnv * env = new TEnv("C_analyze.def");
	TString rdoProc;
	rdoProc = env->GetValue("READOUT", "UnKnown");

// open esone connection
	TMrbEsone * e = new TMrbEsone(rdoProc.Data());
	cout << "[ssca: Setup done - config file = " << cf << ", CAMAC readout = " << rdoProc << "]" << endl;
}
