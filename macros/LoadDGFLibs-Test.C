{
	cout << "[Loading *LOCAL* DGF libs from $LMARABOU/lib]" << endl;
	gSystem->Load("$LMARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$LMARABOU/lib/libEsoneClient.so");
	gSystem->Load("$LMARABOU/lib/libTMrbEsone.so");
	gSystem->Load("$LMARABOU/lib/libN2ll.so");
	gSystem->Load("$LMARABOU/lib/libTMrbDGF.so");
}
