/*FILE////////////////////////////////////////////////////////////////////////
// Name:
//		@(#) %M%
// Purpose:
//		@(#) Read & execute cnaf commands
// Description:
//		Functions to read & execute cnaf commands.
//		Cnafs are expected to be ASCII-coded as described in the
//		MBS User's Manual.
// Modules:
//	.	cnaf_decode		-- decode ASCII string containing cnaf command
//	.	cnaf_read_file	-- read cnaf commands from a file
//	.	cnaf_exec		-- execute a list of cnafs
// Header files:
//	.	stdio.h			-- standard i/o
//	.	gd_cnaf.h		-- cnaf definitions
// Keywords:
//
// Author:
//		@(#) R. Lutter
// Revision:
//		SCCS:  %W% (%G%)
////////////////////////////////////////////////////////////////////////////*/

static char sccsid[]	=	"%W%";

#include <stdio.h>
#include "gd_cnaf.h"
#include "gd_camio.h"

#define STR_LEN		256

/*C FUNCTION/////////////////////////////////////////////////////////////////
// Name:
//		cnaf_decode
// Purpose:
//		Decode an ASCII string containing a cnaf command
// Calling:
//		boolean cnaf_decode(char* string, CnafSpec *cnaf, char *error)
// Arguments:
//	.	char		*cline		-- string containing cnaf command
//	.	CnafSpec	*cnaf		-- structure to store cnaf specs
//	.	char        *error		-- where to store an error message
// Results:
//		returns 1 on success, 0 on error
//	.	cnaf		-- decoded cnaf specs
//	.	error		-- contains type of error
// Exceptions:
//		Several checks are made if cnaf command is legal.
//		A 0 is returned on errors.
//		If <error> != NULL, a detailed error message is stored in <error>.
// Description:
//		Looks for a file on user's disk named .on-trigger-<trigger>
//		File should contain standard MBS cnaf definitions, one per line,
//		cnaf data separated by commas:
//				crate=<c>, nstation=<n>, address=<a>, function=<f>,
//                                             [data=<d>,] [repetition=<r>]
//		Data may be given in decimal, octal (0...), hex (0x...),
//		or binary (0b...) format.
// Keywords:
//
///////////////////////////////////////////////////////////////////////////*/

int cnaf_decode(char *cline, CnafSpec *cnaf, char *error)

{
	register int i;
	register char *sp1, *sp2;
	char *sp3, *eqs;
	char *ep;

	int lng;
	int n, dummy;
	char cstr[STR_LEN];
	char vstr[STR_LEN];

	int errflg;

	struct cs {
		char *key;
		char *shkey;
		int *ptr;
		int llim;
		int ulim;
	};

	struct cs *cp;

	static CnafSpec this_cnaf;

	static struct cs cnaf_x[]	=	{
				{ "crate",		"C",	&this_cnaf.crate,	0,	CRATE_MAX },
				{ "nstation",	"N",	&this_cnaf.station,	1,	31 },
				{ "address",	"A",	&this_cnaf.addr1,	0,	15 },
				{ "address",	"A",	&this_cnaf.addr2,	0,	15 },
				{ "function",	"F",	&this_cnaf.function, 0,	31 },
				{ "data",		"d",	(int *) &this_cnaf.data,	-1, -1 },
				{ "repetition",	"r",	&this_cnaf.repeat,	-1,	-1 },
				{ "",			"",		NULL,				0,	0 }
									};

	struct ls {
		char *label;
		unsigned int type;
	};

	struct ls *lp;

	static struct ls camlabel[]	=	{	
				{	"caminit",		CNAF_LT_INIT },
				{	"camread",		CNAF_LT_READ },
				{	NULL,			0 }
									};

	char *strchr(char *, char);

	if (error != NULL) {				/* return detailed error messages? */
		errflg = 1;
		*error = '\0';
	} else {
		errflg = 0;
	}

	strncpy(cstr, cline, STR_LEN);		/* copy original cnaf string */

	sp1 = cstr;							/* delete leading spaces */
	while (isspace(*sp1)) sp1++;
	if (*sp1 == '\0' || *sp1 == '#') {	/* empty line or comment */
		return(1);
	}
	sp2 = sp1 + strlen(sp1) - 1;		/* delete trailing spaces */
	while (isspace(*sp2)) sp2--;
	*++sp2 = '\0';

	sp2 = sp1;							/* convert ot lower case */
	for (i = 0; i < strlen(sp1); i++, sp2++) *sp2 = tolower(*sp2);

	sp2 = sp1 + strlen(sp1) - 1;		/* append comma if necessary */
	if (*sp2 != ',') {
		*++sp2 = ',';
		*++sp2 = '\0';
	}

	this_cnaf.crate = -1;			/* initialize cnaf spec */
	this_cnaf.station = -1;
	this_cnaf.addr1 = -1;
	this_cnaf.addr2 = -1;
	this_cnaf.function = -1;
	this_cnaf.data = -1;
	this_cnaf.repeat = 1;			/* repeat only once normally */
	this_cnaf.type = -1;			/* cnaf type unknown */
	this_cnaf.label = -1;			/* label unknown */

	sp2 = sp1;						/* test for possible label */
	while (!isspace(*sp2)) {
		if (*sp2 == ':') {
			*sp2 = '\0';
			lp = camlabel;
			while (lp->label) {
				if (strcmp(sp1, lp->label) == 0) {
					this_cnaf.label = lp->type;
					break;
				}
				lp++;
			}
			if (this_cnaf.label == -1) {
				if (errflg) sprintf(error, "Illegal label >> %s: <<", sp1);
				return(0);				/* return with error */
			}
			sp1 = sp2 + 1;
			while (isspace(*sp1)) sp1++;
		}
		sp2++;
	}
	if (this_cnaf.label == -1) this_cnaf.label = 0;

	while (*sp1) {					/* decode cnaf string */

		sp2 = strchr(sp1, ',');		/* comma is delimiter */
		*sp2 = '\0';				/* end of partial command */
		if ((eqs = strchr(sp1, '=')) == NULL) {
			if (errflg) sprintf(error, "Illegal assignment >> %s <<", sp1);
			return(0);				/* return with error */
		}

		*eqs = '\0';				/* extract key word */
		sp3 = eqs + 1;				/* points to value now */
		eqs--;
		while (isspace(*sp1)) sp1++;	/* skip spaces */
		while (isspace(*eqs)) eqs--;
		lng = strlen(sp1);

		cp = cnaf_x;				/* find key word: c,n,a,f,d,r */
		while (*cp->key) {
			if (strncmp(cp->key, sp1, lng) == 0) break;
			cp++;
		}
		if (cp->key == NULL) {
			if (errflg) sprintf(error, "Keyword unknown >> %s <<", sp1);
			return(0);				/* return with error */
		}

		while (isspace(*sp3)) sp3++;	/* skip leading spaces */
		ep = sp3;						/* points to original string */
		if (strchr(sp3, ':') == NULL) {	/* value is single number */
			sprintf(vstr, "%s 1", sp3);	/* test number format */
			sp3 = vstr;
			n = 0; dummy = 0;			/* start with "illegal format" */
			if (isdigit(*sp3)) {
				if (*sp3 == '0') {
					sp3++;
					if (isspace(*sp3)) {
						*cp->ptr = 0;		/* value = 0 */
						n = 2; dummy = 1;	/* fake "format ok" */
					} else if (*sp3 == 'x') {	/* hex */
						sp3++;
						n = sscanf(sp3, "%lx%d", cp->ptr, &dummy);
					} else if (*sp3 == 'b') {	/* binary */
						sp3++;
						*cp->ptr = 0;
						n = 2; dummy = 1;		/* fake "format ok" */
						while (!isspace(*sp3)) {
							*cp->ptr <<= 1;
							switch (*sp3) {
								case '0':
									break;
								case '1':
									*cp->ptr |= 1;
									break;
								default:
									n = 0;		/* error */
									break;
							}
							sp3++;
						}
					} else {					/* octal */
						n = sscanf(sp3, "%lo%d", cp->ptr, &dummy);
					}
				} else {
					n = sscanf(sp3, "%ld%d", cp->ptr, &dummy); /* decimal */
				}
			}
		} else {						/* value is range n:m */
			if (cp->ptr == &this_cnaf.addr1) {	/* must be subaddress */
				n = sscanf(sp3, "%d:%d", &this_cnaf.addr1, &this_cnaf.addr2);
				dummy = 1;
			}
		}
		if (n != 2 || dummy != 1) {
			if (errflg) sprintf(error, "Illegal format  >> %s <<", ep);
			return(0);				/* return with error */
		}
		if (*cp->shkey == 'd') *cp->ptr &= 0xffffff; 	/* data have 24 bits only */
		sp1 = sp2 + 1;
	}

	if (this_cnaf.addr2 == -1) this_cnaf.addr2 = this_cnaf.addr1;

	ep = NULL;
	if (this_cnaf.crate == -1) {ep = "crate C=xx";}
	else if (this_cnaf.station == -1) {ep = "station N=xx";}
	else if (this_cnaf.addr1 == -1) {ep = "subaddress A=xx";}
	else if (this_cnaf.function == -1) {ep = "function F=xx";}
	if (ep != NULL) {
		if (errflg) sprintf(error, "Incomplete cnaf - %s missing", ep);
		return(0);				/* return with error */
	}

	cp = cnaf_x;
	while (*cp->key) {
		if (cp->llim > 0) {
			if (*cp->ptr < cp->llim || *cp->ptr > cp->ulim) {
				if (errflg) sprintf(error,
						"Illegal cnaf - %s %s=%d not in [%d,%d]",
									cp->key, cp->shkey, cp->llim, cp->ulim);
				return(0);
			}
		}
		cp++;
	}

	if (this_cnaf.addr2 < this_cnaf.addr1) {
		if (errflg) sprintf(error,
					"Illegal cnaf - address A=%d > A=%d",
										this_cnaf.addr1, this_cnaf.addr2);
		return(0);
	}

	if (this_cnaf.function < 8) {
		if (this_cnaf.data > 0) {
			if (errflg) sprintf(error,
							"Input cnaf F=%d cannot take data", this_cnaf.function);
			return(0);
		} else {
			this_cnaf.type = CNAF_T_READ;
		}
	} else if (this_cnaf.function < 16) {
		if (this_cnaf.data > 0) {
			if (errflg) sprintf(error,
							"Control cnaf F=%d cannot take data", this_cnaf.function);
			return(0);
		} else {
			this_cnaf.type = CNAF_T_CONTROL;
		}
	} else if (this_cnaf.function < 24) {
		if (this_cnaf.data == -1) {
			if (errflg) sprintf(error,
						"Incomplete output cnaf - data missing for F=%d",
													this_cnaf.function);
			return(0);
		} else {
			this_cnaf.type = CNAF_T_WRITE;
		}
	} else if (this_cnaf.data > 0) {
			if (errflg) sprintf(error,
							"Control cnaf F=%d cannot take data", this_cnaf.function);
			return(0);
	}

	memcpy(cnaf, &this_cnaf, sizeof(CnafSpec));		/* pass data to user */
	return(1);
}

/*C FUNCTION/////////////////////////////////////////////////////////////////
// Name:
//		cnaf_read_file
// Purpose:
//		Read cnaf commands from file
// Calling:
//		int cnaf_read_file(char *file, CnafSpec *cnaf, int ncnafs)
// Arguments:
//	.	char     *file		-- file spec
//	.	CnafSpec *cnaf		-- structure to store cnaf specs
//	.	int      ncnafs		-- max length of struct cnaf
// Results:
//		returns number of cnafs read or 01 on error
//	.	cnaf	-- list of structures containing decoded cnaf specs
// Exceptions:
//		Serveral checks are made by function "cnaf_decode".
//		Returns 0 on error.
// Description:
//		Reads cnaf commands from specified file.
//		Calls function "cnaf_decode" to decode it line by line.
//		Cnafs are NOT executed but stored in a structure.
//		A call function "cnaf_exec" will fire this list.
//
//		If <ncnafs> = 0, no data will be stored but a syntax test is
//		performed.
// Keywords:
//
///////////////////////////////////////////////////////////////////////////*/

int cnaf_read_file(char *file, CnafSpec *cnaf, int ncnafs)

{
	register FILE *f;
	register char *sp1;
	register CnafSpec *cp;
	int lno;
	int lng;
	int nc;
	char cline[STR_LEN];
	char error[STR_LEN];

	CnafSpec dummy_cnaf;

	char *strrchr(char *, char);

	if ((f = fopen(file, "r")) == NULL) {
		fprintf(stderr, "%s: Can't open file\n", file);
		return(0);
	}

	sp1 = strrchr(file, '/');			/* strip off file path */
	if (sp1 != NULL) file = sp1 + 1;

	cp = ncnafs ? cnaf : &dummy_cnaf;
	lno = 0;
	nc = 0;
	while (fgets(cline, STR_LEN, f) != NULL) {
		lno++;							/* line count */
		lng = strlen(cline);
		cline[lng - 1] = '\0';			/* remove newline */
		sp1 = cline;					/* delete leading spaces */
		while (isspace(*sp1)) sp1++;
		if (*sp1 == '\0' || *sp1 == '#') continue;	/* empty line or comment */

		if (cnaf_decode(cline, cp, error) == 0) {
			fprintf(stderr, "%s (%d): %s\n", file, lno, error);
			fclose(f);
			return(0);
		}

		nc++;
		if (ncnafs) cp++;
	}
	return(nc);
}

/*C FUNCTION/////////////////////////////////////////////////////////////////
// Name:
//		cnaf_exec
// Purpose:
//		Execute a list of cnafs
// Calling:
//		boolean cnaf_exec(CnafSpec *cnaf, int ncnafs, long * pl_loc_hwacc, long * pl_rem_cam)
// Arguments:
//	.	CnafSpec *cnaf			-- List of cnaf specs
//	.	int      ncnafs			-- length of struct cnaf
//	.	long * pl_loc_hwacc 	-- camac base (local)
//	.	long * pl_rem_cam		-- (remote)
// Results:
//		returns 1 on success, 0 on error
// Exceptions:
//
// Description:
//		Executes <ncnafs> cnaf specs from list <cnaf>.
//		Cnaf list may have been filled either by a program or
//		by reading data from a string or a file.
// Keywords:
//
///////////////////////////////////////////////////////////////////////////*/

#ifdef MBS
int cnaf_exec(CnafSpec *cnaf, int ncnafs, long * pl_loc_hwacc, long * pl_rem_cam)

{
	register int rep, addr;
	unsigned long data;

	for (; ncnafs--; cnaf++) {
		for (rep = 0; rep < cnaf->repeat; rep++) {
			for (addr = cnaf->addr1; addr <= cnaf->addr2; addr++) {
				switch (cnaf->type) {
					case CNAF_T_CONTROL:
						*(CNAF_ADDR(C(cnaf->crate),N(cnaf->station),A(addr),F(cnaf->function)));
						break;
					case CNAF_T_READ:
						data = *(CNAF_ADDR(C(cnaf->crate),N(cnaf->station),A(addr),F(cnaf->function)));
						printf("C%d.N%d.A%d.F%d: %d %0o %#x\n",
								cnaf->crate, cnaf->station, addr, cnaf->function,
								data, data, data);
						break;
					case CNAF_T_WRITE:
						*(CNAF_ADDR(C(cnaf->crate),N(cnaf->station),A(addr),F(cnaf->function))) = data;
						break;
				}
			}
		}
	}
}
#else
int cnaf_exec(CnafSpec *cnaf, int ncnafs)

{
	register CnafSpec *cp;
	register int rep, addr;
	unsigned long data;

	for (; ncnafs--; cp++) {
		for (rep = 0; rep < cp->repeat; rep++) {
			for (addr = cp->addr1; addr <= cp->addr2; addr++) {
				switch (cp->type) {
					case CNAF_T_CONTROL:
						printf("CTRL: C%d.N%d.A%d.F%d\n",
									cp->crate, cp->station, addr, cp->function);
						break;
					case CNAF_T_READ:
						printf("READ: C%d.N%d.A%d.F%d\n",
									cp->crate, cp->station, addr, cp->function);
						break;
					case CNAF_T_WRITE:
						printf("WRITE: C%d.N%d.A%d.F%d, data=%d\n",
									cp->crate, cp->station, addr, cp->function, cp->data);
						break;
				}
			}
		}
	}
}
#endif
