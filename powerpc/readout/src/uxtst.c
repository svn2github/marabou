#include <stdio.h>

#define UXS_N_VALS	5
#define UXS_L_STR	128

/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUnixEntry
// Purpose:        Store contents of a ROOT environment file
// Description:    Describes a resource line from a ROOT environment
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

int unix_string_getval_i(TUnixEntry * Entry, const char * EntryName, int ValueName, int DefaultValue);
double unix_string_getval_f(TUnixEntry * Entry, const char * EntryName, int ValueNumber, double DefaultValue);
const char * unix_string_getval_s(TUnixEntry * Entry, const char * EntryName, int ValueNumber, const char * DefaultValue);
int unix_string_getval_b(TUnixEntry * Entry, const char * EntryName, int ValueNumber, int DefaultValue);

TUnixEntry * unix_string_get_entry(int EntryNumber);

main(int argc, char * argv[])

{
	int i, n, nv;
	TUnixEntry * e;
		
	n = unix_string_read("ux.dat");
	if (n < 0)
	{
		fprintf(stderr, "File \"ux.dat\" not found\n");
		exit(1);
	}
	
	switch (*argv[1]) {
		case 'i':
			printf("%s(%s): %d\n", argv[2], argv[3], unix_string_getval_i(NULL, argv[2], atoi(argv[3]), 0));
			break;
		case 'f':
			printf("%s(%s): %f\n", argv[2], argv[3], unix_string_getval_f(NULL, argv[2], atoi(argv[3]), 0));
			break;
		case 's':
			printf("%s(%s): %s\n", argv[2], argv[3], unix_string_getval_s(NULL, argv[2], atoi(argv[3]), "none"));
			break;
		case 'b':
			printf("%s(%s): %d\n", argv[2], argv[3], unix_string_getval_b(NULL, argv[2], atoi(argv[3]), 0));
			break;
		case 'a':
			for (i = 0; i < n; i++) {
				e = unix_string_get_entry(i);
				for (nv = 0; nv < e->nvals; nv++) {
					printf("%s(%d): %s\n", e->name, nv, unix_string_getval_s(e, NULL, nv, "none"));
				}
			}
			break;
	}
}	
