//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadConfigLibs.C
// Purpose:          Load MARaBOU's config libraries
// Description:      Loads config libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadConfigLibs.C,v 1.7 2007-05-09 14:12:28 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's config libs]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libGed.so:libTGMrbUtils.so:libTMbsSetup.so:libTMrbConfig.so") == -1) gSystem->Exit(1);
}
