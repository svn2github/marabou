//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFControl
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Main Program
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFControl.cxx,v 1.5 2005-09-08 13:56:38 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TROOT.h"
#include "TRint.h"
#include "TGClient.h"

#include "TGMrbGeometry.h"
#include "TMrbLogger.h"
#include "DGFControlData.h"
#include "DGFRunControlPanel.h"

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;
extern TGMrbGeometry * gMrbGeometry;

extern void InitGui();

VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControl
// Purpose:        Main Program
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TROOT root("GUI", "DGFControl GUI", initfuncs);

int main(Int_t argc, Char_t **argv)
{
	Int_t nofModules;
	TRint theApp("DGFControl", &argc, argv);

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "dgfControl.log");

	nofModules = 0;
	gDGFControlData = new DGFControlData(".DGFControl.rc");
	if (gDGFControlData->IsZombie() || gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "No DGF modules found. Sorry - can't proceed." << endl;
		gMrbLog->Flush("DGFControl");
		gSystem->Exit(1);
	}

	new TGMrbGeometry("DGFControl");

	DGFRunControlPanel mainWindow(gClient->GetRoot(), gMrbGeometry->GetWidth(), gMrbGeometry->GetHeight());

	theApp.SetPrompt("DGFControl [%d] ");
	theApp.Run();
}
