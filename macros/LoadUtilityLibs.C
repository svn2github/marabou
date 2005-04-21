//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         %M%
// @(#)Purpose:      Load MARaBOU's utility libraries
// Description:      Loads utility libraries from /usr/local/marabou
// @(#)Author:       MBS and ROOT Based Online Offline Utility
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Aug 10 14:00:16 1999
// URL:              
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's utility libs from " << gSystem->ExpandPathName("$MARABOU/lib") << "]" << endl;
	gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
	gSystem->Load("$ROOTSYS/lib/libGed.so");
	gSystem->Load("$MARABOU/lib/libTGMrbUtils.so");
}
