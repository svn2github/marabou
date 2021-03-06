#ifndef __TUsrHit_h__
#define __TUsrHit_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit.h
// Purpose:        Class to describe a hit
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrHit.h,v 1.10 2011-02-24 08:40:16 Marabou Exp $
// Date:
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>

#include "TObject.h"
#include "TString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Class to store hit data
// Description:    Describes a single hit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrHit : public TObject {

	public:
// hit data layout depending on param RUNTASK
		enum	{	kHitFastTrigger	=	0	};
		enum	{	kHitEnergy		=	1	};
		enum	{	kHitXiaPSA		=	2	};
		enum	{	kHitUserPSA		=	3	};
		enum	{	kHitGSLTHi		=	4	};
		enum	{	kHitGSLTMi		=	5	};
		enum	{	kHitGSLTLo		=	6	};
		enum	{	kHitRealTimeHi 	=	7	};
		enum	{	kHitLast		=	kHitRealTimeHi	};
		enum	{	kHitWaveForm	=	kHitLast + 1	};
		enum	{	kMaxHitData 	=	kHitLast + 1	};

// alternative layout used by martin lauer's PSA code
		enum	{	kHitPSA1		=	2	};
		enum	{	kHitPSA_T0		=	kHitPSA1	};
		enum	{	kHitPSA2		=	3	};
		enum	{	kHitPSA_Tslope	=	kHitPSA2	};
		enum	{	kHitPSA3		=	4	};
		enum	{	kHitPSA_Qmax	=	kHitPSA3	};
		enum	{	kHitPSA4		=	5	};
		enum	{	kHitPSA_Tqmax	=	kHitPSA4	};
		enum	{	kHitPSA5		=	6	};
		enum	{	kHitPSA_Error	=	kHitPSA5	};
		enum	{	kHitPSA6		=	7	};
		enum	{	kHitPSA_T90		=	kHitPSA6	};

// alternative layout used for 32 bit energy data

		enum	{	kHitEnergyLong	=	2	};

// alternative layout used by c_ptm module
		enum	{	kHitPattern 	=	2	};	// 32 bit hit pattern
		enum	{	kHitCounterT1	=	4	};	// 32 bit counter T1
		enum	{	kHitCounterT2	=	6	};	// 32 bit counter T2

// alternative layout used by mux module
		enum	{	kHitMuxModNo	=	2	};	// module number for multiplexer
		enum	{	kHitMuxChannel	=	3	};	// channel number for multiplexer

// alternative layout used by madc32
		enum	{	kHitMadc32Res	=	2	};	// adc resolution

// alternative layout used by gassiplex
		enum	{	kHitGassiModule		=	2	};	// module
		enum	{	kHitGassiChannel	=	3	};	// channel
		enum	{	kHitGassiPort		=	4	};	// port
		enum	{	kHitGassiRace		=	5	};	// race
		enum	{	kHitGassiSector		=	6	};	// sector
		enum	{	kHitGassiDprRest	=	7	};	// rest

	public:
		TUsrHit() { this->Reset(); };
		TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData);	// ctor
		TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData);	// ctor
		~TUsrHit() {};														// default dtor

		virtual Int_t Compare(const TObject * Hit) const; 				// hits have to be sortable in time
		Int_t Compare(Long64_t ChannelTime) const;

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, Bool_t PrintNames = kFALSE, Bool_t CrLf = kTRUE, Bool_t EnergyLongFlag = kFALSE) const;		// print hit data
		inline void Print(Bool_t PrintNames = kFALSE, Bool_t CrLf = kTRUE, Bool_t EnergyLongFlag = kFALSE) const { Print(cout, PrintNames, CrLf, EnergyLongFlag); };

		inline Int_t GetBufferNumber() const { return(fBufferNumber); };		// get data members
		inline Int_t GetEventNumber() const { return(fEventNumber); };
		inline Int_t GetModuleNumber() const { return(fModuleNumber); };
		inline Int_t GetChannel() const { return(fChannel); };

		inline Long64_t GetChannelTime() const { return(fChannelTime); };		// get time stamp
		const Char_t * ChannelTime2Ascii(TString & TimeString) const;
		Long64_t GetChannelTimeCorr();		// get time stamp corrected by applying module offset

		inline UShort_t * GetDataAddr() { return(fData); };
		inline UShort_t GetData(Int_t Index = TUsrHit::kHitEnergy) const { return(fData[Index]); };

		inline UShort_t GetEnergy() const { return(fData[kHitEnergy]); };
		inline UShort_t GetFastTrigger() const { return(fData[kHitFastTrigger]); };
		Double_t GetCalEnergy(Bool_t Randomize = kTRUE, Bool_t WithinLimits = kFALSE) const;
		Double_t GetDCorrEnergy(Bool_t Randomize = kTRUE) const;

		inline UInt_t GetDataLong(Int_t Index = TUsrHit::kHitEnergyLong) const { UInt_t * dp = (UInt_t *) &fData[Index]; return(*dp); };
		inline UInt_t GetEnergyLong() const { UInt_t * dp = (UInt_t *) &fData[kHitEnergyLong]; return(*dp); };

		inline Int_t GetMuxModuleNumber() const { return((Int_t) fData[kHitMuxModNo]); };
		inline void SetMuxModuleNumber(Int_t ModuleNumber) { fData[kHitMuxModNo] = (UShort_t) ModuleNumber; };
		inline Int_t GetMuxChannel() const { return((Int_t) fData[kHitMuxChannel]); };
		inline void SetMuxChannel(Int_t Channel) { fData[kHitMuxChannel] = (UShort_t) Channel; };

		inline UInt_t GetPatternWord() { return(* (UInt_t *) &fData[kHitPattern]); }; // for c_ptm modules only
		inline UInt_t GetCounterT1() { return(* (UInt_t *) &fData[kHitCounterT1]); };
		inline UInt_t GetCounterT2() { return(* (UInt_t *) &fData[kHitCounterT2]); };

		inline Int_t GetMadc32Resolution() const { return((Int_t) fData[kHitMadc32Res]); }; // for mesytec madc32
		inline void SetMadc32Resolution(Int_t Resolution) { fData[kHitMadc32Res] = (UShort_t) Resolution; };

		virtual inline Bool_t IsSortable() const { return(kTRUE); };	// hit may be sorted by time stamp

		inline void SetBufferNumber(Int_t BufferNumber) { fBufferNumber = BufferNumber; };		// set data members
		inline void SetEventNumber(Int_t EventNumber) { fEventNumber = EventNumber; };
		inline void SetModuleNumber(Int_t ModuleNumber) { fModuleNumber = ModuleNumber; };
		inline void SetChannel(Int_t Channel) { fChannel = Channel; };
		inline Long64_t SetChannelTime(UShort_t * ChannelTime) {
			fChannelTime = 0;
			for (Int_t i = 0; i < 3; i++) fChannelTime = (fChannelTime << 16) | *ChannelTime++;
			return(fChannelTime);
		};
		inline Long64_t SetChannelTime(UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime) {
			fChannelTime = ((ULong64_t) BufferTimeHi << 32) | ((ULong64_t) EventTimeHi << 16) | (ULong64_t) FastTrigTime;
			return(fChannelTime);
		};
		inline Long64_t SetChannelTime(TUsrHit * Hit) {
			fChannelTime = Hit->GetChannelTime();
			return(fChannelTime);
		};

		inline void CopyData(UShort_t * Data, Int_t NofData, Int_t Offset = 0) {		// copy hit data
			if (Data) for (Int_t i = 0; i < NofData; i++) fData[i + Offset] = *Data++;
		};

		inline void SetData(UShort_t Data, Int_t Offset = TUsrHit::kHitEnergy) { fData[Offset] = Data; }
		inline void SetDataLong(UInt_t Data, Int_t Offset = TUsrHit::kHitEnergyLong) { UInt_t * dp = (UInt_t *) &fData[Offset]; *dp = Data; }

		inline void CopyData(TUsrHit * Hit, Int_t NofData, Int_t Offset = 0) {
			UShort_t * dp = Hit->GetDataAddr() + Offset;
			for (Int_t i = 0; i < NofData; i++) fData[i + Offset] = *dp++;
		};

		inline virtual Int_t GetNofData() { return(fNofData); };

		Bool_t WriteToSevtData(Int_t Index = TUsrHit::kHitEnergy);				// write data to subevent
		Bool_t FillHistogram(Int_t Index = TUsrHit::kHitEnergy);				// fill histogram with data

		inline virtual void Reset() {
			fBufferNumber = -1;
			fEventNumber = -1;
			fModuleNumber = -1;
			fChannel = -1;
			fChannelTime = 0LL;
			this->ClearData();
		};

		inline virtual void ClearData() { memset(fData, 0, kMaxHitData * sizeof(UShort_t)); };	// clear data

	protected:
		Int_t fBufferNumber; 			// buffer number
		Int_t fEventNumber; 			// event index within buffer
		Int_t fModuleNumber;			// module serial number
		Int_t fChannel; 			// module channel
		Long64_t fChannelTime; 			// time stamp generated by dgf clock bus (48 bits unsigned!)
		Int_t fNofData; 			// number of data words
		UShort_t fData[kMaxHitData]; 		// data (fast trigger, energy, etc)

	ClassDef(TUsrHit, 1)				// [Analyze] Hit
};

#endif
