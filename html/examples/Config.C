#!/usr/local/bin/xrcm
//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Config.C
// Purpose:          Example configuration for a MARaBOU session
// Description:      Config file to define the experimental setup.
//                   Generates a C readout function to be used with MBS
//                   as well as C++ class definitions and methods for the 
//                   data analysis with ROOT.
// Author:           R. Lutter
// Date:             Nov-2002
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	gROOT->Macro("LoadConfigLibs.C");        // load libraries containing class defs and methods

	TMrbConfig::Version();
	exa = new TMrbConfig("exa", "Example configuration");         // create a C++ object
                                                                  // to store config data
	rdo = new TMrbEvent_10_1(1, "readout", "readout of camac data");   // we use trigger 1 for readout

	data = new TMrbSubevent_10_11("data", "camac data");          // define a subevent type [10,11]:
                                                                  // data stored in a zero-padded list
	adc1 = new TMrbSilena_4418V("adc1", "C1.N3");                 // define camac modules
	adc2 = new TMrbSilena_4418V("adc2", "C1.N5");                 // (module type & position)

	adc1->SetBinSize(4);                                          // define bin size for histogramms
	adc2->SetBinSize(4);

	data->Use("adc1", "etot de1 de2 de3 de4 veto");               // assign event parameters
	data->Use("adc2", "pos1r pos1l pos2r pos2l");                 // to camac modules

	rdo->HasSubevent("data");                                     // connect subevent to trigger

	exa->WriteTimeStamp();                                        // include a time stamp to raw data

	exa->DefineVariables("I", 1000, "v1[3] v2[4] v3[5]");         // define variables and windows
	exa->DefineVariables("F", 1.2345, "v4[2] v5 v6");             // (to be modified interactively)
	exa->DefineWindows("I", 0, 1023, "w1 w3 w2");

	dtsca = new TMrbUct_8904("dtsca", "C1.N14");                  // dead time scaler
	exa->WriteDeadTime("dtsca", 1000);                            // 1 dead-time event every 1000 true events

	exa->MakeReadoutCode("", "Overwrite");                  // generate C readout code for MBS
	exa->MakeAnalyzeCode("", "Default:Overwrite");          // generate C++ class defs & methods for data analysis within ROOT

	exa->WriteToFile("", "Overwrite");    // save configuration data to root file

	gSystem->Exit(0);
}
