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

#include <stdio.h>
#include "root_env.h"

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
	int val;
	if ((ep = _env_find_resource(ResourceName)) == NULL) return(DefaultValue);	
	val = (int) strtol(ep->value, NULL, 0);
	return(val);
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
	int i;

	entry = firstEntry;
	if (entry) {
		printf("%-50s %-15s\n", "Resource", "Value");
		printf("-------------------------------------------------------------------------------------\n");
	}
	while (entry) {
		str[0] = '\0';
		for (i = 0; i < entry->nsubs; i++) {
			if (i > 0) strcat(str, ".");
			strcat(str, entry->substr[i]);
		}
		printf("%-50s:%-15s\n", str, entry->value);
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

	int i, j;
	TEnvEntry e;
	TEnvEntry * entry;
	int n1, n2, match;
	char subs1[100], subs2[100];
	
	_env_decode_name(&e, ResourceName);
	
	entry = firstEntry;
	while (entry) {
		if (entry->nsubs >= e.nsubs) {
			n1 = e.nsubs - 1;
			n2 = entry->nsubs - 1;
			match = TRUE;
			for (i = 0; i < e.nsubs; i++, n1--, n2--) {
				strcpy(subs1, e.substr[n1]);
				for (j = 0; j < 100; j++) {
					if (subs1[j] == '\0') break;
					subs1[j] = toupper(subs1[j]);
				}
				strcpy(subs2, entry->substr[n2]);
				for (j = 0; j < 100; j++) {
					if (subs2[j] == '\0') break;
					subs2[j] = toupper(subs2[j]);
				}
				if (strcmp(subs1, "*") != 0 && strcmp(subs1, subs2) != 0) {
					match = FALSE;
					break;
				}
			}
			if (match) return(entry);
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
// Description:    Decodes a resource name
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	char * sp;
	
	Entry->nsubs = 0;
	strcpy(Entry->resstr, ResourceName);
	sp = Entry->resstr;
	for(;;) {
		Entry->substr[Entry->nsubs++] = sp;
		sp = strchr(sp, '.');
		if (sp == NULL) break;
		*sp = '\0';
		sp++;
	}
}
