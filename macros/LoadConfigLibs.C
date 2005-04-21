//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         %M%
// @(#)Purpose:      Load libraries needed to run MARaBOU's config
// Description:      Loads config libraries from /usr/local/marabou
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's config libs from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$ROOTSYS/lib/libGed.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$MARABOU/lib/libTMbsSetup.so");
	gSystem->Load("$MARABOU/lib/libTMrbConfig.so");
}
