{
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMrbTidy.so");
	gSystem->Load("$MARABOU/lib/libTidy.so");
	TMrbTidyDoc * tdoc = new TMrbTidyDoc("mrb-test");
	tdoc->LoadConfig("tidy.cfg");
	tdoc->StripText();
	tdoc->ParseFile("test.html");
	tdoc->OutputHtml("tidy.html");
	tdoc->Print("tidy.tree");
	gSystem->Exit(0);
}
