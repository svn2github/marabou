#ifndef __DGFOffsetsPanel_h__
#define __DGFOffsetsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFOffsetsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFOffsetsPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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
#include "TNGMrbFocusList.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel
// Purpose:        Main frame to take untriggered traces
// Constructors:   
// Description:    A dialog window to take untriggered traces
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFOffsetsPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFOffsetsActionId 	{	kDGFOffsetsStart,
										kDGFOffsetsAbort,
										kDGFOffsetsSelectAll,
										kDGFOffsetsSelectNone,
										kDGFOffsetsSelectColumn
									};
				
	public:
		DGFOffsetsPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFOffsetsPanel() { fHeap.Delete(); };

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t StartRamp();

	protected:
		Int_t CalibrateDac(const Char_t * DgfName, Int_t Channel, Int_t Data[], Int_t NofWords, Int_t Offset, Int_t & DacValue);
	
	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fActionFrame;
		TNGMrbTextButtonList * fActionButtons;
		TGGroupFrame * fOffsetFrame;
		TNGMrbLabelEntry * fOffsetValue;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TNGMrbFocusList fFocusList;

	ClassDef(DGFOffsetsPanel, 0)		// [DGFControl] Set offset values
};

#endif
