#include "SetColor.h"

void osp2root(const Char_t * FileName = NULL)
{
	gROOT->Macro("LoadUtilityLibs.C"); 					// load utility libs 
	if (FileName == NULL) {
		cerr	<< endl << setred
				<< "osp2root: file name missing"
				<< setblack << endl << endl
				<< "osp2root: convert osp data to ROOT format" << endl << endl
				<< "Usage: .x osp2root.C(\"ospFile\")" << endl
				<< "       ospFile --> name of osp file WITHOUT extension"
				<< setblack << endl << endl;
		return;
	}
	TString ospFile = FileName; ospFile += ".osp";		// append osp ext
	TString rootFile = FileName; rootFile += ".root";	// append root ext
	gSystem->Unlink("tmp.hbook");				// remove temp files
	gSystem->Unlink("tmp.root");				// ...
	TString cmd = "mlle2hbook tmp.hbook ";		// build mlle2hbook command
	cmd += ospFile;
	gSystem->Exec(cmd.Data());					// "mlle2hbook tmp.hbook ospFile": convert osp to hbook
	gSystem->Exec("h2root tmp.hbook");			// "h2root tmp.hbook": convert hbook to root
	cmd = "mv tmp.root ";						// rename tmp file -> original name
	cmd += rootFile;
	gSystem->Exec(cmd.Data());
	cout	<< setblue
			<< "osp2root: osp data converted to ROOT - file " << rootFile
			<< setblack << endl;
	gSystem->Unlink("tmp.hbook");				// remove temp files
	gSystem->Unlink("tmp.root");				// ...
}
