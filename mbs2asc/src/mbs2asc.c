#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "mbsio.h"
#include <errno.h>

#define CAEN_V7X5_SH_WC							8
#define CAEN_V7X5_SH_CHN						16

#define CAEN_V7X5_D_HDR							0x02000000
#define CAEN_V7X5_D_DATA						0x00000000
#define CAEN_V7X5_D_EOB							0x04000000
#define CAEN_V7X5_D_INVALID						0x06000000

#define CAEN_V7X5_M_MSERIAL						0xFF
#define CAEN_V7X5_M_WC							0x3F
#define CAEN_V7X5_M_CHN							0xFF
#define CAEN_V7X5_M_ID							0x07000000
#define CAEN_V7X5_M_HDR							0x07FFFFFF
#define CAEN_V7X5_M_DATA 						0x07FFFFFF
#define CAEN_V7X5_M_ADCDATA	 					0x00000FFF

#define CAEN_V7X5_B_OVERFLOW	 				(0x1 << 12)
#define CAEN_V7X5_B_UNDERTHRESH 				(0x1 << 13)

#define CAEN_V7X5_N_MAXEVENTS					32

#define MADC_SH_CHN							16
#define MADC_SH_SERIAL						16
#define MADC_SH_RES 						11

#define MADC_D_HDR							0x40000000
#define MADC_D_DATA							0x00000000
#define MADC_D_EOE							0xC0000000

#define MADC_D_XTS							0x04800000

#define MADC_M_SERIAL						0xFF
#define MADC_M_RES							0x7
#define MADC_M_WC							0x7FF
#define MADC_M_CHN							0x1F
#define MADC_M_XTS							0xFFFF0000
#define MADC_M_ID							0xC0000000
#define MADC_M_ADCDATA	 					0x00001FFF
#define MADC_M_TSLOW		 				0x3FFFFFFF
#define MADC_M_TSHIGH		 				0x0000FFFF

#define MADC_M_RES2K			 			0x000007FF
#define MADC_M_RES4K			 			0x00000FFF
#define MADC_M_RES8K			 			0x00001FFF

enum 	{	kMrbSevt_Sis_B_Header 			=	(0x1 << 15) };
enum 	{	kMrbSevt_Sis_M_ModuleNumber		=	0xFF		};
enum 	{	kMrbSevt_Sis_M_ModuleId			=	0x7F		};
enum 	{	kMrbSevt_Sis_SH_ModuleId		=	8			};
enum 	{	kMrbModIdDgf					=	0x100010	};
enum 	{	kMrbModIdCaen_v785				=	0x4000d		};
enum 	{	kMrbModIdCaen_V775				=	0x40015		};
enum 	{	kMrbModIdSis_3801				=	0x1000019	};
enum 	{	kMrbModIdSis_3600				=	0x1000018	};
enum 	{	kMrbModIdSis_3820				=	0x100001e	};
enum 	{	kMrbModIdSis_3300				=	0x100001f	};
enum	{	kMrbModIdMadc32 				=	0x8000024 	};

enum	{ kTRUE		=	1	};
enum	{ kFALSE	=	0	};

enum	{ kStrlen		=	1000	};
enum	{ kNofModules	=	100 	};
enum	{ kNofChannels	=	256 	};
enum	{ kNofModes 	=	20	 	};
enum	{ kNofTriggers	=	13		};
enum	{ kMaxErrors	=	10		};

enum	{	kDgfNofChannels 		=	4	};
enum	{	kDgfMaxNofChannelWords 	=	9	};

typedef struct {
	short nofWords;			/* number of data words in this buffer */
	short moduleNumber;		/* module number */
	unsigned short formatDescr;		/* format descriptor */
	unsigned short startTime[3];		/* run start time */
} DgfEventBufferHeader;

typedef struct {
	unsigned short hitPattern;		/* channel hit pattern */
	unsigned short eventTime[2];		/* event time */
} DgfEventHeader;

typedef struct {
	unsigned short fastTriggerTime;	/* fast trigger time */
	unsigned short energy;			/* energy */
	unsigned short xiaPSA;			/* XIA PSA value */
	unsigned short userPSA;			/* user PSA */
	unsigned short GFLTHi;			/* GFLT value */
	unsigned short GFLTMi;
	unsigned short GFLTLO;
	unsigned short raw;				/* reserved */
} DgfEventChannelHeader;

unsigned int mbs_next_sheader(const MBSDataIO *);
unsigned int mbs_next_sdata(const MBSDataIO *);
const MBSDataIO * mbs_open_file(const char *, const char *, int, FILE *);
int mbs_close_file(const MBSDataIO *);
unsigned int mbs_next_event(const MBSDataIO *);

int readMbsData(char *, char *, int);
int processEvent(MBSDataIO *);
int extractSubevents(MBSDataIO *);
int processSubevent_dgf(MBSDataIO *);
int processSubevent_caen(MBSDataIO *);
int processSubevent_madc(MBSDataIO *);
int processSubevent_scaler(MBSDataIO *);
int processSubevent_sevt_10_1x(MBSDataIO *);
int processSubevent_sis(MBSDataIO *);
int mbs_get_sevent_serial(MBSDataIO *);
unsigned long long calcTime48(unsigned short, unsigned short, unsigned short);
void writeHeader(int, char **);
void allocRingBuffer(MBSDataIO *);
char * nextRB(int *);
char * getRB();
void putRB(MBSDataIO *);
void dumpRB();
void usage();
char * strip(char *);
int decode_envstr(char *, char *, char *[], int);

extern char *optarg;		/* getopt(3) */
extern int optind;

char * prgName;
char rcFile[kStrlen];
char mbsFile[kStrlen];
char logFile[kStrlen];
char dumpMode[kStrlen];
int maxEvents;
int absStart;
int absEnd;
int mbsStart;
int mbsEnd;
int readoutTrigger[kNofTriggers + 1];
const MBSDataIO * mbs;
int verboseMode;
int bufferDump;
int dgfFormat;
int nofErrors;
int nofEvents;
int nofEventsProcessed;
int hasRcFile;
int isSevtDump;
int nofEvts;

int filterCamac;
int filterDgf;
int filterCaen;
int filterSis;
int filterMadc;

char * ringBuffer;
char ** rbPtrs;
int * rbEvtno;
int * rbEvtsiz;
int rbGet, rbPut, rbCnt, rbSize;

const char * cmdFmt = "[-r <rcFile>] [-n <maxEvents|start:end>] [-m <mbsStart:mbsEnd>] [-t <rdoTrig>] [-f <dgfFmt>] [-d <sevtType>] [-s <nbufs>][-v] [-b] [-l <logFile>] <mbsFile>";
const char * evtFmt = "%-6s%-6s%8d%8d%8d%8d%8d";
const char * sevtFmt = "%-6s%-6s%8d%8d%8d";
const char * dgfBufFmt = "%-6s%-6s%8d%8d%8d%8d%8d%8d%16lld";
const char * dgfEvtFmt = "%-6s%-6s%8d%32d%8d%16lld";
const char * dgfTraceFmt = "%-6s%-6s%8d%8d";
const char * dgfTdataFmt = "%-6s%-6s%7d%7d%7d%7d%7d";
const char * dgfChnFmt = "%-6s%-6s%8d%8d%8d%24d%16lld";
const char * caenHdrFmt = "%-6s%-6s%8d%8d";
const char * caenChnFmt = "%-6s%-6s%8d%8d";
const char * caenEoeFmt = "%-6s%-6s%8d";
const char * s10xxHdrFmt = "%-6s%-6s%8d";
const char * s10xxData16Fmt = "%-6s%-6s%8d%8d";
const char * s10xxData32Fmt = "%-6s%-6s%8d%16d";
const char * sisHdrFmt = "%-6s%-6s%8d%8d%8d";
const char * sisDataFmt = "%-6s%-6s%8d%8d";
const char * sis33xxInfoFmt = "%-6s%-6s%8d%8d%8d%8x%8x";
const char * madcHdrFmt = "%-6s%-6s%4d%4d%8d";
const char * madcChnFmt = "%-6s%-6s%8d%8d";
const char * madcXtsFmt = "%-6s%-6s%16u";
const char * madcEoe1Fmt = "%-6s%-6s%16u";
const char * madcEoe2Fmt = "%-6s%-6s%16u%16u";

int main(int argc, char * argv[]) {
/*_______________________________________________________________________________________________[MAIN]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   mbs2asc
// Purpose: 	   output mbs/med data in ascii format
// Syntax:         mbs2asc       [-r <rcFile>]
//                               [-n <maxEvents|start:end>]
//                               [-m <mbsStart:mbsEnd>]
//                               [-t <rdoTrig>] [-f <dgfFmt>]
//                               [-s <sevtType>]
//                               [-v] [-d]
//                               [-l logFile] <mbsFile>
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;
	int opt;
	int sts;
	int rTrig;
	int nofRcEntries;

	char * sp;

	prgName = strrchr(argv[0], '/');
	if (prgName == NULL) prgName = argv[0]; else prgName++;

	maxEvents = 0;
	absStart = -1;
	absEnd = -1;
	mbsStart = -1;
	mbsEnd = -1;

	nofEvts = 0;

	rcFile[0] = '\0';
	mbsFile[0] = '\0';
	logFile[0] = '\0';
	dumpMode[0] = '\0';

	verboseMode = kFALSE;
	bufferDump = kFALSE;
	dgfFormat = 0;

	nofErrors = 0;
	nofEvents = 0;
	nofEventsProcessed = 0;

	memset(readoutTrigger, 0, (kNofTriggers + 1) * sizeof(int));
	rTrig = 0;

	if (argc == 1) {
		usage();
		exit(0);
	}

	while ((opt = getopt(argc, argv, "r:n:m:t:f:d:s:l:vb")) != EOF)
	{
		switch (opt)
		{
			case 'r':	strcpy(rcFile, optarg);
						break;
			case 'n':	if ((sp = strchr(optarg, ':')) == NULL) {
							maxEvents = atoi(optarg);
							absStart = 0;
							absEnd = maxEvents - 1;
						} else {
							*sp = '\0';
							absStart = atoi(optarg);
							absEnd = atoi(sp + 1);
							maxEvents = absEnd - absStart + 1;
						}
						if (maxEvents <= 0) {
							fprintf(stderr, "?WRGCNT-[%s]- Wrong event count - start=%d, end=%d, cnt=%d\n", prgName, absStart, absEnd, maxEvents);
							exit(1);
						}
						break;
			case 'm':	if ((sp = strchr(optarg, ':')) == NULL) {
							fprintf(stderr, "?WRGOPT-[%s]- Wrong option - \"-m maxEvents\" (use \"-n\" instead)\n", prgName);
							exit(1);
						} else {
							*sp = '\0';
							mbsStart = atoi(optarg);
							mbsEnd = atoi(sp + 1);
							maxEvents = mbsEnd - mbsStart + 1;
						}
						if (maxEvents <= 0) {
							fprintf(stderr, "?WRGCNT-[%s]- Wrong event count - start=%d, end=%d, cnt=%d\n", prgName, mbsStart, mbsEnd, maxEvents);
							exit(1);
						}
						break;
			case 't':	rTrig = atoi(optarg);
						if (rTrig > 0 && rTrig <= kNofTriggers) {
							readoutTrigger[rTrig] = kTRUE;
						} else {
							fprintf(stderr, "?ILLTRG-[%s]- Illegal readout trigger - %d (should be in [1,%d])\n", prgName, rTrig, kNofTriggers);
							exit(1);
						}
						break;
			case 'f':	dgfFormat = strtol(optarg, NULL, 0);
						break;
			case 'd':	strcpy(dumpMode, optarg);
						if (strcmp(dumpMode, "dgf") != 0 && strcmp(dumpMode, "caen") != 0 && strcmp(dumpMode, "madc") != 0) {
							fprintf(stderr, "?ILLMOD-[%s]- Illegal dump mode - %s (should be \"dgf\", \"caen\", or \"madc\")\n", prgName, dumpMode);
							exit(1);
						}
						break;
			case 's':	nofEvts = atoi(optarg);
						break;
			case 'l':	strcpy(logFile, optarg);
						break;
			case 'v':	verboseMode = kTRUE;
						break;
			case 'b':	bufferDump = kTRUE;
						break;
			default:	exit(1);
		}
	}
	if (argc > optind) strcpy(mbsFile, argv[optind]);	/* mbs data file */

	if (rTrig == 0) {
		for (i = 1; i < kNofTriggers; i++) readoutTrigger[i] = kTRUE;	/* no trigger defined - allow any */
	}

	if (mbsEnd > 0 && absEnd > 0) {
		fprintf(stderr, "?WRGOPT-[%s]- Conflicting options - \"-n\" ... \"-m\"\n", prgName);
		exit(1);
	}

	if (mbsFile[0] == '\0') {
		fprintf(stderr, "?NOFILE-[%s]- MBS data file missing (either .lmd or .med)\n", prgName);
		exit(1);
	}

	hasRcFile = (rcFile[0] != '\0');
	if (hasRcFile == kTRUE) {
		nofRcEntries = root_env_read(rcFile);		/* read indices and defs */
		if (nofRcEntries == -1) {
			fprintf(stderr, "?RCERR-[%s]- Error reading rc file \"%s\"\n", prgName, rcFile);
			hasRcFile = kFALSE;
		} else if (nofRcEntries == 0) {
			fprintf(stderr, "?NORCE-[%s]- No entries in rc file \"%s\"\n", prgName, rcFile);
			hasRcFile = kFALSE;
		}
	}

	isSevtDump = (dumpMode[0] != '\0');

	if (logFile[0] != '\0') mbs_open_log(logFile);

	writeHeader(argc, argv);							/* preface */

	if (isSevtDump == kFALSE) {
		readMbsData(mbsFile, rcFile, maxEvents);
	} else {
		readSevtDump(mbsFile, rcFile, dumpMode);
	}
}

int readMbsData(char * mbsFile, char * rcFile, int maxEvents) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   readMbsData
// Purpose: 	   Read data from mbs file
// Arguments:	   char * mbsFile   -- file name (.med or .lmd)
//  			   char * rcFile 	-- indices and defs (opt.)
//  			   int maxEvents    -- max number of events to be processed
// Results: 	   kTRUE/kFALSE
// Description:    Opens mbs data file and reads event by event.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;
	unsigned int eventType = 0;
	int abortOnError = 0;
	MBSDataIO * mbs;

	if (maxEvents == 0) maxEvents = 1000000000;

	mbs = mbs_open_file(mbsFile, "F", 0x4000, NULL);			/* open file (ext = .lmd or .med) */

	allocRingBuffer(mbs);										/* allocate ring buffer */

	mbs->buf_to_be_dumped = bufferDump;

	if (mbs != NULL) {
		nofErrors = 0;
		nofEvents = 0;
		while (1) {												/* loop over events */
			eventType = mbs_next_event(mbs);					/* next event */
			nofEvents++;
			if (eventType == MBS_ETYPE_EOF) {					/* end of file? */
				fprintf(stderr, "*EOFFND-[%s]- End of file %s\n", prgName, mbsFile);
				mbs_close_file(mbs);
				break;
			} else if (eventType == MBS_ETYPE_ERROR) {			/* error? */
				nofErrors++;
				abortOnError++;
				if (abortOnError > kMaxErrors) {
					fprintf(stderr, "?ABORT-[%s]- Aborting after %d subsequent errors\n", prgName, abortOnError);
					eventType = MBS_ETYPE_ABORT;
         			break;
				}
			} else if (eventType == MBS_ETYPE_ABORT) {			/* abort? */
				nofErrors++;
				fprintf(stderr, "?ABORT-[%s]- Aborting\n", prgName);
				break;
			}

			if (absEnd > 0) {
				if (nofEvents < absStart + 1) continue;
				else if (nofEvents > absEnd + 1) break;
			} else if (mbsEnd > 0) {
				if (mbs->evtno_mbs < mbsStart) continue;
				else if (mbs->evtno_mbs > mbsEnd) break;
			}

			putRB(mbs);

			if (eventType == MBS_ETYPE_START) {
				nofEventsProcessed++;
				if (processEvent(mbs) != kTRUE) {				/* process event */
					nofErrors++;
					eventType = MBS_ETYPE_ERROR;
					break;
				}
				abortOnError = 0;
			} else if (eventType == MBS_ETYPE_STOP) {
				nofEventsProcessed++;
				if (processEvent(mbs) != kTRUE) {				/* process event */
					nofErrors++;
					eventType = MBS_ETYPE_ERROR;
					break;
				}
				abortOnError = 0;
				mbs_close_file(mbs);
				break;
			} else {
				nofEventsProcessed++;
				if (processEvent(mbs) != kTRUE) {				/* process event */
					nofErrors++;
					eventType = MBS_ETYPE_ERROR;
					break;
				}
				abortOnError = 0;
			}
		}
		if (absEnd > 0) {
			fprintf(stderr, "*EVTPRC-[%s]- File %s: processed %d events (#%d ... #%d, %d error(s))\n",
									prgName, mbsFile, nofEventsProcessed, absStart, absEnd, nofErrors);
		} else {
			fprintf(stderr, "*EVTPRC-[%s]- File %s: processed %d events (#%d ... #%d, %d error(s))\n",
									prgName, mbsFile, nofEventsProcessed, mbsStart, mbsEnd, nofErrors);
		}

		if (nofEvts > 0) {
			fprintf(stderr, "*DMPBUF-[%s]- Dumping %d last event(s)\n", prgName, rbCnt);
			dumpRB();
		}
	}
}

int readSevtDump(char * dmpFile, char * rcFile, char * dumpMode) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   readSevtDump
// Purpose: 	   Read data from subevent dump
// Arguments:	   char * dmpFile   -- file name (.dmp)
//  			   char * rcFile 	-- indices and defs (opt.)
//  			   char * dumpMode  -- subevent type (dgf, caen, or madc)
// Results: 	   kTRUE/kFALSE
// Description:    Opens sevt dump file and processes data directly according to dump mode/type.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	FILE * input;
	int fno;
	MBSDataIO * mbs;
	int fsize;
	struct stat sbuf;
	char * p;

	if ((input = fopen(dmpFile, "r")) == NULL) {
		fprintf(stderr, "?SYSERR-[readSevtDump]- %s: %s (%d)", dmpFile, strerror(errno), errno);
		return(kFALSE);
	}
	fno = fileno(input);
	if (stat(dmpFile, &sbuf) != 0) {
		fprintf(stderr, "?SYSERR-[readSevtDump]- %s: %s (%d)", dmpFile, strerror(errno), errno);
		return(kFALSE);
	}
	fsize = sbuf.st_size;
	mbs = calloc(1, sizeof(MBSDataIO));
	if (mbs == NULL)
	{
		fprintf(stderr, "?ALLOC-[readSevtDump]- %s: Can't allocate MBS data structure", dmpFile);
		return(kFALSE);
	}
	mbs->input = input;
	mbs->fileno = fno;
	mbs->sevt_data = calloc(1, fsize);
	if (mbs->sevt_data == NULL)
	{
		fprintf(stderr, "?ALLOC-[readSevtDump]- %s: Can't allocate space for subevent data", dmpFile);
		return(kFALSE);
	}
	mbs->sevt_wc = read(fno, mbs->sevt_data, fsize);
	fclose(input);

	if (strcmp(dumpMode, "dgf") == 0) {
		processSubevent_dgf(mbs);
	} else if (strcmp(dumpMode, "caen") == 0){
		processSubevent_caen(mbs);
	} else if (strcmp(dumpMode, "madc") == 0){
		processSubevent_madc(mbs);
	} else {
		fprintf(stderr, "?ILLMOD-[readSevtDump]- %s: Illegal dump mode / subevent type - %s", dmpFile, dumpMode);
		return(kFALSE);
	}
}

int processEvent(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processEvent
// Purpose: 	   Process mbs event
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Processes a mbs event depending on trigger number.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;
	int sts;
	int trig;
	int indent;

	unsigned int evtType, evtSubtype;

	evtSubtype = mbs->evttype->type;
	evtType = evtSubtype & 0xFFFF;
	evtSubtype >>= 16;

	trig = mbs_get_event_trigger(mbs);

	printf(evtFmt, "MBS", "EVT", evtType, evtSubtype, trig, mbs->evtno, mbs->evtno_mbs);
	indent = 24;
	if (verboseMode == kTRUE) {
		unsigned short * ep = (unsigned short *) mbs->evt_data;
		printf("%24s # %04x %04x %04x %04x %04x %04x %04x %04x ", "",
				*(ep + 1), *ep, *(ep + 2), *(ep + 3), *(ep + 4), *(ep + 5), *(ep + 7), *(ep + 6));
		indent = 0;
	}
	if (readoutTrigger[trig] == kTRUE) {
		printf("%*s # readout (trigger #%d)", indent, "", trig);
		printf("\n");
		sts = extractSubevents(mbs);		/* extract subevents */
		if (sts == kFALSE) return(kFALSE);
		return(kTRUE);
	} else if (trig == 14) {
		printf("%*s # start acquisition (trigger #14)", indent, "");
		printf("\n");
		return(kTRUE);
	} else if (trig == 15) {
		printf("%*s # stop acquisition (trigger #15)", indent, "");
		printf("\n");
		return(kTRUE);
	} else {
		printf("?ILLTRG-[%s]- Illegal event trigger - %d\n", prgName, trig);
		return(kTRUE);
	}
}

int extractSubevents(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   extractSubevents
// Purpose: 	   Extract subevents from event.
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes subevent by subevent. Dispatches over subevent type.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	unsigned int sevtType, sevtSubtype;
	int sevtSerial;
	char envStr[kStrlen];
	char sevtStr[kStrlen];
	int indent;

	for (;;) {
		sevtType = mbs_next_sheader(mbs);
		if (sevtType == MBS_STYPE_EOE) return(kTRUE);		/* end of event */
		if (sevtType == MBS_STYPE_ABORT) return(kFALSE);	/* error */

		sevtSubtype = sevtType;
		sevtType = sevtSubtype & 0xFFFF;
		sevtSubtype >>= 16;
		sevtSerial = mbs_get_sevent_serial(mbs);
		printf(sevtFmt, "MBS", "SEVT", sevtType, sevtSubtype, sevtSerial);
		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Subevent.%d.Name", sevtSerial - 1);
			strcpy(sevtStr, root_env_getval_s(envStr, mbs->sevttype->descr));
			printf("%40s # subevent \"%s\"", "", strip(sevtStr));
		}
		printf("\n");

		sevtType = mbs_next_sdata(mbs); 					/* next subevent */
		if (sevtType == MBS_STYPE_ERROR) return(kFALSE);	/* error */

		if (sevtType == MBS_STYPE_CAMAC_DGF_1
		||  sevtType == MBS_STYPE_CAMAC_DGF_2
		||  sevtType == MBS_STYPE_CAMAC_DGF_3 ) {
			processSubevent_dgf(mbs);
		} else if (sevtType == MBS_STYPE_VME_CAEN_1
		||  sevtType == MBS_STYPE_VME_CAEN_2
		||  sevtType == MBS_STYPE_VME_CAEN_3 ) {
			processSubevent_caen(mbs);
		} else if (sevtType == MBS_STYPE_CAMAC_1) {
			processSubevent_sevt_10_1x(mbs);
		} else if (sevtType == MBS_STYPE_CAMAC_WO_ID_1) {
			processSubevent_sevt_10_1x(mbs);
		} else if (sevtType == MBS_STYPE_VME_SIS_3) {
			processSubevent_sis(mbs);
		} else if (sevtType == MBS_STYPE_VME_SIS_33) {
			processSubevent_sis_33xx(mbs);
		} else if (sevtType == MBS_STYPE_VME_MADC_1
		||  sevtType == MBS_STYPE_VME_MADC_2
		||  sevtType == MBS_STYPE_VME_MADC_3) {
			processSubevent_madc(mbs);
		}
	}
}

int processSubevent_dgf(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processSubevent_dgf
// Purpose: 	   Process DGF buffers
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes a dgf-4c buffer.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;
	char envStr[kStrlen];
	char modStr[kStrlen];
	char moduleName[kStrlen];
	char lofChannels[kStrlen];
	char * pChannels[kNofChannels];

	char * dataPtr = (char *) mbs->sevt_data;
	int sevtWC = mbs->sevt_wc;
	int origSevtWC = sevtWC;

	int wordsProcessed = 0;
	while (sevtWC) {
		DgfEventBufferHeader * bufHdr = (DgfEventBufferHeader *) dataPtr;
		int bufWC = bufHdr->nofWords; 					/* buffer length */
		int moduleNumber = bufHdr->moduleNumber;		/* module number */

		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(moduleName, root_env_getval_s(envStr, "??"));
			strip(moduleName);
			moduleName[0] = toupper(moduleName[0]);
			sprintf(envStr, "*.Module.%s.LofChannels", moduleName);
			decode_envstr(envStr, lofChannels, pChannels, kNofChannels);
		}

		unsigned int runTask = bufHdr->formatDescr;				/* check buffer format */
		unsigned int listMode = runTask & 0x300;					/* list mode descriptor */

		unsigned long long time = calcTime48(bufHdr->startTime[0], bufHdr->startTime[1], bufHdr->startTime[2]);
		printf(dgfBufFmt, "DGF", "BUF", bufWC, moduleNumber, runTask & 0xFFF,
						bufHdr->startTime[0], bufHdr->startTime[1], bufHdr->startTime[2], time);
		if (verboseMode == kTRUE) {
			printf(" # %04x %04x %04x %04x %04x %04x",
						bufHdr->nofWords,
						bufHdr->moduleNumber,
						bufHdr->formatDescr,
						bufHdr->startTime[0], bufHdr->startTime[1], bufHdr->startTime[2]);
		}
		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(modStr, root_env_getval_s(envStr, "??"));
			printf(" # module \"%s\"", strip(modStr));
		}
		printf("\n");
		if (listMode != 0x100 && listMode != 0x200) {		/* normal or fast list mode */
			printf("?-ILLFMT-[processSubevent_dgf]- Module %d: Illegal format descriptor - %#lx\n", moduleNumber, runTask);
			nofErrors++;
			if (dgfFormat != 0) {
				printf("?                                         [Using format %#lx instead]\n", dgfFormat);
				runTask = dgfFormat;
			} else {
				return(kTRUE);
			}
		}  				 /* unable to decode data */
		runTask &= 0x3; 							/* 0x100, 0x101, 0x200, 0x201 --> variable channel size */
		int nofWordsPerChn;
		if (runTask == 2) nofWordsPerChn = 4;		/* 0x102, 0x202 --> 4 words per chn	 */
		else if (runTask == 3) nofWordsPerChn = 2;	/* 0x103, 0x203 --> 2 words per chn */
		int wc = bufWC;
		if (bufWC <= 0) {
			printf("?-WRGBWC-[processSubevent_dgf]- Module %d: Wrong buffer WC - %d\n", moduleNumber, bufWC);
			nofErrors++;
			return(kTRUE);
		}  				 /* reject neg buffer wcs */
		if (wc >= (sizeof(DgfEventBufferHeader) + sizeof(DgfEventHeader)) / sizeof(short)) {  /* ignore header fragments */
			dataPtr += sizeof(DgfEventBufferHeader);	 /* points to start of 1st event */
			wc -= sizeof(DgfEventBufferHeader) / sizeof(short);
			while (wc) {								 /* dgf buffer may contain several events */
				DgfEventHeader * evtHdr = (DgfEventHeader *) dataPtr;	 /* points to event header */
				unsigned int hitPattern = evtHdr->hitPattern;		 /* get hit pattern */

				time = calcTime48(bufHdr->startTime[0], evtHdr->eventTime[0],  evtHdr->eventTime[1]);
				printf(dgfEvtFmt, "DGF", "EVT", hitPattern, evtHdr->eventTime[0], evtHdr->eventTime[1], time);
				if (verboseMode == kTRUE) printf(" # %04x %04x %04x", evtHdr->hitPattern, evtHdr->eventTime[0], evtHdr->eventTime[1]);
				printf("\n");

				if (hitPattern & 0xFFFFFFF0) {
					printf("?-WRGHIT-[processSubevent_dgf]- Module %d: Wrong hit pattern - %#lx\n", moduleNumber, hitPattern);
					nofErrors++;
					return(kTRUE);
				}
				dataPtr += sizeof(DgfEventHeader);		/* points to start of 1st channel */
				wc -= sizeof(DgfEventHeader) / sizeof(short);
				int chn;
				int nofChnData;
				for (chn = 0; chn < kDgfNofChannels; chn++) {   /* up to 4 channels */
					if (hitPattern == 0) break;		 /* no more hits in this event */
					if (hitPattern & 1) {  			 /* channel fired? */
						if (runTask <= 1) {
							nofWordsPerChn = *((unsigned short *) dataPtr);
							nofChnData = nofWordsPerChn;
							dataPtr += sizeof(unsigned short);
							if (nofWordsPerChn <= 0) {
								printf("?-WRGCWC-[processSubevent_dgf]- Module %d: Wrong channel WC - %d\n", moduleNumber, nofWordsPerChn);
								return(kTRUE);
							}
							nofWordsPerChn--;
							wc--;
						}
						int hdrwc, trcwc;
						if (nofWordsPerChn > kDgfMaxNofChannelWords - 1) {
							hdrwc = kDgfMaxNofChannelWords - 1;
							trcwc = nofWordsPerChn - kDgfMaxNofChannelWords + 1;
						} else {
							hdrwc = nofWordsPerChn;
							trcwc = 0;
						}

						DgfEventChannelHeader * chnHdr = (DgfEventChannelHeader *) dataPtr;

						time = calcTime48(bufHdr->startTime[0], evtHdr->eventTime[0],   chnHdr->fastTriggerTime);
						printf(dgfChnFmt, "DGF", "CHN", chn, nofChnData, chnHdr->energy, chnHdr->fastTriggerTime, time);
						if (verboseMode == kTRUE) {
							int i;
							unsigned short *dp = (unsigned short *) dataPtr;
							printf(" #");
							if (runTask <= 1) printf(" %04x", nofChnData);
							for (i = 0; i < hdrwc; i++) printf(" %04x", *dp++);
						}
						if (hasRcFile == kTRUE) printf(" # channel \"%s\"", pChannels[chn]);
						printf("\n");

						if (trcwc > 0) {
							printf(dgfTraceFmt, "DGF", "TRACE", chn, trcwc);
							printf("\n");
							if (verboseMode == kTRUE) {
								int i;
								unsigned short *dp = (unsigned short *) dataPtr + hdrwc;
								printf("DGF   TDATA  ", "");
								for (i = 0; i < trcwc; i++) {
									printf("%7d", *dp++);
									if ((i % 5) == 0) printf("\nDGF   TDATA  ", "");
								}
								printf("\n");
							}
						}

						dataPtr += nofWordsPerChn * sizeof(short);
						wc -= nofWordsPerChn;		 /* continue with next channel */
					}
					hitPattern >>= 1;
				}
			}
		} else  dataPtr += bufWC * sizeof(short);	 /* skip over buffer fragment */
		sevtWC -= bufWC;								 /* remaining subevent data */
		wordsProcessed += bufWC;
		if (bufWC & 1) {								 /* dgf buffers are aligned to 32 bit (even number of shorts) */
			dataPtr += sizeof(short);				 /* word count is odd -> skip over filler */
			sevtWC--;
			wordsProcessed++;
		}
	}
	return(kTRUE);
}

int processSubevent_caen(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processSubevent_caen
// Purpose: 	   Process CAEN buffers
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes a CAEN adc/tdc buffer.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	char envStr[kStrlen];
	char modStr[kStrlen];
	char moduleName[kStrlen];
	char lofChannels[kStrlen];
	char * pChannels[kNofChannels];

	unsigned short * dataPtr = (char *) mbs->sevt_data;
	int sevtWC = mbs->sevt_wc;
	int origSevtWC = sevtWC;

	int wordsProcessed = 0;
	int nofErrors = 0;
	while (sevtWC > 0) {									/* subevent may contain several CAEN modules */
		unsigned int header = (*dataPtr++) << 16; 						/* header word (MSB) */
		header |= *dataPtr++; 								/* header word (LSB) */
		if ((header & CAEN_V7X5_D_HDR) == 0) {
			printf("?-WRGHDR-[processSubevent_caen]- Wrong header - %#lx\n", header);
			nofErrors++;
			return(kTRUE);
		}
		int wc = (header >> CAEN_V7X5_SH_WC) & CAEN_V7X5_M_WC;				/* extract wc */
		int wcs = wc;
		if (wc < 0 || wc > 32) {				/* max 32 data words */
			printf("?-WRGHWC-[processSubevent_caen]- Wrong header word count - %d\n", wc);
			fprintf(stderr, "?-WRGHWC-[processSubevent_caen]- Wrong header word count - %d\n", wc);
		}
		int moduleNumber = header & CAEN_V7X5_M_MSERIAL;	/* extract module number */
		if (moduleNumber <= 0) {
			printf("?-WRGMOD-[processSubevent_caen]- Wrong module number - %d\n", moduleNumber);
			nofErrors++;
		}

		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(moduleName, root_env_getval_s(envStr, "??"));
			strip(moduleName);
			moduleName[0] = toupper(moduleName[0]);
			sprintf(envStr, "*.Module.%s.LofChannels", moduleName);
			decode_envstr(envStr, lofChannels, pChannels, kNofChannels);
		}

		printf(caenHdrFmt, "CAEN", "HDR", wc, moduleNumber);
		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(modStr, root_env_getval_s(envStr, "??"));
		}
		if (verboseMode == kTRUE) {
			printf("%48s # %04x %04x", "", (header >> 16) & 0xFFFF, header & 0xFFFF);
			if (hasRcFile == kTRUE) printf(" # module \"%s\"", strip(modStr));
		} else if (hasRcFile == kTRUE) {
			printf("%48s # module \"%s\"", "", strip(modStr));
		}
		printf("\n");

		for (; wc; wc--) {
			unsigned int data = (*dataPtr++) << 16;		/* data (MSB) */
			data |= *dataPtr++; 			/* data (LSB) */
			if ((data & CAEN_V7X5_M_ID) != CAEN_V7X5_D_DATA) {
				if ((data & CAEN_V7X5_M_ID) == CAEN_V7X5_D_EOB) {
					printf("?-UNXEOB-[processSubevent_caen]- Unexpected EOB - remaining wc=%d ignored. Continuing with next event.\n", wc);
					nofErrors++;
					dataPtr -= 2;
					wcs -= wc;
					break;
					if (moduleNumber <= 0) {
			printf("?-WRGMOD-[processSubevent_caen]- Wrong module number - %d\n", moduleNumber);
			nofErrors++;
		}
	} else {
					printf("?-WRGDAT-[processSubevent_caen]- Wrong data - %#x\n", data);
					nofErrors++;
					return(kTRUE);
				}
			}
			int chn = (data >> CAEN_V7X5_SH_CHN) & CAEN_V7X5_M_CHN;
			if (moduleNumber > 0) {
				char * overUnder = "";
				unsigned int evtData;
				if (data & CAEN_V7X5_B_OVERFLOW) {
					evtData = CAEN_V7X5_B_OVERFLOW;
					overUnder = "Overflow";
				} else if (data & CAEN_V7X5_B_UNDERTHRESH) {
					evtData = (unsigned int) -1;
					overUnder = "Underflow";
				} else {
					evtData = (unsigned int) (data & CAEN_V7X5_M_ADCDATA);
				}
				printf(caenChnFmt, "CAEN", "CHN", chn, evtData);
				if (verboseMode == kTRUE) {
					printf("%48s # %04x %04x", "", (data >> 16) & 0xFFFF, data & 0xFFFF);
					if (*overUnder != '\0') printf(" # %s", overUnder);
				} else if (*overUnder != '\0') {
					printf("%48s # %s", "", overUnder);
				}
				if (hasRcFile == kTRUE) printf(" # channel \"%s\"", pChannels[chn]);
				printf("\n");
			}
		}
		int trailer = (*dataPtr++) << 16; 					/* trailer word (MSB) */
		trailer |= *dataPtr++; 									/* trailer word (LSB) */
		printf(caenEoeFmt, "CAEN", "EOE", trailer & 0xFFFFFF);
		if (verboseMode == kTRUE) {
			printf("%56s # %04x %04x", "", (trailer >> 16) & 0xFFFF, trailer & 0xFFFF);
		}
		printf("\n");
		if ((trailer & CAEN_V7X5_D_EOB) == 0) {
			printf("?-WRGEOB-[processSubevent_caen]- Wrong data - %#lx\n", trailer);
			nofErrors++;
			return(kTRUE);
		}
		wcs = (wcs + 2) * 2;
		sevtWC -= wcs;						/* remaining subevent data */
		wordsProcessed += wcs;
	}
	return(kTRUE);
}

int processSubevent_madc(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processSubevent_madc
// Purpose: 	   Process MADC32 buffers
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes a Mesytec MADC32 buffer.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;
	unsigned short * dataPtr;
	int sevtWC;
	int origSevtWC;
	int wordsProcessed;
	int header;
	int moduleNumber;
	int res;
	int wc, wcs;
	unsigned short * trailPtr;
	unsigned int highTimeStamp;
	unsigned int lowTimeStamp;
	unsigned int trailer;
	int xts;
	unsigned long long ts64;
	unsigned int data, data16;
	int chn;
	char envStr[kStrlen];
	char modStr[kStrlen];

	dataPtr = (char *) mbs->sevt_data;
	sevtWC = mbs->sevt_wc;
	origSevtWC = sevtWC;

	while (sevtWC > 0) {									/* subevent may contain several CAEN modules */
		header = (*dataPtr++ << 16);
		header |= *dataPtr++;
		if ((header & MADC_M_ID) != MADC_D_HDR) {
			printf("?-WRGHDR-[processSubevent_madc]- Wrong header - %#lx\n", header);
			nofErrors++;
			return(kTRUE);
		}

		moduleNumber = (header >> MADC_SH_SERIAL) & MADC_M_SERIAL;		/* extract module id */
		if (moduleNumber <= 0) {
			printf("?-WRGMOD-[processSubevent_madc]- Wrong module number - %d\n", moduleNumber);
			nofErrors++;
		}

		res = (header >> MADC_SH_RES) & MADC_M_RES;						/* extract adc resolution */

		wc = header & MADC_M_WC;							/* extract wc */
		wcs = (wc + 1) * 2; 								/* wc + header */

		printf(madcHdrFmt, "MADC", "HDR", moduleNumber, res, wc);
		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(modStr, root_env_getval_s(envStr, "??"));
		}
		if (verboseMode == kTRUE) {
			printf("%48s # %04x %04x", "", (header >> 16) & 0xFFFF, header & 0xFFFF);
			if (hasRcFile == kTRUE) printf(" # module \"%s\"", strip(modStr));
		} else if (hasRcFile == kTRUE) {
			printf("%48s # module \"%s\"", "", strip(modStr));
		}
		printf("\n");

		trailPtr = dataPtr + 2 * (wc - 2);

		highTimeStamp = (*trailPtr++ << 16);		/* possibly extended timestam */
		highTimeStamp |= *trailPtr++;

		trailer = (*trailPtr++ << 16); 	/* inspect trailer word */
		trailer |= *trailPtr++;

		if ((trailer & MADC_M_ID) != MADC_D_EOE) {
			printf("?-WRGHDR-[processSubevent_madc]- Wrong EOE - %#lx\n", trailer);
			nofErrors++;
			return(kTRUE);
		}
		wc--;								/* - trailer word */

		xts = kFALSE;
		ts64 = 0;

		if ((highTimeStamp & MADC_M_XTS) == MADC_D_XTS) {
			ts64 = ((unsigned long long) (highTimeStamp & MADC_M_TSHIGH)) << 30;
			wc--;							/* - extended ts */
			xts = kTRUE;
		}

		lowTimeStamp = trailer & MADC_M_TSLOW;	/* extract lower bits of time stamp (bits 0-29) */
		ts64 |= lowTimeStamp;

		for (; wc; wc--) {
			data = (*dataPtr++ << 16);
			data |= *dataPtr++;
			if ((data & MADC_M_ID) != MADC_D_DATA) {
				if ((data & MADC_M_XTS) == MADC_D_XTS) continue;
				if ((data & MADC_M_ID) == MADC_D_EOE) {
					printf("?-UNXEOB-[processSubevent_madc]- Unexpected EOB - remaining wc=%d ignored. Continuing with next event.\n", wc);
					nofErrors++;
					dataPtr -= 2;
					wcs -= wc * 2;
					nofErrors++;
					break;
				} else {
					printf("?-WRGDAT-[processSubevent_madc]- Wrong data - %#lx.\n", data);
					nofErrors++;
					break;
				}
			}

			chn = (data >> MADC_SH_CHN) & MADC_M_CHN;
			data16 = data & 0xFFFF;
			printf(madcChnFmt, "MADC", "CHN", chn, data16);
			if (verboseMode == kTRUE) {
				printf("%48s # %04x %04x", "", (data >> 16) & 0xFFFF, data & 0xFFFF);
			}
			printf("\n");
		}

		if (xts) {
			dataPtr += 2;				/* compensate for extended ts */
			dataPtr += 2;						/* and for trailer */
			printf(madcXtsFmt, "MADC", "XTS", (highTimeStamp & MADC_M_TSHIGH));
			if (verboseMode == kTRUE) {
				printf("%48s # %04x", "", (highTimeStamp & MADC_M_TSHIGH));
			}
			printf("\n");
		}

		if (xts) {
			printf(madcEoe2Fmt, "MADC", "EOE", trailer & MADC_M_TSLOW, ts64);

			if (verboseMode == kTRUE) {
				printf("%32s # %04x %04x", "", (trailer >> 16) & 0xFFFF, trailer & 0xFFFF);
			}
		} else {
			printf(madcEoe1Fmt, "MADC", "EOE", trailer & MADC_M_TSLOW);

			if (verboseMode == kTRUE) {
				printf("%48s # %04x %04x", "", (trailer >> 16) & 0xFFFF, trailer & 0xFFFF);
			}
		}
		printf("\n");

		sevtWC -= wcs;						/* remaining subevent data */
	}
	return(kTRUE);
}

int processSubevent_sevt_10_1x(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		for (int i = 0; i < nofModules; i++) {
		   processSubevent_sevt_10_1x
// Purpose: 	   Process subevents [10,1] and [10,11]
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes [10,1] and [10,11] buffers
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i, j, k, m, n;

	char envStr[kStrlen];
	char sevtName[kStrlen];
	char moduleName[kStrlen];
	char lofModules[kStrlen];
	char * pModules[kNofModules];
	char lofModes[kStrlen];
	char * pModes[kNofModes];
	int nofShorts[kNofModules];
	int nofChannels[kNofModules];
	int nofDataBits[kNofModules];
	unsigned int dataMask[kNofModules];
	char chnName[kStrlen];
	char lofChannels[kStrlen];
	char * pChannels[kNofChannels];

	int sevtId = mbs_get_sevent_serial(mbs);
	int sevtWC = mbs->sevt_wc;
	unsigned short * dataPtr = (unsigned short *) mbs->sevt_data;

	int nofModules = 0;
	int totShorts = 0;
	int totChannels = 0;
	if (hasRcFile == kTRUE) {
		sprintf(envStr, "*.Subevent.%d.Name", sevtId - 1);
		strcpy(sevtName, root_env_getval_s(envStr, "??"));
		strip(sevtName);
		sevtName[0] = toupper(sevtName[0]);
		sprintf(envStr, "*.Subevent.%s.LofModules", sevtName);
		int nm = decode_envstr(envStr, lofModules, pModules, kNofModules);
		sprintf(envStr, "*.Subevent.%s.NofModules", sevtName);
		nofModules = root_env_getval_i(envStr, 0);
		if (nofModules != nm) {
			printf("?-WRGMOD-[processSubevent_sevt_10_11]- Wrong number of modules - %d (list contains %d names)\n", nofModules, nm);
			nofErrors++;
		}
		for (i = 0; i < nofModules; i++) {
			strcpy(moduleName, pModules[i]);
			moduleName[0] = toupper(moduleName[0]);
			sprintf(envStr, "*.Module.%s.NofChannelsUsed", moduleName);
			nofChannels[i] = root_env_getval_i(envStr, 0);
			totChannels += nofChannels[i];
			sprintf(envStr, "*.Module.%s.NofShortsPerDatum", moduleName);
			nofShorts[i] = root_env_getval_i(envStr, 0);
			totShorts += nofShorts[i] * nofChannels[i];
			sprintf(envStr, "*.Module.%s.NofDataBits", moduleName);
			nofDataBits[i] = root_env_getval_i(envStr, 0);
			dataMask[i] = (1 << nofDataBits[i]) - 1;
			if (totShorts & 1) totShorts++;
		}
		if (totShorts != sevtWC) {
			printf("?-WRGWCT-[processSubevent_sevt_10_1x]- Wrong word count - %d (should be %d)\n", sevtWC, totShorts);
			nofErrors++;
		}
		sevtName[0] = tolower(sevtName[0]);

		printf(s10xxHdrFmt, "SEVT", "WC", sevtWC);
		if (verboseMode == kTRUE) {
			printf("%56s # %04x", "", sevtWC);
			printf(" # %d modules(s): ", nofModules);
			for (i = 0; i < nofModules; i++) printf("%s ", pModules[i]);
		}
		printf("\n");

		for (i = 0; i < nofModules; i++) {
			strcpy(moduleName, pModules[i]);
			strip(moduleName);
			for (j = 0; j < kNofChannels; j++) pChannels[j] = "??";
			moduleName[0] = toupper(moduleName[0]);
			sprintf(envStr, "*.Module.%s.LofChannels", moduleName);
			decode_envstr(envStr, lofChannels, pChannels, kNofChannels);
			if (nofShorts[i] == 1) {
				unsigned int data;
				for (j = 0; j < nofChannels[i]; j++) {
					data = *dataPtr++;
					printf(s10xxData16Fmt, "SEVT", "CHN", j, data);
					if (verboseMode == kTRUE) printf("%48s # %04x", "", data);
					printf(" # module \"%s\", channel \"%s\"",  pModules[i], pChannels[j]);
					printf("\n");
				}
			} else if (nofShorts[i] == 2) {
				unsigned int data, data1, data2;
				for (j = 0; j < nofChannels[i]; j++) {
					data1 = *dataPtr++;
					data2 = *dataPtr++;
					data = (data1 << 16) | (data2 & 0xFFFF);
					printf(s10xxData32Fmt, "SEVT", "CHN", j, data);
					if (verboseMode == kTRUE) printf("%48s # %04x %04x", "", data1, data2);
					printf(" # module \"%s\", channel \"%s\"",  pModules[i], pChannels[j]);
					printf("\n");
				}
			} else {
				printf("?-WRGDTW-[processSubevent_sevt_10_11]- Wrong data width - %d * 16 (32 bits max)\n", nofShorts[i]);
				nofErrors++;
			}
		}
	} else {
		unsigned int data;
		for (j = 0; j < sevtWC; j++) {
			data = *dataPtr++;
			printf(s10xxData16Fmt, "SEVT", "CHN", j, data);
			if (verboseMode == kTRUE) printf("%48s # %04x", "", data);
		}
	}
	return(kTRUE);
}

int processSubevent_sis(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processSubevent_sis
// Purpose: 	   Process sis data
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes data of sis modules (subevent SIS_3 [10,53])
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int wordsProcessed, eventsProcessed;
	unsigned short * dataPtr;
	int sevtWC;
	unsigned short header;
	int wc, wcs;
	int moduleNumber;
	int sis3801;
	int chn, data, dw;
	char envStr[kStrlen];
	char moduleName[kStrlen];
	int sisID, moduleID;

	wordsProcessed = 0;
	dataPtr = (unsigned short *) mbs->sevt_data;
	eventsProcessed = 0;
	sevtWC = mbs->sevt_wc;
	while (sevtWC > 0) {
		header = *dataPtr++;
		if ((header & kMrbSevt_Sis_B_Header) == 0) {
			printf("?-WRGHDR-[processSubevent_sis]- Wrong header - 0x%x\n", header);
			nofErrors++;
			return(kTRUE);
		}
		wc = *dataPtr++;
		wcs = wc;
		if (wc <= 0) {
			printf("?-WRGWCT-[processSubevent_sis]- Wrong word count - %d\n", wc);
			nofErrors++;
			return(kTRUE);
		}
		moduleNumber = header & kMrbSevt_Sis_M_ModuleNumber;
		if (moduleNumber <= 0) {
			printf("?-WRGWCT-[processSubevent_sis]- Wrong module number - %d\n", moduleNumber);
			nofErrors++;
			return(kTRUE);
		}

		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(moduleName, root_env_getval_s(envStr, ""));
		}

		moduleID = (header >> kMrbSevt_Sis_SH_ModuleId) & kMrbSevt_Sis_M_ModuleId;
		sis3801 = (moduleID == (kMrbModIdSis_3801 & kMrbSevt_Sis_M_ModuleId));
		if (sis3801) sisID = 3801; else sisID = 3600;

		printf(sisHdrFmt, "SIS", "HDR", moduleID, wcs, moduleNumber);
		if (verboseMode == kTRUE) {
			printf("%40s # %08x", "", header);
			if (hasRcFile == kTRUE) printf(" # module \"%s\" id=%d", strip(moduleName), sisID);
		} else if (hasRcFile == kTRUE) {
			printf("%40s # module \"%s\" id=%d", "", strip(moduleName), sisID);
		}
		printf("\n");

		wc -= 2;
		for (; wc; wc-=2) {
			data = (*dataPtr++) << 16;
			data |= *dataPtr++;
			if (sis3801) chn = (data >> 24) & 0xFF; else chn = 0;
			dw = data & 0xFFFFFF;
			printf(sisDataFmt, "SIS", "DATA", chn, dw);
			if (verboseMode == kTRUE) printf("%48s # %08x", "", data);
			printf("\n");
			eventsProcessed++;
		}
		sevtWC -= wcs;
		wordsProcessed += wcs;
	}
	return(kTRUE);
}

int processSubevent_sis_33xx(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   processSubevent_sis_33xx
// Purpose: 	   Process sis data
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   kTRUE/kFALSE
// Description:    Decodes data of sis 33xx modules (subevent SIS_33 [10,54])
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int wordsProcessed, eventsProcessed;
	unsigned int * dataPtr;
	unsigned int * dataPsav;
	int sevtWC;
	unsigned int header;
	int wc;
	int moduleNumber;
	int chn, data, dw;
	char envStr[kStrlen];
	char moduleName[kStrlen];
	int moduleID;
	int acqMode, evtCount, pageSize;
	unsigned int modStatus, acqStatus;

	wordsProcessed = 0;
	dataPtr = (unsigned int *) mbs->sevt_data;
	eventsProcessed = 0;
	sevtWC = mbs->sevt_wc;
	while (sevtWC > 0) {
		dataPsav = dataPtr;
		header = *dataPtr++;
		if ((header & kMrbSevt_Sis_B_Header) == 0) {
			printf("?-WRGHDR-[processSubevent_sis_33xx]- Wrong header - 0x%x\n", header);
			nofErrors++;
			return(kTRUE);
		}
		wc = *dataPtr++;
		if (wc <= 0) {
			printf("?-WRGWCT-[processSubevent_sis_33xx]- Wrong word count - %d\n", wc);
			nofErrors++;
			return(kTRUE);
		}
		moduleNumber = header & kMrbSevt_Sis_M_ModuleNumber;
		if (moduleNumber <= 0) {
			printf("?-WRGWCT-[processSubevent_sis_33xx]- Wrong module number - %d\n", moduleNumber);
			nofErrors++;
			return(kTRUE);
		}

		if (hasRcFile == kTRUE) {
			sprintf(envStr, "*.Module.%d.Name", moduleNumber - 1);
			strcpy(moduleName, root_env_getval_s(envStr, "??"));
		}

		moduleID = (header >> kMrbSevt_Sis_SH_ModuleId) & kMrbSevt_Sis_M_ModuleId;
		if (moduleID != (kMrbModIdSis_3300 & kMrbSevt_Sis_M_ModuleId)) {
			printf("?-WRGWCT-[processSubevent_scaler]- Wrong module id - %d (should be %d for SIS3300)\n",
						moduleID, (kMrbModIdSis_3300 & kMrbSevt_Sis_M_ModuleId));
			nofErrors++;
			return(kTRUE);
		}

		printf(sisHdrFmt, "SIS", "HDR", moduleID, wc, moduleNumber);
		if (verboseMode == kTRUE) {
			printf("%40s # %09x %09x", "", header, wc);
			if (hasRcFile == kTRUE) printf(" # module \"%s\" id=3300", strip(moduleName));
		} else if (hasRcFile == kTRUE) {
			printf("%40s # module \"%s\" id=3300", strip(moduleName));
		}
		printf("\n");

		if (wc > 2) {
			acqMode = *dataPtr++;
			evtCount = *dataPtr++;
			pageSize = *dataPtr++;
			modStatus = *dataPtr++;
			acqStatus = *dataPtr++;

			printf(sis33xxInfoFmt, "SIS", "INFO", acqMode, evtCount, pageSize, modStatus, acqStatus);
			printf("\n");
		}
		sevtWC -= wc * (sizeof(unsigned int) / sizeof(unsigned short));	/* sevtWC counts in 16 bit words while wc is 32 bits */
	}
	return(kTRUE);
}

void allocRingBuffer(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   allocRingBuffer
// Purpose: 	   Allocate ring buffer
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   --
// Description:    Allocates <nofEvts> buffers each <mbs->bufsiz> size
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;

	rbGet = 0;
	rbPut = 0;
	rbCnt = 0;
	rbSize = 0;

	if (nofEvts > 0) {
		rbSize = mbs->bufsiz;
		ringBuffer = calloc(nofEvts, rbSize);
		rbPtrs = calloc(nofEvts, sizeof(char *));
		rbEvtno = calloc(nofEvts, sizeof(int));
		rbEvtsiz = calloc(nofEvts, sizeof(int));
		char * rp = ringBuffer;
		for (i = 0; i < nofEvts; i++) {
			rbPtrs[i] = rp;
			rp += rbSize;
		}
		rbPut = nofEvts - 1;
	}
}

char * nextRB(int * curpos) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   nextRB
// Purpose: 	   Return next buffer
// Arguments:	   int * curpos    -- current position in ring
// Results: 	   char * next     -- pointer to next buffer
// Description:    Steps thru ring buffer
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int pos = *curpos;
	pos++;
	if (pos >= nofEvts) pos = 0;
	*curpos = pos;
	return(rbPtrs[pos]);
}

char * getRB() {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   getRB
// Purpose: 	   Get next buffer from ring
// Arguments:	   --
// Results: 	   char * buf     -- pointer to next buffer
// Description:    Returns pointer to next buffer
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int pos;
	char * buf;

	if (rbCnt > 0) {
		pos = rbGet;
		rbCnt--;
		buf = nextRB(&rbGet);
		return(buf);
	} else {
		return(NULL);
	}
}

void putRB(MBSDataIO * mbs) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   putRB
// Purpose: 	   Put a buffer to buffer ring
// Arguments:	   MBSDataIO * mbs  -- ptr to mbs data structure
// Results: 	   --
// Description:    Writes data to ring buffer (and overwrites previous ones)
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	char * buf;

	if (nofEvts > 0) {
		rbCnt++;
		if (rbCnt > nofEvts) rbCnt = nofEvts;
    	buf = nextRB(&rbPut);
		memcpy(buf, mbs->evt_data, mbs->evtsiz);
		rbEvtno[rbPut] = mbs->evtno_mbs;
		rbEvtsiz[rbPut] = mbs->evtsiz;
	}
}

void dumpRB() {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   dumpRB
// Purpose: 	   Dump contents of ring buffer to file
// Arguments:	   --
// Results: 	   --
// Description:    Write ring buffer to file(s)
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	char * buf;
	char fname[100];
	int evtSize;
	FILE * f;

	if (rbCnt > 0) {
		rbGet = 0;
		while (buf = getRB()) {
			sprintf(fname, "rb-%d.dmp", rbEvtno[rbGet]);
			evtSize = rbEvtsiz[rbGet];
			if ((f = fopen(fname, "w")) != NULL) {
				fwrite(buf, evtSize, 1, f);
				fclose(f);
			} else {
				fprintf(stderr, "?OPNERR-[dmpRB]- Can't open file - %s\n", fname);
			}
		}
	}
}

void writeHeader(int argc, char * argv[]) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   writeHeader
// Purpose: 	   Write header in front of ascii data
// Arguments:	   --
// Results: 	   --
// Description:    Outputs some info concerning output formats.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	int i;

	printf("# Program                    : %s\n", prgName);
	printf("# Syntax                     : %s\n", cmdFmt);
	printf("# Arguments                  :");
	for (i = 1; i < argc; i++) printf(" %s", argv[i]);
	printf("\n");
	printf("# Input                      : %s\n", mbsFile);
	if (rcFile[0] == '\0') {
		printf("# Indices & defs             : none\n");
	} else {
		printf("# Indices & defs             : %s\n", rcFile);
	}
	if (isSevtDump == kFALSE) {
		int i;
		printf("# Event trigger(s)           :");
		for (i = 1; i < kNofTriggers; i++) {
			if (readoutTrigger[i] == kTRUE) printf(" %d", i);
		}
		printf("\n");
		if (maxEvents == 0) {
			printf("# Max number of events       : entire file\n");
		} else {
			printf("# Max number of events       : %d\n", maxEvents);
		}
	} else {
		printf("# Subevent dump type         : %s\n", dumpMode);
	}
	if (dgfFormat != 0) printf("# Default format descr (DGF) : %#x\n", dgfFormat);
	if (logFile[0] == '\0') {
		printf("# Log file                   : none\n");
	} else {
		printf("# Log file                   : %s\n", logFile);
	}
	printf("# Buffer dump                : %s\n", (bufferDump == kTRUE) ? "on" : "off");
	printf("# Verbose mode               : %s\n", (verboseMode == kTRUE) ? "on" : "off");

	printf("# Formats:   [MBS:Event]       MBS   EVT <type> <subtype> <trigger> <evtNo> <evtNoMbs> # <comments>\n");
	printf("#            [MBS:Subevent]    MBS  SEVT <type> <subtype> <serial> # <comments>\n");
	printf("#            [DGF-4C:Buffer]   DGF   BUF <wc> <module> <format> <t48> <t32> <t16> <time> # <comments>\n");
	printf("#            [DGF-4C:Event]    DGF   EVT <hitpattern> <t32> <t16> <time> # <comments>\n");
	printf("#            [DGF-4C:Channel]  DGF   CHN <channel> <chwc> <energy> <fastTrigTime> <time> # <comments>\n");
	printf("#            [DGF-4C:Traces]   DGF TRACE <channel> <wc> # <comments>\n");
	printf("#            [DGF-4C:Tdata]    DGF TDATA <data> <data> ... # <comments>\n");
	printf("#            [Caen:Header]     CAEN  HDR <wc> <module> # <comments>\n");
	printf("#            [Caen:Channel]    CAEN  CHN <chn> <data> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", caenChnFmt);
	printf("#            [Caen:EoE]        CAEN  EOE <evtCount> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", caenEoeFmt);
	printf("#            [Sis:Header]      SIS   HDR <id> <wc> <module> # <comments>\n");
	printf("#            [Sis:Data]        SIS  DATA <chn> <data> # <comments>\n");
	printf("#            [Sis33xx:Info]    SIS  INFO <acqMode> <evtCount> <pageSize> <status> <acqStatus> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", sis33xxInfoFmt);
	printf("#            [Madc32:Header]   MADC  HDR <module> <ofmt> <res> <wc> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", madcHdrFmt);
	printf("#            [Madc32:Channel]  MADC  CHN <chn> <data> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", madcChnFmt);
	printf("#            [Madc32:Xts]      MADC  XTS <timeStamp> # <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", madcXtsFmt);
	printf("#            [Madc32:EoE]      MADC  EOE <timeStamp> [<ts64>]# <comments>\n");
	printf("#                              printf(\"%s # %%s\\n\", ...);\n", madcEoe2Fmt);
}

unsigned long long calcTime48(unsigned short t48, unsigned short t32, unsigned short t16) {
/*_________________________________________________________________________________________[C Function]
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Name:		   calcTime48
// Purpose: 	   Calculate 48-bit time from 16-bit parts
// Arguments:	   unsigned short t48      -- time bits 47-32
//                 unsigned short t32      -- time bits 31-16
//                 unsigned short t16      -- time bits 15-0
// Results: 	   unsigned long long time -- 48-bit resulting time
// Description:    Converts 16-bit time slices to 48 bit.
// Keywords:
/////////////////////////////////////////////////////////////////////////////////////////////////////*/

	unsigned long long t;
	t = (unsigned long long) t48;
	t <<= 16;
	t |= (unsigned long long) t32;
	t <<= 16;
	t |= (unsigned long long) t16;
	return(t);
}

void usage() {
	fprintf(stderr, "\nUsage:    ./%s %s\n\n", prgName, cmdFmt);
	fprintf(stderr, "\n          mbsFile            raw data file (extension .lmd or .med)\n");
	fprintf(stderr, "          -r <rcFile>        use indices and defs from <rcFile> (default: no defs)\n");
	fprintf(stderr, "          -n <maxEvents>     process <maxEvents> only (default: end of file)\n");
	fprintf(stderr, "          -n <start:end>     process events from <start> to <end> (absolute event numbers)\n");
	fprintf(stderr, "          -m <start:end>     process events from <start> to <end> (mbs event numbers)\n");
	fprintf(stderr, "          -t <rdoTrig>       readout trigger is <trigger> (default: 1)\n");
	fprintf(stderr, "                             (there may be more than one option \"-t\" in case of multiple triggers)\n");
	fprintf(stderr, "          -f <dgfFmt>        use DGF-4C format descriptor <dgfFmt> in case of format errors\n");
	fprintf(stderr, "          -d <sevtType>      raw data file contains subevent dumps rather than original mbs data (extension .dmp)\n");
	fprintf(stderr, "                             <sevtType> = \"dgf\" or \"caen\" (default: none)\n");
	fprintf(stderr, "          -b                 turn on buffer dump: write buffer data to file in case of errors\n\n");
	fprintf(stderr, "          -s <nbufs>         store <nbufs> last records for backtracing (default: 0)\n");
	fprintf(stderr, "          -l <logFile>       write (error) messages to log file\n");
	fprintf(stderr, "          -v                 turn on verbose mode: output hex dump in addition to other data\n\n");
}

char * strip(char * string) {
	int i;
	char * sp;
	sp = string + strlen(string);
	for (i = 0; i < strlen(string); i++, sp--) {
		if (isspace(*sp)) *sp = '\0';
	}
	return(string);
}

int decode_envstr(char * envStr, char * result, char * pres[], int nofRes) {
	int i, j;
	char head[kStrlen];
	strcpy(result, root_env_getval_s(envStr, ""));
	strip(result);
	for (i = 0; i < nofRes; i++) pres[i] = "??";
	char * sp1 = strchr(result, '[');
	if (sp1) {
		*sp1++ = '\0';
		strcpy(head, result);
		char * sp2 = strchr(sp1, ':');
		if (sp2) {
			*sp2++ = '\0';
			int n1 = atoi(sp1);
			char * sp3 = strchr(sp2, ']');
			if (sp3) {
				*sp3 = '\0';
				int n2 = atoi(sp2);
				*result = '\0';
				for (j = n1; j <= n2; j++) {
					if (j > n1) strcat(result, ":");
					sprintf(result, "%s%s%d", result, head, j);
				}
			}
		}
	}
	char  * rp = result;
	int n = 0;
	char * sp;
	while (sp = strtok(rp, ":")) { pres[n++] = sp; rp = NULL; }
	return(n);
}
