#ifndef __DGFSetupPanel_h__
#define __DGFSetupPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSetupPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFSetupPanel.h,v 1.12 2008-08-18 08:19:51 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbCheckButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbPictureButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFocusList.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"
#include "DGFModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel
// Purpose:        Main frame to define setup params
// Constructors:   
// Description:    A dialog window to enter setup params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFSetupPanel : public TGCompositeFrame {

	public:

		// number of ppc procs
		enum					{	kNofPPCs				=	8	};

		// cmd ids to dispatch over X events in this panel
		enum EDGFSetupCmdId 	{	kDGFSetupCamacHost,							// group frame "CAMAC"

									kDGFSetupConnectToEsone,					// connect modules
									kDGFSetupReloadDGFs,						// reload code
									kDGFSetupAbortBusySync,						// abort busy-sync loop
									kDGFSetupRestartEsone,						// restart esone server
									kDGFSetupAbortEsone, 						// abort restart procedure

									kDGFSetupModuleSelectAll,					// select all modules
									kDGFSetupModuleSelectNone,					// select none
									kDGFSetupModuleSelectColumn 				// select modules column-wise
																				// Attention: HAS TO BE LAST INDEX!
								};

		// dgf codes
		enum EDGFSetupCodeId	{	kDGFSetupCodeSystemFPGA = BIT(0),
									kDGFSetupCodeFippiFPGA = BIT(1),
									kDGFSetupCodeDSP = BIT(2)
								};
				
	public:
		DGFSetupPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFSetupPanel() { fHeap.Delete(); };

//		DGFSetupPanel(const DGFSetupPanel & f) : TGCompositeFrame(f) {};	// default copy ctor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		Bool_t ConnectToEsone();					// connect modules to esone server
		Bool_t ReloadDGFs();						// reload dgf code
		Bool_t AbortDGFs(); 						// abort busy-sync loop
		Bool_t RestartEsone();						// restart esone server
		Bool_t TurnUserPSAOnOff(Bool_t ActivateFlag);	// turn user psa on/off
		Bool_t TurnUserPSAOnOff(DGFModule * Module, Bool_t ActivateFlag);
		Bool_t SetSwitchBus(Bool_t IndivFlag);		// set switchbus register
		Bool_t SetInSynch(Bool_t SyncFlag);	// synchronize clock with run
		Bool_t SetInSynch(DGFModule * Module, Bool_t ActivateFlag);
		Bool_t SetSynchWait(Bool_t SimulFlag);	// start/stop simultaneously
		Bool_t SetSynchWait(DGFModule * Module, Bool_t ActivateFlag);
		
	protected:
		Bool_t DaqIsRunning();						// check if a daq is running

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fCAMACFrame; 				// camac defs
		TGMrbLabelEntry * fCAMACHostEntry;			//		default host
		TGMrbCheckButtonGroup * fDGFFrame;			// dgf defs
		TGGroupFrame * fCodeFrame;			 		// codes to be loaded
		TGMrbCheckButtonList * fCodes;
		TGMrbRadioButtonList * fBroadCast;
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fSelectFrame;
		TGMrbPictureButtonList * fColSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGMrbTextButtonGroup * fActionFrame;		// connect to camac

		TMrbLofNamedX fSetupDGFModes;				// checkbuttons:	dgf modes
		TMrbLofNamedX fSetupDGFCodes;				// checkbuttons:	dgf codes
		TMrbLofNamedX fSetupConnect;				// text buttons:	connect to camac
		TMrbLofNamedX fSetupBroadCast;				// radio buttons:	broadcast on off
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFSetupPanel, 0)		// [DGFControl] Module setup
};

#endif
