//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrUtils.cxx
//! \brief			Light Weight ROOT: Utility functions
//! \details		Class definitions for ROOT under LynxOs: Utility functions
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-20 08:40:11 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrUtils.h"


//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Swap bytes
//! \param[in]		IntVal	-- integer value, original
//! \retval 		IntVal	-- integer value, swapped
//////////////////////////////////////////////////////////////////////////////

UInt_t swapIt(UInt_t IntVal)
{
   return (((IntVal & 0x000000ffU) << 24) | ((IntVal & 0x0000ff00U) <<  8) |
           ((IntVal & 0x00ff0000U) >>  8) | ((IntVal & 0xff000000U) >> 24));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Swap bytes
//! \param[in]		LongIntVal	-- (long) integer value, original
//! \retval 		LongIntVal	-- (long) integer value, swapped
//////////////////////////////////////////////////////////////////////////////

ULong_t swapIt(ULong_t LongIntVal)
{
   char sw[sizeof(ULong_t)];
   *(ULong_t *)sw = LongIntVal;

   char *sb = (char *)&LongIntVal;
   sb[0] = sw[7];
   sb[1] = sw[6];
   sb[2] = sw[5];
   sb[3] = sw[4];
   sb[4] = sw[3];
   sb[5] = sw[2];
   sb[6] = sw[1];
   sb[7] = sw[0];
   return LongIntVal;
}
