//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadDGFLibs.C
// Purpose:          Load libraries needed for DGF-4C modules
// Description:      Loads dgf libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadDGFLibs.C,v 1.4 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading DGF libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libEsoneClient.so");
	ux.Load("libTMrbEsone.so");
	ux.Load("libTMrbDGFCommon.so");
	ux.Load("libTMrbDGF.so");
}
