//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    How to store experiment data in a hit
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrHit.cxx,v 1.4 2007-08-16 08:09:39 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TRandom.h"

#include "TMrbAnalyze.h"
#include "TMrbString.h"
#include "TUsrHit.h"

extern TMrbAnalyze * gMrbAnalyze;

ClassImp(TUsrHit)

TUsrHit::TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Store data of a single hit
// Arguments:      Int_t BufferNumber     -- buffer number
//                 Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t BufferTimeHi  -- event time, hi order
//                 UShort_t EventTimeHi   -- ..., middle
//                 UShort_t FastTrigTime  -- ..., low
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetBufferNumber(BufferNumber);
	this->SetEventNumber(EventNumber);
	this->SetModuleNumber(ModuleNumber);
	this->SetChannel(Channel);
	this->SetChannelTime(BufferTimeHi, EventTimeHi, FastTrigTime);
	this->ClearData();
	if (Data) this->CopyData(Data, NofData);
}

TUsrHit::TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Store data of a single hit
// Arguments:      Int_t BufferNumber     -- buffer number
//                 Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t * ChannelTime -- channel time (48 bit)
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ChannelTime) {
		this->SetChannelTime(*ChannelTime, *(ChannelTime + 1), *(ChannelTime + 2));
	} else {
		this->SetChannelTime(0, 0, 0);
	}
	this->SetBufferNumber(BufferNumber);
	this->SetEventNumber(EventNumber);
	this->SetModuleNumber(ModuleNumber);
	this->SetChannel(Channel);
	this->ClearData();
	if (Data) this->CopyData(Data, NofData);
}

Int_t TUsrHit::Compare(const TObject * Hit) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Compare
// Purpose:        Compare time stamps
// Arguments:      TObject * hit    -- hit to be compared
// Results:        -1 / 0 / 1
// Exceptions:
// Description:    Compares to hits. Used by Sort() method.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long64_t t1, t2;
	t1 = ((TUsrHit *) this)->GetChannelTime();
	t2 = ((TUsrHit *) Hit)->GetChannelTime();
	if (t1 == t2)	return(0);
	else			return(t1 > t2 ? 1 : -1);
}

Int_t TUsrHit::Compare(Long64_t ChannelTime) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Compare
// Purpose:        Compare time stamps
// Arguments:      Long64_t ChannelTime   -- channel time
// Results:        -1 / 0 / 1
// Exceptions:
// Description:    Compares event time with given value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long64_t t;
	t = ((TUsrHit *) this)->GetChannelTime();
	if (t == ChannelTime)	return(0);
	else					return(t > ChannelTime ? 1 : -1);
}

Double_t TUsrHit::GetCalEnergy(Bool_t Randomize) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::GetCalEnergy
// Purpose:        Return energy calibrated
// Arguments:      Bool_t Randomize  -- add random number if kTRUE
// Results:        Double_t energy   -- calibrated energy value
// Exceptions:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * cal = gMrbAnalyze->GetCalibration(fModuleNumber, fChannel);
	Double_t e = fData[kHitEnergy];
	if (cal) {
		if(e < cal->GetXmin() || e > cal->GetXmax()) return(0);
		if (Randomize && (cal->GetParameter(1) != 1.0)) e += gRandom->Rndm() - 0.5;
		e = cal->Eval(e);
	}
	return(e);
}

Double_t TUsrHit::GetDCorrEnergy(Bool_t Randomize) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::GetDCorrEnergy
// Purpose:        Return energy calibrated and doppler-corrected
// Arguments:      Bool_t Randomize  -- add random number if kTRUE
// Results:        Double_t energy   -- calibrated energy value
// Exceptions:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dcfact;
	TF1 * cal = gMrbAnalyze->GetCalibration(fModuleNumber, fChannel);
	gMrbAnalyze->GetDCorr(fModuleNumber, fChannel, dcfact);
	Double_t e = fData[kHitEnergy];
	if (cal) {
		if(e < cal->GetXmin() || e > cal->GetXmax()) return(0);
		if (Randomize && (cal->GetParameter(1) != 1.0)) e += gRandom->Rndm() - 0.5;
		e = cal->Eval(e);
	}
	return(dcfact * e);
}

void TUsrHit::Print(ostream & Out, Bool_t PrintNames) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Print
// Purpose:        Print data
// Arguments:      ostream & Out      -- output stream
//                 Bool_t PrintNames  -- kTRUE -> print module & param names
// Results:        --
// Exceptions:
// Description:    Prints one line of data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * sp;
	TMrbString moduleName, paramName;
	
	if (PrintNames) {
		sp = gMrbAnalyze->GetModuleName(fModuleNumber);
		if (sp) {
			moduleName = sp;
			moduleName += "(";
			moduleName += fModuleNumber;
			moduleName += ")";
			sp = gMrbAnalyze->GetParamName(fModuleNumber, fChannel);
			if (sp) {
				paramName = sp;
				paramName += "(";
				paramName += fChannel;
				paramName += ")";
			} else {
				paramName = fChannel;
			}
		} else {
			moduleName = fModuleNumber;
			paramName = fChannel;
		}
		Out << Form("      %13d%13s%13d%13s%18lld%13d\n",
						fBufferNumber,
						moduleName.Data(),
						fEventNumber,
						paramName.Data(),
						fChannelTime,
						fData[kHitEnergy]);
	} else {
		Out << Form("      %13d%13d%13d%13d%18lld%13d\n",
						fBufferNumber,
						fModuleNumber,
						fEventNumber,
						fChannel,
						fChannelTime,
						fData[kHitEnergy]);
	}
}
	
const Char_t * TUsrHit::ChannelTime2Ascii(TString & TimeString) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::ChannelTime2Ascii
// Purpose:        Convert 48-bit time to ascii
// Arguments:      TString & TimeString  -- where to store event time
// Results:        Char_t * TimeString   -- same as arg#1
// Exceptions:
// Description:    Converts a 48-bit event time to ascii.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TimeString = Form("%18lld", fChannelTime);
	TimeString = TimeString.Strip(TString::kBoth);
	return(TimeString.Data());
}

