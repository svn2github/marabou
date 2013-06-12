{
	const char setred[]      =   "\033[31m";
	const char setblack[]    =   "\033[39m";
	ofstream of;
	of.open("ppc.mk", ios::out);
	of << "#!/bin/tcsh" << endl;
	of << "source /etc/csh.login" << endl;
	of << "source .login" << endl;
	TString cwd = gSystem->WorkingDirectory();
	of << "cd " << cwd << "/ppc" << endl;
	of << "cp ../*Readout.mk ." << endl;
	of << "make -f *Readout.mk clean all" << endl;
	of.close();
	gSystem->Exec("chmod +x ppc.mk");
	TString ppc = gEnv->GetValue("TMbsSetup.DefaultHost", "");
	if (ppc.IsNull()) {
		cerr << setred << "mkReadout.C: PPC not defined: Please set \"TMbsSetup.DefaultHost\" in .rootrc" << setblack << endl;
		gSystem->Exit(1);
	}
	TString cmd = "rsh ";
	cmd += ppc;
	cmd += " ";
	cmd += cwd;
	cmd += "/ppc.mk";
	gSystem->Exec(cmd);
	gSystem->Exit(0);
}
