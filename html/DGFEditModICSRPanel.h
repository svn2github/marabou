#ifndef __DGFEditModICSRPanel_h__
#define __DGFEditModICSRPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditModICSRPanel
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
#include "TGMrbRadioButton.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel
// Purpose:        Transient frame to edit the ICS reg
// Constructors:   
// Methods:        ProcessMessage   -- process widget events
// Description:    A dialog window to modify the module ICSR
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditModICSRPanel : public TGTransientFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditModICSRCmdId 	{	kDGFEditModICSRButtonApply,			//		apply
										kDGFEditModICSRButtonReset,			//		reset
										kDGFEditModICSRButtonClose,			//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 350					};
		enum					{	kFrameHeight 			= 450					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:
		DGFEditModICSRPanel(const TGWindow * Parent, const TGWindow * Main, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditModICSRPanel() { fHeap.Delete(); };

		DGFEditModICSRPanel(const DGFEditModICSRPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fICSRFrame;
		TGMrbRadioButtonGroup * fDSPTriggerFrame; 	// bits to be edited (DSP trigger)
		TGMrbRadioButtonGroup * fFastTriggerFrame; 	// bits to be edited (fast trigger)
		TGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TGTextEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofDSPTriggerBits;			// dsp trigger chain
		TMrbLofNamedX fLofFastTriggerBits;			// fast trigger chain
		TMrbLofNamedX fLofButtons;					// list of buttons

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFEditModICSRPanel, 0)		// [DGFControl] Edit switch register (ICSR) settings
};

#endif
