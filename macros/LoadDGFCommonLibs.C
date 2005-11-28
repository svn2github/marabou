{
	cout << "[Loading DGF libs (common part for online and offline) from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMrbDGFCommon.so");
}
