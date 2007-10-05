/*_________________________________________________________________[C UTILITY]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       unix_string.c
// @(#)Purpose:    Read a file containing unix-like strings
// Description:    Provides a means to read :-separated strings a la unix
// @(#)Author:     R. Lutter
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// URL:            
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#ifdef MBS_SIMUL_RDO
namespace std {} using namespace std;
#	include <iostream>
#	include <iomanip>
#else
#	include <stdio.h>
#endif

#include "unix_string.h"

FILE * _unix_open(const char * FileName);
TUnixEntry * _unix_find_entry(const char * EntryName, int ValueNumber);
int _unix_find_value(TUnixEntry * Entry, int ValueNumber);
void _unix_decode_entry(TUnixEntry * Entry, const char * UnixString);

#ifdef MBS_SIMUL_RDO
	void * calloc(size_t, size_t);
	char * strchr(const char *, int);
	char * strrchr(const char *, int);
#else
	char * calloc(int, int);
	char * strchr(char *, char);
	char * strrchr(char *, char);
#endif


/*___________________________________________________________________[GLOBALS]
////////////////////////////////////////////////////////////////////////////*/
static TUnixEntry * lofEntries = NULL;
static int nofEntries = 0;

int unix_string_read(const char * FileName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_read
// Purpose:        Read file
// Arguments:      char * FileName     -- file name
// Results:        int nofEntries      -- number of entries
// Exceptions:     returns -1 after errors
// Description:    Opens environment file and decodes entries.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i, n;
	char * sp;
	FILE * fp;
	TUnixEntry * entry;
	char line[UXS_L_STR];
			
	fp = _unix_open(FileName);	
	if (fp == NULL) return(-1);
	
	entry = lofEntries;
	n = 0;
	for (i = 0; i < nofEntries; i++) {
		memset(entry, 0, sizeof(TUnixEntry));
		if (fgets(line, UXS_L_STR - 1, fp) == NULL) break;
		if (line[0] == '#') continue;
		line[strlen(line) - 1] = '\0';
		sp = strchr(line, ':');
		if (sp == NULL) continue;
		_unix_decode_entry(entry, line);		
		entry++;
		n++;
	}
	fclose(fp);
	nofEntries = n;
	return(nofEntries);
}

TUnixEntry * unix_string_get_entry(int EntryNumber) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_get_entry
// Purpose:        Return entry address
// Arguments:      int EntryNumber         -- entry number
// Results:        TUnixEntry * Entry      -- selected entry
// Exceptions:     
// Description:   Returns entry address.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	if (EntryNumber >= nofEntries)	return(NULL);
	else							return(lofEntries + EntryNumber);
}

int unix_string_getval_i(TUnixEntry * Entry, const char * EntryName, int ValueNumber, int DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_getval_i
// Purpose:        Get a resource value
// Arguments:      TUnixEntry * Entry      -- selected entry
//                 char * EntryName        -- resource name
//                 int ValueNumber         -- value number
//                 int DefaultValue        -- default
// Results:        int EntryValue          -- resource value
// Exceptions:     
// Description:    Returns an integer resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TUnixEntry * ep;
	ep = Entry;
	if (ep == NULL) {
		if ((ep = _unix_find_entry(EntryName, ValueNumber)) == NULL)	return(DefaultValue);
		else															return((int) strtol(ep->curval, NULL, 0));
	} else {
		if (_unix_find_value(ep, ValueNumber) == 0)						return(DefaultValue);	
		else															return((int) strtol(ep->curval, NULL, 0));
	}
}

double unix_string_getval_f(TUnixEntry * Entry, const char * EntryName, int ValueNumber, double DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_getval_f
// Purpose:        Get a resource value
// Arguments:      TUnixEntry * Entry      -- selected entry
//                 char * EntryName     -- resource name
//                 double DefaultValue     -- default
// Results:        double EntryValue    -- resource value
// Exceptions:     
// Description:    Returns a float/double resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TUnixEntry * ep;
	ep = Entry;
	if (ep == NULL) {
		if ((ep = _unix_find_entry(EntryName, ValueNumber)) == NULL)	return(DefaultValue);
		else															return(atof(ep->curval));
	} else {
		if (_unix_find_value(ep, ValueNumber) == 0)						return(DefaultValue);	
		else															return(atof(ep->curval));
	}
}

const char * unix_string_getval_s(TUnixEntry * Entry, const char * EntryName, int ValueNumber, const char * DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_getval_s
// Purpose:        Get a resource value
// Arguments:      TUnixEntry * Entry      -- selected entry
//                 char * EntryName     -- resource name
//                 char * DefaultValue     -- default
// Results:        char * EntryValue    -- resource value
// Exceptions:     
// Description:    Returns a string resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TUnixEntry * ep;
	ep = Entry;
	if (ep == NULL) {
		if ((ep = _unix_find_entry(EntryName, ValueNumber)) == NULL)	return(DefaultValue);
		else															return(ep->curval);
	} else {
		if (_unix_find_value(ep, ValueNumber) == 0)						return(DefaultValue);	
		else															return(ep->curval);
	}
}

int unix_string_getval_b(TUnixEntry * Entry, const char * EntryName, int ValueNumber, int DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unix_string_getval_b
// Purpose:        Get a resource value
// Arguments:      TUnixEntry * Entry      -- selected entry
//                 char * EntryName     -- resource name
//                 int DefaultValue        -- default (0/1)
// Results:        int EntryValue       -- resource value (0/1)
// Exceptions:     
// Description:    Returns a boolean resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TUnixEntry * ep;
	ep = Entry;
	if (ep == NULL) {
		if ((ep = _unix_find_entry(EntryName, ValueNumber)) == NULL)					return((DefaultValue > 0) ? 1 : 0);
		else if (strcmp(ep->curval, "true") == 0 || strcmp(ep->curval, "TRUE") == 0)	return(1);
	} else {
		if (_unix_find_value(ep, ValueNumber) == 0)										return((DefaultValue > 0) ? 1 : 0);	
		else if (strcmp(ep->curval, "true") == 0 || strcmp(ep->curval, "TRUE") == 0)	return(1);
	}
	return(0);
}

FILE * _unix_open(const char * FileName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _unix_open
// Purpose:        Open environment file
// Arguments:      char * FileName         -- file name
// Results:        FILE * FilePtr          -- file ptr
// Exceptions:     returns NULL after errors
// Description:    Opens environment file and counts lines.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int n;
	char line[UXS_L_STR];
	FILE * fp;
			
	fp = fopen(FileName, "r");
	if (fp == NULL) return(NULL);
	n = 0;
	while (1) {
		if (fgets(line, UXS_L_STR - 1, fp) == NULL) break;
		if (line[0] != '#') n++;
	}
	fclose(fp);
	fp = fopen(FileName, "r");
	
	if (nofEntries > 0) {
		if (n <= nofEntries) {
			memset(lofEntries, 0, nofEntries * sizeof(TUnixEntry));
		} else {
			free(lofEntries);
			nofEntries = 0;
		}
	}
	
	if (nofEntries == 0) {		
		lofEntries = (TUnixEntry *) calloc(n, sizeof(TUnixEntry));
		if (lofEntries == NULL) return(NULL);
		nofEntries = n;
	}
	
	return(fp);
}

TUnixEntry * _unix_find_entry(const char * EntryName, int ValueNumber) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _unix_find_entry
// Purpose:        Find an entry by its name
// Arguments:      char * EntryName        -- entry name
//                 int ValueNumber         -- value number
// Results:        TUnixEntry * Entry      -- pointer to resource
// Exceptions:     returns NULL if no match
// Description:    Returns a pointer to a given entry or NULL.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	TUnixEntry * ep;
	
	ep = lofEntries;
	for (i = 0; i < nofEntries; i++, ep++) {
		if (strcmp(EntryName, ep->name) == 0) {
			if (ValueNumber < ep->nvals) {
				strncpy(ep->curval, ep->values[ValueNumber], UXS_L_STR - 1);
				return(ep);
			}
		}
	}
	return(NULL);
}		

int _unix_find_value(TUnixEntry * Entry, int ValueNumber) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _unix_find_value
// Purpose:        Prepare an entry value for being read
// Arguments:      TUnixEntry * Entry      -- entry name
//                 int ValueNumber         -- value number
// Results:        0/1
// Exceptions:
// Description:    Copies value to internal member 'curval'.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	if (ValueNumber < Entry->nvals) {
		strncpy(Entry->curval, Entry->values[ValueNumber], UXS_L_STR - 1);
		return(1);
	}
	return(0);
}		

void _unix_decode_entry(TUnixEntry * Entry, const char * UnixString) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _unix_decode_entry
// Purpose:        Decode unix entry
// Arguments:      TUnixEntry * Entry      -- entry, decoded
//                 char * UnixString      -- original entry, string
// Results:        TUnixEntry * Entry      -- entry, decoded
// Exceptions:     
// Description:    Decodes an unix-like entry <name>:<val1>:...:<valN>
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	char * sp;
	char eStr[UXS_L_STR];
	int nvals;
		
	strcpy(eStr, UnixString);
	strcat(eStr, ":");
	memset(Entry, 0, sizeof(TUnixEntry));
	sp = strchr(eStr, ':');
	if (sp) *sp = '\0';
	strncpy(Entry->name, eStr, UXS_L_STR - 1);
	strcpy(eStr, sp + 1);
	nvals = 0;
	while (sp = strchr(eStr, ':')) {
		*sp = '\0';
		strncpy(Entry->values[nvals], eStr, UXS_L_STR - 1);
		strcpy(eStr, sp + 1);
		nvals++;
		if (nvals >= UXS_N_VALS) break;
	}		
	Entry->nvals = nvals;
}
