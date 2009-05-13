/******************************************************************************/
/*!	\file		typedefs.h
	\brief		GSI data types
	\details	central include file to define data types for all platforms
	\author 	GSI
	\version	$Revision: 1.1 $       
	\date		$Date: 2009-05-13 06:59:40 $
*******************************************************************************/


#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Platform independent definitions */

typedef          char   CHARS;		/*!< char/byte */
typedef unsigned char   CHARU;		/*!< char/byte unsigned */
typedef          char   INTS1;		/*!< char/byte */
typedef unsigned char   INTU1;		/*!< char/byte unsigned */
typedef          short  INTS2;		/*!< short 2 bytes */
typedef unsigned short  INTU2;		/*!< short unsigned */
typedef          int    INTS4;		/*!< int 4 bytes */
typedef unsigned int    INTU4;		/*!< int unsigned */
typedef          float  REAL4;		/*!< float */
typedef          double REAL8;		/*!< double */

/* Platform specific definitions */

typedef          long INTS8;		/*!< lynxOs: long 8 bytes */
typedef unsigned long INTU8;		/*!< lynxOs: long unsigned */
typedef unsigned long ADDRS;		/*!< lynxOs: address 8 bytes */

#endif
