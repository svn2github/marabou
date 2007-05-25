//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadDGFLibs.C
// Purpose:          Load libraries needed for DGF-4C modules
// Description:      Loads dgf libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadDGFLibs.C,v 1.7 2007-05-25 10:48:50 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading DGF libs]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libEsoneClient.so:libTMrbC2Lynx.so:libTMrbEsone.so:libTMrbDGFCommon.so:libTMrbDGF.so") == -1) gSystem->Exit(1);
}
