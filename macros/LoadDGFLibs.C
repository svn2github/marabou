{
	cout << "[Loading DGF libs from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libEsoneClient.so");
	gSystem->Load("$MARABOU/lib/libTMrbEsone.so");
	gSystem->Load("$MARABOU/lib/libN2ll.so");
	gSystem->Load("$MARABOU/lib/libTMrbDGF.so");
}
