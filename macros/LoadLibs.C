//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadLibs.C
// Purpose:          Load MARaBOU's standard libraries
// Description:      Loads standard libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadLibs.C,v 1.4 2005-12-07 15:05:10 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libGed.so:libTGMrbUtils.so:libHelpBrowser.so:libMutex.so:libTMrbAnalyze.so") == -1) gSystem->Exit(1);
}
