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

long long ushort2ll48(UShort_t * Array) {
	long long ull;
	ull = (long long) *Array++;
	ull = (ull << 16) + (long long) *Array++;
	ull = (ull << 16) + (long long) *Array;
	return(ull);
}

long long ushort2ll48(UShort_t N1, UShort_t N2, UShort_t N3) {
	long long ull;
	ull = (long long) N1;
	ull = (ull << 16) + (long long) N2;
	ull = (ull << 16) + (long long) N3;
	return(ull);
}

long long ushort2ll64(UShort_t * Array) {
	long long ull;
	ull = (long long) *Array++;
	ull = (ull << 16) + (long long) *Array++;
	ull = (ull << 16) + (long long) *Array++;
	ull = (ull << 16) + (long long) *Array;
	return(ull);
}

long long ushort2ll64(UShort_t N1, UShort_t N2, UShort_t N3, UShort_t N4) {
	long long ull;
	ull = (long long) N1;
	ull = (ull << 16) + (long long) N2;
	ull = (ull << 16) + (long long) N3;
	ull = (ull << 16) + (long long) N4;
	return(ull);
}

long long uint2ll48(UInt_t * Array) {
	long long ull;
	ull = (long long) *Array++;
	ull = (ull << 16) + (long long) *Array++;
	ull = (ull << 16) + (long long) *Array;
	return(ull);
}

UShort_t * ll2ushort48(UShort_t * Array, long long N) {
	UShort_t * ap = Array + 2;
	*ap-- = N & 0xFFFF; N >>= 16;
	*ap-- = N & 0xFFFF; N >>= 16;
	*ap-- = N & 0xFFFF;
}


UShort_t * ll2ushort64(UShort_t * Array, long long N) {
	UShort_t * ap = Array + 3;
	*ap-- = N & 0xFFFF; N >>= 16;
	*ap-- = N & 0xFFFF; N >>= 16;
	*ap-- = N & 0xFFFF; N >>= 16;
	*ap = N & 0xFFFF;
}
