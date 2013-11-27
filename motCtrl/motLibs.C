{
	gROOT->Macro("LoadUtilityLibs.C");
	gROOT->Macro("LoadConfigLibs.C");
	gROOT->Macro("LoadMbsLibs.C");
	cout << "[Loading STEPPER library]" << endl;
	gSystem->Load("libStepper.so");
}
