#ifndef __DGFEditModCSRAPanel_h__
#define __DGFEditModCSRAPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModCSRAPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditModCSRAPanel
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
#include "TGTextEntry.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbCheckButton.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModCSRAPanel
// Purpose:        Transient frame to edit the module control/status reg
// Constructors:   
// Methods:        ProcessMessage   -- process widget events
// Description:    A dialog window to modify the module CSRA
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditModCSRAPanel : public TGTransientFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditModCSRACmdId 	{	kDGFEditModCSRAButtonApply,			//		apply
										kDGFEditModCSRAButtonReset,			//		reset
										kDGFEditModCSRAButtonClose			//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 350					};
		enum					{	kFrameHeight 			= 150					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:
		DGFEditModCSRAPanel(const TGWindow * Parent, const TGWindow * Main, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditModCSRAPanel() { fHeap.Delete(); };

		DGFEditModCSRAPanel(const DGFEditModCSRAPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		TList fHeap;								//! list of objects created on heap
		TGMrbCheckButtonGroup * fCSRAFrame; 		// bits to be edited
		TGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TGTextEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofCSRABits;					// list of CSRA bits
		TMrbLofNamedX fLofButtons;					// list of buttons

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFEditModCSRAPanel, 0)		// [DGFControl] Edit module control & status register
};

#endif
