/********************************************************************
 * ptrevoldcomm.h
 * requires include of ptrevmbsdef.h in advance
 * communication structures multi-threaded remote event server - old
 *    PAW clients
 * subset of structures and definitions needed here
 *    (no support for filtering)
 *
 * created 22. 9.1999, Horst Goeringer
 ********************************************************************
 * 28.10.1999, H.G.: separate structure for info set by ptData
 * 20.11.2001, H.G.: add CLNT__SMALLBUF
 ********************************************************************
 */

/* definitions needed from clnt_buf_def.h */
#define CLNT__SMALLBUF    512           /* size of smallest buffer */
#define CLNT__MSGLEN      256            /* size of message string */
#define CLNT__OUTBUFHEAD  344  /* size of header before MBS buffer */
#define GPS__OUTBUFSIZ  32768                /* size of MBS buffer */

/* definitions needed from gps_sc_def.h */
#define GPS__ENV_TESTBIT 0x00000001

#ifdef BIG__ENDIAN
#define GPS__ENV_ENDIAN  0xFFFFFFFF
#endif
#ifdef LITTLE__ENDIAN
#define GPS__ENV_ENDIAN  0x0
#endif

/*
 ********* PAW client requests events from server (404 byte) ********
 **************** first part of s_clnt_filter.h *********************
 */

typedef struct
{
   unsigned long      l_testbit;       /* bit pattern               */
   unsigned long      l_endian;        /* endian of sender          */
   long               l_numb_of_evt;   /* number of events to send  */
   long               l_sample_rate;   /* flt match sample rate     */
   long               l_flush_rate;    /* buffer flushing rate [sec]*/
   long               l_filter[96];    /* for filters (unused)      */
                                 /* 96 = GPS__MAXFLT * sizeof(long) */
} srevOldComm;

/*
 ************ header ptServ sends to PAW clients (344 byte) *********
 ******************** first part of s_clntbuf.h *********************
 */

typedef struct
{
   /* total length is CLNT__OUTBUFHEAD = 344 */
   unsigned long   l_testbit;         /* bit pattern                */
   unsigned long   l_endian;          /* endian of sender           */
   /* length of all following data is CLNT__BUFHEAD = 336 */
   long            l_dlen;            /* data length [bytes]        */
   long            l_free;            /* free length [bytes]        */
   long            l_events;          /* no. of events in buffer    */
   long            l_maxbufsiz;       /* maximum buffer size        */
   long            l_bytestosnd;      /* sent bytes                 */
   long            l_numbuftosnd;     /* type of buffer to send (1,2)*/
   long            l_client_con;      /* currently connect clients  */
   long  unsigned  l_buffertype;      /* set bit 0: data   ( | 1 )  */
                                      /*         1: msg    ( | 2 )  */
                                      /*         2: flush  ( | 4 )  */
                                      /*         3: last   ( | 8 )  */
                                      /*         4: 1stBUF ( |16 )  */
                                      /* (inclusive, mixing allowed)*/
   long  unsigned  l_msgtyp;          /* I:1 W:2 E:4 F:8 (exclusive)*/
   char            c_message[CLNT__MSGLEN]; /* error msg and other  */
   /* part of s_control structure: */
   long unsigned  l_inbuf_read_cnt;   /* count read buffers         */
   long unsigned  l_inbuf_rdok_cnt;   /* read buffers that are ok   */
   long unsigned  l_inbuf_skip_cnt;   /* count skipped buffers      */
   long unsigned  l_inbuf_byrd_cnt;   /* count read bytes           */
   long unsigned  l_inbuf_proc_cnt;   /* count processed buffers    */
   long unsigned  l_inbuf_prev_cnt;   /* count processed events     */
   /* part of s_client structure: */
   long unsigned  l_clntoutbuf_sdev; /* sent events                 */
   long unsigned  l_clntoutbuf_sdby; /* sent bytes                  */
   long unsigned  l_clntoutbuf_sdbf; /* sent buffers                */
   long unsigned  l_clntoutbuf_prev; /* sum of processed events     */
   long unsigned  l_clntoutbuf_fltm; /* filter matched on events    */

} srevOldHead;

/*
 ************* part of srevOldHead set by ptData (24 byte) **********
 ******************** part of s_control structure *******************
 */

typedef struct
{
   long unsigned  l_inbuf_read_cnt;   /* count read buffers         */
   long unsigned  l_inbuf_rdok_cnt;   /* read buffers that are ok   */
   long unsigned  l_inbuf_skip_cnt;   /* count skipped buffers      */
   long unsigned  l_inbuf_byrd_cnt;   /* count read bytes           */
   long unsigned  l_inbuf_proc_cnt;   /* count processed buffers    */
   long unsigned  l_inbuf_prev_cnt;   /* count processed events     */
} srevOldHeadData;

/*
 *************** acknowledge buffer sent by PAW client **************
 ********************* after received buffer ************************
 */

typedef struct
{
   long          l_ack_buf;        /* read client buffer received  */
   long          l_ack_bytes;        /* read client bytes received */
   long unsigned l_clnt_sts;      /* client sts 1: ok, 9 :last buf */

} srevOldAckn;

