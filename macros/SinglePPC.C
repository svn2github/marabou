#!/usr/local/bin/xrcm
{
	gROOT->Macro("LoadLibs.C");
	gSystem->Load("$MARABOU/lib/libTMbsSetup.so");
	TMbsSetup * stp = new TMbsSetup();
	stp->EvtBuilder()->SetName("ppc-6");
	stp->EvtBuilder()->SetCrate(0);
	stp->SetPath("singleppc");
	stp->SetNofReadouts(1);
	stp->ReadoutProc(0)->SetName("ppc-6");
	stp->ReadoutProc(0)->SetCrate(0);
	stp->ReadoutProc(0)->SetCratesToBeRead(0);
	stp->ReadoutProc(0)->TriggerModule()->SetType("VME");
	stp->ReadoutProc(0)->TriggerModule()->SetTriggerMode("INTERRUPT");
	stp->Print();
	stp->MakeSetupFiles();
	gSystem->Exit(0);
}
