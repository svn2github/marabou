//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVMEChannel.cxx
// Purpose:        MARaBOU configuration: single VME channels
// Description:    Implements class methods to describe a single channel
//                 of a VME module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbString.h"
#include "TMrbConfig.h"
#include "TMrbVMEModule.h"
#include "TMrbVMEChannel.h"

extern class TMrbConfig * gMrbConfig;

ClassImp(TMrbVMEChannel)

TMrbVMEChannel::TMrbVMEChannel(TMrbVMEModule * Module, Int_t Channel) : TMrbModuleChannel(Module, Channel) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacChannel
// Purpose:        Create a single VME channel
// Arguments:      TMrbVMEModule * Module     -- addr of parent module
//                 Int_t Channel              -- channel number
// Results:        --
// Exceptions:
// Description:    Defines a single VME channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fOffset = 0;
		fPosition = "C0.";
		fPosition.AppendInteger(Module->GetBaseAddr(), 0, 0, 16);
	}
}

void TMrbVMEChannel::Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix) {
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

	const Char_t * parent;

	if (IsUsed()) {
		if (SevtFlag) parent = UsedBy()->GetName(); else parent = Parent()->GetName();

		OutStrm << Prefix << setw(23) << " ";
		OutStrm << setiosflags(ios::left);
		OutStrm << setw(13) << "+" << setiosflags(ios::showbase) << setbase(16)
				<< fOffset
				<< setbase(10) << resetiosflags(ios::showbase) << endl;
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
		OutStrm << setw(13) << "+" << setiosflags(ios::showbase) << setbase(16)
				<< fOffset
				<< setbase(10) << resetiosflags(ios::showbase) << endl;
		OutStrm << resetiosflags(ios::left) << " n.a";
		OutStrm << endl;
	}
}
