//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         %M%
// @(#)Purpose:      Load MARaBOU's standard libraries
// Description:      Loads MARaBOU libraries from /usr/local/marabou
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU libs from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libHelpBrowser.so");
	gSystem->Load("$MARABOU/lib/libMutex.so");
	gSystem->Load("$MARABOU/lib/libTMrbAnalyze.so");
}
