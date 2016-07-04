//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadDGFCommonLibs.C
// Purpose:          Load common dgf libraries
// Description:      Loads common dgf libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadDGFCommonLibs.C,v 1.4 2007-05-09 14:12:28 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading DGF libs (common part for online and offline)]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libTMrbDGFCommon.so") == -1) gSystem->Exit(1);
}
