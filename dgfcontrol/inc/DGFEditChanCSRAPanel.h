#ifndef __DGFEditChanCSRA_h__
#define __DGFEditChanCSRA_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditChanCSRAPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFEditChanCSRAPanel.h,v 1.8 2008-12-29 13:48:24 Rudolf.Lutter Exp $       
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

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditChanCSRAPanel
// Purpose:        Main frame to edit the channel control/status reg
// Description:    A dialog window to modify the channel CSRA
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditChanCSRAPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditChanCSRACmdId 	{	kDGFEditChanCSRAButtonApply,			//		apply
										kDGFEditChanCSRAButtonReset,			//		reset
										kDGFEditChanCSRAButtonClose				//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 350					};
		enum					{	kFrameHeight 			= 480					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:	
		DGFEditChanCSRAPanel(const TGWindow * Parent, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditChanCSRAPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot method

	protected:
		TList fHeap;								//! list of objects created on heap
		TGMrbCheckButtonGroup * fCSRAFrame; 		// bits to be edited
		TGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TGTextEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofButtons;					// list of buttons

	ClassDef(DGFEditChanCSRAPanel, 0)		// [DGFControl] Edit channel control & status register
};

#endif
