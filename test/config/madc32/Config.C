#!/share/local_rh9/bin/xrcm

{
	gROOT->Macro("LoadConfigLibs.C");
   
	exa = new TMrbConfig("exa", "Example");

	evt = new TMrbEvent_10_1(1, "readout", "readout");
	data = new TMrbSubevent_Madc_1("data", "subevent data");
	adc = new TMrbMesytec_Madc32("adc", 0xe0000000);
	adc->UseSettings();
	data->Use("adc", "e[32]");
   
	evt->HasSubevent("data");

	exa->MakeReadoutCode("", "Default:Overwrite");
	exa->MakeAnalyzeCode("", "Default:Overwrite");
	exa->MakeRcFile("", "", "Default:Overwrite:ByName");
	exa->WriteToFile("", "Default:Overwrite");

	exa->UpdateMbsSetup();

	adc->PrintSettings();

	if (exa->IsVerbose()) exa->PrintErrors();

	gSystem->Exit(0);
}
