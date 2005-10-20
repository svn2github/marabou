//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbNamedX.cxx
// Purpose:        MARaBOU utilities:
//                 A TNamed having an index and an opt. object assigned
// Description:    Implements class methods to manage indexed TNamed objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbNamedX.cxx,v 1.6 2005-10-20 14:16:12 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbNamedX.h"

ClassImp(TMrbNamedX)

const Char_t * TMrbNamedX::GetFullName(TMrbString & FullName, Int_t Base, Bool_t IndexFlag) const {
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

Int_t TMrbNamedX::Compare(const TObject * Nx) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedX::Compare
// Purpose:        Compare objects
// Arguments:      TObject * Nx   -- TMrbNamedX object to be compared
// Results:        Int_t Restult  -- <0, 0, >0
// Exceptions:
// Description:    Compares TMrbNamedX objects.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t s;
	TMrbNamedX * nx = (TMrbNamedX *) Nx;
	if (this->IsSortedByName()) {
		TString nxm = this->GetName();
		s = nxm.CompareTo(nx->GetName());
	} else {
		s = this->GetIndex() - nx->GetIndex();
	}
	return(s);
}

void TMrbNamedX::Print(ostream & Out, Int_t Base, Bool_t CrFlag) const {
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
