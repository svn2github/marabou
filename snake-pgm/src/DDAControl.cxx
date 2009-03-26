//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           snake/src/DDAControl.cxx
// Purpose:        A GUI to control the DDA0816 device
// Description:    
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DDAControl.cxx,v 1.1 2009-03-26 11:13:41 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TFile.h"
#include "TGFileDialog.h"
#include "TGLabel.h"
#include "TGMsgBox.h"

#include "TMrbEnv.h"
#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "DDAControlPanel.h"
#include "TSnkDDA0816.h"
#include "TSnkDDACommon.h"

#include "SetColor.h"

TSnkDDA0816 * curDDA = NULL;

extern TMrbLogger * gMrbLog;		// access to MARaBOU's message logging

//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControl
// Purpose:        Main Program
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	TApplication * theApp;
	TRint * theRint;
	Bool_t useRint;

	useRint = gEnv->GetValue("DDAControl.UseRint", kFALSE);

	if (useRint) theRint = new TRint("DDAControl", &argc, argv);
	else theApp = new TApplication("DDAControl", &argc, argv);

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("ddaControl.log", "DDAControl");
	
	curDDA = new TSnkDDA0816();

	DDAControlPanel mainWindow(gClient->GetRoot(), kMainFrameWidth, kMainFrameHeight);

	if (useRint) theRint->Run(); else theApp->Run();
}
