#ifndef __DGFEditRunTask_h__
#define __DGFEditRunTask_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditRunTaskPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditRunTaskPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFEditRunTaskPanel.h,v 1.5 2008-12-29 13:48:24 Rudolf.Lutter Exp $       
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
// Name:           DGFEditRunTaskPanel
// Purpose:        Main frame to edit the channel control/status reg
// Description:    A dialog window to modify the channel CSRA
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditRunTaskPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditRunTaskCmdId 	{	kDGFEditRunTaskButtonApply,			//		apply
										kDGFEditRunTaskButtonReset,			//		reset
										kDGFEditRunTaskButtonClose				//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 400					};
		enum					{	kFrameHeight 			= 470					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:
		DGFEditRunTaskPanel(const TGWindow * Parent, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditRunTaskPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot method

	protected:
		TList fHeap;								//! list of objects created on heap
		TGMrbRadioButtonGroup * fRunTaskFrame;	 	// bits to be edited
		TGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TGTextEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofRunTaskBits;				// list of CSRA bits
		TMrbLofNamedX fLofButtons;					// list of buttons

	ClassDef(DGFEditRunTaskPanel, 0)		// [DGFControl] Edit RUNTASK parameter
};

#endif
