//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbLofDGFs.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for lists of DGF modules
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TEnv.h"
#include "TRegexp.h"
#include "TFile.h"
#include "TDatime.h"
#include "TROOT.h"

#include "TMrbLogger.h"

#include "TMrbDGFData.h"
#include "TMrbDGF.h"
#include "TMrbDGFCommon.h"

#include "TMrbLofDGFs.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;
extern TMrbDGFData * gMrbDGFData;

ClassImp(TMrbLofDGFs)

Bool_t TMrbLofDGFs::AddModule(TMrbDGF * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::AddModule
// Purpose:        Add a DGF module to list
// Arguments:      TMrbDGF * Module   -- module to be added
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a DGF module to this list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t stationBit;
	
	if (this->FindObject(Module->GetName())) {
		gMrbLog->Err()	<< "Module " << Module->GetName() << " (C" << Module->GetCrate() << ".N" << Module->GetStation()
						<< ") - already in list" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModule");
		return(kFALSE);
	}
	
	stationBit = 1 << (Module->GetStation() - 1);
	if (fStationMask & stationBit) {
		gMrbLog->Err()	<< "Module " << Module->GetName() << " (C" << Module->GetCrate() << ".N" << Module->GetStation()
						<< ") - station N already occupied" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModule");
		return(kFALSE);
	}
	
	if (fCamac == NULL) {
		fCamac = Module->Camac();
		fCrate = Module->GetCrate();
		fDGFData = Module->Data();
	}
		
	if (fCrate != Module->GetCrate()) {
		gMrbLog->Err()	<< "Module " << Module->GetName() << " (C" << Module->GetCrate() << ".N" << Module->GetStation()
						<< ") - different crate" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModule");
		return(kFALSE);
	}
	
	fStationMask |= stationBit;
	this->Add(Module);
	return(kTRUE);
}

Bool_t TMrbLofDGFs::RemoveModule(TMrbDGF * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::RemoveModule
// Purpose:        Remove a DGF module from list
// Arguments:      TMrbDGF * Module   -- module to be added
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes a DGF module from this list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t stationBit;
	
	if (this->FindObject(Module->GetName())) {
		gMrbLog->Err()	<< "Module " << Module->GetName() << " (C" << Module->GetCrate() << ".N" << Module->GetStation()
						<< ") - already in list" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModule");
		return(kFALSE);
	}
	
	stationBit = 1 << (Module->GetStation() - 1);
	if (fStationMask & stationBit) {
		gMrbLog->Err()	<< "Module " << Module->GetName() << " (C" << Module->GetCrate() << ".N" << Module->GetStation()
						<< ") - station N already occupied" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModule");
		return(kFALSE);
	}
	
	fStationMask |= stationBit;
	this->Add(Module);
	return(kTRUE);
}

Bool_t TMrbLofDGFs::DownloadFPGACode(TMrbDGFData::EMrbFPGAType FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::DownloadFPGACode
// Purpose:        Download FPGA data
// Arguments:      EMrbFPGAType FPGAType   -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loads code down to FPGA.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Bool_t ok;
			
	UInt_t sts;
	Int_t size;
	UShort_t * dp;

	TString sysfip;
	UInt_t csrBits;
	Int_t subAddr;
	TString cnaf;

	UInt_t sysBits, fippiDbits, fippiEbits;

	if (!this->CheckModules("DownLoadFPGACode")) return(kFALSE);
	if (!this->CheckConnect("DownLoadFPGACode")) return(kFALSE);

	fBCN = fCamac->HasBroadCast();
	if (fBCN == -1) {								// no broadcast available - use normal loop
		gMrbLog->Err()	<< "[" << sysfip
						<< " FPGA] No BROADCAST mode available - looping over list members instead ..." << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
		ok = kTRUE;
		dgf = (TMrbDGF *) this->First();
		while (dgf) {
			if (!dgf->DownloadFPGACode(FPGAType)) ok = kFALSE;
			dgf = (TMrbDGF *) this->After(dgf);
		}
		return(ok);
	}
	
	sysBits = 0;
	fippiDbits = 0;
	fippiEbits = 0;

	if (FPGAType == TMrbDGFData::kSystemFPGA) {
		if (!fDGFData->FPGACodeRead(TMrbDGFData::kSystemFPGA)) {
			gMrbLog->Err() << "[System FPGA] No FPGA data" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
		csrBits = TMrbDGFData::kSystemFPGAReset;								// address system fpga
		subAddr = A(10);														// exec A10.F17
		cnaf = ".A10.F17";
		sysfip = "System";
		sysBits = fStationMask;
	} else {
		dgf = (TMrbDGF *) this->First();
		while (dgf) {
			if (dgf->GetRevision()->GetIndex() == TMrbDGFData::kRevD)	fippiDbits |= (1 << (dgf->GetStation() - 1));
			else														fippiEbits |= (1 << (dgf->GetStation() - 1));	
			dgf = (TMrbDGF *) this->After(dgf);
		}
		if (fippiDbits && !fDGFData->FPGACodeRead(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevD)) {
			gMrbLog->Err() << "[Fippi(D) FPGA] No FPGA data" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
		if (fippiEbits && !fDGFData->FPGACodeRead(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevE)) {
			gMrbLog->Err() << "[Fippi(E) FPGA] No FPGA data" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
		csrBits = TMrbDGFData::kFippiFPGAReset; 							// address fippi fpga
		subAddr = A(9);														// exec A9.F17
		cnaf = ".A9.F17";
		sysfip = "Fippi";
	}

	fCamac->SetBroadCast(fCrate, fStationMask); 							// set broadcast register
	if (!this->WriteICSR(csrBits)) return(kFALSE);							// reset FPGA
	this->Wait();															// wait for reset to complete

	if (sysBits != 0) {
		size = fDGFData->GetFPGACodeSize(TMrbDGFData::kSystemFPGA); 			// size of FPGA data
		dp = (UShort_t *) fDGFData->GetFPGACodeAddr(TMrbDGFData::kSystemFPGA);	// addr of FPGA data
		fCamac->SetData(dp, size);												// copy FPGA data to camac buffer
		if (gMrbDGFData->IsVerbose()) {
			gMrbLog->Out()	<< "[System FPGA] Setting broadcast mask for crate C" << fCrate << ": 0x"
							<< setbase(16) << sysBits << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
		}
		fCamac->SetBroadCast(fCrate, sysBits); 									// set broadcast register
		if (fCamac->BlockXfer(fCrate, fBCN, subAddr, F(17), size, kTRUE) == -1) {		// start block xfer, 16 bit
			gMrbLog->Err()	<< "[System FPGA] "
							<< "C" << fCrate << ".N" << fBCN
							<< cnaf << " (pattern=0x" << setbase(16) << sysBits << setbase(10)
							<< "): Block xfer failed - ActionCount=-1" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
	}
	if (fippiDbits != 0) {
		size = fDGFData->GetFPGACodeSize(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevD); 	// size of FPGA data
		dp = (UShort_t *) fDGFData->GetFPGACodeAddr(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevD);	// addr of FPGA data
		fCamac->SetData(dp, size);												// copy FPGA data to camac buffer
		if (gMrbDGFData->IsVerbose()) {
			gMrbLog->Out()	<< "[Fippi(D) FPGA] Setting broadcast mask for crate C" << fCrate << ": 0x"
							<< setbase(16) << fippiDbits << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
		}
		fCamac->SetBroadCast(fCrate, fippiDbits); 									// set broadcast register
		if (fCamac->BlockXfer(fCrate, fBCN, subAddr, F(17), size, kTRUE) == -1) {		// start block xfer, 16 bit
			gMrbLog->Err()	<< "[Fippi(D) FPGA] "
							<< "C" << fCrate << ".N" << fBCN
							<< cnaf << " (pattern=0x" << setbase(16) << fippiDbits << setbase(10)
							<< "): Block xfer failed - ActionCount=-1" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
	}
	if (fippiEbits != 0) {
		size = fDGFData->GetFPGACodeSize(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevE); 	// size of FPGA data
		dp = (UShort_t *) fDGFData->GetFPGACodeAddr(TMrbDGFData::kFippiFPGA, TMrbDGFData::kRevE);	// addr of FPGA data
		fCamac->SetData(dp, size);												// copy FPGA data to camac buffer
		if (gMrbDGFData->IsVerbose()) {
			gMrbLog->Out()	<< "[Fippi(E) FPGA] Setting broadcast mask for crate C" << fCrate << ": 0x"
							<< setbase(16) << fippiEbits << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
		}
		fCamac->SetBroadCast(fCrate, fippiEbits); 									// set broadcast register
		if (fCamac->BlockXfer(fCrate, fBCN, subAddr, F(17), size, kTRUE) == -1) {		// start block xfer, 16 bit
			gMrbLog->Err()	<< "[Fippi(E) FPGA] "
							<< "C" << fCrate << ".N" << fBCN
							<< cnaf << " (pattern=0x" << setbase(16) << fippiEbits << setbase(10)
							<< "): Block xfer failed - ActionCount=-1" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
	}

	this->Wait();															// wait for download to complete

	dgf = (TMrbDGF *) this->First();										// read status
	ok = kTRUE;
	while (dgf) {
		sts = dgf->ReadICSR() & csrBits;
		if (sts != 0) { 											// error?
			gMrbLog->Err()	<< "[" << sysfip << " FPGA] "
							<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
							<< ": Data xfer failed - error 0x" << setbase(16) << sts << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			ok = kFALSE;
		}
		sts = dgf->GetStatus();
		if (FPGAType == TMrbDGFData::kSystemFPGA)	sts |= TMrbDGF::kSystemFPGACodeLoaded;
		else										sts |= TMrbDGF::kFippiFPGACodeLoaded;
		dgf->SetStatus(sts);
		dgf = (TMrbDGF *) this->After(dgf);
	}

	if (gMrbDGFData->IsVerbose()) {
		if (ok) {
			gMrbLog->Out()	<< "[" << sysfip << " FPGA] " << "C" << fCrate << ".N" << fBCN
							<< ": Code successfully loaded via broadcast" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
		} else {
			gMrbLog->Err()	<< "[" << sysfip << " FPGA] " << "C" << fCrate << ".N" << fBCN
							<< ": error(s) during FPGA download via broadcast" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
		}
	}
	return(ok);
}

Bool_t TMrbLofDGFs::DownloadFPGACode(const Char_t * FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::DownloadFPGACode
// Purpose:        Download FPGA data
// Arguments:      Char_t * FPGAType   -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loads code down to FPGA.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = gMrbDGFData->fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
		return(kFALSE);
	}
	return(this->DownloadFPGACode((TMrbDGFData::EMrbFPGAType) sysfip->GetIndex()));
}

Bool_t TMrbLofDGFs::DownloadDSPCode(Int_t Retry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::DownloadDSPCode
// Purpose:        Download DSP data
// Arguments:      Int_t Retry  -- number of retries on error
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loads code down to DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Bool_t ok;
			
	Int_t size;
	UShort_t * dp;
	UInt_t sts;
	
	if (!this->CheckModules("DownLoadDSPCode")) return(kFALSE);
	if (!this->CheckConnect("DownLoadDSPCode")) return(kFALSE);

	fBCN = fCamac->HasBroadCast();
	if (fBCN == -1) {									// no broadcast available - use normal loop
		ok = kTRUE;
		dgf = (TMrbDGF *) this->First();
		while (dgf) {
			if (!dgf->DownloadDSPCode()) ok = kFALSE;
			dgf = (TMrbDGF *) this->After(dgf);
		}
		return(ok);
	}
	
	if (gMrbDGFData->IsVerbose()) {
		gMrbLog->Out()	<< "Setting broadcast mask for crate C" << fCrate << ": 0x"
						<< setbase(16) << fStationMask << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadDSPCode", setblue);
	}
	fCamac->SetBroadCast(fCrate, fStationMask); 				// set broadcast register
	
	if (fDGFData->DSPCodeRead()) {
		if (!this->WriteCSR(TMrbDGFData::kDSPReset)) return(kFALSE);
		this->Wait();													// wait for reset to complete
		
		size = fDGFData->GetDSPCodeSize();									// size of DSP data
		dp = (UShort_t *) fDGFData->GetDSPCodeAddr();	 					// addr of DSP data
		if (!this->WriteTSAR(1)) return(kFALSE); 							// start with addr 1
		fCamac->SetData(dp + 2, size - 2);									// copy DSP[2], DSP[3] ... to camac buffer
		if (fCamac->BlockXfer(fCrate, fBCN, A(0), F(16), size - 2, kTRUE) == -1) {	// start block xfer, 16 bit
			gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
							<< ".A0.F16 failed - DSPAddr=1, wc=" << size - 2 << ", ActionCount=-1" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
			return(kFALSE);
		}

		if (!this->WriteTSAR(0)) return(kFALSE); 							// writing to addr 0 will start DSP pgm
		fCamac->SetData(dp, 2);												// copy DSP[0] & DSP[1] to camac buffer
		if (fCamac->BlockXfer(fCrate, fBCN, A(0), F(16), 2, kTRUE) == -1) {	// start block xfer
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fBCN
							<< ".A0.F16 failed - DSPAddr=0, wc=2, ActionCount=-1" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
			return(kFALSE);
		}

		this->Wait();														// wait to settle down

		dgf = (TMrbDGF *) this->First();									// test if download ok
		ok = kTRUE;
		while (dgf) {
			if (dgf->ReadCSR() & TMrbDGFData::kDSPError) { 					// error?
				if (!dgf->DownloadDSPCode(Retry, kTRUE)) ok = kFALSE;
			}
			sts = dgf->GetStatus();
			sts |= TMrbDGF::kDSPCodeLoaded;
			dgf->SetStatus(sts);
			dgf = (TMrbDGF *) this->After(dgf);
		}
				
		if (gMrbDGFData->IsVerbose()) {
			if (ok) {
				gMrbLog->Out()	<< "C" << fCrate << ".N" << fBCN
								<< ": DSP code successfully loaded via broadcast (" << size << " words)" << endl;
				gMrbLog->Flush(this->ClassName(), "DownloadDSPCode", setblue);
			} else {
				gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
								<< ": error(s) during DSP download via broadcast" << endl;
				gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
			}
		}
		return(ok);
	} else {
		gMrbLog->Err() << "No DSP data" << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
		return(kFALSE);
	}
}

Bool_t TMrbLofDGFs::FPGACodeLoaded(TMrbDGFData::EMrbFPGAType FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::FGPACodeLoaded
// Purpose:        Test if FPGA code is loaded
// Arguments:      EMrbFPGAType FPGAType    -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been downloaded to all modules.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;

	UInt_t bit = (FPGAType == TMrbDGFData::kSystemFPGA) ?
								TMrbDGF::kSystemFPGACodeLoaded :
								TMrbDGF::kFippiFPGACodeLoaded;

	dgf = (TMrbDGF *) this->First();
	while (dgf) {	
		if ((dgf->GetStatus() & bit) == 0) return(kFALSE);
		dgf = (TMrbDGF *) this->After(dgf);
	}
	return(kTRUE);
}

Bool_t TMrbLofDGFs::FPGACodeLoaded(const Char_t * FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FGPACodeLoaded
// Purpose:        Test if FPGA code is loaded
// Arguments:      Char_t * FPGAType    -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been downloaded to all modules.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sysfip;

	sysfip = gMrbDGFData->fLofFPGATypes.FindByName(FPGAType, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (sysfip == NULL) {
		gMrbLog->Err() << "Illegal FPGA type - " << FPGAType << endl;
		gMrbLog->Flush(this->ClassName(), "FPGACodeLoaded");
		return(kFALSE);
	}
	return(this->FPGACodeLoaded((TMrbDGFData::EMrbFPGAType) sysfip->GetIndex()));
}

Bool_t TMrbLofDGFs::DSPCodeLoaded() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::DSPCodeLoaded
// Purpose:        Test if DSP code is loaded
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if DSP code has been downloaded to all modules.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;

	dgf = (TMrbDGF *) this->First();
	while (dgf) {	
		if ((dgf->GetStatus() & TMrbDGF::kDSPCodeLoaded) == 0) return(kFALSE);
		dgf = (TMrbDGF *) this->After(dgf);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WriteICSR
//                 TMrbDGF::WriteCSR
//                 TMrbDGF::WriteTSAR
//                 TMrbDGF::WriteWCR
// Purpose:        Camac output to DGF regs
// Arguments:      UInt_t Data   -- data to be written to DGF reg
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbLofDGFs::WriteICSR(UInt_t Data) {
// Write DGF Control Status Reg
	if (fCamac->ExecCnaf(fCrate, fBCN, A(8), F(17), Data, kTRUE) == -1) {
		gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
						<< ".A8.F17 failed - ActionCount=-1" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteICSR");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbLofDGFs::WriteCSR(UInt_t Data) {
// Write DGF Control Status Reg
	if (fCamac->ExecCnaf(fCrate, fBCN, A(0), F(17), Data, kTRUE) == -1) {
		gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
						<< ".A0.F17 failed - ActionCount=-1" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteCSR");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbLofDGFs::WriteTSAR(UInt_t Addr) {
// Write DGF Transfer Start Addr Reg
	if (fCamac->ExecCnaf(fCrate, fBCN, A(1), F(17), Addr, kTRUE) == -1) {
		gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
						<< ".A1.F17 failed - DSPAddr=0x" << setbase(16) << Addr
						<< setbase(10) << ", ActionCount=-1" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTSAR");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbLofDGFs::WriteWCR(Int_t Data) {
// Write DGF Word Count Reg
	if (fCamac->ExecCnaf(fCrate, fBCN, A(2), F(17), Data, kTRUE) == -1) {
		gMrbLog->Err()	<< "C" << fCrate << ".N" << fBCN
						<< ".A2.F17 failed - ActionCount=-1" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteWCR");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbLofDGFs::CheckConnect(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::CheckConnect
// Purpose:        Check if connected to CAMAC
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks connection to CAMAC, outputs error message if not connected.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Bool_t ok;
		
	dgf = (TMrbDGF *) this->First();
	ok = kTRUE;
	while (dgf) {
		if (!dgf->IsConnected()) {
			gMrbLog->Err() << dgf->GetTitle() << " - assigned to host " << dgf->GetCamacHost() << ", but NOT connected" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
			ok = kFALSE;
		}
		dgf = (TMrbDGF *) this->After(dgf);
	}
	return(ok);
}

Bool_t TMrbLofDGFs::CheckModules(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofDGFs::CheckModules
// Purpose:        Check if any modules in list
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if there is at least 1 module in list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fStationMask == 0) {
		gMrbLog->Err() << "List of DGF modules is empty" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(kTRUE);
}



