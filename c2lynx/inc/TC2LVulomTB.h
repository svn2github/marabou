#ifndef __TC2LVulomTB_h__
#define __TC2LVulomTB_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB.h
// Purpose:        Connect to a SIS 3302 flash ADC
// Class:          TC2LVulomTB            -- base class
// Description:    Class definitions to establish a connection to a VME
//                 module running under LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TC2LVulomTB.h,v 1.3 2009-08-06 08:32:34 Rudolf.Lutter Exp $
// Date:           $Date: 2009-08-06 08:32:34 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TArrayI.h"
#include "TC2LVMEModule.h"
#include "M2L_CommonDefs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVulomTB
// Purpose:        Connect to a SIS 3302 ADC
// Description:    Defines a module and tries to connect to it
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TC2LVulomTB : public TC2LVMEModule {

	public:
		enum	{	kRcModuleSettings	=	1	};

	public:

		TC2LVulomTB() {};		// default ctor

		TC2LVulomTB(const Char_t * ModuleName, UInt_t Address = 0, Int_t NofChannels = 0, Bool_t Offline = kFALSE)
								: TC2LVMEModule(ModuleName, "VulomTB", Address, NofChannels, Offline) {};

		~TC2LVulomTB() {};							// default dtor

		Bool_t GetModuleInfo(Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion);

		Bool_t ReadScaler(TArrayI & ScalerValues, Int_t ScalerNo);
		Bool_t ReadChannel(TArrayI & ScalerValues, Int_t ChannelNo);

		Bool_t EnableChannel(Int_t ChannelNo);
		Bool_t DisableChannel(Int_t ChannelNo);

		Bool_t SetEnableMask(UInt_t & Bits);
		Bool_t GetEnableMask(UInt_t & Bits);

		Bool_t SetScaleDown(Int_t & ScaleDown, Int_t ChannelNo = kVulomTBAllChannels);
		Bool_t GetScaleDown(Int_t & ScaleDown, Int_t ChannelNo);
		Bool_t ReadScaleDown(TArrayI & ScaleDown);

		Bool_t ClearScaler(Int_t ChannelNo = kVulomTBAllChannels);

		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);
		Bool_t RestoreSettings(const Char_t * SettingsFile = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t ExecFunction(Int_t Fcode, TArrayI & DataSend, TArrayI & DataRecv, Int_t ChannelNo = kVulomTBAllChannels);

	ClassDef(TC2LVulomTB, 1)		// [Access to LynxOs] Connect to a Vulom triggerbox
};

#endif
