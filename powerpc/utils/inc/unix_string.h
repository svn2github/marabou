#ifndef __UNIX_STRING_H__
#define __UNIX_STRING_H__

/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUnixEntry
// Purpose:        Store contents of a ROOT environment file
// Description:    Describes a resource line from a ROOT environment
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define UXS_N_VALS	5
#define UXS_L_STR	128

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

/*______________________________________________________________[C PROTOTYPES]
////////////////////////////////////////////////////////////////////////////*/

TUnixEntry * unix_string_get_entry(int EntryNumber);
int unix_string_read(const char * FileName);
int unix_string_getval_i(TUnixEntry * Entry, const char * EntryName, int ValueName, int DefaultValue);
double unix_string_getval_f(TUnixEntry * Entry, const char * EntryName, int ValueNumber, double DefaultValue);
const char * unix_string_getval_s(TUnixEntry * Entry, const char * EntryName, int ValueNumber, const char * DefaultValue);
int unix_string_getval_b(TUnixEntry * Entry, const char * EntryName, int ValueNumber, int DefaultValue);

#endif
