#ifndef __TMrbEsoneCnaf_h__
#define __TMrbEsoneCnaf_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/inc/TMrbEsoneCnaf.h
// Purpose:        Interface to the ESONE client library
// Class:          TMrbEsoneCnaf         -- C.N.A.F handling
// Description:    Class definitions to establish an
//                 ESONE client/server connection.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"
#include "TArrayI.h"
#include "TArrayL.h"

#include "TMrbString.h"
#include "TMrbEsoneCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsoneCnaf
// Purpose:        Handle Camac CNAF definitions
// Methods:        Ascii2Int  -- decode B.C.N.A.F to int values
// Description:    Converts Camac [B]CNAFs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEsoneCnaf : public TObject {

	public:
		enum EMrbEsoneCnaf		{	kCnafBranch 		=	BIT(0),			// cnaf code names
									kCnafCrate			=	BIT(1),
									kCnafStation		=	BIT(2),
									kCnafAddr			=	BIT(3),
									kCnafFunction		=	BIT(4),
									kCnafData			=	BIT(5),
									kCnafQ				=	BIT(6),
									kCnafX				=	BIT(7)
								};

		enum					{	kEsoneCnafBits		=	kCnafCrate | kCnafStation | kCnafAddr | kCnafFunction | kCnafData	};

		enum EMrbCnafType		{	kCnafTypeUndefined	=	0,				// cnaf types
									kCnafTypeRead		=	BIT(0),
									kCnafTypeControl	=	BIT(1),
									kCnafTypeWrite		=	BIT(2),
									kCnafTypeReadStatus =	BIT(3),
									kCnafTypeAny		=	kCnafTypeRead | kCnafTypeControl | kCnafTypeWrite
								};

		enum EMrbCamacFunction	{	kCF_RD1,        /* read group 1 register */
									kCF_RD2,        /* read group 2 register */
									kCF_RC1,        /* read and clear group 1 register */
									kCF_RCM,        /* read complement of group 1 register */
									kCF_TLM = 8,    /* test LAM */
									kCF_CL1,        /* clear group 1 register */
									kCF_CLM,        /* clear LAM */
									kCF_CL2,        /* clear group 2 register */
									kCF_WT1 = 16,   /* write group 1 register */
									kCF_WT2,        /* write group 2 register */
									kCF_SS1,        /* selective set group 1 register */
									kCF_SS2,        /* selective set group 2 register */
									kCF_SC1 = 21,   /* selective clear group 1 register */
									kCF_SC2 = 23,   /* selective clear group 2 register */
									kCF_DIS,        /* disable */
									kCF_XEQ,        /* execute */
									kCF_ENB,        /* enable */
									kCF_TST         /* test */
								};

		enum EMrbCamacAction	{	kCANone 					= 	0x0,
									kCASingle					=	BIT(0),									
									kCAMultiple					=	BIT(1),									
									kCAAddrScan 				=	BIT(2),
									kCABlockXfer 				=	BIT(3),
									kCAQDriven					=	BIT(4),
									kCA16Bit	 				=	BIT(5),
									kCA24Bit			 		=	BIT(6),
									kCA_cfsa					=	kCASingle | kCA24Bit,
									kCA_cssa					=	kCASingle | kCA16Bit,
									kCA_cfga					=	kCAMultiple | kCA24Bit,
									kCA_csga					=	kCAMultiple | kCA16Bit,
									kCA_cfmad					=	kCAAddrScan | kCA24Bit,
									kCA_csmad					=	kCAAddrScan | kCA16Bit,
									kCA_cfubc					=	kCABlockXfer | kCA24Bit,
									kCA_csubc					=	kCABlockXfer | kCA16Bit,
									kCA_cfubr					=	kCABlockXfer | kCAQDriven | kCA24Bit,
									kCA_csubr					=	kCABlockXfer | kCAQDriven | kCA16Bit
								};

	public:
		TMrbEsoneCnaf() { Reset(); };						// default ctor
		~TMrbEsoneCnaf() {}; 								// default dtor

		Bool_t Ascii2Int(const Char_t * Cnaf); 				// decode C.N.A.F from ascii to int values
		const Char_t * Int2Ascii(Bool_t DataFlag = kTRUE);	// convert to ascii

		Bool_t Set(Int_t Crate, Int_t Station = -1, Int_t Subaddr = -1, Int_t Function = -1, Int_t Data = -1);	// set cnaf
		Bool_t SetC(Int_t Crate);
		Bool_t SetN(Int_t Station);
		Bool_t SetA(Int_t Addr);
		Bool_t SetF(Int_t Function);
		Bool_t SetData(Int_t Data, EMrbCnafType Type = kCnafTypeUndefined);
		inline void ClearStatus() { fStatus = 0; };
		Bool_t ClearData(EMrbCnafType Type = kCnafTypeUndefined);
		inline void SetX() { fStatus |= kEsoneX; };
		inline void SetQ() { fStatus |= kEsoneQ; };
		inline void SetXQ() { fStatus |= (kEsoneQ|kEsoneX); };
		inline void SetError() { fStatus |= kEsoneError; };

		inline Int_t GetC() { return(fCrate); };									// get cnaf
		inline Int_t GetN() { return(fStation); };
		inline Int_t GetA() { return(fAddr); };
		inline Int_t GetF() { return(fFunction); };
		Int_t GetData(EMrbCnafType Type = kCnafTypeUndefined);
		Bool_t GetData(Int_t & DataRead, Int_t & DataWrite);
		inline Bool_t GetX() { return(IS_X(fStatus)); };
		inline Bool_t GetQ() { return(IS_Q(fStatus)); };
		inline Bool_t IsError() { return(IS_ERROR(fStatus)); };

		inline EMrbCnafType GetType() { return(fType); };

		inline EMrbCamacAction GetAction() { return(fAction); };	// return action
		inline void SetAction(EMrbCamacAction Action) { fAction = Action; };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEsoneCnaf.html&"); };

		void Reset();										// reset data
		Bool_t CheckCnaf(UInt_t CnafBits = kEsoneCnafBits);	// check for missing cnaf components
		Bool_t CheckCnaf(UInt_t CnafBit, Int_t Data);		// check for valid cnaf component

		inline Bool_t IsWrite() { return(fType == TMrbEsoneCnaf::kCnafTypeWrite); };
		inline Bool_t IsRead() { return(fType == TMrbEsoneCnaf::kCnafTypeRead); };
		inline Bool_t IsControl() { return(fType == TMrbEsoneCnaf::kCnafTypeControl); };
		inline Bool_t IsReadStatus() { return(fType == TMrbEsoneCnaf::kCnafTypeReadStatus); };

	protected:
		TMrbString fAscii; 				// ascii representation

		Int_t fCrate;					// Camac crate: Cxx
		Int_t fStation; 				// Camac station: N1 .. N31
		Int_t fAddr;					// Camac subaddress: A0 .. A15
		Int_t fFunction;				// Camac function: F0 .. F31
		Int_t fDataRead;				// Camac data (read)
		Int_t fDataWrite;				// Camac data (write)

		UInt_t fStatus;					// X, Q, error

		EMrbCamacAction fAction;		// Camac action

		EMrbCnafType fType; 			// Camac read / control / write

	ClassDef(TMrbEsoneCnaf, 1)		// [CAMAC Access] Camac command C.N.A.F
};

#endif
