//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEControl
// Purpose:        A GUI to control VME modules via TCP/IP
// Description:    Main Program
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEControl.cxx,v 1.2 2008-08-28 07:16:48 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-08-28 07:16:48 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TROOT.h"
#include "TRint.h"
#include "TGClient.h"

#include "TGMrbGeometry.h"
#include "TMrbLogger.h"
#include "VMEControlData.h"
#include "VMEMainFrame.h"

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;
extern TGMrbGeometry * gMrbGeometry;

extern void InitGui();

VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControl
// Purpose:        Main Program
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TROOT root("GUI", "VMEControl GUI", initfuncs);

int main(Int_t argc, Char_t **argv)
{
	TRint theApp("VMEControl", &argc, argv);

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("vmeControl.log");

	gVMEControlData = new VMEControlData();
	if (gVMEControlData->IsZombie()) {
		gMrbLog->Err()	<< "Sorry - can't proceed." << endl;
		gMrbLog->Flush("VMEControl");
		gSystem->Exit(1);
	}


	VMEMainFrame * mainWindow = new VMEMainFrame(gClient->GetRoot(), gVMEControlData->GetFrameWidth(), gVMEControlData->GetFrameHeight());

	new TGMrbGeometry(mainWindow);

	theApp.SetPrompt("VMEControl [%d] ");
	theApp.Run();
}
