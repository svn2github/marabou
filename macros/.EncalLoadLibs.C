//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             EncalLoadLibs.C
// Purpose:          Load libraries for energy calibration
// Description:      Loads additional libs needed for energy calibration macro "Encal.C"
// Author:           R. Lutter
// Mailto:           <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:         $Id: .EncalLoadLibs.C,v 1.1 2007-03-13 14:25:25 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

{
	if (gROOT->GetVersionInt() >= 51400) {
		cout << "[Loading additional libs: libSpectrum.so]" << endl;
		TMrbSystem ux;
		ux.Load("libSpectrum.so", kTRUE);
	}
}
