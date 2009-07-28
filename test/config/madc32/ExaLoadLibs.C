//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             ExaLoadLibs.C
// Purpose:          Load libraries needed in an interactive ROOT session
// Description:      RootCint macro for experiment "exa"
//                   (Example)
//                   Loads any libraries needed for an interactive session.
// Author:           Rudolf.Lutter
// Revision:         
// Date:             Thu Jul 16 08:59:50 2009
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading Exa libs]" << endl;
	gSystem->Load("$ROOTSYS/lib/libGed.so");
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMrbTransport.so");
	gSystem->Load("$MARABOU/lib/libMutex.so");
	gSystem->Load("$MARABOU/lib/libTMrbAnalyze.so");
	gSystem->Load("libExaAnalyze.so");
}
