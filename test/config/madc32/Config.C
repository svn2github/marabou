#!/share/local_rh9/bin/xrcm

{
	gROOT->Macro("LoadConfigLibs.C");
   
	exa = new TMrbConfig("exa", "Example");

	evt = new TMrbEvent_10_1(1, "readout", "readout");

	data1 = new TMrbSubevent_Madc_1("data1", "subevent data");
	adc1 = new TMrbMesytec_Madc32("adc1", 0x00F10000);
	data2 = new TMrbSubevent_Madc_1("data2", "subevent data");
	adc2 = new TMrbMesytec_Madc32("adc2", 0x00F30000);
	data3 = new TMrbSubevent_Madc_1("data3", "subevent data");
	adc3 = new TMrbMesytec_Madc32("adc3", 0x00F40000);
	data4 = new TMrbSubevent_Madc_1("data4", "subevent data");
	adc4 = new TMrbMesytec_Madc32("adc4", 0x00F50000);
	data5 = new TMrbSubevent_Madc_1("data5", "subevent data");
	adc5 = new TMrbMesytec_Madc32("adc5", 0x00F60000);

	adc1->UseSettings();
	adc2->UseSettings();
	adc3->UseSettings();
	adc4->UseSettings();
	adc5->UseSettings();

	data1->Use("adc1", "e1[32]");
	data2->Use("adc2", "e2[32]");
	data3->Use("adc3", "e3[32]");
	data4->Use("adc4", "e4[32]");
	data5->Use("adc5", "e5[32]");
   
	evt->HasSubevent("data1 data2 data3 data4 data5");

	exa->MakeReadoutCode("", "Default:Overwrite");
	exa->MakeAnalyzeCode("", "Default:Overwrite");
	exa->MakeRcFile("", "", "Default:Overwrite:ByName");
	exa->WriteToFile("", "Default:Overwrite");

	exa->UpdateMbsSetup();

	if (exa->IsVerbose()) exa->PrintErrors();

	gSystem->Exit(0);
}
