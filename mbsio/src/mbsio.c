/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbsio.c
// Purpose:        MSB raw data input
// Description:    Procedures to read MBS data from (remote) tape, disk, or tcp socket
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// Keywords:       
//////////////////////////////////////////////////////////////////////////// */

/* include files needed by mbsio */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include "byte_order.h"

#define TRUE	1
#define FALSE	0

#include "mbsio.h"

/* include files needed by m.muench's tcp package */

#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* if you want to have remote tape access */

extern int errno;

char loc_errbuf[MBS_L_STR]; 		// where to store error messages
char *rem_errbuf = NULL;

char loc_logbuf[MBS_L_STR];
char log_file[MBS_L_STR];
FILE * log_out = NULL;

char med_file[MBS_L_STR];
FILE * med_out = NULL;
		
char lmd_file[MBS_L_STR];
FILE * lmd_out = NULL;
		
int total = 0;

void _mbs_show_fheader();
void _mbs_show_bheader();
void _mbs_show_evhe_10_1();
void _mbs_show_sev_10_1();
void _mbs_show_sev_10_11();
void _mbs_show_sev_raw();
void _mbs_copy_fheader();
void _mbs_convert_bheader();
void _mbs_convert_eheader();
void _mbs_convert_sheader();
void _mbs_type_error();
unsigned int *_mbs_unpack_sev_10_1();
unsigned int *_mbs_unpack_sev_10_11();
unsigned int *_mbs_unpack_sev_raw();

void _mbs_show_sev_9000_1();
void _mbs_show_sev_9000_2();
unsigned int *_mbs_unpack_sev_9000_X();

static MBSBufferElem buffer_types[] = {
				{	MBS_BTYPE_HEADER,		// [subtype,type]
					"File header",			// description
					sizeof(s_filhe),		// size
					0,						// hits
					NULL,					// proc to unpack
					_mbs_show_fheader,		// proc to show data
					_mbs_copy_fheader		// proc to convert data
				},
				{	MBS_BTYPE_VME,
					"VME buffer",
					sizeof(s_bufhe),
					0,
					NULL,
					_mbs_show_bheader,
					_mbs_convert_bheader
				},
				{0, "", 0,  0,  NULL,   NULL,   NULL}
			};

static MBSBufferElem event_types[] = {
				{	MBS_ETYPE_VME,
					"Standard MBS event",
					sizeof(s_vehe),
					0,
					NULL,
					_mbs_show_evhe_10_1,
					_mbs_convert_eheader
				},
				{0, "", 0,  0,  NULL,   NULL,   NULL}
			};

static MBSBufferElem sevent_types[] = {
				{	MBS_STYPE_CAMAC_1,
					"Chn + Data",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_1,
					_mbs_show_sev_10_1,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_WO_ID_1,
					"Data w/o Chn",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_2,
					"Chn + Data",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_1,
					_mbs_show_sev_10_1,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_WO_ID_2,
					"Data w/o Chn",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_TIME_STAMP,
					"Time stamp",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_9000_X,
					_mbs_show_sev_9000_1,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_DEAD_TIME,
					"Dead time",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_9000_X,
					_mbs_show_sev_9000_2,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_DGF_1,
					"XIA DGF-4C (1)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_DGF_2,
					"XIA DGF-4C (2)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_DGF_3,
					"XIA DGF-4C (3, time stamp)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_SILENA_1,
					"Silena 4418 (1, zero suppr)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_CAMAC_SILENA_2,
					"Silena 4418 (2, zero suppr)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_VME_CAEN_1,
					"Caen VME ADCs/TDCs (1)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_VME_CAEN_2,
					"Caen VME ADCs/TDCs (2)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_DATA_SHORT,
					"Plain data (16 bit)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_DATA_INT,
					"Plain data (32 bit)",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_11,
					_mbs_show_sev_10_11,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_DUMMY,
					"MBS dummy",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_10_1,
					_mbs_show_sev_10_1,
					_mbs_convert_sheader
				},
				{	MBS_STYPE_RAW,
					"Raw data",
					sizeof(s_veshe),
					0,
					_mbs_unpack_sev_raw,
					_mbs_show_sev_raw,
					_mbs_convert_sheader
				},
				{0, "", 0,  0,  NULL,   NULL,   NULL}
			};

static MBSBufferElem * sevent_type_raw = NULL;

static MBSBufferElem triggers[MBS_N_TRIGGERS];

static MBSBufferElem buffer_type_error =
				{	MBS_BTYPE_ERROR,
					"Illegal buffer type",
					0,
					0,
					_mbs_type_error,
					_mbs_type_error,
					_mbs_type_error
				};

static MBSBufferElem event_type_error =
				{	MBS_ETYPE_ERROR,
					"Illegal event type",
					0,
					0,
					_mbs_type_error,
					_mbs_type_error,
					_mbs_type_error
				};

static MBSBufferElem sevent_type_error =
				{	MBS_STYPE_ERROR,
					"Illegal subevent type",
					0,
					0,
					_mbs_type_error,
					_mbs_type_error,
					_mbs_type_error
				};

MBSDataIO *mbs_open_file(char *device, char *connection, int bufsiz, FILE *out) {
/*________________________________________________________[C PUBLIC FUNCTION]
/////////////////////////////////////////////////////////////////////////////
// Name:           mbs_open_file
// Purpose:        Open a MBS data file
// Arguments:      char * device       -- name of input dev
//                 char * connection   -- mode of connection: f(ile), s(ynchronous), or a(synchronous)
//                 int bufsiz          -- buffer size, default = 16 kbytes
//                 FILE * out          -- stream to send header info to (NULL = don't output header)
// Results:        MBSDataIO * mbs     -- structure address or NULL
// Exceptions:     mbs = NULL in case of errors
// Description:    Opens an input stream for mbs data.
// Keywords:
/////////////////////////////////////////////////////////////////////////// */

	register FILE *input;
	MBSDataIO *mbs;
	char cmode;
	char *dev;
	char *ext;
	int fno;
	int bps;
	unsigned int ctype;
	unsigned int buffer_type;

	MBSBufferElem *tlist;

	char host[MBS_L_STR];
	MBSServerInfo server_info;

	char *calloc();
	MBSServerInfo * _mbs_read_server_info();
	void _mbs_output_error();

	if (sevent_type_raw == NULL) {
		tlist = sevent_types;
		while (tlist->type != 0) {
			if (tlist->type == MBS_STYPE_RAW) {
				sevent_type_raw = tlist;
				break;
			}
		tlist++;
		}
	}	

	cmode = tolower(*connection);

	if (cmode == 'f') {
		ctype = MBS_CTYPE_FILE;
		dev = strchr(device, ':');
		if (dev != NULL) {
			ctype |= MBS_CTYPE_REMOTE;
			*dev++ = '\0';
			strcpy(host, device);
		} else {
			dev = device;
			host[0] = '\0';
		}

		if (ctype & MBS_CTYPE_REMOTE) {
			sprintf(loc_errbuf, "?NO_REM-[mbs_open_file]- %s: No REMOTE tape access", device);
			_mbs_output_error();
			return(NULL);
		} else {
			if (strcmp(device, "-") == 0) {
				input = stdin;
				device = "<stdin>";
			} else if ((input = fopen(device, "r")) == NULL) {
				sprintf(loc_errbuf, "?SYSERR-[mbs_open_file]- %s: %s (%d)",
													device, sys_errlist[errno], errno);
				_mbs_output_error();
				return(NULL);
			}
			ext = strchr(device, '.');
			if (ext != NULL) {
				if (strcmp(ext, ".med") == 0) ctype = MBS_CTYPE_FILE_MED;
				else if (strcmp(ext, ".lmd") == 0) ctype = MBS_CTYPE_FILE_LMD;
			}
			fno = fileno(input);
		}
	} else if (cmode == 's') {
		ctype = MBS_CTYPE_SYNC;
	} else if (cmode == 'a') {
		ctype = MBS_CTYPE_ASYNC;
	} else {
		sprintf(loc_errbuf
,
				"?ILLMOD-[mbs_open_file]- %s: Illegal I/O mode \"%s\"",
							device, connection);
		_mbs_output_error();
		return(NULL);
	}

	bps = 1;
	if (ctype & (MBS_CTYPE_SYNC | MBS_CTYPE_ASYNC)) {
		strcpy(host, device);
		fno = _mbs_connect_to_server(host, ctype);
		if (fno == -1) {
			_mbs_output_error();
			return(NULL);
		}
		if (_mbs_read_server_info(fno, &server_info) == NULL) {
			_mbs_output_error();
			return(NULL);
		}
		bufsiz = server_info.buf_size;
		bps = server_info.buf_p_stream;
		input = fdopen(fno, "r");
	}

	bufsiz = (bufsiz <= 0) ? MBS_SIZEOF_DATA : bufsiz;

	mbs = (MBSDataIO *) calloc(1, sizeof(MBSDataIO));
	if (mbs == NULL)
	{
		sprintf(loc_errbuf, "?ALLOC-[mbs_open_file]- %s: Can't allocate data structure",
															device);
		_mbs_output_error();
		return(NULL);
	}

	strcpy(mbs->id, MBS_ID_WORD);
	mbs->input = input;
	mbs->fileno = fno;
	mbs->cur_bufno_stream = bps;
	strcpy(mbs->device, device);
	strcpy(mbs->host, host);
	mbs->connection = ctype;
	mbs->bufsiz = bufsiz;
	mbs->bufno_mbs = -1;
	mbs->evtno_mbs = -1;
	mbs->buf_to_be_dumped = 0;
	
	if (ctype & MBS_CTYPE_FILE) {
		mbs->server_info = NULL;
	} else {
		mbs->server_info = calloc(1, sizeof(MBSServerInfo));
		memcpy(mbs->server_info, &server_info, sizeof(MBSServerInfo));
	}

	mbs->hdr_data = calloc(1, bufsiz);
	if (mbs->hdr_data == NULL)
	{
		sprintf(loc_errbuf, "?ALLOC-[mbs_open_file]- %s: Can't allocate internal buffer",
															device);
		_mbs_output_error();
		return(NULL);
	}

	if (out) {
		mbs->show_elems[MBS_X_FHEADER].out = out;
		mbs->show_elems[MBS_X_FHEADER].redu = 1;
	}

	_mbs_init_hit(buffer_types);
	_mbs_init_hit(event_types);
	_mbs_init_hit(sevent_types);
	_mbs_init_triggers();

	_mbs_init_pool(mbs);

	mbs->buf_valid = FALSE;

	if (ctype == MBS_CTYPE_FILE) {
		buffer_type = _mbs_next_buffer(mbs);
		if (buffer_type == MBS_BTYPE_ERROR || buffer_type == MBS_BTYPE_ABORT) return(NULL);
		if (buffer_type == MBS_BTYPE_HEADER) mbs->buf_valid = FALSE;
	}

	return(mbs);
}

int mbs_close_file(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_close_file
// Purpose:        Close MBS data file
// Arguments:      MBSDataIO * mbs -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Close i/o stream and free memory
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */
	
	if (!_mbs_check_active(mbs)) return(FALSE);

	if (mbs->connection & MBS_CTYPE_FILE) {
		fclose(mbs->input);
	} else {
		_mbs_disconnect_from_server(mbs->fileno, mbs->connection);
		free(mbs->server_info);
	}

	if (log_out != NULL) {
		sprintf(loc_logbuf,
			"[mbs_close_file]- %s: %d buffer(s), %d out of phase, %d event(s)",
					mbs->device, mbs->nof_buffers, mbs->buf_oo_phase, mbs->nof_events);
		_mbs_output_log();
		fclose(log_out);
	}

	mbs->fileno = -1;		/* indicate "file inactive" */
	return(TRUE);
}

int mbs_free_dbase(MBSDataIO * mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_free_dbase
// Purpose:        Free data base
// Arguments:      MBSDataIO * mbs -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Frees memory allocated so far.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */
	
	if (mbs != NULL) {
		free(mbs->hdr_data);
		free(mbs->evt_data);
		free(mbs->sevt_data);
		_mbs_free_pool(mbs);
		free(mbs);
	}
	return(TRUE);
}

unsigned int _mbs_next_buffer(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_next_buffer
// Purpose:        Get next buffer
// Arguments:      mbs                -- ptr as returned by mbs_open_file
// Results:        unsigned int type  -- buffer type: MBS_BTYPE_xxxx
// Exceptions:     type = MBS_BTYPE_ERROR if error
// Description:    Requests next buffer either from data source or from
//                 buffer pool.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register MBSBufferPool * bpp;
	int sc;
	unsigned int buffer_type;
	s_bufhe * bh;

	MBSBufferPool * _mbs_find_subseq_buffer();
	void _mbs_output_error();
	void (*s)();

	if (!_mbs_check_active(mbs)) return(MBS_BTYPE_ABORT);

	bpp = NULL;
	while (bpp == NULL) {
		bpp = _mbs_find_subseq_buffer(mbs);
		if (bpp == NULL) {
			buffer_type = _mbs_read_buffer(mbs);
			if (buffer_type == MBS_BTYPE_VME) {
				if (!_mbs_check_sequence(mbs, MBS_TY_BUFFER)) {
					mbs->buf_oo_phase++;
					if (log_out != NULL) {
						sprintf(loc_errbuf,
				"?ILLSEQ-[_mbs_next_buffer]- %s: illegal buffer sequence - last=%d <> this=%d",
							mbs->device, mbs->bufno_mbs, (mbs->poolpt)->bufno_mbs);
						_mbs_output_error();
					}
				} else {
					bh = (mbs->poolpt)->data;
					mbs->bufno_mbs = bh->l_buf;
					bpp = mbs->poolpt;
				}
			} else if (buffer_type == MBS_BTYPE_HEADER) break; else return(buffer_type);
		} else {
			mbs->poolpt = bpp;
			mbs->bufpt = bpp->data;
		}
	}

	(mbs->poolpt)->bufno_mbs = -1;

	mbs->buf_valid = TRUE;
	mbs->evtno = 0;
	mbs->evtpt = NULL;
	mbs->sevtno = 0;
	mbs->sevtpt = NULL;
	mbs->sevt_wc = 0;

	if (buffer_type == MBS_BTYPE_HEADER) {
		mbs->nof_events = 0;

		sc = mbs->show_elems[MBS_X_FHEADER].redu;
		if (sc > 0) {
			s = (mbs->buftype)->show;			// show file header
			if (s != NULL) (*s)(mbs, mbs->show_elems[MBS_X_FHEADER].out);
		}
	} else {
		sc = mbs->show_elems[MBS_X_BUFFER].redu;
		if (sc > 0 && ((mbs->nof_buffers % sc) == 0)) {
			s = (mbs->buftype)->show;			// show buffer header
			if (s != NULL) (*s)(mbs, mbs->show_elems[MBS_X_BUFFER].out);
		}

		sc = mbs->show_elems[MBS_X_STAT].redu;
		if (sc > 0 && ((mbs->nof_buffers % sc) == 0))
				mbs_show_stat(mbs, mbs->show_elems[MBS_X_STAT].out);
	}

	return(buffer_type);
}

unsigned int _mbs_read_buffer(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_read_buffer
// Purpose:        Read next buffer
// Arguments:      mbs                -- ptr as returned by mbs_open_file
// Results:        unsigned int type  -- buffer type: MBS_BTYPE_xxxx
// Exceptions:     type = MBS_BTYPE_ERROR if error
// Description:    Reads one buffer and determines buffer type
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register MBSBufferPool * bpp;
	int bytes;
	int bytes_read;
	unsigned int buffer_type;

	MBSBufferPool * _mbs_get_pool_pointer();
	void _mbs_output_error();

	if (!_mbs_check_active(mbs)) return(MBS_BTYPE_ABORT);

	bytes = mbs->bufsiz;

	bpp = _mbs_get_pool_pointer(mbs);
	if (bpp == NULL) return(MBS_BTYPE_ABORT);
	mbs->poolpt = bpp;

	if (mbs->connection & MBS_CTYPE_FILE) {
		if (mbs->connection & MBS_CTYPE_REMOTE) {
			sprintf(loc_errbuf, "?NO_REM-[_mbs_read_buffer]- %s: No REMOTE tape access",
																mbs->device);
			_mbs_output_error();
			return(MBS_BTYPE_ABORT);
		} else {
			bytes_read = read(mbs->fileno, bpp->data, bytes);
		}	
		mbs->cur_bufno_stream = 1;
		mbs->bufpt = bpp->data;
	} else {
		if ((mbs->max_streams > 0) && (mbs->nof_streams > mbs->max_streams)) {
			sprintf(loc_errbuf, "%%MXNSTR-[_mbs_read_buffer]- Max # of streams read: %d", mbs->max_streams);
			_mbs_output_error();
			mbs_close_file(mbs);
			return(MBS_BTYPE_EOF);
		} else if (mbs->connection & MBS_CTYPE_ASYNC) {
			if ((mbs->slow_down > 0) && (mbs->nof_streams > 0)) sleep(mbs->slow_down);
			_mbs_request_stream(mbs->fileno);
			bytes_read = _mbs_read_stream(mbs->fileno, bpp->data, mbs->server_info);
			mbs->cur_bufno_stream = 1;
			mbs->bufpt = bpp->data;
			mbs->nof_streams++;
		} else if (mbs->connection & MBS_CTYPE_SYNC) {
			bytes_read = _mbs_read_stream(mbs->fileno, bpp->data, mbs->server_info);
			mbs->cur_bufno_stream = 1;
			mbs->bufpt = bpp->data;
			mbs->nof_streams++;
		}
	}

	if (bytes_read == 0) return(MBS_BTYPE_EOF);

	if (bytes_read == -1)
	{
		sprintf(loc_errbuf, "?INPERR-[_mbs_read_buffer]- %s (buf %d): %s (%d)",
							mbs->device, mbs->cur_bufno, sys_errlist[errno], errno);
		_mbs_output_error();
		return(MBS_BTYPE_ABORT);
	}

	mbs->nof_buffers++;

	if (bytes_read != bytes)
	{
		sprintf(loc_errbuf,
		"%%INPERR-[_mbs_read_buffer]- %s (buf %d): Short byte count %d (should be %d)",
								mbs->device, mbs->cur_bufno, bytes_read, bytes);
		_mbs_output_error();
	}

	if (mbs->buf_to_be_dumped > 0 && mbs->nof_buffers % mbs->buf_to_be_dumped == 0) _mbs_dump_buffer(mbs);
	
	buffer_type = _mbs_convert_data(mbs);
	if (buffer_type == MBS_BTYPE_ERROR || buffer_type == MBS_BTYPE_ABORT) return(buffer_type);

	_mbs_store_bufno(mbs);

	return(buffer_type);
}

unsigned int mbs_next_event(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_event
// Purpose:        Setup next event
// Arguments:      MBSDataIO * mbs     -- ptr as returned by mbs_open_file
// Results:        unsigned int etype  -- event type
// Exceptions:     etype = MBS_ETYPE_ERROR or MBS_ETYPE_EOF
// Description:    Sets ptrs to next event struct
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	unsigned int _mbs_next_med_event(MBSDataIO *);
	unsigned int _mbs_next_lmd_event(MBSDataIO *);

	if (!_mbs_check_active(mbs)) return(MBS_ETYPE_ABORT);

	if (mbs->connection & MBS_CTYPE_FILE_MED)	return(_mbs_next_med_event(mbs));
	else													return(_mbs_next_lmd_event(mbs));
}

unsigned int _mbs_next_lmd_event(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_next_lmd_event
// Purpose:        Setup next event
// Arguments:      MBSDataIO * mbs     -- ptr as returned by mbs_open_file
// Results:        unsigned int etype  -- event type
// Exceptions:     etype = MBS_ETYPE_ERROR or MBS_ETYPE_EOF
// Description:    Sets ptrs to next event struct
//                 (expects original LMD data from tcp or file)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_bufhe *bh;
	s_evhe *eh;
	s_vehe *vh;
	unsigned int bo;
	unsigned int btype, etype;
	int evl;
	int frag1, frag2;
	int sc;
	unsigned int (*s)();

	MBSBufferElem *_mbs_check_type();
	char *calloc();
	void _mbs_output_error();

	unsigned char eHdr[sizeof(s_vehe)];
	int ehs;

	bh = mbs->bufpt;
	bo = mbs->byte_order;

	if (mbs->evtpt == NULL || mbs->evtno == bh->l_evt) {
		if (!mbs->buf_valid) {
			btype = _mbs_next_buffer(mbs);
			if (btype == MBS_BTYPE_ERROR || btype == MBS_BTYPE_ABORT || btype == MBS_BTYPE_EOF) return(btype);
			bo = mbs->byte_order;
			bh = mbs->bufpt;
			mbs->cur_bufno = bh->l_buf;
		}

		if (bh->h_begin == 1) {
			sprintf(loc_errbuf,
			"?EVTERR-[_mbs_next_lmd_event]- %s (buf %d, evt 1): Illegal event spanning - unexp frag on top",
						mbs->device, mbs->cur_bufno);
			_mbs_output_error();
			mbs->buf_valid = FALSE;
			return(MBS_ETYPE_ERROR);
		}
		mbs->evtpt = mbs->bufpt + sizeof(s_bufhe);
		mbs->evtno = 1;
	} else if (mbs->evtno == 1 && bh->h_begin == 1) {
		mbs->evtpt = mbs->bufpt + sizeof(s_bufhe);
		eh = (s_evhe *) mbs->evtpt;
		bto_get_long(&evl, &eh->l_dlen, 1, bo);
		mbs->evtpt += evl * sizeof(unsigned short) + sizeof(s_evhe);
		mbs->evtno++;
	} else {
		eh = (s_evhe *) mbs->evt_data;
		mbs->evtpt += eh->l_dlen * sizeof(unsigned short) + sizeof(s_evhe);
		mbs->evtno++;
	}
	eh = (s_evhe *) mbs->evtpt;
	mbs->buf_valid = FALSE;

	if (mbs->evtno == bh->l_evt && bh->h_end == 1) {
		evl = bh->l_free[1];
		bto_get_long(&frag1, &eh->l_dlen, 1, bo);
	} else {
		bto_get_long(&evl, &eh->l_dlen, 1, bo);
		frag1 = evl;
	}

	evl = evl * sizeof(unsigned short) + sizeof(s_evhe);
	frag1 = frag1 * sizeof(unsigned short) + sizeof(s_evhe);

	if (evl <= mbs->evtsiz) {
		memset(mbs->evt_data, 0, mbs->evtsiz);
	} else {
		if (mbs->evt_data != NULL) free(mbs->evt_data);
		mbs->evt_data = calloc(1, evl);
		memset(mbs->evt_data, 0, evl);
	}
	mbs->evtsiz = evl;
	memcpy(mbs->evt_data, mbs->evtpt, frag1);

	eh = (s_evhe *) mbs->evt_data;
	bto_get_long(&etype, &eh->i_subtype, 1, bo);

	mbs->evttype = _mbs_check_type(etype, mbs->evttype, event_types);
	etype = (mbs->evttype)->type;
	if (etype == MBS_ETYPE_ERROR || etype == MBS_ETYPE_ABORT) {
		sprintf(loc_errbuf,
		"?EVTERR-[_mbs_next_lmd_event]- %s (buf %d, evt %d): Not a legal event type - %#lx",
					mbs->device, mbs->cur_bufno, mbs->evtno, etype);
		_mbs_output_error();
		return(etype);
	}

	memcpy(eHdr, mbs->evt_data, sizeof(s_vehe)); 	/* save header ###unswapped### */

	s = (mbs->evttype)->convert;
	(*s)(mbs);

	frag2 = 0;

	while (frag1 < evl) {
		btype = _mbs_next_buffer(mbs);
		if (btype == MBS_BTYPE_ERROR || btype == MBS_BTYPE_ERROR || btype == MBS_BTYPE_EOF) return(btype);
		bh = mbs->bufpt;
		mbs->cur_bufno = bh->l_buf;

		if (bh->h_begin != 1) {
			sprintf(loc_errbuf,
			"?EVTERR-[_mbs_next_lmd_event]- %s (buf %d, evt %d): Illegal event spanning - 2nd frag missing",
						mbs->device, mbs->cur_bufno, mbs->evtno);
			_mbs_output_error();
			return(MBS_ETYPE_ABORT);
		}

		mbs->evtpt = mbs->bufpt + sizeof(s_bufhe);
		mbs->evtno = 1;
		eh = (s_evhe *) mbs->evtpt;
		bto_get_long(&frag2, &eh->l_dlen, 1, bo);
		frag2 *= sizeof(unsigned short);

		memcpy(mbs->evt_data + frag1, mbs->evtpt + sizeof(s_evhe), frag2);
		mbs->evtno = 1;
		frag1 += frag2;
	}

	if (frag1 != evl) {
		sprintf(loc_errbuf,
		"?EVTERR-[_mbs_next_lmd_event]- %s (buf %d, evt %d): Illegal event fragmentation",
					mbs->device, mbs->cur_bufno, mbs->evtno);
		_mbs_output_error();
		return(MBS_ETYPE_ABORT);
	}

	mbs->nof_events++;
	(mbs->evttype)->hit++;
	mbs->sevtpt = NULL;
	mbs->sevtno = 0;
	mbs->sevt_wc = 0;

	if (!_mbs_check_sequence(mbs, MBS_TY_EVENT)) {
		sprintf(loc_errbuf,
		"?ILLSEQ-[_mbs_next_lmd_event]- %s (buf %d): Illegal event sequence - last=%d <> this=%d",
						mbs->device, mbs->evtno_mbs, ((s_vehe *) mbs->evt_data)->l_count);
			_mbs_output_error();
		return(MBS_ETYPE_ERROR);
	}
	mbs->evtno_mbs = ((s_vehe *) mbs->evt_data)->l_count;

	if ((mbs->evttype)->type == MBS_ETYPE_VME) {
		vh = (s_vehe *) mbs->evt_data;
		triggers[vh->i_trigger].hit++;
	}

	sc = mbs->show_elems[MBS_X_EVENT].redu;
	if (sc > 0 && ((mbs->nof_events % sc) == 0)) {
		s = (mbs->evttype)->show;
		if (s != NULL) (*s)(mbs, mbs->show_elems[MBS_X_EVENT].out);
	}

	if (med_out) {
		eh = eHdr;
		bto_put_long(&evl, &eh->l_dlen, 1, bo);
		ehs = sizeof(s_vehe);
		fwrite(eHdr, 1, ehs, med_out);								/* write event header - take unswapped data */
		fwrite((char *) mbs->evt_data + ehs, 1, mbs->evtsiz - ehs, med_out);	/* write subevent data unswapped */
	} 

	return((mbs->evttype)->type);
}

unsigned int _mbs_next_med_event(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_event
// Purpose:        Setup next event
// Arguments:      MBSDataIO * mbs     -- ptr as returned by mbs_open_file
// Results:        unsigned int etype  -- event type
// Exceptions:     etype = MBS_ETYPE_ERROR or MBS_ETYPE_EOF
// Description:    Sets ptrs to next event struct
//                 (expects MBS event data from file)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_evhe *eh;
	s_vehe *vh;
	unsigned int bo;
	unsigned int btype, etype;
	int evl;
	int sc;
	unsigned int (*s)();
	int bytes_read;

	MBSBufferElem *_mbs_check_type();
	char *calloc();
	void _mbs_output_error();

	unsigned char eHdr[sizeof(s_evhe)];

/**************************************************/
/*	use FIXED byte ordering                       */
	mbs->byte_order = BYTE_ORDER_REV;
/**************************************************/

	bo = mbs->byte_order;

	bytes_read = read(mbs->fileno, eHdr, sizeof(s_evhe));
	if (bytes_read == 0) return(MBS_ETYPE_EOF);
	if (bytes_read == -1)
	{
		sprintf(loc_errbuf, "?INPERR-[_mbs_next_med_event]- %s (evt %d): %s (%d)",
													mbs->device, mbs->evtno, sys_errlist[errno], errno);
		_mbs_output_error();
		return(MBS_ETYPE_ABORT);
	}
	total += bytes_read;

	eh = eHdr;
	bto_get_long(&evl, &eh->l_dlen, 1, bo);
	printf("@@ l_dlen=%ld total=%#lx %ld\n", evl, total, total);

	evl = evl * sizeof(unsigned short) + sizeof(s_evhe);

	if (evl <= mbs->evtsiz) {
		memset(mbs->evt_data, 0, mbs->evtsiz);
	} else {
		if (mbs->evt_data != NULL) free(mbs->evt_data);
		mbs->evt_data = calloc(1, evl);
		memset(mbs->evt_data, 0, evl);
	}

	mbs->evtsiz = evl;

	memcpy(mbs->evt_data, eHdr, sizeof(s_evhe));

	bytes_read = read(mbs->fileno, mbs->evt_data + sizeof(s_evhe), evl - sizeof(s_evhe));
	if (bytes_read != evl - sizeof(s_evhe)) {
		sprintf(loc_errbuf, "?INPERR-[_mbs_next_med_event]- %s (evt %d): short event data (this=%d, expected=%d)",
														mbs->device, mbs->evtno, bytes_read + sizeof(s_evhe), evl);
		_mbs_output_error();
		return(MBS_ETYPE_ABORT);
	}
	total += bytes_read;

	eh = (s_evhe *) mbs->evt_data;
	bto_get_long(&etype, &eh->i_subtype, 1, bo);
	printf("@@ type=%#lx\n", etype);

	mbs->evttype = _mbs_check_type(etype, mbs->evttype, event_types);

	etype = (mbs->evttype)->type;
	if (etype == MBS_ETYPE_ERROR || etype == MBS_ETYPE_ABORT) {
		sprintf(loc_errbuf,
		"?EVTERR-[_mbs_next_med_event]- %s (evt %d): Not a legal event type - %#lx",
														mbs->device, mbs->evtno, etype);
		_mbs_output_error();
		return(etype);
	}

	s = (mbs->evttype)->convert;
	(*s)(mbs);

	mbs->nof_events++;
	(mbs->evttype)->hit++;
	mbs->sevtpt = NULL;
	mbs->sevtno = 0;
	mbs->sevt_wc = 0;

	mbs->evtno_mbs = ((s_vehe *) mbs->evt_data)->l_count;

	if ((mbs->evttype)->type == MBS_ETYPE_VME) {
		vh = (s_vehe *) mbs->evt_data;
		printf("@@ trigger=%d\n", vh->i_trigger);
		triggers[vh->i_trigger].hit++;
	}

	sc = mbs->show_elems[MBS_X_EVENT].redu;
	if (sc > 0 && ((mbs->nof_events % sc) == 0)) {
		s = (mbs->evttype)->show;
		if (s != NULL) (*s)(mbs, mbs->show_elems[MBS_X_EVENT].out);
	}

	return((mbs->evttype)->type);
}

unsigned int mbs_next_sheader(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_sheader
// Purpose:        Setup next subevent and decode header
// Arguments:      MBSDataIO * mbs       -- ptr as returned by mbs_open_file
// Results:        unsigned int setype   -- subevent type
// Exceptions:     setype = MBS_STYPE_ERROR
// Description:    Sets ptrs to next subevent struct
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_evhe *eh;
	s_evhe *sh;
	unsigned int bo;
	unsigned int stype;

	MBSBufferElem *_mbs_check_type();

	if (!_mbs_check_active(mbs)) return(MBS_STYPE_ABORT);

	eh = mbs->evt_data;
	bo = mbs->byte_order;

	if (mbs->sevtpt == NULL) {
		mbs->sevtpt = mbs->evt_data + (mbs->evttype)->hsize;
	} else {
		sh = (s_evhe *) mbs->sevtpt;
		mbs->sevtpt += sh->l_dlen * sizeof(unsigned short) + sizeof(s_evhe);
	}
	
	if (mbs->sevtpt >= (mbs->evt_data + mbs->evtsiz)) return(MBS_STYPE_EOE);

	mbs->sevtno++;

	_mbs_convert_sheader(mbs);
	mbs->sevttype = _mbs_check_type(mbs->sevt_otype, mbs->sevttype, sevent_types);
	stype = (mbs->sevttype)->type;

	return(stype);
}

unsigned int mbs_next_sdata(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_sdata
// Purpose:        Get next subevent
// Arguments:      MBSDataIO * mbs       -- ptr as returned by mbs_open_file
// Results:        unsigned int setype   -- subevent type
// Exceptions:     setype = MBS_STYPE_ERROR
// Description:    Converts next subevent.
//                 Needs 'mbs_next_sheader' to be called.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	int sc;
	unsigned int (*s)();

	(mbs->sevttype)->hit++;
	mbs->sevt_wc = 0;

	s = (mbs->sevttype)->unpack;
	(*s)(mbs);

	sc = mbs->show_elems[MBS_X_SUBEVENT].redu;
	if (sc > 0 && ((mbs->nof_events % sc) == 0)) {
		s = (mbs->sevttype)->show;			// show subevent
		if (s != NULL) (*s)(mbs, mbs->show_elems[MBS_X_SUBEVENT].out);
	}

	return((mbs->sevttype)->type);
}

unsigned int mbs_next_sdata_raw(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_sdata_raw
// Purpose:        Get next subevent (raw mode)
// Arguments:      MBSDataIO * mbs       -- ptr as returned by mbs_open_file
// Results:        unsigned int setype   -- subevent type
// Exceptions:     setype = MBS_STYPE_ERROR
// Description:    Reads next subevent in raw mode.
//                 Needs 'mbs_next_sheader' to be called.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	mbs->sevttype = sevent_type_raw;
	return(mbs_next_sdata(mbs));
}

unsigned int mbs_next_sevent(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_sevent
// Purpose:        Decode next subevent
// Arguments:      MBSDataIO * mbs       -- ptr as returned by mbs_open_file
// Results:        unsigned int setype   -- subevent type
// Exceptions:     setype = MBS_STYPE_ERROR
// Description:    Decodes next subevent.
//                 Error if subevent type unknown.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	unsigned int stype;
	unsigned int stp, sstp;

	void _mbs_output_error();

	stype = mbs_next_sheader(mbs);
	if (stype == MBS_STYPE_ABORT || stype == MBS_STYPE_EOE) return(stype);

	if (stype == MBS_STYPE_ERROR) {
		sstp = (mbs->sevt_otype >> 16) & 0xffff;
		stp = mbs->sevt_otype & 0xffff;
		sprintf(loc_errbuf,
		"?EVTERR-[mbs_next_sevent]- %s (buf %d, evt %d, sevt %d): Not a legal subevent type - [%d, %d]",
					mbs->device, mbs->cur_bufno, mbs->evtno, mbs->sevtno, stp, sstp);
		_mbs_output_error();
		return(stype);
	}

	return(mbs_next_sdata(mbs));
}

unsigned int mbs_next_sevent_raw(MBSDataIO *mbs) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_next_sevent_raw
// Purpose:        Get next subevent in raw mode
// Arguments:      MBSDataIO * mbs       -- ptr as returned by mbs_open_file
// Results:        unsigned int setype   -- subevent type
// Exceptions:     setype = MBS_STYPE_ERROR
// Description:    Reads next subevent using raw mode.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	unsigned int stype;

	stype = mbs_next_sheader(mbs);
	if (stype == MBS_STYPE_ABORT || stype == MBS_STYPE_EOE) return(stype);
	mbs->sevttype = sevent_type_raw;
	return(mbs_next_sdata(mbs));
}

int mbs_pass_sevent(MBSDataIO *mbs, unsigned short *data) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_pass_sevent
// Purpose:        Pass subevent data to user
// Arguments:      MBSDataIO * mbs        -- ptr as returned by mbs_open_file
//                 unsigned short * data  -- array to hold subevent data
//                                           (filled with zeros)
// Results:        int wc                 -- length of subevent vector
// Exceptions:     |
// Description:    Passes a subevent vector to the user.
//                 Subevent data are converted to a "stretched vector"
//                 filled with zeros
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (!_mbs_check_active(mbs)) return(-1);

	memcpy(data, mbs->sevt_data, mbs->sevt_wc * sizeof(unsigned short));
	return(mbs->sevt_wc);
}

void mbs_set_sevt_minwc(MBSDataIO *mbs, unsigned int wc) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_set_sevt_minwc
// Purpose:        Define min subevent length
// Arguments:      MBSDataIO * mbs        -- ptr as returned by mbs_open_file
//                 unsigned int wc        -- minimal word count
// Results:        
// Exceptions:     
// Description:    Sets the minimal length of a subevent.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	mbs->sevt_minwc = wc;
}

int mbs_set_stat(MBSDataIO *mbs, int redu, FILE *out) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_set_stat
// Purpose:        Define statistics to be shown
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 int redu         -- reduction: show statistics every
//                                                redu-th buffer only
//                 FILE * out       -- stream to send output to
//                                     (NULL = stdout)
// Results:        --
// Exceptions:     |
// Description:    Defines how often to show events statistics
// Keywords:       |
/////////////////////////////////////////////////////////////////////////// */

	register FILE *old;

	if (!_mbs_check_dbase(mbs)) return(FALSE);

	old = mbs->show_elems[MBS_X_STAT].out;
	if (old != NULL && old != stdout && old != stderr) fclose(old);
	mbs->show_elems[MBS_X_STAT].out = out;
	mbs->show_elems[MBS_X_STAT].redu = redu;
	return(TRUE);
}

int mbs_show_stat(MBSDataIO *mbs, FILE *out) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_show_stat
// Purpose:        Show event statistics
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to (NULL = stdout)
// Results:        --
// Exceptions:     
// Description:    Shows header/data information on stdout.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	MBSBufferElem *tp;
	register int i;
	char *x1;
	char x2[30];
	unsigned int itype, isubtype;

	if (!_mbs_check_dbase(mbs)) return(FALSE);

	if (out == NULL) out = stdout;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  EVENT STATISTICS: %s", mbs->device);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Buffers processed    : %d\n", mbs->nof_buffers);
	fprintf(out, "  Buffers out of phase : %d\n", mbs->buf_oo_phase);
	fprintf(out, "  Events read          : %d\n", mbs->nof_events);
	fprintf(out, "------------------------------------------------------------------------------\n");

	fprintf(out, "  Profile              :\n");
	tp = buffer_types;
	x1 = "Buffers   : ";
	while (tp->type) {
		if (tp->hit) {
			itype = tp->type & 0xffff;
			isubtype = (tp->type >> 16) & 0xffff;
			sprintf(x2, "[%d,%d]", itype, isubtype);
			fprintf(out, "             %-12s%-25s%-12s%8d\n",
										x1, tp->descr, x2, tp->hit);
			x1 = "";
		}
		tp++;
	}
	tp = event_types;
	x1 = "Events    : ";
	while (tp->type) {
		if (tp->hit) {
			itype = tp->type & 0xffff;
			isubtype = (tp->type >> 16) & 0xffff;
			sprintf(x2, "[%d,%d]", itype, isubtype);
			fprintf(out, "             %-12s%-25s%-12s%8d\n",
										x1, tp->descr, x2, tp->hit);
			x1 = "";
		}
		tp++;
	}
	tp = triggers;
	x1 = "Triggers  : ";
	for (i = 0; i < MBS_N_TRIGGERS; i++, tp++) {
		if (tp->hit) {
			fprintf(out, "             %-12s%-25s%-12s%8d\n", x1, tp->descr, "", tp->hit);
			x1 = "";
		}
	}
	tp = sevent_types;
	x1 = "Subevents : ";
	while (tp->type) {
		if (tp->hit) {
			itype = tp->type & 0xffff;
			isubtype = (tp->type >> 16) & 0xffff;
			sprintf(x2, "[%d,%d]", itype, isubtype);
			fprintf(out, "             %-12s%-25s%-12s%8d\n",
										x1, tp->descr, x2, tp->hit);
			x1 = "";
		}
		tp++;
	}
	fprintf(out, "==============================================================================\n");
	fprintf(out, "\n");
	
	return(TRUE);
}

int mbs_show(MBSDataIO *mbs, char *show_elem, FILE *out) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_show
// Purpose:        Show buffer element
// Arguments:      MBSDataIO * mbs    -- ptr as returned by mbs_open_file
//                 char * show_elem   -- buffer element to be shown
//                                       (one char out of "FBES")
//                 FILE * out         -- stream to send output to
//                                       (NULL = stdout)
// Results:        
// Exceptions:     
// Description:    Shows header/data information on stdout.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	void _mbs_output_error();
	void (*s)();

	if (!_mbs_check_dbase(mbs)) return(FALSE);

	if (out == NULL) out = stdout;

	switch (toupper(*show_elem)) {
		case 'F':	_mbs_show_fheader(mbs, out); return;
		case 'B':	s = (mbs->buftype)->show; break;
		case 'E':	s = (mbs->evttype)->show; break;
		case 'S':	s = (mbs->sevttype)->show; break;
		default:
			sprintf(loc_errbuf,
				"?ILLFMT-[_mbs_show]- %s: Illegal show specs - %s",
													mbs->device, show_elem);
			_mbs_output_error();
			return(FALSE);
	}
	if (s != NULL) (*s)(mbs, out);
	return(TRUE);
}

int mbs_set_show(MBSDataIO *mbs, char *show_elems, int redu, FILE *out) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_set_show
// Purpose:        Define buffer elements to be shown
// Arguments:      MBSDataIO * mbs    -- ptr as returned by mbs_open_file
//                 char * show_elems  -- buffer element(s) to be shown
//                                       (out of "FBES")
//                 int redu           -- reduction: show every redu-th
//                                                  element only
//                 FILE * out         -- stream to send output to
//                                       (NULL = stdout)
// Results:        
// Exceptions:     
// Description:    Defines buffer elements to be shown automatically
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register FILE *old;
	register char c;
	register int x;

	if (!_mbs_check_dbase(mbs)) return(FALSE);

	while (c = toupper(*show_elems++)) {
		switch (c) {
			case 'F':	x = MBS_X_FHEADER; break;
			case 'B':	x = MBS_X_BUFFER; break;
			case 'E':	x = MBS_X_EVENT; break;
			case 'S':	x = MBS_X_SUBEVENT; break;
			default:	x = -1; break;
		}
		if (x != -1) {
			old = mbs->show_elems[x].out;
			if (old != NULL && old != stdout && old != stderr) fclose(old);
			mbs->show_elems[x].out = out;
			mbs->show_elems[x].redu = redu;
		}
	}
	return(TRUE);
}

int mbs_set_dump(MBSDataIO *mbs, int count) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_set_dump
// Purpose:        Define buffers to be dumped
// Arguments:      MBSDataIO * mbs    -- ptr as returned by mbs_open_file
//                 int count          -- dump count
// Results:        
// Exceptions:     
// Description:    Defines every Nth buffer to be dumped to file.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	mbs->buf_to_be_dumped = count;
	return(TRUE);
}

int mbs_set_stream(MBSDataIO *mbs, int nstreams, int slow_down) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_set_stream
// Purpose:        Define stream profile
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 int nstreams     -- max # of streams to be processed
//                                     (0 = ad infinitum)
//                 int slow_down    -- # of secs to wait after each stream
//                                     (0 = don't wait)
// Results:        
// Exceptions:     
// Description:    Defines how to process stream data
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (!_mbs_check_active(mbs)) return(FALSE);
	if ((mbs->connection & (MBS_CTYPE_ASYNC | MBS_CTYPE_SYNC)) != 0) {
		mbs->max_streams = nstreams;
		mbs->slow_down = slow_down;
	} else {
		sprintf(loc_errbuf, "?NCSERV-[mbs_set_stream]- Not connected to MBS server");
		_mbs_output_error();
		return(FALSE);
	}
	return(TRUE);
}

int mbs_open_log(char *logfile) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_open_log
// Purpose:        Open a file for (error) logging
// Arguments:      char * logfile     -- log file, will be opened in append mode
// Results:        
// Exceptions:     
// Description:    Defines buffer elements to be shown automatically
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	FILE * f;

	if ((f = fopen(logfile, "a")) == NULL) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_open_log]- %s (%d)", sys_errlist[errno], errno);
		_mbs_output_error();
		return(FALSE);
	}
	strcpy(log_file, logfile);
	log_out = f;
	return(TRUE);
}

int mbs_open_med(char *medfile) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_open_med
// Purpose:        Open a file write raw event data
// Arguments:      char * medfile     -- med file
// Results:        
// Exceptions:     
// Description:    Opens a file to store MBS event data.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	FILE * f;

	if ((f = fopen(medfile, "w")) == NULL) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_open_med]- %s (%d)", sys_errlist[errno], errno);
		_mbs_output_error();
		return(FALSE);
	}
	strcpy(med_file, medfile);
	med_out = f;
	return(TRUE);
}

int mbs_close_med() {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_close_med
// Purpose:        Close med file
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    Closes med file if open.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (med_out) fclose(med_out);
	return(TRUE);
}

int mbs_open_lmd(char *lmdfile) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_open_lmd
// Purpose:        Open a file to dump lmd data
// Arguments:      char * lmdfile     -- lmd file
/ Results:        
// Exceptions:     
// Description:    Opens a file to store original MBS data (LMD format)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	FILE * f;

	if ((f = fopen(lmdfile, "w")) == NULL) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_open_lmd]- %s (%d)", sys_errlist[errno], errno);
		_mbs_output_error();
		return(FALSE);
	}
	strcpy(lmd_file, lmdfile);
	lmd_out = f;
	return(TRUE);
}

int mbs_close_lmd() {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_close_lmd
// Purpose:        Close lmd file
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    Closes lmd file if open.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (lmd_out) fclose(lmd_out);
	return(TRUE);
}

void mbs_pass_errors(char * errbuf) {
/*_________________________________________________________[C PUBLIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbs_pass_errors
// Purpose:        Define a external buffer for error messages
// Arguments:      errbuf  -- buffer to store error messages
//                            (NULL = output errors to stderr)
// Results:        --
// Exceptions:     
// Description:    Defines a remote buffer to store errors.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	rem_errbuf = errbuf;
}

void _mbs_show_fheader(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           	_mbs_show_fheader
// Purpose:        Output file header data to stdout
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        --
// Exceptions:     
// Description:    Decodes file header data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	s_filhe *fh;
	char text[MBS_L_STR];

	char *mbs_xfht();

	fh = mbs->hdr_data;

	if (out == NULL) out = stdout;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  FILE HEADER: %s, buf# %d", mbs->device, mbs->nof_buffers);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_filhe));
	fprintf(out, "  Length w/o header    : %d words\n", fh->filhe_dlen);
	fprintf(out, "  Type                 : File header [%d,%d]\n",
										fh->filhe_type, fh->filhe_subtype);
	fprintf(out, "  Fragment indicator   : ");
	if (fh->filhe_frag == 0x0) fprintf(out, "none\n");
	else if (fh->filhe_frag == 0x001) fprintf(out, "fragment at BEGIN of buffer\n");
	else if (fh->filhe_frag == 0x100) fprintf(out, "fragment at END of buffer\n");
	else fprintf(out, "fragments at BEGIN & END of buffer\n");

	fprintf(out, "  Used length of data  : %d words\n", fh->filhe_used);
	fprintf(out, "  Buffer number        : %d\n", fh->filhe_buf);
	fprintf(out, "  # of buffer elements : %d\n", fh->filhe_evt);

	fprintf(out, "  Tape label           : %s\n",
			mbs_xfht(text, fh->filhe_label_l, fh->filhe_label));
	fprintf(out, "  File name            : %s\n",
			mbs_xfht(text, fh->filhe_file_l, fh->filhe_file));
	fprintf(out, "  User name            : %s\n",
			mbs_xfht(text, fh->filhe_user_l, fh->filhe_user));
	fprintf(out, "  Creation date        : %s\n",
			mbs_xfht(text, 24, fh->filhe_time));
	fprintf(out, "  Run ID               : %s\n",
			mbs_xfht(text, fh->filhe_run_l, fh->filhe_run));
	fprintf(out, "  Experiment           : %s\n",
			mbs_xfht(text, fh->filhe_exp_l, fh->filhe_exp));
	fprintf(out, "------------------------------------------------------------------------------\n");

	if (fh->filhe_lines) {
		fprintf(out, "  Comments             :\n");
		for (i = 0; i < fh->filhe_lines; i++) {
			fprintf(out, "         %s\n",
				mbs_xfht(text, fh->s_strings[i].string_l,\
				                fh->s_strings[i].string));
		}
	}
}

void _mbs_show_bheader(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           	_mbs_show_bheader
// Purpose:        Output buffer header data to stdout
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        --
// Exceptions:     
// Description:    Decodes buffer header data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_bufhe *bh;

	char *mbs_xfht();

	if (out == NULL) out = stdout;

	bh = (mbs->poolpt)->data;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  BUFFER HEADER: %s, buf# %d (%d)", mbs->device, mbs->nof_buffers, mbs->cur_bufno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_bufhe));
	fprintf(out, "  Length w/o header    : %d words\n", bh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->buftype)->descr, bh->i_type, bh->i_subtype);

	fprintf(out, "  Fragment indicator   : ");
	if (bh->h_begin == 0) {
		if (bh->h_end == 0) fprintf(out, "none\n");
		else fprintf(out, "fragment at END of buffer\n");
	} else {
		if (bh->h_end == 0) fprintf(out, "fragment at BEGIN of buffer\n");
		else fprintf(out, "fragments at BEGIN & END of buffer\n");
	}

	fprintf(out, "  Used length of data  : %d words\n", bh->i_used);
	fprintf(out, "  Buffer number        : %d\n", bh->l_buf);
	fprintf(out, "  # of buffer elements : %d\n", bh->l_evt);
	fprintf(out, "  Length of last event : %d\n", bh->l_free[1]);
	fprintf(out, "------------------------------------------------------------------------------\n");
}

char *mbs_xfht(char *t, int l, char *s)

{
	memset(t, 0, MBS_L_STR);
	strncpy(t, s, l);
	return(t);
}

void _mbs_show_evhe_10_1(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_evhe_10_1
// Purpose:        Output current event header to stdout
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes event header data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_vehe *eh;

	if (out == NULL) out = stdout;

	eh = mbs->evt_data;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  EVENT HEADER: %s, buf# %d (%d), evt# %d", mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_vehe));
	fprintf(out, "  Data length          : %d words\n", eh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->evttype)->descr, eh->i_type, eh->i_subtype);
	fprintf(out, "  Trigger              : %d\n", eh->i_trigger);
	fprintf(out, "  Event counter        : %d\n", eh->l_count);
	fprintf(out, "------------------------------------------------------------------------------\n");
}

void _mbs_show_sev_10_1(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_sev_10_1
// Purpose:        Output subevent data (type [10,1], CAMAC)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes subevent data and outputs them to stdout
//                 See manual "GOOSY Buffer Structure" for details
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	int n;
	s_veshe *sh;
	unsigned short *dp;

	if (out == NULL) out = stdout;

	sh = mbs->sevtpt;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  SUBEVENT HEADER: %s, buf# %d (%d), evt# %d, sevt# %d",
					mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno, mbs->sevtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_veshe));
	fprintf(out, "  Data length          : %d words\n", sh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->sevttype)->descr, sh->i_type, sh->i_subtype);
	fprintf(out, "  Control              : %d\n", sh->h_control);
	fprintf(out, "  Subcrate             : %d\n", sh->h_subcrate);
	fprintf(out, "  Subevent/Proc ID     : %d\n", sh->i_procid);
	fprintf(out, "  # of data words      : %d\n", mbs->sevt_wc);
	fprintf(out, "------------------------------------------------------------------------------\n");
	fprintf(out, "  SUBEVENT DATA:");

	dp = (unsigned short *) mbs->sevt_data;
	n = 0;
	for (i = 0; i < mbs->sevtsiz; i++, dp++) {
		if (*dp) {
			if ((n % 5) == 0) fprintf(out, "\n");
			fprintf(out, "%6d:%8d", i, *dp);
			n++;
		}
	}
	fprintf(out, "\n------------------------------------------------------------------------------\n");
}

unsigned int *_mbs_unpack_sev_10_1(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_unpack_sev_10_1
// Purpose:        Unpack subevent (type [10,1] - CAMAC)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        unsigned int * sevtpt   -- pointer to subevent data
// Exceptions:     
// Description:    Unpacks subevent data of type [10,1] (CAMAC)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	s_veshe *sh;
	char *idp, *odp;
	unsigned short *sdp;
	MBSCamacDataSW *cp;
	int wc;
	int maxaddr, addrspace;

	if (mbs->sevt_wc > 0) return(mbs->sevt_data);

	sh = mbs->sevtpt;
	wc = (sh->l_dlen
			- (sizeof(s_veshe) - sizeof(s_evhe)) / sizeof(unsigned short)) / 2;
	mbs->sevt_wc = wc;
	idp = mbs->sevtpt + sizeof(s_veshe);
	bto_get_short(idp, idp, wc * 2, mbs->byte_order);

	cp = (MBSCamacDataSW *) idp;
	maxaddr = mbs->sevt_minwc;
	for (i = 0; i < wc; i++, cp++) {
		if (cp->subaddr > maxaddr) maxaddr = cp->subaddr;
	}

	addrspace = maxaddr + 1;

	if (addrspace <= mbs->sevtsiz) {
		if (mbs->sevt_data == NULL) mbs->sevt_data = calloc(addrspace, sizeof(unsigned short));
		else memset(mbs->sevt_data, 0, mbs->sevtsiz * sizeof(unsigned short));
	} else {
		if (mbs->sevt_data != NULL) free(mbs->sevt_data);
		mbs->sevt_data = calloc(addrspace, sizeof(unsigned short));
		mbs->sevtsiz = addrspace;
	}

	cp = (MBSCamacDataSW *) idp;
	for (i = 0; i < wc; i++, cp++) {
		odp = mbs->sevt_data 
			+ cp->subaddr * sizeof(unsigned short);
		sdp = (unsigned short *) odp;
		*sdp = cp->data;
	}
	return(mbs->sevt_data);
}

void _mbs_show_sev_10_11(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_sev_10_11
// Purpose:        Output subevent data (type [10,11], CAMAC w/o ids)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes subevent data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	s_veshe *sh;
	unsigned short *dp;

	if (out == NULL) out = stdout;

	sh = mbs->sevtpt;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  SUBEVENT HEADER: %s, buf# %d (%d), evt# %d, sevt# %d",
					mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno, mbs->sevtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_veshe));
	fprintf(out, "  Data length          : %d words\n", sh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->sevttype)->descr, sh->i_type, sh->i_subtype);
	fprintf(out, "  Control              : %d\n", sh->h_control);
	fprintf(out, "  Subcrate             : %d\n", sh->h_subcrate);
	fprintf(out, "  Subevent/Proc ID     : %d\n", sh->i_procid);
	fprintf(out, "  # of data words      : %d\n", mbs->sevt_wc);
	fprintf(out, "------------------------------------------------------------------------------\n");
	fprintf(out, "  SUBEVENT DATA:");

	dp = (unsigned short *) mbs->sevt_data;
	for (i = 0; i < mbs->sevt_wc; i++, dp++) {
		if ((i % 5) == 0) fprintf(out, "\n%6d: ", i + 1);
		fprintf(out, "%8d", *dp);
	}
	fprintf(out, "\n------------------------------------------------------------------------------\n");
}

unsigned int *_mbs_unpack_sev_10_11(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_unpack_sev_10_11
// Purpose:        Unpack subevent (type [10,11] - CAMAC w/o ids)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        unsigned int * sevtpt   -- pointer to subevent data
// Exceptions:     
// Description:    Unpacks subevent data of type [10,11] (CAMAC w/o ids)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_veshe *sh;
	char *dp;
	int wc;
	int mwc;

	if (mbs->sevt_wc > 0) return(mbs->sevt_data);

	sh = mbs->sevtpt;
	wc = (sh->l_dlen - (sizeof(s_veshe) - sizeof(s_evhe)) / sizeof(unsigned short));
	mbs->sevt_wc = wc;
	dp = mbs->sevtpt + sizeof(s_veshe);
	bto_get_short(dp, dp, wc, mbs->byte_order);

	mwc = mbs->sevt_minwc;
	if (mwc < wc) mwc = wc;
	if (mwc <= mbs->sevtsiz) {
		if (mbs->sevt_data == NULL) mbs->sevt_data = calloc(mwc, sizeof(unsigned short));
		else memset(mbs->sevt_data, 0, mbs->sevtsiz * sizeof(unsigned short));
	} else {
		if (mbs->sevt_data != NULL) free(mbs->sevt_data);
		mbs->sevt_data = calloc(mwc, sizeof(unsigned short));
		mbs->sevtsiz = mwc;
	}

	memcpy(mbs->sevt_data, dp, wc * sizeof(unsigned short));

	return(mbs->sevt_data);
}

unsigned int *_mbs_unpack_sev_raw(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_unpack_sev_raw
// Purpose:        Unpack subevent with raw data
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        unsigned int * sevtpt   -- pointer to subevent data
// Exceptions:     
// Description:    Unpacks raw data. No byte ordering will be done.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_veshe *sh;
	char *dp;
	int wc;
	int mwc;

	if (mbs->sevt_wc > 0) return(mbs->sevt_data);

	sh = mbs->sevtpt;
	wc = (sh->l_dlen - (sizeof(s_veshe) - sizeof(s_evhe)) / sizeof(unsigned short));
	mbs->sevt_wc = wc;
	dp = mbs->sevtpt + sizeof(s_veshe);

	mwc = mbs->sevt_minwc;
	if (mwc < wc) mwc = wc;
	if (mwc <= mbs->sevtsiz) {
		if (mbs->sevt_data == NULL) mbs->sevt_data = calloc(mwc, sizeof(unsigned short));
		else memset(mbs->sevt_data, 0, mbs->sevtsiz * sizeof(unsigned short));
	} else {
		if (mbs->sevt_data != NULL) free(mbs->sevt_data);
		mbs->sevt_data = calloc(mwc, sizeof(unsigned short));
		mbs->sevtsiz = mwc;
	}

	memcpy(mbs->sevt_data, dp, wc * sizeof(unsigned short));

	return(mbs->sevt_data);
}

void _mbs_show_sev_9000_1(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_sev_9000_1
// Purpose:        Output subevent data (type [9000,1], TimeStamp)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes subevent data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_veshe *sh;
	unsigned long *dp;
	char tstr[100];
	unsigned long sec;

	if (out == NULL) out = stdout;

	sh = mbs->sevtpt;

	dp = (unsigned long *) mbs->sevt_data;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  SUBEVENT HEADER: %s, buf# %d (%d), evt# %d, sevt# %d",
					mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno, mbs->sevtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_veshe));
	fprintf(out, "  Data length          : %d words\n", sh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->sevttype)->descr, sh->i_type, sh->i_subtype);
	fprintf(out, "  Clock resolution     : %ld nsecs\n", *dp++);
	sec = *dp++;
	fprintf(out, "  Seconds              : %ld\n", sec);
	fprintf(out, "  Nano seconds         : %ld\n", *dp++);
	strftime(tstr, 100, "%e-%b-%y %H:%M:%S", localtime(&sec));
	fprintf(out, "  Time stamp           : %s\n", tstr);
	fprintf(out, "------------------------------------------------------------------------------\n");
}

unsigned int *_mbs_unpack_sev_9000_X(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_unpack_sev_9000_X
// Purpose:        Unpack subevent (type [9000,X] - X=1:TimeStamp, X=2:DeadTime)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        unsigned int * sevtpt   -- pointer to subevent data
// Exceptions:     
// Description:    Unpacks subevent data of type [9000,X] (TimeStamp or DeadTime)
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_veshe *sh;
	char *dp;
	int wc, wc2;

	if (mbs->sevt_wc > 0) return(mbs->sevt_data);

	sh = mbs->sevtpt;
	wc = (sh->l_dlen
			- (sizeof(s_veshe) - sizeof(s_evhe)) / sizeof(unsigned short)) / 2;
	wc2 = wc * 2;
	mbs->sevt_wc = wc2;
	dp = mbs->sevtpt + sizeof(s_veshe);
	bto_get_long(dp, dp, wc, mbs->byte_order);

	if (wc2 <= mbs->sevtsiz) {
		if (mbs->sevt_data == NULL) mbs->sevt_data = calloc(wc, sizeof(unsigned short));
		else memset(mbs->sevt_data, 0, mbs->sevtsiz * sizeof(unsigned short));
	} else {
		if (mbs->sevt_data != NULL) free(mbs->sevt_data);
		mbs->sevt_data = calloc(wc, sizeof(unsigned long));
		mbs->sevtsiz = wc2;
	}

	memcpy(mbs->sevt_data, dp, wc * sizeof(unsigned long));

	return(mbs->sevt_data);
}

void _mbs_show_sev_9000_2(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_sev_9000_2
// Purpose:        Output subevent data (type [9000,2], DeadTime)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes subevent data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_veshe *sh;
	unsigned long *dp;
	char tstr[100];
	long sec;
	long dtevc;
	long scacon;
	float dtime;

	if (out == NULL) out = stdout;

	sh = mbs->sevtpt;

	dp = (unsigned long *) mbs->sevt_data;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  SUBEVENT HEADER: %s, buf# %d (%d), evt# %d, sevt# %d",
					mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno, mbs->sevtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_veshe));
	fprintf(out, "  Data length          : %d words\n", sh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->sevttype)->descr, sh->i_type, sh->i_subtype);
	fprintf(out, "  Clock resolution     : %ld nsecs\n", *dp++);
	sec = *dp++;
	fprintf(out, "  Seconds              : %ld\n", sec);
	fprintf(out, "  Nano seconds         : %ld\n", *dp++);
	strftime(tstr, 100, "%e-%b-%y %H:%M:%S", localtime(&sec));
	fprintf(out, "  Time stamp           : %s\n", tstr);
	fprintf(out, "  Events since start   : %ld\n", *dp++);
	dtevc = *dp++;
	fprintf(out, "  Events during interv : %ld\n", dtevc);
	scacon = *dp++;
	fprintf(out, "  Scaler contents      : %ld\n", scacon);
	dtime = (1. - (float) dtevc / (float) scacon) * 100.;
	fprintf(out, "  Dead time            : %f4.2 %\n", dtime);
	fprintf(out, "------------------------------------------------------------------------------\n");
}

void _mbs_show_sev_raw(MBSDataIO *mbs, FILE *out) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_show_sev_raw
// Purpose:        Output subevent raw data
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
//                 FILE * out       -- stream to send output to
// Results:        
// Exceptions:     
// Description:    Decodes subevent data and outputs them to stdout
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	s_veshe *sh;
	unsigned short *dp;

	if (out == NULL) out = stdout;

	sh = mbs->sevtpt;

	fprintf(out, "\n==============================================================================\n");
	fprintf(out, "  SUBEVENT HEADER: %s, buf# %d (%d), evt# %d, sevt# %d",
					mbs->device, mbs->nof_buffers, mbs->cur_bufno, mbs->evtno, mbs->sevtno);
	fprintf(out, "\n==============================================================================\n");

	fprintf(out, "  Header length        : %d bytes\n", sizeof(s_veshe));
	fprintf(out, "  Data length          : %d words\n", sh->l_dlen);
	fprintf(out, "  Type                 : %s [%d,%d]\n",
							(mbs->sevttype)->descr, sh->i_type, sh->i_subtype);
	fprintf(out, "  Control              : %d\n", sh->h_control);
	fprintf(out, "  Subcrate             : %d\n", sh->h_subcrate);
	fprintf(out, "  Subevent/Proc ID     : %d\n", sh->i_procid);
	fprintf(out, "  # of data words      : %d\n", mbs->sevt_wc);
	fprintf(out, "------------------------------------------------------------------------------\n");
	fprintf(out, "  SUBEVENT DATA:");

	dp = (unsigned short *) mbs->sevt_data;
	for (i = 0; i < mbs->sevt_wc; i++, dp++) {
		if ((i % 5) == 0) fprintf(out, "\n%6d: ", i + 1);
		fprintf(out, "%8d", *dp);
	}
	fprintf(out, "\n------------------------------------------------------------------------------\n");
}

unsigned int _mbs_convert_data(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_convert_data
// Purpose:        Test and convert data
// Arguments:      MBSDataIO * mbs    -- ptr as returned by mbs_open_file
// Results:        unsigned int type  -- buffer type
// Exceptions:     type = MBS_BTYPE_ERROR if error
// Description:    Tests byte ordering, converts data, and determines data
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	unsigned long bo_tag;
	unsigned int byte_order;
	unsigned int btype;
	s_bufhe *bh;

	void _mbs_output_error();
	unsigned int (*s)();

	MBSBufferElem *_mbs_check_type();

	bh = (s_bufhe *) (mbs->poolpt)->data;

	bo_tag = bh->l_free[0];
	if (bo_tag == 0 && mbs->nof_buffers == 1) {
		byte_order = BYTE_ORDER_1_TO_1;
		bto_get_long(&btype, &bh->i_subtype, 1, byte_order);
		if (btype != MBS_BTYPE_HEADER) {
			byte_order = BYTE_ORDER_REV;
			bto_get_long(&btype, &bh->i_subtype, 1, byte_order);
			if (btype != MBS_BTYPE_HEADER) {
				sprintf(loc_errbuf,
"?ILLFMT-[_mbs_convert_data]- %s (buf %d): Can't determine byte ordering - %#lx",
									mbs->device, mbs->nof_buffers, bh->l_free[0]);
				_mbs_output_error();
				mbs->buftype = &buffer_type_error;
				return(MBS_BTYPE_ABORT);
			}
		}
	} else if (bo_tag == 1) {
		byte_order = BYTE_ORDER_1_TO_1;
	} else {
		bto_get_long(&bo_tag, &bh->l_free[0], 1, BYTE_ORDER_REV);
		if (bo_tag == 1) {
			byte_order = BYTE_ORDER_REV;
		} else {
			sprintf(loc_errbuf,
	"?ILLFMT-[_mbs_convert_data]- %s (buf %d): Can't determine byte ordering - %#lx",
										mbs->device, mbs->nof_buffers, bh->l_free[0]);
			_mbs_output_error();
			mbs->buftype = &buffer_type_error;
			return(MBS_BTYPE_ABORT);
		}
	}

	mbs->byte_order = byte_order;

	bto_get_long(&btype, &bh->i_subtype, 1, byte_order);
	mbs->buftype = _mbs_check_type(btype, mbs->buftype, buffer_types);
	btype = (mbs->buftype)->type;
	if (btype == MBS_BTYPE_ERROR || btype == MBS_BTYPE_ABORT) {
		sprintf(loc_errbuf,
		"?ILLFMT-[_mbs_convert_data]- %s (buf %d): Not a legal buffer type - %#lx",
										mbs->device, mbs->nof_buffers, btype);
		_mbs_output_error();
		mbs->buftype = &buffer_type_error;
		return(MBS_BTYPE_ABORT);
	}

	s = (mbs->buftype)->convert;
	(*s)(mbs);

	(mbs->buftype)->hit++;

	return((mbs->buftype)->type);
}

void _mbs_type_error(MBSDataIO *mbs) {}

void _mbs_init_hit(MBSBufferElem *tlist) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_init_hit
// Purpose:        Initialize buffer type
// Arguments:      MBSBufferElem * tlist  -- list of types descriptions
// Results:        --
// Exceptions:     
// Description:    Initializes type counters
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	while (tlist->type != 0) {
		tlist->hit = 0;
		tlist++;
	}	
}

void _mbs_init_triggers() {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_init_triggers
// Purpose:        Initialize trigger list
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes triggers & trigger counters
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	MBSBufferElem * tp;

	tp = triggers;
	for (i = 0; i < MBS_N_TRIGGERS; i++, tp++) {
		tp->descr = calloc(MBS_L_NAME, sizeof(char));
		sprintf(tp->descr, "Trig %d", i);
		tp->hit = 0;
	}
	strcat(triggers[14].descr, " (Start)");
	strcat(triggers[15].descr, " (Stop)");
}

MBSBufferElem *_mbs_check_type(unsigned int btype, MBSBufferElem *ltdescr, MBSBufferElem *tlist) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_check_type
// Purpose:        Check buffer type
// Arguments:      unsigned int btype       -- buffer type to be tested
//                 MBSBufferElem * ltdescr  -- ptr to previously found type descr
//                 MBSBufferElem * tlist    -- list of types descriptions
// Results:        MBSBufferElem * btpt     -- ptr to type descr
// Exceptions:     NULL on error
// Description:    Tests buffer / event / subevent types.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	unsigned int t;
	if (ltdescr != NULL && (btype == ltdescr->type)) return(ltdescr);

	while (tlist->type != 0) {
		t = tlist->type;
		if (t == btype) return(tlist);
		tlist++;
	}	
	return(&buffer_type_error);
}

int _mbs_check_sequence(MBSDataIO *mbs, unsigned int type) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_check_sequence
// Purpose:        Check if data in sequence
// Arguments:      MbsDataIO * mbs          -- pointer to MBS data base
//                 unsigned int type        -- type: buffer or event
// Results:        TRUE/FALSE
// Exceptions:     FALSE if data not in order
// Description:    Tests if subsequent buffers/events have increasing numbers.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	s_bufhe *bh;
	s_vehe *eh;

	if (type == MBS_TY_BUFFER) {
		bh = (mbs->poolpt)->data;
		if ((mbs->bufno_mbs > 0) && (bh->l_buf != mbs->bufno_mbs + 1)) return(FALSE);
	} else if (type == MBS_TY_EVENT) {
		eh = mbs->evt_data;
		if ((mbs->evtno_mbs >= 0) && (eh->l_count != mbs->evtno_mbs + 1)) return(FALSE);
	}
	return(TRUE);
}

void _mbs_copy_fheader(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_copy_fheader
// Purpose:        Copy file header (swapped)
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Copies file header data from input buffer to hdr_data.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	register char *ipnt;
	s_filhe *bh, *fh;
	unsigned int bo;
	struct cv_string *cmt;

	bh = (s_filhe *) (mbs->poolpt)->data;
	fh = (s_filhe *) mbs->hdr_data;
	bo = mbs->byte_order;

	if (bo == BYTE_ORDER_1_TO_1) {
		memcpy(mbs->hdr_data, (mbs->poolpt)->data, mbs->bufsiz);
	} else {
		bto_get_short(&fh->filhe_tlen, bh, 6, bo);
		ipnt = bto_get_short(&fh->filhe_label_l, &bh->filhe_label_l, 1, bo);
		ipnt = bto_get_string(fh->filhe_label, ipnt, 30, bo);
		ipnt = bto_get_short(&fh->filhe_file_l, ipnt, 1, bo);
		ipnt = bto_get_string(fh->filhe_file, ipnt, 86, bo);
		ipnt = bto_get_short(&fh->filhe_user_l, ipnt, 1, bo);
		ipnt = bto_get_string(fh->filhe_user, ipnt, 30, bo);
		ipnt = bto_get_string(fh->filhe_time, ipnt, 24, bo);
		ipnt = bto_get_short(&fh->filhe_run_l, ipnt, 1, bo);
		ipnt = bto_get_string(fh->filhe_run, ipnt, 66, bo);
		ipnt = bto_get_short(&fh->filhe_exp_l, ipnt, 1, bo);
		ipnt = bto_get_string(fh->filhe_exp, ipnt, 66, bo);
		ipnt = bto_get_long(&fh->filhe_lines, ipnt, 1, bo);
		cmt = fh->s_strings;
		for (i = 0; i < fh->filhe_lines; i++, cmt++) {
			ipnt = bto_get_short(&cmt->string_l, ipnt, 1, bo);
			ipnt = bto_get_string(cmt->string, ipnt, 78, bo);
		}
	}
}

void _mbs_convert_bheader(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_convert_bheader
// Purpose:        Convert header data
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Converts header data according to byte order
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register char *ipnt;
	unsigned int bo;
	s_bufhe *bh;

	bo = mbs->byte_order;
	if (bo == BYTE_ORDER_1_TO_1) return;

	bh = (s_bufhe *) (mbs->poolpt)->data;

	ipnt = bto_get_long(&bh->l_dlen, &bh->l_dlen, 1, bo);
	ipnt = bto_get_short(&bh->i_subtype, ipnt, 2, bo);
	ipnt = bto_get_short(&bh->h_begin, ipnt, 1, bo);
	ipnt = bto_get_short(&bh->i_used, ipnt, 1, bo);
	ipnt = bto_get_long(&bh->l_buf, ipnt, 9, bo);
}

void _mbs_convert_eheader(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_convert_eheader
// Purpose:        Convert event header data
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Converts event header data.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register char *ipnt;
	unsigned int bo;
	s_vehe *eh;

	bo = mbs->byte_order;
	if (bo == BYTE_ORDER_1_TO_1) return;

	eh = (s_vehe *) mbs->evt_data;

	ipnt = bto_get_long(&eh->l_dlen, &eh->l_dlen, 1, bo);
	ipnt = bto_get_short(&eh->i_subtype, ipnt, 2, bo);
	ipnt = bto_get_short(&eh->i_trigger, ipnt, 2, bo);
	ipnt = bto_get_long(&eh->l_count, ipnt, 1, bo);
}

void _mbs_convert_sheader(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_convert_sev_10_1
// Purpose:        Convert subevent data
// Arguments:      MBSDataIO * mbs  -- ptr as returned by mbs_open_file
// Results:        --
// Exceptions:     
// Description:    Converts subevent header data.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register char *ipnt;
	unsigned int bo;
	s_veshe *seh;

	bo = mbs->byte_order;
	if (bo == BYTE_ORDER_1_TO_1) return;

	seh = (s_veshe *) mbs->sevtpt;

	ipnt = bto_get_long(&seh->l_dlen, &seh->l_dlen, 1, bo);
	ipnt = bto_get_short(&seh->i_subtype, ipnt, 2, bo);
	ipnt = bto_get_short(&seh->h_control, ipnt, 2, bo);
	mbs->sevt_otype = (seh->i_subtype << 16) | seh->i_type;
	mbs->sevt_id = seh->i_procid;
}

int _mbs_check_active(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_check_active
// Purpose:        Check if a given mbs struct is active
// Arguments:      MBSDataIO * mbs  -- pointer to mbs struct
// Results:        0/1
// Exceptions:     
// Description:    	Tests the addr given by mbs and outputs an error message
//                  it isn't a valid mbs i/o struct.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (mbs == NULL || mbs->fileno == -1) {
		sprintf(loc_errbuf, "?MBSNAC-[mbs_check_active]- MBSIO not active");
		_mbs_output_error();
		return(FALSE);
	} else if (strncmp(mbs->id, MBS_ID_WORD, 15) != 0) {
		sprintf(loc_errbuf, "?NMBSIO-[mbs_check_active]- Not a MBSIO struct");
		_mbs_output_error();
		return(FALSE);
	} else {
		return(TRUE);
	}
}

int _mbs_check_dbase(MBSDataIO *mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_check_dbase
// Purpose:        Check if a given mbs struct has valid data
// Arguments:      MBSDataIO * mbs  -- pointer to mbs struct
// Results:        0/1
// Exceptions:     
// Description:    	Tests the addr given by mbs and outputs an error message
//                  it isn't a valid mbs i/o struct.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	if (mbs == NULL) {
		sprintf(loc_errbuf, "?MBSDNV-[mbs_check_dbase]- data base not valid");
		_mbs_output_error();
		return(FALSE);
	} else if (strncmp(mbs->id, MBS_ID_WORD, 15) != 0) {
		sprintf(loc_errbuf, "?NMBSIO-[mbs_check_dbase]- Not a MBSIO struct");
		_mbs_output_error();
		return(FALSE);
	} else {
		return(TRUE);
	}
}

void _mbs_output_error() {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_output_error
// Purpose:        Output error message
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Outputs an error message, either to stderr or to rem_errbuf.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	char datestr[MBS_L_STR];
	time_t now;

	if (rem_errbuf == NULL) {
		fprintf(stderr, "%s\n", loc_errbuf);
	} else {
		strcpy(rem_errbuf, loc_errbuf);
	}
	if (log_out) {
		time(&now);
		strftime(datestr, MBS_L_STR, "%e-%b-%y %H:%M:%S", localtime(&now));
		fprintf(log_out, "%-18s: %s\n", datestr, loc_errbuf);
	}
}

void _mbs_output_log() {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_output_log
// Purpose:        Output message to logfile
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Outputs a message to the log channel.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	char datestr[MBS_L_STR];
	time_t now;

	if (log_out) {
		now = time(NULL);
		strftime(datestr, MBS_L_STR, "%e-%b-%y %H:%M:%S", localtime(&now));
		fprintf(log_out, "%-18s: %s\n", datestr, loc_logbuf);
	}
}

int _mbs_connect_to_server(char * host, unsigned int server_type) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_connect_to_server
// Purpose:        Connect to MBS server
// Arguments:      char * host              -- host name to connect to
//                 unsigned int server_type -- server type (MBS_CTYPE_xxxx)
// Results:        int fildes               -- file descriptor or -1 if error
// Exceptions:     -1 if error
// Description:    Connects to a MBS port.
// Author:         M. Münch Hades/E12
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	int s;
	int len;
	struct sockaddr_in sa;
	struct hostent *hp;
	struct servent *sp;
	int port=0;

	s = 0;
	len = 0;

	hp = NULL;
	sp = NULL;

	switch (server_type) {
		case MBS_CTYPE_SYNC:
			port = 6000;
			break;
		case MBS_CTYPE_ASYNC:
			port = 6002;
			break;
		default:
			sprintf(loc_errbuf,
				"?UKSRVTP-[mbs_connect_to_server]- Unknown server type: %#x", server_type);
			return(-1);
	}

	if ((hp = gethostbyname(host)) == NULL) {
		sprintf(loc_errbuf, "?UKHOSTN-[mbs_connect_to_server]- Host name unknown: %s", host);
		return(-1);
	}
	memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons(port);
	if((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_connect_to_server]- %s (%d)", sys_errlist[errno], errno);
		return(-1);
	}
	if(connect(s, &sa, sizeof sa) < 0) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_connect_to_server]- %s (%d)", sys_errlist[errno], errno);
		return(-1);
	}
	return(s);
}

MBSServerInfo * _mbs_read_server_info(int fildes, MBSServerInfo *info) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_read_server_info
// Purpose:        Read info block from MBS server
// Arguments:      int fildes            -- connection's file descriptor
//                 MBSServerInfo * info  -- info block
// Results:        MBSServerInfo * info  -- addr of info block or NULL if error.
// Exceptions:     NULL on error
// Description:    Reads the info block.
// Author:         M. Münch Hades/E12
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	long infoWord;
	long swInfoWord;

	errno = 0;

	read(fildes, &infoWord, sizeof(infoWord));
	if (infoWord == 1) {
		info->is_swapped = FALSE;
	} else {
		bto_get_long(&swInfoWord, &infoWord, 1, BYTE_ORDER_REV);
		if (swInfoWord == 1) {
			info->is_swapped = TRUE;
		} else {
			sprintf(loc_errbuf,
				"?ILINFB-[mbs_read_server_info]- Illegal info data (version mismatch?)");
			return(NULL);
		}
	}

	read(fildes, &infoWord, sizeof(infoWord));
	if (info->is_swapped) {
		bto_get_long(&swInfoWord, &infoWord, 1, BYTE_ORDER_REV);
		info->buf_size = swInfoWord;
	} else {
		info->buf_size = infoWord;
	}

	read(fildes, &infoWord, sizeof(infoWord));
	if (info->is_swapped) {
		bto_get_long(&swInfoWord, &infoWord, 1, BYTE_ORDER_REV);
		info->buf_p_stream = swInfoWord;
	} else {
		info->buf_p_stream = infoWord;
	}

	read(fildes, &infoWord, sizeof(infoWord));
	if (info->is_swapped) {
		bto_get_long(&swInfoWord, &infoWord, 1, BYTE_ORDER_REV);
		info->nof_streams = swInfoWord;
	} else {
		info->nof_streams = infoWord;
	}
	
	printf("mbsio: ServerInfo >> buffer size: %d, bufs per stream: %d, number of streams: %d\n",
			info->buf_size,
			info->buf_p_stream,
			info->nof_streams);
	if (errno != 0) {
		sprintf(loc_errbuf, "?SYSERR-[mbs_read_server_info]- %s (%d)", sys_errlist[errno], errno);
		return(NULL);
	}
	return(info);
}

int _mbs_read_stream(int fildes, char * buf, MBSServerInfo *info) {
/*_______________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_read_stream
// Purpose:        Read data from remote stream
// Arguments:      int fildes           -- server's file descriptor
//                 char * buf           -- addr where to put the data
//                 MBSServerINfo * info -- server's info block
// Results:        int bytes_read       -- number of bytes read by this request
// Exceptions:     
// Description:    Reads a buffer from remote stream.
// Author:         M. Münch Hades/E12
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	int nof_read, nof_tot_read;

	nof_read = 0;
	nof_tot_read = 0;

	while (nof_tot_read < info->buf_size &&
			(nof_read = read(fildes, buf + nof_tot_read, info->buf_size - nof_tot_read)) > 0) {
		nof_tot_read += nof_read;
	}

	if (nof_read < 0) return(nof_read);
	if (lmd_out) fwrite(buf, 1, nof_tot_read, lmd_out);
	return(nof_tot_read);
}

int _mbs_request_stream(int fildes) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_request_stream
// Purpose:        Request a stream from server
// Arguments:      int fildes  -- server's file descriptor
// Results:        0/-1
// Exceptions:     
// Description:    Sends a request to server.
// Author:         M. Münch Hades/E12
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	return(write(fildes, "            ", 12));
}

int _mbs_disconnect_from_server(int fildes, unsigned int server_type) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_disconnect_from_server
// Purpose:        Disconnect from MBS server
// Arguments:      int fildes               -- server's file descriptor
//                 unsigned int server_type -- server type (MBS_CTYPE_xxxx)
// Results:        0/1
// Exceptions:     
// Description:    Disconnects from current MBS port.
// Author:         M. Münch Hades/E12
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	switch (server_type) {
		case MBS_CTYPE_SYNC:
			break;
		case MBS_CTYPE_ASYNC:
			_mbs_request_stream(fildes);
			break;
	}
	shutdown(fildes, 2);
	close(fildes);
	return(TRUE);
}

void _mbs_init_pool(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_init_pool
// Purpose:        Initialize buffer pool
// Arguments:      MBSDataIO * mbs    -- mbs data base
// Results:        
// Exceptions:     
// Description:    Initializes buffer numbers and data pointers in the
//                 buffer pool.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	register MBSBufferPool * bpp;

	bpp = mbs->buf_pool;
	for (i = 0; i < MBS_N_BUFFERS; i++, bpp++) {
		bpp->bufno_mbs = -1;
		bpp->data = NULL;
	}
	mbs->poolpt = NULL;
	mbs->buf_oo_phase = 0;
}

void _mbs_free_pool(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_free_pool
// Purpose:        Free memory allocated for the buffer pool
// Arguments:      MBSDataIO * mbs    -- mbs data base
// Results:        
// Exceptions:     
// Description:    Frees any memory allocated for the buffer pool.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	register MBSBufferPool * bpp;

	bpp = mbs->buf_pool;
	for (i = 0; i < MBS_N_BUFFERS; i++, bpp++) {
		if (bpp->data != NULL) free(bpp->data);
	}
}

MBSBufferPool * _mbs_get_pool_pointer(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_get_pool_pointer
// Purpose:        Find an empty place in the buffer pool
// Arguments:      MBSDataIO * mbs       -- mbs data base
// Results:        MBSBufferPool * bpp   -- pointer to empty buffer
// Exceptions:     Returns NULL if all buffers occupied
// Description:    Searches for an empty position in the buffer pool.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	register MBSBufferPool * bpp;

	bpp = mbs->buf_pool;
	for (i = 0; i < MBS_N_BUFFERS; i++, bpp++) {
		if (bpp->bufno_mbs == -1) {
			if (bpp->data == NULL) {
				bpp->data = calloc(1, mbs->bufsiz);
				if (bpp->data == NULL)
				{
					sprintf(loc_errbuf, 
						"?ALLOC-[_mbs_get_pool_pointer]- %s: Can't allocate internal buffer",
														mbs->device);
					_mbs_output_error();
					return(NULL);
				}
			}
			return(bpp);
		}
	}
	sprintf(loc_errbuf, 
		"?NOBUF-[_mbs_get_pool_pointer]- %s: Can't find empty slot in buffer pool",
														mbs->device);
	_mbs_output_error();
	return(NULL);
}


void _mbs_store_bufno(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_store_bufno
// Purpose:        Store current buffer number in pool header
// Arguments:      MBSDataIO * mbs       -- mbs data base
// Results:        
// Exceptions:     
// Description:    Extracts buffer number from MBS data and stores it in
//                 the pool header.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	MBSBufferPool * bpp;

	bpp = mbs->poolpt;
	bpp->bufno_mbs = ((s_bufhe *) bpp->data)->l_buf;
}

MBSBufferPool * _mbs_find_subseq_buffer(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_find_subseq_buffer
// Purpose:        Search for next buffer in the pool
// Arguments:      MBSDataIO * mbs       -- mbs data base
// Results:        MBSBufferPool * bpp   -- pointer to selected buffer
// Exceptions:     Returns NULL if buffer not found
// Description:    Inspects the buffer pool to find a buffer with the right
//                 buffer number = last+1.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register int i;
	register MBSBufferPool * bpp;
	int bufno;

	if ((bufno = mbs->bufno_mbs) <= 0) return(NULL);
	bufno++;

	bpp = mbs->buf_pool;
	for (i = 0; i < MBS_N_BUFFERS; i++, bpp++) {
		if (bpp->bufno_mbs == bufno) return(bpp);
	}
	return(NULL);
}

void _mbs_dump_buffer(MBSDataIO * mbs) {
/*________________________________________________________[C PRIVATE FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _mbs_dump_buffer
// Purpose:        Dump buffer data
// Arguments:      MBSDataIO * mbs       -- mbs data base
// Results:        
// Exceptions:     
// Description:    Opens a file named "XXX_buf_NNN.dmp"
//                 XXX is the device and NNN is the buffer number.
//                 Then dumps bufsiz data to this file.
// Keywords:       
/////////////////////////////////////////////////////////////////////////// */

	register FILE * f;
	char fname[MBS_L_STR];

	sprintf(fname, "%s_buf_%d.dmp", mbs->device, mbs->cur_bufno);
	if ((f = fopen(fname, "w")) != NULL) {
		sprintf(loc_logbuf,
			"%%BUFDMP-[_mbs_dump_buffers]- Dumping buffer #%d (%d bytes) to file \"%s\"",
									mbs->cur_bufno, mbs->bufsiz, fname);
		_mbs_output_log();
		fwrite(mbs->bufpt, mbs->bufsiz, 1, f);
		fclose(f);
	}
}
