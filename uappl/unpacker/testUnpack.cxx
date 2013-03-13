//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           testUnpack.cxx
// Purpose:        Unpack root data
// Description:    Unpacker for MiniballMLL data
//
// Header files:   MiniballMLLAnalyze.h        -- class definitions
//                 MiniballMLLCommonIndices.h  -- common indices
//                 MiniballMLLAnalyzeGlobals.h -- global objects
//
// Author:         R. Lutter
// Revision:
// Date:           Fri Mar  1 15:07:08 2013
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <iostream>

// class definitions
#include "MiniballMLLAnalyze.h"
// common index declarations for analysis and readout
#include "MiniballMLLCommonIndices.h"
// global objects
#include "MiniballMLLAnalyzeGlobals.h"

#include "SetColor.h"

Bool_t UnpackInitialize(const Char_t * RootFile);		// prototype

int main(int argc, char **argv) {

	TString rootFile = argv[1];
	
	TArrayI adcData(32);							// vector to hold 1 adc event
	ULong64_t ts;									// timestamp (48bit) of adc event
	
	if (UnpackInitialize(rootFile.Data())) {		// open root file and init tree
		Int_t entryNo = 0;
		while ((entryNo = gReadout->UnpackNextEvent(entryNo)) != -1) {	// read mbs event by mbs event from tree
			Int_t hidx = 0;
			TUsrHBX * hbx = gReadout->GetHBX(kMrbSevtAdcSevt1);			// get data from subevent/adc1 (index defined in MiniballMLLCommonIndices.h)
			while (hidx != -1) {							// loop over adc events in mbs buffer
				hidx = gReadout->FillArrayFromHB(hbx, ts, adcData, hidx, TUsrHit::kHitEnergy, -1);	// fill adc vector and extract timestamp (reset with data=-1)				
				cout << "ts=" << ts << " data= ";			// example output
				for (Int_t i = 0; i < 32; i++) cout << adcData[i] << " ";
				cout << endl;
			}
		}
	}
}
