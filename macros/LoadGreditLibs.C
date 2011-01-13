//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             LoadHprLibs.C
// Purpose:          Load MARaBOU's utility libraries + HistPresent
// Description:      Loads utility libraries via LD_LIBRARY_PATH
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: LoadGreditLibs.C,v 1.2 2011-01-13 11:20:51 Otto.Schaile Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	cout << "[Loading MARaBOU's utility libs]" << endl;
	Int_t sts = gSystem->Load("libGed.so");
	if (sts == -1) gSystem->Exit(1);
	Int_t sts = gSystem->Load("libTMrbUtils.so");
	if (sts == -1) gSystem->Exit(1);
	TMrbSystem ux;
	ux.PrintLoadPath(sts, "libTMrbUtils.so");
	if (ux.Load("libTGMrbUtils.so") == -1) gSystem->Exit(1);
//	ux.PrintLoadPath(sts, "libFitCal.so");
//	if (ux.Load("libFitCal.so") == -1) gSystem->Exit(1);
//	ux.PrintLoadPath(sts, "libHpr.so");
//	if (ux.Load("libHpr.so") == -1) gSystem->Exit(1);
	ux.PrintLoadPath(sts, "libGrEdit.so");
	if (ux.Load("libGrEdit.so") == -1) gSystem->Exit(1);
}
