//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           n2ll.cxx
// Purpose:        Implement support for 'long long' integers
// Description:    Provides functions to convert 48 or 64 bit numbers
//                 to 'long long'
//
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"

unsigned long long ushort2ll48(UShort_t * Array) {
	unsigned long long ull;
	ull = (unsigned long long) *Array++;
	ull = (ull << 16) + (unsigned long long) *Array++;
	ull = (ull << 16) + (unsigned long long) *Array;
	return(ull);
}

unsigned long long ushort2ll48(UShort_t N1, UShort_t N2, UShort_t N3) {
	unsigned long long ull;
	ull = (unsigned long long) N1;
	ull = (ull << 16) + (unsigned long long) N2;
	ull = (ull << 16) + (unsigned long long) N3;
	return(ull);
}

unsigned long long uint2ll48(UInt_t * Array) {
	unsigned long long ull;
	ull = (unsigned long long) (*Array++ & 0xFFFF);
	ull = (ull << 16) + (unsigned long long) (*Array++ & 0xFFFF);
	ull = (ull << 16) + (unsigned long long) (*Array & 0xFFFF);
	return(ull);
}

unsigned long long uint2ll48(UInt_t N1, UInt_t N2, UInt_t N3) {
	unsigned long long ull;
	ull = (unsigned long long) (N1 & 0xFFFF);
	ull = (ull << 16) + (unsigned long long) (N2 & 0xFFFF);
	ull = (ull << 16) + (unsigned long long) (N3 & 0xFFFF);
	return(ull);
}

unsigned long long ushort2ll64(UShort_t * Array) {
	unsigned long long ull;
	ull = (unsigned long long) *Array++;
	ull = (ull << 16) + (unsigned long long) *Array++;
	ull = (ull << 16) + (unsigned long long) *Array++;
	ull = (ull << 16) + (unsigned long long) *Array;
	return(ull);
}

unsigned long long ushort2ll64(UShort_t N1, UShort_t N2, UShort_t N3, UShort_t N4) {
	unsigned long long ull;
	ull = (unsigned long long) N1;
	ull = (ull << 16) + (unsigned long long) N2;
	ull = (ull << 16) + (unsigned long long) N3;
	ull = (ull << 16) + (unsigned long long) N4;
	return(ull);
}

unsigned long long us2ll64(UInt_t * Array) {
	long long ull;
	ull = (unsigned long long) *Array++;
	ull = (ull << 32) + (unsigned long long) *Array;
	return(ull);
}

unsigned long long us2ll64(UInt_t N1, UInt_t N2) {
	unsigned long long ull;
	ull = (unsigned long long) N1;
	ull = (ull << 32) + (unsigned long long) N2;
	return(ull);
}

