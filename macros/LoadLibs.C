//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadLibs.C
// Purpose:          Load MARaBOU's standard libraries
// Description:      Loads standard libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadLibs.C,v 1.3 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libGed.so");
	ux.Load("libTGMrbUtils.so");
	ux.Load("libHelpBrowser.so");
	ux.Load("libMutex.so");
	ux.Load("libTMrbAnalyze.so");
}
