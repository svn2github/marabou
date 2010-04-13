#ifndef __LwrTypes_h__
#define __LwrTypes_h__

/*__________________________________________________________[TYPE DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrTypes.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: Type definitions
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $
//! $Date: 2010-04-13 08:28:52 $
////////////////////////////////////////////////////////////////////////////*/

typedef char           Char_t;      //Signed Character 1 byte (char)
typedef unsigned char  UChar_t;     //Unsigned Character 1 byte (unsigned char)
typedef short          Short_t;     //Signed Short integer 2 bytes (short)
typedef unsigned short UShort_t;    //Unsigned Short integer 2 bytes (unsigned short)
typedef int            Int_t;       //Signed integer 4 bytes (int)
typedef unsigned int   UInt_t;      //Unsigned integer 4 bytes (unsigned int)
typedef int            Seek_t;      //File pointer (int)
typedef long           Long_t;      //Signed long integer 8 bytes (long)
typedef unsigned long  ULong_t;     //Unsigned long integer 8 bytes (unsigned long)
typedef float          Float_t;     //Float 4 bytes (float)
typedef float          Float16_t;   //Float 4 bytes written with a truncated mantissa
typedef double         Double_t;    //Double 8 bytes
typedef double         Double32_t;  //Double 8 bytes in memory, written as a 4 bytes float
typedef char           Text_t;      //General string (char)
typedef bool           Bool_t;      //Boolean (0=false, 1=true) (bool)
typedef unsigned char  Byte_t;      //Byte (8 bits) (unsigned char)
typedef short          Version_t;   //Class version identifier (short)
typedef const char     Option_t;    //Option string (const char)
typedef int            Ssiz_t;      //String size (int)
typedef float          Real_t;      //TVector and TMatrix element type (float)
typedef double         Axis_t;      //Axis values type (double)
typedef double         Stat_t;      //Statistics type (double)

typedef short          Font_t;      //Font number (short)
typedef short          Style_t;     //Style number (short)
typedef short          Marker_t;    //Marker number (short)
typedef short          Width_t;     //Line width (short)
typedef short          Color_t;     //Color number (short)
typedef short          SCoord_t;    //Screen coordinates (short)
typedef double         Coord_t;     //Pad world coordinates (double)
typedef float          Angle_t;     //Graphics angle (float)
typedef float          Size_t;      //Attribute size (float)

typedef void         (*VoidFuncPtr_t)();  //pointer to void function


/* ---- constants --------------------------------------------------------------- */

#ifndef NULL
#define NULL 0
#endif

const Bool_t kTRUE   = true;
const Bool_t kFALSE  = false;

const Ssiz_t    kBitsPerByte = 8;
const Ssiz_t    kNPOS        = ~(Ssiz_t)0;


/* --- bit manipulation --------------------------------------------------------- */

#define BIT(n)       (1 << (n))
#define SETBIT(n,i)  ((n) |= BIT(i))
#define CLRBIT(n,i)  ((n) &= ~BIT(i))
#define TESTBIT(n,i) ((Bool_t)(((n) & BIT(i)) != 0))

#endif
