#ifndef __mbsio_h__
#define __mbsio_h__

/*INCLUDEFILE*****************************************************************
NAME
	mbsio/inc/mbsio.h
AUTHOR
	R. Lutter
PURPOSE
	Structs & defs to read MBS data from tape or disk
DATE
	%G%
VERSION
	%I%
TYPEDEFS

KEYWORDS

SEE ALSO

*****************************************************************************/

#include "typedefs.h"
#include "s_bufhe.h"
#include "s_evhe.h"
#include "s_filhe.h"
#include "s_vehe.h"
#include "s_vesfb.h"
#include "s_veshe.h"

/*BITS & BYTES****************************************************************
NAME
	Miscellaneous
AUTHOR
	R. Lutter
PURPOSE
	Definitions, constants
DEFINITION
.	MBS_ID_WORD			-- internal struct id
.	MBS_SIZEOF_DATA		-- length of data (bytes)

							Connect type:
.	MBS_CTYPE_FILE		--		file
.	MBS_CTYPE_SYNC		--		synchronous via transport manager
.	MBS_CTYPE_ASYNC 	--		asynchronous via stream server
.	MBS_CTYPE_REMOTE	--		remote tape

.	MBS_TY_BUFFER			Buffer type:
.	MBS_BTYPE_HEADER	--		FileHeader
.	MBS_BTYPE_VME		--		VME formatted data
.	MBS_BTYPE_EOF		--		End of file
.	MBS_BTYPE_ERROR		--		Error
.	MBS_BTYPE_RAW		--		Raw mode

.	MBS_TY_EVENT			Event type:
.	MBS_ETYPE_VME		--		VME formatted data
.	MBS_ETYPE_EOF		--		End of file
.	MBS_ETYPE_ERROR		--		Error
.	MBS_ETYPE_RAW		--		Raw mode

.	MBS_TY_SUBEVENT			Subevent type:
.	MBS_STYPE_CAMAC		--		CAMAC data
.	MBS_STYPE_CAMAC_WO_ID		--		CAMAC data without id
.	MBS_STYPE_TIME_STAMP		--		Time stamp
.	MBS_ETYPE_EOE		--		End of event
.	MBS_STYPE_ERROR		--		Error
.	MBS_STYPE_RAW		--		Raw mode
.	MBS_STYPE_DUMMY		--		MBS dummy subevent

.	MBS_L_STR			-- string length
.	MBS_L_NAME          -- length of an object name

.	MBS_N_BUFFERS		-- number of buffers in the buffer ring

.	MBS_ODD_NOF_PARAMS	-- indicates an odd number of params
DESCRIPTION

KEYWORDS

SEE ALSO

*****************************************************************************/

#define MBS_ID_WORD				"%MBS_RAW_DATA%"
#define MBS_SIZEOF_DATA			0x4000

#define MBS_RTYPE_ERROR			0xffffffff

#define MBS_CTYPE_FILE			0x1
#define MBS_CTYPE_SYNC			0x2
#define MBS_CTYPE_ASYNC 		0x4
#define MBS_CTYPE_REMOTE		0x8
#define MBS_CTYPE_FILE_LMD		(MBS_CTYPE_FILE | 0x10)
#define MBS_CTYPE_FILE_MED		(MBS_CTYPE_FILE | 0x20)

#define MBS_TY_FHEADER			0x1
#define MBS_X_FHEADER			0

#define MBS_TY_BUFFER			0x2
#define MBS_X_BUFFER			1
#define MBS_BTYPE_HEADER		0x000107d0		// [1,2000]
#define MBS_BTYPE_VME			0x0001000a		// [1,10]
#define MBS_BTYPE_EOF			0
#define MBS_BTYPE_ERROR			0xffffffff
#define MBS_BTYPE_ABORT			0xfffffffe
#define MBS_BTYPE_RAW			0xfffffffd

#define MBS_TY_EVENT			0x4
#define MBS_X_EVENT				2
#define MBS_ETYPE_VME			0x0001000a		// [1,10]
#define MBS_ETYPE_EOF			MBS_BTYPE_EOF
#define MBS_ETYPE_ERROR			MBS_BTYPE_ERROR
#define MBS_ETYPE_ABORT			MBS_BTYPE_ABORT
#define MBS_ETYPE_RAW			MBS_BTYPE_RAW

#define MBS_TY_SUBEVENT				0x8
#define MBS_X_SUBEVENT				3
#define MBS_STYPE_CAMAC_1			0x0001000a		// [1,10]
#define MBS_STYPE_CAMAC_WO_ID_1		0x000b000a		// [11,10]
#define MBS_STYPE_CAMAC_MULT_MOD	0x000c000a		// [12,10]
#define MBS_STYPE_CAMAC_2			0x000d000a		// [13,10]
#define MBS_STYPE_CAMAC_WO_ID_2		0x000e000a		// [14,10]
#define MBS_STYPE_CAMAC_DGF_1		0x0015000a		// [21,10]
#define MBS_STYPE_CAMAC_DGF_2		0x0016000a		// [22,10]
#define MBS_STYPE_CAMAC_DGF_3		0x0017000a		// [23,10]
#define MBS_STYPE_CAMAC_SILENA_1	0x001f000a		// [31,10]
#define MBS_STYPE_CAMAC_SILENA_2	0x0020000a		// [32,10]
#define MBS_STYPE_VME_CAEN_1		0x0029000a		// [41,10]
#define MBS_STYPE_VME_CAEN_2		0x002a000a		// [42,10]
#define MBS_STYPE_VME_CAEN_3		0x002b000a		// [43,10]
#define MBS_STYPE_DATA_SHORT		0x0040000a		// [64,10]
#define MBS_STYPE_DATA_INT			0x0041000a		// [65,10]
#define MBS_STYPE_DATA_FLOAT		0x0042000a		// [66,10]
#define MBS_STYPE_TIME_STAMP		0x00012328		// [1,9000]
#define MBS_STYPE_DEAD_TIME 		0x00022328		// [2,9000]
#define MBS_STYPE_DUMMY 	 		0x006f006f		// [111,111]
#define MBS_STYPE_EOE				MBS_BTYPE_EOF
#define MBS_STYPE_ERROR				MBS_BTYPE_ERROR
#define MBS_STYPE_ABORT				MBS_BTYPE_ABORT
#define MBS_STYPE_RAW		 		MBS_BTYPE_RAW

#define MBS_TY_STAT				0x10
#define MBS_X_STAT				4

#define MBS_N_BELEMS			5
#define MBS_N_TRIGGERS			16
#define MBS_N_BUFFERS			5

#define MBS_L_STR				256
#define MBS_L_NAME				64

#define MBS_ODD_NOF_PARAMS		1

/*C_STRUKTUR******************************************************************
NAME
	MBSShowElem
AUTHOR
	R. Lutter
PURPOSE
	Describe how to show a buffer element
ELEMENTS
.	out		-- stream to send output to
.	redu	-- reduction rate
DESCRIPTION
	Structure to describe how to show a buffer element
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	FILE *out;
	int redu;
} MBSShowElem;

/*C_STRUKTUR******************************************************************
NAME
	MBSBufferElem
AUTHOR
	R. Lutter
PURPOSE
	Describe a buffer element
ELEMENTS
.	type	-- element type
.	descr	-- description
.	hsize	-- header size (bytes)
.	hit		-- # of hits
.	unpack	-- proc to unpack element
.	show	-- ... to show element
.	convert	-- ... to convert data
DESCRIPTION
	Structure to describe how to handle a buffer element
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	unsigned int type;
	char *descr;
	int hsize;
	int hit;
	int (*unpack)();
	int (*show)();
	int (*convert)();
} MBSBufferElem;

/*C_STRUKTUR******************************************************************
NAME
	MBSServerInfo
AUTHOR
	M. Muench
PURPOSE
	Server info from MBS
ELEMENTS
.	is_swapped		-- true, if data swapped
.	buf_size 		-- buffer size
.	buf_p_stream	-- buffers per stream
.	nof_streams 	-- number of streams
DESCRIPTION
	Holds server info.
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	int is_swapped;
	int buf_size;
	int buf_p_stream;
	int nof_streams;
} MBSServerInfo;

/*C_STRUKTUR******************************************************************
NAME
	MBSBufferPool
AUTHOR
	R. Lutter
PURPOSE
	Establish a buffer pool
ELEMENTS
.	bufno_mbs         -- MBS buffer number
.	data              -- address of data
DESCRIPTION
	Structure to establish a buffer pool
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	int bufno_mbs;
	char * data;
} MBSBufferPool;

/*C_STRUCTURE*****************************************************************
NAME
	MBSDataIO
AUTHOR
	R. Lutter
PURPOSE
	MBS raw data structure
ELEMENTS
.	id					-- internal struct id: %MBS_RAW_DATA%
.	input				-- input stream descr (fopen/fread)
.	fileno				-- ... channel # (open/read)
.	device				-- name of input dev
.	host				-- host name
.	connection			-- device type, MBS_DTYPE_xxxx
.	buftype				-- buffer type
.	byte_order			-- byte ordering
.	show_elems			-- buffer elements to be shown automatically
.	bufsiz				-- buffer size
.	server_info 		-- info block for server access
.	max_streams 		-- max # of streams to process
.	slow_down			-- # of secs to wait after each stream
.	nof_streams 		-- # of streams processed so far
.	nof_buffers			-- # of buffers
.	nof_events			-- # of events
.	cur_bufno			-- buffer number
.	cur_bufno_stream	-- ... within current stream
.	bufno_mbs           -- buffer number as given by MBS
.   buf_to_be_dumped    -- if N>0 every Nth buffer will be dumped
.	hdr_data			-- file header data
.	buf_pool			-- buffer pool
.	poolpt              -- ... pointer to current buffer in pool
.	bufpt				-- pointer to current data
.	buf_valid			-- TRUE if buffer data valid
.	buf_oo_phase        -- buffer out of phase
.	evttype				-- event type
.	evtsiz				-- event size (bytes)
.	evtpt				-- ptr to current event in buffer
.	evtno				-- current event number within buffer
.	evtno_mbs           -- event number as given by MBS
.	evt_data			-- copy of event data (original, byte-swapped if necessary)
.	sevttype			-- subevent type
.	sevtsiz				-- subevent size (bytes)
.	sevtpt				-- ptr to original subevent in evt_data
.	sevtno				-- current subevent number within event
.	sevt_id				-- current subevent id
.	sevt_otype			-- original subevent [subtype,type]
.	sevt_minwc			-- min # of data words
.	sevt_wc				-- # of data words
.	sevt_data			-- subevent data (unpacked)
DESCRIPTION
	structure to keep track of input data
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	char id[16];
	FILE *input;
	int fileno;
	char device[MBS_L_STR];
	char host[MBS_L_STR];
	unsigned int connection;
	MBSBufferElem *buftype;
	int byte_order;
	MBSShowElem show_elems[MBS_N_BELEMS];
	int bufsiz;
	MBSServerInfo *server_info;
	int max_streams;
	int slow_down;
	int nof_streams;
	int nof_buffers;
	int nof_events;
	int cur_bufno;
	int cur_bufno_stream;
	int bufno_mbs;
	int buf_to_be_dumped;
	char *hdr_data;
	MBSBufferPool buf_pool[MBS_N_BUFFERS];
	MBSBufferPool * poolpt;
	char *bufpt;
	int buf_valid;
	int buf_oo_phase;
	MBSBufferElem *evttype;
	int evtsiz;
	char *evtpt;
	int evtno;
	int evtno_mbs;
	char *evt_data;
	MBSBufferElem *sevttype;
	int sevtsiz;
	char *sevtpt;
	int sevtno;
	int nof_sevents;
	int sevt_id;
	unsigned int sevt_otype;
	int sevt_minwc;
	int sevt_wc;
	char *sevt_data;
} MBSDataIO;

/*C_STRUKTUR******************************************************************
NAME
	MBSCamacData
	MBSCamacDataSW
AUTHOR
	R. Lutter
PURPOSE
	Structure of a CAMAC data word (subevent type [10,1])
ELEMENTS
.	data		-- data word
.	subaddr		-- camac subaddr A(x)+1
DESCRIPTION
	Describes one camac data word
KEYWORDS

SEE ALSO

*****************************************************************************/

typedef struct {
	unsigned short data;
	short subaddr;
} MBSCamacData;

typedef struct {
	short subaddr;
	unsigned short data;
} MBSCamacDataSW;

#endif
