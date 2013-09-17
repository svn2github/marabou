#ifndef __SrvCaen785_h__
#define __SrvCaen785_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785.h
// Purpose:        MARaBOU to Lynx: VME modules
// Class:          SrvCaen785        -- adc
// Description:    Class definitions for M2L server
// Author:         R. Lutter
// Revision:       $Id: SrvCaen785.h,v 1.1 2010-10-04 10:43:26 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "SrvVMEModule.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785
// Purpose:        Define methods for a CAEN V785 adc
// Description:    Definitions for CAEN V785
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class SrvCaen785 : public SrvVMEModule {

	public:

		SrvCaen785();
		~SrvCaen785() {};

		Bool_t GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & Serial, Int_t & Revision, Bool_t PrintFlag = kTRUE);
		Bool_t TryAccess(SrvVMEModule * Module);
		M2L_MsgHdr * Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data);

	public:
		inline const Char_t * ClassName() const { return "SrvCaen785"; };

};	

#endif
