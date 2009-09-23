#ifndef __DGFEditModICSRPanel_h__
#define __DGFEditModICSRPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditModICSRPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFEditModICSRPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditModICSRPanel
// Purpose:        Main frame to edit the ICS reg
// Constructors:   
// Methods:        ProcessMessage   -- process widget events
// Description:    A dialog window to modify the module ICSR
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditModICSRPanel : public TGMainFrame {

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
		DGFEditModICSRPanel(const TGWindow * Parent, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditModICSRPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot method

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

	ClassDef(DGFEditModICSRPanel, 0)		// [DGFControl] Edit switch register (ICSR) settings
};

#endif
