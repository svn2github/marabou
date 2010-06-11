#ifndef __TMrbSis_3820_h__
#define __TMrbSis_3820_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSis_3820.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSis_3820        -- 32 chn 32 bit VME scaler
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSis_3820.h,v 1.13 2010-06-11 08:34:42 Rudolf.Lutter Exp $
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

#include "TMrbVMEScaler.h"

class TMrbVMEChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3820
// Purpose:        Define a VME scaler type SIS 3820
// Description:    Defines a VME scaler SIS 3820 (list mode)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSis_3820 : public TMrbVMEScaler {

	public:
		enum				{	kSegSize		=	0x800000	};
		enum				{	kAddrMod		=	0x09		};

		enum EMrbOffsets	{	kOffsControlStatus	=	0x0
							};

		enum EMrbRegisters	{	kRegControlStatus,
							};

	public:

		TMrbSis_3820() {};  												// default ctor
		TMrbSis_3820(const Char_t * ModuleName, UInt_t BaseAddr, Int_t FifoDepth = 1); 	// define a new scaler
		~TMrbSis_3820() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sis_3820"); }; 	// module mnemonic

		inline void SetFifoDepth(Int_t FifoDepth) { fFifoDepth = FifoDepth; };
		inline Int_t GetFifoDepth() const { return(fFifoDepth); };

		inline void SetNonClearingMode(Bool_t Flag = kTRUE) { fNonClearingMode = Flag; };
		inline Bool_t NonClearingMode() { return(fNonClearingMode); };

		inline void SetDataFormat24(Bool_t Flag = kTRUE) { fDataFormat24 = Flag; };
		inline Bool_t DataFormat24() { return(fDataFormat24); };

		inline void EnableRefPulser(Bool_t Flag = kTRUE) { fEnableRefPulser = Flag; };
		inline Bool_t RefPulserIsOn() { return(fEnableRefPulser); };

		inline void SelectLNEChannel(Int_t Channel) { fLNEChannel = Channel; };
		inline Int_t GetLNEChannel() { return(fLNEChannel); };

		inline void SetExtension48(Bool_t Flag = kTRUE) { fExtension48 = Flag; };
		inline Bool_t Extension48() { return(fExtension48); };

		inline void XferOverflows(Bool_t Flag = kTRUE) { fXferOverflows = Flag; };
		inline Bool_t OverflowsToBeXferred() { return(fXferOverflows); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Int_t fFifoDepth;			// fifo depth per channel
		Int_t fLNEChannel;			// channel to act as LNE source
		Bool_t fNonClearingMode;	// kTRUE if non-clearing mode
		Bool_t fDataFormat24;		// kTRUE data format 24 bit + channel + user
		Bool_t fEnableRefPulser;	// kTRUE if reference pulser is to be used in ch0
		Bool_t fExtension48;		// kTRUE if chn0 & chn16 extended to 48 bits (data will be stored in chn#31)
		Bool_t fXferOverflows;		// kTRUE if overflows should be included into data (data will be stored in ch#30)
	ClassDef(TMrbSis_3820, 1)		// [Config] SIS 3820, 32 x 32 bit VME scaler
};

#endif
