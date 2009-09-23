#ifndef __DGFParamsPanel_h__
#define __DGFParamsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFParamsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFParamsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFParamsPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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

#include "TNGMrbCheckButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbTextButton.h"
#include "TNGMrbPictureButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TNGMrbFocusList.h"

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

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void SelectParam(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);
		void RadioButtonPressed(Int_t FrameId, Int_t Selection);

		Bool_t ReadParams();
		Bool_t ApplyParams(Bool_t Marked = kFALSE);
		
	protected:
		Int_t fActiveParam; 						// index of active param
	
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fSelectFrame; 				// select
		TNGMrbLabelCombo * fSelectParam; 			//		param
		TNGMrbRadioButtonList * fAlpha;				//		alpha list
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fValueFrame; 			 	// param values
		TGHorizontalFrame * fClusterVals[kNofClusters];
		TNGMrbLabelEntry * fParVal[kNofClusters * kNofModulesPerCluster];
		TNGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		TMrbLofNamedX fLofParams;					// param names, sorted
		TMrbLofNamedX fLofInitials; 				// chars A .. Z
		
		TNGMrbFocusList fFocusList;

		TMrbDGFData * fDGFData; 					// local data struct

	ClassDef(DGFParamsPanel, 0)		// [DGFControl] Set/display module params
};

#endif
