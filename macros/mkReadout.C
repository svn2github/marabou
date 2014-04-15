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
	TString ppc = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (ppc.IsNull()) {
		ppc = gEnv->GetValue("TMbsSetup.PPCName", "");
		if (ppc.IsNull()) {
			cerr << setred << "mkReadout.C: Name of ppc/eventBuilder missing: Please define \"TMbsSetup.PPCName\" or \"TMbsSetup.EventBuilder.Name\"  in .rootrc" << setblack << endl;
			gSystem->Exit(1);
		}
	}
	TString cmd = "rsh ";
	cmd += ppc;
	cmd += " ";
	cmd += cwd;
	cmd += "/ppc.mk";
	gSystem->Exec(cmd);
	gSystem->Exit(0);
}
