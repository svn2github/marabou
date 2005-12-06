//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadDGFCommonLibs.C
// Purpose:          Load common dgf libraries
// Description:      Loads common dgf libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadDGFCommonLibs.C,v 1.2 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading DGF libs (common part for online and offline)]" << endl;
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	ux.Load("libTMrbDGFCommon.so");
}
