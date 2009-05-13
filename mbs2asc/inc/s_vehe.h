#include "typedefs.h"

/******************************************************************************/
/*!	\file		s_vehe.h
	\brief		GSI VME event header
	\author 	GSI
	\version	$Revision: 1.1 $       
	\date		$Date: 2009-05-13 06:59:40 $
*******************************************************************************/

struct _s_vehe				/*! GSI VME event header */
{
	INTS4  l_dlen;			/*!< data length + 4 in words */
	INTS2 i_subtype;		/*!< subtype */
	INTS2 i_type;			/*!< type */ 
	INTS2 i_trigger;		/*!< trigger number */
	INTS2 i_dummy;			/*!< not used */
	INTS4  l_count; 		/*!< event number */
};

typedef struct _s_vehe s_vehe;	/*!< GSI VME event header */
