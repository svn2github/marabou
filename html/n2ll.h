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

extern unsigned long long ushort2ll48(UShort_t * Array);
extern unsigned long long ushort2ll48(UShort_t N1, UShort_t N2, UShort_t N3); 
extern unsigned long long uint2ll48(UInt_t * Array);
extern unsigned long long uint2ll48(UInt_t N1, UInt_t N2, UInt_t N3); 
extern unsigned long long ushort2ll64(UShort_t * Array);
extern unsigned long long ushort2ll64(UShort_t N1, UShort_t N2, UShort_t N3, UShort_t N4); 
extern unsigned long long uint2ll64(UInt_t * Array);
extern unsigned long long uint2ll64(UInt_t N1, UInt_t N2); 
#endif
