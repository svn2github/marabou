#ifndef __VMEControlData_h__
#define __VMEControlData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData.h
// Purpose:        Class defs for VME modules
// Class:          VMEControlData
// Description:    A GUI to control VME modules
// Author:         R. Lutter
// Revision:       $Id: VMEControlData.h,v 1.4 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-09-23 10:44:11 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TEnv.h"
#include "TString.h"
#include "TList.h"

#include "TGLayout.h"
#include "TGClient.h"
#include "TGMsgBox.h"

#include "TGMrbLayout.h"

#include "TMrbResource.h"
#include "TMrbNamedX.h"

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst(x)

enum						{	kAutoWidth			= 1 		};
enum						{	kAutoHeight			= 1 		};

// geometry settings
enum						{	kFrameWidth 		= 1000		};
enum						{	kFrameHeight 		= 600		};
enum						{	kTabWidth 			= kFrameWidth		};
enum						{	kTabHeight 			= kFrameHeight		};
enum						{	kLEWidth			= kFrameWidth/2			};
enum						{	kLEHeight			= 20					};
enum						{	kEntryWidth			= kFrameWidth/4			};
enum						{	kButtonWidth		= kFrameWidth/8			};
enum						{	kButtonHeight		= kLEHeight				};

enum						{	kVMENofPPCs 		= 20					};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData
// Purpose:        Common database to store any DGF data
// Description:    Common data base for all DGF modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMEControlData : public TObject {

	friend class VMEMainFrame;
	friend class VMEServerPanel;
	friend class VMESis3302Panel;
	friend class VMESis3302SettingsPanel;
	friend class VMECaen785Panel;

	public:
		// global status bits
		enum EDGFStatusBit		{	kVMEVerboseMode 		= BIT(0),
									kVMEDebugMode			= BIT(1),
									kVMEOfflineMode			= BIT(3),
								};
		
		enum EDGFAccessBit		{	kVMEAccessDirectory 	= BIT(0),
									kVMEAccessRegular	 	= BIT(1),
									kVMEAccessRead	 		= BIT(2),
									kVMEAccessWrite 		= BIT(3)
								};

	public:
		VMEControlData();						// ctor

		inline const Char_t * NormalFont() { return(fNormalFont.Data()); };
		inline const Char_t * BoldFont() { return(fBoldFont.Data()); };
		inline const Char_t * SlantedFont() { return(fSlantedFont.Data()); };
		inline const Char_t * FixedFont() { return(fFixedFont.Data()); };
		
		Bool_t CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly = kFALSE);

		inline Bool_t IsOffline() { return((fStatus & kVMEOfflineMode) != 0); };
		inline Bool_t IsVerbose() { return((fStatus & VMEControlData::kVMEVerboseMode) != 0); };
		inline Bool_t IsDebug() { return((fStatus & VMEControlData::kVMEDebugMode) != 0); };
		
		inline Int_t GetFrameHeight() { return(fFrameHeight); };
		inline Int_t GetFrameWidth() { return(fFrameWidth); };

		Int_t MsgBox(TGWindow * Caller, const Char_t * Method, const Char_t * Title, const Char_t * Msg, EMsgBoxIcon Icon = kMBIconStop, Int_t Buttons = kMBDismiss);

		inline TMrbResource * Rootrc() { return(fRootrc); };
		inline TMrbResource * Vctrlrc() { return(fVctrlrc); };

		Bool_t SetupModuleList(TMrbLofNamedX & LofModules, const Char_t * ClassName = NULL);

		virtual ~VMEControlData() {};

	protected:
		TList fHeap;					//!

		UInt_t fStatus; 				// status bits

		TMrbResource * fRootrc; 		// environment (ROOT)
		TMrbResource * fVctrlrc; 		// ... (VMEControl)
		TString fRcFile;				// name of rc file (VMEControl)

		TString fNormalFont; 			// gui fonts
		TString fBoldFont;
		TString fSlantedFont;
		TString fFixedFont;
		
		Int_t fFrameHeight;
		Int_t fFrameWidth;

		ULong_t fColorBlack;			// gui colors
		ULong_t fColorWhite;
		ULong_t fColorGray;
		ULong_t fColorBlue;
		ULong_t fColorDarkBlue;
		ULong_t fColorGold;
		ULong_t fColorGreen;
		ULong_t fColorYellow;
		ULong_t fColorRed;

	ClassDef(VMEControlData, 0) 		// [VMEControl] Common data base
};

#endif
