#ifndef __n2ll_h__
#define __n2ll_h__
//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           n2ll.h
// Purpose:        'Long long' support
// Description:    Provides support for long long integers.
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

extern long long ushort2ll48(UShort_t * Array);
extern long long ushort2ll48(UShort_t N1, UShort_t N2, UShort_t N3); 
extern long long ushort2ll64(UShort_t * Array);
extern long long ushort2ll64(UShort_t N1, UShort_t N2, UShort_t N3, UShort_t N4); 
extern void ll2ushort48(UShort_t * Array, long long N);
extern void ll2ushort64(UShort_t * Array, long long N);
#endif
