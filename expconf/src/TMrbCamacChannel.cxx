//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCamacChannel.cxx
// Purpose:        MARaBOU configuration: single CAMAC channels
// Description:    Implements class methods to describe a single channel
//                 of a CAMAC module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
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

void TMrbCamacChannel::Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix) const {
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
	const Char_t * parent;
	const Char_t * cnaf;

	cnaf = fCNAF.Int2Ascii(cnafAscii);

	if (this->IsUsed()) {
		if (SevtFlag) parent = this->UsedBy()->GetName(); else parent = Parent()->GetName();

		OutStrm << Prefix << setw(23) << " ";
		OutStrm << setiosflags(ios::left);
		OutStrm << setw(13) << cnaf;
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
		OutStrm << setw(10) << par;
		OutStrm << setw(10) << parent;
		OutStrm << resetiosflags(ios::left) << endl;
	} else {
		OutStrm << Prefix << setw(23) << " ";
		OutStrm << setiosflags(ios::left);
		OutStrm << setw(13) << cnaf;
		OutStrm << resetiosflags(ios::left);
		OutStrm << " n.a";
		OutStrm << endl;
	}
}
