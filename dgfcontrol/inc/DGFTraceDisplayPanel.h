#ifndef __DGFTraceDisplayPanel_h__
#define __DGFTraceDisplayPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFTraceDisplayPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <iostream>
#include <iomanip>

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbCheckButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbPictureButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFocusList.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel
// Purpose:        Transient frame to take untriggered traces
// Constructors:   
// Description:    A dialog window to take untriggered traces
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFTraceDisplayPanel : public TGTransientFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFTraceDisplayActionId 	{
														kDGFTraceDisplayXwait,
														kDGFTraceDisplayStart,
														kDGFTraceDisplayClose,
														kDGFTraceDisplaySelectAll,
														kDGFTraceDisplaySelectNone,
														kDGFTraceDisplaySelectColumn
										};
				
		// geometry settings
		enum					{	kFrameWidth 			= 750					};
		enum					{	kFrameHeight 			= 450					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kNumEntryWidth			= 80					};
		enum					{	kLEHeight				= 20					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFTraceDisplayPanel(const TGWindow * Parent, const TGWindow * Main, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFTraceDisplayPanel() { fHeap.Delete(); };

		DGFTraceDisplayPanel(const DGFTraceDisplayPanel & f) : TGTransientFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t StartTrace();
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fTFrame;
		TGMrbLabelEntry * fTraceLength;						// trace length
		TGGroupFrame * fXFrame;
		TGMrbLabelEntry * fXwait;							// wait states
		TGMrbCheckButtonGroup * fSelectChannel;		//	channel
		TGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFTraceDisplayPanel, 0)		// [DGFControl] Collect a trace
};

#endif
