#ifndef __TMrbMesytec_Mux16_h__
#define __TMrbMesytec_Mux16_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Mux16.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Mux16        -- VME digitizer adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Mux16.h,v 1.2 2007-10-22 12:20:58 Marabou Exp $       
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

#include "TMrbModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mux16
// Purpose:        Define a (pseudo) module type Mesytec MUX
// Description:    Defines a (pseudo) module type Mesytec MUX
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMesytec_Mux16 : public TMrbModule {

	public:
		TMrbMesytec_Mux16() {};  													// default ctor
		TMrbMesytec_Mux16(const Char_t * MuxName, const Char_t * ModuleName, Int_t FirstChannel, Int_t NofSubmodules = 1); 	// define a new module
		~TMrbMesytec_Mux16() {};													// default dtor

		const Char_t * GetParamNames(TString & ParamNames);

		Bool_t SetHistoName(Int_t Channel, const Char_t * HistoName, const Char_t * HistoTitle = "");
		const Char_t * GetHistoName(Int_t Channel);
		const Char_t * GetHistoTitle(Int_t Channel);
		inline TObjArray * GetHistoNames() { return(&fHistoNames); };

		Bool_t BookHistograms();

		Bool_t WriteLookup(const Char_t * LkpFile);

		inline Int_t GetFirstChannel() { return(fFirstChannel); };
		inline Int_t GetNofSubmodules() { return(fNofSubmodules); };
		inline TMrbModule * GetModule() { return(fModule); };
		inline Int_t GetModuleSerial() { return(fModule ? fModule->GetSerial() : 0); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:

		TMrbModule * fModule;			// module mux is assigned to
		Int_t fFirstChannel;			// where mux channels start
		Int_t fNofSubmodules;			// number of muxes stacked together
		TObjArray fHistoNames;			// histo names

	ClassDef(TMrbMesytec_Mux16, 1)		// [Config] Mesytec multiplexer
};

#endif
