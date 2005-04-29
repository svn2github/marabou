//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFControl
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Main Program
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: CptmControl.cxx,v 1.1 2005-04-29 11:35:22 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TROOT.h"
#include "TRint.h"
#include "TGClient.h"

#include "TMrbLogger.h"
#include "CptmControlData.h"
#include "CptmPanel.h"

CptmControlData * gCptmControlData = NULL;

extern TMrbLogger * gMrbLog;

extern void InitGui();

VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControl
// Purpose:        Main Program
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TROOT root("GUI", "CptmControl GUI", initfuncs);

int main(Int_t argc, Char_t **argv)
{
	Int_t nofModules;
	TRint theApp("CptmControl", &argc, argv);

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "cptmControl.log");

	nofModules = 0;
	gEnv->ReadFile(".DGFControl.rc", kEnvLocal);
	gEnv->ReadFile(".CptmControl.rc", kEnvLocal);
	gCptmControlData = new CptmControlData();

	CptmPanel mainWindow(gClient->GetRoot(), kTabWidth, kTabHeight);

	theApp.Run();
}
