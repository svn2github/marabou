//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbKinetics_3655.cxx
// Purpose:        MARaBOU configuration: Kinetics-3655 Timing Generator
// Description:    Implements class methods to handle Kinetics-3655 modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TDirectory.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbCamacRegister.h"
#include "TMrbKinetics_3655.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbKinetics_3655)

const SMrbNamedXShort kMrbLofFrequencies[] =
		{
			{0,		"1Hz"},
			{1,		"10Hz"},
			{2,		"100Hz"},
			{3,		"1kHz"},
			{4,		"10kHz"},
			{5,		"100kHz"},
			{6,		"1MHz"},
			{7,		"Extern"},
			{0, 	NULL	}
		};

TMrbKinetics_3655::TMrbKinetics_3655(const Char_t * ModuleName, const Char_t * ModulePosition) :
									TMrbCamacModule(ModuleName, "Kinetics_3655", ModulePosition, 0, 0, 0) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655
// Purpose:        Create a module Kinetics-3655
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type Kinetics-3655.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString codeFile;
	UInt_t mTypeBits;
	TString mType;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (gMrbConfig == NULL) {
			gMrbLog->Err() << "No config defined" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->FindModule(ModuleName)) {
			gMrbLog->Err() << ModuleName << ": Module name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle("Kinetics 3655 Timing Generator"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac;
			mType = gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				this->DefineRegisters();
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
		this->SetFrequency("1MHz"); 					// default clock 1 MHz
		this->SetRecycleMode(kFALSE);					// recycle mode off
		this->SetExtStart(kFALSE);						// started internally via camac cmd
		this->DontAssertInhibit();						// don't assert inhibit
		fNofIntervals = 0;								// no output pulses yet defined
		fIntervals.Set(TMrbKinetics_3655::kNofOutputs); // reset interval values to 0
		fIntervals.Reset();
		this->AllocateHistograms(kFALSE);				// don't allocate histograms
	}
}

void TMrbKinetics_3655::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::DefineRegisters
// Purpose:        Define camac registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines registers:
//                 cycle control, inhibit control, and lam mask.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

// cycleControl
	kp = new TMrbNamedX(TMrbKinetics_3655::kRegCycleControl, "CycleControl");
	rp = new TMrbCamacRegister(this, 0, kp, "F17.A0", "F17.A0", NULL, 0, 0, 0177);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// inhibitControl
	kp = new TMrbNamedX(TMrbKinetics_3655::kRegInhibitControl, "InhibitControl");
	rp = new TMrbCamacRegister(this, 0, kp, "F17.A9", "F17.A9", NULL, 0, 0, 077);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// lamMask
	kp = new TMrbNamedX(TMrbKinetics_3655::kRegLamMask, "LamMask");
	rp = new TMrbCamacRegister(this, 0, kp, "F17.A13", "F17.A13", NULL, 0, 0, 0377);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbKinetics_3655::SetFrequency(Int_t Frequency) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetFrequency
// Purpose:        Define clock frequency
// Arguments:      Int_t Frequency     -- frequency (Hz)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the frequency value in the cylce control reg.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;
	Int_t freqVal;
	Int_t cycleControl;

	switch (Frequency) {
		case 0: 		freqVal = 7; break;
		case 1: 		freqVal = 0; break;
		case 10: 		freqVal = 1; break;
		case 100: 		freqVal = 2; break;
		case 1000:  	freqVal = 3; break;
		case 10000: 	freqVal = 4; break;
		case 100000: 	freqVal = 5; break;
		case 1000000: 	freqVal = 6; break;
		default:
			gMrbLog->Err() << "Wrong frequency setting - " << Frequency << endl;
			gMrbLog->Flush(this->ClassName(), "SetFrequency");
			return(kFALSE);
	}
	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	cycleControl = rp->Get();
	rp->Set((cycleControl & ~TMrbKinetics_3655::kMaskFrequency) | freqVal);
	return(kTRUE);
}

Bool_t TMrbKinetics_3655::SetFrequency(const Char_t * Frequency) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetFrequency
// Purpose:        Define clock frequency
// Arguments:      Char_t * Frequency     -- frequency
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the frequency value in the cylce control reg.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;
	Int_t cycleControl;
	TMrbLofNamedX freqList;
	TMrbNamedX * nx;

	freqList.AddNamedX(kMrbLofFrequencies);
	nx = freqList.FindByName(Frequency);
	if (nx == NULL) {
		gMrbLog->Err() << "Wrong frequency setting - " << Frequency << endl;
		gMrbLog->Flush(this->ClassName(), "SetFrequency");
		return(kFALSE);
	}
	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	cycleControl = rp->Get();
	rp->Set((cycleControl & ~TMrbKinetics_3655::kMaskFrequency) | nx->GetIndex());
	return(kTRUE);
}

TMrbNamedX * TMrbKinetics_3655::GetFrequency() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::GetFrequency
// Purpose:        Return clock frequency
// Arguments:      --
// Results:        TMrbNamedX * Frequency   -- frequency
// Exceptions:
// Description:    Returns the frequency value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;
	Int_t freqVal;
	TMrbLofNamedX freqList;

	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	freqVal = rp->Get() & TMrbKinetics_3655::kMaskFrequency;
	freqList.AddNamedX(kMrbLofFrequencies);
	return(freqList.FindByIndex(freqVal));
}

void TMrbKinetics_3655::SetRecycleMode(Bool_t RecycleFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetRecycleMode
// Purpose:        Define recycle mode
// Arguments:      Bool_t RecycleFlag    -- kTRUE if recycle mode on
// Results:        --
// Exceptions:
// Description:    Sets the recycle bit in the cylce control reg.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;
	Int_t cycleControl;

	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	cycleControl = rp->Get();
	if (RecycleFlag) {
		rp->Set(cycleControl | BIT(TMrbKinetics_3655::kBitRecycle));
	} else {
		rp->Set(cycleControl & ~BIT(TMrbKinetics_3655::kBitRecycle));
	}
}

Bool_t TMrbKinetics_3655::IsRecycleMode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::GetFrequency
// Purpose:        Return recycle bit
// Arguments:      --
// Results:        kTRUE/KFALSE
// Exceptions:
// Description:    Returns the recycle bit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	return((rp->Get() & BIT(TMrbKinetics_3655::kBitRecycle)) != 0);
}

Int_t TMrbKinetics_3655::SetIntervals(Int_t Inv1, Int_t Inv2, Int_t Inv3, Int_t Inv4,
										Int_t Inv5, Int_t Inv6, Int_t Inv7, Int_t Inv8) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetIntervals
// Purpose:        Define counting intervals
// Arguments:      Int_t InvX          -- interval X
// Results:        Int_t NofIntervals  -- number of active intervals
// Exceptions:
// Description:    Defines counting intervals 1 ... 8
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t invVal[8];
	Int_t cycleControl;
	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

	invVal[0] = Inv1;
	invVal[1] = Inv2;
	invVal[2] = Inv3;
	invVal[3] = Inv4;
	invVal[4] = Inv5;
	invVal[5] = Inv6;
	invVal[6] = Inv7;
	invVal[7] = Inv8;

	fIntervals.Reset();
	fNofIntervals = 0;
	for (Int_t i = 0; i < 8; i++) {
		if (invVal[i] == -1) break;
		if (invVal[i] <= 0 || invVal[i] > 65536) {
			gMrbLog->Err()	<< "Interval (#" << i + 1 << ") out of range - " << invVal[i]
							<< " (should be in [1,65536])" << endl;
			gMrbLog->Flush(this->ClassName(), "SetIntervals");
			fIntervals.Reset();
			fNofIntervals = 0;
			break;
		}
		fIntervals[i] = invVal[i];
		fNofIntervals++;
	}

	kp = this->FindRegister(TMrbKinetics_3655::kRegCycleControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	cycleControl = rp->Get();
	rp->Set((cycleControl & ~TMrbKinetics_3655::kMaskOutput) | ((fNofIntervals - 1) << TMrbKinetics_3655::kBitOutput));
	return(fNofIntervals);
}

Int_t TMrbKinetics_3655::GetInterval(Int_t InvNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::GetInterval
// Purpose:        Return counting interval
// Arguments:      Int_t InvNo          -- interval number (1 ... 8)
// Results:        Int_t Interval       -- setting
// Exceptions:
// Description:    Returns interval settings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNofIntervals == 0) {
		gMrbLog->Err()	<< "No intervals defined" << endl;
		gMrbLog->Flush(this->ClassName(), "GetInterval");
		return(-1);
	}

	if (InvNo <= 0 || InvNo > fNofIntervals) {
		gMrbLog->Err()	<< "Wrong interval number - " << InvNo
						<< " (should be in [1," << fNofIntervals << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetInterval");
		return(-1);
	}
	return(fIntervals[InvNo - 1]);
}

Bool_t TMrbKinetics_3655::SetInhibit(Int_t InhClear, Int_t InhSet) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetInhibit
// Purpose:        Define assertion of dataway inhibit
// Arguments:      Int_t InhClear    -- point to clear inhibit
//                 Int_t InhSet      -- point to set inhibit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets the inhibit control reg.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

	kp = this->FindRegister(TMrbKinetics_3655::kRegInhibitControl);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();

	if (InhClear == -1 && InhSet == -1) {
		fInhibitFlag = kFALSE;
		rp->Set(0);
		return(kTRUE);
	}

	if (fNofIntervals == 0) {
		gMrbLog->Err()	<< "No intervals defined - can't set inhibit points" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInhibit");
		return(kFALSE);
	}
	if (InhClear <= 0 || InhClear > fNofIntervals) {
		gMrbLog->Err()	<< "Wrong interval number - " << InhClear
						<< " (should be in [1," << fNofIntervals << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInhibit");
		return(kFALSE);
	}
	if (InhSet <= 0 || InhSet > fNofIntervals) {
		gMrbLog->Err()	<< "Wrong interval number - " << InhSet
						<< " (should be in [1," << fNofIntervals << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInhibit");
		return(kFALSE);
	}

	rp->Set(((InhClear - 1) << TMrbKinetics_3655::kBitInhClear) | (InhSet - 1));
	fInhibitFlag = kTRUE;
	return(kTRUE);
}

Bool_t TMrbKinetics_3655::SetLamMask(Bool_t Lam1, Bool_t Lam2, Bool_t Lam3, Bool_t Lam4,
										Bool_t Lam5, Bool_t Lam6, Bool_t Lam7, Bool_t Lam8) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::SetLamMask
// Purpose:        Define lam mask
// Arguments:      Int_t LamX          -- lam X
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines lam bits 1 ... 8
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t highLam;
	UInt_t lamMask;

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

	kp = this->FindRegister(TMrbKinetics_3655::kRegLamMask);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();

	if (fNofIntervals == 0) {
		gMrbLog->Err()	<< "No intervals defined - can't set LAM mask" << endl;
		gMrbLog->Flush(this->ClassName(), "SetLamMask");
		return(kFALSE);
	}

	lamMask = 0;
	highLam = 0;
	if (Lam1) { lamMask |= BIT(0); highLam = 1; }
	if (Lam2) { lamMask |= BIT(1); highLam = 2; }
	if (Lam3) { lamMask |= BIT(2); highLam = 3; }
	if (Lam4) { lamMask |= BIT(3); highLam = 4; }
	if (Lam5) { lamMask |= BIT(4); highLam = 5; }
	if (Lam6) { lamMask |= BIT(5); highLam = 6; }
	if (Lam7) { lamMask |= BIT(6); highLam = 7; }
	if (Lam8) { lamMask |= BIT(7); highLam = 8; }
	if (highLam > fNofIntervals) {
		gMrbLog->Err()	<< "Wrong LAM settings - highest LAM is " << highLam
						<< " (should be in [1," << fNofIntervals << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "Setsk");
		return(kFALSE);
	}
	rp->Set(lamMask);
	return(kTRUE);
}

UInt_t TMrbKinetics_3655::GetLamMask() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::GetLamMask
// Purpose:        Return lam bits
// Arguments:      --
// Results:        UInt_t LamMask   -- lam bits
// Exceptions:
// Description:    Returns active lam bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbCamacRegister * rp;

	kp = this->FindRegister(TMrbKinetics_3655::kRegLamMask);	
	rp = (TMrbCamacRegister *) kp->GetAssignedObject();
	return(rp->Get());
}

Bool_t TMrbKinetics_3655::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::MakeReadoutCode
// Purpose:        Write a piece of code for a Kinetics-3655
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of an Kinetics 3655 module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString mnemoLC, mnemoUC;
	TString iniTag;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode("%C%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$inhibitControl", this->Get(TMrbKinetics_3655::kRegInhibitControl), 8);
			fCodeTemplates.Substitute("$lamMask", (Int_t) this->GetLamMask(), 8);
			fCodeTemplates.WriteCode(RdoStrm);
			for (i = 0; i < fNofIntervals; i++) {
				fCodeTemplates.InitializeCode("%WI%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$inv0No", i);
				fCodeTemplates.Substitute("$inv1No", i + 1);
				fCodeTemplates.Substitute("$invVal", this->GetInterval(i + 1));
				fCodeTemplates.WriteCode(RdoStrm);
			}
			if (this->AssertsInhibit()) {
				fCodeTemplates.InitializeCode("%AI%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
		case TMrbConfig::kModuleStartAcquisition:
			iniTag = this->IsExtStartMode() ? "%XS%" : "%IS%";
			fCodeTemplates.InitializeCode(iniTag.Data());
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$cycleControl", this->Get(TMrbKinetics_3655::kRegCycleControl), 8);
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleStopAcquisition:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbKinetics_3655::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655::MakeReadoutCode
// Purpose:        Write a piece of code for a Kinetics-3655
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of an Kinetics-3655 module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacChannel * chn;
	TString mnemoLC, mnemoUC;

	chn = (TMrbCamacChannel *) Channel;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	switch (TagIndex) {}
	return(kTRUE);
}
