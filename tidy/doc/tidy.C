{
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMrbTidy.so");
	gSystem->Load("$MARABOU/lib/libTidy.so");
	TMrbTidyDoc * tdoc = new TMrbTidyDoc("demo", "tidy.html");
	tdoc->Print();
}
