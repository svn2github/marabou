//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadConfigLibs.C
// Purpose:          Load MARaBOU's config libraries
// Description:      Loads config libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadConfigLibs.C,v 1.5 2005-12-06 14:20:30 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's config libs]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libGed.so");
	ux.Load("libTGMrbUtils.so");
	ux.Load("libTMbsSetup.so");
	ux.Load("libTMrbConfig.so");
}
