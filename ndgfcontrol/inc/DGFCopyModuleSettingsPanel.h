#ifndef __DGFCopyModuleSettingsPanel_h__
#define __DGFCopyModuleSettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControl.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFCopyModuleSettingsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFCopyModuleSettingsPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGLabel.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TNGMrbTextButton.h"
#include "TNGMrbCheckButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbPictureButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFileEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"

#include "DGFControlCommon.h"
#include "DGFControlData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel
// Purpose:        Main frame to copy module settings
// Constructors:   
// Methods:        ProcessMessage   -- process widget events
// Description:    A dialog window to copy module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFCopyModuleSettingsPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFCopyModuleSettingsCmdId 		{	kDGFCopyModuleSettingsButtonCopy,			//		copy to DSP
													kDGFCopySelectModule,						//		select src
													kDGFCopyModuleSelectAll,					//		select all
													kDGFCopyModuleSelectNone,					//		select none
													kDGFCopyModuleSelectColumn					//		select columns
																						// Attention: HAS TO BE LAST INDEX!
												};

		enum EDGFCopyModuleSettingsActionId 	{
													kDGFCopyBitGain 		= BIT(0),
													kDGFCopyBitOffset		= BIT(1),
													kDGFCopyBitSum			= BIT(2),
													kDGFCopyBitFilter		= BIT(3),
													kDGFCopyBitTrace		= BIT(4),
													kDGFCopyBitPileup		= BIT(5),
													kDGFCopyBitFIFO 		= BIT(6),
													kDGFCopyBitCSR			= BIT(7),
													kDGFCopyBitSwitchBus	= BIT(8),
													kDGFCopyBitCoinc		= BIT(9),
													kDGFCopyBitMCA			= BIT(10),
													kDGFCopyBitThresh		= BIT(11),
													kDGFCopyBitTau			= BIT(12),
													kDGFCopyBitUserPSA		= BIT(13)
												};

	public:
		DGFCopyModuleSettingsPanel(TGCompositeFrame * TabFrame);

		void SelectSource(Int_t FrameId, Int_t Selection);		// slot methods
		void SelectDestination(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t CopyModuleSettings();

	protected:
		TList fHeap;								//! list of objects created on heap

		Int_t fModuleFrom;							// where to copy from
		
		TGGroupFrame * fSelectFrame; 				// select
		TNGMrbLabelCombo * fSelectModule; 			//		module
		TNGMrbRadioButtonList * fSelectChannel;		//		channel

		TNGMrbCheckButtonGroup * fCopyBits;

		TGVerticalFrame * fVFrame;					// vertical frame
		TNGMrbCheckButtonGroup * fChannels;
		TGGroupFrame * fModules;
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
				
		TNGMrbTextButtonGroup * fButtonFrame;

		TMrbLofNamedX fLofSrcModuleKeys;						//! src modules
		TMrbLofNamedX fLofDestModuleKeys[kNofClusters];			//! destination
		TMrbLofNamedX fLofChannels;								//! channel numbers

		TMrbLofNamedX fLofCopyBits;					// list of copy bits
		TMrbLofNamedX fLofButtons;					// list of actions

	ClassDef(DGFCopyModuleSettingsPanel, 0) 		// [DGFControl] Copy module/channel settings
};

#endif
