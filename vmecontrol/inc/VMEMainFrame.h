#ifndef __VMEMainFrame_h__
#define __VMEMainFrame_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMEMainFrame
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: VMEMainFrame.h,v 1.3 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTab.h"

#include "TMrbLofNamedX.h"

#include "TGMrbTextButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbMessageViewer.h"
#include "TGMrbLofKeyBindings.h"

#include "VMEServerPanel.h"
#include "VMESis3302Panel.h"
#include "VMECaen785Panel.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame
// Purpose:        Main frame for the VME Control GUI
// Description:    Defines a main frame for the GUI
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMEMainFrame : public TGMainFrame {

	public:
		// cmd ids to dispatch over X events in main frame
		enum EVMEMainFrameCmdId {		kVMEFileExit,					// pulldown menu "File"

									kVMEViewErrors, 				// pulldown menu "View"

									kVMEGeneralOutputNormal, 		// pulldown menu "General"
									kVMEGeneralOutputVerbose,
									kVMEGeneralOutputDebug,
									kVMEGeneralOffline,
									kVMEGeneralOnline,

									kVMEHelpContents,				// pulldown menu "Help"
									kVMEHelpAbout,
								};

		enum EVMEMainFrameTabId {
									kVMETabServer = 0,
									kVMETabSis3302,
									kVMETabCaen785,
									kVMELastTab
								};


	public:
		VMEMainFrame(const TGWindow * Window, UInt_t Width, UInt_t Height);
		virtual ~VMEMainFrame() { fHeap.Delete(); };

		virtual void CloseWindow();

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
		void MenuSelect(Int_t Selection);			// slot methods
		void TabChanged(Int_t Selection);

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

		TGTab * fTabFrame; 							// main tab frame

		TGCompositeFrame * fServerTab;
		TGCompositeFrame * fSis3302Tab;
		TGCompositeFrame * fCaen785Tab;

		VMEServerPanel * fServerPanel;
		VMESis3302Panel * fSis3302Panel;
		VMECaen785Panel * fCaen785Panel;

		TMrbLofMacros * fLofMacros; 				// list of macros

		TGMrbMessageViewer * fMsgViewer;
		
		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(VMEMainFrame, 0) 	// [VMEControl] Main control panel
};

#endif
