#ifndef __DGFSetFilesPanel_h__
#define __DGFSetFilesPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSetFilesPanel
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

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbFocusList.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel
// Purpose:        Main frame to define filenames and paths
// Description:    A dialog window to enter file names and paths
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFSetFilesPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFSetFilesCmdId 	{	kDGFSetFilesUserPath,
									kDGFSetFilesUserSettings,
									kDGFSetFilesUserRunData,
									kDGFSetFilesSystemPath,
									kDGFSetFilesSystemDSPCode,
									kDGFSetFilesSystemDSPParams,
									kDGFSetFilesSystemSystemFPGAConfig,
									kDGFSetFilesSystemFippiFPGAConfig,
									kDGFSetFilesApply,
								};

	public:
		DGFSetFilesPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFSetFilesPanel() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		TList fHeap;									//! list of objects created on heap
		TGGroupFrame * fUserFrame; 						// user
		TGMrbLabelEntry * fUserRunDataEntry;			//		run data file
		TGGroupFrame * fSystemFrame; 					// system
		TGMrbFileEntry * fSystemDSPCodeEntry;			//		DSP code
		TGMrbFileEntry * fSystemDSPParamsEntry; 		//		params
		TGMrbFileEntry * fSystemSystemFPGAConfigEntry;	//		FPGA config (system)
		TGMrbFileEntry * fSystemFippiFPGAConfig[TMrbDGFData::kNofRevs];	//		... (fippi, rev D, E)
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fButtonFrame; 			// buttons: apply/close

		TGFileInfo fSystemDSPCodeFileInfo;				//		DSP code
		TGFileInfo fSystemDSPParamsFileInfo; 			//		params
		TGFileInfo fSystemSystemFPGAFileInfo;			//		FPGA config (system)
		TGFileInfo fSystemFippiFPGAFileInfo[TMrbDGFData::kNofRevs];	//		... (fippi, revD, revE)

		TMrbLofNamedX fLofButtons; 
		TGMrbFocusList fFocusList;

	ClassDef(DGFSetFilesPanel, 0)		// [DGFControl] Define file paths
};

#endif
