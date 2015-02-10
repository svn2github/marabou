{
	const char setred[]      =   "\033[31m";
	const char setblack[]    =   "\033[39m";
	const char setblue[] 	=	"\033[34m";
	
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	TString ppcName = gEnv->GetValue("TMbsSetup.PPCName", "");
	if (ppcName.IsNull()) {
		cerr << setred << "mbscomp.C: Name of ppc missing: Please define \"TMbsSetup.PPCName\" in .rootrc" << setblack << endl;
		gSystem->Exit(1);
	}
	TString cwd = gSystem->WorkingDirectory();

	TString ppcPath = gEnv->GetValue("TMbsSetup.PPCWorkingDir", "");
	if (ppcPath.IsNull()) {
		cerr << setred << "mbscomp.C: PPC's working directory missing: Please define \"TMbsSetup.PPCWorkingDir\" in .rootrc (\"ppc\" recommended)" << setblack << endl;
		gSystem->Exit(1);
	}
	if (!ppcPath.BeginsWith("/")) {
		TString p = cwd;
		p += "/";
		p += ppcPath;
		ppcPath = p;
	}
	
	void * dp = gSystem->OpenDirectory(cwd.Data());
	if (dp == NULL) {
		cerr << setred << "mbscomp.C: Error while searching for ppc .mk files in your working dir" << setblack << endl;
		gSystem->Exit(1);
	}

	Char_t * mkf;
	TString ppcMkFile = "";
	while (mkf = gSystem->GetDirEntry(dp)) {
		TString mkf2 = mkf;
		if (mkf2.EndsWith("Readout.mk")) {
			if (!ppcMkFile.IsNull()) {
				cerr << setred << "mbscomp.C: More than one ppc .mk file in your working dir - can't proceed" << setblack << endl;
				gSystem->Exit(1);
			}
			ppcMkFile = mkf;
		}
	}
	
	ofstream mk(Form("%s/ppc.mk", ppcPath.Data()));
	if (!mk.good()) {
		cerr << setred << "mbscomp.C: Can't open \"ppc.mk\" in your working dir" << setblack << endl;
		gSystem->Exit(1);
	}
	
	mk << "#!/bin/tcsh" << endl;
	mk << "source /etc/csh.login" << endl;
	mk << "source .login" << endl;
	mk << "cd " << ppcPath << endl;
	mk << "cp ../" << ppcMkFile << " ." << endl;
	mk << "make -f " << ppcMkFile << " clean all" << endl;
	mk.close();
	
	cout << setblue << "[mbscomp.C: Compiling readout code for " << ppcName << " using " << ppcMkFile << setblack << endl;
	gSystem->Exec(Form("chmod +x %s/ppc.mk", ppcPath.Data()));
	gSystem->Exec(Form("rsh %s %s/ppc.mk", ppcName.Data(), ppcPath.Data()));
	cout << setblue << "[mbscomp.C: " << ppcMkFile << " done]" << setblack << endl;

	gSystem->Exit(0);
}
