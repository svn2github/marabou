#ifndef __DGFSetupPanel_h__
#define __DGFSetupPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSetupPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

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

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel
// Purpose:        Transient frame to define setup params
// Constructors:   
// Description:    A dialog window to enter setup params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFSetupPanel : public TGTransientFrame {

	public:

		// number of ppc procs
		enum					{	kNofPPCs				=	8	};

		// cmd ids to dispatch over X events in this panel
		enum EDGFSetupCmdId 	{	kDGFSetupCamacHost,							// group frame "CAMAC"

									kDGFSetupConnectConnect,					// connect modules
									kDGFSetupConnectReloadDGFs, 				// reload code
									kDGFSetupConnectAbortBusySync,				// abort busy-sync loop
									kDGFSetupConnectRestartEsone,				// restart esone server
									kDGFSetupConnectAbortEsone, 				// abort restart procedure
									kDGFSetupConnectClose,						// do nothing

									kDGFSetupBroadCastOn,						// broadCast on/off
									kDGFSetupBroadCastOff,

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
				
		// geometry settings
		enum					{	kFrameWidth 			= 900					};
		enum					{	kFrameHeight 			= 700					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kNumEntryWidth			= 80					};
		enum					{	kLEHeight				= 20					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFSetupPanel(const TGWindow * Parent, const TGWindow * Main, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFSetupPanel() { fHeap.Delete(); };

		DGFSetupPanel(const DGFSetupPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		Bool_t StartDGFs(EDGFSetupCmdId Mode);	 	// (start ESONE server and) initialize DGF modules
		Bool_t AbortDGFs(); 						// abort busy-sync loop
		
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
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGMrbTextButtonGroup * fActionFrame;		// connect to camac

		TMrbLofNamedX fSetupDGFModes;				// checkbuttons:	dgf modes
		TMrbLofNamedX fSetupDGFCodes;				// checkbuttons:	dgf codes
		TMrbLofNamedX fSetupConnect;				// text buttons:	connect to camac
		TMrbLofNamedX fSetupBroadCast;				// radio buttons:	broadcast on off
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFSetupPanel, 0)		// [DGFControl] Module setup
};

#endif
