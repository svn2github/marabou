//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         %M%
// @(#)Purpose:      Load libraries needed to run MARaBOU's config
// Description:      Loads config libraries from $LMARABOU/lib
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading *LOCAL* MARaBOU libs from " << gSystem->ExpandPathName("$LMARABOU/lib") << "]" << endl;
	gSystem->Load("$LMARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$LMARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$LMARABOU/lib/libMutex.so");
	gSystem->Load("$LMARABOU/lib/libTMrbAnalyze.so");
}
