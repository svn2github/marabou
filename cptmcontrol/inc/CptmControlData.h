#ifndef __CptmControlData_h__
#define __CptmControlData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          CptmControlData
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: CptmControlData.h,v 1.2 2008-08-18 08:19:51 Rudolf.Lutter Exp $       
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
#include "TMrbResource.h"

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
		
		Bool_t CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly = kFALSE);

		inline TMrbResource * Rootrc() { return(fRootrc ? fRootrc : NULL); };
		inline TMrbResource * Cptmrc() { return(fCptmrc ? fCptmrc : NULL); };

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

		TMrbResource * fRootrc;						// environment
		TMrbResource * fCptmrc;

	ClassDef(CptmControlData, 1) 		// [CptmControl] Common data base
};

#endif
