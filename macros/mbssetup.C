{
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	setup = new TMbsSetup(".mbssetup");
	TString ppcName = gEnv->GetValue("TMbsSetup.PPCName", "iram-cg-004018");
	setup->EvtBuilder()->SetProcName(ppcName.Data());
	TString remoteHome = setup->RemoteHomeDir();
	setup->SetNofReadouts(1);
	TString ppcPath = gEnv->GetValue("TMbsSetup.PPCPath", "ppc");
	setup->SetPath(ppcPath.Data());
	setup->ReadoutProc(0)->SetProcName("iram-cg-004018");
	setup->SetMode(1);
	setup->ReadoutProc(0)->TriggerModule()->SetType(2);
	Bool_t ok = setup->MakeSetupFiles();
	setup->Save();
	gSystem->Exit(0);
}
