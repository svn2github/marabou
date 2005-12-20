/*FILE************************************************************************
NAME
	%M%
AUTOR
	R. Lutter
FUNKTION
	Zugriff zu Daten in unterschiedlicher Byte-Anordnung
DATUM
	%G%
VERSION
	%I%
BESCHREIBUNG
	Liest Daten und ordnet die Bytes
MODULE

INCLUDE

STICHWORTE

SONSTIGES

*****************************************************************************/

static char sccs_id[] = "%W%";

#include <string.h>
#include "byte_order.h"

/*BIBLIOTHEKSFUNKTION*********************************************************
NAME
	bto_get_short
	bto_get_int32
	bto_get_string
	bto_put_int32
AUTOR
	R. Lutter
FUNKTION
	Konversion der Daten
AUFRUF
.	char *bto_get_short(out, in, cnt, bytord)
	short *out;
	char *in;
	int cnt;
	int bytord;

.	char *bto_get_int32(out, in, cnt, bytord)
	long *out;
	char *in;
	int cnt;
	int bytord;

.	char *bto_get_string(out, in, cnt, bytord)
	char *out;
	char *in;
	int cnt;
	int bytord;

.	char *bto_put_int32(out, in, cnt, bytord)
	long *out;
	char *in;
	int cnt;
	int bytord;
ARGUMENTE
.	out			-- Destination
.	in			-- Source-Adresse
.	cnt			-- Anzahl der Worte bzw. Bytes
.	bytord		-- Anordnung der Bytes (BYTE_ORDER_xxx)
RETURNWERT
.	addr		-- neue Source-Adresse
FEHLER

BESCHREIBUNG
	Konvertiert die Originaldaten.
VERFAHREN

FILES
	
ROUTINEN

STICHWORTE

SONSTIGES

*****************************************************************************/

char *bto_get_short(short *out, char *in, int cnt, int bytord) {

	register int i;
	union x {
		short s;
		char b[2];
	} x;

	switch (bytord)
	{
		case BYTE_ORDER_LSW:
		case BYTE_ORDER_1_TO_1:	for (i = 0; i < cnt; i++)
								{
									x.b[0] = *in++;
									x.b[1] = *in++;
									*out++ = x.s;
								}
								break;

		case BYTE_ORDER_REV:
		case BYTE_ORDER_BSW:	for (i = 0; i < cnt; i++)
								{
									x.b[1] = *in++;
									x.b[0] = *in++;
									*out++ = x.s;
								}
								break;
	}
	return(in);
}

char *bto_get_int32(int *out, char *in, int cnt, int bytord) {

	register int i;
	union x {
		int l;
		char b[4];
	} x;

	switch (bytord)
	{
		case BYTE_ORDER_1_TO_1:	for (i = 0; i < cnt; i++)
								{
									x.b[0] = *in++;
									x.b[1] = *in++;
									x.b[2] = *in++;
									x.b[3] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_BSW:	for (i = 0; i < cnt; i++)
								{
									x.b[1] = *in++;
									x.b[0] = *in++;
									x.b[3] = *in++;
									x.b[2] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_LSW:	for (i = 0; i < cnt; i++)
								{
									x.b[2] = *in++;
									x.b[3] = *in++;
									x.b[0] = *in++;
									x.b[1] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_REV:	for (i = 0; i < cnt; i++)
								{
									x.b[3] = *in++;
									x.b[2] = *in++;
									x.b[1] = *in++;
									x.b[0] = *in++;
									*out++ = x.l;
								}
								break;
	}
	return(in);
}

char *bto_get_string(char *out, char *in, int cnt, int bytord) {

	register int i;
	register char b;

	switch (bytord)
	{
		case BYTE_ORDER_REV:
		case BYTE_ORDER_1_TO_1:	if (cnt <= 0)
								{
									strcpy(out, in);
									return(in + strlen(in));
								} else {
									memcpy(out, in, cnt);
									return(in + cnt);
								}

		case BYTE_ORDER_LSW:
		case BYTE_ORDER_BSW:	if (cnt > 0)
								{
									for (i = 0; i < cnt; i += 2)
									{
										*out++ = *(in + 1);
										*out++ = *in;
										in += 2;
									}
									return(in);
								} else {
									for (;;)
									{
										b = *(in + 1);
										*out++ = b;
										if (b == '\0') return(in + 1);
										b = *in;
										*out++ = b;
										in += 2;
										if (b == '\0') return(in);
									}
								}
	}
	return(in); 		/* will not be reached */
}

char *bto_put_int32(int *out, char *in, int cnt, int bytord) {

	register int i;
	union x {
		int l;
		char b[4];
	} x;

	switch (bytord)
	{
		case BYTE_ORDER_1_TO_1:	for (i = 0; i < cnt; i++)
								{
									x.b[0] = *in++;
									x.b[1] = *in++;
									x.b[2] = *in++;
									x.b[3] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_BSW:	for (i = 0; i < cnt; i++)
								{
									x.b[1] = *in++;
									x.b[0] = *in++;
									x.b[3] = *in++;
									x.b[2] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_LSW:	for (i = 0; i < cnt; i++)
								{
									x.b[2] = *in++;
									x.b[3] = *in++;
									x.b[0] = *in++;
									x.b[1] = *in++;
									*out++ = x.l;
								}
								break;

		case BYTE_ORDER_REV:	for (i = 0; i < cnt; i++)
								{
									x.b[3] = *in++;
									x.b[2] = *in++;
									x.b[1] = *in++;
									x.b[0] = *in++;
									*out++ = x.l;
								}
								break;
	}
	return(in);
}

