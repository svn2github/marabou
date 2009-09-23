#ifndef __DGFEditCoincPatternPanel_h__
#define __DGFEditCoincPatternPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditCoincPattern.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditCoincPatternPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFEditCoincPatternPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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
// Name:           DGFEditCoincPatternPanel
// Purpose:        Main frame to edit coinc patterns
// Description:    A dialog window to modify coinc patterns
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFEditCoincPatternPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFEditCoincPatternCmdId 	{	kDGFEditCoincPatternButtonApply,			//		apply
											kDGFEditCoincPatternButtonSetAll,			//		set all bits
											kDGFEditCoincPatternButtonReset,			//		reset
											kDGFEditCoincPatternButtonClose				//		close
									};

		// geometry settings
		enum					{	kFrameWidth 			= 350					};
		enum					{	kFrameHeight 			= 380					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 400					};
		enum					{	kLEHeight				= 20					};

		enum					{	kButtonWidth			= kAutoWidth			};
		enum					{	kButtonHeight			= 20					};

	public:
		DGFEditCoincPatternPanel(const TGWindow * Parent, TNGMrbLabelEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditCoincPatternPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot method

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fPatternFrame;
		TNGMrbCheckButtonGroup * fLeftFrame;
		TNGMrbCheckButtonGroup * fRightFrame;
		TNGMrbLabelEntry * fCoincTime;				// coinc time
		TNGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TNGMrbLabelEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofCoincPatternsLeft;		// list of coinc patterns (left)
		TMrbLofNamedX fLofCoincPatternsRight;		// list of coinc patterns (right)
		TMrbLofNamedX fLofButtons;					// list of buttons

	ClassDef(DGFEditCoincPatternPanel, 0)	// [DGFControl] Edit coincidence pattern
};

#endif
