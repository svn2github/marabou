//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbCPTM.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbCPTM.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCPTM.cxx,v 1.3 2005-04-18 10:02:50 rudi Exp $       
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

const SMrbNamedX kMrbCptmMaskBits[] = {
			{	BIT(0), "E2"	},
			{	BIT(1), "T2"	},
			{	BIT(2), "T1"	},
			{	BIT(3), "Q4"	},
			{	BIT(4), "Q3"	},
			{	BIT(5), "Q2"	},
			{	BIT(6), "Q1",	},
			{	BIT(7), "Ge"	}
		};

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

		this->Camac()->ConnectToHost(HostName);
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

		this->Camac()->ConnectToHost(HostName);
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
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A1.F8 failed" << endl;
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
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A2.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ResetWrite");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::ResetMemory() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ResetMemory
// Purpose:        Reset memory
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets memory contents
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("ResetMemory")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(6), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A6.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ResetMemory");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::ResetDacs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ResetDacs
// Purpose:        Reset dacs
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets dac contents
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("ResetDacs")) return(kFALSE);
	TArrayI dac(4);
	dac.Reset(0);
	return(this->WriteAllDacs(dac));
}

Bool_t TMrbCPTM::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::Reset
// Purpose:        Reset
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Reset all values to 0.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("Reset")) return(kFALSE);

	this->SetGeDGG(0, 0);			// Ge delay & gate generator
	this->SetAuxDGG(0, 0);			// Aux ...
	this->SetTimeWindowAux(0); 		// Aux time window
	this->SetMask(0);				// mask register

	this->ResetDacs();				// dac 0 .. 3

	this->ResetRead();				// read pointer
	this->ResetWrite(); 			// write ...
	this->ResetMemory();			// sram

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
	if (!this->CheckValue(Delay, 0xFF, "delay", "SetGeDGG")) return(kFALSE);
	if (!this->CheckValue(Width, 0xFF, "width", "SetGeDGG")) return(kFALSE);

	if (!this->SetGeDelay(Delay)) return(kFALSE);
	if (!this->SetGeWidth(Width)) return(kFALSE);
	return(kTRUE);
}

Bool_t TMrbCPTM::SetGeDelay(Int_t Delay) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetGeDelay
// Purpose:        Set Ge delay
// Arguments:      Int_t Delay    -- delay in 25ns ticks
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Ge delay.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetGeDelay")) return(kFALSE);
	if (!this->CheckValue(Delay, 0xFF, "delay", "SetGeDelay")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(16), Delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetGeDelay");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::SetGeWidth(Int_t Width) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetGeDelay
// Purpose:        Set Ge width
// Arguments:      Int_t Width    -- width in 25ns ticks
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Ge width.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetGeWidth")) return(kFALSE);
	if (!this->CheckValue(Width, 0xFF, "width", "SetGeWidth")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(16), Width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A1.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetGeWidth");
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
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F0 failed" << endl;
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
// Results:        Int_t Width    -- width in 25ns ticks
// Exceptions:
// Description:    Reads width value of the Ge DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetGeWidth")) return(-1);
	Int_t width;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(0), width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A1.F0 failed" << endl;
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
	if (!this->CheckValue(Delay, 0xFF, "delay", "SetAuxDGG")) return(kFALSE);
	if (!this->CheckValue(Width, 0xFF, "width", "SetAuxDGG")) return(kFALSE);

	if (!this->SetAuxDelay(Delay)) return(kFALSE);
	if (!this->SetAuxWidth(Width)) return(kFALSE);
	return(kTRUE);
}

Bool_t TMrbCPTM::SetAuxDelay(Int_t Delay) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetAuxDelay
// Purpose:        Set Aux delay
// Arguments:      Int_t Delay    -- delay in 25ns ticks
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Aux delay.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetAuxDelay")) return(kFALSE);
	if (!this->CheckValue(Delay, 0xFF, "delay", "SetAuxDelay")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(16), Delay, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A2.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetAuxDelay");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::SetAuxWidth(Int_t Width) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetAuxDelay
// Purpose:        Set Aux width
// Arguments:      Int_t Width    -- width in 25ns ticks
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Aux width.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetAuxWidth")) return(kFALSE);
	if (!this->CheckValue(Width, 0xFF, "width", "SetAuxWidth")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(3), F(16), Width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A3.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetAuxWidth");
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
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A2.F0 failed" << endl;
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
// Results:        Int_t Width    -- width in 25ns ticks
// Exceptions:
// Description:    Reads width value of the Aux DGG.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetAuxWidth")) return(-1);
	Int_t width;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(3), F(0), width, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A3.F0 failed" << endl;
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
// Arguments:      Int_t Mask   -- mask register
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetMask")) return(kFALSE);
	if (!this->CheckValue(Mask, 0xFF, "mask", "SetMask")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(5), F(16), Mask, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A5.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMask");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::SetMask(const Char_t * Mask) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetMask
// Purpose:        Set mask register
// Arguments:      Char_t * Mask   -- mask register
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetMask")) return(kFALSE);

	TMrbLofNamedX maskBits;
	maskBits.AddNamedX(kMrbCptmMaskBits);

	Int_t maskValue = 0;
	TMrbString mstr = Mask;
	TObjArray mbits;
	Int_t nb = mstr.Split(mbits, ":");
	for (Int_t i = 0; i < nb; i++) {
		TString mb = ((TObjString *) mbits[i])->GetString();
		TMrbNamedX * mbx = (TMrbNamedX *) maskBits.FindByName(mb.Data());
		if (mbx == NULL) {
			gMrbLog->Err()	<< "Illegal mask bit - " << mb << endl;
			gMrbLog->Flush(this->ClassName(), "SetMask");
			return(kFALSE);
		}
		maskValue |= mbx->GetIndex();
	}
	return(this->SetMask(maskValue));
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
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A5.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetMask");
		return(-1);
	}
	return(mask);
}

Bool_t TMrbCPTM::SetTimeWindowAux(Int_t Window) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetTimeWindowAux
// Purpose:        Set time window
// Arguments:      Int_t Window    -- window in 25ns ticks
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the Aux time window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetTimeWindowAux")) return(kFALSE);
	if (!this->CheckValue(Window, 0xFF, "window", "SetTimeWindowAux")) return(kFALSE);

	if (!fCamac.ExecCnaf(fCrate, fStation, A(4), F(16), Window, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A4.F16 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTimeWindowAux");
		return(kFALSE);
	}
	return(kTRUE);
}

Int_t TMrbCPTM::GetTimeWindowAux() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetTimeWindowAux
// Purpose:        Get time window
// Arguments:      --
// Results:        Int_t Window    -- window in 25ns ticks
// Exceptions:
// Description:    Reads the mask value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetTimeWindowAux")) return(-1);
	Int_t window;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(4), F(0), window, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A4.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTimeWindowAux");
		return(-1);
	}
	return(window);
}

Bool_t TMrbCPTM::EnableSynch() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::EnableSynch
// Purpose:        Enable synch
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables the busy-synch loop.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("EnableSynch")) return(kFALSE);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(4), F(8), kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A4.F8 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "EnableSynch");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::SetDac(Int_t DacNo, Int_t DacValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetDac
// Purpose:        Set dac value
// Arguments:      Int_t DacNo      -- dac number (0..3)
//                 Int_t DacValue   -- dac value to be set
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets dac value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SetDac")) return(kFALSE);
	if (!this->CheckValue(DacNo, 3, "dacNo", "SetDac")) return(kFALSE);
	if (!this->CheckValue(DacValue, 0xFF, "dacVal", "SetDac")) return(kFALSE);

	TArrayI dac(4);
	if (!this->ReadAllDacs(dac)) return(kFALSE);
	dac[DacNo] = DacValue;
	return(this->WriteAllDacs(dac));
}

Int_t TMrbCPTM::GetDac(Int_t DacNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::SetDac
// Purpose:        Get dac value
// Arguments:      Int_t DacNo      -- dac number (0..3)
// Results:        Int_t DacValue   -- dac value
// Exceptions:
// Description:    Reads dac value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetDac")) return(-1);
	if (!this->CheckValue(DacNo, 3, "dacNo", "GetDac")) return(-1);

	TArrayI dac(4);
	if (!this->ReadAllDacs(dac)) return(-1);
	return(dac[DacNo]);
}

Int_t TMrbCPTM::GetReadAddr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetReadAddr
// Purpose:        Get read pointer
// Arguments:      --
// Results:        Int_t Addr   -- SRAM addr
// Exceptions:
// Description:    Reads current value of read addr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetReadAddr")) return(-1);
	Int_t addr;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(14), F(0), addr, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A14.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetReadAddr");
		return(-1);
	}
	return(addr);
}

Int_t TMrbCPTM::GetWriteAddr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::GetWriteAddr
// Purpose:        Get write pointer
// Arguments:      --
// Results:        Int_t Addr   -- SRAM addr
// Exceptions:
// Description:    Reads current value of write addr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("GetWriteAddr")) return(-1);
	Int_t addr;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(13), F(0), addr, kTRUE)) { 		// exec cnaf, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A13.F0 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "GetWriteAddr");
		return(-1);
	}
	return(addr);
}

Bool_t TMrbCPTM::SaveSettings(const Char_t * SaveFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SaveSettings
// Purpose:        Save settings to file
// Arguments:      Char_t * SaveFile    -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Saves settings to file.
//                 Format is "unix style":
//                       <name>:<value>:<hexval>[:<trueval>:<unit>]
//                 Additional entries may be used to identify the module:
//                       FileType:par
//                       Module:<name>
//                       Crate:<crate>
//                       Station:<station>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("SaveSettings")) return(kFALSE);

	ofstream sf;
	TString saveFile;
	TString savePath;
	TDatime dt;

	saveFile = SaveFile;
	if (saveFile.Index("/") != 0 && saveFile.Index("./", 0) != 0) {
		savePath = gEnv->GetValue("TMrbCPTM.SettingsPath", "");
		if (savePath.IsNull()) savePath = gEnv->GetValue("TMrbDGF.SettingsPath", ".");
		saveFile = savePath + "/";
		saveFile += SaveFile;
	}
	if (saveFile.Index(".par") > 0) {			// write ascii data
		sf.open(saveFile, ios::out);
		if (!sf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << saveFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveSettings");
			return(kFALSE);
		}

		sf << "#----------------------------------------------------------------------------------------" << endl;
		sf << "# Name          : "								<< SaveFile << endl;
		sf << "# Contents      : Settings for C_PTM "			<< this->GetName()
																<< " in C" << this->GetCrate()
																<< ".N" << this->GetStation() << endl;
		sf << "# Creation date : " 								<< dt.AsString() << endl;
		sf << "# Created by    : " 								<< gSystem->Getenv("USER") << endl;
		sf << "#" << endl;
		sf << "# Data format   : <name>:<value>:<hexval>[:<trueval>:<unit>]" << endl;
		sf << "#----------------------------------------------------------------------------------------" << endl;
		sf << "#+FileType:par" << endl;
		sf << "#+Module:"										<< this->GetName() << endl;
		sf << "#+Crate:"										<< this->GetCrate() << endl;
		sf << "#+Station:" 										<< this->GetStation() << endl;
		sf << "#........................................................................................" << endl;
		sf << endl;
		Int_t val = this->GetGeDelay();
		sf << "DelayGe:" << val << ":0x" << setbase(16) << val << setbase(10);
		if (val) sf << ":" << (Double_t) (val * 25. / 1000.) << ":us"; else sf << "::";
		sf << endl;
		val = this->GetGeWidth();
		sf << "WidthGe:" << val << ":0x" << setbase(16) << val << setbase(10);
		if (val) sf << ":" << (Double_t) (val * 25. / 1000.) << ":us"; else sf << "::";
		sf << endl;
		val = this->GetAuxDelay();
		sf << "DelayAux:" << val << ":0x" << setbase(16) << val << setbase(10);
		if (val) sf << ":" << (Double_t) (val * 25. / 1000.) << ":us"; else sf << "::";
		sf << endl;
		val = this->GetAuxWidth();
		sf << "WidthAux:" << val << ":0x" << setbase(16) << val << setbase(10);
		if (val) sf << ":" << (Double_t) (val * 25. / 1000.) << ":us"; else sf << "::";
		sf << endl;
		val = this->GetTimeWindowAux();
		sf << "TimeWindowAux:" << val << ":0x" << setbase(16) << val << setbase(10);
		if (val) sf << ":" << (Double_t) (val * 25. / 1000.) << ":us"; else sf << "::";
		sf << endl;
		val = this->GetMask();
		sf << "Mask:" << val << ":0x" << setbase(16) << val << setbase(10) << "::" << endl;
		val = this->GetDac(0);
		sf << "Dac0:" << val << ":0x" << setbase(16) << val << setbase(10) << "::" << endl;
		val = this->GetDac(1);
		sf << "Dac1:" << val << ":0x" << setbase(16) << val << setbase(10) << "::" << endl;
		val = this->GetDac(2);
		sf << "Dac2:" << val << ":0x" << setbase(16) << val << setbase(10) << "::" << endl;
		val = this->GetDac(3);
		sf << "Dac3:" << val << ":0x" << setbase(16) << val << setbase(10) << "::" << endl;
 		sf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << SaveFile << " (should be .par)" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings");
		return(kFALSE);
	}

	return(kTRUE);
}


Bool_t TMrbCPTM::RestoreSettings(const Char_t * RestoreFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::RestoreSettings
// Purpose:        Read settings from file
// Arguments:      Char_t * RestoreFile    -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Restores settings from file.
//                 Format is "unix style":
//                       <name>:<value>:<hexval>[:<trueval>:<unit>]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("RestoreSettings")) return(kFALSE);

	ifstream rf;
	TString restFile;

	if (gSystem->AccessPathName(RestoreFile, kFileExists) == 0) {
		restFile = RestoreFile;
	} else {
		gMrbLog->Err() << "Settings file not found - " << RestoreFile << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreSettings");
		return(kFALSE);
	}

	if (restFile.Index(".par") > 0) {			// write ascii data
		rf.open(restFile, ios::out);
		if (!rf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << restFile << endl;
			gMrbLog->Flush(this->ClassName(), "RestoreSettings");
			return(kFALSE);
		}
		Int_t lineCnt = 0;
		TMrbString rLine;
		for (;;) {
			rLine.ReadLine(rf, kFALSE);
			lineCnt++;
			if (rf.eof()) break;
			rLine = rLine.Strip(TString::kBoth);
			if (rLine.Length() == 0) continue;						// empty line
			if (rLine(0) == '#') continue;							// comment
			TObjArray rFields;
			rFields.Delete();
			Int_t nFields = rLine.Split(rFields, ":");
			if (nFields < 3) {
				gMrbLog->Err() << restFile << " (line " << lineCnt << "): Wrong number of fields - " << nFields << endl;
				gMrbLog->Flush(this->ClassName(), "RestoreSettings");
				continue;
			}
			TString rName = ((TObjString *) rFields[0])->GetString();
			TMrbString rVal	= (((TObjString *) rFields[1])->GetString()).Data();
			Int_t val;
			rVal.ToInteger(val);
			if (rName.CompareTo("DelayGe") == 0)			this->SetGeDelay(val);
			else if (rName.CompareTo("WidthGe") == 0)		this->SetGeWidth(val);
			else if (rName.CompareTo("DelayAux") == 0)		this->SetAuxDelay(val);
			else if (rName.CompareTo("WidthAux") == 0)		this->SetAuxWidth(val);
			else if (rName.CompareTo("TimeWindowAux") == 0)	this->SetTimeWindowAux(val);
			else if (rName.CompareTo("Mask") == 0)			this->SetMask(val);
			else if (rName.CompareTo("Dac0") == 0)			this->SetDac(0, val);
			else if (rName.CompareTo("Dac1") == 0)			this->SetDac(1, val);
			else if (rName.CompareTo("Dac2") == 0)			this->SetDac(2, val);
			else if (rName.CompareTo("Dac3") == 0)			this->SetDac(3, val);
			else  {
				gMrbLog->Err() << restFile << " (line " << lineCnt << "): No such key word - " << rName << " (ignored)" << endl;
				gMrbLog->Flush(this->ClassName(), "RestoreSettings");
				continue;
			}
		}
		rf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << restFile << " (should be .par)" << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreSettings");
		return(kFALSE);
	}

	return(kTRUE);
}

Bool_t TMrbCPTM::DownloadAlteraCode(const Char_t * CodeFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::DownloadAlteraCode
// Purpose:        Download code
// Arguments:      Char_t * CodeFile    -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads binary data from file and loads down to altera chip
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("DownloadAlteraCode")) return(kFALSE);

	ifstream cf;
	TString codeFile;

	Bool_t ok = kTRUE;

	if (gSystem->AccessPathName(CodeFile, kFileExists) == 0) {
		codeFile = CodeFile;
	} else {
		gMrbLog->Err() << "Codefile not found - " << CodeFile << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
		return(kFALSE);
	}

	if (codeFile.Index(".rbf") > 0) {			// write ascii data
		cf.open(codeFile, ios::out);
		if (!cf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << codeFile << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			return(kFALSE);
		}
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(28), kTRUE)) { 		// enable program mode
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F28 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(13), kTRUE)) { 		// check status
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F13 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (fCamac.GetX() || fCamac.GetQ()) {
			gMrbLog->Err()	<< "Q/X error afer F28 (enter pgm mode): Q=" << (fCamac.GetQ() ? "1" : "0") << " X=" << (fCamac.GetX() ? "1" : "0") << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(30), kTRUE)) { 		// start download
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F30 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (!fCamac.GetX() || fCamac.GetQ()) {
			gMrbLog->Err()	<< "Q/X error afer F30 (start download): Q=" << (fCamac.GetQ() ? "1" : "0") << " X=" << (fCamac.GetX() ? "1" : "0") << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		Char_t byte;
		Int_t nofBytes = 0;
		TArrayI codeData;
		codeData.Set(kMrbCptmMaxCodeSize);
		codeData.Reset(0);
		for (;;) {
			cf.get(byte);
			if (cf.eof()) break;
			codeData[nofBytes++] = byte;
		}
		cf.close();
		if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), codeData, 0, nofBytes, kTRUE) == -1) {		// start block xfer, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F16 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (!fCamac.GetQ()) {
			gMrbLog->Err()	<< "Q error afer F16 (write code): Q=" << (fCamac.GetQ() ? "1" : "0") << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(29), kTRUE)) { 		// start download
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A0.F29 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadAlteraCode");
			ok = kFALSE;
		}
	}
	return(ok);
}

void TMrbCPTM::Print(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::Print
// Purpose:        Print current settings
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Printout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << endl << "C_PTM - CLock and Programmable Trigger Module:" << endl;
	Out << "----------------------------------------------" << endl;
	Out << "Name                 : " << this->GetName() << endl;
	Out << "Camac address        : C" << fCrate << ".N" << fStation << endl;
	Out << "DGG (Ge)       delay : " << (Double_t) (this->GetGeDelay() * 25.)/1000. << " us (" << this->GetGeDelay() << ")" << endl;
	Out << "               width : " << (Double_t) (this->GetGeWidth() * 25.)/1000. << " us (" << this->GetGeWidth() << ")" << endl;
	Out << "DGG (Aux)      delay : " << (Double_t) (this->GetAuxDelay() * 25.)/1000. << " us (" << this->GetAuxDelay() << ")" << endl;
	Out << "               width : " << (Double_t) (this->GetAuxWidth() * 25.)/1000. << " us (" << this->GetAuxWidth() << ")" << endl;
	Out << "Time window (Aux)    : " << (Double_t) (this->GetTimeWindowAux() * 25.)/1000. << " us (" << this->GetTimeWindowAux() << ")" << endl;
	Out << "DAC settings    dac0 : " << this->GetDac(0) << endl;
	Out << "                dac1 : " << this->GetDac(1) << endl;
	Out << "                dac2 : " << this->GetDac(2) << endl;
	Out << "                dac3 : " << this->GetDac(3) << endl;
	TString mStr;
	Out << "Mask register        : " << this->ConvertMask(mStr, this->GetMask()) << endl;
	Out << "SRAM pointers (read) : " << this->GetReadAddr() << endl;
	Out << "             (write) : " << this->GetWriteAddr() << endl << endl;
}

const Char_t * TMrbCPTM::ConvertMask(TString & Mask, Int_t MaskValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ConvertMask
// Purpose:        Decode mask bits
// Arguments:      TString & Mask    -- string to store mask bits in ascii
//                 Int_t MaskValue   -- mask bits
// Results:        --
// Exceptions:
// Description:    Converts mask from number to ascii.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString mstr;
	mstr.FromInteger(MaskValue, 0, ' ', 16);

	TMrbLofNamedX maskBits;
	maskBits.AddNamedX(kMrbCptmMaskBits);

	Bool_t first = kTRUE;
	for (Int_t i = 0; i <= 7; i++) {
		if (MaskValue & 1) {
			mstr += first ? " = " : " + ";
			first = kFALSE;
			mstr += ((TObjString *) maskBits[i])->GetString();
		}
		MaskValue >>= 1;
	}
	Mask = mstr;
	return(Mask.Data());
}	

Bool_t TMrbCPTM::CheckValue(Int_t Value, Int_t MaxValue, const Char_t * ArgName, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::CheckValue
// Purpose:        Check input value
// Arguments:      Int_t Value          -- input value to be checked
//                 Int_t MaxValue       -- max value allowed
//                 Char_t * ArgName     -- name of argument
//                 Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if a given value is allowed.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Value < 0 || Value > MaxValue) {
		gMrbLog->Err() << "Illegal input value: " << ArgName << "=" << Value << " - should be in [0, " << MaxValue << "]" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(kTRUE);
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

Bool_t TMrbCPTM::ReadAllDacs(TArrayI & Dac) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::ReadAllDacs
// Purpose:        Read dac values
// Arguments:      TArrayI & Dac    -- array to store dac values
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads DAC0 .. DAC3 (a 12 bits each) and stores values in array.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("ReadAllDacs")) return(kFALSE);
	Int_t j = 0;
	for (Int_t i = 8; i <= 11; i++, j++) {
		if (!fCamac.ExecCnaf(fCrate, fStation, A(i), F(0), Dac[j], kTRUE)) { 		// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A" << i << ".F0 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadAllDacs");
			return(kFALSE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbCPTM::WriteAllDacs(TArrayI & Dac) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM::WriteAllDacs
// Purpose:        Write dac values
// Arguments:      TArrayI & Dac    -- array containing dac values
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes DAC0 .. DAC3 (a 12 bits each).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnect("WriteAllDacs")) return(kFALSE);
	Long64_t dacBits = 0;
	for (Int_t i = 3; i >= 0; i--) {
		dacBits <<= 12;
		dacBits |= Dac[i];
	}
	TArrayI dac8(6);
	for (Int_t i = 0; i < 6; i++) {
		dac8[i] = dacBits & 0xFF;
		dacBits >>= 8;
	}
	Int_t j = 0;
	for (Int_t i = 8; i <= 13; i++, j++) {			// write 6 x 8 bits: F(0).A(8)...A(13)
		if (!fCamac.ExecCnaf(fCrate, fStation, A(i), F(16), dac8[j], kTRUE)) { 		// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": A" << i << ".F16 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadAllDacs");
			return(kFALSE);
		}
	}
	return(kTRUE);
}
