//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadConfigLibs.C
// Purpose:          Load MARaBOU's config libraries
// Description:      Loads config libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadConfigLibs.C,v 1.4 2005-12-06 14:01:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's config libs]" << endl;
	gSystem->Load("libTMrbUtils.so");
	gSystem->Load("libGed.so");
	gSystem->Load("libTGMrbUtils.so");
	gSystem->Load("libTMbsSetup.so");
	gSystem->Load("libTMrbConfig.so");
}
