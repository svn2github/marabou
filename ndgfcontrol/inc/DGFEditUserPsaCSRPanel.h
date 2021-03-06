#ifndef __DGFEditUserPsaCSR_h__
#define __DGFEditUserPsaCSR_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditUserPsaCSRPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditUserPsaCSRPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFEditUserPsaCSRPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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

#include "TNGMrbTextButton.h"
#include "TNGMrbCheckButton.h"
#include "TNGMrbLabelEntry.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditUserPsaCSRPanel
// Purpose:        Main frame to edit the channel control/status reg
// Description:    A dialog window to modify the channel CSRA
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditUserPsaCSRPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditUserPsaCSRCmdId 	{	kDGFEditUserPsaCSRButtonApply,			//		apply
										kDGFEditUserPsaCSRButtonReset,			//		reset
										kDGFEditUserPsaCSRButtonClose				//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 350					};
		enum					{	kFrameHeight 			= 600					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:
		DGFEditUserPsaCSRPanel(const TGWindow * Parent, TNGMrbLabelEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditUserPsaCSRPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot method

	protected:
		TList fHeap;								//! list of objects created on heap
		TNGMrbCheckButtonGroup * fCSRFrame;	 		// bits to be edited
		TNGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TNGMrbLabelEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofButtons;					// list of buttons

	ClassDef(DGFEditUserPsaCSRPanel, 0)		// [DGFControl] Edit UserPSA control register
};

#endif
