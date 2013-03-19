//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    How to store experiment data in a hit
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrHit.cxx,v 1.9 2011-02-24 08:40:16 Marabou Exp $
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

Bool_t TUsrHit::WriteToSevtData(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::WriteToSevtData
// Purpose:        Write data to subevent
// Arguments:      Int_t Index    -- index within hit data
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes data referred to by index to data field given by
//                 module and hit number, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

      Int_t * paramAddr = (Int_t *) gMrbAnalyze->GetParamAddr(this->GetModuleNumber(), this->GetChannel());
      if (paramAddr == NULL) return(kFALSE);
	  *paramAddr = this->GetData(Index);
	  return(kTRUE);
}

Bool_t TUsrHit::FillHistogram(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::FillHistogram
// Purpose:        Write data to histogram
// Arguments:      Int_t Index    -- index within hit data
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes data referred to by index to histogram given by
//                 module and hit number, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

      TH1F * h = (TH1F *) gMrbAnalyze->GetHistoAddr(this->GetModuleNumber(), this->GetChannel());
      if (h == NULL) return(kFALSE);
	  h->Fill(this->GetData(Index));
	  return(kTRUE);
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

Double_t TUsrHit::GetCalEnergy(Bool_t Randomize, Bool_t WithinLimits) const {
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
		if(WithinLimits && (e < cal->GetXmin() || e > cal->GetXmax())) return(0);
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

void TUsrHit::Print(ostream & Out, Bool_t PrintNames, Bool_t CrLf, Bool_t EnergyLongFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Print
// Purpose:        Print data
// Arguments:      ostream & Out           -- output stream
//                 Bool_t PrintNames       -- kTRUE -> print module & param names
//                 Bool_t CrLf             -- print <cr> at end
//                 Bool_t EnergyLongFlag   -- kTRUE -> energy data is 32bit
// Results:        --
// Exceptions:
// Description:    Prints one line of data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * sp;
	TMrbString moduleName, paramName;

	UInt_t energy = EnergyLongFlag ? *((UInt_t *) &fData[kHitEnergyLong]) : (UInt_t) fData[kHitEnergy];

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
		Out << Form("      %13d%13s%13d%13s%18lld%13d",
						fBufferNumber,
						moduleName.Data(),
						fEventNumber,
						paramName.Data(),
						fChannelTime,
						energy);
	} else {
		Out << Form("      %13d%13d%13d%13d%18lld%13d",
						fBufferNumber,
						fModuleNumber,
						fEventNumber,
						fChannel,
						fChannelTime,
						energy);
	}
	if (CrLf) Out << endl;
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

Long64_t TUsrHit::GetChannelTimeCorr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::GetChannelTimeCorr
// Purpose:        Get time stamp
// Arguments:      --
// Results:        Long64_t TimeStamp   -- 48bit time stamp
// Exceptions:
// Description:    Returns time stamp corrected by module offset
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long64_t ts = this->GetChannelTime() - gMrbAnalyze->GetTimeOffset(this->GetModuleNumber());
	return ts;
}

