#ifndef __LwrArrayI_h__
#define __LwrArrayI_h__

#include "LwrArray.h"

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrArrayI.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TArrayI<br>
//! 				An array of integers (32 bits per element).
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

class TArrayI : public TArray {

public:
	Int_t    *fArray;       //!< array of fN 32 bit integers

	//! Default constructor
	TArrayI();

	//! Constructor
	//! \param[in]	n	-- number of array elements
	TArrayI(Int_t n);

	//! Constructor
	//! \param[in]	n		-- number of array elements
	//! \param[in]	array	-- array to copy values from
	TArrayI(Int_t n, const Int_t *array);

	//! Copy constructor
	//! \param[in]	array	-- array to create a copy from
	TArrayI(const TArrayI &array);

	TArrayI    &operator=(const TArrayI &rhs);
	virtual    ~TArrayI();

	void         Adopt(Int_t n, Int_t *array);
	void         AddAt(Int_t i, Int_t idx);
	Int_t        At(Int_t i) const ;
	void         Copy(TArrayI &array) const {array.Set(fN,fArray);}
	const Int_t *GetArray() const { return fArray; }
	Int_t       *GetArray() { return fArray; }
	Double_t     GetAt(Int_t i) const { return At(i); }
	Stat_t       GetSum() const {Stat_t sum=0; for (Int_t i=0;i<fN;i++) sum+=fArray[i]; return sum;}
	void         Reset()           {memset(fArray, 0, fN*sizeof(Int_t));}
	void         Reset(Int_t val)  {for (Int_t i=0;i<fN;i++) fArray[i] = val;}
	void         Set(Int_t n);
	void         Set(Int_t n, const Int_t *array);
	void         SetAt(Double_t v, Int_t i) { AddAt((Int_t)v, i); }
	Int_t       &operator[](Int_t i);
	Int_t        operator[](Int_t i) const;
};

//! \details	Checks if element index is within boundaries<br>
//! 			returns integer value if ok
//! \param[in]	Idx 	-- element index
//! \retval 	Value	-- integer value
inline Int_t TArrayI::At(Int_t Idx) const
{
	if (!BoundsOk("TArrayI::At", Idx)) return 0;
	return fArray[Idx];
}

inline Int_t &TArrayI::operator[](Int_t i)
{
   if (!BoundsOk("TArrayI::operator[]", i)) i = 0;
   return fArray[i];
}

inline Int_t TArrayI::operator[](Int_t i) const
{
   if (!BoundsOk("TArrayI::operator[]", i)) return 0;
   return fArray[i];
}

#endif
