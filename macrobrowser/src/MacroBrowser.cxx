//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           MacroBrowser
// Purpose:        A GUI to display and exec root macros
// Description:    Main Program
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: MacroBrowser.cxx,v 1.4 2004-09-28 13:47:33 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TRint.h"
#include "TGClient.h"

#include "TGMrbMacroBrowser.h"

#include "SetColor.h"

//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           MacroBrowser
// Purpose:        Main Program
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	TApplication * theApp = NULL;
	TRint * theRint = NULL;
	Bool_t useRint;

	useRint = gEnv->GetValue("MacroBrowser.UseRint", kFALSE);

	if (useRint) theRint = new TRint("MacroBrowser", &argc, argv);
	else theApp = new TApplication("MacroBrowser", &argc, argv);

	TMrbLofMacros * lofMacros = new TMrbLofMacros("");
	if (argc <= 1) {
		lofMacros->AddMacro("*.C");
	} else {
		for (Int_t i = 1; i < argc; i++) {
			lofMacros->AddMacro(argv[i]);
		}
	}
	if (lofMacros->GetLast() == -1) {
		cerr	<< setred
				<< "MacroBrowser: No ROOT macros found or wrong format - *.C" << endl
				<< "              (Path=" << lofMacros->GetPath() << ")"
				<< setblack << endl;
	}

	TGMrbMacroBrowserMain mainWindow((const TGWindow *) gClient->GetRoot(), lofMacros, 600, 500);

	if (useRint) theRint->Run(); else theApp->Run();
}
