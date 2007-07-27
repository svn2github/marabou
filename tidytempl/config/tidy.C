{
	gROOT->Macro("LoadUtilityLibs.C");
	gSystem->Load("$MARABOU/lib/libTMrbTidy.so");
	gSystem->Load("$MARABOU/lib/libTidy.so");
	TMrbTidyDoc * tdoc = new TMrbTidyDoc("mrb-test");
	tdoc->LoadConfig("tidy.cfg");
	tdoc->StripText();
	tdoc->ParseFile("Analyze.cxx.code");
	tdoc->OutputHtml("tidy.html");
	tdoc->Print("tidy.tree");
	tdoc->Print("tidy.tree.html", kFALSE, kTRUE);
	gSystem->Exit(0);
}
