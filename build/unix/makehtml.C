#!/usr/local/bin/xrcm

// Script to generate html files from MARaBOU classes
// Called by main Makefile.
//
// Author: O. Schaile, R. Lutter

{
    gSystem.Load("libGed.so");
	gSystem.Load("libTMrbUtils.so");
    gSystem.Load("libTMbsControl.so");
    gSystem.Load("libTMrbConfig.so");
    gSystem.Load("libTMrbTransport.so");
    gSystem.Load("libTMbsSetup.so");
    gSystem.Load("libTMrbC2Lynx.so");
    gSystem.Load("libTMrbEsone.so");
    gSystem.Load("libTMrbDGF.so");
    gSystem.Load("libTMrbDGFCommon.so");
    gSystem.Load("libTGMrbUtils.so");
    gSystem.Load("libTMrbHelpBrowser.so");
    gSystem.Load("libTidy.so");
    gSystem.Load("libTMrbTidy.so");
    gSystem.Load("libMutex.so");
    gSystem.Load("libTMrbAnalyze.so");
    gSystem.Load("libDGFControl.so");
    gSystem.Load("libTSnkDDA0816.so");
    gSystem.Load("libTPolControl.so");
    THtml html;
    html.SetInputDir("include");
    html.SetProductName("MARaBOU Data Acquisition");
    html.CreateAuxiliaryFiles();
	html.MakeAll(kTRUE);
    TEnv env(".rootrc");
    TString odir;
    odir = env.GetValue("Root.Html.OutputDir", "");
    cout << endl << "Generated doc in directory " << odir.Data() << endl;
	TString cindex(odir);
	cindex += "/ClassIndex.html";
	TString corig(cindex);
	corig.Append("_orig");
	TString mvcmd("mv ");
	mvcmd.Append(cindex);
	mvcmd.Append(" ");
	mvcmd.Append(corig);
	gSystem->Exec(mvcmd);
	ifstream ifile(corig.Data(), ios::in);
	ofstream ofile(cindex.Data(), ios::out);
	TString line;
	TRegexp r1("TMrb");
	TRegexp r2("TGMrb");
	TRegexp r3("TMbs");
	TRegexp r4("TUsr");
	TRegexp r5("DGF");
	TRegexp r6("TSnk");
	TRegexp r7("TPol");
	while (!ifile.eof()) {
		line.ReadLine(ifile, kTRUE); 
		line.Strip(TString::kBoth);
		if (line.BeginsWith("<li><tt>")) {
			if (	line.Index(r1) > 0
				||	line.Index(r2) > 0
				||	line.Index(r3) > 0
				||	line.Index(r4) > 0
				||	line.Index(r5) > 0
				||	line.Index(r6) > 0
				||	line.Index(r7) > 0) ofile << line << endl;
		} else {
			ofile << line << endl;
		}
	}
	gSystem->Exit(0);
}  

