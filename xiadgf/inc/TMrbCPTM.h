#ifndef __TMrbCPTM_h__
#define __TMrbCPTM_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbCPTM.h
// Purpose:        Interface to module C_PTM
// Class:          TMrbCPTM            -- base class
// Description:    Class definitions to operate "Clock and Programmable Trigger Module"
// Author:         R. Lutter
// Revision:       $Id: TMrbCPTM.h,v 1.4 2005-04-18 14:02:02 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TNamed.h"
#include "TEnv.h"
#include "TArrayS.h"

#include "TMrbLofNamedX.h"
#include "TMrbEsone.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTMEvent
// Purpose:        Class defs describing module C_PTM
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCPTMEvent : public TObject {

	public:
		TMrbCPTMEvent() { this->Reset(); }; 		// default ctor
		~TMrbCPTMEvent() {}; 						// default dtor

		void Reset();

		inline Long64_t GetTimeStamp() { return(fTimeStamp); };
		inline Int_t GetTimeAux() { return(fTimeAux); };
		Long64_t GetTimeStampAdjusted();
		inline void SetTimeStamp(Long64_t * TsAddr) { fTimeStamp = *TsAddr; };
		inline void SetTimeStamp(Int_t LowWord, Int_t MiddleWord, Int_t HighWord) {
			fTimeStamp = (HighWord << 32) | (MiddleWord << 16) | LowWord;
		};
		inline void SetTimeAux(Int_t TimeAux) { fTimeAux = TimeAux; };

		inline UInt_t GetCounterT1() { return(fCounterT1); };
		inline void SetCounterT1(UInt_t Counts) { fCounterT1 = Counts; };

		inline UInt_t GetCounterT2() { return(fCounterT2); };
		inline void SetCounterT2(UInt_t Counts) { fCounterT2 = Counts; };

		inline UInt_t GetPattern() { return(fPattern); };
		inline void SetPattern(UInt_t Pattern) { fPattern = Pattern; };
		const Char_t * Pattern2Ascii(TString & PatStr);

	 	void Print(Option_t * option) const { TObject::Print(option); };
		void Print(ostream & Out = cout);

	protected:
		Long64_t fTimeStamp;
		Long64_t fTimeStampAdjusted;
		Int_t fTimeAux;
		UInt_t fCounterT1;
		UInt_t fCounterT2;
		UInt_t fPattern;

	ClassDef(TMrbCPTMEvent, 1)		// [C_PTM] Event structure
};
		

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCPTM
// Purpose:        Class defs describing module C_PTM
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCPTM : public TNamed {

	public:
		enum	{	kMrbCptmMaxCodeSize =	128 * 1024	};

	public:
		TMrbCPTM(const Char_t * ModuleName = "CPTM") : TNamed(ModuleName, "Clock & Programmable Trigger Module") {};	// default ctor

		TMrbCPTM(	const Char_t * ModuleName, 								// ctor: host, crate, station
					const Char_t * HostName, Int_t Crate, Int_t Station);

		TMrbCPTM(	const Char_t * ModuleName,
					const Char_t * HostName, const Char_t * CamacAddr);		// ctor: host, camac addr

		~TMrbCPTM() {}; 	// default dtor

		Bool_t SetGeDGG(Int_t Delay, Int_t Width);
		Bool_t SetAuxDGG(Int_t Delay, Int_t Width);
		Bool_t SetGeDelay(Int_t Delay);
		Bool_t SetGeWidth(Int_t Width);
		Bool_t SetAuxDelay(Int_t Delay);
		Bool_t SetAuxWidth(Int_t Width);
		Int_t GetGeDelay();
		Int_t GetGeWidth();
		Int_t GetAuxDelay();
		Int_t GetAuxWidth();

		Bool_t SetMask(Int_t Mask);
		Bool_t SetMask(const Char_t * Mask);
		Int_t GetMask();

		Bool_t SetTimeWindowAux(Int_t Window);
		Int_t GetTimeWindowAux();

		Bool_t SetDac(Int_t DacNo, Int_t DacValue);
		Int_t GetDac(Int_t DacNo);
		inline Bool_t ClearDac(Int_t DacNo) { return(this->SetDac(DacNo, 0)); };

		Bool_t ResetRead();
		Bool_t ResetWrite();
		Bool_t ResetMemory();
		Bool_t ResetDacs();
		Bool_t Reset();

		Bool_t EnableSynch();

		Bool_t DownloadAlteraCode(const Char_t * CodeFile = "cptm.rbf");

		Int_t GetReadAddr();
		Int_t GetWriteAddr();

		Bool_t SaveSettings(const Char_t * SaveFile = "cptm.par");
		Bool_t RestoreSettings(const Char_t * RestoreFile = "cptm.par");

	 	void Print(Option_t * option) const { TObject::Print(option); };
		void Print(ostream & Out = cout);
		void PrintBuffer(ostream & Out = cout) {};

		Bool_t Connect();
		inline Bool_t IsConnected() { return(fCamac.IsConnected()); };
		Bool_t CheckConnect(const Char_t * Method = "CheckConnect");

		Bool_t SetCamacHost(const Char_t * HostName);
		Bool_t SetCrate(Int_t Crate);
		Bool_t SetStation(Int_t Station);
		inline const Char_t * GetCamacHost() { return(fCamacHost); };
		inline Int_t  GetCrate() { return(fCrate); };
		inline Int_t GetStation() { return(fStation); };

		inline TMrbEsone * Camac() { return(&fCamac); }; 				// camac handle

		Bool_t CheckValue(Int_t Value, Int_t MaxValue, const Char_t * ArgName = "Arg", const Char_t * Method = "CheckValue");

		Int_t ReadNext();
		Bool_t ReadEvent(TMrbCPTMEvent & Event);

	protected:
		Bool_t ReadAllDacs(TArrayI & DacBits);
		Bool_t WriteAllDacs(TArrayI & DacBits);
		const Char_t * ConvertMask(TString & Mask, Int_t MaskValue);

	protected:
		TString fCamacHost; 	// host name
		Int_t fCrate;			// crate
		Int_t fStation; 		// station
		TMrbEsone fCamac; 		// esone camac port

	ClassDef(TMrbCPTM, 1)		// [C_PTM] Base class for C_PTM (Clock & Programmable Trigger Module)
};

#endif
