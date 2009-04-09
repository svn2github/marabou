#ifndef __LwrArray_h__
#define __LwrArray_h__


//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrArray.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TArray<br>
//! 				Abstract array base class.<br>
//! 				Used by TArrayC, TArrayS, TArrayI, TArrayL, TArrayF and TArrayD.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-04-09 13:34:50 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include <string.h>

class TArray {

protected:
	Bool_t        OutOfBoundsError(const char *where, Int_t i) const;
	inline Bool_t BoundsOk(const char *where, Int_t at) const
	{
		return (at < 0 || at >= fN) ? OutOfBoundsError(where, at) : kTRUE;
	};


public:
	Int_t     fN;            //!< number of array elements

	//! Default constructor
	TArray(): fN(0) { }

	//! Constructor
	//! \param[in]	n	-- number of array elements
	TArray(Int_t n): fN(n) { }

	//! Copy constructor
	//! \param[in]	a	-- array to get copied
	TArray(const TArray &a): fN(a.fN) { }

	TArray         &operator=(const TArray &rhs) {if(this!=&rhs) fN = rhs.fN; return *this; }
	virtual        ~TArray() { fN = 0; }

	Int_t          GetSize() const { return fN; }
	virtual void   Set(Int_t n) = 0;

	virtual Double_t GetAt(Int_t i) const = 0;
	virtual void   SetAt(Double_t v, Int_t i) = 0;

};

#endif
