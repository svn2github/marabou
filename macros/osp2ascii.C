void osp2ascii(const Char_t * FileName)
{
    gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 
    gROOT->Macro("LoadColors.C");
	if (FileName == NULL) {
		cerr	<< endl << setred
				<< "osp2ascii: file name missing"
				<< setblack << endl << endl
				<< "osp2ascii: convert osp data to ASCII format" << endl << endl
				<< "Usage: .x osp2ascii.C(\"ospFile\")" << endl
				<< "       ospFile --> name of osp file WITHOUT extension"
				<< setblack << endl << endl;
		return;
	}
	TString ospFile = FileName; ospFile += ".osp";		// append osp ext
	gSystem->Unlink("tmp.hbook");				// remove temp files
	gSystem->Unlink("tmp.root");				// ...
	TString cmd = "mlle2hbook tmp.hbook ";		// build mlle2hbook command
	cmd += ospFile;
	gSystem->Exec(cmd.Data());					// "mlle2hbook tmp.hbook ospFile": convert osp to hbook
	gSystem->Exec("h2root tmp.hbook");			// "h2root tmp.hbook": convert hbook to root

	TFile * f = new TFile("tmp.root");		 	// open root file containing histogram
	TH1F * h = (TH1F *) f->Get("h1");			// histogram name is h1 (created by h2root by default)
	Int_t nofChannels = h->GetEntries();		// read number of channels
	cout	<< setblue
			<< "osp2ascii: File " << ospFile << ": " << nofChannels << " channels"
			<< setblack << endl;

	TString datFile = FileName; datFile += ".dat";		// name of dat file
	ofstream dat;
	dat.open(datFile.Data(), ios::out); 		// try to open
	if (!dat.good()) {
		cerr	<< setred
				<< "?? osp2ascii: Can't open file " << datFile
				<< setblack << endl;
		return;
	}
	for (Int_t i = 0; i < nofChannels; i++) {	// read channel by channel
		dat << i << "		" << h->GetBinContent(i + 1) << endl;	// output x, y
	}
	dat.close();								// close file
	cout	<< setblue
			<< "osp2ascii: osp data converted to ascii - file " << datFile
			<< setblack << endl;
	gSystem->Unlink("tmp.hbook");				// remove temp files
	gSystem->Unlink("tmp.root");				// ...
}
