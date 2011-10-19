{
	static const char setred[]      =   "\033[31m";
	static const char setblack[]    =   "\033[39m";
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	setup = new TMbsSetup(".mbssetup");
	TString evtBuilder = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (evtBuilder.IsNull()) {
		cerr << setred << "mbssetup.C: Name of eventBuilder missing: Please define \"TMbsSetup.EventBuilder.Name\" in .rootrc" << setblack << endl;
		gSystem->Exit(0);
	}
	setup->EvtBuilder()->SetProcName(evtBuilder.Data());
	TString remoteHome = setup->RemoteHomeDir();
	setup->SetNofReadouts(1);
	TString ppcPath = gEnv->GetValue("TMbsSetup.WorkingDir", "");
	if (ppcPath.IsNull()) {
		cerr << setred << "mbssetup.C: Working directory missing: Please define \"TMbsSetup.WorkingDir\" in .rootrc (\"ppc\" recommended)" << setblack << endl;
		gSystem->Exit(0);
	}
	setup->SetPath(ppcPath.Data());
	TString rdoProc = gEnv->GetValue("TMbsSetup.Readout0.Name", "");
	if (rdoProc.IsNull()) rdoProc = evtBuilder;
	setup->ReadoutProc(0)->SetProcName(rdoProc.Data());
	setup->SetMode(1);
	setup->ReadoutProc(0)->TriggerModule()->SetType(2);
	Bool_t ok = setup->MakeSetupFiles();
	if (ok) setup->Save();
	gSystem->Exit(0);
}
