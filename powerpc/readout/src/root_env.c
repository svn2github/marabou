/*_________________________________________________________________[C UTILITY]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       root_env.c
// @(#)Purpose:    Read ROOT's environment file using plain C
// Description:    Provides a means to get values from ROOT's environment
// @(#)Author:     R. Lutter
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// URL:            
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

static char sccsid[]	=	"%W%";

#include <stdio.h>

#define ENV_L_STR	128

/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TEnvEntry
// Purpose:        Store contents of a ROOT environment file
// Description:    Describes a resource line from a ROOT environment
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	char system[ENV_L_STR];
	char prog[ENV_L_STR];
	char name[ENV_L_STR];
	char value[ENV_L_STR];
	void * next;
} TEnvEntry;

/*______________________________________________________________[C PROTOTYPES]
////////////////////////////////////////////////////////////////////////////*/
int root_env_read(const char * FileName);
int root_env_getval_i(const char * ResourceName, int DefaultValue);
int root_env_getval_x(const char * ResourceName, int DefaultValue);
double root_env_getval_f(const char * ResourceName, double DefaultValue);
const char * root_env_getval_s(const char * ResourceName, const char * DefaultValue);
int root_env_getval_b(const char * ResourceName, int DefaultValue);
void root_env_print();

static TEnvEntry * _env_find_resource(const char * ResourceName);
static void _env_decode_name(TEnvEntry * Entry, const char * ResourceName);

char * strchr(char *, char);
char * strrchr(char *, char);
char * calloc(int, int);

/*___________________________________________________________________[GLOBALS]
////////////////////////////////////////////////////////////////////////////*/
static TEnvEntry * firstEntry = NULL;
static TEnvEntry * lastEntry = NULL;
static int nofEntries = 0;

int root_env_read(const char * FileName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_read
// Purpose:        Read ROOT's environment file
// Arguments:      char * FileName     -- file name
// Results:        int nofEntries      -- number of entries
// Exceptions:     returns -1 after errors
// Description:    Opens environment file and decodes entries.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i, n;
	char * sp;
	FILE * fp;
	TEnvEntry * entry;

	char line[ENV_L_STR];

	fp = fopen(FileName, "r");
	if (fp == NULL) return(-1);
	
	while (fgets(line, ENV_L_STR - 1, fp) != NULL) {
		if (line[0] == '#') continue;
		line[strlen(line) - 1] = '\0';
		sp = strchr(line, ':');
		if (sp == NULL) continue;
		*sp = '\0';
		entry = _env_find_resource(line);
		if (entry == NULL) {
			entry = (TEnvEntry *) calloc(1, sizeof(TEnvEntry)); 
			if (lastEntry) lastEntry->next = entry;
			lastEntry = entry;
			if (firstEntry == NULL) firstEntry = entry;
			_env_decode_name(entry, line);
			nofEntries++;		
		}
		sp++;
		while (*sp && isspace(*sp)) sp++;
		if (*sp != '#') strncpy(entry->value, sp, ENV_L_STR - 1);		
	}
	fclose(fp);
	return(nofEntries);
}

int root_env_getval_i(const char * ResourceName, int DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_getval_i
// Purpose:        Get a resource value
// Arguments:      char * ResourceName     -- resource name
//                 int DefaultValue        -- default
// Results:        int ResourceValue       -- resource value
// Exceptions:     
// Description:    Returns an integer resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * ep;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return(DefaultValue);	
	return((int) strtol(ep->value, NULL, 0));
}

int root_env_getval_x(const char * ResourceName, int DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_getval_x
// Purpose:        Get a resource value
// Arguments:      char * ResourceName     -- resource name
//                 int DefaultValue        -- default
// Results:        int ResourceValue       -- resource value
// Exceptions:     
// Description:    Returns a hex resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * ep;
	char str[64];
	int n, val;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return(DefaultValue);	
	strcpy(str, ep->value);
	n = (strncmp(str, "0x", 2) == 0) ? 2 : 0;
	sscanf(&str[n], "%x", &val);
	return(val);
}

double root_env_getval_f(const char * ResourceName, double DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_getval_f
// Purpose:        Get a resource value
// Arguments:      char * ResourceName     -- resource name
//                 double DefaultValue     -- default
// Results:        double ResourceValue    -- resource value
// Exceptions:     
// Description:    Returns a float/double resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * ep;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return(DefaultValue);
	return(atof(ep->value));
}

const char * root_env_getval_s(const char * ResourceName, const char * DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_getval_s
// Purpose:        Get a resource value
// Arguments:      char * ResourceName     -- resource name
//                 char * DefaultValue     -- default
// Results:        char * ResourceValue    -- resource value
// Exceptions:     
// Description:    Returns a string resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * ep;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return(DefaultValue);
	return(ep->value);
}

int root_env_getval_b(const char * ResourceName, int DefaultValue) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           root_env_getval_b
// Purpose:        Get a resource value
// Arguments:      char * ResourceName     -- resource name
//                 int DefaultValue        -- default (0/1)
// Results:        int ResourceValue       -- resource value (0/1)
// Exceptions:     
// Description:    Returns a boolean resource.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * ep;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return((DefaultValue > 0) ? 1 : 0);
	if (strcmp(ep->value, "true") == 0 || strcmp(ep->value, "TRUE") == 0) return(1); else return(0);
}

void root_env_print() {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _env_print
// Purpose:        Print environment
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Outputs environment settings to stdout.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	TEnvEntry * entry;
	char str[3 * ENV_L_STR];

	entry = firstEntry;
	if (entry) {
		printf("%-50s%-15s\n", "Resource", "Value");
		printf("-------------------------------------------------------------------------------------\n");
	}
	while (entry) {
		strcpy(str, entry->system);
		strcat(str, ".");
		strcat(str, entry->prog);
		strcat(str, ".");
		strcat(str, entry->name);
		strcat(str, ":");
		printf("%-50s%-15s\n", str, entry->value);
		entry = (TEnvEntry *) entry->next;
	}
	printf("\n[%d entries]\n\n", nofEntries);
}

TEnvEntry * _env_find_resource(const char * ResourceName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _env_find_resource
// Purpose:        Find a resource by its name
// Arguments:      char * ResourceName    -- resource name
// Results:        TEnvEntry * Entry      -- pointer to resource
// Exceptions:     returns NULL if no match
// Description:    Returns a pointer to a given resource or NULL.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	TEnvEntry e;
	TEnvEntry * entry;
	
	_env_decode_name(&e, ResourceName);
	
	entry = firstEntry;
	while (entry) {
		if (strcmp(e.name, entry->name) == 0) {
			if (entry->prog[0] == '*' || e.prog[0] == '*' || strcmp(e.prog, entry->prog) == 0) {
				if (entry->system[0] == '*' || e.system[0] == '*' || strcmp(e.system, entry->system) == 0) return(entry);
			}
		}
		entry = (TEnvEntry *) entry->next;
	}
	return(NULL);
}		

void _env_decode_name(TEnvEntry * Entry, const char * ResourceName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _env_decode_name
// Purpose:        Decode resource name
// Arguments:      TEnvEntry * Entry      -- resource name, decoded
//                 char * ResourceName    -- resource name, string
// Results:        TEnvEntry * Entry      -- resource name, decoded
// Exceptions:     
// Description:    Decodes a resource name <System>.<Prog>.<Name>
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	char * sp;
	char resName[ENV_L_STR];
		
	strcpy(resName, ResourceName);
	
	strcpy(Entry->system, "*");
	strcpy(Entry->prog, "*");
		
	sp = strrchr(resName, '.');
	if (sp == NULL) {
		strcpy(Entry->name, resName);
	} else {
		*sp = '\0';
		strcpy(Entry->name, sp + 1);
		sp = strrchr(resName, '.');
		if (sp == NULL) {
			strcpy(Entry->prog, resName);
		} else {
			*sp = '\0';
			strcpy(Entry->prog, sp + 1);
			strcpy(Entry->system, resName);
		}
	}
}
