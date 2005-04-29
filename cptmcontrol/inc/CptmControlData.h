#ifndef __CptmControlData_h__
#define __CptmControlData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          CptmControlData
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: CptmControlData.h,v 1.1 2005-04-29 11:35:22 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"
#include "TList.h"

#include "TGLayout.h"
#include "TGMrbLayout.h"

#include "TMrbNamedX.h"
#include "TMrbDGF.h"

// geometry settings
enum						{	kAutoWidth			= 1 		};
enum						{	kAutoHeight			= 1 		};

enum						{	kTabWidth 				= 1100					};
enum						{	kTabHeight 				= 400					};
enum						{	kVFrameWidth			= kTabWidth / 2 		};
enum						{	kVFrameHeight			= kTabHeight / 2 		};
enum						{	kLEWidth				= kAutoWidth			};
enum						{	kEntryWidth				= 100					};
enum						{	kFileEntryWidth			= 200					};
enum						{	kLEHeight				= 20					};
enum						{	kButtonWidth			= 400					};
enum						{	kButtonHeight			= 20					};

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst(x)

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData
// Purpose:        Common database to store any DGF data
// Description:    Common data base for all DGF modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class CptmControlData : public TNamed {

	friend class CptmPanel;

	public:
		// global status bits
		enum EDGFStatusBit {	kDGFVerboseMode 		= BIT(0),
								kDGFDebugMode			= BIT(1),
								kDGFModNumGlobal		= BIT(2),
								kDGFOfflineMode			= BIT(3),
								kDGFSimulStartStop		= BIT(4),
								kDGFSyncClocks		 	= BIT(5),
								kDGFIndivSwitchBusTerm	= BIT(6),
								kDGFUserPSA 			= BIT(7)
							};
		
		enum EDGFAccessBit	{	kDGFAccessDirectory 	= BIT(0),
								kDGFAccessRegular	 	= BIT(1),
								kDGFAccessRead	 		= BIT(2),
								kDGFAccessWrite 		= BIT(3)
							};

	public:
		CptmControlData();				// default ctor

		virtual ~CptmControlData() { 	// dtor
			fHeap.Delete();
		};

		CptmControlData(const CptmControlData & f) {};	// default copy ctor

		const Char_t * GetHost() { return(fCAMACHost.Data()); };	// get host name
		
		inline Bool_t IsOffline() { return((fStatus & kDGFOfflineMode) != 0); };
		inline Bool_t IsVerbose() { return((fStatus & CptmControlData::kDGFVerboseMode) != 0); };
		inline Bool_t IsDebug() { return((fStatus & CptmControlData::kDGFDebugMode) != 0); };
		
		inline const Char_t * NormalFont() { return(fNormalFont.Data()); };
		inline const Char_t * BoldFont() { return(fBoldFont.Data()); };
		inline const Char_t * SlantedFont() { return(fSlantedFont.Data()); };
		
		inline void SetLH(TGMrbLayout * Layout1, TGMrbLayout * Layout2, TGLayoutHints * Hints) {
			Layout1->SetLH(Hints);
			Layout2->SetLH(Hints);
		};
		
		const Char_t * GetResource(TString & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource);
		Int_t GetResource(Int_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource, Int_t Base = 10);
		Bool_t GetResource(Bool_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource);
	
		Bool_t CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly = kFALSE);

	protected:
		TList fHeap;								//!

		UInt_t fStatus; 							// status bits

		TString fCAMACHost; 						// default camac host
		Int_t fDefaultCrate;						// default crate number

		TString fCptmCodeFile;						// where to load CPTM code from
		TString fCptmSettingsPath; 					// ... (cptm)

		TString fNormalFont; 						// gui fonts
		TString fBoldFont;
		TString fSlantedFont;
		
		ULong_t fColorBlack;						// gui colors
		ULong_t fColorWhite;
		ULong_t fColorGray;
		ULong_t fColorBlue;
		ULong_t fColorDarkBlue;
		ULong_t fColorGold;
		ULong_t fColorGreen;
		ULong_t fColorYellow;
		ULong_t fColorRed;

	ClassDef(CptmControlData, 1) 		// [CptmControl] Common data base
};

#endif
