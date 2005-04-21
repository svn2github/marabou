//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         LoadConfigLibs.C
// @(#)Purpose:      Load libraries needed to run MARaBOU's config
// Description:      Loads config libraries from $LMARABOU/lib
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's *LOCAL* config libs from " << gSystem->ExpandPathName("$LMARABOU/lib") << "]" << endl;
	gSystem->Load("$LMARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$ROOTSYS/lib/libGed.so");
	gSystem->Load("$LMARABOU/lib/libTGMrbUtils.so");
	gSystem->Load("$LMARABOU/lib/libTMbsSetup.so");
	gSystem->Load("$LMARABOU/lib/libTMrbConfig.so");
}
