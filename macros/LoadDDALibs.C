{
	cout << "[Loading SNAKE libs from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTSnkDDA0816.so");
}
