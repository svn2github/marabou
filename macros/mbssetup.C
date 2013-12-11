{
	const char setred[]      =   "\033[31m";
	const char setblack[]    =   "\033[39m";
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	setup = new TMbsSetup(".mbssetup");
	TString evtBuilder = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (evtBuilder.IsNull()) {
		evtBuilder = gEnv->GetValue("TMbsSetup.PPCName", "");
		if (evtBuilder.IsNull()) {
			cerr << setred << "mbssetup.C: Name of ppc/eventBuilder missing: Please define \"TMbsSetup.PPCName\" or \"TMbsSetup.EventBuilder.Name\"  in .rootrc" << setblack << endl;
			gSystem->Exit(1);
		}
	}
	setup->EvtBuilder()->SetProcName(evtBuilder.Data());
	setup->SetNofReadouts(1);
	TString cwd = gSystem->WorkingDirectory();
	TString ppcPath = gEnv->GetValue("TMbsSetup.PPCWorkingDir", "");
	if (ppcPath.IsNull()) {
		cerr << setred << "mbssetup.C: PPC's working directory missing: Please define \"TMbsSetup.PPCWorkingDir\" in .rootrc (\"ppc\" recommended)" << setblack << endl;
		gSystem->Exit(1);
	}
	if (!ppcPath.BeginsWith("/")) {
		TString p = cwd;
		p += "/";
		p += ppcPath;
		ppcPath = p;
	}
	setup->SetPath(ppcPath.Data());
	TString rdoProc = gEnv->GetValue("TMbsSetup.Readout0.Name", "");
	if (rdoProc.IsNull()) rdoProc = evtBuilder;
	setup->ReadoutProc(0)->SetProcName(rdoProc.Data());
	TString rdoCtrl = gEnv->GetValue("TMbsSetup.Readout0.Controller", "");
	setup->ReadoutProc(0)->SetController(rdoCtrl.Data());
	setup->SetMode(1);
	setup->ReadoutProc(0)->TriggerModule()->SetType(2);
	TString mbsVersion = gEnv->GetValue("TMbsSetup.MbsVersion", "");
	if (mbsVersion.IsNull()) {
		cerr << setred << "mbssetup.C: MBS version missing: Please define \"TMbsSetup.MbsVersion\" in .rootrc" << setblack << endl;
		gSystem->Exit(1);
	}
	TString lynxVersion = gEnv->GetValue("TMbsSetup.LynxVersion", "");
	if (lynxVersion.IsNull()) {
		cerr << setred << "mbssetup.C: LynxOs version missing: Please define \"TMbsSetup.LynxVersion\" in .rootrc" << setblack << endl;
		gSystem->Exit(1);
	}
	setup->Set("MbsVersion", mbsVersion.Data());
	setup->Set("LynxVersion", lynxVersion.Data());

	Bool_t ok = setup->MakeSetupFiles();
	if (ok) setup->Save();
	gSystem->Exit(0);
}
