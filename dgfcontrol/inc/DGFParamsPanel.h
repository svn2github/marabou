#ifndef __DGFParamsPanel_h__
#define __DGFParamsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFParamsPanel
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
#include "TGMrbLabelCombo.h"
#include "TGMrbFocusList.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel
// Purpose:        Main frame to set module parameters
// Constructors:   
// Description:    A dialog window to set module parameters
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFParamsPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFParamsActionId 	{	kDGFParamsSelectParam,
										kDGFParamsRead,
										kDGFParamsApply,
										kDGFParamsClose,
										kDGFParamsSelectAll,
										kDGFParamsSelectNone,
										kDGFParamsSelectColumn
																// Attention: HAS TO BE LAST INDEX!
									};
				
		// geometry settings
		enum					{	kFrameWidth 			= 900					};
		enum					{	kFrameHeight 			= 780					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kNumEntryWidth			= 80					};
		enum					{	kLEHeight				= 20					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFParamsPanel(const TGWindow * Parent, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFParamsPanel() { fHeap.Delete(); };

		DGFParamsPanel(const DGFParamsPanel & f) : TGMainFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		Bool_t ReadParams();
		Bool_t ApplyParams();
		
	protected:
		Int_t fActiveParam; 						// index of active param
	
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectParam; 			//		param
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fValueFrame; 			 	// param values
		TGHorizontalFrame * fClusterVals[kNofClusters];
		TGMrbLabelEntry * fParVal[kNofClusters * kNofModulesPerCluster];
		TGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFParamsPanel, 0)		// [DGFControl] Set/display module params
};

#endif
