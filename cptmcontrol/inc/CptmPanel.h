#ifndef __CptmPanel_h__
#define __CptmPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          CptmPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: CptmPanel.h,v 1.1 2005-04-29 11:35:22 rudi Exp $       
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

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmPanel
// Purpose:        Main frame to control a cptm module
// Constructors:   
// Description:    A dialog window to enter settings for a cptm module
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class CptmPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum ECptmCmdId 	{
									kCptmFileExit,					// pulldown menu "File"
									kCptmSelectModule, 				//		module
									kCptmGeDelayEntry, 				//		DGG(Ge):		delay
									kCptmGeWidthEntry,			  	//						width
									kCptmAuxDelayEntry, 				//		DGG(Aux):		delay
									kCptmAuxWidthEntry,			  	//						width

									kCptmTimeWdwEntry,				//		time window aux
									kCptmMultValueEntry, 			//		multiplicity	value
									kCptmMultDacEntry,				//						dac
									kCptmAddrReadEntry,				//		address 		(read)
									kCptmAddrWriteEntry,				//						(write)
									kCptmMaskRegisterBtns,				//		mask register
									kCptmButtonDownloadCode,			//	buttons:	download
									kCptmButtonReset,				//				reset
									kCptmButtonSave, 				//				save settings
									kCptmButtonRestore, 				//				restore settings
									kCptmButtonSynchEnable, 			//				synch enable
									kCptmButtonSynchEnableReset, 	//				+ reset clock
									kCptmButtonShowBuffer,		 	//				show buffer data
								};


		enum ECptmMaskBits 	{		kCptmMaskE2	=	BIT(0),
									kCptmMaskT2	=	BIT(1),
									kCptmMaskT1	=	BIT(2),
									kCptmMaskQ4	=	BIT(3),
									kCptmMaskQ3	=	BIT(4),
									kCptmMaskQ2	=	BIT(5),
									kCptmMaskQ1	=	BIT(6),
									kCptmMaskGe	=	BIT(7)
								};

	public:
		CptmPanel(const TGWindow * Window, UInt_t Width, UInt_t Height);
		virtual ~CptmPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		Int_t GetLofCptmModules();
		inline Int_t GetNofCptmModules() { return(fLofCptmModules.GetEntriesFast()); };

		virtual void CloseWindow();
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

		TGMenuBar * fMenuBar;						// main menu bar
		TGPopupMenu * fMenuFile;					//					pulldown menu: file

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

	ClassDef(CptmPanel, 0) 	// [CptmControl] Edit C_PTM settings
};

#endif
