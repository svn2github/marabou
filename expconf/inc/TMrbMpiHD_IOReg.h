#ifndef __TMrbMpiHD_IOReg_h__
#define __TMrbMpiHD_IOReg_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMpiHD_IOReg.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMpiHD_IOReg        -- 16 bit i/o reg (MPI HD)
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMpiHD_IOReg
// Purpose:        Define a VME 16 bit i/O register
// Description:    Defines a VME input/output register
//                 (8 bits in / 8 bits out, MPI HD)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMpiHD_IOReg : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x100	};
		enum				{	kAddrMod		=	0x29	};

	public:

		TMrbMpiHD_IOReg() {};  											// default ctor
		TMrbMpiHD_IOReg(const Char_t * ModuleName, UInt_t BaseAddr); 		// define a new instance
		~TMrbMpiHD_IOReg() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("mpiorl"); }; 	// module mnemonic

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbMpiHD_IOReg.html&"); };

	ClassDef(TMrbMpiHD_IOReg, 1)		// [Config] 16 bit I/O register (8in, 8out, MPI HD)
};

#endif
