#ifndef __DGFControlData_h__
#define __DGFControlData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFControlData
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFControlData.h,v 1.16 2008-08-18 08:19:51 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TEnv.h"
#include "TString.h"
#include "TList.h"

#include "TGLayout.h"
#include "TGMrbLayout.h"

#include "HistPresent.h"

#include "TMrbResource.h"
#include "TMrbNamedX.h"
#include "TMrbDGF.h"

#include "DGFModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlData
// Purpose:        Common database to store any DGF data
// Description:    Common data base for all DGF modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFControlData : public TNamed {

	friend class DGFRunControlPanel;
	friend class DGFSetupPanel;
	friend class DGFSetFilesPanel;
	friend class DGFInstrumentPanel;
	friend class DGFParamsPanel;
	friend class DGFTauDisplayPanel;
	friend class DGFTraceDisplayPanel;
	friend class DGFUntrigTracePanel;
	friend class DGFOffsetsPanel;
	friend class DGFMcaDisplayPanel;
	friend class DGFTauFitPanel;
	friend class DGFEditModICSRPanel;
	friend class DGFEditChanCSRAPanel;
	friend class DGFEditUserPsaCSRPanel;
	friend class DGFEditRunTaskPanel;
	friend class DGFEditCoincPatternPanel;
	friend class DGFCopyModuleSettingsPanel;
	friend class DGFRestoreModuleSettingsPanel;
	friend class DGFSaveModuleSettingsPanel;
	friend class DGFMiscPanel;
	friend class DGFModule;
	friend class DGFCptmPanel;

	public:
		// global status bits
		enum EDGFStatusBit		{	kDGFVerboseMode 		= BIT(0),
									kDGFDebugMode			= BIT(1),
									kDGFModNumGlobal		= BIT(2),
									kDGFOfflineMode			= BIT(3),
									kDGFSimulStartStop		= BIT(4),
									kDGFSyncClocks		 	= BIT(5),
									kDGFIndivSwitchBusTerm	= BIT(6),
									kDGFUserPSA 			= BIT(7),
									kDGFEsoneSingleStepMode = BIT(8)
								};
		
		enum EDGFAccessBit		{	kDGFAccessDirectory 	= BIT(0),
									kDGFAccessRegular	 	= BIT(1),
									kDGFAccessRead	 		= BIT(2),
									kDGFAccessWrite 		= BIT(3)
								};

	public:
		DGFControlData(const Char_t * RcFile = ".DGFControl.rc");				// default ctor

		virtual ~DGFControlData() { 	// dtor
			fHeap.Delete();
			fLofModules.Delete();
		};

//		DGFControlData(const DGFControlData & f) {};	// default copy ctor

		Int_t SetupModuleList();									// create module list and key list
		inline Int_t GetNofModules() { return(fNofModules); };		// number of dgf modules
		inline Int_t GetNofClusters() { return(fNofClusters); };	// number of clusters
		TMrbLofNamedX * CopyKeyList(TMrbLofNamedX * KeyList, Int_t Cluster = 0, Int_t NofClusters = -1, Bool_t ArrayMode = kTRUE);
		TMrbLofNamedX * CopyKeyList(TMrbLofNamedX * KeyList, Int_t Cluster, UInt_t Pattern, Bool_t ClearFlag = kFALSE);

		inline DGFModule * FirstModule() { return((DGFModule *) fLofModules.First()); };		// return first
		inline DGFModule * NextModule(DGFModule * Last) { return((DGFModule *) fLofModules.After(Last)); }; // get next module
		Bool_t CheckIfStarted();								// kTRUE if at least 1 module is active

		const Char_t * GetHost() { return(fCAMACHost.Data()); };	// get host name
		
		DGFModule * GetModule(const Char_t * ModuleName);		// get module by name
		DGFModule * GetModule(Int_t GlobIndex);					// get module by its global index
		DGFModule * GetModule(Int_t Cluster, Int_t ModNo);		// get module by [cluster,modno]
		Int_t GetChannel(Int_t ChannelIndex);					// get channel by index
		Int_t GetChannelIndex(Int_t Channel);					// convert channel number to channel index

		inline UInt_t ModuleIndex(Int_t Cluster, Int_t ModNo) { return(0x10000 << Cluster | 0x1 << ModNo); };
		TMrbNamedX * GetKey(const Char_t * ModuleName);			// get module key
		TMrbNamedX * GetKey(const Char_t * ModuleName, Int_t & Cluster, Int_t & ModuleNumber);
		UInt_t GetIndex(const Char_t * ModuleName); 			// get module index
		UInt_t GetIndex(const Char_t * ModuleName, Int_t & Cluster, Int_t & ModuleNumber);
 
		inline UInt_t GetPatEnabled(Int_t Cluster) { return(fPatEnabled[Cluster]); };
		inline UInt_t GetPatInUse(Int_t Cluster) { return(fPatInUse[Cluster]); };
		inline void SetPatInUse(Int_t Cluster, UInt_t Pattern) { fPatInUse[Cluster] = Pattern & 0xFFFF; };

		inline DGFModule * GetSelectedModule() { return(this->GetModule(fSelectedModule)); };
		inline Int_t GetSelectedModuleIndex() { return(fSelectedModule); };
		inline void SetSelectedModuleIndex(Int_t ModuleIndex) { fSelectedModule = ModuleIndex; };
		void SetSelectedModule(const Char_t * ModuleName);
		inline void SetSelectedModule(DGFModule * Module) { this->SetSelectedModule(Module->GetName()); };

		inline Int_t GetSelectedChannel() { return(this->GetChannel(fSelectedChannel)); };
		inline Int_t GetSelectedChannelIndex() { return(fSelectedChannel); };
		inline void SetSelectedChannel(Int_t Channel) { fSelectedChannel = this->GetChannelIndex(Channel); };
		inline void SetSelectedChannelIndex(Int_t ChannelIndex) { fSelectedChannel = ChannelIndex; };

		Int_t ChannelIndex2Number(Int_t ChannelIndex);

		Bool_t ModuleInUse(DGFModule * Module); 				// check if module is in use
		Bool_t ModuleInUse(const Char_t * ModuleName);

		inline HistPresent * GetHistPresent() { return(fHistPresent); };
		
		inline Bool_t IsOffline() { return((fStatus & kDGFOfflineMode) != 0); };
		inline Bool_t IsVerbose() { return((fStatus & DGFControlData::kDGFVerboseMode) != 0); };
		inline Bool_t IsDebug() { return((fStatus & DGFControlData::kDGFDebugMode) != 0); };
		inline Bool_t IsSingleStep() { return((fStatus & DGFControlData::kDGFEsoneSingleStepMode) != 0); };
		
		inline const Char_t * NormalFont() { return(fNormalFont.Data()); };
		inline const Char_t * BoldFont() { return(fBoldFont.Data()); };
		inline const Char_t * SlantedFont() { return(fSlantedFont.Data()); };
		
		inline void SetLH(TGMrbLayout * Layout1, TGMrbLayout * Layout2, TGLayoutHints * Hints) {
			Layout1->SetLH(Hints);
			Layout2->SetLH(Hints);
		};
		
		Bool_t CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly = kFALSE);

		void UpdateParamsAndFPGAs();
		void AddToUpdateList(DGFModule * Module);

		inline TMrbResource * Rootrc() { return(fRootrc ? fRootrc : NULL); };
		inline TMrbResource * Dgfrc() { return(fDgfrc ? fDgfrc : NULL); };

	protected:
		TList fHeap;								//!

		UInt_t fStatus; 							// status bits

		Double_t fDeltaT;							// time granularity

		TString fCAMACHost; 						// default camac host
		Int_t fDefaultCrate;						// default crate number

		Int_t fNofClusters;							// number of clusters
		Int_t fNofModules;							// number of DGF modules
		TList fLofModules;							// list of modules

		TMrbLofNamedX fLofModuleKeys[kNofClusters];	//! ... key list
		UInt_t fPatEnabled[kNofClusters];			// pattern of modules enabled
		UInt_t fPatInUse[kNofClusters];				// pattern of modules in use
		TMrbLofNamedX fLofChannels;					//! channel numbers

		TMrbLofNamedX fLofClusters; 				// clusters: name, serial, color

		TObjArray fLofModulesToBeUpdated;			// modules to be updated

		Int_t fSelectedModule;						// index of module currently selected
		Int_t fSelectedChannel;						// index of channel currently selected

		Bool_t fSimulStartStop;						// kTRUE if to start/stop simultaneously
		Bool_t fSyncClocks;							// kTRUE if clocks have to be synchronized
		Bool_t fIndivSwitchBusTerm;					// kTRUE if switchbus should be terminated individually
		Bool_t fUserPSA;							// kTRUE if user PSA code should be used

		TString fLoadPath;						// where to load DSP/FPGA code from
		TString fDSPCodeFile;						// ... DSP code
		TString fDSPParamsFile;						// ... param table
		TString fUPSAParamsFile;					// ... extension for user psa
		TString fSystemFPGAConfigFile;				// ... FPGA config (system)
		TString fFippiFPGAConfigFile[TMrbDGFData::kNofRevs];		// ...             (fippi, rev D,E)
		TString fCptmCodeFile;						// where to load CPTM code from

		TString fDataPath;							// where to read user's setup files from
		TString fRunDataFile;						// ... run data

		TString fDgfSettingsPath; 					// param settings (dgf)
		TString fCptmSettingsPath; 					// ... (cptm)

		HistPresent * fHistPresent;					//! the famous histogram presenter
		
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
		TMrbResource * fDgfrc;

	ClassDef(DGFControlData, 1) 		// [DGFControl] Common data base
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlInfo
// Purpose:        Pass results from transient wdw to caller
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFControlInfo {

	public:
		UInt_t fActionBits; 			// selected actions
		UInt_t fChannels;				// selected channels
		UInt_t fModules[kNofClusters];	// selected modules
		Bool_t fExecute;				// execute
		Bool_t fAccessDSP;				// read from / write to DSP
		TString fPath;					// file path
};

#endif
