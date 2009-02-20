//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrNamedX.cxx
//! \brief			Light Weight ROOT: TMrbNamedX
//! \details		Class definitions for ROOT under LynxOs: TMrbNamedX
//!                 A TNamed having an index and an opt. object assigned
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-20 08:40:11 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrNamedX.h"

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns a string representing the full index name:<br>
//! 						Name[: Title][ (Index)]
//! \param[out]		FullName			-- resulting string
//! \param[in]		Base				-- numerical base
//! \param[in]		IndexFlag			-- TRUE if to append index
//! \retval 		FullName			-- resulting string
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMrbNamedX::GetFullName(TString & FullName, Int_t Base, Bool_t IndexFlag) const
{
	FullName = this->GetName();
	if (this->HasTitle()) {
		FullName += ": ";
		FullName += this->GetTitle();
	}
	if (IndexFlag) {
		switch (Base) {
			case 2:
				{
					UInt_t patt = 0;
					Int_t bit = 1;
					UInt_t idx = this->GetIndex();
					while (idx) {
						if (idx & 1) patt += bit;
						bit *= 10;
						idx >>= 1;
					}
					FullName += Form("(0b%d)", patt);
				}
				break;
			case 8: FullName += Form("(%#o)", this->GetIndex()); break;
			case 10: FullName += Form("(%d)", this->GetIndex()); break;
			case 16: FullName += Form("(%#x)", this->GetIndex()); break;
		}
	}
	return(FullName.Data());
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Compares two TMrbNamedX objects.
//! 				Returns
//! 				<ul>
//! 				<li>	0 when equal
//! 				<li>	-1 if \a this is smaller
//! 				<li>	+1 if \a this is bigger than \a Nx
//! 				</ul>
//! 				(like strcmp)
//! \param[out]		Nx				-- TMrbNamedX object to be compared
//! \retval 		Relation		-- {0, -1, +1}
//////////////////////////////////////////////////////////////////////////////

Int_t TMrbNamedX::Compare(const TMrbNamedX * Nx) const
{
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs TMrbNamedX data
//! \param[in]		Out 		-- stream where to output
//! \param[in]		Base		-- numerical base
//! \param[in]		CrFlag		-- appends \a cr (=endl) if TRUE
//////////////////////////////////////////////////////////////////////////////

void TMrbNamedX::Print(ostream & Out, Int_t Base, Bool_t CrFlag) const
{
	TString fullName;
	Out << this->GetFullName(fullName, Base, kTRUE);
	if (CrFlag) Out << endl;
}
