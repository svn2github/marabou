//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVMEScaler.cxx
// Purpose:        MARaBOU configuration: scaler definitions
// Description:    Implements class methods to handle scalers
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

#include "TMrbVMEScaler.h"

ClassImp(TMrbVMEScaler)

void TMrbVMEScaler::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEScaler::Print
// Purpose:        Output current scaler definitions
// Arguments:      ofstream & OutStrm   -- output stream
//                 Char_t * Prefix      -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current scaler definition to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	OutStrm << endl;
	OutStrm << Prefix << "Scaler Definition:" << endl;
	OutStrm << Prefix << "   Name          : " << this->GetName() << endl;
	OutStrm << Prefix << "   Module        : " << this->GetTitle() << endl;
	OutStrm << Prefix << "   Type          : VME" << endl;
	OutStrm << Prefix << "   Location      : " << this->GetPosition() << endl;
}
