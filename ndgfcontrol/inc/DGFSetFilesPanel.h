#ifndef __DGFSetFilesPanel_h__
#define __DGFSetFilesPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSetFilesPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFSetFilesPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TNGMrbTextButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFileEntry.h"
#include "TNGMrbFocusList.h"

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
									kDGFSetFilesSystemUPSAParams,
									kDGFSetFilesSystemSystemFPGAConfig,
									kDGFSetFilesSystemFippiFPGAConfig,
									kDGFSetFilesSystemDgfSettings,
									kDGFSetFilesSystemCptmCode,
									kDGFSetFilesSystemCptmSettings,
									kDGFSetFilesApply,
								};

	public:
		DGFSetFilesPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFSetFilesPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	//slot method

	protected:
		TList fHeap;									//! list of objects created on heap
		TGGroupFrame * fUserFrame; 						// user
		TNGMrbLabelEntry * fUserRunDataEntry;			//		run data file
		TGGroupFrame * fSystemDgfFrame; 				// system (dgf)
		TNGMrbFileEntry * fSystemDSPCodeEntry;			//		DSP code
		TNGMrbFileEntry * fSystemDSPParamsEntry; 		//		params
		TNGMrbFileEntry * fSystemUPSAParamsEntry; 		//		... user psa
		TNGMrbFileEntry * fSystemSystemFPGAConfigEntry;	//		FPGA config (system)
		TNGMrbFileEntry * fSystemFippiFPGAConfig[TMrbDGFData::kNofRevs];	//		... (fippi, rev D, E)
		TNGMrbFileEntry * fSystemDgfSettingsEntry;		//		dgf settings
		TGGroupFrame * fSystemCptmFrame; 				// system (cptm)
		TNGMrbFileEntry * fSystemCptmCodeEntry;			//		C_PTM module
		TNGMrbFileEntry * fSystemCptmSettingsEntry;		//		cptm settings

		TGGroupFrame * fActionFrame;
		TNGMrbTextButtonList * fButtonFrame; 			// buttons: apply/close

														// file info
		TGFileInfo fSystemDSPCodeFileInfo;				//		DSP code
		TGFileInfo fSystemDSPParamsFileInfo; 			//		params
		TGFileInfo fSystemUPSAParamsFileInfo; 			//		... user psa
		TGFileInfo fSystemSystemFPGAFileInfo;			//		FPGA config (system)
		TGFileInfo fSystemFippiFPGAFileInfo[TMrbDGFData::kNofRevs];	//		... (fippi, revD, revE)
		TGFileInfo fSystemDgfSettingsPathInfo;			//		dgf settings
		TGFileInfo fSystemCptmCodeFileInfo;				//		CPTM code
		TGFileInfo fSystemCptmSettingsPathInfo;			//		cptm settings

		TMrbLofNamedX fLofButtons; 
		TNGMrbFocusList fFocusList;

	ClassDef(DGFSetFilesPanel, 0)		// [DGFControl] Define file paths
};

#endif
