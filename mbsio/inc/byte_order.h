/*INCLUDEFILE*****************************************************************
NAME
	byte_order.h
AUTOR
	R. Lutter
FUNKTION
	Definition der moeglichen Byte-Anordnungen
DATUM
	3/11/97
VERSION
	1.1
TYPEDEFS

STICHWORTE

SONSTIGES

*****************************************************************************/

/*BITS & BYTES****************************************************************
NAME
	BYTE_ORDER_xxxx
AUTOR
	R. Lutter
FUNKTION
	Byte-Anordnungen
DEFINITION
.	BYTE_ORDER_1_TO_1	-- 1:1, nichts zu tun
.	BYTE_ORDER_BSW		-- ByteSwap
.	BYTE_ORDER_LSW		-- LongWordSwap
.	BYTE_ORDER_REV		-- ReverseOrder
BESCHREIBUNG

STICHWORTE

SONSTIGES

*****************************************************************************/

#define BYTE_ORDER_1_TO_1		0
#define BYTE_ORDER_BSW			1
#define BYTE_ORDER_LSW			2
#define BYTE_ORDER_REV			3
