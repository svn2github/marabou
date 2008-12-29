#ifndef __DGFOffsetsPanel_h__
#define __DGFOffsetsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFOffsetsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFOffsetsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFOffsetsPanel.h,v 1.10 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
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
#include "TGMrbFocusList.h"

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
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fActionButtons;
		TGGroupFrame * fOffsetFrame;
		TGMrbLabelEntry * fOffsetValue;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFOffsetsPanel, 0)		// [DGFControl] Set offset values
};

#endif
