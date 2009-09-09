#ifndef __TMrbVulomTB_h__
#define __TMrbVulomTB_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVulomTB.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVulomTB        -- Vulom TriggerBox
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVulomTB.h,v 1.2 2009-09-09 14:36:03 Rudolf.Lutter Exp $
// Date:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB
// Purpose:        Define a VME Vulom TriggerBox
// Description:    Defines a VME Vulom TriggerBox
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVulomTB : public TMrbVMEModule {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x09	};
		enum				{	kNofChannels		=	16		};
		enum				{	kMaxScaleDown		=	15		};

		enum				{	kScSetInput			=	BIT(0)		};
		enum				{	kScSetInhibit		=	BIT(1)		};
		enum				{	kScSetScaleDown		=	BIT(2)		};
		enum				{	kScSetAll			=	kScSetInput | kScSetInhibit | kScSetScaleDown	};

	public:

		TMrbVulomTB() {};  												// default ctor
		TMrbVulomTB(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofScalerChans = kNofChannels, UInt_t ScalerSet = kScSetAll); 			// define a new trigbox
		~TMrbVulomTB() {};												// default dtor

		inline Int_t GetNofScalerChans() { return(fNofScalerChans); };

		virtual inline const Char_t * GetMnemonic() const { return("vulomTB"); }; 	// module mnemonic

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		inline void SetEnableMask(UInt_t EnableMask) { fEnableMask = EnableMask; };
		inline UInt_t GetEnableMask() { return(fEnableMask); };
		Bool_t SetScaleDown(Int_t ScaleDown, Int_t ChannelNo);
		Int_t GetScaleDown(Int_t ChannelNo);
		inline void SetRdoScaleDown(Int_t RdoScd) { fRdoScaleDown = RdoScd; };
		inline Int_t GetRdoScaleDown() { return(fRdoScaleDown); };

inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fNofScalerChans;
		Int_t fNofScalerSets;
		UInt_t fScalerSetMask;
		UInt_t fEnableMask;
		TArrayI fScaleDown;
		Int_t fRdoScaleDown;

	ClassDef(TMrbVulomTB, 1)		// [Config] VulomTB, triggerBox GSI
};

#endif
