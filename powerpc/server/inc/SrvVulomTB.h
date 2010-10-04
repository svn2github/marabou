#ifndef __SrvVulomTB_h__
#define __SrvVulomTB_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvVulomTB.h
//! \brief			MARaBOU to Lynx: VME modules
//! \details		Class definitions for M2L server:<br>
//! 				Module Vulom3 (GSI), mode "trigger box"
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $
//! $Date: 2010-10-04 10:43:26 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrArrayI.h"
#include "SrvVMEModule.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"
#include "M2L_CommonDefs.h"

class SrvVulomTB : public SrvVMEModule {

	public:

		SrvVulomTB();
		~SrvVulomTB() {};

		//! try access to module
		Bool_t TryAccess(SrvVMEModule * Module);

		//! dispatch over function
		M2L_MsgHdr * Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data);

		//! get module info
		Bool_t GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion, Bool_t PrintFlag = kTRUE);

		//! read scaler contents (all channels)
		Bool_t ReadScaler(SrvVMEModule * Module, TArrayI & ScalerValues, Int_t ScalerNo);

		//! read contents of a single scaler channel
		Bool_t ReadChannel(SrvVMEModule * Module, TArrayI & ScalerValue, Int_t ChannelNo);

		//! enable a given channel
		Bool_t EnableChannel(SrvVMEModule * Module, Int_t ChannelNo);

		//! disable a given channel
		Bool_t DisableChannel(SrvVMEModule * Module, Int_t ChannelNo);

		//! enable/disable channel(s) via mask bits
		Bool_t SetEnableMask(SrvVMEModule * Module, Int_t & Bits);

		//! read enable pattern
		Bool_t GetEnableMask(SrvVMEModule * Module, Int_t & Bits);

		//! set scaledown value for a given channel
		Bool_t SetScaleDown(SrvVMEModule * Module, Int_t & ScaleDown, Int_t ChannelNo);

		//!  read scaledown value of a given channel
		Bool_t GetScaleDown(SrvVMEModule * Module, Int_t & ScaleDown, Int_t ChannelNo);

		//!  read scaledown values of all channels
		Bool_t ReadScaleDown(SrvVMEModule * Module, TArrayI & ScaleDown);

		//!  clear scaler for a given channel
		Bool_t ClearScaler(SrvVMEModule * Module, Int_t ChannelNo);

	protected:
		//! setup things before calling a function
		void SetupFunction(M2L_MsgData * Data, TArrayI & Array, Int_t & ScalerNo, Int_t & ChannelNo);

		//! finish function call, inform client
		M2L_MsgHdr * FinishFunction(TArrayI & Array);

	public:
		inline const Char_t * ClassName() const { return "SrvVulomTB"; };

};

#endif
