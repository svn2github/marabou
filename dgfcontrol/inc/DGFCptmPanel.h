#ifndef __DGFCptmPanel_h__
#define __DGFCptmPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFCptmPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFCptmPanel.h,v 1.1 2005-04-28 13:00:26 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbCheckButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFModule.h"
#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel
// Purpose:        Main frame to control a cptm module
// Constructors:   
// Description:    A dialog window to enter settings for a cptm module
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFCptmPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFCptmCmdId 	{
									kDGFCptmSelectModule, 				//		module
									kDGFCptmGeDelayEntry, 				//		DGG(Ge):		delay
									kDGFCptmGeWidthEntry,			  	//						width
									kDGFCptmAuxDelayEntry, 				//		DGG(Aux):		delay
									kDGFCptmAuxWidthEntry,			  	//						width

									kDGFCptmTimeWdwEntry,				//		time window aux
									kDGFCptmMultValueEntry, 			//		multiplicity	value
									kDGFCptmMultDacEntry,				//						dac
									kDGFCptmAddrReadEntry,				//		address 		(read)
									kDGFCptmAddrWriteEntry,				//						(write)
									kDGFCptmMaskRegisterBtns,				//		mask register
									kDGFCptmButtonDownloadCode,			//	buttons:	download
									kDGFCptmButtonReset,				//				reset
									kDGFCptmButtonSave, 				//				save settings
									kDGFCptmButtonRestore, 				//				restore settings
									kDGFCptmButtonSynchEnable, 			//				synch enable
									kDGFCptmButtonSynchEnableReset, 	//				+ reset clock
									kDGFCptmButtonShowBuffer,		 	//				show buffer data
								};


		enum EDGFCptmMaskBits 	{	kDGFCptmMaskE2	=	BIT(0),
									kDGFCptmMaskT2	=	BIT(1),
									kDGFCptmMaskT1	=	BIT(2),
									kDGFCptmMaskQ4	=	BIT(3),
									kDGFCptmMaskQ3	=	BIT(4),
									kDGFCptmMaskQ2	=	BIT(5),
									kDGFCptmMaskQ1	=	BIT(6),
									kDGFCptmMaskGe	=	BIT(7)
								};

	public:
		DGFCptmPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFCptmPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		Int_t GetLofCptmModules();
		inline Int_t GetNofCptmModules() { return(fLofCptmModules.GetEntriesFast()); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		void InitializeValues(Int_t ModuleIndex = -1);

		void Reset(Int_t ModuleIndex = -1);
		Bool_t DownloadCode(Int_t ModuleIndex = -1);
		Bool_t SaveSettings(Int_t ModuleIndex = -1);
		Bool_t RestoreSettings(Int_t ModuleIndex = -1);
		void EnableSynch(Int_t ModuleIndex = -1, Bool_t ResetClock = kFALSE);
		void ShowBuffer(Int_t ModuleIndex) {};

	protected:
		void UpdateValue(Int_t EntryId, Int_t ModuleIndex);
		void MoveFocus(Int_t EntryId);

	protected:
		TList fHeap;								//! list of objects created on heap

		Int_t fCptmIndex;							// current cptm index

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule; 			//		module

		TGHorizontalFrame * fH1Frame;				// horiz frame for DGGs & Mult
		TGGroupFrame * fGeFrame;					//		DGG(Ge)
		TGMrbLabelEntry * fGeDelayEntry;			//			delay
		TGMrbLabelEntry * fGeWidthEntry;			//			width
		TGGroupFrame * fAuxFrame;					//		DGG(Aux)
		TGMrbLabelEntry * fAuxDelayEntry;			//			delay
		TGMrbLabelEntry * fAuxWidthEntry;			//			width
		TGMrbLabelEntry * fTimeWdwEntry; 			//			time window
		TGGroupFrame * fMultFrame;					//		multiplicity
		TGMrbLabelEntry * fMultValueEntry; 			//			value
		TGMrbLabelEntry * fMultVoltsEntry; 			//			volts
		TGMrbLabelEntry * fMultDacEntry; 			//			dac

		TGHorizontalFrame * fH2Frame;				// horiz frame for mask & addr
		TGMrbCheckButtonGroup * fCptmMaskReg;		//		mask register
		TGGroupFrame * fAddrFrame;					//		addr pointers
		TGMrbLabelEntry * fAddrReadEntry; 			//			read
		TGMrbLabelEntry * fAddrWriteEntry; 			//			write

		TGMrbTextButtonGroup * fCptmButtonFrame;	// buttons

		TMrbLofNamedX fLofCptmModules;				//! list of modules
		TMrbLofNamedX fCptmActions;					//! list of actions

		TGMrbFocusList fFocusList;

	ClassDef(DGFCptmPanel, 0) 	// [DGFControl] Edit C_PTM settings
};

#endif
