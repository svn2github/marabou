//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           Caen785.cxx
// Purpose:        MARaBOU to Lynx: VME modules
// Description:    Implements class methods for Caen785 adc
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: SrvCaen785.cxx,v 1.2 2010-10-04 11:23:30 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

#ifdef CPU_TYPE_RIO2
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif

#include "SrvCaen785.h"
#include "SrvCaen785_Layout.h"
#include "SrvUtils.h"
#include "SetColor.h"

extern TMrbLofNamedX * gLofVMEProtos;		// list of prototypes
extern TMrbLofNamedX * gLofVMEModules;		// list of actual modules

extern Bool_t gSignalTrap;

SrvCaen785::SrvCaen785() : SrvVMEModule(	"Caen785",						// prototype
											"ADC 32ch 12bit",				// description
											0x39,							// address modifier: A32
											0x10000,  						// segment size
											32,								// number of channels
											1 << 12) {						// range
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785
// Purpose:        Create a Caen785 prototype
// Arguments:      --
// Description:    Prototype Caen785
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetID(SrvVMEModule::kModuleCaen785);
}

Bool_t SrvCaen785::TryAccess(SrvVMEModule * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785::TryAccess()
// Purpose:        Try to access the module
// Arguments:      SrvVMEModule * Module  -- module address
// Results:        kTRUE/kFALSE
// Description:    Access one of the registers to be sure the module is present
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t boardId, serial, revision;
	if (!this->GetModuleInfo(Module, boardId, serial, revision, kTRUE)) return(kFALSE);
	if (boardId != 785) {
		cerr << setred << this->ClassName() << "::TryAccess(): [" << Module->GetName()
						<< "]: Wrong module type - not a " << this->GetName() << setblack << endl;
		return(kFALSE);
	}
	return(kTRUE);
}

M2L_MsgHdr * SrvCaen785::Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785::Dispatch()
// Purpose:        Dispatch over function
// Arguments:      SrvVMEModule * Module  -- module address
//                 TMrbNamedX * Function  -- function
//                 M2L_MsgData * Data     -- data
// Results:        M2L_MsgHdr * Hdr       -- ptr to message
// Description:    Execute function
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Function->GetIndex()) {
		case kM2L_FCT_GET_MODULE_INFO:
			{
				Int_t BoardId;
				Int_t Serial;
				Int_t Revision;
				if (!this->GetModuleInfo(Module, BoardId, Serial, Revision, kFALSE)) return(NULL);
				M2L_VME_Return_Module_Info * m = (M2L_VME_Return_Module_Info *) allocMessage(sizeof(M2L_VME_Return_Module_Info), 1, kM2L_MESS_VME_EXEC_FUNCTION);
				m->fBoardId = swapIt(BoardId);
				m->fSerial = swapIt(Serial);
				m->fMajorVersion = swapIt(Revision);
				m->fMinorVersion = -1;
				return((M2L_MsgHdr *) m);
			}
	}
	cerr << setred << this->ClassName() << "::TryAccess(): [" << Module->GetName() << "]: Function not implemented - "
					<< Function->GetName() << " (" << setbase(16) << Function->GetIndex() << ")" << setblack << endl;
	return(NULL);
}

Bool_t SrvCaen785::GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & Serial, Int_t & Revision, Bool_t PrintFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvCaen785::GetModuleInfo()
// Purpose:        Get module info
// Arguments:      SrvVMEModule * Module  -- module address
//                 Int_t BoardId          -- id "785"
//                 Int_t Serial           -- serial number
//                 Int_t Revision         -- rev number
//                 Bool_t PrintFlag       -- output to gMrbLog if kTRUE
// Results:        kTRUE/kFALSE
// Description:    Reads (and prints) module info
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	volatile UShort_t * firmWare = (volatile UShort_t *) Module->MapAddress(CAEN_V785_R_ROM);
	if (firmWare == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	UShort_t boardIdMSB = *(firmWare + 0x3A / sizeof(UShort_t)) & 0xFF;
	if (this->CheckBusTrap(Module)) return(kFALSE);
	UShort_t boardIdLSB = *(firmWare + 0x3E / sizeof(UShort_t)) & 0xFF;
	BoardId = (int) (boardIdMSB << 8) + (int) boardIdLSB;
	Revision = (Int_t) *(firmWare + 0x4E / sizeof(UShort_t)) & 0xFF;
	UShort_t serialLSB = *(firmWare + 0xF06 / sizeof(UShort_t)) & 0xFF;
	UShort_t serialMSB = *(firmWare + 0xF02 / sizeof(UShort_t)) & 0xFF;
	Serial = (Int_t) (serialMSB << 8) + (Int_t) serialLSB;
	if (PrintFlag) {
		cout << this->ClassName() << "::GetModuleInfo(): " << setbase(16)
						<< "CAEN module info: addr (phys) " << Module->GetPhysAddr()
						<< " addr (log) " << Module->GetBaseAddr()
						<< " mod " << Module->GetAddrModifier()
						<< setbase(10)
						<< " type " << BoardId
						<< " serial " << Serial
						<< " revision " << Revision << endl;
	}
	return(kTRUE);
}
