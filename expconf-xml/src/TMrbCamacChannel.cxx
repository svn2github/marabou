//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCamacChannel.cxx
// Purpose:        MARaBOU configuration: single CAMAC channels
// Description:    Implements class methods to describe a single channel
//                 of a CAMAC module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCamacChannel.cxx,v 1.1 2008-03-05 12:37:16 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"

extern class TMrbConfig * gMrbConfig;

ClassImp(TMrbCamacChannel)

TMrbCamacChannel::TMrbCamacChannel(TMrbCamacModule * Module, Int_t Channel, TMrbCNAF & ModuleCNAF) :
																	TMrbModuleChannel(Module, Channel) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacChannel
// Purpose:        Create a single camac channel
// Arguments:      TMrbCamacModule * Module  -- addr of parent module
//                 Int_t Channel              -- channel number
//                 TMrbCNAF * ModuleCNAF     -- B.C.N of the parent module
// Results:        --
// Exceptions:
// Description:    Defines a single camac channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cnafAscii;

	if (!this->IsZombie()) {
		fCNAF = ModuleCNAF; 							// copy B.C.N of parent
		fCNAF.SetBit(TMrbCNAF::kCnafBranch | TMrbCNAF::kCnafCrate | TMrbCNAF::kCnafStation | TMrbCNAF::kCnafAddr,
																TMrbCNAF::kCnafStation | TMrbCNAF::kCnafAddr);
		fCNAF.Set(TMrbCNAF::kCnafAddr, Channel);		// add subaddr Axx

		SetName(fCNAF.Int2Ascii(cnafAscii));
	}
}

void TMrbCamacChannel::Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacChannel::Show
// Purpose:        Output current channel settings
// Arguments:      ofstream & OutStrm   -- output stream
//                 Bool_t ArrayFlag     -- kTRUE, if array-like output
//                 Bool_t SevtFlag      -- if kTRUE, use subevent name as prefix
//                 Char_t * Prefix      -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current channel definition to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cnafAscii;
	TString parent;
	TString cnaf;

	cnaf = fCNAF.Int2Ascii(cnafAscii);

	if (this->IsUsed()) {
		if (SevtFlag) parent = this->UsedBy()->GetName(); else parent = Parent()->GetName();

		OutStrm << Prefix << Form("%-23s%-18s", "", cnaf.Data());
		TMrbString par(this->GetName());
		if (this->GetStatus() == TMrbConfig::kChannelArray) {
			if (ArrayFlag) {
				par += "[";
				par += this->GetIndexRange();
				par += "]";
			} else {
				par += 0;
			}
		}
		OutStrm << Form("%-15s%s", par.Data(), parent.Data()) << endl;
	} else {
		OutStrm << Prefix << Form("%-23s%-18s", "", cnaf.Data()) << endl;
	}
}
