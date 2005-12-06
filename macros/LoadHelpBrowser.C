//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadHelpBrowser.C
// Purpose:          Load libraries needed for the help browser
// Description:      Loads browser libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadHelpBrowser.C,v 1.2 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's help browser libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libHelpBrowser.so");
}
