#!/share/local_rh9/bin/xrcm

{
	gROOT->Macro("LoadUtilityLibs.C");
	gSystem->Load("$MARABOU/lib/libTSnkDDA0816.so");
	TSnkDDAMessage * ddaMsg = new TSnkDDAMessage();
	ddaMsg->Reset();
	ddaMsg->fProgram = "ddaStart.C";
	ddaMsg->fPath = gSystem->WorkingDirectory();
	ddaMsg->fSaveScan = "DDA_ScanData.root";
	ddaMsg->fAction = "StartScan";
	ddaMsg->fExecMode = gEnv->GetValue("DDAControl.ExecMode", "normal");
	TString server = gEnv->GetValue("DDAControl.Server", "localHost");
	Int_t port = gEnv->GetValue("DDAControl.Port", 9010);
	TSocket * s = new TSocket(server.Data(), port);
	if (s) s->SendObject(ddaMsg);
	gSystem->Sleep(1000);
	s->Close();
	gSystem->Exit(0);
}
