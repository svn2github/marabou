#ifndef __DGFSetFilesPanel_h__
#define __DGFSetFilesPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSetFilesPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFSetFilesPanel.h,v 1.8 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
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
		TGMrbLabelEntry * fUserRunDataEntry;			//		run data file
		TGGroupFrame * fSystemDgfFrame; 				// system (dgf)
		TGMrbFileEntry * fSystemDSPCodeEntry;			//		DSP code
		TGMrbFileEntry * fSystemDSPParamsEntry; 		//		params
		TGMrbFileEntry * fSystemUPSAParamsEntry; 		//		... user psa
		TGMrbFileEntry * fSystemSystemFPGAConfigEntry;	//		FPGA config (system)
		TGMrbFileEntry * fSystemFippiFPGAConfig[TMrbDGFData::kNofRevs];	//		... (fippi, rev D, E)
		TGMrbFileEntry * fSystemDgfSettingsEntry;		//		dgf settings
		TGGroupFrame * fSystemCptmFrame; 				// system (cptm)
		TGMrbFileEntry * fSystemCptmCodeEntry;			//		C_PTM module
		TGMrbFileEntry * fSystemCptmSettingsEntry;		//		cptm settings

		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fButtonFrame; 			// buttons: apply/close

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
		TGMrbFocusList fFocusList;

	ClassDef(DGFSetFilesPanel, 0)		// [DGFControl] Define file paths
};

#endif
