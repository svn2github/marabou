#ifndef __TMrbKinetics_3655_h__
#define __TMrbKinetics_3655_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbKinetics_3655.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbKinetics_3655     -- timing generator Kinetics-3655
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbKinetics_3655
// Purpose:        Define a timing generator Kinetics-3655
// Description:    Defines a timing generator Kinetics-3655.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbKinetics_3655 : public TMrbCamacModule {

	public:
		enum	EMrbRegisters		{	kRegCycleControl,
										kRegInhibitControl,
										kRegLamMask
									};

		enum	{	kNofOutputs 	=	8		};
		enum	{	kBitFrequency	=	0		};
		enum	{	kMaskFrequency	=	07		};
		enum	{	kBitOutput		=	3		};
		enum	{	kMaskOutput 	=	070 	};
		enum	{	kBitRecycle		=	6		};
		enum	{	kBitInhSet		=	0		};
		enum	{	kMasInhSet		=	07		};
		enum	{	kBitInhClear	=	3		};
		enum	{	kMaskInhClear	=	070 	};

	public:
		TMrbKinetics_3655() {};															// default ctor
		TMrbKinetics_3655(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new timing gen
		~TMrbKinetics_3655() {};														// remove timing gen from list

		Bool_t SetFrequency(Int_t Frequency = 1000000);
		Bool_t SetFrequency(const Char_t * Frequency);
		TMrbNamedX * GetFrequency();

		void SetRecycleMode(Bool_t RecycleFlag = kTRUE);
		Bool_t IsRecycleMode();

		Int_t SetIntervals(Int_t Inv1, Int_t Inv2 = -1, Int_t Inv3 = -1, Int_t Inv4 = -1,
							Int_t Inv5 = -1, Int_t Inv6 = -1, Int_t Inv7 = -1, Int_t Inv8 = -1);
		Int_t GetInterval(Int_t InvNo);
		inline Int_t GetNofIntervals() { return(fNofIntervals); };

		Bool_t SetInhibit(Int_t InhClear, Int_t InhSet);
		inline void DontAssertInhibit() { SetInhibit(-1, -1); };
		inline Bool_t AssertsInhibit() { return(fInhibitFlag); };

		Bool_t SetLamMask(Bool_t Lam1, Bool_t Lam2 = kFALSE, Bool_t Lam3 = kFALSE, Bool_t Lam4 = kFALSE,
							Bool_t Lam5 = kFALSE, Bool_t Lam6 = kFALSE, Bool_t Lam7 = kFALSE, Bool_t Lam8 = kFALSE);
		inline UInt_t GetLamMask();

		inline void SetExtStart(Bool_t StartFlag = kTRUE) { fExtStartFlag = StartFlag; };
		inline Bool_t IsExtStartMode() { return(fExtStartFlag); };

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("Kinetics_3655"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbKinetics_3655.html&"); };

	protected:
		void DefineRegisters(); 			// define camac registers

	protected:
		Bool_t fExtStartFlag;
		Bool_t fInhibitFlag;
		Int_t fNofIntervals;
		TArrayI fIntervals;

	ClassDef(TMrbKinetics_3655, 1) 			// [Config] KINETICS 3655 Timing generator
};

#endif
