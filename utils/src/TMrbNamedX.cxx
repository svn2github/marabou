//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbNamedX.cxx
// Purpose:        MARaBOU utilities:
//             A TNamed having an index and an opt. object assigned
// Description:        Implements class methods to manage indexed TNamed objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbNamedX.h"

ClassImp(TMrbNamedX)

const Char_t * TMrbNamedX::GetFullName(TMrbString & FullName, Int_t Base, Bool_t IndexFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedX::GetFullName
// Purpose:        Return full index name
// Arguments:      TString & FullName  -- resulting string
//                 Int_t Base          -- numerical base
//                 Bool_t IndexFlag    -- append index?
// Results:        Char_t * FullName   -- index name
// Exceptions:
// Description:    Returns a string representing the full index name:
//                      Name[: Title][ (Index)]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FullName = this->GetName();
	if (this->HasTitle()) {
		FullName += ": ";
		FullName += this->GetTitle();
	}
	if (IndexFlag) {
		FullName += " (";
		switch (Base) {
			case 2: FullName += "0b"; break;
			case 8: FullName += "0"; break;
			case 16: FullName += "0x"; break;
		}
		FullName.AppendInteger(this->GetIndex(), 0, ' ', Base);
		FullName += ")";
	}
	return(FullName.Data());
}

void TMrbNamedX::Print(ostream & Out, Int_t Base, Bool_t CrFlag){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedX::Print
// Purpose:        Print key data
// Arguments:      ostream & Out   -- where to send it
//                 Int_t Base      -- numerical base
//                 Bool_t CrFlag   -- append <cr> (=endl) if kTRUE
// Results:        --
// Exceptions:
// Description:    Outputs index data to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString fullName;
	Out << this->GetFullName(fullName, Base, kTRUE);
	if (CrFlag) Out << endl;
}
