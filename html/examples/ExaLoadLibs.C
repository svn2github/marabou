//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             ExaLoadLibs.C
// Purpose:          Load libraries needed in an interactive ROOT session
// Description:      RootCint macro for experiment "exa"
//                   (Example configuration)
//                   Loads any libraries needed for an interactive session.
// Author:           MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:             Thu Nov 28 09:12:18 2002
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading Exa libs]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMrbTransport.so");
	gSystem->Load("$MARABOU/lib/libMutex.so");
	gSystem->Load("$MARABOU/lib/libTMrbAnalyze.so");
	gSystem->Load("libExaAnalyze.so");
}
