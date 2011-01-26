{
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	setup = new TMbsSetup(".mbssetup");
	TString evtBuilder = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "iram-cg-004018");
	setup->EvtBuilder()->SetProcName(evtBuilder.Data());
	TString remoteHome = setup->RemoteHomeDir();
	setup->SetNofReadouts(1);
	TString ppcPath = gEnv->GetValue("TMbsSetup.WorkingDir", "ppc");
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
