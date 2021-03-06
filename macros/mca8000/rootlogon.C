{
//	gSystem->Load("libSerialComm.so");
	const char setred[]   = "\033[31m";
	const char setgreen[] = "\033[32m";
	const char setblue[]  = "\033[34m"; 
	const char setblack[] = "\033[39m"; 
   Int_t rvers_need = 53418;
	Int_t rvers = gROOT->GetVersionInt();
	if ( rvers < rvers_need ) {
		printf("%sTrying to load library for MCA8000a%s\n", setgreen, setblack);
		printf("%sCurrent MARABOU Version is: %d needed:: %d\n", setred, rvers, rvers_need);
		printf("Please exit HistPresent and execute:\n");
		printf("%smodule unload marabou\n", setblue);
		printf("module load marabou/5.34.18 %s\n", setblack);
		return;
	}
   TString line;
 	TString cmd ("fuser -u /dev/ttyS0 2>&1");
 	FILE *fp = gSystem->OpenPipe(cmd, "r");
   line.Gets(fp);
   if (line.Length() > 0) {
		cout << setred << "Serial device already in use: " << line  << endl 
		<< "Will not load MCA8000a library" << setblack<< endl;
		return;
	} 
	cmd = "if [ -w ";
	cmd +=  device.Data();
	cmd += " ]; then echo ok; else echo no ; fi";
	fp = gSystem->OpenPipe(cmd, "r");
   line.Gets(fp);
   if ( line.Contains("no") ) {
		cout << setred << "You have no write access to " << device.Data()
		<< setblack<< endl;
		return;
	} else {
		cout << "Write access ok: " << device.Data() << endl;
	}

	printf("%sLoading library for MCA8000a%s\n", setgreen, setblack);
	gROOT->ProcessLine(".L mca8000.cxx+");
	MCA8000 * mca = new MCA8000();
	mca->StartGui();	
	
	Int_t nread = mca->OpenDevice();
	if (nread == 20) {
		printf("%sMCA connected%s\n", setblue, setblack);
	} else {
		printf("%sMCA connect failed%s\n", setred, setblack);
	}
	
	
}
