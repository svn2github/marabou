#ifndef __DGFEditCoincPatternPanel_h__
#define __DGFEditCoincPatternPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFEditCoincPattern.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFEditCoincPatternPanel
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
#include "TGMrbLabelEntry.h"
#include "TGMrbLofKeyBindings.h"

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
		DGFEditCoincPatternPanel(const TGWindow * Parent, TGTextEntry * Entry,
											UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFEditCoincPatternPanel() { fHeap.Delete(); };

		DGFEditCoincPatternPanel(const DGFEditCoincPatternPanel & f) : TGMainFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fPatternFrame;
		TGMrbCheckButtonGroup * fLeftFrame;
		TGMrbCheckButtonGroup * fRightFrame;
		TGMrbLabelEntry * fCoincTime;				// coinc time
		TGMrbTextButtonList * fButtonFrame; 		// buttons: apply/close

		TGTextEntry * fEntry;						// entry to be editied

		TMrbLofNamedX fLofCoincPatternsLeft;		// list of coinc patterns (left)
		TMrbLofNamedX fLofCoincPatternsRight;		// list of coinc patterns (right)
		TMrbLofNamedX fLofButtons;					// list of buttons

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFEditCoincPatternPanel, 0)	// [DGFControl] Edit coincidence pattern
};

#endif
