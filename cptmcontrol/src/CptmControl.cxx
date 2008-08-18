//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFControl
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Main Program
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: CptmControl.cxx,v 1.3 2008-08-18 08:19:51 Rudolf.Lutter Exp $       
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
#include "CptmControlData.h"
#include "CptmPanel.h"

CptmControlData * gCptmControlData = NULL;

extern TMrbLogger * gMrbLog;
extern TGMrbGeometry * gMrbGeometry;

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("cptmControl.log");

	nofModules = 0;
	gEnv->ReadFile(".DGFControl.rc", kEnvLocal);
	gEnv->ReadFile(".CptmControl.rc", kEnvLocal);
	gCptmControlData = new CptmControlData();

	new TGMrbGeometry("CptmControl");

	CptmPanel mainWindow(gClient->GetRoot(), gMrbGeometry->GetWidth(), gMrbGeometry->GetHeight());

	theApp.SetPrompt("CptmControl [%d] ");
	theApp.Run();
}
