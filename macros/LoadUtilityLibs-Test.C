//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         %M%
// @(#)Purpose:      Load MARaBOU's utility libs
// Description:      Loads utility libraries from $LMARABOU/lib
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading *LOCAL* utility libs from " << gSystem->ExpandPathName("$LMARABOU/lib") << "]" << endl;
	gSystem->Load("$LMARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$ROOTSYS/lib/libGed.so");
	gSystem->Load("$LMARABOU/lib/libTGMrbUtils.so");
}
