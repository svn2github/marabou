//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbCPTM.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbCPTM.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCPTM.cxx,v 1.1 2005-04-14 09:02:31 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "Rtypes.h"
#include "TRegexp.h"
#include "TFile.h"
#include "TDatime.h"
#include "TROOT.h"
#include "TObjString.h"

#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "TMrbCPTM.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

ClassImp(TMrbCPTM)

TMrbCPTM::TMrbCPTM(const Char_t * ModuleName, const Char_t * HostName, Int_t Crate, Int_t Station) : TNamed(ModuleName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM
// Purpose:        Define a module of type C_PTM
// Arguments:      Char_t * ModuleName     -- module name
//                 Char_t * HostName       -- host to be connected to
//                 Int_t Crate             -- crate number Cxx
//                 Int_t Station           -- camac station Nxx
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafCrate, Crate) || !cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafStation, Station)) {
		gMrbLog->Err() << "C_PTM in " << Crate << ".N" << Station << " - illegal camac address" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		TMrbString title = "C_PTM \"";					// title: C_PTM "xyz" in Cx.Ny
		title += this->GetName();
		title += "\" in C";
		title += Crate;
		title += ".N";
		title += Station;
		this->SetTitle(title.Data());

		fCamacHost = HostName;							// store host name
		fCrate = Crate; 								// store crate & station
		fStation = Station;

		this->Reset();
	}
}

TMrbCPTM::TMrbCPTM(const Char_t * ModuleName, const Char_t * HostName, const Char_t * CamacAddr) : TNamed(ModuleName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM
// Purpose:        Define a module of type DGF-4C
// Arguments:      Char_t * DGFName     -- module name
//                 Char_t * HostName    -- host to be connected to
//                 Char_t * CamacAddr   -- camac addr: Cxx.Nxx
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	TMrbString title;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!cnaf.Ascii2Int(CamacAddr)) {	// check crate & station
		gMrbLog->Err() << "DGF in " << CamacAddr << " - illegal camac address" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		TMrbString title = "DGF-4C \"";							// title: C_PTMC "xyz" in Cx.Ny
		title += this->GetName();
		title += "\" in ";
		title += cnaf.Int2Ascii();
		this->SetTitle(title.Data());

		fCamacHost = HostName;							// store host name
		fCrate = cnaf.GetC(); 	// store crate & station
		fStation = cnaf.GetN();

		this->Reset();
	}
}

Bool_t TMrbCPTM::Connect() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::Connect
// Purpose:        Connect to CAMAC
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Connect to CAMAC thru TMrbEsone.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kTRUE);
	}

	if (fCamac.ConnectToHost(fCamacHost) == 0) { 		// try to connect to camac via given host
		gMrbLog->Err() << this->GetTitle() << " - can't connect to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	return(kFALSE);
}

Bool_t TMrbCPTM::SetCamacHost(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetCamacHost
// Purpose:        Redefine camac host
// Arguments:      Char_t * HostName    -- host name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Redefines camac host. Method mainly used by DGFControl.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "SetCamacHost");
		return(kFALSE);
	}
	fCamacHost = HostName;
	return(kTRUE);
}

Bool_t TMrbCPTM::SetCrate(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetCrate
// Purpose:        Redefine camac crate
// Arguments:      Int_t Crate    -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (Re)define camac crate. Method used by DGFControl.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (this->IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "SetCrate");
		return(kFALSE);
	}

	if (!cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafCrate, Crate)) return(kFALSE);

	fCrate = Crate;
	return(kTRUE);
}

Bool_t TMrbCPTM::SetStation(Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetStation
// Purpose:        Redefine camac station
// Arguments:      Int_t Station    -- station number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (Re)define camac station. Method used by DGFControl.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "SetStation");
		return(kFALSE);
	}

	if (!cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafStation, Station)) return(kFALSE);

	fStation = Station;
	return(kTRUE);
}

Bool_t TMrbCPTM::ResetRead() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ResetRead
// Purpose:        Reset read
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets read pointer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("ResetRead")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A1.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ResetRead");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::ResetWrite() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ResetWrite
// Purpose:        Reset write
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets write pointer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("ResetWrite")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A2.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ResetWrite");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::Reset(Bool_t MemoryFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::Reset
// Purpose:        Reset
// Arguments:      Bool_t MemoryFlag  -- if kTRUE -> reset memory too
// Results:        --
// Exceptions:
// Description:    Resets C_PTM: Pointers and/or Memory.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("Reset")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(3), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A3.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}

	if (!MemoryFlag) return(kTRUE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(6), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A6.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::SetGeDGG(Int_t Delay, Int_t Width) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetGeDGG
// Purpose:        Set Ge DGG
// Arguments:      Int_t Delay    -- delay in 25ns ticks
//                 Int_t Width    -- width
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Ge DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetGeDGG")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(16), Delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetGeDGG");
		return(kFALSE);
	}
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(16), Width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A1.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetGeDGG");
		return(kFALSE);
	}
	return(kTRUE);
}

Int_t TMrbCPTM::GetGeDelay() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetGeDelay
// Purpose:        Read DGG Ge delay
// Arguments:      --
// Results:        Int_t Delay    -- delay in 25ns ticks
// Exceptions:
// Description:    Reads delay value of the Ge DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetGeDelay")) return(-1);
	Int_t delay;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(0), delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGeDelay");
		return(-1);
	}
	return(delay);
}

Int_t TMrbCPTM::GetGeWidth() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetGeWidth
// Purpose:        Read DGG Ge width
// Arguments:      --
// Results:        Int_t Delay    -- width in 25ns ticks
// Exceptions:
// Description:    Reads width value of the Ge DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetGeWidth")) return(-1);
	Int_t width;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(0), width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A1.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGeWidth");
		return(-1);
	}
	return(width);
}

Bool_t TMrbCPTM::SetAuxDGG(Int_t Delay, Int_t Width) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetAuxDGG
// Purpose:        Set Aux DGG
// Arguments:      Int_t Delay    -- delay in 25ns ticks
//                 Int_t Width    -- width
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Aux DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetAuxDGG")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(16), Delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A2.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetAuxDGG");
		return(kFALSE);
	}
	if (!fCamac.ExecCnaf(fCrate, fStation, A(3), F(16), Width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A3.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetAuxDGG");
		return(kFALSE);
	}
	return(kTRUE);
}

Int_t TMrbCPTM::GetAuxDelay() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetAuxDelay
// Purpose:        Read DGG Aux delay
// Arguments:      --
// Results:        Int_t Delay    -- delay in 25ns ticks
// Exceptions:
// Description:    Reads delay value of the Aux DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetAuxDelay")) return(-1);
	Int_t delay;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(0), delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A2.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetAuxDelay");
		return(-1);
	}
	return(delay);
}

Int_t TMrbCPTM::GetAuxWidth() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetAuxWidth
// Purpose:        Read DGG Aux width
// Arguments:      --
// Results:        Int_t Delay    -- width in 25ns ticks
// Exceptions:
// Description:    Reads width value of the Aux DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetAuxWidth")) return(-1);
	Int_t width;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(3), F(0), width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A3.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetAuxWidth");
		return(-1);
	}
	return(width);
}

Bool_t TMrbCPTM::SetMask(Int_t Mask) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetMask
// Purpose:        Set mask register
// Arguments:      UInt_t Mask   -- mask register
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetMask")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(5), F(16), Mask, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A5.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMask");
		return(kFALSE);
	}
	return(kTRUE);
}

Int_t TMrbCPTM::GetMask() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetMask
// Purpose:        Get mask register
// Arguments:      --
// Results:        Int_t Mask   -- mask register
// Exceptions:
// Description:    Reads the mask value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetMask")) return(-1);
	Int_t mask;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(5), F(0), mask, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A5.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMask");
		return(-1);
	}
	return(mask);
}

Bool_t TMrbCPTM::CheckConnect(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::CheckConnect
// Purpose:        Check if connected to CAMAC
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks connection to CAMAC, outputs error message if not connected.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) return(kTRUE);
	gMrbLog->Err() << this->GetTitle() << " - assigned to host " << fCamacHost << ", but NOT connected" << endl;
	gMrbLog->Flush(this->ClassName(), Method);
	return(kFALSE);
}

