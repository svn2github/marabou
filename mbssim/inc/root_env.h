#ifndef __ROOT_ENV_H__
#define __ROOT_ENV_H__

#define ENV_L_STR	128
#define ENV_N_DOTS	10

#define TRUE	1
#define FALSE	0

#define MBS_SIMUL_RDO 1

/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TEnvEntry
// Purpose:        Store contents of a ROOT environment file
// Description:    Describes a resource line from a ROOT environment
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	int nsubs;
	char resstr[ENV_L_STR];
	char * substr[ENV_N_DOTS + 1];
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

TEnvEntry * _env_find_resource(const char * ResourceName);
void _env_decode_name(TEnvEntry * Entry, const char * ResourceName);

#endif
