#ifndef __TMrbCologne_CPTM_h__
#define __TMrbCologne_CPTM_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCologne_CPTM.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCologne_CPTM     -- clock and programmable trigger module
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCologne_CPTM.h,v 1.2 2005-09-09 06:59:13 Rudolf.Lutter Exp $       
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM
// Purpose:        Define a module of type C_PTM
// Description:    Defines an module of type C_PTM
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCologne_CPTM : public TMrbCamacModule {

	public:

		TMrbCologne_CPTM() {};														// default ctor
		TMrbCologne_CPTM(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new dgf module
		virtual ~TMrbCologne_CPTM() {};												// remove module from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code

		Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName);
		
		virtual inline const Char_t * GetMnemonic() const { return("cptm"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TObject * Subevent) const;		// check if subevent type is [10,2x]

		virtual inline Bool_t HasRandomReadout() const { return(kFALSE); };	// block mode only
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters();

	protected:

	ClassDef(TMrbCologne_CPTM, 1)	// [Config] C_PTM: clock and programmable trigger module
};

#endif
