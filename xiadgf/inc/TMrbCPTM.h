#ifndef __TMrbCPTM_h__
#define __TMrbCPTM_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbCPTM.h
// Purpose:        Interface to module C_PTM
// Class:          TMrbCPTM            -- base class
// Description:    Class definitions to operate "Clock and Programmable Trigger Module"
// Author:         R. Lutter
// Revision:       $Id: TMrbCPTM.h,v 1.3 2005-04-18 10:02:50 rudi Exp $       
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

		Bool_t DownloadAlteraCode(const Char_t * CodeFile = "altera.rbf");

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
