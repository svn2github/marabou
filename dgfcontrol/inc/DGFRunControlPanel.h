#ifndef __DGFRunControlPanel_h__
#define __DGFRunControlPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRunControlPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbMessageViewer.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel
// Purpose:        Main frame for the DGF Control GUI
// Description:    Defines a main frame for the GUI
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFRunControlPanel : public TGMainFrame {

	public:
		// cmd ids to dispatch over X events in main frame
		enum EDGFRunControlCmdId {	kDGFFileExit,					// pulldown menu "File"
									kDGFFileSave,

									kDGFViewErrors, 				// pulldown menu "View"

									kDGFGeneralOutputNormal, 		// pulldown menu "General"
									kDGFGeneralOutputVerbose,
									kDGFGeneralOutputDebug,
									kDGFGeneralModNumLocal,
									kDGFGeneralModNumGlobal,
									kDGFGeneralOffline,
									kDGFGeneralOnline,

									kDGFHelpContents,				// pulldown menu "Help"
									kDGFHelpAbout,

									kDGFRunControlSettingsSystem,	// run control panel "Edit Settings"
									kDGFRunControlSettingsFiles,
									kDGFRunControlSettingsModules,
									kDGFRunControlSettingsParams,

									kDGFRunControlCalibrateTauTrace, 		// run control panel "Tau"
									kDGFRunControlCalibrateTrace, 			// run control panel "Trace"
									kDGFRunControlCalibrateUntrigTrace, 	// run control panel "Untriggered"
									kDGFRunControlCalibrateOffsets,			// run control panel "Offsets"

									kDGFRunControlRunEvents,		// run control panel "Run"
									kDGFRunControlRunMCA,
									kDGFRunControlRunStatistics,

									kDGFRunControlDatabaseSave, 	// run control panel "Database"
									kDGFRunControlDatabaseRestore,

									kDGFRunControlClose 			// close session
								};

		// geometry settings
		enum					{	kFrameWidth 			= 400					};
		enum					{	kFrameHeight 			= 450					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kLEHeight				= 20					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFRunControlPanel(const TGWindow * Window, UInt_t Width, UInt_t Height);
		virtual ~DGFRunControlPanel() { fHeap.Delete(); };

		DGFRunControlPanel(const DGFRunControlPanel & f) : TGMainFrame(f) {};	// default copy ctor

		virtual void CloseWindow();
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
	protected:
		void PopupMessageViewer();
	
	protected:
		TList fHeap;								//! list of objects created on heap

		TGMenuBar * fMenuBar;						// main menu bar
		TGPopupMenu * fMenuFile;					//					pulldown menu: file
		TGPopupMenu * fMenuView;					//					pulldown menu: view
		TGPopupMenu * fMenuGeneral;					//					pulldown menu: general
		TGMrbMacroBrowserPopup * fMenuMacros;		//					pulldown menu: macros
		TGPopupMenu * fMenuHelp;					//					pulldown menu: help

													// run control panel
		TGVerticalFrame * fRunControlFrame;			//					vert. frame to store different items
		TGMrbTextButtonGroup * fSettingsFrame;		//					edit settings
		TGMrbTextButtonGroup * fDatabaseFrame;		//					database
		TGMrbTextButtonGroup * fCalibrateFrame; 	//					calibrate
		TGMrbTextButtonGroup * fRunFrame;			//					run

		TMrbLofMacros * fLofMacros; 				// list of macros

		TMrbLofNamedX fRunControlSettings;			// list of buttons: edit settings
		TMrbLofNamedX fRunControlDatabase;			//					database
		TMrbLofNamedX fRunControlCalibrate;			//					calibrate					
		TMrbLofNamedX fRunControlRun;				//					run

		TGMrbMessageViewer * fMsgViewer;
		
		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFRunControlPanel, 0) 	// [DGFControl] Main control panel
};

#endif
