//__________________________________________________[C++ CLASS IMPLEMENTATION]
///////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/src/TMrbDGF.cxx
// Purpose:        Methods to operate a module XIA DGF-4C
// Description:    Implements class methods for module XIA DGF-4C
// Header files:   TMrbDGF.h          -- class defs
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbDGF.cxx,v 1.32 2005-05-02 08:45:43 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TRegexp.h"
#include "TFile.h"
#include "TDatime.h"
#include "TROOT.h"
#include "TObjString.h"

#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "TMrbDGFData.h"
#include "TMrbDGF.h"
#include "TMrbDGFCommon.h"
#include "TMrbDGFEvent.h"
#include "TMrbDGFEventBuffer.h"
#include "TMrbDGFHistogramBuffer.h"

#include "SetColor.h"

const SMrbNamedXShort kMrbTriggerPolarity[] =
							{
								{1, 			"positive" 		},
								{-1, 			"negative" 		},
								{0, 			NULL			}
							};

const SMrbNamedXShort kMrbAccuTimeScale[] =
							{
								{1,				"secs" 			},
								{60,	 		"mins"			},
								{3600,	 		"hours"			},
								{0, 			NULL			}
							};

const SMrbNamedXShort kMrbSwitchBusModes[] =
							{
								{TMrbDGFData::kSwitchBusNormal,				"Normal" 	},
								{TMrbDGFData::kTerminateDSP,				"DSP-Terminate" 	},
								{TMrbDGFData::kTerminateFast,				"Fast-Terminate"	},
								{0, 										NULL				}
							};

const SMrbNamedXShort kMrbPSANames[] =
							{
								{0, 	"Baseline"		},
								{1, 	"CutOff01"		},
								{2, 	"CutOff23"		},
								{3, 	"T0Thresh01"	},
								{4, 	"T0Thresh23"	},
								{5, 	"T90Thresh" 	},
								{6, 	"PSACh0"		},
								{7, 	"PSACh1"		},
								{8, 	"PSACh2"		},
								{9, 	"PSACh3"		},
								{10,	"PSALength01"	},
								{11,	"PSALength23"	},
								{12,	"PSAOffset01"	},
								{13,	"PSAOffset23"	},
								{14,	"TFACutOff0"	},
								{15,	"TFACutOff1"	},
								{0, 	NULL			}
							};

TMrbDGFData * gMrbDGFData = NULL;				// common data base for all DGF modules
extern TMrbLogger * gMrbLog;

ClassImp(TMrbDGF)

TMrbDGF::TMrbDGF(	const Char_t * DGFName,
					const Char_t * HostName, Int_t Crate, Int_t Station,
					Bool_t Connect,
					Bool_t Offline,
					Bool_t LocalData) : TNamed(DGFName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF
// Purpose:        Define a module of type DGF-4C
// Arguments:      Char_t * DGFName     -- module name
//                 Char_t * HostName    -- host to be connected to
//                 Int_t Crate          -- crate number Cxx
//                 Int_t Station        -- camac station Nxx
//                 Bool_t Connect       -- kTRUE: connect to CAMAC
//                 Bool_t Offline       -- kTRUE: don't access hardware
//                 Bool_t LocalData     -- kTRUE: TMrbDGFData will be held locally
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	TMrbString title;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fStatusM = 0;				// setup not done, not connected
	if (Offline) fStatusM = TMrbDGF::kOffline;
	fCamac.SetOffline(Offline);	
	
	if (!cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafCrate, Crate) || !cnaf.CheckCnaf(TMrbEsoneCnaf::kCnafStation, Station)) {
		gMrbLog->Err() << "DGF in " << Crate << ".N" << Station << " - illegal camac address" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (Connect && fCamac.ConnectToHost(HostName, kTRUE) == 0) { 	// try to (re)connect to camac via given host
		gMrbLog->Err() << "DGF in C" << Crate << ".N" << Station << " - can't connect to host " << HostName << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		title = "DGF-4C \"";							// title: DGF-4C "xyz" in Cx.Ny
		title += this->GetName();
		title += "\" in C";
		title += Crate;
		title += ".N";
		title += Station;
		this->SetTitle(title.Data());

		fStatusM |= TMrbDGF::kSetupOk;					// setup ok
		if (Connect) fStatusM |= TMrbDGF::kConnected;	// connected to camac
		fCamacHost = HostName;							// store host name
		fCrate = Crate; 								// store crate & station
		fStation = Station;

		fTauDistr = NULL;								// tau distribution
		fGaussian = NULL;								// gaussian fit

		this->SetClusterID();
		this->SetRevision();							// module revision unknown

		SetupDGF(LocalData);							// initialize data base
	}
}

TMrbDGF::TMrbDGF(	const Char_t * DGFName,
					const Char_t * HostName,
					const Char_t * CamacAddr,
					Bool_t Connect,
					Bool_t Offline,
					Bool_t LocalData) : TNamed(DGFName, "Xia DGF-4C") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF
// Purpose:        Define a module of type DGF-4C
// Arguments:      Char_t * DGFName     -- module name
//                 Char_t * HostName    -- host to be connected to
//                 Char_t * CamacAddr   -- camac addr: Cxx.Nxx
//                 Bool_t Connect       -- kTRUE: connect to CAMAC
//                 Bool_t Offline       -- kTRUE: don't access hardware
//                 Bool_t LocalData     -- kTRUE: TMrbDGFData will be held locally
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	TMrbString title;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fStatusM = 0;			// setup not done, not connected
	if (Offline) fStatusM = TMrbDGF::kOffline;
	fCamac.SetOffline(Offline);	

	if (!cnaf.Ascii2Int(CamacAddr)) {	// check crate & station
		gMrbLog->Err() << "DGF in " << CamacAddr << " - illegal camac address" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (Connect && fCamac.ConnectToHost(HostName) == 0) { 		// try to connect to camac via given host
		gMrbLog->Err() << "DGF in " << CamacAddr
				<< " - can't connect to host " << HostName << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		title = "DGF-4C \"";							// title: DGF-4C "xyz" in Cx.Ny
		title += this->GetName();
		title += "\" in ";
		title += cnaf.Int2Ascii();
		this->SetTitle(title.Data());

		fStatusM |= TMrbDGF::kSetupOk;					// setup ok
		if (Connect) fStatusM |= TMrbDGF::kConnected;	// connected to camac
		fCamacHost = HostName;							// store host name
		fCrate = cnaf.GetC(); 	// store crate & station
		fStation = cnaf.GetN();

		fTauDistr = NULL;								// tau distribution
		fGaussian = NULL;								// gaussian fit

		this->SetClusterID();
		this->SetRevision();							// module revision unknown

		SetupDGF(LocalData);							// initialize data base
	}
}

Bool_t TMrbDGF::Connect() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::Connect
// Purpose:        Connect to CAMAC
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Connect to CAMAC thru TMrbEsone.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!IsOffline()) {
		gMrbLog->Err() << this->GetTitle() << " is OFFLINE - can't connect to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	if (!SetupOk()) {
		gMrbLog->Err() << this->GetTitle() << " - something went wrong during setup " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	if (IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kTRUE);
	}
	if (fCamac.ConnectToHost(fCamacHost) == 0) { 		// try to connect to camac via given host
		gMrbLog->Err() << this->GetTitle() << " - can't connect to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}
	fStatusM |= TMrbDGF::kConnected;
	return(kTRUE);
}

Bool_t TMrbDGF::SetCamacHost(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetCamacHost
// Purpose:        Redefine camac host
// Arguments:      Char_t * HostName    -- host name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Redefines camac host. Method mainly used by DGFControl.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (IsConnected()) {
		gMrbLog->Err() << this->GetTitle() << " - already connected to host " << fCamacHost << endl;
		gMrbLog->Flush(this->ClassName(), "SetCamacHost");
		return(kFALSE);
	}
	fCamacHost = HostName;
	return(kTRUE);
}

Bool_t TMrbDGF::SetCrate(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetCrate
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

Bool_t TMrbDGF::SetStation(Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetStation
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

void TMrbDGF::SetupDGF(Bool_t LocalData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetupDGF
// Purpose:        Setup data base
// Arguments:      Bool_t LocalData     -- kTRUE: TMrbDGFData will be held locally
// Results:        --
// Exceptions:
// Description:    Initializes DGF.
//                 Code for DSP and FPGA normally resides in a
//                 TMrbDGFData object common to all DGF modules.
//                 Setting LocalData = kTRUE allows to use private
//                 code for this DGF. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (LocalData) {
		fDGFData = new TMrbDGFData();
	} else {
		if (gMrbDGFData == NULL) gMrbDGFData = new TMrbDGFData();
		fDGFData = gMrbDGFData;
	}

	if (!this->Data()->CheckXiaRelease()) {
		this->MakeZombie();
		return;
	}
	
	gMrbDGFData->fVerboseMode = gEnv->GetValue("TMrbDGF.VerboseMode", kFALSE);

	fLofDGFStatusMBits.SetName("Soft Status (Module)");
	fLofDGFStatusMBits.AddNamedX(kMrbDGFStatusMBits);
	fLofDGFStatusMBits.SetPatternMode();

	fLofDGFSwitchBusModes.SetName("Switch Bus Modes");
	fLofDGFSwitchBusModes.AddNamedX(kMrbSwitchBusModes);
	fLofDGFSwitchBusModes.SetPatternMode();

	this->ResetDGF();

	if (LocalData) fStatusM |= TMrbDGF::kLocalData;
}

void TMrbDGF::ResetDGF() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ResetDGF
// Purpose:        Reset to initial values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets DGF.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fParams.Set(TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams);			// clear param section
}

void TMrbDGF::ClearRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ClearRegisters
// Purpose:        Initialize main registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Initializes registers: MODCSRA, CHANCSRA, COINPATTERN
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetModCSRA(0, TMrbDGF::kBitSet, kTRUE);
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) this->SetChanCSRA(chn, 0, TMrbDGF::kBitSet, kTRUE);
	this->SetParValue("COINCPATTERN", 0, kTRUE);
}

Bool_t TMrbDGF::DownloadFPGACode(TMrbDGFData::EMrbFPGAType FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::DownloadFPGACode
// Purpose:        Download FPGA data
// Arguments:      EMrbFPGAType FPGAType   -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loads code down to FPGA.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t sts;
	Int_t size = 0;
	UShort_t * dp = NULL;
	TMrbDGFData::EMrbDGFRevision rev;

	TString sysfip;
	UInt_t csrBits = 0;
	Int_t subAddr = 0;
	TString cnaf;

	TArrayI cData;

	Bool_t dataOk;
	if (!this->CheckConnect("DownLoadFPGACode")) return(kFALSE);

	dataOk = kFALSE;
	if (FPGAType == TMrbDGFData::kSystemFPGA) {
		if (fDGFData->FPGACodeRead(TMrbDGFData::kSystemFPGA)) {
			csrBits = TMrbDGFData::kSystemFPGAReset;								// address system fpga
			size = fDGFData->GetFPGACodeSize(TMrbDGFData::kSystemFPGA); 			// size of FPGA data
			dp = (UShort_t *) fDGFData->GetFPGACodeAddr(TMrbDGFData::kSystemFPGA);	// addr of FPGA data
			subAddr = A(10);														// exec A10.F17
			cnaf = ".A10.F17";
			sysfip = "System";
			dataOk = kTRUE;
		}
	} else {
		rev = (TMrbDGFData::EMrbDGFRevision) this->GetRevision()->GetIndex();
		if (fDGFData->FPGACodeRead(TMrbDGFData::kFippiFPGA, rev)) {
			csrBits = TMrbDGFData::kFippiFPGAReset; 									// address fippi fpga
			size = fDGFData->GetFPGACodeSize(TMrbDGFData::kFippiFPGA, rev);  			// size of FPGA data
			dp = (UShort_t *) fDGFData->GetFPGACodeAddr(TMrbDGFData::kFippiFPGA, rev);	// addr of FPGA data
			subAddr = A(9);																// exec A9.F17
			cnaf = ".A9.F17";
			sysfip = (rev == TMrbDGFData::kRevD) ? "Fippi(D)" : "Fippi(E)";
			dataOk = kTRUE;
		}
	}

	if (dataOk) {
		if (!this->WriteICSR(csrBits)) {										// reset FPGA
			gMrbLog->Err()	<< "[" << sysfip << " FPGA] "
							<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Reset failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
		this->Wait();															// wait for reset to complete
		this->CopyData(cData, dp, size);										// copy FPGA data to camac buffer
		if (fCamac.BlockXfer(fCrate, fStation, subAddr, F(17), cData, 0, size, kTRUE) == -1) {		// start block xfer, 16 bit
			gMrbLog->Err()	<< "[" << sysfip << " FPGA] "
							<< fName << " in C" << fCrate << ".N" << fStation
							<< cnaf << ": Block xfer failed" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}
		this->Wait();															// wait for download to complete
		sts = this->ReadICSR() & csrBits;								// read status
		if (sts != 0) { 														// error?
			gMrbLog->Err()	<< "[" << sysfip << " FPGA] "
							<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Data xfer failed - error 0x" << setbase(16) << sts << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
			return(kFALSE);
		}

		if (FPGAType == TMrbDGFData::kSystemFPGA) {
			fStatusM |= TMrbDGF::kSystemFPGACodeLoaded;
			if (gMrbDGFData->fVerboseMode) {
				gMrbLog->Out()	<< "[System FPGA] " << fName << " in C" << fCrate << ".N" << fStation
								<< ": Code successfully loaded (" << size << " bytes)" << endl;
				gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
			}
		} else {
			fStatusM |= TMrbDGF::kFippiFPGACodeLoaded;
			if (gMrbDGFData->fVerboseMode) {
				gMrbLog->Out()	<< "[Fippi FPGA] " << fName << " in C" << fCrate << ".N" << fStation
								<< ": Code (Rev" << fRevision.GetName() << ") successfully loaded ("
								<< size << " bytes)" << endl;
				gMrbLog->Flush(this->ClassName(), "DownloadFPGACode", setblue);
			}
		}

		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << sysfip << "FPGA] No FPGA data" << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadFPGACode");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::DownloadFPGACode(const Char_t * FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::DownloadFPGACode
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

Bool_t TMrbDGF::SetSwitchBusDefault(Bool_t IndiFlag, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetSwitchBusDefault
// Purpose:        Set switch bus register to default value
// Arguments:      Bool_t IndiFlag            -- kTRUE if switch bus to be set individually
//                 Char_t * Prefix            -- resource prefix
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets switchbus bits:
//                      IndiFlag = kTRUE      inspect TMrbDGF.<name>.SwitchBusTerm
//                      IndiFlag = kFALSE     terminate if module doesn't contain a core
//                                            (segment id like "B:ssss")
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t switchBus;
	TString resource, segmentID;
	Bool_t terminate;

	if (!this->CheckConnect("SetSwitchBusDefault")) return(kFALSE);

	if (IndiFlag) {
		if (Prefix == NULL || *Prefix == '\0') Prefix = "TMrbDGF";
		resource = this->GetName();
		resource(0,1).ToUpper();
		resource.Prepend(".Module.");
		resource.Prepend(Prefix);
		resource += ".SwitchBusTerm";
		terminate = gEnv->GetValue(resource.Data(), kFALSE);
	} else {
		segmentID = this->GetClusterID()->GetTitle();
		terminate = (segmentID.Index("c", 0) >= 0);
	}

	if (terminate)	switchBus = TMrbDGFData::kTerminateDSP | TMrbDGFData::kTerminateFast;
	else 			switchBus = 0;
	
	switchBus &= TMrbDGFData::kSwitchBus;
	this->SetSwitchBus(switchBus, TMrbDGF::kBitSet);
	return(kTRUE);
}

Bool_t TMrbDGF::SetSwitchBus(UInt_t Bits, TMrbDGF::EMrbBitOp BitOp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetSwitchBus
// Purpose:        Set switch bus register
// Arguments:      UInt_t Bits       -- bits
//                 EMrbBitOp BitOp   -- set, or, clear ...
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t icsrBits;

	if (!this->CheckConnect("SetSwitchBus")) return(kFALSE);

	switch (BitOp) {
		case TMrbDGF::kBitSet:
			icsrBits = Bits & TMrbDGFData::kSwitchBus;
			break;
		case TMrbDGF::kBitOr:
			icsrBits = this->ReadICSR();
			icsrBits |= Bits;
			break;
		case TMrbDGF::kBitClear:  
			icsrBits = this->ReadICSR();
			icsrBits &= ~Bits;
			break;
		case TMrbDGF::kBitMask:
			icsrBits = this->ReadICSR();
			icsrBits &= Bits;
			break;
		default:
			gMrbLog->Err() << "Illegal bit operator - " << BitOp << endl;
			gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
			return(kFALSE);
	}
	
	icsrBits &= TMrbDGFData::kSwitchBus;
	this->WriteICSR(icsrBits);
	if (gMrbDGFData->fVerboseMode) {
		cout	<< setblue << this->ClassName() << "::SetSwitchBus(): [" << this->GetName()
				<< "] Setting switch bus to 0x" << setbase(16) << icsrBits << setbase(10)
				<< setblack << endl;
	}
	return(kTRUE);
}

Bool_t TMrbDGF::SetSwitchBus(const Char_t * DSPTrigger, const Char_t * FastTrigger) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetSwitchBus
// Purpose:        Set switch bus register
// Arguments:      Char_t * DSPTrigger   -- DSP trigger settings
//                 Char_t * FastTrigger  -- fast trigger settings
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t icsrBits;
	TString trig;
	TMrbNamedX *swbm;

	trig = "DSP-"; trig += DSPTrigger;

	swbm = fLofDGFSwitchBusModes.FindByName(trig.Data(), TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (swbm == NULL) {
		gMrbLog->Err() << "Illegal DSP trigger mode - " << DSPTrigger << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
		return(kFALSE);
	}
	icsrBits = swbm->GetIndex();

	if (FastTrigger == NULL) FastTrigger = DSPTrigger;
	trig = "Fast-"; trig += FastTrigger;

	swbm = fLofDGFSwitchBusModes.FindByName(trig.Data(), TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (swbm == NULL) {
		gMrbLog->Err() << "Illegal fast trigger mode - " << FastTrigger << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
		return(kFALSE);
	}
	icsrBits |= swbm->GetIndex();

	icsrBits &= TMrbDGFData::kSwitchBus;
	this->WriteICSR(icsrBits);
	cout	<< setblue << this->ClassName() << "::SetSwitchBus(): [" << this->GetName()
			<< "] Setting switch bus to 0x" << setbase(16) << icsrBits << setbase(10)
			<< setblack << endl;
	return(kTRUE);
}

UInt_t TMrbDGF::GetSwitchBus() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetSwitchBus
// Purpose:        Read switch bus register
// Arguments:      --
// Results:        UInt_t SwitchBus    -- switch bus bits
// Exceptions:
// Description:    Reads ICSR (switchbus) bits
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t switchBus = this->ReadICSR() & TMrbDGFData::kSwitchBus;
	return(switchBus);
}

Bool_t TMrbDGF::ActivateUserPSACode(Bool_t Activate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ActivateUserPSACode
// Purpose:        Activate user PSA
// Arguments:      Bool_t Activate            -- kTRUE/kFALSE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Activates user PSA code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csrBit;
	if (Activate) {
		csrBit = this->GetParValue("MODCSRB");
		csrBit |= 1;
		this->SetParValue("MODCSRB", csrBit);
		for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			csrBit = this->GetParValue(chn, "CHANCSRB");
			csrBit |= 1;
			this->SetParValue(chn, "CHANCSRB", csrBit);
		}
		if (gMrbDGFData->fVerboseMode) {
			cout	<< fName << " in C" << fCrate << ".N" << fStation
					<< ": User PSA code activated" << endl;
		}
	} else {
		csrBit = this->GetParValue("MODCSRB");
		csrBit &= ~1;
		this->SetParValue("MODCSRB", csrBit);
		for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			csrBit = this->GetParValue(chn, "CHANCSRB");
			csrBit &= ~1;
			this->SetParValue(chn, "CHANCSRB", csrBit);
		}
		if (gMrbDGFData->fVerboseMode) {
			cout	<< fName << " in C" << fCrate << ".N" << fStation
					<< ": User PSA code deactivated" << endl;
		}
	}
	return(kTRUE);
}

Bool_t TMrbDGF::DownloadDSPCode(Int_t Retry, Bool_t TriedOnce) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::DownloadDSPCode
// Purpose:        Download DSP data
// Arguments:      Int_t Retry       -- number of retries on error
//                 Bool_t TriedOnce  -- kTRUE if already tried once
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loads code down to DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t size = 0;
	Bool_t downloadFailed;
	Int_t retriesNeeded;
	TArrayI cData;

	if (!this->CheckConnect("DownLoadDSPCode")) return(kFALSE);

	downloadFailed = kFALSE;
	retriesNeeded = 0;

	if (fDGFData->DSPCodeRead()) {
		for (Int_t retry = 0; retry < Retry; retry++) {
			if (!this->WriteCSR(TMrbDGFData::kDSPReset)) downloadFailed = kTRUE; 		// reset DSP
			this->Wait();																// wait for reset to complete

			size = fDGFData->fDSPSize;											// size of DSP data
			this->CopyData(cData, fDGFData->fDSPCode.GetArray(), size); 		// copy DSP data to camac buffer
			if (!downloadFailed) {
				if (this->WriteTSAR(1)) {											// start with addr 1
					if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), cData, 2, size - 2, kTRUE) == -1) {	// start block xfer, 16 bit
						gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
										<< ".A0.F16 failed - DSPAddr=1, wc=" << size - 2 << endl;
						gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
						downloadFailed = kTRUE;
					}
				} else {
					downloadFailed = kTRUE;
				}
			}

			if (!downloadFailed && this->WriteTSAR(0)) { 							// writing to addr 0 will start DSP pgm
				if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), cData, 0, 2, kTRUE) == -1) {	// start block xfer
					gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
									<< ".A0.F16 failed - DSPAddr=0, wc=2" << endl;
					gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
					downloadFailed = kTRUE;
				}
			}

			this->Wait();														// wait to settle down

			if (this->ReadCSR() & TMrbDGFData::kDSPError) downloadFailed = kTRUE;

			if (!downloadFailed) {
				retriesNeeded = retry;
				if (TriedOnce) retriesNeeded++;
				break;
			}
		}
		
		if (downloadFailed) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ": Downloading DSP failed (" << Retry << " retries)" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
			this->WriteCSR(TMrbDGFData::kDSPReset);
			return(kFALSE);
		} else if (retriesNeeded > 0) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ": Downloading DSP ok after " << retriesNeeded << " retries" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadDSPCode", setblue);
		}

		fStatusM |= TMrbDGF::kDSPCodeLoaded;

		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": DSP code successfully loaded (" << size << " words)" << endl;
			gMrbLog->Flush(this->ClassName(), "DownloadDSPCode", setblue);
			return(kTRUE);
		}
	} else {
		gMrbLog->Err() << "No DSP data" << endl;
		gMrbLog->Flush(this->ClassName(), "DownloadDSPCode");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbDGF::UploadDSPCode(TArrayS & DSPCode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::UploadDSPCode
// Purpose:        Load DSP data back to memory
// Arguments:      TArrayS & DSPCode   -- where to write DSP data
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads code back from DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t size;
	TArrayI cData;

	if (!this->CheckConnect("UpLoadDSPCode")) return(kFALSE);

	if (this->DSPCodeLoaded()) {
		size = fDGFData->fDSPSize;								// size of DSP data
		if (!this->WriteTSAR(0)) return(kFALSE); 				// where DSP pgm begins
		Int_t fc = fCamac.HasFastCamac();
		if (fc == -1) fc = 0;
		cData.Set(size);
		if (fCamac.BlockXfer(fCrate, fStation, A(0), F(fc), cData, 0, size, kTRUE) == -1) {	// start block xfer, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ".A0.F" << fc << " failed - DSPAddr=0, wc=" << size << endl;
			gMrbLog->Flush(this->ClassName(), "UploadDSPCode");
			return(kFALSE);
		}
		this->CopyData(DSPCode, cData.GetArray(), size);
		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": DSP code successfully read back (" << size << " words)" << endl;
			gMrbLog->Flush(this->ClassName(), "UploadDSPCode", setblue);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": DSP code must be DOWNloaded first" << endl;
		gMrbLog->Flush(this->ClassName(), "UploadDSPCode");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::ReadParamMemory(Bool_t ReadInputParams, Bool_t ReadOutputParams) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadParamMemory
// Purpose:        Read param values from DSP to memory
// Arguments:      Bool_t ReadInputParams   -- if input params should be read
//                 Bool_t ReadOutputParams  -- if output params should be read
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param values from DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	Int_t startAddr;
	Int_t idx;
	TArrayI cData;

	if (!this->CheckConnect("ReadParamMemory")) return(kFALSE);
	if (!this->CheckActive("ReadParamMemory")) return(kFALSE);
	if (this->IsOffline()) return(kTRUE);

	if (fDGFData->ParamNamesRead()) {
		if (ReadInputParams && ReadOutputParams) {
			nofParams = TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams; // size of both sections
			startAddr = TMrbDGFData::kDSPInparStartAddr;
			idx = 0;
		} else if (ReadOutputParams) {
			nofParams = TMrbDGFData::kNofDSPOutputParams;				 	// size of pointers section
			startAddr = TMrbDGFData::kDSPOutparStartAddr;
			idx = TMrbDGFData::kNofDSPInputParams;
		} else if (ReadInputParams) {
			nofParams = TMrbDGFData::kNofDSPInputParams;				 	// size of input section
			startAddr = TMrbDGFData::kDSPInparStartAddr;
			idx = 0;
		} else {
			return(kFALSE);
		}
		
		cData.Set(nofParams);
		if (!this->WriteTSAR(startAddr)) return(kFALSE); 										// where to read from
		if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ".A0.F0 failed - DSPAddr=0x" << setbase(16) << TMrbDGFData::kDSPInparStartAddr
							<< setbase(10) << ", wc=" << nofParams << endl;
			gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
			return(kFALSE);
		}
		fParams.Set(TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams);										// copy camac data to param area
		for (Int_t i = 0; i < nofParams; i++, idx++) fParams[idx] = (UShort_t) cData[i];
		fStatusM |= TMrbDGF::kParamValuesRead;
		if (ReadInputParams) fStatusM &= ~TMrbDGF::kParamsIncoreChanged;
		
		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": " << nofParams << " words read from DSP (addr=0x" << setbase(16)
							<< startAddr << setbase(10) << ")";
			if (ReadInputParams && !ReadOutputParams) gMrbLog->Out() << " (*INPUT SECTION ONLY*)";
			if (!ReadInputParams && ReadOutputParams) gMrbLog->Out() << " (*OUTPUT SECTION ONLY*)";
			gMrbLog->Out() << endl;
			gMrbLog->Flush(this->ClassName(), "ReadParamMemory", setblue);
		}
		
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": You have to read param names first" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::ReadParamMemory(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadParamMemory
// Purpose:        Read param memory for a single channel
// Arguments:      Int_t Channel   -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param data from DSP. Channel == -1 -> module params only
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString par;

	if (this->IsOffline()) return(kTRUE);

	if (Channel == -1) {
		return(this->ReadParamMemory((const Char_t *) NULL));
	} else {
		if (!this->CheckChannel("ReadParamMemory", Channel)) return(kFALSE);
		par = "*";
		par += Channel;
		return(this->ReadParamMemory(par.Data()));
	}
}

Bool_t TMrbDGF::ReadParamMemory(const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadParamMemory
// Purpose:        Read DSP data for a group of params
// Arguments:      Char_t * ParamName  -- param name, may be wild-carded
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads DSP data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	TString pType;
	TMrbNamedX * key;
	Int_t nofParams;
	Int_t from, to;
	Int_t idx;
	TString paramName;

	if (this->IsOffline()) return(kTRUE);

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		return(kFALSE);
	}

	if (ParamName == NULL) {					 	// read module params only (0 .. 63)
		from = 0;
		to = TMrbDGFData::kNofDSPInparModule - 1;
		paramName = "*";
	} else {										// read params for a given channel mask
		from = 0;
		to = TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams;
		paramName = ParamName;
	}

	TRegexp paramMask(paramName.Data(), kTRUE);

	key = fDGFData->FirstParam();
	nofParams = 0;
	while (key) {
		pName = key->GetName();
		if (pName.Index(paramMask) >= 0) {
			idx = key->GetIndex();
			if (idx >= from && idx <= to) {
				this->GetParValue(pName.Data(), kTRUE);
				nofParams++;
			}
		}
		key = fDGFData->NextParam(key);
	}

	if (nofParams > 0) {
		if (gMrbDGFData->fVerboseMode) {
			if (ParamName == NULL) {
				gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ": " << nofParams << " params read from DSP (*MODULE PARAMS ONLY*)" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadParamMemory", setblue);
			} else {
				gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": " << nofParams << " params read from DSP (masked by \"" << ParamName << "\")"	<< endl;
				gMrbLog->Flush(this->ClassName(), "ReadParamMemory", setblue);
			}
		}
		return(kTRUE);
	} else {
		if (ParamName == NULL) {
			gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": No module params found" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		} else {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ": No such params - "
							<< ParamName << endl;
			gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		}
		return(kFALSE);
	}
}

Bool_t TMrbDGF::ReadParamMemory(Int_t FromParam, Int_t ToParam) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadParamMemory
// Purpose:        Partially read param memory
// Arguments:      UInt_t FromParam    -- param to start with
//                 UInt_t ToParam      -- param to end with
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param data from DSP. Partial read.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	Int_t startAddr;
	Int_t idx = 0;
	Int_t i;
	TArrayI cData;

	if (!this->CheckConnect("ReadParamMemory")) return(kFALSE);
	if (!this->CheckActive("ReadParamMemory")) return(kFALSE);
	if (this->IsOffline()) return(kTRUE);

	if (FromParam > ToParam
	||	FromParam < 0
	||	ToParam > TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< "- addr error, param index should be in [0,"
						<< TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		return(kFALSE);
	}
			
	nofParams = ToParam - FromParam + 1;
	startAddr = TMrbDGFData::kDSPInparStartAddr + FromParam * sizeof(Int_t);
	if (!this->WriteTSAR(startAddr)) return(kFALSE); 							// where to read from
	cData.Set(nofParams);
	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0 failed - DSPAddr=0x" << setbase(16) << startAddr
						<< setbase(10) << ", wc=" << nofParams << endl;
		gMrbLog->Flush(this->ClassName(), "ReadParamMemory");
		return(kFALSE);
	}
	fParams.Set(TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams);										// copy camac data to param area
	for (i = 0; i < nofParams; i++, idx++) fParams[idx] = (UShort_t) cData[i];

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": " << nofParams << " words read from DSP (addr=0x" << setbase(16)
						<< startAddr << setbase(10) << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadParamMemory", setblue);
	}
	return(kTRUE);
}

Bool_t TMrbDGF::WriteParamMemory(Bool_t Reprogram) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WriteParamMemory
// Purpose:        Write param values from memory to DSP
// Arguments:      Bool_t Reprogram    -- kTRUE if DAC and FPGA should be re-programmed
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes param values to DSP.
//                 Only param DATA will be written (256 words).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	TArrayI cData;

	if (!this->CheckConnect("WriteParamMemory")) return(kFALSE);
	if (!this->CheckActive("WriteParamMemory")) return(kFALSE);
	if (this->IsOffline()) return(kTRUE);

	if (this->ParamValuesRead()) {
		this->WriteTSAR(TMrbDGFData::kDSPInparStartAddr);				// where to write to
		nofParams = TMrbDGFData::kNofDSPInputParams;					// size of params section (input only)
		this->CopyData(cData, fParams.GetArray(), nofParams);			// xfer param data to camac buffer
		if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ".A0.F16 failed - DSPAddr=0x" << setbase(16) << TMrbDGFData::kDSPInparStartAddr
							<< setbase(10) << ", wc=" << nofParams << endl;
			gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
			return(kFALSE);
		}
		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out() << fName << " in C" << fCrate << ".N" << fStation
					<< ": " << nofParams << " words written to DSP (addr=0x"
					<< setbase(16) << TMrbDGFData::kDSPInparStartAddr << setbase(10) << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteParamMemory", setblue);
		}

		if (Reprogram) {
			cout	<< setmagenta
					<< this->ClassName() << "::WriteParamMemory(): Reprogramming DACs and FPGA - Wait ... "
					<< ends << setblack << flush;
			this->SetParValue("RUNTASK", TMrbDGFData::kRunControl, kTRUE);		// reprogram DACs
			this->SetParValue("CONTROLTASK", TMrbDGFData::kProgramDACs, kTRUE);
			this->StartRun();
			this->WaitActive(1);

			TArrayS calAdcs(TMrbDGF::kCalibrationSize);
			calAdcs.Reset();
			UInt_t addr = this->GetParValue("AOUTBUFFER", kTRUE);
			this->WriteTSAR(addr);															// write to AOUTBUFFER
			this->CopyData(cData, calAdcs.GetArray(), TMrbDGF::kCalibrationSize); 	// write zeroes -> no calibration
			if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), cData, 0, TMrbDGF::kCalibrationSize, kTRUE) == -1) {	// start block xfer, 16 bit
				gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ".A0.F16 failed - DSPAddr=0x" << setbase(16) << addr
								<< setbase(10) << ", wc=" << nofParams << ", ActionCount=-1" << endl;
				gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
				return(kFALSE);
			}
		
			this->SetParValue("CONTROLTASK", TMrbDGFData::kUpdateFPGA, kTRUE);	// reprogram FPGA
			this->StartRun();
			this->WaitActive(1);

			cout	<< setmagenta << " done." << setblack << endl;
			gMrbLog->Out() << fName << " in C" << fCrate << ".N" << fStation << ": DACs and FPGA reprogrammed" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteParamMemory", setblue);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": No param data read so far" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::WriteParamMemory(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WriteParamMemory
// Purpose:        Update param memory for a single param
// Arguments:      Int_t Channel   -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString par;

	if (!this->CheckChannel("WriteParamMemory", Channel)) return(kFALSE);
	par = "*";
	par += Channel;
	return(this->WriteParamMemory(par.Data()));
}

Bool_t TMrbDGF::WriteParamMemory(const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WriteParamMemory
// Purpose:        Update memory for a group of params
// Arguments:      Char_t * ParamName  -- param name, may be wild-carded
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	TString pType;
	TMrbNamedX * key;
	Int_t nofParams;

	TRegexp paramMask(ParamName, kTRUE);

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
		return(kFALSE);
	}

	key = fDGFData->FirstParam();
	nofParams = 0;
	while (key) {
		pName = key->GetName();
		if (pName.Index(paramMask) >= 0 && key->GetIndex() <= TMrbDGFData::kNofDSPInputParams) {
			this->SetParValue(pName.Data(), this->GetParValue(pName.Data(), kFALSE), kTRUE);
			nofParams++;
		}
		key = fDGFData->NextParam(key);
	}

	if (nofParams > 0) {
		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": " << nofParams << " params written to DSP (masked by \""
							<< ParamName << "\")" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteParamMemory", setblue);
			}
			return(kTRUE);
	} else {
		gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": No such params - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::CopyParamMemory(TMrbDGF * Module, Bool_t CopyInputParams, Bool_t CopyOutputParams) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CopyParamMemory
// Purpose:        Read param values from other module
// Arguments:      TMrbDgf * Module         -- where to read params from
//                 Bool_t CopyInputParams   -- copy input params only
//                 Bool_t CopyOutputParams   -- copy output params only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param values from neighbouring module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	Int_t idx;
	Int_t i;
	Short_t * dp;

	if (fDGFData->ParamNamesRead()) {
		if (CopyInputParams && CopyOutputParams) {
			nofParams = TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams; // size of both sections
			idx = 0;
		} else if (CopyOutputParams) {
			nofParams = TMrbDGFData::kNofDSPOutputParams;				 	// size of pointers section
			idx = TMrbDGFData::kNofDSPInputParams;
		} else if (CopyInputParams) {
			nofParams = TMrbDGFData::kNofDSPInputParams;				 	// size of input section
			idx = 0;
		} else {
			return(kFALSE);
		}
		fParams.Set(TMrbDGFData::kNofDSPInputParams + TMrbDGFData::kNofDSPOutputParams);										// copy camac data to param area
		dp = Module->GetParamAddr() + idx;
		for (i = idx; i < nofParams; i++) fParams[i] = (UShort_t) *dp++;

		fStatusM |= TMrbDGF::kParamValuesRead;
		if (gMrbDGFData->fVerboseMode) {
			gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": " << nofParams << " words read from module " << Module->GetName()
							<< " in C" << Module->GetCrate() << ".N" << Module->GetStation();
			if (CopyInputParams && !CopyOutputParams) gMrbLog->Out() << " (*INPUT SECTION ONLY*)";
			if (!CopyInputParams && CopyOutputParams) gMrbLog->Out() << " (*OUTPUT SECTION ONLY*)";
			gMrbLog->Out() << endl;
			gMrbLog->Flush(this->ClassName(), "CopyParamMemory", setblue);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": You have to read param names first" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyParamMemory");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::CopyParamMemory(TMrbDGF * Module, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CopyParamMemory
// Purpose:        Copy param memory for a single channel
// Arguments:      TMrbDGF * Module   -- where to copy data from
//                 Int_t Channel      -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param data from neighbouring module. Channel == -1 -> module params only
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString par;

	if (Channel == -1) {
		return(this->CopyParamMemory(NULL));
	} else {
		if (!this->CheckChannel("CopyParamMemory", Channel)) return(kFALSE);
		par = "*";
		par += Channel;
		return(this->CopyParamMemory(Module, par.Data()));
	}
}

Bool_t TMrbDGF::CopyParamMemory(TMrbDGF * Module, const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CopyParamMemory
// Purpose:        Read param data for a group of params
// Arguments:      TMrbDGF * Module     -- where to copy from
//                 Char_t * ParamName   -- param name, may be wild-carded
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param data from neighbouring module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	TString pType;
	TMrbNamedX * key;
	Int_t nofParams;
	Int_t from, to;
	Int_t idx;
	TString paramName;

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyParamMemory");
		return(kFALSE);
	}

	if (ParamName == NULL || *ParamName == '*') { 	// read module params only (0 .. 63)
		from = 0;
		to = TMrbDGFData::kNofDSPInparModule - 1;
		paramName = "*";
	} else {										// read params for a given channel mask
		from = 0;
		to = TMrbDGFData::kNofDSPInputParams;
		paramName = ParamName;
	}

	TRegexp paramMask(paramName.Data(), kTRUE);

	key = fDGFData->FirstParam();
	nofParams = 0;
	while (key) {
		pName = key->GetName();
		if (pName.Index(paramMask) >= 0) {
			idx = key->GetIndex();
			if (idx >= from && idx <= to) {
				this->SetParValue(pName.Data(), Module->GetParValue(pName.Data(), kFALSE), kFALSE);
				nofParams++;
			}
		}
		key = fDGFData->NextParam(key);
	}

	if (nofParams > 0) {
		if (gMrbDGFData->fVerboseMode) {
			if (ParamName == NULL) {
				gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ": " << nofParams << " params read from modue "
								<< Module->GetName() << " in C" << Module->GetCrate() << ".N" << Module->GetStation()
								<< " (*MODULE PARAMS ONLY*)" << endl;
				gMrbLog->Flush(this->ClassName(), "CopyParamMemory", setblue);
			} else {
				gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
								<< ": " << nofParams << " params read from module "
								<< Module->GetName() << " in C" << Module->GetCrate() << ".N" << Module->GetStation()
								<< " (masked by \"" << ParamName << "\")" << endl;
				gMrbLog->Flush(this->ClassName(), "CopyParamMemory", setblue);
			}
		}
		return(kTRUE);
	} else {
		if (ParamName == NULL) {
			gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": No module params found" << endl;
			gMrbLog->Flush(this->ClassName(), "CopyParamMemory");
		} else {
			gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": No such params - " << ParamName << endl;
			gMrbLog->Flush(this->ClassName(), "CopyParamMemory");
		}
		return(kFALSE);
	}
}

Bool_t TMrbDGF::SetParValue(const Char_t * ParamName, Int_t Value, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetParValue
// Purpose:        Set a param value
// Arguments:      Char_t * ParamName   -- param name
//                 Int_t Value          -- value
//                 Bool_t UpdateDSP     -- kTRUE if DSP memory is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the value of a given parameter in memory.
//                 Writes param value back to DSP if immediate flag is set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * param;
	Int_t pOffset;

	param = fDGFData->FindParam(ParamName); 			// find param offset by param name
	if (param == NULL) {
		gMrbLog->Err() << "No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "SetParValue");
		return(kFALSE);
	}

	pOffset = param->GetIndex();						// offset has to be in the data region
	if (pOffset > TMrbDGFData::kNofDSPInputParams) {
		TString pName = param->GetName();
		if (pName.CompareTo("SYNCHDONE") != 0) {
			gMrbLog->Err() << "Param \"" << ParamName << "\" is in the OUTPUT section - can't be set" << endl;
			gMrbLog->Flush(this->ClassName(), "SetParValue");
			return(kFALSE);
		}
	}

	fParams[pOffset] = (UShort_t) (Value & 0xffff);		// set value in memory
	if (UpdateDSP && !this->IsOffline()) {				// update param data in DSP?
		if (!this->CheckConnect("SetParValue")) return(kFALSE);
		if (!this->CheckActive("SetParValue")) return(kFALSE);
		this->WritePSA(pOffset);						// where to write to
		Int_t cVal = Value & 0xffff;
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(16), cVal, kTRUE)) { 		// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F16 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "SetParValue");
			return(kFALSE);
		}
	}

	if (!UpdateDSP) fStatusM |= TMrbDGF::kParamsIncoreChanged;
	return(kTRUE);
}

Bool_t TMrbDGF::SetParValue(Int_t Offset, Int_t Value, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetParValue
// Purpose:        Set a param value
// Arguments:      Int_t Offset         -- offset
//                 Int_t Value          -- value
//                 Bool_t UpdateDSP     -- kTRUE if DSP memory is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the value of a given parameter in memory.
//                 Writes param value back to DSP if update flag is set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fParams[Offset] = (UShort_t) (Value & 0xffff);		// set value in memory
	if (UpdateDSP && !this->IsOffline()) {				// update param data in DSP?
		if (!this->CheckConnect("SetParValue")) return(kFALSE);
		if (!this->CheckActive("SetParValue")) return(kFALSE);
		this->WritePSA(Offset);						// where to write to
		Int_t cVal = Value & 0xffff;
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(16), cVal, kTRUE)) { 		// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F16 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "SetParValue");
			return(kFALSE);
		}
	}

	if (!UpdateDSP) fStatusM |= TMrbDGF::kParamsIncoreChanged;
	return(kTRUE);
}

Int_t TMrbDGF::GetParValue(const Char_t * ParamName, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetParValue
// Purpose:        Get param value or pointer
// Arguments:      Char_t * ParamName   -- param name
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Int_t Value          -- value
// Exceptions:
// Description:    Reads the value of a given parameter from memory or DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * param;
	Int_t pValue;
	Int_t pOffset;

	param = fDGFData->FindParam(ParamName); 			// find param offset by param name
	if (param == NULL) {
		gMrbLog->Err() << "No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "GetParValue");
		return(-1);
	}

	pOffset = param->GetIndex();						// offset has to be in the data region

	pValue = fParams[pOffset] & 0xffff;					// incore value

	if (ReadFromDSP && !this->IsOffline()) {									// reread from DSP?
		if (!this->CheckConnect("GetParValue")) return(-1);
		if (!this->CheckActive("GetParValue")) return(-1);
		this->WritePSA(pOffset);						// where to read from
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(0), pValue, kTRUE)) {	// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F0 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "GetParValue");
			return(-1);
		}
		fParams[pOffset] = (UInt_t) pValue & 0xffff;	// update incore data
	}

	return(pValue);
}

Bool_t TMrbDGF::SetParValue(Int_t Channel, const Char_t * ParamName, Int_t Value, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetParValue
// Purpose:        Set param value for a single channel
// Arguments:      Int_t Channel        -- channel number
//                 Char_t * ParamName   -- param name
//                 Int_t Value          -- value
//                 Bool_t UpdateDSP     -- kTRUE if DSP memory is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets param value of a given channel in memory.
//                 Writes param value back to DSP if immediate flag is set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString paramName;

	if (!this->CheckChannel("SetParValue", Channel)) return(kFALSE);
	paramName = ParamName;
	paramName += Channel;
	return(this->SetParValue(paramName.Data(), Value, UpdateDSP));
}

Int_t TMrbDGF::GetParValue(Int_t Channel, const Char_t * ParamName, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetParValue
// Purpose:        Get param value or pointer for a given channel
// Arguments:      Int_t Channel        -- channel number
//                 Char_t * ParamName   -- param name
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Int_t Value          -- value
// Exceptions:
// Description:    Reads param value of a given channel from memory or DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString paramName;

	if (!this->CheckChannel("GetParValue", Channel)) return(-1);
	paramName = ParamName;
	paramName += Channel;
	return(this->GetParValue(paramName.Data(), ReadFromDSP));
}

Int_t TMrbDGF::GetParValue(Int_t Offset, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetParValue
// Purpose:        Get param value
// Arguments:      Int_t Offset         -- param offset
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Int_t Value          -- value
// Exceptions:
// Description:    Reads the value of a given parameter from memory or DSP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pValue;

	pValue = fParams[Offset] & 0xffff;					// incore value

	if (ReadFromDSP && !this->IsOffline()) {									// reread from DSP?
		if (!this->CheckConnect("GetParValue")) return(-1);
		if (!this->CheckActive("GetParValue")) return(-1);
		this->WritePSA(Offset);						// where to read from
		if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(0), pValue, kTRUE)) {	// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F0 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "GetParValue");
			return(-1);
		}
		fParams[Offset] = (UInt_t) pValue & 0xffff;	// update incore data
	}

	return(pValue);
}

Bool_t TMrbDGF::FPGACodeLoaded(TMrbDGFData::EMrbFPGAType FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FGPACodeLoaded
// Purpose:        Test if FPGA code is loaded
// Arguments:      EMrbFPGAType FPGAType    -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been loaded.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t bit = (FPGAType == TMrbDGFData::kSystemFPGA) ?
								TMrbDGF::kSystemFPGACodeLoaded :
								TMrbDGF::kFippiFPGACodeLoaded;
	return((fStatusM & bit) != 0);
}

Bool_t TMrbDGF::FPGACodeLoaded(const Char_t * FPGAType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FGPACodeLoaded
// Purpose:        Test if FPGA code is loaded
// Arguments:      Char_t * FPGAType    -- fpga type (system or fippi)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if FPGA code has been loaded.
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

void TMrbDGF::Print(ostream & OutStrm) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::Print
// Purpose:        Print status
// Arguments:      ostream & OutStrm     -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs status info.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vDSP, vvDSP, vFPGA, vvFPGA, vHard, vvHard, vIntrfc, vvIntrfc;

	vDSP = vvDSP = vFPGA = vvFPGA = vHard = vvHard = vIntrfc = vvIntrfc = 0;

	if (IsConnected() && this->DSPCodeLoaded()) {							// get version numbers from DSP
		vDSP = this->GetParValue("SOFTWAREID", kTRUE);
		vvDSP = this->GetParValue("SOFTVARIANT", kTRUE);
		vFPGA = this->GetParValue("FIPPIID", kTRUE);
		vvFPGA = this->GetParValue("FIPPIVARIANT", kTRUE);
		vHard = this->GetParValue("HARDWAREID", kTRUE);
		vvHard = this->GetParValue("HARDVARIANT", kTRUE);
		vIntrfc = this->GetParValue("INTRFCID", kTRUE);
		vvIntrfc = this->GetParValue("INTRFCVARIANT", kTRUE);
	}

	cout	<< "===========================================================================================" << endl
			<< " DGF-4C Module Section" << endl
			<< "..........................................................................................." << endl;
	cout	<< " Module name                  :	" << this->GetName() << endl;
	if (IsConnected()) {
		cout	<< " Connected to host            :	" << fCamacHost << endl;
	} else {
		cout	<< " CAMAC host (NOT connected)   :	" << fCamacHost << endl;
	}
	cout	<< " Crate                        : C" << fCrate << endl;
	cout	<< " Station                      : N" << fStation << endl;
	cout	<< " Local data base              : ";

	if (this->HasLocalDataBase()) cout << "Yes"; else cout << "No"; cout << endl;

	cout	<< " XIA Release                  : "	<< gEnv->GetValue("TMrbDGF.XiaRelease", "unknown")
													<< " (" << gEnv->GetValue("TMrbDGF.XiaDate", "")
													<< ")" << endl;
	
	cout	<< " DGF hardware                 : Version "
			<< setiosflags(ios::uppercase) << setbase(16)
			<< vHard << " (" << vvHard << ")"
			<< setbase(10) << resetiosflags(ios::uppercase)
			<< endl;

	cout	<< " DGF interface                : Version "
			<< setiosflags(ios::uppercase) << setbase(16)
			<< vIntrfc << " (" << vvIntrfc << ")"
			<< setbase(10) << resetiosflags(ios::uppercase)
			<< endl;

	cout	<< " FPGA code loaded [System]    : ";
	if (this->FPGACodeLoaded(TMrbDGFData::kSystemFPGA)) {
		cout	<< "Version " << setiosflags(ios::uppercase) << setbase(16)
				<< vFPGA << " (" << vvFPGA << ")"
				<< setbase(10) << resetiosflags(ios::uppercase);
	} else {
		cout << "No";
	}
	cout << endl;

	cout	<< " FPGA code loaded [Fippi]     : ";
	if (this->FPGACodeLoaded(TMrbDGFData::kFippiFPGA)) {
		cout	<< "Version " << setiosflags(ios::uppercase) << setbase(16)
				<< vFPGA << " (" << vvFPGA << ")"
				<< setbase(10) << resetiosflags(ios::uppercase);
	} else {
		cout << "No";
	}
	cout << endl;

	cout	<< " DSP code loaded              : ";
	if (this->DSPCodeLoaded()) {
		cout	<< "Version " << setiosflags(ios::uppercase) << setbase(16)
				<< vDSP << " (" << vvDSP << ")"
				<< setbase(10) << resetiosflags(ios::uppercase);
	} else {
		cout << "No";
	}
	cout << endl;

	cout	<< " Param values read            : ";
	if (this->ParamValuesRead()) cout << fDGFData->GetNofParams(); else cout << "No"; cout << endl;

	cout	<< " Trigger channels             : ";
	this->PrintTriggerMask(kFALSE);
	cout << endl;
	cout	<< " Good channels                : ";
	this->PrintChannelMask(kFALSE);
	cout << endl;

	cout	<< "-------------------------------------------------------------------------------------------" << endl;
	if (HasLocalDataBase()) fDGFData->Print();
}

Bool_t TMrbDGF::PrintParamsToFile(const Char_t * FileName, const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintParamsToFile
// Purpose:        Print param(s)
// Arguments:      Char_t * FileName      -- file name
//                 Char_t * ParamName     -- param name (may be wildcarded)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;
	TString fileName;
	ofstream paramOut;
	
	fileName = FileName;
	fileName = fileName.Strip(TString::kBoth);
	if (fileName.Length() == 0) fileName = this->GetName();
	if (fileName.Index(".", 0) == -1) fileName += ".par";
	
	paramOut.open(fileName.Data(), ios::out);
	if (!paramOut.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParamsToFile");
		return(kFALSE);
	}
	
	sts = PrintParams(paramOut, ParamName);
	paramOut.close();
	gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation << ": Params written to file " << fileName << endl;
	gMrbLog->Flush(this->ClassName(), "PrintParamsToFile", setblue);
	return(sts);
}

Bool_t TMrbDGF::PrintParamsToFile(const Char_t * FileName, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintParamsToFile
// Purpose:        Print param(s)
// Arguments:      Char_t * FileName      -- file name
//                 Int_t Channel          -- channel no (-1 -> all channels)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;
	TString fileName;
	ofstream paramOut;
	
	fileName = FileName;
	fileName = fileName.Strip(TString::kBoth);
	if (fileName.Length() == 0) fileName = this->GetName();
	if (fileName.Index(".", 0) == -1) fileName += ".par";
	
	paramOut.open(fileName.Data(), ios::out);
	if (!paramOut.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParamsToFile");
		return(kFALSE);
	}
	
	sts = PrintParams(paramOut, Channel);
	paramOut.close();
	gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation << ": Params written to file " << fileName << endl;
	gMrbLog->Flush(this->ClassName(), "PrintParamsToFile", setblue);
	return(sts);
}

Bool_t TMrbDGF::PrintParams(ostream & OutStrm, const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintParams
// Purpose:        Print param(s)
// Arguments:      ostream & OutStrm       -- output stream
//                 Char_t * ParamName      -- param name (may be wildcarded)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	Int_t pOffset;
	TString pType;
	TMrbNamedX * key;
	Bool_t found;

	TRegexp paramMask(ParamName, kTRUE);

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParams");
		return(kFALSE);
	}

	key = fDGFData->FirstParam();
	found = kFALSE;
	while (key) {
		pName = key->GetName();
		if (pName.Index(paramMask) >= 0) {
			if (!found) {
				OutStrm	<< "===========================================================================================" << endl
						<< " DGF-4C Param Section: Module " << this->GetTitle() << endl
						<< "..........................................................................................." << endl;
				OutStrm	<< setiosflags(ios::left) << setw(15) << " ParamName";
				OutStrm	<< setiosflags(ios::left) << setw(10) << "Type";
				OutStrm	<< resetiosflags(ios::left) << setw(10) << "Offset";
				OutStrm	<< setw(10) << "Value" << endl;
				OutStrm	<< "..........................................................................................." << endl;
			}
			found = kTRUE;
			Int_t nofWords = ((pName.CompareTo("USERIN") == 0) || (pName.CompareTo("USEROUT") == 0)) ? 16 : 1;
			pOffset = key->GetIndex();
			for (Int_t nw = 0; nw < nofWords; nw++, pOffset++) {
				pType = (pOffset < TMrbDGFData::kNofDSPInputParams) ? "Input" : "Output";
				TMrbString pn;
				if (nw > 0) {
					pn = pName;
					pn += "+";
					pn += nw;
				} else {
					pn = pName;
				}
				OutStrm	<< " " << setiosflags(ios::left) << setw(14) << pn;
				OutStrm	<< setiosflags(ios::left) << setw(10) << pType;
				OutStrm	<< resetiosflags(ios::left) << setw(10) << pOffset;
				if (this->ParamValuesRead()) {
					OutStrm	<< setw(10) << (UShort_t) fParams[pOffset]
							<< "    0x" << setbase(16) << (UShort_t) fParams[pOffset] << setbase(10);
				} else if (pOffset <= TMrbDGFData::kNofDSPInputParams) {
					OutStrm	<< setw(10) << "undef";
				}
				OutStrm	<< endl;
			}
		}
		key = fDGFData->NextParam(key);
	}

	if (found) {
		OutStrm	<< "-------------------------------------------------------------------------------------------" << endl;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "No param found matching \"" << ParamName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParams");
		return(kFALSE);
	}
}

Bool_t TMrbDGF::PrintParams(ostream & OutStrm, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintParams
// Purpose:        Print param(s)
// Arguments:      ostream & OutStrm       -- output stream
//                 Int_t Channel           -- channel
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs param data for a given channel.
//                 Channel = -1 --> output module params.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	Int_t pOffset;
	TString pType;
	TMrbNamedX * key;
	Bool_t found;
	Int_t from, to;
	TMrbString title;

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParams");
		return(kFALSE);
	}

	if (Channel == -1) { 			// output module params only (0 .. 63)
		from = 0;
		to = from + TMrbDGFData::kNofDSPInparModule - 1;
		title = "Common input params";
	} else if (Channel < TMrbDGFData::kNofChannels) {		// output params for a given channel
		from = TMrbDGFData::kNofDSPInparModule + Channel * TMrbDGFData::kNofDSPInparChannel;
		to = from + TMrbDGFData::kNofDSPInparChannel - 1;
		title = "Input params for channel ";
		title += Channel;
	} else {
		gMrbLog->Err() << "Illegal channel number - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "PrintParams");
		return(kFALSE);
	}

	key = fDGFData->FirstParam();
	found = kFALSE;
	while (key) {
		pOffset = key->GetIndex();
		if (pOffset >= from && pOffset <= to) {
			if (!found) {
				OutStrm	<< "===========================================================================================" << endl
						<< " DGF-4C Param Section: Module " << this->GetTitle()
						<< " - " << title << endl
						<< "..........................................................................................." << endl;
				OutStrm	<< setiosflags(ios::left) << setw(15) << " ParamName";
				OutStrm	<< setiosflags(ios::left) << setw(10) << "Type";
				OutStrm	<< resetiosflags(ios::left) << setw(10) << "Offset";
				OutStrm	<< setw(10) << "Value" << endl;
				OutStrm	<< "..........................................................................................." << endl;
			}
			found = kTRUE;
			pName = key->GetName();
			OutStrm	<< " " << setiosflags(ios::left) << setw(14) << pName;
			OutStrm	<< setiosflags(ios::left) << setw(10) << "Input";
			OutStrm	<< resetiosflags(ios::left) << setw(10) << pOffset;
			if (this->ParamValuesRead()) {
				OutStrm	<< setw(10) << (UShort_t) fParams[pOffset]
							<< "    0x" << setbase(16) << (UShort_t) fParams[pOffset] << setbase(10);
			} else {
				OutStrm	<< setw(10) << "undef";
			}
			OutStrm	<< endl;
		}
		key = fDGFData->NextParam(key);
	}

	OutStrm	<< "-------------------------------------------------------------------------------------------" << endl;
	return(kTRUE);
}

Bool_t TMrbDGF::PrintStatus(ostream & OutStrm, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintStatus
// Purpose:        Print status info
// Arguments:      ostream & OutStrm       -- output stream
//                 Int_t Channel           -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs register contents for a given channel.
//                 Channel = -1 --> output module regs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t reg;
	TMrbNamedX * pKey;

	if (!this->CheckConnect("PrintStatus")) return(kFALSE);

	if (Channel == -1) {
		cout	<< "===========================================================================================" << endl
				<< " DGF-4C Status: " << this->GetTitle() << endl
				<< "..........................................................................................." << endl;
		reg = this->ReadICSR();
		cout	<< " CAMAC Control & Status (System, F1.A9), data=0x" << setbase(16) << reg << setbase(10) << endl
				<< "..........................................................................................." << endl;
		fDGFData->fLofCamacICSRBits.Print(cout, "      ", reg);
	
		reg = this->ReadCSR();
		cout	<< " CAMAC Control & Status (F1.A0), data=0x" << setbase(16) << reg << setbase(10) << endl
				<< "..........................................................................................." << endl;
		fDGFData->fLofCamacCSRBits.Print(cout, "      ", reg);
	
		if (!fDGFData->ParamNamesRead()) {
			gMrbLog->Err() << "No param names read" << endl;
			gMrbLog->Flush(this->ClassName(), "PrintStatus");
			cout	<< "-------------------------------------------------------------------------------------------" << endl;
			return(kFALSE);
		}
		if (!this->ParamValuesRead()) {
			gMrbLog->Err() << "No param values read" << endl;
			gMrbLog->Flush(this->ClassName(), "PrintStatus");
			cout	<< "-------------------------------------------------------------------------------------------" << endl;
			return(kFALSE);
		}
		reg = this->GetParValue("RUNTASK");
		pKey = fDGFData->fLofRunTasks.FindByIndex(reg);
		cout	<< "..........................................................................................." << endl
				<< " Param RUNTASK (Offset=" << fDGFData->FindParam("RUNTASK")->GetIndex()
				<< "), data=0x" << setbase(16) << reg << setbase(10) << endl
				<< "..........................................................................................." << endl;
		if (pKey != NULL) {
			cout	<< "      " << pKey->GetName() << " (" << reg << ")" << endl;
		} else {
			cout	<< "      Illegal Value (" << reg << ")" << endl;
		}

		reg = this->GetParValue("CONTROLTASK");
		pKey = fDGFData->fLofControlTasks.FindByIndex(reg);
		cout	<< "..........................................................................................." << endl
				<< " Param CONTROLTASK (Offset=" << fDGFData->FindParam("CONTROLTASK")->GetIndex()
				<< "), data=0x" << setbase(16) << reg << setbase(10) << endl
				<< "..........................................................................................." << endl;
		if (pKey != NULL) {
			cout	<< "      " << pKey->GetName() << " (" << reg << ")" << endl;
		} else {
			cout	<< "      Illegal Value (" << reg << ")" << endl;
		}

		cout	<< "-------------------------------------------------------------------------------------------" << endl;
		return(kTRUE);
	} else if (Channel >= 0 && Channel < TMrbDGFData::kNofChannels) {
		if (!fDGFData->ParamNamesRead()) {
			gMrbLog->Err() << "No param names read" << endl;
			gMrbLog->Flush(this->ClassName(), "PrintStatus");
			return(kFALSE);
		}
		if (!this->ParamValuesRead()) {
			gMrbLog->Err() << "No param values read" << endl;
			gMrbLog->Flush(this->ClassName(), "PrintStatus");
			return(kFALSE);
		}
		reg = this->GetParValue(Channel, "CHANCSRA");
		cout	<< "===========================================================================================" << endl
				<< " DGF-4C Status: " << this->GetTitle() << endl
				<< "..........................................................................................." << endl;
		cout	<< " Channel " << Channel << " Control & Status Reg, Param CHANCSRA" << Channel
				<< " (Offset=" << fDGFData->FindParam(Channel, "CHANCSRA")->GetIndex()
				<< "), data=0x" << setbase(16) << reg << setbase(10) << endl
				<< "..........................................................................................." << endl;
		fDGFData->fLofChanCSRABits.Print(cout, "      ", reg);
		cout	<< "-------------------------------------------------------------------------------------------" << endl;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Illegal channel number - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "PrintStatus");
		return(kFALSE);
	}
}

Int_t TMrbDGF::LoadParams(const Char_t * ParamFile, const Char_t * AltParamFile, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::LoadParams
// Purpose:        Read param values from file
// Arguments:      Char_t * ParamFile     -- file name
//                 Char_t * AltParamFile  -- alternative file name
//                 Bool_t UpdateDSP       -- kTRUE if DSP is to be updated
// Results:        Int_t NofParams        -- number of params read
// Exceptions:
// Description:    Reads a set param values from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	Int_t pValue;
	TString pName;
	TMrbNamedX * pKey;
	ifstream pf;
	Int_t nofParams;
	TString paramFile;
	TMrbString pLine;
	TObjArray pFields;
	TMrbString pStr;
	Int_t line;
	Int_t nFields;
	TMrbSystem uxSys;
		
	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParams");
		return(-1);
	}

	if (gSystem->AccessPathName(ParamFile, kFileExists) == 0) {
		paramFile = ParamFile;
	} else if (*AltParamFile != '\0' && gSystem->AccessPathName(AltParamFile, kFileExists) == 0) {
		paramFile = AltParamFile;
	} else {
			gMrbLog->Err() << "Param file not found - " << ParamFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadParams");
			if (*AltParamFile != '\0') {
				gMrbLog->Err() << "Alternative param file not found - " << AltParamFile << endl;
				gMrbLog->Flush(this->ClassName(), "LoadParams");
			}
			return(-1);
	}

	if (uxSys.CheckExtension(paramFile.Data(), ".par")) {
		pf.open(paramFile, ios::in);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadParams");
			return(-1);
		}

		line = 0;
		nofParams = 0;
		for (;;) {
			pLine.ReadLine(pf, kFALSE);
			line++;
			if (pf.eof()) break;
			pLine = pLine.Strip(TString::kBoth);
			if (pLine.Length() == 0) continue;						// empty line
			if (pLine(0) == '#') continue;							// comment
			pFields.Delete();
			nFields = pLine.Split(pFields, ":");
			if (nFields < 3) {
				gMrbLog->Err() << paramFile << " (line " << line << "): Wrong number of fields - " << nFields << endl;
				gMrbLog->Flush(this->ClassName(), "LoadParams");
				continue;
			}
			pName = ((TObjString *) pFields[0])->GetString();
			pKey = fDGFData->FindParam(pName.Data());
			if (pKey == NULL) {
				gMrbLog->Err() << paramFile << " (line " << line << "): No such param - " << pName << endl;
				gMrbLog->Flush(this->ClassName(), "LoadParams");
				continue;
			} else {
				nofParams++;
				pStr = ((TObjString *) pFields[1])->GetString();
				pStr.ToInteger(pOffset);
				pStr = ((TObjString *) pFields[2])->GetString();
				pStr.ToInteger(pValue);				
				fParams[pOffset] = pValue;
			}
		}
		pf.close();
	} else if (uxSys.CheckExtension(paramFile.Data(), ".pdmp")) {
		pf.open(paramFile, ios::in);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadParams");
			return(-1);
		}

		line = 0;
		nofParams = 0;
		for (;;) {
			pLine.ReadLine(pf, kFALSE);
			if (pf.eof()) break;
			pLine = pLine.Strip(TString::kBoth);
			if (pLine.Length() == 0) continue;						// empty line
			if (pLine(0) == '#') continue;							// comment
			nofParams++;
			pStr = pLine;
			pStr.ToInteger(pValue);				
			fParams[line] = pValue;
			line++;
		}
		pf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << paramFile << " (should be .par or .pdmp)" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParams");
		return(-1);
	}

	fStatusM |= TMrbDGF::kParamValuesRead;

	if (UpdateDSP && !this->IsOffline()) {
		if (!this->CheckConnect("LoadParams")) return(-1);
		this->WriteParamMemory();
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofParams << " params read from " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParams", setblue);
	}

	return(nofParams);
}

Int_t TMrbDGF::LoadParamsToEnv(TEnv * Env, const Char_t * ParamFile, const Char_t * AltParamFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::LoadParamsToEnv
// Purpose:        Read param values to ROOT environment
// Arguments:      TEnv * Env             -- environment to store params
//                 Char_t * ParamFile     -- file name
//                 Char_t * AltParamFile  -- alternate file name
// Results:        Int_t NofParams      -- number of params read
// Exceptions:
// Description:    Reads a set param values from file and stores them into
//                 ROOT environment.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	TMrbNamedX * pKey;
	ifstream pf;
	Int_t nofParams;
	TString paramFile;
	TMrbString pLine;
	TObjArray pFields;
	TString pStr;
	Int_t line;
	Int_t nFields;
	TMrbSystem uxSys;

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
		return(0);
	}

	if (gSystem->AccessPathName(ParamFile, kFileExists) == 0) {
		paramFile = ParamFile;
	} else if (*AltParamFile != '\0' && gSystem->AccessPathName(AltParamFile, kFileExists) == 0) {
		paramFile = AltParamFile;
	} else {
			gMrbLog->Err() << "Param file not found - " << ParamFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
			if (*AltParamFile != '\0') {
				gMrbLog->Err() << "Alternative param file not found - " << AltParamFile << endl;
				gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
			}
			return(-1);
	}

	if (uxSys.CheckExtension(paramFile.Data(), ".par")) {
		pf.open(paramFile, ios::in);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
			return(0);
		}

		line = 0;
		nofParams = 0;
		for (;;) {
			pLine.ReadLine(pf, kFALSE);
			line++;
			if (pf.eof()) break;
			pLine = pLine.Strip(TString::kBoth);
			if (pLine.Length() == 0) continue;						// empty line: ignore
			if (pLine(0) == '#' && pLine(1) != '+') continue;		// comment (#): ignore
			Int_t ncmt = pLine.Index("#", 0);
			if (ncmt > 0) pLine.Resize(ncmt);
			pLine = pLine.Strip(TString::kBoth);
			pFields.Delete();
			nFields = pLine.Split(pFields, ":");
			pName = ((TObjString *) pFields[0])->GetString();
			if (pName(0) == '#') {									// special entry (#+): strip off leading chars
				if (nFields != 2) {
					gMrbLog->Err() << paramFile << " (line " << line << "): Wrong number of fields - " << nFields << endl;
					gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
					continue;
				}
				pStr = pName(2, pName.Length() - 2);
				pStr = pStr.Strip(TString::kBoth);
				pStr += "=";
				pStr += ((TObjString *) pFields[1])->GetString();
			} else {
				if (nFields < 3) {
					gMrbLog->Err() << paramFile << " (line " << line << "): Wrong number of fields - " << nFields << endl;
					gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
					continue;
				}
				pKey = fDGFData->FindParam(pName.Data());
				if (pKey == NULL) {
					gMrbLog->Err() << paramFile << " (line " << line << "): No such param - " << pName << endl;
					gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
					continue;
				}
				nofParams++;
				pStr = ((TObjString *) pFields[0])->GetString();
				pStr += "=";
				pStr += ((TObjString *) pFields[2])->GetString();
			}
			Env->SetValue(pStr.Data(), kEnvUser);
		}
		pf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << paramFile << " (should be .par)" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv");
		return(0);
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofParams << " params loaded from " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "LoadParamsToEnv", setblue);
	}

	return(nofParams);
}

Int_t TMrbDGF::SaveParams(const Char_t * ParamFile, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SaveParams
// Purpose:        Save params to file
// Arguments:      Char_t * ParamFile   -- file name
//                 Bool_t ReadFromDSP   -- kTRUE if params to be read from DSP
// Results:        Int_t NofParams      -- number of params written
// Exceptions:
// Description:    Saves params to file (input params only).
//                 Format is "unix style":
//                       <name>:<offset>:<value>:<hexval>
//                 Additional entries may be used to identify the module:
//                       FileType:par
//                       Module:<name>
//                       Revision:<rev>
//                       Crate:<crate>
//                       Station:<station>
//                       Cluster:<id>
//                       XiaRelease:<release>
//                       XiaDate:<date>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	UShort_t pValue;
	TString pName;
	TMrbNamedX * pKey;
	ofstream pf;
	Int_t nofParams;
	TString paramFile;
	TString paramPath;
	TMrbString info;
	TDatime dt;

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	if (ReadFromDSP && !this->IsOffline()) {
		if (!this->CheckConnect("SaveParams")) return(-1);
		this->ReadParamMemory(kTRUE, kTRUE);
	}

	if (!this->ParamValuesRead()) {
		gMrbLog->Err() << "No param values read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	paramFile = ParamFile;
	if (paramFile.Index("/") != 0 && paramFile.Index("./", 0) != 0) {
		paramPath = gEnv->GetValue("TMrbDGF.SettingsPath", ".");
		paramFile = paramPath + "/";
		paramFile += ParamFile;
	}
	if (paramFile.Index(".par") > 0) {			// write ascii data
		pf.open(paramFile, ios::out);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveParams");
			return(-1);
		}

		pf << "#----------------------------------------------------------------------------------------" << endl;
		pf << "# Name          : "								<< paramFile << endl;
		pf << "# Contents      : Settings for DGF-4C "			<< this->GetName()
																<< " in C" << this->GetCrate()
																<< ".N" << this->GetStation() << endl;
		pf << "# Creation date : " 								<< dt.AsString() << endl;
		pf << "# Created by    : " 								<< gSystem->Getenv("USER") << endl;
		pf << "#" << endl;
		pf << "# Data format   : <name>:<offset>:<value>:<hexval>" << endl;
		pf << "#----------------------------------------------------------------------------------------" << endl;
		pf << "#+FileType:par" << endl;
		pf << "#+Module:"										<< this->GetName() << endl;
		pf << "#+Revision:" 									<< this->GetRevision()->GetName() << endl;
		pf << "#+Crate:"										<< this->GetCrate() << endl;
		pf << "#+Station:" 										<< this->GetStation() << endl;
		pf << "#+Cluster:"										<< this->GetClusterInfo(info) << endl;
		pf << "#+XiaRelease:"									<< gEnv->GetValue("TMrbDGF.XiaRelease", "") << endl;
		pf << "#+XiaDate:" 										<< gEnv->GetValue("TMrbDGF.XiaDate", "") << endl;
		pf << "#........................................................................................" << endl;
		pf << endl;
		nofParams = 0;
		pKey = fDGFData->FirstParam();
		while (pKey != NULL) {
			pOffset = pKey->GetIndex();
			if (pOffset < TMrbDGFData::kNofDSPInputParams) {
				pValue = fParams[pOffset];
				pf << pKey->GetName() << ":" << pOffset << ":" << pValue << ":0x" << setbase(16) << pValue << setbase(10) << endl;
				nofParams++;
			}
			pKey = fDGFData->NextParam(pKey);
		}
		pf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << paramFile << " (should be .par)" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofParams << " params written to " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams", setblue);
	}

	return(nofParams);
}

Int_t TMrbDGF::LoadPsaParams(const Char_t * ParamFile, const Char_t * AltParamFile, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::LoadPsaParams
// Purpose:        Read PSA params from file
// Arguments:      Char_t * ParamFile    -- file name
//                 Char_t * AltParamFile -- alternative param file
//                 Bool_t UpdateDSP      -- kTRUE if DSP is to be updated
// Results:        Int_t NofParams       -- number of params read
// Exceptions:
// Description:    Reads a set PSA values from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	Int_t pValue;
	TString pName;
	TMrbNamedX * pKey;
	ifstream pf;
	Int_t nofParams;
	TString paramFile;
	TMrbString pLine;
	TObjArray pFields;
	TMrbString pStr;
	Int_t line;
	Int_t nFields;
	TMrbSystem uxSys;
	TMrbLofNamedX psaNames;
		
	Int_t psaVal[16];

	psaNames.AddNamedX(kMrbPSANames);

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
		return(-1);
	}

	paramFile = ParamFile;
	if (uxSys.CheckExtension(paramFile.Data(), ".psa")) {
		pf.open(paramFile, ios::in);
		if (!pf.good()) {
			if (*AltParamFile != '\0') {
				paramFile = AltParamFile;
				pf.open(paramFile, ios::in);
				if (!pf.good()) return(-1); 	// give up if none of these files exist
			} else {
				return(-1);
			}
		}
		line = 0;
		nofParams = 0;
		Int_t pOffset0 = -1;
		while (nofParams < 16) {
			pLine.ReadLine(pf, kFALSE);
			line++;
			if (pf.eof()) break;
			pLine = pLine.Strip(TString::kBoth);
			if (pLine.Length() == 0) continue;						// empty line
			if (pLine(0) == '#') continue;							// comment
			Int_t ncmt = pLine.Index("#", 0);
			if (ncmt > 0) pLine.Resize(ncmt);
			pLine = pLine.Strip(TString::kBoth);
			pFields.Delete();
			nFields = pLine.Split(pFields, ":");
			if (nFields < 3) {
				gMrbLog->Err() << paramFile << " (line " << line << "): Wrong number of fields - " << nFields << endl;
				gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
				continue;
			}
			pName = ((TObjString *) pFields[0])->GetString();
			pKey = psaNames.FindByName(pName.Data());
			if (pKey == NULL) {
				gMrbLog->Err() << paramFile << " (line " << line << "): No such PSA name - " << pName << endl;
				gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
				continue;
			} else {
				pStr = ((TObjString *) pFields[1])->GetString();
				pStr.ToInteger(pOffset);
				if (pOffset0 == -1) pOffset0 = pOffset;
				if (pOffset - pOffset0 > 16) {
					gMrbLog->Err() << paramFile << " (line " << line << "): [" << pName << "] PSA offset > 16" << endl;
					gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
					continue;
				}
				pStr = ((TObjString *) pFields[2])->GetString();
				pStr.ToInteger(pValue);				
				psaVal[pOffset - pOffset0] = pValue;
				nofParams++;
			}
		}
		pf.close();
	} else if (uxSys.CheckExtension(paramFile.Data(), ".psadmp")) {
		pf.open(paramFile, ios::in);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
			return(-1);
		}

		line = 0;
		nofParams = 0;
		while (nofParams < 16) {
			pLine.ReadLine(pf, kFALSE);
			line++;
			if (pf.eof()) break;
			pLine = pLine.Strip(TString::kBoth);
			if (pLine.Length() == 0) continue;						// empty line
			if (pLine(0) == '#') continue;							// comment
			Int_t ncmt = pLine.Index("#", 0);
			if (ncmt > 0) pLine.Resize(ncmt);
			pLine = pLine.Strip(TString::kBoth);
			pStr = pLine;
			pStr.ToInteger(pValue);				
			psaVal[nofParams] = pValue;
			nofParams++;
		}
		pf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << paramFile << " (should be .psa or .psadmp)" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
		return(-1);
	}

	if (UpdateDSP && !this->IsOffline()) {
		if (!this->CheckConnect("LoadPsaParams")) return(-1);
		pKey = fDGFData->FindParam("USERIN");
		if (pKey == NULL) {
			gMrbLog->Err() << "No such param - USERIN" << endl;
			gMrbLog->Flush(this->ClassName(), "LoadPsaParams");
			return(-1);
		}
		pOffset = pKey->GetIndex();
		for (Int_t i = 0; i < nofParams; i++) {
			this->SetParValue(pOffset + i, psaVal[i]);
		}
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofParams << " PSA params read from " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "LoadPsaParams", setblue);
	}

	return(nofParams);
}

Int_t TMrbDGF::SavePsaParams(const Char_t * ParamFile, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SavePsaParams
// Purpose:        Save PSA params to file
// Arguments:      Char_t * ParamFile   -- file name
//                 Bool_t ReadFromDSP   -- kTRUE if params to be read from DSP
// Results:        Int_t NofParams      -- number of params written
// Exceptions:
// Description:    Saves PSA params to file. Param start at USERIN
//                 Format is "unix style":
//                       <name>:<offset>:<value>:<hexval>
//                 Additional entries may be used to identify the module:
//                       FileType:par
//                       Module:<name>
//                       Revision:<rev>
//                       Crate:<crate>
//                       Station:<station>
//                       Cluster:<id>
//                       XiaRelease:<release>
//                       XiaDate:<date>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	UShort_t pValue;
	TString pName;
	TMrbNamedX * pKey;
	ofstream pf;
	Int_t nofParams;
	TString paramFile;
	TString paramPath;
	TMrbString info;
	TDatime dt;

	TMrbLofNamedX psaNames;
		
	psaNames.AddNamedX(kMrbPSANames);

	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	if (ReadFromDSP && !this->IsOffline()) {
		if (!this->CheckConnect("SaveParams")) return(-1);
		this->ReadParamMemory(kTRUE, kTRUE);
	}

	if (!this->ParamValuesRead()) {
		gMrbLog->Err() << "No param values read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	paramFile = ParamFile;
	if (paramFile.Index("/") != 0 && paramFile.Index("./", 0) != 0) {
		paramPath = gEnv->GetValue("TMrbDGF.SettingsPath", ".");
		paramFile = paramPath + "/";
		paramFile += ParamFile;
	}
	if (paramFile.Index(".psa") > 0) {
		pf.open(paramFile, ios::out);
		if (!pf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveParams");
			return(-1);
		}

		pf << "#----------------------------------------------------------------------------------------" << endl;
		pf << "# Name          : "								<< paramFile << endl;
		pf << "# Contents      : PSA settings for DGF-4C "		<< this->GetName()
																<< " in C" << this->GetCrate()
																<< ".N" << this->GetStation() << endl;
		pf << "# Creation date : " 								<< dt.AsString() << endl;
		pf << "# Created by    : " 								<< gSystem->Getenv("USER") << endl;
		pf << "#" << endl;
		pf << "# Data format   : <name>:<offset>:<value>:<hexval>" << endl;
		pf << "#----------------------------------------------------------------------------------------" << endl;
		pf << "#+FileType:psa" << endl;
		pf << "#+Module:"										<< this->GetName() << endl;
		pf << "#+Revision:" 									<< this->GetRevision()->GetName() << endl;
		pf << "#+Crate:"										<< this->GetCrate() << endl;
		pf << "#+Station:" 										<< this->GetStation() << endl;
		pf << "#+Cluster:"										<< this->GetClusterInfo(info) << endl;
		pf << "#+XiaRelease:"									<< gEnv->GetValue("TMrbDGF.XiaRelease", "") << endl;
		pf << "#+XiaDate:" 										<< gEnv->GetValue("TMrbDGF.XiaDate", "") << endl;
		pf << "#........................................................................................" << endl;
		pf << endl;
		nofParams = 0;
		pKey = fDGFData->FindParam("USERIN");
		if (pKey == NULL) {
			gMrbLog->Err() << "No such param - USERIN" << endl;
			gMrbLog->Flush(this->ClassName(), "SavePsaParams");
			return(-1);
		}

		pOffset = pKey->GetIndex();
		for (Int_t i = 0; i < 16; i++, pOffset++) {
			pValue = fParams[pOffset];
			pf << psaNames[i]->GetName() << ":" << pOffset << ":" << pValue << ":0x" << setbase(16) << pValue << setbase(10) << endl;
			nofParams++;
		}
		pf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << paramFile << " (should be .par)" << endl;
		gMrbLog->Flush(this->ClassName(), "SavePsaParams");
		return(-1);
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofParams << " PSA params written to " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "SavePsaParams", setblue);
	}

	return(nofParams);
}

Int_t TMrbDGF::SaveValues(const Char_t * ValueFile, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SaveValues
// Purpose:        Save high-level values to file
// Arguments:      Char_t * ParamFile   -- file name
//                 Bool_t ReadFromDSP   -- kTRUE if params to be read from DSP
// Results:        Int_t NofValues      -- number of values written
// Exceptions:
// Description:    Saves high-level values to file.
//                 Format is "unix style":
//                       <name>:<chn>:<value>:<unit>
//                 Additional entries may be used to identify the module:
//                       FileType:val
//                       Module:<name>
//                       Revision:<rev>
//                       Crate:<crate>
//                       Station:<station>
//                       Cluster:<id>
//                       XiaRelease:<release>
//                       XiaDate:<date>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName;
	ofstream vf;
	Int_t nofValues;
	TString valueFile;
	TString valuePath;
	TDatime dt;
	TString strVal;
	TMrbString info;
	
	if (!fDGFData->ParamNamesRead()) {
		gMrbLog->Err() << "No param names read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveValues");
		return(-1);
	}

	if (ReadFromDSP && !this->IsOffline()) {
		if (!this->CheckConnect("SaveValues")) return(-1);
		this->ReadParamMemory(kTRUE, kTRUE);
	}

	if (!this->ParamValuesRead()) {
		gMrbLog->Err() << "No param values read" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveValues");
		return(-1);
	}

	valueFile = ValueFile;
	if (valueFile.Index("/") != 0 && valueFile.Index("./", 0) != 0) {
		valuePath = gEnv->GetValue("TMrbDGF.SettingsPath", ".");
		valueFile = valuePath + "/";
		valueFile += ValueFile;
	}
	if (valueFile.Index(".val") > 0) {			// write ascii data
		vf.open(valueFile, ios::out);
		if (!vf.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << valueFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveParams");
			return(-1);
		}

		vf << "#----------------------------------------------------------------------------------------" << endl;
		vf << "# Name          : "								<< valueFile << endl;
		vf << "# Contents      : Settings for DGF-4C "			<< this->GetName()
																<< " in C" << this->GetCrate()
																<< ".N" << this->GetStation() << endl;
		vf << "# Creation date : " 								<< dt.AsString() << endl;
		vf << "# Created by    : " 								<< gSystem->Getenv("USER") << endl;
		vf << "#" << endl;
		vf << "# Data format   : <name>:<chn>:<value>:<unit>" << endl;
		vf << "#----------------------------------------------------------------------------------------" << endl;
		vf << "#+FileType:val" << endl;
		vf << "#+Module:"										<< this->GetName() << endl;
		vf << "#+Revision:"										<< this->GetRevision()->GetName() << endl;
		vf << "#+Crate:"										<< this->GetCrate() << endl;
		vf << "#+Station:" 										<< this->GetStation() << endl;
		vf << "#+Cluster:"										<< this->GetClusterInfo(info) << endl;
		vf << "#+XiaRelease:"									<< gEnv->GetValue("TMrbDGF.XiaRelease", "") << endl;
		vf << "#+XiaDate:" 										<< gEnv->GetValue("TMrbDGF.XiaDate", "") << endl;
		vf << "#........................................................................................" << endl;
		nofValues = 0;
		for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			vf << endl;
			vf << "# Channel " << chn << ": ............................................................................" << endl;
			vf << "Polarity:"		<< chn << ":"		<< this->GetTriggerPolarity(chn)			<< endl;	nofValues++;
			vf << "Tau:"			<< chn << ":"		<< this->GetTau(chn)			<< ":us"	<< endl;	nofValues++;
			vf << "Gain:"			<< chn << ":"		<< this->GetGain(chn)						<< endl;	nofValues++;
			vf << "Offset:"			<< chn << ":"		<< this->GetOffset(chn) 		<< ":V" 	<< endl;	nofValues++;
			vf << "SlowPeak:"		<< chn << ":"		<< this->GetPeakTime(chn)		<< ":us"	<< endl;	nofValues++;
			vf << "SlowGap:"		<< chn << ":"		<< this->GetGapTime(chn)		<< ":us"	<< endl;	nofValues++;
			vf << "FastPeak:"		<< chn << ":"		<< this->GetFastPeakTime(chn)	<< ":us"	<< endl;	nofValues++;
			vf << "FastGap:"		<< chn << ":"		<< this->GetFastGapTime(chn)	<< ":us"	<< endl;	nofValues++;
			vf << "Threshold:"		<< chn << ":"		<< this->GetThreshold(chn)					<< endl;	nofValues++;
			vf << "Delay:"			<< chn << ":"		<< this->GetDelay(chn)			<< ":us"	<< endl;	nofValues++;
		}
		vf.close();
	} else {
		gMrbLog->Err() << "Wrong file extension - " << valueFile << " (should be .val)" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(-1);
	}

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << nofValues << " values written to " << valueFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveValues", setblue);
	}

	return(nofValues);
}

Bool_t TMrbDGF::SaveParams(TArrayS & TempStorage) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SaveParams
// Purpose:        Save (input) params temporarily
// Arguments:      TArrayS & TempStorage    -- where to store param values
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Save params from DSP to internal storage.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	Int_t startAddr;
	Int_t idx;
	TArrayI cData;

	if (!this->CheckConnect("SaveParams")) return(kFALSE);
	if (!this->CheckActive("SaveParams")) return(kFALSE);
	if (!fDGFData->ParamNamesRead()) return(kFALSE);
	if (this->IsOffline()) return(kTRUE);

	nofParams = TMrbDGFData::kNofDSPInputParams;				 	// size of input section
	startAddr = TMrbDGFData::kDSPInparStartAddr;
	idx = 0;
		
	this->WriteTSAR(startAddr); 									// where to read from
	cData.Set(nofParams + 1);										// save params + switchbus

	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0 failed - DSPAddr=0x" << setbase(16) << TMrbDGFData::kDSPInparStartAddr
						<< setbase(10) << ", wc=" << nofParams << endl;
		gMrbLog->Flush(this->ClassName(), "SaveParams");
		return(kFALSE);
	}

	cData[nofParams] = this->GetSwitchBus();						// save switchbus separately

	this->CopyData(TempStorage, cData.GetArray(), nofParams + 1);
	return(kTRUE);
}

Bool_t TMrbDGF::SaveParams(EMrbDGFSaveIdx SaveIdx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SaveParams
// Purpose:        Save (input) params temporarily
// Arguments:      EMrbDGFSaveIdx SaveIdx    -- where to store param values
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Save params from DSP to internal storage.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (SaveIdx) {
		case kSaveTrace:			return(this->SaveParams(fTmpParTrace));
		case kSaveUntrigTrace:	return(this->SaveParams(fTmpParUntrigTrace));
		case kSaveDacRamp:		return(this->SaveParams(fTmpParDacRamp));
		case kSaveTaufit:			return(this->SaveParams(fTmpParTaufit));
		case kSaveAccuHist:		return(this->SaveParams(fTmpParAccuHist));
	}
	return(kFALSE);
}

Bool_t TMrbDGF::RestoreParams(TArrayS & TempStorage) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::RestoreParams
// Purpose:        Restore (input) params to DSP
// Arguments:      TArrayS & TempStorage    -- where to get param values from
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Restores DSP from temp storage.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofParams;
	TArrayI cData;

	if (!this->CheckConnect("WriteParamMemory")) return(kFALSE);
	if (!this->CheckActive("WriteParamMemory")) return(kFALSE);
	if (!this->ParamValuesRead()) return(kFALSE);
	if (this->IsOffline()) return(kTRUE);

	this->WriteTSAR(TMrbDGFData::kDSPInparStartAddr);				// where to write to
	nofParams = TMrbDGFData::kNofDSPInputParams;					// size of params section (input only)
	this->CopyData(cData, TempStorage.GetArray(), nofParams + 1);	// params + switchbus

	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(16), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F16 failed - DSPAddr=0x" << setbase(16) << TMrbDGFData::kDSPInparStartAddr
						<< setbase(10) << ", wc=" << nofParams << endl;
		gMrbLog->Flush(this->ClassName(), "WriteParamMemory");
		return(kFALSE);
	}
	this->SetSwitchBus(cData[nofParams], TMrbDGF::kBitSet);			// restore switchbus separately
	return(kTRUE);
}

Bool_t TMrbDGF::RestoreParams(EMrbDGFSaveIdx SaveIdx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::RestoreParams
// Purpose:        Restore (input) params to DSP
// Arguments:      EMrbDGFSaveIdx SaveIdx    -- where to restore params from
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Restores DSP from temp storage.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (SaveIdx) {
		case kSaveTrace:			return(this->RestoreParams(fTmpParTrace));
		case kSaveUntrigTrace:	return(this->RestoreParams(fTmpParUntrigTrace));
		case kSaveDacRamp:		return(this->RestoreParams(fTmpParDacRamp));
		case kSaveTaufit:			return(this->RestoreParams(fTmpParTaufit));
		case kSaveAccuHist:		return(this->RestoreParams(fTmpParAccuHist));
	}
	return(kFALSE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WriteICSR
//                 TMrbDGF::ReadICSR
//                 TMrbDGF::WriteCSR
//                 TMrbDGF::ReadCSR
//                 TMrbDGF::WriteTSAR
//                 TMrbDGF::ReadTSAR
//                 TMrbDGF::WriteWCR
//                 TMrbDGF::ReadWCR
// Purpose:        Camac I/O from/to DGF regs
// Arguments:      UInt_t Data   -- data to be written to DGF reg
// Results:        UInt_t Data   -- data read from DGF reg
// Exceptions:
// Description:    Performs i/o from/to dgf regs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbDGF::WriteICSR(UInt_t Data) {
// Write DGF Control Status Reg
	if (this->IsOffline()) return(kTRUE);
	Int_t cVal = (Int_t) Data;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(8), F(17), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A8.F17 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteICSR");
		return(kFALSE);
	}
	return(kTRUE);
}

UInt_t TMrbDGF::ReadICSR() {
// Read DGF Control Status Reg
	if (this->IsOffline()) return(0);
	Int_t cVal;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(8), F(1), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A8.F1 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadICSR");
		return(0xffffffff);
	}
	return((UInt_t) cVal & TMrbDGFData::kCamacICSRMask);
}

Bool_t TMrbDGF::WriteCSR(UInt_t Data) {
// Write DGF Control Status Reg
	if (this->IsOffline()) return(kTRUE);
	Int_t cVal = (Int_t) Data;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(17), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F17 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteCSR");
		return(kFALSE);
	}
	return(kTRUE);
}

UInt_t TMrbDGF::ReadCSR() {
// Read DGF Control Status Reg
	UInt_t csr;
	Int_t cVal;
	if (this->IsOffline()) return(0);
	if (!fCamac.ExecCnaf(fCrate, fStation, A(0), F(1), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A0.F1 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCSR");
		return(0xffffffff);
	}
	csr = (UInt_t) cVal & TMrbDGFData::kCamacCSRMask;
	if (csr & TMrbDGFData::kDSPError) {
		gMrbLog->Err() << "DSP error for module " << fName << " in C" << fCrate << ".N" << fStation << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCSR");
	}
	return(csr);
}

Bool_t TMrbDGF::WriteTSAR(UInt_t Addr) {
	UInt_t addr;
// Write DGF Transfer Start Addr Reg
	if (this->IsOffline()) return(kTRUE);
	Int_t cVal = (Int_t) Addr;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(17), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A1.F17 failed - DSPAddr=0x" << setbase(16) << Addr << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTSAR");
		return(kFALSE);
	}
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(1), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A1.F1 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTSAR");
		return(kFALSE);
	}
	addr = (UInt_t) cVal;
	if (Addr != addr) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< " - addr = " << setbase(16) << addr << ", should be " << Addr << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTSAR");
		return(kFALSE);
	}
	return(kTRUE);
}

UInt_t TMrbDGF::ReadTSAR() {
// Read DGF Transfer Start Addr Reg
	if (this->IsOffline()) return(0);
	Int_t cVal;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(1), F(1), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A1.F1 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTSAR");
		return(0xffffffff);
	}
	return((UInt_t) cVal);
}

Bool_t TMrbDGF::WriteWCR(Int_t Data) {
// Write DGF Word Count Reg
	if (this->IsOffline()) return(kTRUE);
	Int_t cVal = Data;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(17), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A2.F17 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteWCR");
		return(kFALSE);
	}
	return(kTRUE);
}

Int_t TMrbDGF::ReadWCR() {
// Read DGF Word Count Reg
	if (this->IsOffline()) return(0);
	Int_t cVal;
	if (!fCamac.ExecCnaf(fCrate, fStation, A(2), F(1), cVal, kTRUE)) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A2.F1 failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadWCR");
		return(-1);
	}
	return(cVal);
}

Int_t TMrbDGF::ReadEventBuffer(TMrbDGFEventBuffer & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadEventBuffer
// Purpose:        Copy event data from DSP to user
// Arguments:      TMrbDFGEventBuffer * Buffer    -- where to copy data
// Results:        Int_t NofWords                 -- number of data words
// Exceptions:
// Description:    Reads data from DGF's event buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofWords;
	Int_t wc;
	UInt_t addr;

	Buffer.Reset();

	if (!this->CheckConnect("ReadEventBuffer")) return(-1);
	if (!this->CheckActive("ReadEventBuffer")) return(-1);

	nofWords = this->ReadWCR();
	if (nofWords <= 0) {
		gMrbLog->Err() << fName << " in C" << fCrate << ".N" << fStation << ": Word count is zero" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEventBuffer");
		nofWords = 8192;
	}

	addr = this->GetParValue("AOUTBUFFER", kTRUE);
	wc = this->GetParValue("LOUTBUFFER", kTRUE);
	Buffer.Set(wc);
	this->WriteTSAR(addr);

	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), Buffer, 0, wc, kTRUE) == -1) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0: Reading histogram energies failed - DSPAddr=0x" << setbase(16) << addr
						<< setbase(10) << ", wc=" << wc << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEventBuffer");
		return(-1);
	}

	Buffer.MakeIndex();
	return(nofWords);
}

Bool_t TMrbDGF::CheckBufferSize(const Char_t * Method, Int_t BufSize) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckBufferSize
// Purpose:        Check buffer size

// Arguments:      Char_t * Method    -- calling method
//                 Int_t BufSize      -- buffer size
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if there is enough space in DSP's output buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bs = (Int_t) this->GetParValue("LOUTBUFFER", kTRUE);
	if (BufSize > bs) {
		gMrbLog->Err() << "Buffer overflow - " << BufSize << " words requested, " << bs << " available" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(kTRUE);
}
	
Bool_t TMrbDGF::StartRun(Bool_t ClearMCA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::StartRun
// Purpose:        DGF Run Control: Start run
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Checks status reg for active bit. If absent starts run.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csr;

	if (!this->CheckConnect("StartRun")) return(kFALSE);
	if (!this->CheckActive("StartRun")) return(kFALSE);
	csr = this->ReadCSR();
	if (csr == 0xffffffff) {
		gMrbLog->Err() << "Can't start " << this->GetTitle() << endl;
		gMrbLog->Flush(this->ClassName(), "StartRun");
		return(kFALSE);
	}
	if (ClearMCA) csr |= TMrbDGFData::kNewRun;
	this->WriteCSR(csr | TMrbDGFData::kRunEna);
	this->Wait();
	fStatusM &= ~TMrbDGF::kDSPAborted;
	fStatusM |= TMrbDGF::kDSPRunning;
	return(kTRUE);
}

Bool_t TMrbDGF::StopRun() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::StopRun
// Purpose:        DGF Run Control: Stop run
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Stops run and waits for active bit to disappear.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	UInt_t csr;

	if (!this->CheckConnect("StopRun")) return(kFALSE);

	csr = this->ReadCSR();
	if (csr != 0xffffffff) {
		this->WriteCSR(csr & ~(TMrbDGFData::kRunEna | TMrbDGFData::kNewRun));
		this->Wait();
		for (i = 0; i < TMrbDGF::kTimeout; i++) {
			csr = this->ReadCSR();
			if (csr == 0xffffffff) break;
			if ((csr & TMrbDGFData::kActive) == 0) {
				fStatusM &= ~(TMrbDGF::kDSPRunning | TMrbDGF::kDSPAborted);
				return(kTRUE);
			}
		}
	}
	gMrbLog->Err() << "Can't stop " << this->GetTitle() << " (RUNACTIVE bit stuck)" << endl;
	gMrbLog->Flush(this->ClassName(), "StopRun");
	return(kFALSE);
}

Double_t TMrbDGF::Rounded(Double_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::Rounded
// Purpose:        Return value rounded up
// Arguments:      Double_t Value       -- original value
// Results:        Double_t RValue      -- rounded value
// Exceptions:
// Description:    Rounds up a value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t v;
	if (Value == 0.) return(0.);
	v = fabs(Value) + 0.5;
	if (Value > 0.) return(v); else return(-v);
}

UInt_t * TMrbDGF::ToFixedPoint(Double_t Value, UInt_t * DspFixPt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ToFixedPoint
// Purpose:        Convert to DSP FixedPoint
// Arguments:      Double_t Value         -- value
//                 UInt_t * DspFixPt      -- where to store DSP data 
// Results:        UInt_t * DspFixPt      -- result
// Exceptions:
// Description:    Converts number to DSP fixed point format.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t valFloor;
	valFloor = floor(fabs(Value));
	DspFixPt[0] = (UShort_t) valFloor;
	DspFixPt[1] = (UShort_t) fabs(65536 * (Value - valFloor));
	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::ToFixedPoint(): Value=" << Value
				<< " --> high word [0]=" << DspFixPt[0] << " (0x" << setbase(16) << DspFixPt[0] << setbase(10)
				<< "), low word [1]=" << DspFixPt[1] << " (0x" << setbase(16) << DspFixPt[1] << setbase(10) << ")"
				<< endl;
	}
	return(DspFixPt);
}

Double_t TMrbDGF::FromFixedPoint(UInt_t * DspFixPt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::FromFixedPoint
// Purpose:        Convert from DSP FixedPoint
// Arguments:      UInt_t * DspFixPt      -- where to read DSP data from 
// Results:        Double_t Value         -- resulting value
// Exceptions:
// Description:    Converts number from DSP fixed point format.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t value;
	value = (Double_t) DspFixPt[1] / 65636 + (Double_t) DspFixPt[0];
#ifdef DEBUG
	cout	<< this->ClassName() << "::FromFixedPoint(): high word [0]="
			<< DspFixPt[0] << " (0x" << setbase(16) << DspFixPt[0] << setbase(10)
			<< "), low word [1]=" << DspFixPt[1] << " (0x" << setbase(16) << DspFixPt[1] << setbase(10) << ")"
			<< " --> Value = " << value << endl;
#endif
	return(value);
}

UInt_t * TMrbDGF::ToDouble(Double_t Value, UInt_t * DspDouble) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ToDouble
// Purpose:        Convert to DSP Double
// Arguments:      Double_t Value         -- value
//                 UInt_t * DspDouble     -- where to store DSP data 
// Results:        UInt_t * DspDouble     -- result
// Exceptions:
// Description:    Converts number to DSP double format.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sign;
	Double_t mantissa;
	Int_t exp;
	Int_t m0, m1;

	sign = kFALSE;

	if (Value == 0.) {
			DspDouble[0]=0;
			DspDouble[1]=0;
			DspDouble[2]=0;
			return(DspDouble);
	}

	if (Value < 0.) {
		sign = kTRUE;
		Value = -Value;
	}

	exp = (Int_t) this->Rounded(log(Value) / log(2.) + 0.5);
	mantissa = Value / pow(2., exp);
  
	m1 = (Int_t) (mantissa * pow(2., 15));
	m0 = (Int_t) (mantissa * pow(2., 31) - m1 * pow(2., 16) + 0.5);
  
	if ((sign == kFALSE) & ((m1 & 0x8000) != 0)) {
		m0 = m0 / 2 + (m1 % 2) * 0x8000;
		m1 = m1 / 2;
		exp++;
    } else if (sign == kTRUE) {
		m1 = 65535 - m1;
		m0 = 65535 - m0;
		m0++;
		if (m0 > 65535) {
			m0 = 0;
			m1++;
		}
	}

	DspDouble[0] = exp;
	DspDouble[1] = m1;
	DspDouble[2] = m0;

#ifdef DEBUG
	cout	<< this->ClassName() << "::ToDouble(): Value=" << Value
			<< " --> exp=" << exp
			<< ", m1=" << m1 << " (0x" << setbase(16) << m1 << setbase(10)
			<< "), m0=" << m0 << " (0x" << setbase(16) << m0 << setbase(10) << ")"
			<< endl;
#endif
	return(DspDouble);
}

Bool_t TMrbDGF::SetModCSRA(UInt_t Bits, EMrbBitOp BitOp, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetModCSRA
// Purpose:        Set bits in control/status reg
// Arguments:      UInt_t Bits          -- bit mask
//                 EMrbBitOp BitOp      -- set / or / clear / mask
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Manipulates bits in module control/status reg
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t reg;

	if (UpdateDSP && !this->CheckConnect("SetModCSRA")) return(kFALSE);

	switch (BitOp) {
		case TMrbDGF::kBitSet:
			this->SetParValue("MODCSRA", Bits, UpdateDSP);
			break;
		case TMrbDGF::kBitOr:
			reg = this->GetParValue("MODCSRA");
			reg |= Bits;
			this->SetParValue("MODCSRA", reg, UpdateDSP);
			break;
		case TMrbDGF::kBitClear:  
			reg = this->GetParValue("MODCSRA");
			reg &= ~Bits;
			this->SetParValue("MODCSRA", reg, UpdateDSP);
			break;
		case TMrbDGF::kBitMask:
			reg = this->GetParValue("MODCSRA");
			reg &= Bits;
			this->SetParValue("MODCSRA", reg, UpdateDSP);
			break;
		default:
			gMrbLog->Err() << "Illegal bit operator - " << BitOp << endl;
			gMrbLog->Flush(this->ClassName(), "SetModCSRA");
			return(kFALSE);
	}
	return(kTRUE);
}

UInt_t  TMrbDGF::GetModCSRA(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetModCSRA
// Purpose:        Read control/status reg
// Arguments:      Bool_t ReadFromDSP  -- kTRUE if data should be read from DSP
// Results:        UInt_t Bits         -- CSRA bits
// Exceptions:
// Description:    Returns current value of the channel control/status reg
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("GetModCSRA")) return(0xffffffff);
	return(this->GetParValue("MODCSRA", ReadFromDSP));
}

Bool_t TMrbDGF::SetChanCSRA(Int_t Channel, UInt_t Bits, EMrbBitOp BitOp, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetChanCSRA
// Purpose:        Set bits in control/status reg
// Arguments:      Int_t Channel       -- channel number
//                 UInt_t Bits          -- bit mask
//                 EMrbBitOp BitOp      -- set / or / clear / mask
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Manipulates bits in channel control/status reg
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t reg;

	if (UpdateDSP && !this->CheckConnect("SetChanCSRA")) return(kFALSE);
	if (!this->CheckChannel("SetChanCSRA", Channel)) return(kFALSE);

	switch (BitOp) {
		case TMrbDGF::kBitSet:
			this->SetParValue(Channel, "CHANCSRA", Bits, UpdateDSP);
			break;
		case TMrbDGF::kBitOr:
			reg = this->GetParValue(Channel, "CHANCSRA", UpdateDSP);
			reg |= Bits;
			this->SetParValue(Channel, "CHANCSRA", reg, UpdateDSP);
			reg = this->GetParValue(Channel, "CHANCSRA", UpdateDSP);
			break;
		case TMrbDGF::kBitClear:  
			reg = this->GetParValue(Channel, "CHANCSRA", UpdateDSP);
			reg &= ~Bits;
			this->SetParValue(Channel, "CHANCSRA", reg, UpdateDSP);
			break;
		case TMrbDGF::kBitMask:
			reg = this->GetParValue(Channel, "CHANCSRA", UpdateDSP);
			reg &= Bits;
			this->SetParValue(Channel, "CHANCSRA", reg, UpdateDSP);
			break;
		default:
			gMrbLog->Err() << "Illegal bit operator - " << BitOp << endl;
			gMrbLog->Flush(this->ClassName(), "SetChanCSRA");
			return(kFALSE);
	}
	return(kTRUE);
}

UInt_t  TMrbDGF::GetChanCSRA(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetChanCSRA
// Purpose:        Read control/status reg
// Arguments:      Int_t Channel       -- channel number
//                 Bool_t ReadFromDSP  -- kTRUE if data should be read from DSP
// Results:        UInt_t Bits         -- CSRA bits
// Exceptions:
// Description:    Returns current value of the channel control/status reg
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csra;

	if (ReadFromDSP && !this->CheckConnect("GetChanCSRA")) return(0xffffffff);
	if (!this->CheckChannel("GetChanCSRA", Channel)) return(0xffffffff);

	csra = this->GetParValue(Channel, "CHANCSRA", ReadFromDSP);
	return(csra);
}

Bool_t TMrbDGF::SetCoincPattern(UInt_t Pattern, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetCoincPattern
// Purpose:        Set coinc pattern bits
// Arguments:      Bool_t UpdateDSP    -- kTRUE if data should be written to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets module's coinc pattern.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("SetCoincPattern")) return(kFALSE);
	this->SetParValue("COINCPATTERN", Pattern, UpdateDSP);
	return(kTRUE);
}

UInt_t TMrbDGF::GetCoincPattern(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetCoincPattern
// Purpose:        Return coinc pattern bits
// Arguments:      Bool_t ReadFromDSP    -- kTRUE if data should be read from DSP
// Results:        UInt_t Pattern        -- pattern bits
// Exceptions:
// Description:    Returns module's coinc pattern.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("GetCoincPattern")) return(0);
	return(this->GetParValue("COINCPATTERN", ReadFromDSP));
}

Bool_t TMrbDGF::SetCoincWait(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetCoincWait
// Purpose:        Set coinc time in dsp
// Arguments:      Bool_t ReadFromDSP     -- kTRUE if value is to be read from DSP first
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Calculates coincidence time window and sets dsp value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t coincWait;

	if (!this->CheckConnect("SetCoincWait")) return(kFALSE);
	coincWait = this->CalcCoincWait(ReadFromDSP);
	if (coincWait == -1) return(kFALSE);
	return(this->SetParValue("COINCWAIT", coincWait, kTRUE));
}

Bool_t TMrbDGF::SetDelay(Int_t Channel, Double_t Delay, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetDelay
// Purpose:        Set channel delay
// Arguments:      Int_t Channel        -- channel number
//                 Double_t Delay       -- delay in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets delay for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t trgDelay = 0;
	Int_t pafLength, peakSep, peakSample, dec, dec2;

	cout << "@@ " << Channel << " " << Delay << endl;

	if (UpdateDSP && !this->CheckConnect("SetDelay")) return(kFALSE);
	if (!this->CheckChannel("SetDelay", Channel)) return(kFALSE);

    dec = this->GetParValue("DECIMATION");
	dec2 = 1 << dec;
    peakSep = this->GetParValue(Channel, "PEAKSEP");
    peakSample = this->GetParValue(Channel, "PEAKSAMPLE");
    
	trgDelay = (peakSample + 6) * dec2;

	pafLength = trgDelay + (Int_t) (Delay * 40 + .5) + 8;
	this->SetParValue(Channel, "PAFLENGTH", pafLength, UpdateDSP);
    this->SetParValue(Channel, "TRIGGERDELAY", trgDelay, UpdateDSP);
	cout << "@@ PAFLENGTH=" << pafLength << " TRIGGERDELAY=" << trgDelay << endl;
   
	if (gMrbDGFData->fVerboseMode) {
		cout 	<< this->ClassName() << "::SetDelay(): PEAKSEP" << Channel
				<< "=" << peakSep
				<< ", TRIGGERDELAY" << Channel << "=" << trgDelay
				<< ", PAFLENGTH" << Channel << "=" << pafLength
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetDelay(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDelay
// Purpose:        Recalculates channel delay from dgf params
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t Delay       -- delay in us
// Exceptions:
// Description:    Returns delay for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t trgDelay, pafLength;
	Double_t delayVal;

	if (ReadFromDSP && !this->CheckConnect("GetDelay")) return(-1);
	if (!this->CheckChannel("GetDelay", Channel)) return(-1);

	pafLength = this->GetParValue(Channel, "PAFLENGTH", ReadFromDSP);
    trgDelay = this->GetParValue(Channel, "TRIGGERDELAY", ReadFromDSP);
   
	delayVal = (Double_t) (pafLength - trgDelay - 8);
	return(delayVal / 40.);
}

Int_t TMrbDGF::SetGain(Int_t Channel, Double_t Gain, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetGain
// Purpose:        Set gain
// Arguments:      Int_t Channel        -- channel number
//                 Double_t Gain        -- gain
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        Int_t GainVal        -- converted GAINDAC value
// Exceptions:
// Description:    Sets GAINDAC for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dacVal;
	Int_t idacVal;

	if (UpdateDSP && !this->CheckConnect("SetGain")) return(-1);
	if (!this->CheckChannel("SetGain", Channel)) return(-1);

//	if (Gain < 0.01) return(32768); 	// gain negative?
	if (Gain < 0.1639) Gain = 0.1639;
	dacVal = 65535 - 32768 * log10(Gain / 0.1639);
	idacVal = (Int_t) (dacVal + 0.5);

	this->SetParValue(Channel, "GAINDAC", idacVal, UpdateDSP);
	return(idacVal);
}

Double_t TMrbDGF::GetGain(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetGain
// Purpose:        Recalculate gain value
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t Gain        -- gain value
// Exceptions:
// Description:    Returns gain for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t gainVal;

	if (ReadFromDSP && !this->CheckConnect("GetGain")) return(-1);
	if (!this->CheckChannel("GetGain", Channel)) return(-1);

	gainVal = (Double_t) (65535 - this->GetParValue(Channel, "GAINDAC", ReadFromDSP)) / 32768;
	gainVal = 0.1639 * pow(10., gainVal);
	return(gainVal);
}

Int_t TMrbDGF::SetOffset(Int_t Channel, Double_t Offset, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetOffset
// Purpose:        Set offset
// Arguments:      Int_t Channel        -- channel number
//                 Double_t Offset      -- offset
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        Int_t OffsetVal      -- converted TRACKDAC value
// Exceptions:
// Description:    Sets TRACKDAC for a given channel.
//                 Offset in Volts (after DAC amplification).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dacVal;
	Int_t idacVal;

	if (UpdateDSP && !this->CheckConnect("SetOffset")) return(0);
	if (!this->CheckChannel("SetOffset", Channel)) return(0);

	dacVal = 32768 - (Offset / 3) * 32768;
	idacVal = (UInt_t) dacVal;
	this->SetParValue(Channel, "TRACKDAC", idacVal, UpdateDSP);

	return(idacVal);
}

Double_t TMrbDGF::GetOffset(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetOffset
// Purpose:        Recalculate offset
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t Offset      -- offset value
// Exceptions:
// Description:    Returns offset for a given channel.
//                 Offset in Volts (after DAC amplification).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t offset;

	if (ReadFromDSP && !this->CheckConnect("GetOffset")) return(-1);
	if (!this->CheckChannel("GetOffset", Channel)) return(-1);

	offset = (Double_t) this->GetParValue(Channel, "TRACKDAC", ReadFromDSP);
	offset = 3 * (1 - offset / 32768);
	return(offset);
}

Bool_t TMrbDGF::SetTraceLength(Int_t Channel, Double_t TraceLength, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTraceLength
// Purpose:        Set trace length
// Arguments:      Int_t Channel        -- channel number
//                 Double_t TraceLength -- tracelength in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets TRACKDAC in us for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t traceLength;

	if (UpdateDSP && !this->CheckConnect("SetTraceLength")) return(kFALSE);
	if (!this->CheckChannel("SetTraceLength", Channel)) return(kFALSE);
	traceLength = (Int_t) (TraceLength * 40 +.5);
	this->SetParValue(Channel, "TRACELENGTH", traceLength);
	return(kTRUE);
}

Double_t TMrbDGF::GetTraceLength(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTraceLength
// Purpose:        Return trave length in us
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t TraceLength -- trace length in us
// Exceptions:
// Description:    Returns trace length for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t traceLength;

	if (ReadFromDSP && !this->CheckConnect("GetTraceLength")) return(-1);
	if (!this->CheckChannel("GetTraceLength", Channel)) return(-1);

	traceLength = (Double_t) this->GetParValue(Channel, "TRACELENGTH", ReadFromDSP);
	return(traceLength / 40);
}

Bool_t TMrbDGF::SetPSALength(Int_t Channel, Double_t PsaLength, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetPSALength
// Purpose:        Set psa length
// Arguments:      Int_t Channel        -- channel number
//                 Double_t PsaLength   -- psa length in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets PSALENGTH for given channel. Input is in us.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t psaLength;

	if (UpdateDSP && !this->CheckConnect("SetPSALength")) return(kFALSE);
	if (!this->CheckChannel("SetPSALength", Channel)) return(kFALSE);
	psaLength = (Int_t) (PsaLength * 40 +.5);
	this->SetParValue(Channel, "PSALENGTH", psaLength);
	return(kTRUE);
}

Double_t TMrbDGF::GetPSALength(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetPSALength
// Purpose:        Return psa length in us
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t PsaLength   -- psa length in us
// Exceptions:
// Description:    Returns psa length for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t psaLength;

	if (ReadFromDSP && !this->CheckConnect("GetPSALength")) return(-1);
	if (!this->CheckChannel("GetPSALength", Channel)) return(-1);

	psaLength = (Double_t) this->GetParValue(Channel, "PSALENGTH", ReadFromDSP);
	return(psaLength / 40);
}

Bool_t TMrbDGF::SetPSAOffset(Int_t Channel, Double_t PsaOffset, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetPSAOffset
// Purpose:        Set psa offset
// Arguments:      Int_t Channel        -- channel number
//                 Double_t PsaLength   -- psa length in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets PSAOFFSET for given channel. Input is in us.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t psaOffset;

	if (UpdateDSP && !this->CheckConnect("SetPSAOffset")) return(kFALSE);
	if (!this->CheckChannel("SetPSAOffset", Channel)) return(kFALSE);
	psaOffset = (Int_t) (PsaOffset * 40 +.5);
	this->SetParValue(Channel, "PSAOFFSET", psaOffset);
	return(kTRUE);
}

Double_t TMrbDGF::GetPSAOffset(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetPSAOffset
// Purpose:        Return psa offset in us
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t PsaOffset   -- psa length in us
// Exceptions:
// Description:    Returns psa offset for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t psaOffset;

	if (ReadFromDSP && !this->CheckConnect("GetPSAOffset")) return(-1);
	if (!this->CheckChannel("GetPSAOffset", Channel)) return(-1);

	psaOffset = (Double_t) this->GetParValue(Channel, "PSAOFFSET", ReadFromDSP);
	return(psaOffset / 40);
}

Bool_t TMrbDGF::UpdateSlowFilter(Int_t Channel, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::UpdateSlowFilter
// Purpose:        Update slow filter
// Arguments:      Int_t Channel        -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Update params responsible for slow filter.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t slowLength, slowGap, peakSep, pafLength, trigDelay, dec, dec2;
	Int_t peakSample;
	Double_t userDelay;

	if (UpdateDSP && !this->CheckConnect("UpdateSlowFilter")) return(kFALSE);
	if (!this->CheckChannel("UpdateSlowFilter", Channel)) return(kFALSE);

	slowLength = this->GetParValue(Channel, "SLOWLENGTH");
	slowGap = this->GetParValue(Channel, "SLOWGAP");
	pafLength = this->GetParValue(Channel, "PAFLENGTH");
	peakSep = this->GetParValue(Channel, "PEAKSEP");
	trigDelay = this->GetParValue(Channel, "TRIGGERDELAY");

	dec = this->GetParValue("DECIMATION");
	dec2 = 1 << dec;

	userDelay = (pafLength - 8 - trigDelay) * 0.025;	// extract user delay

	switch (dec) {
		case 0: peakSample = slowLength + slowGap - 7; if (peakSample < 0) peakSample = 0; break;
		case 1: peakSample = slowLength + slowGap - 4; if (peakSample < 2) peakSample = 2; break;
		case 2: peakSample = slowLength + slowGap - 2; break;
		default:
		case 3:
		case 4:
		case 6: peakSample = slowLength + slowGap - 1; break;
	}
	peakSep = peakSample + 5;
	if (peakSep > 33) peakSep = peakSample + 1;

	this->SetParValue(Channel, "PEAKSAMPLE", peakSample, UpdateDSP);	// set new values
	this->SetParValue(Channel, "PEAKSEP", peakSep, UpdateDSP);

	this->SetDelay(Channel, userDelay, UpdateDSP);		// re-calculate user delay

	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::UpdateSlowFilter(): PEAKSAMPLE" << Channel << "=" << peakSample
				<< ", PEAKSEP" << Channel << "=" << peakSep
				<< ", SLOWLENGTH" << Channel << "=" << slowLength
				<< ", SLOWGAP" << Channel << "=" << slowGap
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetDeltaT(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDeltaT
// Purpose:        Get time granularity
// Arguments:      Bool_t ReadFromDSP     -- kTRUE if value is to be read from DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Calculates time granularity.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dt; 
	Int_t dec;

	if (ReadFromDSP && !this->CheckConnect("GetDeltaT")) return(-1);

	dec = this->GetParValue("DECIMATION", ReadFromDSP);
	dt = (1 << dec) * 0.025;				// deltaT
	return(dt);
}

Int_t TMrbDGF::CalcCoincWait(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CalcCoincWait
// Purpose:        Calculate coinc time window
// Arguments:      Bool_t ReadFromDSP     -- kTRUE if value is to be read from DSP
// Results:        Int_t CoincWait        -- coinc window in units of 25ns
// Exceptions:
// Description:    Calculates coincidence time window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;
	Int_t dec;
	Int_t coincWait;
	Int_t pt;

	if (ReadFromDSP && !this->CheckConnect("CalcCoincWait")) return(-1);

	dec = this->GetParValue("DECIMATION", ReadFromDSP);

	coincWait = (Int_t) ceil(1.5 * (1 << dec) - 70);
	if (coincWait < 1) coincWait = 1;

	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) { 	// calculate corrections for each channel
		if (this->IsGoodChannel(chn)) {
			pt =	this->GetParValue(chn, "PAFLENGTH", ReadFromDSP) -
					this->GetParValue(chn, "TRIGGERDELAY", ReadFromDSP) - 70;
			if (pt > coincWait) coincWait = pt;
		}
	}
	return(coincWait);
}

Bool_t TMrbDGF::ResetPGTime(Int_t Channel, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ResetPGTime
// Purpose:        Reset peak and gap time
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Resets peak and gap time values to 0.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("ResetPGTime")) return(kFALSE);
	if (!this->CheckChannel("ResetPGTime", Channel)) return(kFALSE);

	this->SetParValue(Channel, "SLOWLENGTH", 0, UpdateDSP);
	this->SetParValue(Channel, "SLOWGAP", 0, UpdateDSP);
	return(kTRUE);
}

Bool_t TMrbDGF::SetPeakTime(Int_t Channel, Double_t PeakTime, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetPeakTime
// Purpose:        Set peak time
// Arguments:      Int_t Channel        -- channel number
//                 Double_t PeakTime    -- length in ns
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets peak time (length of slow filter) for a channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dt, peakTime; 
	Int_t slowLength, slowGap, dec;

	if (UpdateDSP && !this->CheckConnect("SetPeakTime")) return(kFALSE);
	if (!this->CheckChannel("SetPeakTime", Channel)) return(kFALSE);

	dec = this->GetParValue("DECIMATION");

	dt = (1 << dec) * 0.025;				// deltaT
	slowGap = this->GetParValue(Channel, "SLOWGAP");
	slowLength = (Int_t) (PeakTime / dt + 0.5);
  
	if (slowLength + slowGap > 31) slowLength = 31 - slowGap;	// has to be < 32
	this->SetParValue(Channel, "SLOWLENGTH", slowLength, UpdateDSP);

	this->UpdateSlowFilter(Channel, UpdateDSP);

	peakTime = slowLength * dt;

	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetPeakTime(): deltaT=" << dt
				<< ", SLOWLENGTH" << Channel << "=" << slowLength
				<< ", PeakTime=" << peakTime
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetPeakTime(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetPeakTime
// Purpose:        Recalculate peak time
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t PeakTime    -- peak time, rounded up
// Exceptions:
// Description:    Returns peak time (length of slow filter) for a channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dt, peakTime; 
	Int_t slowLength, dec;

	if (ReadFromDSP && !this->CheckConnect("GetPeakTime")) return(-1);
	if (!this->CheckChannel("GetPeakTime", Channel)) return(-1);

	dec = this->GetParValue("DECIMATION");
	dt = (1 << dec) * 0.025;				// deltaT
	slowLength = this->GetParValue(Channel, "SLOWLENGTH", ReadFromDSP);
	peakTime = slowLength * dt;
	return(peakTime);
}

Bool_t TMrbDGF::SetGapTime(Int_t Channel, Double_t GapTime, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetGapTime
// Purpose:        Set gap time
// Arguments:      Int_t Channel        -- channel number
//                 Double_t GapTime     -- length in ns
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets gap time for a channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dt, gapTime; 
	Int_t slowLength, slowGap, dec;

	if (UpdateDSP && !this->CheckConnect("SetGapTime")) return(kFALSE);
	if (!this->CheckChannel("SetGapTime", Channel)) return(kFALSE);

	dec = this->GetParValue("DECIMATION");

	dt = (1 << dec) * 0.025;						// deltaT

	slowLength = this->GetParValue(Channel, "SLOWLENGTH");
	slowGap = (Int_t) (GapTime / dt + 0.5);
	if (slowLength + slowGap > 31) slowGap = 31 - slowLength;

	this->SetParValue(Channel, "SLOWGAP", slowGap, UpdateDSP);

	this->UpdateSlowFilter(Channel, UpdateDSP);

	gapTime = dt * slowGap;
	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetGapTime(): deltaT=" << dt
				<< ", SLOWGAP" << Channel << "=" << slowGap
				<< ", GapTime=" << gapTime
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetGapTime(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetGapTime
// Purpose:        Recalculate gap time
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Double_t GapTime     -- gap time
// Exceptions:
// Description:    Returns gap time for a channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dt, gapTime; 
	Int_t slowGap, dec;

	if (ReadFromDSP && !this->CheckConnect("GetGapTime")) return(-1);
	if (!this->CheckChannel("GetGapTime", Channel)) return(-1);

	dec = this->GetParValue("DECIMATION");
	dt = (1 << dec) * 0.025;						// deltaT
	slowGap = this->GetParValue(Channel, "SLOWGAP", ReadFromDSP);
	gapTime = slowGap * dt;
	return(gapTime);
}

Bool_t TMrbDGF::ResetFastPGTime(Int_t Channel, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ResetFastPGTime
// Purpose:        Reset fast peak and gap times
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Resets fast peak and gap time values to 0.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("ResetFastPGTime")) return(kFALSE);
	if (!this->CheckChannel("ResetFastPGTime", Channel)) return(kFALSE);

	this->SetParValue(Channel, "FASTLENGTH", 0, UpdateDSP);
	this->SetParValue(Channel, "FASTGAP", 0, UpdateDSP);
	this->SetParValue(Channel, "MINWIDTH", 0, UpdateDSP);
	return(kTRUE);
}

void TMrbDGF::SetFastFilter(Int_t Channel, Int_t Length, Int_t Gap, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetFastFilter
// Purpose:        Set fast filter
// Arguments:      Int_t Channel        -- channel number
//                 Int_t Length         -- length
//                 Int_t Gap            -- gap
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        --
// Exceptions:
// Description:    Sets fast filter params.
//                 Method obsolete, replaced by SetFastPeakTime/SetFastGapTime.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("SetFastFilter")) return;
	if (!this->CheckChannel("SetFastFilter", Channel)) return;

	if (Gap + Length > 31) Gap = 31 - Length;
   
	this->SetParValue(Channel, "FASTLENGTH", Length, UpdateDSP);
	this->SetParValue(Channel, "FASTGAP", Gap, UpdateDSP);
	this->SetParValue(Channel, "MINWIDTH", Gap + Length, UpdateDSP);

	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetFastFilter(): FASTLENGTH" << Channel << "=" << Length
				<< ", FASTGAP" << Channel << "=" << Gap
				<< endl;
	}
}

Bool_t TMrbDGF::SetFastPeakTime(Int_t Channel, Double_t PeakTime, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetFastPeakTime
// Purpose:        Set fast filter peaking time
// Arguments:      Int_t Channel        -- channel number
//                 Double_t PeakTime    -- peaking time in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets fast filter peaking time.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t length, gap;

	if (UpdateDSP && !this->CheckConnect("SetFastPeakTime")) return(kFALSE);
	if (!this->CheckChannel("SetFastPeakTime", Channel)) return(kFALSE);

	gap = this->GetParValue(Channel, "FASTGAP");
	length = (Int_t) (PeakTime * 40. + .5);
//	if (length < 1) length = 1;
//	if (length > 4) length = 4;
//	if (gap + length > 31) gap = 31 - length;
	if (gap + length > 31) length = 31 - gap;
   
	this->SetParValue(Channel, "FASTLENGTH", length, UpdateDSP);
	this->SetParValue(Channel, "FASTGAP", gap, UpdateDSP);
	this->SetParValue(Channel, "MINWIDTH", gap + length, UpdateDSP);

	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetFastPeakTime(): FASTLENGTH" << Channel << "=" << length
				<< ", FASTGAP" << Channel << "=" << gap
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetFastPeakTime(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetFastPeakTime
// Purpose:        Return fast filter peaking time
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if value is to read form DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Returns current fast filter peaking time.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t length;
	Double_t peakTime;

	if (ReadFromDSP && !this->CheckConnect("GetFastPeakTime")) return(-1);
	if (!this->CheckChannel("GetFastPeakTime", Channel)) return(-1);

	length = this->GetParValue(Channel, "FASTLENGTH", ReadFromDSP);
	peakTime = ((Double_t) length) / 40;
	return(peakTime);
}

Bool_t TMrbDGF::SetFastGapTime(Int_t Channel, Double_t GapTime, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetFastGapTime
// Purpose:        Set fast filter gap time
// Arguments:      Int_t Channel        -- channel number
//                 Double_t GapTime     -- peaking time in us
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets fast filter gap time.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t length, gap;

	if (UpdateDSP && !this->CheckConnect("SetFastGapTime")) return(kFALSE);
	if (!this->CheckChannel("SetFastGapTime", Channel)) return(kFALSE);

	length = this->GetParValue(Channel, "FASTLENGTH");
	gap = (Int_t) (GapTime * 40. + .5);
//	if (gap < 1) gap = 1;
//	if (gap > 4) gap = 4;
//	if (gap + length > 31) length = 31 - gap;
	if (gap + length > 31) gap = 31 - length;
   
	this->SetParValue(Channel, "FASTLENGTH", length, UpdateDSP);
	this->SetParValue(Channel, "FASTGAP", gap, UpdateDSP);
	this->SetParValue(Channel, "MINWIDTH", gap + length, UpdateDSP);

	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetFastGapTime(): FASTLENGTH" << Channel << "=" << length
				<< ", FASTGAP" << Channel << "=" << gap
				<< endl;
	}
	return(kTRUE);
}

Double_t TMrbDGF::GetFastGapTime(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetFastGapTime
// Purpose:        Return fast filter gap time
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if value is to read form DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Returns current fast filter gap time.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intGap;
	Double_t gapTime;

	if (ReadFromDSP && !this->CheckConnect("GetFastGapTime")) return(-1);
	if (!this->CheckChannel("GetFastGapTime", Channel)) return(-1);

	intGap = this->GetParValue(Channel, "FASTGAP", ReadFromDSP);
	gapTime = ((Double_t) intGap) / 40;
	return(gapTime);
}

Bool_t TMrbDGF::SetThreshold(Int_t Channel, Int_t ADCThresh, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetThreshold
// Purpose:        Set threshold
// Arguments:      Int_t Channel        -- channel number
//                 Int_t ADCThresh      -- threshold (ADC counts)
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets threshold.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t threshVal, fastLength;

	if (UpdateDSP && !this->CheckConnect("SetThreshold")) return(kFALSE);
	if (!this->CheckChannel("SetThreshold", Channel)) return(kFALSE);

	fastLength = this->GetParValue(Channel, "FASTLENGTH");

	this->SetParValue(Channel, "FASTADCTHR", ADCThresh, UpdateDSP);
	threshVal = ADCThresh * fastLength;
	this->SetParValue(Channel, "FASTTHRESH", threshVal);
 
	if (gMrbDGFData->fVerboseMode) {
		cout	<< this->ClassName() << "::SetThreshold(): FASTTHRESH"
				<< Channel << "=" << threshVal
				<< endl;
	}
	return(kTRUE);
}  

Int_t TMrbDGF::GetThreshold(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetThreshold
// Purpose:        Set threshold
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if param to be read from DSP
// Results:        Int_t Threshold      -- threshold
// Exceptions:
// Description:    Returns threshold value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("GetThreshold")) return(-1);
	if (!this->CheckChannel("GetThreshold", Channel)) return(-1);

	return(this->GetParValue(Channel, "FASTADCTHR", ReadFromDSP));
}  

Bool_t TMrbDGF::SetTau(Int_t Channel, Double_t Tau, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTau
// Purpose:        Set tau
// Arguments:      Int_t Channel        -- channel number
//                 Double Tau           -- tau
//                 Bool_t UpdateDSP     -- kTRUE if DSP is to be updated
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets preamp tau.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t PreAmpTau[2];

	if (UpdateDSP && !this->CheckConnect("SetTau")) return(kFALSE);
	if (!this->CheckChannel("SetTau", Channel)) return(kFALSE);

	this->ToFixedPoint(Tau, PreAmpTau);

	this->SetParValue(Channel, "PREAMPTAUA", PreAmpTau[0], UpdateDSP);
	this->SetParValue(Channel, "PREAMPTAUB", PreAmpTau[1], UpdateDSP);
	return(kTRUE);
}

Double_t TMrbDGF::GetTau(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTau
// Purpose:        Get tau value
// Arguments:      Int_t Channel        -- channel number
//                 Bool_t ReadFromDSP   -- kTRUE if value is to be read from DSP
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Returns preamp tau.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t PreAmpTau[2];
	Double_t tau;

	if (ReadFromDSP && !this->CheckConnect("GetTau")) return(kFALSE);
	if (!this->CheckChannel("GetTau", Channel)) return(kFALSE);

	PreAmpTau[0] = this->GetParValue(Channel, "PREAMPTAUA", ReadFromDSP);
	PreAmpTau[1] = this->GetParValue(Channel, "PREAMPTAUB", ReadFromDSP);

	tau = this->FromFixedPoint(PreAmpTau);
	return(tau);
}

Bool_t TMrbDGF::SetBinning(Int_t Channel, Int_t EnBin, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetBinning
// Purpose:        Set binning
// Arguments:      Int_t Channel       -- channel number
//                 Int_t EnBin         -- energy binning
//                 Bool_t UpdateDSP    -- kTRUE if DSP is to be updated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets binning values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t val;

	if (UpdateDSP && !this->CheckConnect("SetBinning")) return(kFALSE);
	if (!this->CheckChannel("SetBinning", Channel)) return(kFALSE);

	val = (EnBin == 0) ? 0 : (65536 - EnBin);
	this->SetParValue(Channel, "LOG2EBIN", val);

	return(kTRUE);
}

Int_t TMrbDGF::ReadHistograms(TMrbDGFHistogramBuffer & Buffer, UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadHistogram
// Purpose:        Read MCA data
// Arguments:      TMrbDGFHistogramBuffer   -- histogram buffer
//                 UInt_t ChannelPattern    -- pattern of active channels
// Results:        Int_t NofWords           -- number of words read
// Exceptions:
// Description:    Reads MCA data from DSP. Use ESONE server
//                 For XiaRelease = 2.0 (32k memory)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i, j, k, chn;
	UInt_t addr;
	Int_t wc;
	Int_t nofChannels;
	Int_t data;
	Int_t cTask;
	Int_t nofWords;
	TArrayI cData;
		
	if (!this->CheckConnect("ReadHistograms")) return(-1);
	if (!this->CheckActive("ReadHistograms")) return(-1);

	Buffer.Reset(); 						// reset buffer layout
	nofChannels = 0;						// calc number of DGF channels in use
	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			Buffer.SetActive(chn, nofChannels);
			nofChannels++;
		}
	}
	Buffer.SetNofChannels(nofChannels);

	addr = this->GetParValue("AOUTBUFFER");
	wc = this->GetParValue("LOUTBUFFER");
	cData.Set(wc);

	nofWords = TMrbDGFData::kMCAPageSize * nofChannels;
	Buffer.SetSize(nofWords);		// size of energy region
	Buffer.Set(nofWords);			// reset data region
	
	Int_t fc = fCamac.HasFastCamac();
	if (fc == -1) fc = 0;

	k = 0;			// buffer index
	nofWords = 0;
	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (Buffer.IsActive(chn)) {
			this->SetParValue("HOSTIO", chn, kTRUE);
			for (j = 0; j < 8; j++) {
				this->SetParValue("RUNTASK", TMrbDGFData::kRunControl, kTRUE);		// reprogram DACs
				cTask = (j == 0) ? TMrbDGFData::kReadHistoFirstPage : TMrbDGFData::kReadHistoNextPage;
				this->SetParValue("CONTROLTASK", cTask, kTRUE); 	// xfer histo page to i/o buffer
				this->StartRun();
				this->WaitActive(1);								// wait for xfer to complete
				this->WriteTSAR(addr); 							// read data from i/o buffer
				if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, wc, kTRUE) == -1) {	// xfer energies, 2 * 16 bits
					gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
									<< ".A0.F0: Reading histogram energies failed - DSPAddr=0x" << setbase(16) << addr
									<< setbase(10) << ", wc=" << wc << endl;
					gMrbLog->Flush(this->ClassName(), "ReadHistograms");
					return(-1);
				}
				nofWords += wc / 2;
				data = 0;
				for (i = 0; i < wc; i++) {
					if (i & 1) {
						data |= cData[i];
						Buffer[k] = (Int_t) data;
						k++;
					} else {
						data = cData[i] << 8;
					}
				}
			}
		}
	}
	return(nofWords);
}

Int_t TMrbDGF::ReadHistogramsViaRsh(TMrbDGFHistogramBuffer & Buffer, UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ReadHistogramsViaRsh
// Purpose:        Read MCA data
// Arguments:      TMrbDGFHistogramBuffer   -- histogram buffer
//                 UInt_t ChannelPattern    -- pattern of active channels
// Results:        Int_t NofWords           -- number of words read
// Exceptions:
// Description:    Reads MCA data via remote shell.
//                 For XiaRelease = 2.0 (32k memory)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;
	Int_t nofChannels;
	Int_t nofWords;
		
	if (!this->CheckConnect("ReadHistogramsViaRsh")) return(-1);
	if (!this->CheckActive("ReadHistogramsViaRsh")) return(-1);

	Buffer.Reset(); 						// reset buffer layout
	nofChannels = 0;						// calc number of DGF channels in use
	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			Buffer.SetActive(chn, nofChannels);
			nofChannels++;
		}
	}
	Buffer.SetNofChannels(nofChannels);

	nofWords = TMrbDGFData::kMCAPageSize * nofChannels;
	Buffer.SetSize(nofWords);		// size of energy region
	Buffer.Set(nofWords);			// reset data region
	
	TString datFile = this->GetName();			// store binary data in file <dgf>.histoDump.dat
	datFile += ".histoDump.dat";

	TString hdPgm = gEnv->GetValue("TMrbDGF.ProgramToDumpHistos", "/nfs/mbssys/standalone/histoDump");
	TString hdPath = gSystem->Getenv("PWD");
	if (hdPath.IsNull()) hdPath = gSystem->WorkingDirectory();
	TString cmd = Form("rsh %s 'cd %s; %s %d %d %#x %s b'",
							fCamacHost.Data(),
							hdPath.Data(),
							hdPgm.Data(),
							this->GetCrate(),
							this->GetStation(),
							ChannelPattern,
							datFile.Data());
	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out()	<< "Calling program \"" << fCamacHost << ":" << hdPgm << "\" via rsh" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh", setblue);
		gMrbLog->Out()	<< ">> " << cmd << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh", setblue);
	}
	gSystem->Exec(cmd.Data());

	Bool_t fileOk = kFALSE;
	TString hsFile = ".";
	hsFile += datFile;
	hsFile += ".ok";
	for (Int_t i = 0; i < 100; i++) {			// wait for handshake file
		if (!gSystem->AccessPathName(hsFile.Data(), (EAccessMode) F_OK)) {
			fileOk = kTRUE;
			break;
		}
		usleep(1000);
	}
	if (!fileOk) {
		gMrbLog->Err()	<< "Handshake error - can't open file " << hsFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh");
		return(-1);
	}
	TEnv * hdEnv = new TEnv(hsFile.Data());
	if (hdEnv->GetValue("HistoDump.Errors", 0) != 0) {
		gMrbLog->Err()	<< "Reading MCA data failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh");
		delete hdEnv;
		return(-1);
	}
	nofWords = hdEnv->GetValue("HistoDump.NofDataWords", 0);
	if (nofWords != nofChannels * 32 * 1024) {
		gMrbLog->Err()	<< "Number of data words unexpected -  " << nofWords
						<< " (should be " << nofChannels << " * 32k = " << nofChannels * 32 * 1024 << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh");
		delete hdEnv;
		return(-1);
	}
	FILE * f = fopen(datFile.Data(), "r");
	if (f == NULL) {
		gMrbLog->Err() << gSystem->GetError() << " - " << datFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadHistogramsViaRsh");
	}
	fread(Buffer.GetArray(), nofChannels * 32 * 1024 * sizeof(UInt_t), 1, f);
	for (Int_t i = 0; i < nofChannels * 32 * 1024; i++) {
		Int_t d = Buffer[i];
		Int_t res  = (d >> 24) & 0xff;
		res += (d >> 8)  & 0xff00;
		res += (d << 8)  & 0xff0000;
		res += (d << 24) & 0xff000000;
		Buffer[i] = res;
	}
	delete hdEnv;
	return(nofWords);
}

Bool_t TMrbDGF::AccuHistograms(Int_t Time, const Char_t * Scale, UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::AccuHistograms
// Purpose:        Accumulate MCA histogram
// Arguments:      Int_t Time             -- measuring time
//                 Char_t * Scale         -- time scale ("s", "m", "h")
//                 UInt_t ChannelPattern  -- pattern of active channels
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Starts a MCA accumulation.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString tScale;
	Int_t waitInv;
	Int_t secsToWait;
	
	TMrbLofNamedX timeScale;
	TMrbNamedX * tp;

	if (!this->CheckConnect("AccuHistograms")) return(kFALSE);
	if (!this->CheckChannelMask("AccuHistograms")) return(kFALSE);

	timeScale.AddNamedX(kMrbAccuTimeScale);
	tp = timeScale.FindByName(Scale, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (tp == NULL) {
		gMrbLog->Err() << "Illegal time scale - " << Scale
				<< " (should be \"secs\", \"mins\", or \"hours\")" << endl;
		gMrbLog->Flush(this->ClassName(), "AccuHistograms");
		return(kFALSE);
	}
	tScale = tp->GetName();
	waitInv = tp->GetIndex();
	secsToWait = Time * waitInv;
	
	if (!this->AccuHist_Init(ChannelPattern)) return(kFALSE);
	if (!this->AccuHist_Start()) return(kFALSE);
	if (!this->AccuHist_Stop(secsToWait)) return(kFALSE);
	this->RestoreParams(kSaveAccuHist);
	return(kTRUE);
}

Bool_t TMrbDGF::AccuHist_Init(UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::AccuHist_Init
// Purpose:        Initialize histogramming
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;
	UInt_t csr;
	Bool_t sts;

	if (!this->CheckConnect("AccuHistograms")) return(kFALSE);

	this->SaveParams(kSaveAccuHist);

	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			csr =	TMrbDGFData::kEnableTrigger | TMrbDGFData::kCorrBallDeficit | TMrbDGFData::kHistoEnergies;
			this->SetChanCSRA(chn, csr, TMrbDGF::kBitOr, kTRUE);
		} else {
			this->SetChanCSRA(chn, TMrbDGFData::kChanCSRAMask, TMrbDGF::kBitClear, kTRUE);
		}
		this->SetParValue(chn, "TRACELENGTH", 64);
	}

 	this->SetParValue("MAXEVENTS", 0);
 	this->SetParValue("SYNCHWAIT", 0);
 	this->SetParValue("INSYNCH", 1);
 	this->SetParValue("COINCPATTERN", 0xffff);
 	this->SetParValue("COINCWAIT", 40);

	sts = this->WriteParamMemory(kTRUE);
	if (!sts) this->RestoreParams(kSaveAccuHist);
	return(sts);
}

Bool_t TMrbDGF::AccuHist_Start() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::AccuHist_Start
// Purpose:        Start histogramming
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	this->SetParValue("RUNTASK", TMrbDGFData::kRunMCA, kTRUE);
	this->StartRun();			// start & clear MCA
	fStatusW = TMrbDGF::kWaitActive;
	return(kTRUE);
}

Bool_t TMrbDGF::AccuHist_Stop(Int_t SecsToWait) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::AccuHist_Stop
// Purpose:        Stop histogramming
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (SecsToWait > 0) {
		for (Int_t i = 0; i < SecsToWait; i++) {
			sleep(1);
			gSystem->ProcessEvents();
			if (this->IsAborted()) {
				gMrbLog->Err() << "Aborted after " << i << " secs. Stopping current run." << endl;
				gMrbLog->Flush(this->ClassName(), "AccuHistograms");
				fStatusW = TMrbDGF::kWaitAborted;
				break;
			}
		}
	}
	this->StopRun();
	this->RestoreParams(kSaveAccuHist);
	return(kTRUE);
}

Int_t TMrbDGF::GetTrace(TMrbDGFEventBuffer & Buffer, Int_t TraceLength,
								UInt_t ChannelPattern, Int_t XwaitStates, Bool_t AutoTrigFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTrace
// Purpose:        Perform tracing
// Arguments:      TMrbDGFEventBuffer & Buffer    -- where to store trace data
//                 Int_t TraceLength              -- trace length
//                 UInt_t ChannelPattern          -- channel pattern
//                 Int_t XwaitStates              -- extra wait states
//                 Bool_t AutoTrigFlag            -- kTRUE: enable trigger for each active channel
//                                                -- kFALSE: use trigger settings as found
// Results:        Int_t NofTraceData             -- number of trace data taken
// Exceptions:
// Description:    Get trace from module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->GetTrace_Init(TraceLength, ChannelPattern, XwaitStates, AutoTrigFlag)) return(-1);
	if (!this->GetTrace_Start()) return(-1);
	return(this->GetTrace_Stop(Buffer));
}

Bool_t TMrbDGF::GetTrace_Init(Int_t TraceLength, UInt_t ChannelPattern, Int_t XwaitStates, Bool_t AutoTrigFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTrace_Init
// Purpose:        Initialize tracing
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	Int_t nofWords;
	Bool_t sts;
	Int_t nofChannels;
	
	if (!this->CheckConnect("GetTrace")) return(kFALSE);
	
	this->SaveParams(TMrbDGF::kSaveTrace);			// save current settings
	
	this->SetParValue("MAXEVENTS", 1);
 	this->SetParValue("SYNCHWAIT", 0);
 	this->SetParValue("INSYNCH", 1);
 	this->SetParValue("COINCPATTERN", 0xffff);
 	this->SetCoincWait();
  
	SetParValue("MODCSRB", 0);
	for (Int_t i = 0; i < TMrbDGFData::kNofChannels; i++) SetParValue(i, "CHANCSRB", 0);
	nofChannels = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			if (AutoTrigFlag) {
				this->SetChanCSRA(chn,	TMrbDGFData::kEnableTrigger |
												TMrbDGFData::kGoodChannel
												| TMrbDGFData::kGroupTriggerOnly
												| TMrbDGFData::kReadAlways,
												TMrbDGF::kBitOr, kTRUE);
				this->SetChanCSRA(chn,	TMrbDGFData::kGFLTValidate,
												TMrbDGF::kBitClear, kTRUE);
			}
			this->SetParValue(chn, "TRACELENGTH", TraceLength);
			this->SetParValue(chn, "XWAIT", XwaitStates);
			nofChannels++;
		} else {
			this->SetChanCSRA(chn,	TMrbDGFData::kEnableTrigger |
											TMrbDGFData::kGoodChannel,
											TMrbDGF::kBitClear, kTRUE);
			this->SetParValue(chn, "TRACELENGTH", 0);
			this->SetParValue(chn, "XWAIT", 0);
		}
	}

#if 0
	Int_t ngc = this->GetNofGoodChannels();
	if (ngc != nofChannels) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": Wrong number of good channels - "
						<< ngc << " (should be " << nofChannels << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTrace");
		this->RestoreParams(TMrbDGF::kSaveTrace);
		return(kFALSE);
	}
#endif

	nofWords = TMrbDGFEventBuffer::kBufHeaderLength + TMrbDGFEventBuffer::kEventHeaderLength;
	nofWords += nofChannels * (TMrbDGFEventBuffer::kChanHeaderLength + TraceLength);

	if (!this->CheckBufferSize("GetTrace", nofWords)) {
		this->RestoreParams(TMrbDGF::kSaveTrace);
		return(kFALSE);
	}

	sts = this->WriteParamMemory();
	if (!sts) this->RestoreParams(TMrbDGF::kSaveTrace);
	return(sts);
}

Bool_t TMrbDGF::GetTrace_Start() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTrace_Start
// Purpose:        Start tracing
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	this->SetParValue("RUNTASK", TMrbDGFData::kRunLinear, kTRUE);
	this->StartRun();
	return(kTRUE);
}

Int_t TMrbDGF::GetTrace_Stop(TMrbDGFEventBuffer & Buffer, Int_t SecsToWait) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTrace_Read
// Purpose:        Stop tracing and read data
// Results:        Int_t NofWords   -- number of trace data read
//////////////////////////////////////////////////////////////////////////////

	Int_t nofWords;

	if (SecsToWait > 0) {
		sleep(1);
		this->WaitActive(SecsToWait);
	}
	this->StopRun();
	this->RestoreParams(TMrbDGF::kSaveTrace);
	if (this->GetWaitStatus() != TMrbDGF::kWaitOk) return(-1);
	nofWords = this->ReadEventBuffer(Buffer);
	Buffer.SetTraceError(0);
	return(nofWords);
}

Int_t TMrbDGF::GetUntrigTrace(TArrayI & Buffer, UInt_t ChannelPattern, Int_t XwaitStates) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetUntrigTrace
// Purpose:        Get a untriggered trace
// Arguments:      TArrayI & Buffer               -- where to store trace data
//                 UInt_t ChannelPattern          -- channel pattern
//                 Int_t XwaitStates              -- extra wait states
// Results:        Int_t NofTraceData             -- number of trace data taken
// Exceptions:
// Description:    Takes data from an untriggered trace.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofWords, n;

	if (!this->GetUntrigTrace_Init(Buffer, ChannelPattern, XwaitStates)) return(-1);

	nofWords = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			if (!this->GetUntrigTrace_Start(chn)) return(-1);
			if ((n = this->GetUntrigTrace_Stop(chn, Buffer)) != -1) nofWords += n;
		}
	}
	this->GetUntrigTrace_Restore();
	return(nofWords);
}

Bool_t TMrbDGF::GetUntrigTrace_Init(TArrayI & Buffer, UInt_t ChannelPattern, Int_t XwaitStates) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetUntrigTrace_Init
// Purpose:        Initialize untriggered trace
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	Int_t deltaT, xWait;
	Int_t unuseda;
	Bool_t sts;
		
	if (!this->CheckConnect("GetUntrigTrace")) return(kFALSE);
	
	this->SaveParams(TMrbDGF::kSaveUntrigTrace);						// save current settings

	if (XwaitStates <= 3) {
		xWait = XwaitStates;
		deltaT = 3;
	} else if (XwaitStates <= 5) {
		xWait = XwaitStates;
		deltaT = xWait;
	} else if (XwaitStates <= 11) {
		xWait = XwaitStates;
		deltaT = xWait + 3;
	} else {
		xWait = (XwaitStates + 3) / 4;
		xWait *= 4;
		deltaT = xWait + 3;
	}
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelPattern & (1 << chn)) {
			this->SetParValue(chn, "XWAIT", xWait);
			unuseda = (deltaT >= 11) ? (Int_t) (floor(65536. / ((deltaT - 3) /4 ))) : 0;
			this->SetParValue(chn, "UNUSEDA", unuseda);
		}
	}
	Buffer.Reset();
	Buffer.Set(TMrbDGFData::kUntrigTraceLength * TMrbDGFData::kNofChannels);

	sts = this->WriteParamMemory();
	if (!sts) this->RestoreParams(TMrbDGF::kSaveUntrigTrace);
	return(sts);
}

Bool_t TMrbDGF::GetUntrigTrace_Start(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetUntrigTrace_Start
// Purpose:        Start untriggered trace for a given channel
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	this->SetParValue("HOSTIO", Channel, kTRUE);
	this->SetParValue("RUNTASK", TMrbDGFData::kRunControl, kTRUE);
	this->SetParValue("CONTROLTASK", TMrbDGFData::kSampleADCs, kTRUE);
	this->StartRun();
	return(kTRUE);
}

Int_t TMrbDGF::GetUntrigTrace_Stop(Int_t Channel, TArrayI & Buffer, Int_t SecsToWait) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetUntrigTrace_Stop
// Purpose:        Stop untriggered trace and read data
// Results:        Int_t NofWords   -- number of words read
//////////////////////////////////////////////////////////////////////////////

	UInt_t addr;
	Int_t wc;
	Int_t offset;
	Int_t data;
	TArrayI cData;

	if (SecsToWait > 0) {
		sleep(1);
		this->WaitActive(SecsToWait);
	}
	this->StopRun();

	if (this->GetWaitStatus() != TMrbDGF::kWaitOk) return(-1);

	addr = this->GetParValue("AOUTBUFFER");
	wc = this->GetParValue("LOUTBUFFER");
	this->WriteTSAR(addr);

	cData.Set(wc);
	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, wc, kTRUE) == -1) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0: Reading untriggered traces failed - DSPAddr=0x" << setbase(16) << addr
						<< setbase(10) << ", wc=" << wc << ", ActionCount=-1" << endl;
		gMrbLog->Flush(this->ClassName(), "GetUntrigTrace");
		return(-1);
	}

	offset = Channel * wc;
	Int_t min = 1000000;
	Int_t max = 0;
	for (Int_t i = 0; i < wc; i++) {
		data = cData[i];
		if (data > max) max = data;
		if (data < min) min = data;
		Buffer[i + offset] = data;
	}
	if (min == max) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << " (channel " << Channel
						<< ") - data const = " << min << endl;
		gMrbLog->Flush(this->ClassName(), "GetUntrigTrace");
	}
	return(wc);
}

Bool_t TMrbDGF::GetUntrigTrace_Restore() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetUntrigTrace_Restore
// Purpose:        Restore settings after untrig trace
// Results:        
//////////////////////////////////////////////////////////////////////////////

	this->RestoreParams(TMrbDGF::kSaveUntrigTrace);
	return(kTRUE);
}

Int_t TMrbDGF::GetDacRamp(TArrayI & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDacRamp
// Purpose:        Calibrate dac offsets
// Arguments:      TArrayI & Buffer               -- where to store trace data
// Results:        Int_t nofWords                 -- number of data words
// Exceptions:
// Description:    Starts a ramp cycle.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->GetDacRamp_Init()) return(-1);
	if (!this->GetDacRamp_Start()) return(-1);
	return(this->GetDacRamp_Stop(Buffer));
}

Bool_t TMrbDGF::GetDacRamp_Init() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDacRamp_Init
// Purpose:        Initialize ramp
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	if (!this->CheckConnect("GetDacRamp")) return(kFALSE);
	
	this->SaveParams(TMrbDGF::kSaveDacRamp);			// save current settings
	
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		this->SetParValue(chn, "FASTTHRESH", 1020);
		this->SetParValue(chn, "FASTADCTHR", 255);
		this->SetParValue(chn, "BLCUT", 0);
	}
	sts = this->WriteParamMemory();
	if (!sts) this->RestoreParams(TMrbDGF::kSaveDacRamp);
	return(sts);
}
	
Bool_t TMrbDGF::GetDacRamp_Start() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDacRamp_Start
// Purpose:        Start ramp
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	this->SetParValue("RUNTASK", TMrbDGFData::kRunControl, kTRUE);
	this->SetParValue("CONTROLTASK", TMrbDGFData::kCalibrate, kTRUE);
	this->StartRun();
	return(kTRUE);
}

Int_t TMrbDGF::GetDacRamp_Stop(TArrayI & Buffer, Int_t SecsToWait) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetDacRamp_Stop
// Purpose:        Stop ramp and read data
// Results:        Int_t NofWords  -- number of words read
//////////////////////////////////////////////////////////////////////////////

	UInt_t addr;
	Int_t wc;
		
	this->WaitActive(SecsToWait);
	this->StopRun();
	this->RestoreParams(TMrbDGF::kSaveDacRamp);		// restore current settings
	
	Buffer.Reset();

	addr = this->GetParValue("AOUTBUFFER");
	wc = this->GetParValue("LOUTBUFFER");
	Buffer.Set(wc);
	this->WriteTSAR(addr);

	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), Buffer, 0, wc, kTRUE) == -1) {
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0: Reading DAC values failed - DSPAddr=0x" << setbase(16) << addr
						<< setbase(10) << ", wc=" << wc << endl;
		gMrbLog->Flush(this->ClassName(), "GetDacRamp");
		return(-1);
	}

	return(wc);
}

Double_t TMrbDGF::TauFit(Int_t Channel, Int_t NofTraces, Int_t TraceLength,
													Double_t A0, Double_t A1,
													Int_t From, Int_t To,
													Stat_t TraceError, Stat_t ChiSquare, Bool_t DrawIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::TauFit
// Purpose:        Perform fit on tau distribution
// Arguments:      Int_t Channel        -- channel number
//                 Int_t NofTraces      -- number of traces to collect
//                 Int_t TraceLength    -- trace length
//                 Double_t A0          -- start value for a(0)
//                 Double_t A1          -- start value for a(1)
//                 Int_t From           -- fit region, lower limit
//                 Int_t To             -- fit region, upper limit
//                 Stat_t TraceError    -- const error for trace data
//                 Stat_t ChiSquare     -- chi square
//                 Bool_t DrawIt        -- draw histo + fit
// Results:        Double_t Tau         -- resulting tau value
// Exceptions:
// Description:    Collects the given number of traces and performs a tau fit.
//                 Stores tau values in a histogram and fits results with a gaussian.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEventBuffer buffer;
	Int_t i, j;
	Int_t bs, nt;
	Double_t tau, tauSum, tauMean;
	Int_t nofTraces, nofGoodTaus, nofBadTaus;
	TString fitOpt;
	ofstream hList, hGood, hBad;
	TMrbString hln;

	if (!this->CheckConnect("TauFit")) return(-1);
	if (!this->CheckChannel("TauFit", Channel)) return(-1);
	if (!this->CheckChannelMask("TauFit")) return(-1);

	if (this->GetNofGoodChannels() != 1) {
		gMrbLog->Err() << "Can't fit more than 1 channel" << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
		return(-1);
	}

	this->SaveParams(TMrbDGF::kSaveTaufit); 		// save current settings

	buffer.SetModule(this); 	// connect to dgf module
	
	bs = this->GetParValue("LOUTBUFFER", kTRUE);
	nt = bs / (TraceLength + TMrbDGFEventBuffer::kTotalHeaderLength);
	if (nt == 0) {
		gMrbLog->Err()	<< "Illegal trace length - " << TraceLength
						<< " (+ " << TMrbDGFEventBuffer::kTotalHeaderLength
						<< " header words), buffer size = " << bs << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
		this->RestoreParams(TMrbDGF::kSaveTaufit);
		return(-1);
	}

	this->SetChanCSRA(Channel,
						TMrbDGFData::kGoodChannel |
						TMrbDGFData::kEnableTrigger,
						TMrbDGF::kBitOr, kTRUE);

	this->SetParValue("MAXEVENTS", nt);
 	this->SetParValue("SYNCHWAIT", 0);
 	this->SetParValue("INSYNCH", 1);
 	this->SetParValue("COINCPATTERN", 0xffff);
 	this->SetCoincWait();
  
	this->SetParValue(Channel, "TRACELENGTH", TraceLength);

	this->WriteParamMemory();

	this->SetParValue("RUNTASK", TMrbDGFData::kRunLinear, kTRUE);

	if (fTauDistr) {
		gROOT->GetList()->Remove(fTauDistr);
		delete fTauDistr;
	}

	fTauDistr = new TH1F("hTau", "tau distribution", 100, 0., 100.);

	buffer.SetTraceError(TraceError);

	TFile * f = new TFile("tau.root", "RECREATE");
	gSystem->Exec("rm -f tau-*.histlist");

	cout	<< this->ClassName() << "::TauFit(): Collecting " << NofTraces << " traces (chi2 < " << ChiSquare
			<< ") - wait " << ends << flush;
	tauSum = 0;
	nofTraces = 0;
	nofGoodTaus = 0;
	nofBadTaus = 0;
	for (i = 0; i < NofTraces; i += nt) {
		cout	<< "." << ends << flush;
		this->StartRun();
		this->WaitActive(10);
		this->StopRun();
		if (this->ReadEventBuffer(buffer) > 0) {
			for (j = 0; j < buffer.GetNofEvents(); j++) {
				nofTraces++;
				tau = buffer.CalcTau(j, Channel, nofTraces, A0, A1, From, To, kFALSE, kTRUE);
				if ((nofTraces % 16) == 1) {
					if (nofTraces != 1) hList.close();
					hln = "tau-";
					hln += nofTraces;
					hln += ".histlist";
					hList.open(hln.Data(), ios::out);
				}
				buffer.Histogram()->Write();
				hList << "tau.root " << buffer.Histogram()->GetName() << endl;
				if (buffer.Fit()->GetChisquare() < ChiSquare) {
					nofGoodTaus++;
					if ((nofGoodTaus % 16) == 1) {
						if (nofGoodTaus != 1) hGood.close();
						hln = "tau-good-"; hln += nofGoodTaus; hln += ".histlist";
						hGood.open(hln.Data(), ios::out);
					}
					hGood << "tau.root " << buffer.Histogram()->GetName() << endl;
					tauSum += tau;
					fTauDistr->Fill((Axis_t) tau, 1.);
				} else {
					nofBadTaus++;
					if ((nofBadTaus % 16) == 1) {
						if (nofBadTaus != 1) hBad.close();
						hln = "tau-bad-"; hln += nofBadTaus; hln += ".histlist";
						hBad.open(hln.Data(), ios::out);
					}
					hBad << "tau.root " << buffer.Histogram()->GetName() << endl;
				}
			}
		} else if (this->GetWaitStatus() != TMrbDGF::kWaitOk) return(-1);
	}
	cout	<< " done." << endl;

	this->RestoreParams(TMrbDGF::kSaveTaufit);			// restore current settings

	tauMean = tauSum / nofGoodTaus;
	fGaussian = new TF1("fTau", "gaus", tauMean - 6, tauMean + 6);
	fGaussian->SetParameters(tauSum, tauMean, 3);
	fGaussian->SetParName(0,"gaus_const");
	fGaussian->SetParName(1,"gaus_mean");
	fGaussian->SetParName(2,"gaus_sigma");
	fGaussian->SetLineColor(4);
	fGaussian->SetLineWidth(3);

	if (DrawIt) fTauDistr->Draw();

	fitOpt = "R";
	if (!DrawIt) fitOpt += "N";
	fitOpt += "+";
	fTauDistr->Fit("fTau", fitOpt, "SAME");

	fTauDistr->Write();
	f->Close();
	hList.close();
	hGood.close();
	hBad.close();

	tau = fGaussian->GetParameter(1);

	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out() << "Fitted tau value = " << tau << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit", setblue);
	}
	return(tau);
}

TMrbDGF::EMrbWaitStatus TMrbDGF::WaitActive(Int_t Timeout) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WaitActive
// Purpose:        Wait for active bit to drop
// Arguments:      Int_t Timeout    -- timeout in seconds
// Results:        Status: error, timeout, abort, ok
// Exceptions:
// Description:    Waits until bit TMrbDGFData::kActive disapears in the csr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csr;
	time_t start;

	fStatusW = TMrbDGF::kWaitActive;
	
	start = time(NULL);
	for (;;) {
		csr = this->ReadCSR();
		if (csr == 0xffffffff) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Unexpected error. Giving up." << endl;
			gMrbLog->Flush(this->ClassName(), "WaitActive");
			fStatusW = TMrbDGF::kWaitError;
			break;
		}
		if ((csr & TMrbDGFData::kActive) == 0) {
			fStatusW = TMrbDGF::kWaitOk;
			break;
		}

		gSystem->ProcessEvents();
		if (this->IsAborted()) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Aborted after " << (time(NULL) - start) << " secs. Stopping current run." << endl;
			gMrbLog->Flush(this->ClassName(), "WaitActive");
			fStatusW = TMrbDGF::kWaitAborted;
			break;
		}
		if ((time(NULL) - start) > Timeout) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Timed out after " << Timeout << " secs" << endl;
			gMrbLog->Flush(this->ClassName(), "WaitActive");
			fStatusW = TMrbDGF::kWaitTimedOut;
			break;
		}
	}
	return(fStatusW);
}

TMrbDGF::EMrbWaitStatus TMrbDGF::WaitLAM(Int_t Timeout) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::WaitLAM
// Purpose:        Wait for LAM
// Arguments:      Int_t Timeout    -- timeout in seconds
// Results:        Status: error, timeout, abort, ok
// Exceptions:
// Description:    Waits until bit TMrbDGFData::kLAMActive shows up in the csr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csr;
	time_t start;

	fStatusW = TMrbDGF::kWaitForLAM;
	
	start = time(NULL);
	for (;;) {
		csr = this->ReadCSR();
		if (csr == 0xffffffff) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Unexpected error. Giving up." << endl;
			gMrbLog->Flush(this->ClassName(), "WaitLAM");
			fStatusW = TMrbDGF::kWaitError;
			break;
		}
		if ((csr & TMrbDGFData::kLAMActive) != 0) {
			fStatusW = TMrbDGF::kWaitOk;
			break;
		}

		gSystem->ProcessEvents();
		if (this->IsAborted()) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Aborted after " << (time(NULL) - start) << " secs. Stopping current run." << endl;
			gMrbLog->Flush(this->ClassName(), "WaitLAM");
			fStatusW = TMrbDGF::kWaitAborted;
			break;
		}
		if ((time(NULL) - start) > Timeout) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Timed out after " << Timeout << " secs" << endl;
			gMrbLog->Flush(this->ClassName(), "WaitLAM");
			fStatusW = TMrbDGF::kWaitTimedOut;
			break;
		}
	}
	return(fStatusW);
}

Bool_t TMrbDGF::CheckConnect(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckConnect
// Purpose:        Check if connected to CAMAC
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks connection to CAMAC, outputs error message if not connected.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (IsConnected()) return(kTRUE);

	gMrbLog->Err() << this->GetTitle() << " - assigned to host " << fCamacHost << ", but NOT connected" << endl;
	gMrbLog->Flush(this->ClassName(), Method);
	return(kFALSE);
}

Bool_t TMrbDGF::CheckActive(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckActive
// Purpose:        Check if active bit set
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if bit "ACTIVE" is set in CSR.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csr;

	return(kTRUE);
	
	csr = this->ReadCSR();
	if (csr == 0xffffffff) {
		gMrbLog->Err() << "Unexpected error. Giving up." << endl;
		gMrbLog->Flush(this->ClassName(), "CheckActive");
		return(kFALSE);
	}
	if ((csr & TMrbDGFData::kActive) != 0) {
		gMrbLog->Err() << this->GetTitle() << " is active - access is refused" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbDGF::CheckChannel(const Char_t * Method, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckChannel
// Purpose:        Check channel number
// Arguments:      Char_t * Method      -- calling method
//                 Int_t Channel        -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if channel number is legal (0 ... 3)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel >= 0 && Channel < TMrbDGFData::kNofChannels) return(kTRUE);

	gMrbLog->Err() << this->GetTitle() << " - illegal channel number " << Channel << endl;
	gMrbLog->Flush(this->ClassName(), Method);
	return(kFALSE);
}

Bool_t TMrbDGF::CheckChannelMask(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckChannelMask
// Purpose:        Check channel mask
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if any channel is active.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetChannelMask() == 0) {
		gMrbLog->Err() << this->GetTitle() << " - no good (=active) channels" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	} else return(kTRUE);
}

Bool_t TMrbDGF::CheckTriggerMask(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CheckTriggerMask
// Purpose:        Check trigger mask
// Arguments:      Char_t * Method      -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if any trigger defined.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetTriggerMask() == 0) {
		gMrbLog->Err() << this->GetTitle() << " - no trigger channels" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	} else return(kTRUE);
}

Bool_t TMrbDGF::SetTriggerMask(UInt_t ChannelMask, Int_t Polarity, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerMask
// Purpose:        Set mask of trigger channels
// Arguments:      UInt_t ChannelMask     -- channel mask
//                 Int_t Polarity         -- trigger polarity (+1/-1)
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Set pattern of trigger channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	EMrbBitOp bitOp;

	if (UpdateDSP && !this->CheckConnect("SetTriggerMask")) return(kFALSE);

	if ((ChannelMask & 0xfffffff0) != 0) {
		gMrbLog->Err() << "Illegal mask bits - 0x" << setbase(16) << ChannelMask << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMask");
		return(kFALSE);
	}

	if (Polarity == 1)			bitOp = kBitOr;
	else if (Polarity == -1)	bitOp = kBitClear;
	else {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be +1/-1)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMask");
		return(kFALSE);
	}
	
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelMask & 1) {
			this->SetChanCSRA(chn, TMrbDGFData::kEnableTrigger, kBitOr, UpdateDSP);
			this->SetChanCSRA(chn, TMrbDGFData::kTriggerPositive, bitOp, UpdateDSP);
		} else {
			this->SetChanCSRA(chn, TMrbDGFData::kEnableTrigger | TMrbDGFData::kTriggerPositive, kBitClear, UpdateDSP);
		}
		ChannelMask >>= 1;
	}

	return(kTRUE);
}

Bool_t TMrbDGF::SetTriggerMask(const Char_t * ChannelMask, const Char_t * Polarity, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerMask
// Purpose:        Set mask of trigger channels
// Arguments:      const Char_t * ChannelMask     -- channel mask
//                 const Char_t * Polarity        -- trigger polarity
//                 Bool_t UpdateDSP               -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Set pattern of good (=active) channels. Format: any out of "0123"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cMask, cIds, c;
	UInt_t channelMask;
	Int_t n;
	Int_t lng;
	TMrbLofNamedX trigPol;
	TMrbNamedX * tp;

	if (UpdateDSP && !this->CheckConnect("SetTriggerMask")) return(kFALSE);

	trigPol.AddNamedX(kMrbTriggerPolarity);
	tp = trigPol.FindByName(Polarity, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (tp == NULL) {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be \"pos\" or \"neg\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMask");
		return(kFALSE);
	}

	cIds = "0123";

	cMask = ChannelMask;
	lng = cMask.Length();
	if (lng == 0 || lng > TMrbDGFData::kNofChannels) {
		gMrbLog->Err() << "Illegal channel mask - " << ChannelMask << endl;
		gMrbLog->Flush(this->ClassName(), "SetChannelMask");
		return(kFALSE);
	}

	channelMask = 0;
	for (Int_t i = 0; i < lng; i++) {
		c = cMask(i);
		n = cIds.Index(c, 0);
		if (n == -1) {
			gMrbLog->Err() << "Illegal channel number - " << c << " (ignored)" << endl;
			gMrbLog->Flush(this->ClassName(), "SetChannelMask");
		} else {
			channelMask |= 1 << n;
		}
	}
	return(this->SetTriggerMask(channelMask, tp->GetIndex(), UpdateDSP));
}

Bool_t TMrbDGF::SetTriggerMask(UInt_t ChannelMask, const Char_t * Polarity, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerMask
// Purpose:        Set mask of trigger channels
// Arguments:      UInt_t ChannelMask             -- channel mask
//                 const Char_t * Polarity        -- trigger polarity
//                 Bool_t UpdateDSP               -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Set pattern of good (=active) channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX trigPol;
	TMrbNamedX * tp;

	if (UpdateDSP && !this->CheckConnect("SetTriggerMask")) return(kFALSE);

	trigPol.AddNamedX(kMrbTriggerPolarity);
	tp = trigPol.FindByName(Polarity, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (tp == NULL) {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be \"pos\" or \"neg\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMask");
		return(kFALSE);
	}

	return(this->SetTriggerMask(ChannelMask, tp->GetIndex(), UpdateDSP));
}

Bool_t TMrbDGF::SetTriggerBit(Int_t Channel, Int_t Polarity, EMrbBitOp SetOrClear, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerBit
// Purpose:        Set a single trigger
// Arguments:      Int_t Channel          -- channel number
//                 Int_t Polarity         -- trigger polarity (+1/-1)
//                 EMrbBitOp SetOrClear   -- set / clear
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a single trigger.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	EMrbBitOp posPol;

	if (UpdateDSP && !this->CheckConnect("SetTriggerBit")) return(kFALSE);
	if (!this->CheckChannel("SetTriggerBit", Channel)) return(kFALSE);

	if (Polarity == 1)			posPol = kBitOr;
	else if (Polarity == -1)	posPol = kBitClear;
	else {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be +1/-1)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerBit");
		return(kFALSE);
	}
	
	if (SetOrClear == kBitSet) {
		this->SetChanCSRA(Channel, TMrbDGFData::kEnableTrigger, kBitOr, UpdateDSP);
		this->SetChanCSRA(Channel, TMrbDGFData::kTriggerPositive, posPol, UpdateDSP);
	} else {
		this->SetChanCSRA(Channel, TMrbDGFData::kEnableTrigger | TMrbDGFData::kTriggerPositive, kBitClear, UpdateDSP);
	}
	return(kTRUE);
}

Bool_t TMrbDGF::SetTriggerBit(Int_t Channel, const Char_t * Polarity, EMrbBitOp SetOrClear, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerBit
// Purpose:        Set a single trigger
// Arguments:      Int_t Channel          -- channel number
//                 Char_t * Polarity      -- trigger polarity (pos/neg)
//                 EMrbBitOp SetOrClear   -- set / clear
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a single trigger.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX trigPol;
	TMrbNamedX * tp;

	if (UpdateDSP && !this->CheckConnect("SetTriggerBit")) return(kFALSE);
	if (!this->CheckChannel("SetTriggerBit", Channel)) return(kFALSE);

	trigPol.AddNamedX(kMrbTriggerPolarity);
	tp = trigPol.FindByName(Polarity, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (tp == NULL) {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be \"pos\" or \"neg\")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerBit");
		return(kFALSE);
	}
	if (SetOrClear == kBitSet || SetOrClear == kBitOr) SetOrClear = kBitOr; else SetOrClear = kBitClear;
	return(this->SetTriggerBit(Channel, tp->GetIndex(), SetOrClear, UpdateDSP));
}

Bool_t TMrbDGF::HasTriggerBitSet(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::HasTriggerBitSet
// Purpose:        Check if trigger channel
// Arguments:      Int_t Channel       -- channel number
//                 Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if channel has trigger bit set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csra;

	csra = this->GetChanCSRA(Channel, ReadFromDSP);
	if (csra == 0xffffffff) return(kFALSE);
	return((csra & TMrbDGFData::kEnableTrigger) != 0);
}

UInt_t TMrbDGF::GetTriggerMask(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTriggerMask
// Purpose:        Return pattern of trigger channels
// Arguments:      Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        UInt_t TriggerMask  -- mask bits
// Exceptions:
// Description:    Returns a pattern of trigger channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t channelMask;
	UInt_t bit;

	if (ReadFromDSP && !this->CheckConnect("GetChannelMask")) return(0);

	bit = 1;
	channelMask = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (this->HasTriggerBitSet(chn, ReadFromDSP)) channelMask |= bit;
		bit <<= 1;
	}
	return(channelMask);
}

Bool_t TMrbDGF::SetTriggerPolarity(Int_t Channel, Int_t Polarity, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetTriggerPolarity
// Purpose:        Set a single trigger
// Arguments:      Int_t Channel          -- channel number
//                 Int_t Polarity         -- trigger polarity (+1/-1)
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets trigger polarity.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	EMrbBitOp posPol;

	if (UpdateDSP && !this->CheckConnect("SetTriggerPOlarity")) return(kFALSE);
	if (!this->CheckChannel("SetTriggerPolarity", Channel)) return(kFALSE);

	if (Polarity == 1)			posPol = kBitOr;
	else if (Polarity == -1)	posPol = kBitClear;
	else {
		gMrbLog->Err() << "Illegal trigger polarity - " << Polarity << " (should be +1/-1)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerPolarity");
		return(kFALSE);
	}
	
	this->SetChanCSRA(Channel, TMrbDGFData::kTriggerPositive, posPol, UpdateDSP);
	return(kTRUE);
}

Int_t TMrbDGF::GetTriggerPolarity(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetTriggerPolarity
// Purpose:        Get trigger polarity
// Arguments:      Int_t Channel          -- channel number
//                 Bool_t ReadFromDSP     -- kTRUE: read data from DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Returns polarity bit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csra;

	if (ReadFromDSP && !this->CheckConnect("GetTriggerPolarity")) return(0);
	if (!this->CheckChannel("GetTriggerPolarity", Channel)) return(0);

	csra = this->GetChanCSRA(Channel, ReadFromDSP);
	if (csra == 0xffffffff) return(0);
	if (csra & TMrbDGFData::kTriggerPositive) return(1); else return(-1);
}

void TMrbDGF::PrintTriggerMask(Bool_t CrFlag, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintTriggerMask
// Purpose:        Output trigger channels
// Arguments:      Bool_t CrFlag       -- kTRUE if CR to be output
//                 Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        --
// Exceptions:
// Description:    Outputs list of trigger channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("PrintTriggerMask")) return;

	if (this->GetChannelMask(ReadFromDSP) == 0) {
		cout << setred << "None" << setblack;
	} else {
		for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			if (this->HasTriggerBitSet(chn)) {
				if (this->GetTriggerPolarity(chn) == 1) cout << setblue << chn << ":pos " << setblack;
				else									cout << setblue << chn << ":neg " << setblack;
			} else {
				cout << setred << chn << ":off " << setblack;
			}
		}
	}
	if (CrFlag) cout << endl; else cout << flush;
}

Bool_t TMrbDGF::SetChannelMask(UInt_t ChannelMask, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetChannelMask
// Purpose:        Set mask of good (=active) channels
// Arguments:      UInt_t ChannelMask     -- channel mask
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Set pattern of good (=active) channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	EMrbBitOp bitOp;

	if (UpdateDSP && !this->CheckConnect("SetChannelMask")) return(kFALSE);

	if ((ChannelMask & 0xfffffff0) != 0) {
		gMrbLog->Err() << "Illegal mask bits - 0x" << setbase(16) << ChannelMask << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "SetChannelMask");
		return(kFALSE);
	}

	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (ChannelMask & 1)	bitOp = kBitOr;
		else					bitOp = kBitClear;
		this->SetChanCSRA(chn, TMrbDGFData::kGoodChannel, bitOp, UpdateDSP);
		ChannelMask >>= 1;
	}
	return(kTRUE);
}

Bool_t TMrbDGF::SetChannelMask(const Char_t * ChannelMask, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetChannelMask
// Purpose:        Set mask of good (=active) channels
// Arguments:      const Char_t * ChannelMask     -- channel mask
//                 Bool_t UpdateDSP               -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Set pattern of good (=active) channels. Format: any out of "0123"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cMask, cIds, c;
	UInt_t channelMask;
	Int_t n;
	Int_t lng;

	if (UpdateDSP && !this->CheckConnect("SetChannelMask")) return(kFALSE);

	cIds = "0123";

	cMask = ChannelMask;
	lng = cMask.Length();
	if (lng == 0 || lng > TMrbDGFData::kNofChannels) {
		gMrbLog->Err() << "Illegal channel mask - " << ChannelMask << endl;
		gMrbLog->Flush(this->ClassName(), "SetChannelMask");
		return(kFALSE);
	}

	channelMask = 0;
	for (Int_t i = 0; i < lng; i++) {
		c = cMask(i);
		n = cIds.Index(c, 0);
		if (n == -1) {
			gMrbLog->Err() << "Illegal channel number - " << c << " (ignored)" << endl;
			gMrbLog->Flush(this->ClassName(), "SetChannelMask");
		} else {
			channelMask |= 1 << n;
		}
	}
	return(this->SetChannelMask(channelMask, UpdateDSP));
}

Bool_t TMrbDGF::SetXwait(Int_t Xwait, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetXwait
// Purpose:        Set XWAIT parameter
// Arguments:      Int_t Xwait            -- wait states
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets channel param(s) XWAITn.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("SetXwait")) return(kFALSE);

	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) this->SetParValue(chn, "XWAIT", Xwait, UpdateDSP);
	return(kTRUE);
}

Bool_t TMrbDGF::SetSynchWait(Bool_t SynchFlag, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetSynchWait
// Purpose:        Set SYNCHWAIT flag
// Arguments:      Bool_t SynchFlag       -- synch wait flag
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets module param SYNCHWAIT.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t synchVal;
	TString synchMsg;
		
	if (UpdateDSP && !this->CheckConnect("SetSynchWait")) return(kFALSE);

	if (SynchFlag) {
		synchVal = 1;
		synchMsg = "Start/stop synchronized (SYNCHWAIT=1)";
	} else {
		synchVal = 0;
		synchMsg = "Start/stop asynchronous (SYNCHWAIT=0)";
	}
	
	this->SetParValue("SYNCHWAIT", synchVal, UpdateDSP);
	
	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": " << synchMsg << endl;
		gMrbLog->Flush(this->ClassName(), "SetSynchWait", setblue);
	}
		
	return(kTRUE);
}

Bool_t TMrbDGF::SetInSynch(Bool_t SynchFlag, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetInSynch
// Purpose:        Set INSYNCH flag
// Arguments:      Bool_t SynchFlag       -- synch flag
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets module param INSYNCH.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t synchVal;
	TString synchMsg;
		
	if (UpdateDSP && !this->CheckConnect("SetInSynch")) return(kFALSE);

	if (SynchFlag) {
		synchVal = 0;
		synchMsg = "Clock synchronized with new run (INSYNCH=0)";
	} else {
		synchVal = 1;
		synchMsg = "Clock running thru (INSYNCH=1)";
	}
	
	this->SetParValue("INSYNCH", synchVal, UpdateDSP);
	
	if (gMrbDGFData->fVerboseMode) {
		gMrbLog->Out()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ": " << synchMsg << endl;
		gMrbLog->Flush(this->ClassName(), "SetInSynch", setblue);
	}
		
	return(kTRUE);
}

Bool_t TMrbDGF::SetGoodChannel(Int_t Channel, EMrbBitOp SetOrClear, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetGoodChannel
// Purpose:        Set "good channel" bit
// Arguments:      Int_t Channel          -- channel number
//                 EMrbBitOp SetOrClear   -- set / clear
//                 Bool_t UpdateDSP       -- kTRUE: write data to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Mark channel active.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (UpdateDSP && !this->CheckConnect("SetGoodChannel")) return(kFALSE);
	if (!this->CheckChannel("SetGoodChannel", Channel)) return(kFALSE);

	if (SetOrClear == kBitSet || SetOrClear == kBitOr) SetOrClear = kBitOr; else SetOrClear = kBitClear;
	this->SetChanCSRA(Channel, TMrbDGFData::kGoodChannel, SetOrClear, UpdateDSP);
	return(kTRUE);
}

Bool_t TMrbDGF::IsGoodChannel(Int_t Channel, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::IsGoodChannel
// Purpose:        Check if channel active
// Arguments:      Int_t Channel       -- channel number
//                 Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if given channel active.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t csra;

	csra = this->GetChanCSRA(Channel, ReadFromDSP);
	if (csra == 0xffffffff) return(kFALSE);
	return((csra & TMrbDGFData::kGoodChannel) != 0);
}

UInt_t TMrbDGF::GetChannelMask(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetChannelMask
// Purpose:        Return channel mask
// Arguments:      Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        UInt_t ChannelMask  -- mask bits
// Exceptions:
// Description:    Returns a pattern of good (=active) channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t channelMask;
	UInt_t bit;

	if (ReadFromDSP && !this->CheckConnect("GetChannelMask")) return(0);

	bit = 1;
	channelMask = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (this->IsGoodChannel(chn, ReadFromDSP)) channelMask |= bit;
		bit <<= 1;
	}
	return(channelMask);
}

void TMrbDGF::PrintChannelMask(Bool_t CrFlag, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::PrintChannelMask
// Purpose:        Output good (=active) channels
// Arguments:      Bool_t CrFlag       -- kTRUE if CR to be output
//                 Bool_t ReadFromDSP  -- kTRUE: read channel CSR first
// Results:        --
// Exceptions:
// Description:    List of good (=active) channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("PrintChannelMask")) return;

	if (this->GetChannelMask(ReadFromDSP) == 0) {
		cout << setred << "None" << setblack;
	} else {
		for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
			if (this->IsGoodChannel(chn)) {
				cout << setblue << chn << ":on " << setblack;
			} else {
				cout << setred << chn << ":off " << setblack;
			}
		}
	}
	if (CrFlag) cout << endl; else cout << flush;
}

Int_t TMrbDGF::GetNofGoodChannels(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetNofGoodChannels
// Purpose:        Return number of good (=active) channels
// Arguments:      Bool_t ReadFromDSP   -- kTRUE: read channel CSR first
// Results:        Int_t NofChannels    -- number of good (=active) channels
// Exceptions:
// Description:    Calculate number of good (=active) channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ReadFromDSP && !this->CheckConnect("GetNofGoodChannels")) return(0);

	Int_t nofChannels = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (this->IsGoodChannel(chn, ReadFromDSP)) nofChannels++;
	}
	return(nofChannels);
}

Bool_t TMrbDGF::ModifyCoincPattern(UInt_t Pattern, EMrbBitOp AddOrRemove, Bool_t UpdateDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::ModifyCoincPattern
// Purpose:        Add/remove a hit pattern
// Arguments:      UInt_t Pattern         -- hit pattern
//                 EMbrBitOp AddOrRemove  -- add or remove?
//                 Bool_t UpdateDSP       -- kTRUE: write value to DSP
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Activate a certain hit pattern.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t coinc;

	if (UpdateDSP && !this->CheckConnect("ModifyCoincPattern")) return(0);

	if ((Pattern == 0) || (Pattern & 0xfffffff0) != 0) {
		gMrbLog->Err()	<< "Illegal hit pattern - 0x" << setbase(16) << setiosflags(ios::uppercase) << Pattern
						<< resetiosflags(ios::uppercase) << setbase(10) << " (should be in [0x1, 0xF])" << endl;
		gMrbLog->Flush(this->ClassName(), "ModifyCoincPattern");
		return(kFALSE);
	}

	Int_t pat = (Int_t) Pattern;
	UInt_t bit = 1 << (pat - 1);
	coinc = this->GetCoincPattern();
	if (AddOrRemove == kBitSet || AddOrRemove == kBitOr)	coinc |= bit;
	else													coinc &= ~bit;
	return(this->SetCoincPattern(coinc, UpdateDSP));
}

Bool_t TMrbDGF::CoincPatternSet(UInt_t Pattern, Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::CoincPatternSet
// Purpose:        Test if hit pattern active
// Arguments:      UInt_t Pattern       -- pattern to be tested
//                 Bool_t ReadFromDSP   -- read value from DSP first
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if hit pattern is active.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t coinc;

	if (ReadFromDSP && !this->CheckConnect("CoincPatternSet")) return(0);

	if ((Pattern == 0) || (Pattern & 0xfffffff0) != 0) {
		gMrbLog->Err()	<< "Illegal hit pattern - 0x" << setbase(16) << setiosflags(ios::uppercase) << Pattern
						<< resetiosflags(ios::uppercase) << setbase(10) << " (should be in [0x1, 0xF])" << endl;
		gMrbLog->Flush(this->ClassName(), "CoincPatternSet");
		return(kFALSE);
	}

	Int_t pat = (Int_t) Pattern;
	Int_t bit = 1 << (pat - 1);
	coinc = this->GetCoincPattern();
	return((coinc & bit) != 0);
}

const Char_t * TMrbDGF::GetClusterInfo(TMrbString & Info) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetClusterInfo
// Purpose:        Return cluster info
// Arguments:      TString & Info   -- where to store info string
// Results:        Char_t * Info    -- info string
// Exceptions:
// Description:    Returns cluster info.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Info = "CLU";
	Info += this->GetClusterSerial();
	Info += " ";
	TMrbString h;
	h.FromInteger(this->GetClusterHexNum(), 0, ' ', 16);
	Info += h;
	Info += " ";
	Info += this->GetClusterColor();
	Info += " (";
	Info += this->GetClusterSegments();
	Info += ")";
	return(Info.Data());
}

void TMrbDGF::SetRevision(Int_t ManufactIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetRevision
// Purpose:        Set revision number
// Arguments:      Int_t ManufactIndex  -- revision index as given by manufacturer
// Results:        --
// Exceptions:
// Description:    Defines module revision.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ManufactIndex &= 0xF;
	switch (ManufactIndex) {
		case TMrbDGFData::kOrigRevD:
			fRevision.Set(TMrbDGFData::kRevD, "D");
			break;
		case TMrbDGFData::kOrigRevE:
			fRevision.Set(TMrbDGFData::kRevE, "E");
			break;
		default:
			fRevision.Set(TMrbDGFData::kRevUnknown, "unknown");
			break;
	}
}

TMrbNamedX * TMrbDGF::GetRevision(Bool_t Renew) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::GetRevision
// Purpose:        Return module revision
// Arguments:      Bool_t Renew           -- read again from module if kTRUE
// Results:        TMrbNamedX * Revision  -- revision (code & 1char-id)
// Exceptions:
// Description:    Defines module revision.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFData::EMrbDGFRevision rev;

	if (Renew || fRevision.GetIndex() == TMrbDGFData::kRevUnknown) {
		Int_t cVal;
		if (!fCamac.ExecCnaf(fCrate, fStation, A(13), F(1), cVal, kTRUE)) {	// exec cnaf, 16 bit
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation << ".A13.F1 failed" << endl;
			gMrbLog->Flush(this->ClassName(), "GetParValue");
		}
		rev = (TMrbDGFData::EMrbDGFRevision) (cVal & 0xf);
		this->SetRevision(rev);
		if (fRevision.GetIndex() == TMrbDGFData::kRevUnknown) {
			gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
							<< ": Module revision UNKNOWN (" << rev << ") - using RevD" << endl;
			gMrbLog->Flush(this->ClassName(), "GetRevision");
			this->SetRevision(TMrbDGFData::kOrigRevD);
		}
	}

	return(&fRevision);
}

void TMrbDGF::SetVerboseMode(Bool_t VerboseFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGF::SetVerboseMode
// Purpose:        Turn verbosity flag on/off
// Arguments:      Bool_t VerboseFlag  -- kTRUE/kFALSE
// Results:        --
// Exceptions:
// Description:    Verbose mode on/off
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbDGFData) gMrbDGFData->SetVerboseMode(VerboseFlag);
}

UShort_t TMrbDGF::ReadDspAddr(Int_t Addr) {
	Int_t startAddr;
	Int_t nofParams;
	TArrayI cData;

	startAddr = Addr;
	if (!this->WriteTSAR(startAddr)) return(kFALSE); 							// where to read from
	nofParams = 1;
	cData.Set(nofParams);
	if (fCamac.BlockXfer(fCrate, fStation, A(0), F(0), cData, 0, nofParams, kTRUE) == -1) {	// start block xfer, 16 bit
		gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
						<< ".A0.F0 failed - DSPAddr=0x" << setbase(16) << startAddr
						<< setbase(10) << ", wc=" << nofParams << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDspAddr");
		return(kFALSE);
	}
	return(cData[0]);
}
