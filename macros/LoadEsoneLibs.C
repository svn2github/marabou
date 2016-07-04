//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadEsoneLibs.C
// Purpose:          Load esone libraries
// Description:      Loads esone libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadEsoneLibs.C,v 1.5 2007-05-09 14:12:28 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading ESONE libs]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libTMrbC2Lynx.so:libEsoneClient.so:libTMrbEsone.so") == -1) gSystem->Exit(1);
}
