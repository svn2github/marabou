#ifndef __DGFParamsPanel_h__
#define __DGFParamsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFParamsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFParamsPanel.h,v 1.9 2007-10-22 16:45:37 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

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

class DGFParamsPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFParamsActionId 	{	kDGFParamsSelectParam,
										kDGFParamsSelectChannel,
										kDGFParamsRead,
										kDGFParamsApply,
										kDGFParamsApplyMarked,
										kDGFParamsSelectAll,
										kDGFParamsSelectNone,
										kDGFParamsSelectColumn
																// Attention: HAS TO BE LAST INDEX!
									};
				
	public:
		DGFParamsPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFParamsPanel() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		Bool_t ReadParams();
		Bool_t ApplyParams(Bool_t Marked = kFALSE);
		
		void RadioButtonPressed(Int_t Button);		// signal catcher

	protected:
		Int_t fActiveParam; 						// index of active param
	
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectParam; 			//		param
		TGMrbRadioButtonList * fAlpha;				//		alpha list
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
		TMrbLofNamedX fLofParams;					// param names, sorted
		TMrbLofNamedX fLofInitials; 				// chars A .. Z
		
		TGMrbFocusList fFocusList;

		TMrbDGFData * fDGFData; 					// local data struct

	ClassDef(DGFParamsPanel, 0)		// [DGFControl] Set/display module params
};

#endif
