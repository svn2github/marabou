//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadUtilityLibs.C
// Purpose:          Load MARaBOU's utility libraries
// Description:      Loads utility libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadUtilityLibs.C,v 1.5 2007-05-09 14:12:28 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's utility libs]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libGed.so:libTGMrbUtils.so") == -1) gSystem->Exit(1);
}
