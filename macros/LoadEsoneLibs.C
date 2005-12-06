//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadEsoneLibs.C
// Purpose:          Load esone libraries
// Description:      Loads esone libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadEsoneLibs.C,v 1.2 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading ESONE libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libEsoneClient.so");
	ux.Load("libTMrbEsone.so");
}
