//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods to connect to VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TC2LVMEModule.cxx,v 1.4 2008-10-18 17:09:14 Marabou Exp $     
// Date:           $Date: 2008-10-18 17:09:14 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TEnv.h"
#include "TROOT.h"

#include "TMrbC2Lynx.h"
#include "TC2LVMEModule.h"

#include "TMrbLogger.h"
#include "SetColor.h"

#include "M2L_MessageTypes.h"

extern TMrbLogger * gMrbLog;

extern TMrbC2Lynx * gMrbC2Lynx;

ClassImp(TC2LVMEModule)

TC2LVMEModule::TC2LVMEModule(const Char_t * ModuleName, const Char_t * ModuleType,
								UInt_t Address, Int_t NofChannels, Bool_t Offline)
																: TMrbNamedX(0, ModuleName, ModuleType) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule
// Purpose:        Connect to a VME module
// Arguments:      Char_t * ModuleName    -- module name
//                 Char_t * ModuleType    -- type
//                 UInt_t Address         -- VME address
//                 Int_t NofChannels      -- number of channels used
//                 Bool_t Offline         -- kTRUE if in offline mode
// Results:        --
// Description:    Class constructor
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("c2lynx.log");

	fOffline = Offline;

	fLofFunctionTypes.AddNamedX(kMrbLofFunctionTypes);

	if (this->Connect(Address, NofChannels)) {
		gROOT->Append(this);
	} else {
		this->MakeZombie();
	}
}

Bool_t TC2LVMEModule::Connect(UInt_t Address, Int_t NofChannels) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule::Connect
// Purpose:        Connect to module via tcp socket
// Arguments:      UInt_t Address         -- VME address
//                 Int_t NofChannels      -- number of channels used
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Asks LynxOs server to connect to given module
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbC2Lynx == NULL && !this->IsOffline()) {
		gMrbLog->Err()	<< "No connection to LynxOs server" << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	if (this->GetHandle()) {
		gMrbLog->Err()	<< "Module already connected - " << this->GetName() << ", VME addr "
						<< setbase(16) << this->GetAddress() << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");\
		return(kFALSE);
	}

	if (this->IsOffline()) {
		this->SetHandle(0xAFFEC0C0);
	} else {
		M2L_VME_Connect c;
		gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &c, sizeof(M2L_VME_Connect), kM2L_MESS_VME_CONNECT);
		strcpy(c.fModuleName, this->GetName());
		strcpy(c.fModuleType, this->GetType());
		c.fBaseAddr = Address;
		c.fNofChannels = NofChannels;
		if (gMrbC2Lynx->Send((M2L_MsgHdr *) &c)) {
			M2L_VME_Return_Handle h;
			gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &h, sizeof(M2L_VME_Return_Handle), kM2L_MESS_VME_CONNECT);
			if (gMrbC2Lynx->Recv((M2L_MsgHdr *) &h)) {
				this->SetHandle(h.fHandle);
				gMrbC2Lynx->AddModule(this);
			}
		}
	}

	if (this->GetHandle() == 0) {
		gMrbLog->Err()	<< "Can't connect to module - " << this->GetName() << " (server reports error)" << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	this->SetAddress(Address);
	this->SetNofChannels(NofChannels);
	return(kTRUE);
}

Bool_t TC2LVMEModule::GetModuleInfo() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule::GetModuleInfo
// Purpose:        Get module info from server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Asks server for module info
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsOffline()) {
		gMrbLog->Out()	<< "[" << this->GetName() << "] type=" << this->GetTitle()
						<< " running on OFFLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleInfo");
		return(kTRUE);		
	} else {
		M2L_VME_Exec_Function x;
		gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &x, sizeof(M2L_VME_Exec_Function), kM2L_MESS_VME_EXEC_FUNCTION);
		x.fXhdr.fHandle = this->GetHandle();
		x.fXhdr.fCode = kM2L_FCT_GET_MODULE_INFO;
		if (gMrbC2Lynx->Send((M2L_MsgHdr *) &x)) {
			M2L_VME_Return_Module_Info r;
			gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &r, sizeof(M2L_VME_Return_Module_Info), kM2L_MESS_VME_EXEC_FUNCTION);
			if (gMrbC2Lynx->Recv((M2L_MsgHdr *) &r)) {
				gMrbLog->Out()	<< "[" << this->GetName() << "] type=" << this->GetTitle()
								<< " boardId=" << r.fBoardId;
				if (r.fSerial >= 0) gMrbLog->Out() << " serial=" << r.fSerial;
				gMrbLog->Out()	<< " version=" << r.fMajorVersion;
				if (r.fMinorVersion >= 0) gMrbLog->Out() << "." << r.fMinorVersion;
				gMrbLog->Out()	<< endl;
				gMrbLog->Flush(this->ClassName(), "GetModuleInfo");
				return(kTRUE);		
			} else {
				return(kFALSE);
			}
		} else {
			return(kFALSE);
		}
	}
}
