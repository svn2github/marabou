/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany  
 * created  1. 2.1999 by Horst Goeringer
 *********************************************************************
 * mrevserv.c
 * multi-threaded remote event server for MBS: port no. 6003
 *
 * req. argument 1: node name data acquisition (required)
 * opt. argument 2: port number server on data acquisition (def.: 6002)
 * opt. argument 3: trials to connect to stream server
 *                   -1: only one trial
 *                    0: try until success (def.)
 *                  t>0: try at most for about t sec
 * opt. argument 4: reduction factor buffers (def.: all in stream)
 *                  (not yet implemented)
 * opt. argument 5: max no. of simultaneous clients (def.: 20)
 * opt. argument 6: max wait time for next buffer (def.: 120 s)
 * opt. argument 7: server mode
 *                  0: input from data acquisition (def.)
 *                  1: input from data acqu., write buffers to file
 *                     (not yet implemented)
 *                  2: input from file (written with (1), from acqu.)
 *                  3: input from lmd file (with file header)
 *                     (not yet implemented)
 *                  4: generate random events (routines M. Hemberger)
 *                  5: generate random events with specified delay   
 *                  9: benchmark mode without clients:
 *                     measure data rate from stream server
 *                  10+i: verbose (debug mode)
 * server mode 1, 11:
 *    opt. argument 8: output file name
 * server mode 2, 3, 4, 12, 13, 14:
 *    opt. argument 8: input file name
 * server mode 5, 15:
 *    opt. argument 8: input file name
 *    opt. argument 9: delay for each even buffer no. (def.: 20 s)
 * server mode 9, 19:
 *    opt. argument 8: max no of MBS buffers for benchmark (def.: 24)
 *
 *******************************************************************
 *  2. 6. 1999, H.G.: acknowledgement introduced
 * 24. 6. 1999, H.G.: DAQ benchmark introduced
 * 21. 7. 1999, H.G.: notify clients if no events available
 * 14. 9. 1999, H.G.: generate random events
 * 22. 9. 1999, H.G.: serve also old PAW clients
 * 15.11. 1999, H.G.: provide always correct byte order to clients
 * 26.11. 1999, H.G.: renamed from ptrevserv to mrevserv
 * 14.12. 1999, H.G.: client specific buffer flush time old clients
 *  3. 3. 2000, H.G.: change port no.: 6013 -> 6003
 *                    meaning of iDaqTry for rconnect same as here
 *                    rename iExit -> imySigS
 * 14. 3. 2000, H.G.: create threads detachable
 * 22. 5. 2000, H.G.: enable simulation of large events > 16 kB
 * 10. 5. 2001, H.G.: connect to transport, if port 6000 specified
 * 26. 7. 2001, H.G.: allow continuous stream of events
 * 10.12. 2001, H.G.: built events of any length from fragments
 *******************************************************************
 */

#include <pthread.h>                    /* should be first include */
#include <stdio.h>
#include <stdlib.h>               
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <socket.h>
#include <sys/time.h>
   
#define PT_DEF NULL

#include <netinet/in.h>

#include "ptrevlock.h"                    /* resources to be locked */
#include "ptrevclient.h"                /* client control structure */
#include "ptrevcomm.h"               /* client communication buffer */
#include "ptrevent.h"                         /* entry declarations */
#include "ptrevmbsdef.h"                     /* MBS data structures */
#include "s_str_info.h"                   /* info header MBS stream */
#include "ptrevoldcomm.h"         /* structures for old PAW clients */

/* available in other files via "extern" */
int iDebug = 0;                                      /* debug level */
int iTimeOut = 0;                   /* wait time client after CTL C */
int imySigS = 0;           /* for rconnect: if = 1, CTL C specified */ 
int iTimeBuf = 0;      /* if over without new buffer, notify client */

/* arguments passed */
static char cAcqNode[128] = "";        /* node name data acquisition */
static int iAcqPort = 0;  /* port number server on data acquisition */
static int iDaqTry = 0;        /* specifies how to reconnect to DAQ */
static int iBufReduc = 0;     /* reduction factor buffers (0: all)  */ 
static int iClientMax = 0;    /* max no. of simult. clients allowed */
/* int iTimeBuf = 0;                        see above (def.: 120 s) */
static int iServMode = 0;                            /* server mode */ 
static char cAcqFile[64] = "";             /* file with DAQ buffers */
static int iBufDelay = 0;
                     /* delay for even-numbered buffers (def.: 30s) */

/* some global MBS parameters fixed here */
static int iTransport = 0;
  /* =1: input from transport (no request for data, get all buffers */
static int igenBufSize = 40000; /* generated MBS buffer size (byte) */
/* static int iLargeEvt = 40000; */
   /* >0: simulate large events increased by iLargeEvt 2-byte-words */
static int iLargeEvt = 0;

static int iBufHead = sizeof(sMbsBufHead);
                                   /* size MBS buffer header (byte) */
static int iBufHeadHW;         /* half words (2 byte) buffer header */
static int iBufHeadPar;            /* params (4 byte) buffer header */

static int iEvtHead = 8;            /* size MBS event header (byte) */
static int iEvtHeadHW;          /* half words (2 byte) event header */
static int iEvtHeadPar;             /* params (4 byte) event header */

/* stream server data */
static int inewStr = 1;                  /* initial: get new stream */
static int iStrSocket = 0;                  /* socket stream server */
static int iStrMode = 0; /* =1: stream data, iServMode = 0, 1, 2, 9 */

/* stream buffer data */
static struct s_str_info sStrInfo;
static int iStrSize;               /* size of stream buffer (bytes) */
static int iStrSizek;             /* size of stream buffer (kbytes) */
static int iStrNo = 0;                    /* no. of streams handled */
static int iStrBufNo = 0; /* current buffer number locked in stream */
static int iStrBufMax;       /* no. of MBS  buffers in input stream */
static int iStrBufOut = 0;  /* no. of MBS  buffers in output stream */
static int iStrSwap = 0;        /* if > 0: swap stream server infos */
static int iStrInfoSize;       /* size of stream server info buffer */
static int iStrMerge = 0;      /* if > 0: stream with merged events */
 
/* MBS buffer header data (10.1) */   
static sMbsBufHead *pBufHead;
                   /* current MBS buffer header readable for server */
static int imbsFragEnd = 0;  /* fragment at begin of current buffer */
static int imbsFragBegin = 0;  /* fragment at end of current buffer */
static int imbsFragBeginOld;      /* fragment at end of prev buffer */

static srevOldHeadData sOldHeadData, *pOldHeadData;
     /* buffer header for old client: global part written by ptData */

/* for DAQ benchmark mode */
static time_t labsTimei, labsTimeo, labsTimex;     /* absolut times */

/* for generated events */
static int iRandomMode = 0;
static FILE *fIniFile;       /* file with initialization parameters */
static char cIniFile[64] =                     /* default file name */
   "/home/rg/marabou/GSI/EventServer/srv/mbs-random.ini"; 
static int igenSE;                       /* no. of subevents (1..2) */
static int igenPar;            /* no. of subevent parameters (1..5) */
static int igenMode;         /* =1: parameters SE1 = parameters SE2 */
                            /* =2: parameters SE1 != parameters SE2 */
static int igenFunc;      /* selects random function (not yet used) */

/* other global parameters */
static FILE *fAcqFile;        /* file with data acquisition buffers */
static int iEOF = 0;                  /* > 0, file input: EOF found */
static int iBufMax;        /* for DAQ benchmark: no. of MBS buffers */
static int iBufNoIn = 0;          /* no. of buffers in input stream */
static int iBufNoOut = 0;        /* no. of buffers in output stream */
static int iBufNoLock = 0;     /* buffer number (handled by ptdata) */

static int iOutBufMax = 0;   /* max no. of buffers in output stream */
static int iBuffer = 131072;       /* total size data buffer (byte) */
static char *pBuffer;                 /* total data buffer (stream) */
static char *pBufferOut;     /* total data buffer (expanded events) */
static char *pBufferSwap;     /* swapped total data buffer (stream) */
static int *piBuffer;                 /* total data buffer (stream) */
static int *piBufferOut;     /* total data buffer (expanded events) */
static int *piBufferSwap;     /* swapped total data buffer (stream) */

static int iBufSize = 0;/* MBS buffersize (byte) from stream server */
static int iBufSizeOut;        /* size of current MBS buffer (byte) */
static char *pcBufMbsIn;              /* MBS buffer in input stream */
static char *pcBufMbsOut;            /* MBS buffer in output stream */
static char *pcBufMbsCur;           /* current MBS buffer in stream */
static char *pcBufMbsCurSwap;                    /* swapped version */

static int iEventBuf = 0;        /* max events per buffer (0: all)  */
static int iEvtNo = 0;                      /* running event number */ 

static int ichar = sizeof(char);
static int ishort = sizeof(short);
static int iint = sizeof(int);
time_t t0;

static int iCommSizeAll = COMMSIZE; 
                     /* max size comm. buffer (old and new clients) */
static int iCommSize = sizeof(srevComm);              /* new client */ 

static int iInfoSizeAll = INFOALL;
    /* max size info buffers (old and new clients) and header (old) */
static int iInfoSize = sizeof(srevInfo);    /* info size new client */
static int iOldHead = HEADOLD;            /* header size old client */

static int iAcknSize = sizeof(srevOldAckn);
                          /* size acknowledgement buffer old client */

static int iListenSocket = 0;
struct sockaddr_in sAddr;
int iaddrSize = sizeof(struct sockaddr_in);

static int iClientAll = 0;                  /* total no. of clients */ 
static int iClientAct = 0;      /* current no. of connected clients */ 

pthread_t ptListen;
    /* thread listening for new clients and creating server threads */
void *ptrevListen(void *);                /* listen thread function */

pthread_t ptData;                 /* thread providing event buffers */
void *ptrevData(void *);                    /* data thread function */

pthread_t ptServ;                        /* threads serving clients */
void *ptrevServ(void *);                  /* server thread function */

/* event buffer read/write lock structure */
ptBufLock_t sptBufLock, *ptBufLock;    

/* for exit main() */
pthread_cond_t condExit = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexExit = PTHREAD_MUTEX_INITIALIZER;

void ptrevExit();

main( int argc, char **argv ) 
{
   int iPort = 6003;                           /* local port number */
   char cHost[128] = "";                          /* local host name */
   char cModule[32] = "revserver";                   /* module name */
   char cMsg[128] = "";
   char cTimeStamp[256] = "";

   int iCloseMode = 2;/* for rclose(): shutdown level; <0: no close */
   int iRC = 0, ii, jj, ipar;
   int iError = 0;
   int iSize;

   void *pv;
   void *pvStatus;

   signal(SIGINT, ptrevExit);               /* from now catch Ctl C */

   if (--argc > 0) strcpy(cAcqNode, argv[1]);
   if (strlen(cAcqNode) == 0)
   {
      printf("-E- please specify data acquisistion node\n");
      return 4;
   }

   if (--argc > 0)
   {
      iRC = sscanf(argv[2], "%d", &iAcqPort);
      if (iRC != 1)
      {
         printf("-E- invalid port number of data acquisition: %s\n",
                argv[2]);
         return 4;
      }
   }
   if (iAcqPort < 0)
   {
      printf("-E- invalid port number of data acquisition: %d\n",
             iAcqPort);
      return 4;
   }
   if (iAcqPort == 0) iAcqPort = 6002;

   if (--argc > 0)
   {
      iRC = sscanf(argv[3], "%d", &iDaqTry);
      if (iRC != 1)
      {
         printf("-E- invalid specification how to connect to stream server: %s\n",
                argv[3]);
         printf("     -1: only one trial\n");
         printf("      0: try until success (default)\n");
         printf("    t>0: try at most for about t sec\n");
         return 4;
      }
   }

   if (--argc > 0)
   {
      iRC = sscanf(argv[4], "%d", &iBufReduc);
      if (iRC != 1)
      {
         printf("-E- reduction factor buffers invalid: %s\n", argv[4]);
         return 4;
      }
   }
   if (iBufReduc < 0)
   {
      printf("-E- reduction factor buffers invalid: %d\n", iBufReduc);
      return 4;
   }
   if (iBufReduc > 1)
   {
      printf("-W- reduction factor not yet implemented and ignored\n");
      iBufReduc = 0;
   }

   if (--argc > 0)
   {
      iRC = sscanf(argv[5], "%d", &iClientMax);
      if (iRC != 1)
      {
         printf("-E- invalid max no. of simultaneous clients: %s\n",
                argv[5]);
         return 4;
      }
   }
   if (iClientMax < 0)
   {
      printf("-E- invalid max no. of simultaneous clients: %d\n",
             iClientMax);
      return 4;
   }
   if (iClientMax == 0) iClientMax = 20;                /* default */

   if (--argc > 0)
   {
      iRC = sscanf(argv[6], "%d", &iTimeBuf);
      if (iRC != 1)
      {
         printf("-E- invalid buffer timeout: %s\n", argv[6]);
         return 4;
      }
   }
   if (iTimeBuf < 0)
   {
      printf("-E- invalid buffer timeout: %d\n", iTimeBuf);
      return 4;
   }
   if (iTimeBuf == 0) iTimeBuf = 120;                   /* default */

   if (--argc > 0)
   {
      iRC = sscanf(argv[7], "%d", &iServMode);
      if (iRC != 1)
      {
         printf("-E- invalid server mode: %s\n", argv[7]);
         return 4;
      }
   }
   if (iServMode >= 10)
   {
      iServMode -= 10;
      iDebug = 1;
   }
   if ( (iServMode < 0) || 
        ( (iServMode > 5) && (iServMode != 9) ) )
   {
      printf("-E- invalid server mode: %d\n", iServMode);
      return 4;
   }

   if ( (iServMode >= 1) && (iServMode <= 3) )
   {
      if (--argc > 0) strcpy(cAcqFile, argv[8]);
      if (strlen(cAcqFile) == 0)
      {
         switch (iServMode)
         {
            case 1:
               printf("-E- please specify output file for data buffers\n");
               return 4;
            case 2:
            case 3:
               printf("-E- please specify input file containing data buffers\n");
               return 4;
            default: ;
         }
      }
   }

   if ( (iServMode == 4) || (iServMode == 5) )
   {
      if (--argc > 0) strcpy(cAcqFile, argv[8]);
      if (strlen(cAcqFile) > 0) strcpy(cIniFile, cAcqFile);

      if (iServMode == 5)
      {
         if (--argc > 0)
         {
            iRC = sscanf(argv[9], "%d", &iBufDelay);
            if (iRC != 1)
            {
               printf("-E- invalid delay for even-numbered buffers: %s\n",
                      argv[9]);
               return 4;
            }
         }
         if (iBufDelay <= 0) iBufDelay = 30;
      }
      iRandomMode = 1;

   } /* (iServMode == 4) || (iServMode == 5) */

   if (iServMode == 9)
   {
      if (--argc > 0)
      {
         iRC = sscanf(argv[8], "%d", &iBufMax);
         if (iRC != 1)
         {
            printf("-E- invalid buffer no. for benchmark: %s\n",
                   argv[8]);
            return 4;
         }
      }
      if (iBufMax <= 0) iBufMax = 24;            /* def.: 3 streams */
   }

   iRC = gethostname( cHost, sizeof(cHost));
   iRC = ptrevTime(NULL, cTimeStamp, 256);
   if (iRC)
      printf("-I- MBS remote event server (%s:%d) started\n",
             cHost, iPort);
   else
     printf("--> %s: MBS remote event server started\n       (%s:%d)\n",
            cTimeStamp, cHost, iPort);

   if ( (iServMode == 0) || (iServMode == 1) ||
        (iServMode == 2) || (iServMode == 9) )
   {
      iStrMode = 1;                           /* handle stream data */

      if (iServMode == 2)
         printf("    input buffers will be read from file (stream server format):\n       %s\n",
                cAcqFile);
      else
      {
         if (iAcqPort == 6000)
         {
            iTransport = 1;
            printf(
               "    input from transport on data acquisition (%s:%d)\n",
               cAcqNode, iAcqPort);
         }
         else printf("    input from data acquisition (%s:%d)\n",
                     cAcqNode, iAcqPort);
      }

      if (iServMode == 1)
         printf("    buffers will be written to '%s' (not yet implemented)\n",
                cAcqFile);
      if (iServMode == 9)
         printf("    measure data transfer rate from DAQ (%d MBS buffers) - no clients acepted\n",
                iBufMax);
   }

   if (iServMode == 3)
      printf("    input buffers will be read from '%s' (normal lmd file - not yet implemented)\n",
             cAcqFile);

   if (iRandomMode)
   {
      printf("    random events will be generated\n       (init. parameters from %s)\n",
             cIniFile);
      if (iServMode == 5)
         printf("       delay for even-numbered buffers %d s\n",
                iBufDelay);
      if (iLargeEvt) 
      {
         printf("       event size increased by %d (2 byte) words\n",
                iLargeEvt);
      }
   }

   if ( (iServMode != 3) && (iServMode != 9) )
   {
      if ( (iBufReduc == 0) || (iBufReduc == 1) )
         printf("    take all buffers in stream\n");
      else
         printf("    take each %dth buffer in stream\n",
                iBufReduc);
   }
   else iBufReduc = 1;

   if (iServMode != 9)
   {
      printf("    max %d simultaneous clients allowed\n", iClientMax);
      printf("    if no DAQ events available: send dummy buffer after each %d s to clients\n",
             iTimeBuf);
   }

   if ( (iServMode == 1) || (iServMode == 3) ) return 4;

   iBufHeadHW = iBufHead/ishort;
   iBufHeadPar = iBufHead/iint;
   iEvtHeadHW = iEvtHead/ishort;
   iEvtHeadPar = iEvtHead/iint;
   ptBufLock = &sptBufLock;
   iRC = ptrevLockInit(ptBufLock); /* init request lock structure */

   time(&t0);

   if (iServMode != 9)
   {
      /********************** establish server **********************/

      if ( (iListenSocket = socket( AF_INET, SOCK_STREAM,
                                    IPPROTO_TCP ) ) == -1 )
      {      
         sprintf(cMsg, "-E- %s: getting server socket failed", cModule);
         perror(cMsg);
         iError = 1;
         goto gEndMain;
      }              

      sAddr.sin_port = htons(iPort);
      sAddr.sin_family = AF_INET;
      sAddr.sin_addr.s_addr = INADDR_ANY;

      if ( ( iRC = bind( iListenSocket, (struct sockaddr *) &sAddr,
                         iaddrSize ) ) != 0 )
      {
         sprintf(cMsg, "-E- %s: bind failed", cModule);
         perror(cMsg);
         iError = 1;
         goto gEndMain;
      }
      if ( ( iRC = listen( iListenSocket, 1 ) ) == -1 )
      {
         sprintf(cMsg, "-E- %s: listen failed", cModule);
         perror(cMsg);
         iError = 1;
         goto gEndMain;
      }

      /* create Listen thread after connection to DAQ, else problems
         with locks occur, if client requests before DAQ connected  */

      if (iDebug) printf("-D- %s: server established\n", cModule);

   } /* (iServMode != 9) */

   /**************** open connection to stream server **************/

   if ( (iServMode == 0) || (iServMode == 1) || (iServMode == 9) )
   {
      time(&labsTimei);                          /* abs time init. */
      printf("-I- connecting to stream server %s:%d",
          cAcqNode, iAcqPort);
      if (iDaqTry == -1)
         printf(" - try only once\n");
      if (iDaqTry == 0)
         printf(" - try until success\n");
      if (iDaqTry > 0)
         printf(" - try for at most %d s\n", iDaqTry);

      iRC = rconnect(cAcqNode, iAcqPort, &iDaqTry, &iStrSocket);
      if (iRC)
      {
         printf("-E- %s: cannot connect to stream server %s:%d\n",
                cModule, cAcqNode, iAcqPort);
         iError = 1;
         iStrSocket = 0;
         goto gEndMain;
      }

      printf("    successfully connected to stream server %s:%d",
             cAcqNode, iAcqPort);
      if (iDaqTry) printf(" (after %d s)\n", iDaqTry);
      else printf("\n");

      iStrInfoSize = sizeof(sStrInfo);
      iRC = ptrevRecv(0, iStrSocket, (char *) &sStrInfo, iStrInfoSize);
      if (iRC)
      {  
         printf("-E- %s: receiving stream info from server\n",
                 cModule);
         iError = 1;
         goto gEndMain;
      }

      if (iDebug)
         printf("    %s: stream server info received (%d byte)\n",
                cModule, iStrInfoSize);

   } /* (iServMode == 0) || (iServMode == 1) || (iServMode == 9) */

   /******************** initialize event generation ***************/

   if (iRandomMode)
   {
      fIniFile = fopen(cIniFile, "r");
      if (fIniFile == NULL)
      {
         sprintf(cMsg, "-E- %s: opening initialization file %s",
                cModule, cIniFile);
         perror(cMsg);
         iError = 1;
         goto gEndMain;
      }

      if (iDebug)
         printf("-D- %s: Init file for event generation opened\n",
                cModule);

      fscanf(fIniFile, "%d %d %d %d\n", 
             &igenSE, &igenPar, &igenMode, &igenFunc);

      if (iDebug)
      {
        printf("-D- generate %d subevents with at most %d parameters\n",
               igenSE, igenPar);
        if ( (igenSE > 1) && (igenMode == 1) )
           printf("    both subevents have same size\n");
      }

      fclose(fIniFile);

   } /* (iRandomMode) */

   /**************** open file containing stream buffers ***********/

   if (iServMode == 2)
   {
      fAcqFile = fopen(cAcqFile, "rb");
      if (fAcqFile == NULL)
      {
         sprintf(cMsg, "-E- %s: opening input file %s to read streams",
                cModule, cAcqFile);
         perror(cMsg);
         iError = 1;
         goto gEndMain;
      }

      if (iDebug)
         printf("-D- %s: stream file opened\n", cModule);

      iStrInfoSize = sizeof(sStrInfo);
      iRC = fread(&sStrInfo, ichar, iStrInfoSize, fAcqFile);
      if (iRC < iStrInfoSize)
      {
         if (iRC < 0)
         {
            sprintf(cMsg, "-E- %s: reading stream info from file %s",
                   cModule, cAcqFile);
            perror(cMsg);
         }
         else
            printf("-E- %s: %d bytes read, %d bytes requested (stream server info)\n",
                   cModule, iRC, iStrInfoSize);

         fclose(fAcqFile);
         iError = 1;
         goto gEndMain;

      } /* (iRC < iStrInfoSize) */

      if (iDebug)
         printf("    %s: stream server info read (%d byte)\n",
                cModule, iStrInfoSize);

   } /* (iServMode == 2) */

   /**************** evaluate stream server info *******************/

   if (iStrMode == 1)
   {
      /* look if 4-byte swap necessary */
      if (sStrInfo.l_order != 1)
      {
         iStrSwap = 1;
         if (iDebug)
            printf("    %s: swap stream server info\n", cModule);

         iRC = (int) swaplw( &(sStrInfo.l_order), iStrInfoSize/iint, 0);

      } /* (sStrInfo.l_order != 1) */

      iBufSize = sStrInfo.l_bufsize;
      iStrBufMax = sStrInfo.l_num_buf;
      iStrSize = iBufSize*iStrBufMax;
      iOutBufMax = 1;  /* future option: lock all buffers in stream */

      if (iDebug)
      {
         printf("-D- stream size:    %d\n", iStrSize);
         printf("    buffer size:    %d\n", iBufSize);
         printf("    no. of buffers: %d\n", iStrBufMax);
         printf("    must be ""1"":  %d\n", sStrInfo.l_order);
      }

      if (iStrSize > iBuffer)
      {
        iBuffer = iStrSize; /* buffer size will be prefixed in ptServ */
        if (iDebug)
           printf("    %s: buffer size will be increased to %d byte\n",
                  cModule, iBuffer);
      }

   } /* (iStrMode) */

   /********************* allocate data buffer **********************/

   if (iRandomMode)
   {
      iBuffer = igenBufSize;
      iOutBufMax = 1;
   }

   if ( ( pBuffer = calloc(iBuffer, ichar) ) == NULL )
   {
      sprintf(cMsg, "-E- %s: allocating data buffer", cModule);
      perror(cMsg);
      iError = 1;
      goto gEndMain;
   }

   piBuffer = (int *) pBuffer;
   if (iDebug)
      printf("-D- %s: data buffer allocated (%d byte)\n",
             cModule, iBuffer);

   if ( ( pBufferSwap = calloc(iBuffer, ichar) ) == NULL )
   {
      sprintf(cMsg, "-E- %s: allocating swapped data buffer",
              cModule);
      perror(cMsg);
      iError = 1;
      goto gEndMain;
   }

   piBufferSwap = (int *) pBufferSwap;
   if (iDebug)
      printf("-D- %s: swapped data buffer allocated (%d byte)\n",
             cModule, iBuffer);

   iEvtNo = 0;               /* reset, as incremented in event loop */

   /*************** create thread filling data buffer ***************/

   iRC = pthread_create( &ptData, PT_DEF, &ptrevData, pv);
   if (iRC != 0 )
   {
      printf("-E- %s: pthread_create Data: %s\n",
             cModule, strerror(iRC));
      return 4;
   }
   printf("-I- Data pthread created\n");
   pthread_detach(ptData);
                     /* will be completely removed after completion */

   /********************* create Listen thread **********************/

   if (iServMode != 9)
   {
      iRC = pthread_create( &ptListen, PT_DEF, &ptrevListen, pv);
      if (iRC != 0 )
      {
         printf("-E- %s: pthread_create Listen: %s\n",
                cModule, strerror(iRC));
         return 4;
      }
      printf("-I- Listen pthread created\n");
      pthread_detach(ptListen);
                     /* will be completely removed after completion */

   } /* (iServMode != 9) */

   /****************** wait for signal to exit **********************/

   pthread_mutex_lock(&mutexExit);
   pthread_cond_wait(&condExit, &mutexExit);
   pthread_mutex_unlock(&mutexExit);
   printf("-I- %s: Exit condition was signaled\n", cModule);

gEndMain:
   iCloseMode = 2;
   if (iStrSocket)
   {
      if (iDebug)
         printf("    %s: close(%d) connection to stream server", 
                cModule, iCloseMode);
      iRC = rclose(&iStrSocket, iCloseMode);
      iStrSocket = 0;
      if ( (iDebug) && (iRC == 0) )
         printf(" - done\n");
   }

   if (iListenSocket)
   {
#ifdef _AIX 
      /* no close() call: hangs as ptListen is in accept call! */
      if (iDebug)
         printf("    %s: listen socket needs not be closed!\n",
                cModule);
#else
      iCloseMode = 3;
         /* only close() call: shutdown not possible! */
      if (iDebug)
         printf("    %s: close(%d) listen socket",
                cModule, iCloseMode);
      iRC = rclose(&iListenSocket, iCloseMode);
      iListenSocket = 0;
      if ( (iDebug) && (iRC == 0) )
         printf(" - done\n");
#endif
   }

   /* provide time stamp for messages */
   iRC = ptrevTime(NULL, cTimeStamp, 256);
   if (iRC)
      printf("-I- MBS remote event server stopped\n");
   else
      printf("--> %s: MBS remote event server stopped\n", cTimeStamp);
   return 0;

} /* main */

/*********************** local thread functions *********************/

void *ptrevListen(void *pv)
{
   char cModule[32] = "ptListen";                   /* pthread name */
   int iSocket;
   int iCloseMode = 2;/* for rclose(): shutdown level; <0: no close */
   int iRC = 0;
   int inc;
   char cMsg[128] = "";

   sptrevClient *ptrevClient;   /* control structure current client */

   /* create client control structure */
   ptrevClient = calloc(sizeof(sptrevClient), ichar); 
   if (ptrevClient == NULL)
   {
      sprintf(cMsg, "-E- %s: allocating client control structure",
              cModule);
      perror(cMsg);
      goto gErrorListen;
   }

   printf("-I- %s: listen for clients\n", cModule);
   for (;;)                       /* listen permanently for clients */
   {
gNext:
      if ( ( iSocket = accept( iListenSocket,
                               (struct sockaddr *) &sAddr,
                               &iaddrSize ) )
         == -1 )
      {
         if (imySigS == 0)
         {
            sprintf(cMsg, "-E- %s: accept failed", cModule);
            perror(cMsg);
         }
         goto gErrorListen;
      }

      iClientAll++;                        /* total no. of clients */
      if (++iClientAct > iClientMax)
      {
         printf("-E- %s: max number of clients (%d) already active - new client refused\n",
                cModule, iClientMax);

         if (iSocket)
         {               
            if (iDebug)
               printf("    %s: close(%d) client socket",
                      cModule, iCloseMode);
            iRC = rclose(&iSocket, iCloseMode);
            if ( (iDebug) && (iRC == 0) )
               printf(" - done\n");
         }                 
         printf("    %s: client no. %d disconnected\n",
                cModule, iClientAll);

         goto gNext;
      }

      ptrevClient->iClient = iClientAll;
      ptrevClient->iSocket = iSocket;
      if (iDebug)
         printf("-D- %s: Client no. %d connected, socket %d\n",
                cModule, iClientAll, iSocket);

      iRC = pthread_create( &ptServ,
                            PT_DEF,
                            &ptrevServ,
                            ptrevClient);
      if (iRC != 0 )
      {
         printf("-E- %s: pthread_create ptServ(%d): %s\n",
                 cModule, iClientAll, strerror(iRC));
         goto gErrorListen;
      }
      pthread_detach(ptServ);
                     /* will be completely removed after completion */
      if (iDebug)
         printf("    %s: pthread Serv(%d) created (detachable)\n",
                cModule, iClientAll);

   } /* for(;;) */

gErrorListen:
   /* signal main() to stop */
   if (iDebug)
      printf("    %s: signal Exit to main program\n", cModule);
   pthread_mutex_lock(&mutexExit);
   pthread_cond_signal(&condExit);
   pthread_mutex_unlock(&mutexExit);
   return;

} /* ptrevListen */

/********************************************************************/

void *ptrevData(void *pv)
{
   char cModule[32] = "ptData";                     /* pthread name */
   char cTimeStamp[256] = "";
   char cMsg[256];
   char cStrRequ[12] = "";       /* dummy buffer to request streams */
   int iCloseMode = 2;/* for rclose(): shutdown level; <0: no close */

   time_t t1;
   int iRC;
   int ilenRem;
   int iBufNext;
   int iBufSkip = 0;
   int iReconnect = 0;    /* if = 1, try reconnect to stream server */
   int iFragBeginCount;
   int iFragEndCount;

   int iFirstFrag = 0;
             /* = 1: current buffer contains begin of spanned event */
   int iOutAvail = 0;
     /* = 1: new output buffer already available with complete events */
   int iMergeEvt = 0;
      /* = 1: current input buffer with fragment begin */
      /* = 2: current event spanned */
   int iStrNoCopy = 0;              /* no. of output streams copied */
   int iEvtLen;        /* summing up length of event fragments (HW) */
   int iEvtLenAll;    /* total data length of fragmented event (HW) */
   int iCopySize;
   int iOffsetMerge;
       /* offset of event header with merged event in output buffer */

   char *pcIn;                    /* ptr in input stream (for copy) */
   short *psIn;                 /* ptr in input stream (for events) */
   char *pcOut;                             /* ptr in output stream */
   int *pint;
   int ii, jj, kk, ipar;

   int iEOB = 0;         /* for generated events: =1 -> buffer full */
   struct timespec timeout;     /* for generated events: delay time */
   struct timeval now;
   int iDelay = 1;                          /* if = 1: delay buffer */
   pthread_mutex_t mutexDelay;
   pthread_cond_t condDelay;

   long lDataSum = 0;                     /* total bytes transfered */
   time_t lTimex, lTimes;                         /* relative times */
   double dDataRatex, dDataRates;           /* data rates (kByte/s) */

   int igenFree = 0;  /* free space for random events (2byte words) */
   int igenParm = 0;    /* total length random events (2byte words) */

   sMbsBufHead *pBufHeadOut;  /* MBS buffer header in output stream */
   sMbsBufFrag *pFrag;                    /* fragmented event flags */
   sMbsEv101 *pEvtHead, *pEvtHeadMerge;        /* header event 10.1 */

   printf("-I- %s: provide MBS event buffers\n", cModule);

   pthread_mutex_init( &mutexDelay, NULL);
   pthread_cond_init( &condDelay, NULL);

   /* initialize global data for (old) Paw clients */
   pOldHeadData = &sOldHeadData;
   pOldHeadData->l_inbuf_read_cnt = 0;
   pOldHeadData->l_inbuf_rdok_cnt = 0;
   pOldHeadData->l_inbuf_skip_cnt = 0;
   pOldHeadData->l_inbuf_byrd_cnt = 0;
   pOldHeadData->l_inbuf_proc_cnt = 0;
   pOldHeadData->l_inbuf_prev_cnt = 0;

   if (iServMode == 9)
      ptBufLock->iRequestors = 1;        /* simulate client request */

   time(&labsTimeo);                               /* abs time open */
   time(&t1);
   for (;;)                           /* infinite input buffer loop */
   {
      if (iServMode == 9)
         ptBufLock->iAccess = 1;   /* simulate previous buffer read */

      /* try lock to write next buffer */
      iBufNext = iBufNoLock + iBufSkip + 1;
      if (iBufSkip)
         iBufSkip = 0;
      iRC = ptrevWriteLock(ptBufLock, iBufNext);
      if (iRC)
      {
         printf("-E- %s(0): rc=%d ptrevWriteLock\n", cModule, iRC);
         goto gErrorData;
      }
      iBufNoLock = iBufNext;  /* increment buffer number AFTER lock */

      if (iDebug)
         printf("-D- %s: buffer %d locked for write\n",
                cModule, iBufNoLock);


      /***************** provide next data buffer *******************/

      /* generate random events */
      if (iRandomMode)
      {
         iStrSwap = 0;
         pBufHead = (sMbsBufHead *) pBuffer;   /* MBS buffer header */
         iBufSize = igenBufSize;
         iBufSizeOut = igenBufSize;
         igenFree = (igenBufSize - iBufHead)/2;
                             /* free space for events (2byte words) */

         pBufHead->lBuf_dlen = 0;                 /* init used data */
         pBufHead->sBuf_type = 10;
         pBufHead->sBuf_subtype = 1;
         pBufHead->sBuf_frag = 0;                   /* no fragments */
         pBufHead->sBuf_ulen = 0;                 /* no longer used */
         pBufHead->lBuf_bufno = iBufNoLock;
         pBufHead->lBuf_rem[3] = 1;         /* for byte order check */

         pBufHead->sBuf_ulen = 0;
         pBufHead->lBuf_ele = 0;
         igenFree /= 2;  /* in following loop: only in units of int */

         pint = &piBuffer[iBufHeadPar];              /* first event */
         iEOB = 0;
         for (;;)                       /* event loop in MBS buffer */
         {
            pEvtHead = (sMbsEv101 *) pint;  /* next event in buffer */
            iRC = f_ut_gen_evt(
                pint, igenFree, igenSE, igenPar, igenMode);
            if (iRC < 0)
            {
               if (iRC == -1) 
                  iEOB = 1;         /* no more space for this event */
               else
               {
                  printf("-E- %s: event generation failed, rc = %d\n",
                         cModule, iRC);
                  goto gErrorData;
               }
            }

            pBufHead->lBuf_ele++;
            psIn = (short *) pint;
            
            /* simulate large events (only in header, no data) */
            if (iLargeEvt)
            {
               pEvtHead->iMbsEv101_dlen += iLargeEvt;
               iEOB = 1;            /* no more space for next event */
            }

            igenParm = pEvtHead->iMbsEv101_dlen + 4; /* total event */
            pBufHead->lBuf_dlen += igenParm; /* increment used data */
            igenFree -= igenParm/2;         /* decrement free space */
            pint += igenParm/2;                   /* ptr next event */

            if (iDebug)
            {
               if ( (iDebug == 3) && (pBufHead->lBuf_ele == 1) )
                  for (ii=1; ii<=igenParm; ii++)
               {
                  printf("    %d: %d\n", ii, *(psIn++) );
               }

               printf("    %s: event %d: data len %d, type %d, subtype %d, trigger %d\n",
                   cModule,
                   pBufHead->lBuf_ele,
                   pEvtHead->iMbsEv101_dlen,
                   pEvtHead->sMbsEv101_type,
                   pEvtHead->sMbsEv101_subtype,
                   pEvtHead->sMbsEv101_trigger);
            }

            if (iEOB)
            {
               if (iDebug)
                  printf("    %s: buffer filled (free: %d bytes)\n",
                         cModule, igenFree*4);
               break;                                /* buffer full */
            }

         } /* event loop */

         if (iDelay == 0) iDelay = 1;
         else iDelay = 0;
         if ( (iServMode == 5) && (iDelay) )
         {
            iRC = gettimeofday(&now,0);
            if (iRC)
            {
               printf("-E- %s: unexpected rc(gettimeofday) = %d\n",
                      cModule, iRC);
               goto gErrorData;
            }

            if (iDebug)
               printf("-D- %s: now wait for %d s\n",
                      cModule, iBufDelay);

            /* wait for condition for at most iBufDelay sec */
            timeout.tv_sec = now.tv_sec + iBufDelay;
            timeout.tv_nsec = 0;

            pthread_mutex_lock( &mutexDelay );
            iRC = pthread_cond_timedwait( &condDelay,
                                          &mutexDelay,
                                          &timeout);
            pthread_mutex_unlock( &mutexDelay );
         } /* (iServMode == 5) && (iDelay) */

         iRC = (int) swaplw( piBuffer, iBufSize/iint, piBufferSwap);
         if (iDebug)
            printf("-D- %s: random data swapped (%d byte)\n",
                   cModule, iBufSize);

         /* set MBS buffer pointers to begin of new stream */
         pcBufMbsCur = pBuffer;
         pcBufMbsCurSwap = pBufferSwap;          /* swapped version */

         iStrMode = 0;                                /* no streams */
         inewStr = 0;            
         iBufNoIn++;
         iBufNoOut++;

      } /* (iRandomMode) */

      if ( (inewStr == 0) && (iStrMode == 1) )
      {
gNextLock:
         if (iDebug) printf(
            "    %s: prev buffer used %d of %d, skipped %d\n",
            cModule, iStrBufNo, iStrBufOut, iBufSkip);
         if (iStrBufNo < iStrBufOut - iBufSkip)
         {
            if (iStrMerge)
            {
               if (iStrSwap)
                  pBufHead = (sMbsBufHead *) pcBufMbsCurSwap;
               else
                  pBufHead = (sMbsBufHead *) pcBufMbsCur;

               iBufSizeOut = ishort*pBufHead->lBuf_dlen + iBufHead;
                                             /* size previous buffer */
            }
            else iBufSizeOut = iBufSize;

            iStrBufNo++;
            pcBufMbsCur += iBufSizeOut; /* next MBS buffer in stream */
            pcBufMbsCurSwap += iBufSizeOut;       /* swapped version */

            if (iDebug == 1) printf(
               "    %s: buffer ptrs incremented (%d byte), now buffer %d\n",
                      cModule, iBufSizeOut, iStrBufNo);

            if (iStrMerge)
            {
               if (iStrSwap)
                  pBufHead = (sMbsBufHead *) pcBufMbsCurSwap;
               else
                  pBufHead = (sMbsBufHead *) pcBufMbsCur;
               iBufSizeOut = ishort*pBufHead->lBuf_dlen + iBufHead;
            }                                     /* size new buffer */
            else iBufSizeOut = iBufSize;
        
            if (iDebug == 1)
               printf("    %s: size new buffer %d (%d), new data %d\n",
                      cModule, iBufSizeOut, pBufHead,
                      pBufHead->lBuf_dlen);
         }
         else
         {
            inewStr = 1;                  /* get new stream buffer */
            if (iDebug == 1)
               printf("-D- %s: request new stream\n", cModule);
         }
      } /* (inewStr == 0) && (iStrMode == 1) */

gNextStream:
      if (inewStr)
      {
         iOutAvail = 0;         /* no remainder from previous stream */
         iStrNo++;                           /* no. of input streams */

         if ( (iServMode == 0) || (iServMode == 1) || (iServMode == 9) )
         {
            if (iReconnect)
            {
gReconnect:
               iDaqTry = -1;  /* try reconnect always until success */
               iRC = rconnect(cAcqNode, iAcqPort,
                                &iDaqTry, &iStrSocket);
               if (iRC)                          /* rconnect failed */
               {
                 printf("-E- reconnect to stream server %s:%d failed\n",
                        cAcqNode, iAcqPort);

                  iStrSocket = 0;
                  goto gErrorData;
               }

               /* provide time stamp for messages */
               iRC = ptrevTime(NULL, cTimeStamp, 256);
               if (iRC) printf(
                  "    successfully reconnected to stream server %s:%d",
                  cAcqNode, iAcqPort);
               else printf(
                  "--> %s: successfully reconnected to stream server %s:%d",
                  cTimeStamp, cAcqNode, iAcqPort);
               if (iDaqTry) printf(" (after %d s)\n", iDaqTry);
               else printf("\n");

               iRC = ptrevRecv(0, iStrSocket,
                               (char *) &sStrInfo, iStrInfoSize);
               if (iRC)
               {  
                  printf("-E- %s: receiving stream info from server\n",
                         cModule);
                  iStrSocket = 0;
                  goto gErrorData;
               }

               if (iDebug) printf(
                  "    %s: stream server info received (%d byte)\n",
                  cModule, iStrInfoSize);

               /* check consistency of info data */
               if ( ( (sStrInfo.l_order != 1) && (!iStrSwap) ) ||
                    ( (sStrInfo.l_order == 1) && (iStrSwap) ) )
               {  
                  printf("-E- %s: new stream server info (swap) inconsistent with previous one\n",
                         cModule);
                  iStrSocket = 0;
                  goto gErrorData;
               }

               if (sStrInfo.l_order != 1)
               {
                  iStrSwap = 1;
                  if (iDebug)
                  printf("    %s: swap stream server info again\n",
                         cModule);

                  iRC = (int) swaplw(
                     &(sStrInfo.l_order), iStrInfoSize/iint, 0);

               } /* (sStrInfo.l_order != 1) */

               iBufSize = sStrInfo.l_bufsize;
               iStrBufMax = sStrInfo.l_num_buf;
               iStrSize = iBufSize*iStrBufMax;
               iOutBufMax = 1;
                       /* future option: lock all buffers in stream */

               if (iDebug)
               {
                  printf("-D- stream size:    %d\n", iStrSize);
                  printf("    buffer size:    %d\n", iBufSize);
                  printf("    no. of buffers: %d\n", iStrBufMax);
                  printf("    must be ""1"":  %d\n", sStrInfo.l_order);
               }

               if (iStrSize > iBuffer)
               {
                  iBuffer = iStrSize;
                  printf("-I- %s: increase data buffer to %d byte\n",
                         cModule, iBuffer);

                  free(pBuffer);
                  if ( ( pBuffer = calloc(iBuffer, ichar) ) == NULL )
                  {
                     sprintf(cMsg,
                        "-E- %s: reallocating data buffer", cModule);
                     perror(cMsg);
                     iStrSocket = 0;
                     goto gErrorData;
                  }

                  piBuffer = (int *) pBuffer;
                  if (iDebug) printf(
                     "-D- %s: data buffer reallocated (%d byte)\n",
                     cModule, iBuffer);

                  free(pBufferSwap);
                  if ( (pBufferSwap = calloc(iBuffer, ichar) ) == NULL)
                  {
                     sprintf(cMsg,
                        "-E- %s: reallocating swapped data buffer",
                        cModule);
                     perror(cMsg);
                     iStrSocket = 0;
                     goto gErrorData;
                  }

                  piBufferSwap = (int *) pBufferSwap;
                  if (iDebug) printf(
                     "-D- %s: swapped data buffer reallocated (%d byte)\n",
                     cModule, iBuffer);

                  if (pBufferOut)
                  {
                     free(pBufferOut);
                     pBufferOut = calloc(iBuffer, ichar);
                     if (pBufferOut == NULL)
                     {
                        sprintf(cMsg,
                           "-E- %s: reallocating output buffer (%d byte)",
                           cModule, iBuffer);
                        perror(cMsg);
                        iStrSocket = 0;
                        goto gErrorData;
                     }

                     piBufferOut = (int *) pBufferOut;
                     if (iDebug) printf(
                        "-D- %s: output data buffer reallocated (%d byte)\n",
                        cModule, iBuffer);
                  }
               } /* (iStrSize > iBuffer) */

               iReconnect = 0;

            } /* iReconnect */

            if (iTransport == 0)
            {
               if (iDebug)
                  printf("    %s: requesting next stream %d\n",
                         cModule, iStrNo);

               iRC = ptrevSend(0, iStrSocket, cStrRequ, sizeof(cStrRequ));
               if (iRC)
               {
                  printf("-E- %s: requesting stream %d\n",
                         cModule, iStrNo);
                  if (iStrSocket)
                  {
                     iCloseMode = 2;
                     if (iDebug)
                        printf("    %s: close(%d) connection to stream server", 
                               cModule, iCloseMode);
                     iRC = rclose(&iStrSocket, iCloseMode);
                     iStrSocket = 0;
                     if ( (iDebug) && (iRC == 0) )
                        printf(" - done\n");
                  }

                  goto gReconnect;
               }
            } /* (iTransport == 0) */

            if (iDebug)
               printf("    %s: receiving stream %d (%d bytes)\n",
                      cModule, iStrNo, iStrSize);

            iRC = ptrevRecv(0, iStrSocket, pBuffer, iStrSize);
            if (iRC)
            {
               printf("-E- %s: receiving stream from server\n",
                          cModule);
               if (iStrSocket)
               {
                  iCloseMode = 2;
                  if (iDebug) printf(
                     "    %s: close(%d) connection to stream server", 
                     cModule, iCloseMode);
                  iRC = rclose(&iStrSocket, iCloseMode);
                  iStrSocket = 0;
                  if ( (iDebug) && (iRC == 0) )
                     printf(" - done\n");
               }

               goto gReconnect;
            }

            if (iDebug)
               printf("    %s: stream %d received (%d bytes)\n",
                      cModule, iStrNo, iStrSize);

         } /* (iServMode == 0) || (iServMode == 1) || (iServMode == 9)*/

         if (iServMode == 2)
         {
            /* read complete stream */
            iRC = fread(pBuffer, ichar, iStrSize, fAcqFile);
            if (iRC < iStrSize)
            {
               if (iRC < 0)
               {
                  printf("-E- %s: reading stream from file '%s'\n",
                         cModule, cAcqFile);
                  if (ferror(fAcqFile)) perror("    ");
               }
               else
               {
                  if (iRC == 0)
                  {
                     iEOF = 1;
                     printf("\n-I- %s: EOF found on input file\n",
                            cModule);
                     goto gEndData;
                  }
                  else
                     printf("-E- %s: %d bytes read, %d bytes requested (stream buffer)\n",
                            cModule, iRC, iStrSize);
               }

               fclose(fAcqFile);
               goto gErrorData;

            } /* (iRC < iStrSize) */

            if (iDebug)
               printf("    %s: stream %d read (%d bytes)\n",
                      cModule, iStrNo, iStrSize);

         } /* (iServMode == 2) */

         inewStr = 0;
         iStrBufOut = 0;

         /* create swapped version of input stream */
         iRC = (int) swaplw( piBuffer, iBuffer/iint, piBufferSwap);
         if (iDebug)
            printf("-D- %s: stream swapped\n", cModule);

         /***************** merge event fragments ********************/

         /* set ptr to begin of input stream */
         if (iStrSwap)
            pcBufMbsIn = pBufferSwap;
         else
            pcBufMbsIn = pBuffer;

         /* scan input stream for fragments */
         iFragBeginCount = 0;
         iFragEndCount = 0;
         for (ii=1; ii<=iStrBufMax; ii++)
         {
            pBufHead = (sMbsBufHead *) pcBufMbsIn;
            pFrag = (sMbsBufFrag *) &pcBufMbsIn[8];
            if (pFrag->cBuf_fragBegin)
               iFragBeginCount++;
            if (pFrag->cBuf_fragEnd)
               iFragEndCount++;
            pcBufMbsIn += iBufSize;
         }

         /* reset ptr to begin of input stream */
         if (iStrSwap)
            pcBufMbsIn = pBufferSwap;
         else
            pcBufMbsIn = pBuffer;

         if (iFragBeginCount != iFragEndCount)
         {
            printf("-W- %s: inconsistent no.s of fragment begin (%d) and end (%d)\n",
                   cModule, iFragBeginCount, iFragEndCount);
         }

         /* at least 1 fragment in input stream */
         if (iFragBeginCount)
         {
            if (iDebug) printf(
               "-D- %s: stream %d: fragment begin (%d) and end (%d)\n",
               cModule, iStrNo, iFragBeginCount, iFragEndCount);

            iStrMerge = 1;
            if (pBufferOut == NULL)
            {
               pBufferOut = calloc(iBuffer, ichar);
               if (pBufferOut == NULL)
               {
                  sprintf(cMsg,
                          "-E- %s: allocating output buffer (%d byte)",
                          cModule, iBuffer);
                  perror(cMsg);
                  goto gErrorData;
               }
               piBufferOut = (int *) pBufferOut;

               if (iDebug) printf(
                 "    %s: output buffer 1 (%d)\n", cModule, pBufferOut);
            }

            iBufNoOut++;
            iStrBufOut++;
            pcBufMbsOut = pBufferOut;
                               /* set ptr to begin of output stream */
            iStrNoCopy++;                  /* stream must be copied */
            iMergeEvt = 0;
            iFirstFrag = 0;

            /* loop over buffers of input stream: merge fragments */
            for (ii=1; ii<=iStrBufMax; ii++)
            {
               iBufNoIn++;        /* no. of buffers in input stream */
               pBufHead = (sMbsBufHead *) pcBufMbsIn;

               if (pBufHead->lBuf_ele == 0)
               {
                  if (iDebug) printf(
                     "-W- %s: stream %d, buffer %d: empty buffer - ignored\n",
                     cModule, iStrNo, ii);
                  goto gNextBuf;
               }

               pFrag = (sMbsBufFrag *) &pcBufMbsIn[8];
               if (iDebug)
               {
                  printf("    %s: input buffer %d (%d): dlen %d, %d elements\n",
                         cModule, iBufNoIn, pBufHead,
                         pBufHead->lBuf_dlen, pBufHead->lBuf_ele);
                  if (pBufHead->lBuf_rem[4])
                     printf("    %s:    merged event %d, fragment %d\n",
                            cModule, pBufHead->lBuf_rem[4],
                            pBufHead->lBuf_rem[7]);
                  else printf("    %s:    first event %d\n",
                     cModule, pBufHead->lBuf_rem[7]);
                  printf("    %s:   ", cModule);

                  if (pFrag->cBuf_fragEnd)
                     printf(" frag end,");
                  if (pFrag->cBuf_fragBegin)
                     printf(" frag begin,");
               }

               if (iMergeEvt == 0)
               {
                  /* buffer contains begin of spanned event */
                  if (pFrag->cBuf_fragBegin)
                  {
                     iOffsetMerge = 0;
                     iMergeEvt = 1;
                     iFirstFrag = 1;
                     iEvtLen = 0;
                     iEvtLenAll = pBufHead->lBuf_rem[4];
                                  /* total data length spanned event */

                     pBufHeadOut = (sMbsBufHead *) pcBufMbsOut;
                     pBufHeadOut->lBuf_dlen = 0;       /*  set later */
                                                 /* lBuf_ele is okay */
                     iCopySize = iBufHeadHW;

                     if (iDebug) printf(
                       "  begin spanned event (len %d)\n", iEvtLenAll);
                  }
                  else
                  {
                     if (iDebug)
                        printf(" only complete events\n", cModule);
                  }
               } /* (iMergeEvt == 0) */
               else if (iDebug) printf("\n");

               if (iMergeEvt)
               {
                  if (iFirstFrag)   /* this buffer with 1st fragment */
                     pcIn = pcBufMbsIn;      /* copy complete buffer */

                  psIn = (short *) &(pBufHead->lBuf_rem[7]);
                                         /* event header first event */
                  jj = 0;              /* no. current buffer element */

                  /* loop over buffer elements: get lengths */
                  while (jj < pBufHead->lBuf_ele)
                  {
                     jj++;
                     pEvtHead = (sMbsEv101 *) psIn;

                     if (iDebug) printf(
                        "    %s:    event %d, len %d (%d):",
                        cModule, jj, pEvtHead->iMbsEv101_dlen,
                        pEvtHead);

                     if (iFirstFrag)   /* buffer with fragment begin */
                     {
                        iCopySize += (iEvtHeadHW +
                                     pEvtHead->iMbsEv101_dlen);

                        if (jj == pBufHead->lBuf_ele)  /* frag begin */
                        {
                           iMergeEvt = 2;
                           iEvtLen = pEvtHead->iMbsEv101_dlen;
                           if (iDebug) printf(" fragment begin\n");
                        }
                        else   /* complete event before 1st fragment */
                        {
                           iOffsetMerge += iEvtHeadHW +
                                           pEvtHead->iMbsEv101_dlen;
                           if (iDebug)
                              printf(" complete, before frag\n");
                        }
                     }
                     else           /* buffer with nth fragment, n>1 */
                     {
                        if (jj == 1)           /* fragment continued */
                        {
                           iCopySize = pEvtHead->iMbsEv101_dlen;
                           pcIn = (char *) pEvtHead + iEvtHead;
                             /* append fragment without event header */
                           iEvtLen += pEvtHead->iMbsEv101_dlen;
                           if (iDebug) printf(" fragment continued\n");
                        }
                        else        /* complete event after fragment */
                        {
                           iMergeEvt = 0;
                           iCopySize += pEvtHead->iMbsEv101_dlen +
                                        iEvtHeadHW;
                           if (iOutAvail)
                              ilenRem += pEvtHead->iMbsEv101_dlen +
                                         iEvtHeadHW;
                           if (iDebug)
                              printf(" complete, after frag\n");
                        }
                     } /* buffer with nth fragment, n>1 */

                     if ( (iEvtLenAll) && (iEvtLen == iEvtLenAll) )
                     {

                        /* update header of merged event */
                        pcOut = (char *) pBufHeadOut;
                        pcOut += (iBufHeadHW + iOffsetMerge)*ishort;
                           /* skip buffer header and complete events */
                        pEvtHeadMerge = (sMbsEv101 *) pcOut;
                        pEvtHeadMerge->iMbsEv101_dlen = iEvtLenAll;

                        if (iDebug) printf(
                           "    %s:    mod event header(%d): offset %d, dlen %d\n",
                           cModule, pcOut, iOffsetMerge,
                           pEvtHeadMerge->iMbsEv101_dlen);

                        /******* finish current output buffer ********/

                        iCopySize *= ishort;          /* now in byte */
                        memcpy(pcBufMbsOut, pcIn, iCopySize);
                        if (iDebug) printf(
                           "    %s:    last fragment copied, %d byte (%d)\n",
                           cModule, iCopySize, pcBufMbsOut);

                        /* update buffer header in output stream */
                        pBufHeadOut->lBuf_dlen =
                           iOffsetMerge +         /* complete events */
                           iEvtHeadHW + iEvtLenAll;  /* merged event */

                        if (iOutAvail)
                           iOutAvail = 0;

                        /* set fragment flags and used len = 0 */
                        pint = (int *) pBufHeadOut;
                        pint += 2;
                        *pint = 0;

                        if (iDebug) printf(
                           "    %s:    mod header output buffer %d (%d): dlen %d\n",
                           cModule, iBufNoOut, pBufHeadOut,
                           pBufHeadOut->lBuf_dlen);

                        pcBufMbsOut += iCopySize;     /* next buffer */
                        iCopySize = 0;       /* init for next buffer */

                        /* begin new output buffer */
                        if ( (jj == 1) && (jj < pBufHead->lBuf_ele) )
                                               /* check input buffer */
                        {
                           if (pFrag->cBuf_fragBegin)
                           {
                              iMergeEvt = 1;
                              iFirstFrag = 1;
                                /* new fragment begin in this buffer */

                              iEvtLenAll = pBufHead->lBuf_rem[4];
                                 /* total data len new spanned event */
                              if (iDebug) printf(
                                 "    %s:    new fragment begin (len spanned event %d)\n",
                                 cModule, iEvtLenAll);

                           } /* (pFrag->cBuf_fragBegin) */
                           else
                           {
                              iMergeEvt = 0;
                              iEvtLenAll = 0;
                              iFirstFrag = 0;
                              iOutAvail = 1;
                              ilenRem = 0;
                           }

                           iEvtLen = 0;
                           iOffsetMerge = 0;

                           /* copy buffer header */
                           pBufHeadOut = (sMbsBufHead *) pcBufMbsOut;
                           memcpy(pcBufMbsOut, pcBufMbsIn, iBufHead);
                           iBufNoOut++;
                           iStrBufOut++;

                           if (iDebug) printf(
                              "    %s:    create new output buffer %d,%d (%d): header copied (%d byte)\n",
                              cModule, iStrBufOut, iBufNoOut,
                              pcBufMbsOut, iBufHead);

                           pBufHeadOut->lBuf_dlen -= 
                              (iEvtHeadHW + pEvtHead->iMbsEv101_dlen);
                                            /* data of last fragment */
                           pBufHeadOut->sBuf_ulen = 0;
                           pBufHeadOut->lBuf_ele--;
                                                 /* no more frag end */

                           pcOut = (char *) pBufHeadOut;
                           pFrag = (sMbsBufFrag *) &(pcOut[8]);
                           pFrag->cBuf_fragEnd = 0;

                           if (iDebug) printf(
                              "    %s:    header output buffer %d modified: dlen %d, ele %d\n",
                              cModule, iBufNoOut,
                              pBufHeadOut->lBuf_dlen,
                              pBufHeadOut->lBuf_ele);

                           pcBufMbsOut += iBufHead;
                           psIn += iEvtHeadHW + 
                                   pEvtHead->iMbsEv101_dlen;
                                               /* skip frag end data */
                           pcIn = (char *) psIn;  /* copy next event */

                           continue;            /* handle next event */

                        }/* (jj == 1) && (jj < pBufHeadOut->lBuf_ele) */
                        else
                        {
                           iMergeEvt = 0;
                           iEvtLenAll = 0;
                        }

                     } /* (iEvtLen == iEvtLenAll) */
                     else
                        psIn += iEvtHeadHW + pEvtHead->iMbsEv101_dlen;

                  } /* while (jj < pBufHead->lBuf_ele) */

                  /* correct output buffer header (dlen not reliable) */
                  if ( (ilenRem) && (iOutAvail) &&
                       (pBufHeadOut->lBuf_dlen != ilenRem) )
                  {
                     pBufHeadOut->lBuf_dlen = ilenRem;
                     if (iDebug) printf(
                        "    %s:    output buffer header corrected: dlen %d\n",
                        cModule, pBufHeadOut->lBuf_dlen);
                  }

                  /* copy (remainder of) buffer */
                  if (iCopySize)
                  {
                     iCopySize *= ishort;             /* now in byte */
                     memcpy(pcBufMbsOut, pcIn, iCopySize);

                     if (iFirstFrag)
                     {
                        if (iDebug)
                        {
                           if (iOffsetMerge) printf(
                              "    %s:    complete events and first fragment copied, %d byte (%d)\n",
                              cModule, iCopySize, pcBufMbsOut);
                           else printf(
                              "    %s:    first fragment copied, %d byte (%d)\n",
                              cModule, iCopySize, pcBufMbsOut);
                        }
                        iFirstFrag = 0;     /* reset for next buffer */

                     } /* (iFirstFrag) */
                     else
                     {
                        if (iDebug)
                        {
                           if (iMergeEvt)
                              printf(
                                 "    %s:    cont. fragment copied, %d byte (%d)\n",
                                 cModule, iCopySize, pcBufMbsOut);
                           else
                              printf(
                                 "    %s:    complete events copied, %d byte (%d)\n",
                                 cModule, iCopySize, pcBufMbsOut);
                        }
                     }

                     pcBufMbsOut += iCopySize;

                  } /* (iCopySize) */
               }                                      /* (iMergeEvt) */
               else                /* only complete events in buffer */
               {
                  memcpy(pcBufMbsOut, pcBufMbsIn, iBufSize);
                  if (iDebug) printf(
                     "    %s:    full buffer copied, %d byte, %d -> %d(%d)\n",
                     cModule, iBufSize, iBufNoIn, iBufNoOut,
                     pcBufMbsOut);

                  pcBufMbsOut += iBufSize;
                  iBufNoOut++;
                  iStrBufOut++;
               }

gNextBuf:
               pcBufMbsIn += iBufSize;
               iCopySize = 0;             /* init for next iteration */

            } /* loop MBS buffers in input stream */

            /* create swapped version of input stream */
            if (iStrSwap)
            {
               iRC = (int) swaplw( piBufferOut, iBuffer/iint, piBuffer);
               if (iDebug) printf(
                  "    %s: output stream already swapped: swapped again\n",
                  cModule);

               pcBufMbsCur = pBuffer;
               pcBufMbsCurSwap = pBufferOut;
            }
            else
            {
               iRC = (int) swaplw( piBufferOut, iBuffer/iint, piBufferSwap);
               if (iDebug) printf(
                  "    %s: output stream swapped\n", cModule);

               pcBufMbsCur = pBufferOut;
               pcBufMbsCurSwap = pBufferSwap;
            }

         } /* (iFragBeginCount) */
         else                      /* no copy, provide input stream */
         {
            iStrMerge = 0;
            iMergeEvt = 0;
            iBufNoIn++;
            iBufNoOut++;
            iStrBufOut++;
            pcBufMbsCur = pBuffer;
            pcBufMbsCurSwap = pBufferSwap;       /* swapped version */
         }

         if (iStrMerge)
         {
            if (iStrSwap)
               pBufHead = (sMbsBufHead *) pcBufMbsCurSwap;
            else
               pBufHead = (sMbsBufHead *) pcBufMbsCur;

            iBufSizeOut = ishort*pBufHead->lBuf_dlen + iBufHead;

            if (iDebug)
            {
               printf(
                  "-D- %s: merged %d input buffers into %d output buffers\n",
                  cModule, iStrBufMax, iStrBufOut);
               printf("    %s: size first buffer %d, dlen %d (%d)\n",
                  cModule, iBufSizeOut, pBufHead->lBuf_dlen, pBufHead);
            }
         }
         else
         {
            iBufSizeOut = iBufSize;
            if (iDebug)
               printf("-D- %s: no buffers merged\n", cModule);
         }

         iStrBufNo = 1;    /* current buffer number locked in stream */

      } /* (inewStr) */

      /* provide pointers to data to be read by server */
      if (iStrSwap)
      {
         pBufHead = (sMbsBufHead *) pcBufMbsCurSwap;
         pEvtHead = (sMbsEv101 *) &pcBufMbsCurSwap[iBufHead];
         pFrag = (sMbsBufFrag *) &pcBufMbsCurSwap[8];
      }
      else
      {
         pBufHead = (sMbsBufHead *) pcBufMbsCur;
         pEvtHead = (sMbsEv101 *) &pcBufMbsCur[iBufHead];
         pFrag = (sMbsBufFrag *) &pcBufMbsCur[8];
      }

      /* pBufHead->lBuf_rem[3] = 7; DDD */
      if (pBufHead->lBuf_rem[3] != 1)
      {
         printf("--> -W- %s: stream %d, buffer %d: byte order tag in buffer header: expected 1, found %d - buffer ignored\n",
                cModule, iStrNo, iBufNoLock, pBufHead->lBuf_rem[3]);
         iBufSkip++;
         inewStr = 1;                       /* get new stream buffer */
         /* goto gNextStream; */
         /* goto gNextLock; */
         goto gErrorData;
      }

      iEventBuf = pBufHead->lBuf_ele;
      if (pFrag->cBuf_fragBegin)
      {
         imbsFragBegin = 1;
         iEventBuf--;
      }
      else imbsFragBegin = 0;

      if (pFrag->cBuf_fragEnd)
      {
         imbsFragEnd = 1;
         iEventBuf--;
      }
      else imbsFragEnd = 0;

      if (iDebug)
      {
         printf("-D- %s: buffer header %d:\n", cModule, iBufNoLock);
         printf("    %s: data len %d, used len %d, type %d, subtype %d, bufno %d\n",
                cModule,
                pBufHead->lBuf_dlen,
                pBufHead->sBuf_ulen,
                pBufHead->sBuf_type,
                pBufHead->sBuf_subtype,
                pBufHead->lBuf_bufno);
         if (imbsFragEnd)
            printf("-W- %s: first event is fragment\n", cModule);
         if (imbsFragBegin)
            printf("-W- %s: last event is fragment\n", cModule);

         printf("-D- %s: %d complete events in buffer %d\n",
                cModule, iEventBuf, iBufNoLock);
         printf("    %s: first element (%d): data len %d, type %d, subtype %d, trigger %d\n",
                cModule,
                pEvtHead->iMbsEv101_count,
                pEvtHead->iMbsEv101_dlen,
                pEvtHead->sMbsEv101_type,
                pEvtHead->sMbsEv101_subtype,
                pEvtHead->sMbsEv101_trigger);

         printf("-D- %s: buffer %d provided, call WriteUnLock\n",
                cModule, iBufNoLock);
      }

      /* update parameters for (old) Paw clients */
      pOldHeadData->l_inbuf_read_cnt++;
      pOldHeadData->l_inbuf_rdok_cnt++;
      pOldHeadData->l_inbuf_proc_cnt++;
      pOldHeadData->l_inbuf_byrd_cnt += (pBufHead->sBuf_ulen)*2;
      pOldHeadData->l_inbuf_prev_cnt += iEventBuf;

gEndData:
      iRC = ptrevWriteUnlock(ptBufLock);
      if (iRC)
      {
         printf("-E- %s(0): rc=%d ptrevWriteUnlock\n", cModule, iRC);
         goto gErrorData;
      }

      if (iEOF)
      {
         if (iDebug)
            printf("-D- %s: finish thread\n", cModule);
         goto  gStopData;   /* exit signaled by last client thread */
      }

      if (iServMode == 9)
      {
         lDataSum += iBufSize;

         if (iBufNoLock >= iBufMax)         /* finish DAQ benchmark */
         {
            time(&labsTimex);                   /* abs time execute */
            lTimex = (labsTimex > labsTimeo)
                     ? labsTimex - labsTimeo : 1;
            lTimes = (labsTimex > labsTimei)
                     ? labsTimex - labsTimei : 1;
            dDataRatex = (double) lDataSum/(lTimex*1024);
            dDataRates = (double) lDataSum/(lTimes*1024);

            printf("\n-I- Data transfer from DAQ (%s:%d):\n",
                   cAcqNode, iAcqPort);
            printf("    %ld Bytes in %ld sec: %7.1f kB/s, overall: %7.1f kB/s\n",
                   lDataSum,
                   lTimex,
                   dDataRatex,
                   dDataRates);

            goto gErrorData;                         /* stop server */

         } /* (iBufNoLock >= iBufMax) */
      } /* (iServMode == 9) */

      if (iDebug)
         printf("-D- %s: buffer %d available for clients\n",
                cModule, iBufNoLock);

   } /* infinite input buffer loop */

gErrorData:
   /* signal main() to stop */
   if (iDebug)
      printf("    %s: signal Exit to main program\n", cModule);
   pthread_mutex_lock(&mutexExit);
   pthread_cond_signal(&condExit);
   pthread_mutex_unlock(&mutexExit);

gStopData:
   return;

} /* ptrevData */

/********************************************************************/

void *ptrevServ(void *pvClient)
{
   char cModule[32] = "ptServ";                     /* pthread name */

   int iOldClient = 0;                /* default: new (ROOT) client */
   char *pCommAll; /* data storage comm buffer (old and new client) */
   srevComm *pCommBuf;                 /* client request for events */
   srevOldComm *pOldComm; /* old client request (only parts needed) */

   char *pInfoAll; /* data storage info buffer (old and new client) */
   srevInfo *pInfoBuf;                    /* server info for client */
   srevOldHead *pOldHead;           /* buffer header for old client */
   srevOldHead *pOldHeadSwap;                    /* swapped version */
   srevOldHead sOldHeadFlush,        /* flush buffer for old client */
               *pOldHeadFlush;

   srevOldAckn sOldAckn, *pOldAckn;   /* acknowledgement old client */

   sptrevClient sptClient, *ptClient;
                                /* control structure current client */
   sptrevDummy sptDummy, *ptDummy;
           /* send to client, if no events within timeout available */

   int iSocket;
   int iCloseMode = 2; /* in rclose(): shutdown level; <0: no close */
   int iClient;                  /* no. of client handled here here */
   int iFirst = 1;              /* init: send info buffer only once */
   int iLock = 0;                     /* = 1, if buffer read locked */
   int iErrorServ = 0;

   int iEvtRequ;/* no. of events requ. in comm. buffer (old client) */
   int iEvtToSent;   /* no. of events still to be sent (old client) */
   int iBufRequ;            /* no. of buffers requ. in comm. buffer */
   int iBufSend;                          /* number of buffers sent */
   int iBufLock = 0;          /* number of event buffer read-locked */
   int iCliSwap = 0;             /* =1: client info must be swapped */
   int iDataSwap = 0;            /* =1: client data must be swapped */

   int iByte, iByte0;
   int iSize, iSizeSend, iSizeData, iSizeFrag;
   int iRC;
   int iLoop;
   int ii, jj, kk;
   int *pint;
   int *pFirstEvt, *pFirstEvtSwap;

   long lTemp;

   char *pchar;
   char cMsg[256];
   char cTimeStamp[256] = "";
   time_t t1;

   time(&t1);
   sptDummy.iBufNo = htonl(0);             /* counter dummy buffers */
   sptDummy.iSize = htonl(sizeof(sptDummy)-iint); /* data following */
   sptDummy.iEvtNo = htonl(0);     /* indicate: no events available */

   pOldHeadFlush = &sOldHeadFlush;

   /* save client info locally */
   ptClient = (sptrevClient *) pvClient;
   iSocket = ptClient->iSocket;
   iClient = ptClient->iClient;
   sptClient.iSocket = iSocket;               /* in local structure */
   sptClient.iClient = iClient;
   ptClient = &sptClient;

   iRC = ptrevTime(NULL, cTimeStamp, 256);
   if (iRC)
      printf("-I- %s(%d): start serving\n", cModule, iClient);
   else
      printf("--> %s, %s(%d): start serving client\n",
             cTimeStamp, cModule, iClient);

   /***************** receive client request ************************/

   if ( ( pCommAll = calloc(iCommSizeAll, ichar) ) == NULL )
   {
      sprintf(cMsg, "-E- %s(%d): allocating communication buffer",
              cModule, iClient);
      perror(cMsg);
      iErrorServ = 1;
      goto gEndServ;
   }
   pCommBuf = (srevComm *) pCommAll;
   if (iDebug) 
      printf("-D- %s(%d): comm. buffer storage allocated (%d byte)\n",
             cModule, iClient, iCommSizeAll);

   iLock = 0;                    /* currently no buffer read locked */
   iBufSend = 0;
   iRC = ptrevRecv(iClient, iSocket, pCommAll, iCommSize);
   if (iRC)
   {               
      if (iRC < 0)
         printf("-E- %s(%d): receiving communication buffer\n",
                     cModule, iClient);
      goto gEndServ;
   }              

   iBufRequ = ntohl(pCommBuf->iBufRequ);
   if (iDebug) 
      printf("-D- %s(%d): comm. buffer received (%d byte)\n",
             cModule, iClient, iCommSize);
              
   if (ntohl(pCommBuf->iMode) != 1)      /* probably old PAW client */
   {
      pOldComm = (srevOldComm *) pCommAll;
      if (pOldComm->l_testbit != GPS__ENV_TESTBIT) iCliSwap = 1;
      if (pOldComm->l_endian == 0)
      {
         if (iDebug)
            printf("-D- %s(%d): client little endian, swap = %d\n",
                   cModule, iClient, iCliSwap);
#ifdef LITTLE__ENDIAN
         if (iCliSwap)
         {
            printf("-E- %s(%d): invalid comm. buffer received:\n",
                   cModule, iClient);
            printf("-E- %s(%d): client and server little endian, but swap seems necessary\n",
                   cModule, iClient);
            goto gEndServ;
         }
#endif
      } 
      else
      {
         if (iDebug)
            printf("-D- %s(%d): client big endian, swap = %d\n",
                   cModule, iClient, iCliSwap);
#ifdef BIG__ENDIAN
         if (iCliSwap)
         {
            printf("-E- %s(%d): invalid comm. buffer received:\n",
                   cModule, iClient);
            printf("-E- %s(%d): client and server big endian, but swap seems necessary\n",
                   cModule, iClient);
            goto gEndServ;
         }
#endif
      } /* pOldComm->l_endian != 0 */
    
      iOldClient = 1;                          /* old (PAW) client */
      if (iCliSwap)
         swaplw( &(pOldComm->l_testbit), iCommSize/iint, 0);

      if (pOldComm->l_testbit != GPS__ENV_TESTBIT)
      {
         printf("-E- %s(%d): invalid comm. buffer received: testbit %d (should be 1)\n",
                cModule, iClient, pOldComm->l_testbit);
         goto gEndServ;
      }

      iEvtRequ = pOldComm->l_numb_of_evt;
      if (iEvtRequ < 0)
      {
         pOldComm->l_numb_of_evt = 2000000000;   /* nearly unlimited */
         iEvtRequ = pOldComm->l_numb_of_evt;
         if (iDebug) printf(
            "-D- %s(%d): old client: request for continuous stream of events\n",
                cModule, iClient, iEvtRequ);
      }
      else if (iDebug)
         printf("-D- %s(%d): old client: request for %d events\n",
                cModule, iClient, iEvtRequ);

      /* get remainder of client request */
      iRC = ptrevRecv(iClient, iSocket, &pCommAll[iCommSize],
                      iCommSizeAll-iCommSize);
      if (iRC)
      {               
         if (iRC < 0)
            printf("-E- %s(%d): receiving 2nd part of communication buffer\n",
                   cModule, iClient);
         goto gEndServ;
      }              

      if (iDebug) 
         printf("-D- %s(%d): remainder comm. buffer received (%d byte)\n",
                cModule, iClient, iCommSizeAll-iCommSize);
              
      if (iCliSwap)
         /* swaplw( &(pOldComm->l_flush_rate), */
         swaplw( (long *) &pCommAll[iCommSize],
                (iCommSizeAll-iCommSize)/iint, 0);

      if (iDebug)
         printf("-D- %s(%d): sample rate %d, flush rate %d\n",
                cModule, iClient,
                pOldComm->l_sample_rate,
                pOldComm->l_flush_rate);

      if (iEvtRequ == 0) goto gEndServ;
         /* client requested no events, close socket */
      iEvtToSent = iEvtRequ;

   } /* ntohl(pCommBuf->iMode) != 1 */
   else 
   {                                                  /* new client */
      if (iDebug) 
         printf("-D- %s(%d): comm. buffer received: data size %d, mode %d, requ. buffers %d\n",
                cModule, iClient,
                ntohl(pCommBuf->iSize),
                ntohl(pCommBuf->iMode),
                iBufRequ);

      if ( (ntohl(pCommBuf->iMode) != 1) ||
           (ntohl(pCommBuf->iSize) != iCommSize-iint) )
      {
         printf("-E- %s(%d): invalid communication buffer (data %d byte) received: %d, %d\n",
                cModule, iClient,
                ntohl(pCommBuf->iSize),
                ntohl(pCommBuf->iMode),
                iBufRequ);
         goto gEndServ;
      }

      if (pCommBuf->iIdent != 1)
      {
         swaplw( &(pCommBuf->iIdent), 1, 0);
         if (pCommBuf->iIdent != 1)
         {
            printf("-E- %s(%d): invalid identifier in communication buffer: must be 1, received %d (after swap)\n",
                   cModule, iClient,
                   pCommBuf->iIdent);
            goto gEndServ;
         }

         iCliSwap = 1;
         if (iDebug)
            printf("-D- %s(%d): different endian type of client\n",
                   cModule, iClient);

      } /* (pCommBuf->iIdent != 1) */

      if (iBufRequ == 0) goto gEndServ;
         /* client finished, close socket */

   } /* new client */

   /*************** initialize and send first buffer ****************/

   if (iFirst)
   {
      if ( ( (iStrSwap) && (!iCliSwap) ) ||
           ( (!iStrSwap) && (iCliSwap) ) ) 
      {
         iDataSwap = 1;
         if (iDebug)
            printf("-D- %s(%d): send swapped version of MBS data to client\n",
                   cModule, iClient);
      }

      if ( ( pInfoAll = calloc(iInfoSizeAll, ichar) ) == NULL )
      {
         sprintf(cMsg, "-E- %s(%d): allocating info buffer",
                 cModule, iClient);
         perror(cMsg);
         iErrorServ = 1;
         goto gEndServ;
      }
      if (iDebug) 
         printf("-D- %s(%d): info buffer storage allocated (%d byte)\n",
                cModule, iClient, iInfoSizeAll);

      if (iOldClient)                           /* old (PAW) client */
      {
         sptClient.iOld = 1;
         sptClient.iTimeBuf = pOldComm->l_flush_rate*10;

         if (iCliSwap)
         {
            /* allocate space for swapped header */
            pOldHeadSwap = calloc(iInfoSizeAll, ichar);
            if (pOldHeadSwap == NULL)
            {
               sprintf(cMsg, "-E- %s(%d): allocating space for swapped info buffer",
                       cModule, iClient);
               perror(cMsg);
               iErrorServ = 1;
               goto gEndServ;
            }
            if (iDebug)
               printf("-D- %s(%d): swapped info buffer storage allocated (%d byte)\n",
                      cModule, iClient, iInfoSizeAll);

         } /* (iCliSwap) */

         pOldHead = (srevOldHead *) pInfoAll;
         pOldHead->l_testbit = GPS__ENV_TESTBIT;
         pOldHead->l_endian  = GPS__ENV_ENDIAN;
         pOldHead->l_dlen = 0;               /* 1st buffer: no data */
         pOldHead->l_free = iBuffer;          /* init: buffer empty */
         pOldHead->l_events = 0;         /* no. of events in buffer */
         pOldHead->l_maxbufsiz = iBuffer;
                      /* total data size sent (header + MBS buffer) */
         pOldHead->l_bytestosnd = INFOALL; /* 1st buffer: 512 bytes */
         pOldHead->l_numbuftosnd = 1;/* 1st: send only small buffer */
         pOldHead->l_client_con = iClientAct;
                           /* number of currently connected clients */
         pOldHead->l_buffertype = 16;                 /* 1st buffer */
         pOldHead->l_msgtyp = 1;           /* default: info message */
         pOldHead->c_message[0] = '\0';            /* empty message */

         /* global server parameters set by ptData */
         pOldHead->l_inbuf_read_cnt = pOldHeadData->l_inbuf_read_cnt;
                                         /* no. of MBS buffers read */
         pOldHead->l_inbuf_rdok_cnt = pOldHeadData->l_inbuf_rdok_cnt;
                                  /* no. of MBS buffers read and ok */
         pOldHead->l_inbuf_skip_cnt = pOldHeadData->l_inbuf_skip_cnt;
                                      /* no. of MBS buffers skipped */
         pOldHead->l_inbuf_proc_cnt = pOldHeadData->l_inbuf_proc_cnt;
                                    /* no. of MBS buffers processed */
         pOldHead->l_inbuf_byrd_cnt = pOldHeadData->l_inbuf_byrd_cnt;
                                            /* bytes of read events */
         pOldHead->l_inbuf_prev_cnt = pOldHeadData->l_inbuf_prev_cnt;
                                         /* no. of processed events */


         pOldHead->l_clntoutbuf_sdev = 0;  /* 1st buffer: no events */
         pOldHead->l_clntoutbuf_sdby = 0;            /* sent bytes  */
         pOldHead->l_clntoutbuf_sdbf = 1;           /* sent buffers */
         pOldHead->l_clntoutbuf_prev = 0;/* yet no processed events */
         pOldHead->l_clntoutbuf_fltm = 0; /* yet no matching events */

         if (iDebug)
            printf("-D- %s(%d): buffer header initialized (%d byte)\n", 
                   cModule, iClient, iOldHead);

         /*************** send (old) message buffer *****************/

         if (iCliSwap)
         {
            swaplw( &(pOldHead->l_testbit), 
                    iOldHead/iint,
                    &(pOldHeadSwap->l_testbit) );
            pchar = (char *) pOldHeadSwap;
         }
         else pchar = (char *) pOldHead;

         iRC = ptrevSend(iClient, iSocket, pchar, iInfoSizeAll);
         if (iRC)
         {
            printf("-E- %s(%d): sending first buffer to old client (%d byte)\n",
                   cModule, iClient, iInfoSizeAll);
            goto gEndServ;
         }
         if (iDebug)
            printf("    %s(%d): first buffer sent to old client (%d byte)\n",
                   cModule, iClient, iInfoSizeAll);

         /* prepare flush buffer */
         pOldHeadFlush->l_buffertype = 6;
                                  /* flush buffer (4) + message (2) */
         pOldHeadFlush->l_msgtyp = 1;               /* info message */
         pOldHeadFlush->l_numbuftosnd = 1;  /* send only one buffer */
         pOldHeadFlush->l_bytestosnd = INFOALL;
         sprintf(pOldHeadFlush->c_message,
                 "MRevServ: Buffer contains no event data");
         if (iDebug)
            printf("    %s(%d): flush message '%s'\n",
                   cModule, iClient, pOldHeadFlush->c_message);

         if (iCliSwap)
         {
            swaplw( &pOldHeadFlush->l_testbit, 11, 0);
            if (iDebug)
               printf("    %s(%d): flush message after swap: '%s'\n",
                      cModule, iClient, pOldHeadFlush->c_message);
         }
         ptDummy = (sptrevDummy *) pOldHeadFlush;

         iBufRequ = 1;                  /* each buffer acknowledged */

      } /* (iOldClient) */
      else                                     /* new (ROOT) client */
      {
         sptClient.iOld = 0;
         sptClient.iTimeBuf = iTimeBuf;

         pInfoBuf = (srevInfo *) pInfoAll;
         pInfoBuf->iSize = htonl(iInfoSize-iint);
                                          /* size of data following */
         pInfoBuf->iMode = htonl(1);    /* to see if swap necessary */
         pInfoBuf->iHeadPar = htonl(iBufHeadPar);
                              /* no. of header parms (int) in event */
         pInfoBuf->iTimeOut = htonl(iTimeOut);
                                    /* wait time client after CTL C */

         if (iDebug)
            printf("-D- %s(%d): client info: data size %d, mode %d, header parms %d, timeout %d\n",
                   cModule, iClient,
                   ntohl(pInfoBuf->iSize),
                   ntohl(pInfoBuf->iMode),
                   iBufHeadPar,
                   iTimeOut);

         iRC = ptrevSend(iClient, iSocket, (char *)pInfoBuf, iInfoSize);
         if (iRC)
         {
            printf("-E- %s(%d): sending info buffer to client\n",
                   cModule, iClient);
            goto gEndServ;
         }
         if (iDebug)
            printf("    %s(%d): client info buffer sent\n",
                   cModule, iClient);

         ptDummy = &sptDummy;                /* ptr to dummy buffer */

      } /* new (ROOT) client */
   } /* (iFirst) */

   /******************* loop over buffers in stream *****************/

gNextServ:
   for (ii=1; ii<=iOutBufMax; ii++)
   {
      if (iOldClient)                           /* old (PAW) client */
      {
         /* receive next acknowledge buffer */
         pOldAckn = &sOldAckn;
         iRC = ptrevRecv(iClient, iSocket, (char *)pOldAckn, iAcknSize);
         if (iRC)
         {               
            if (iRC < 0)
               printf("-E- %s(%d): receiving acknowledge buffer\n",
                      cModule, iClient);
            goto gEndServ;
         }              
         if (iDebug) printf(
            "    %s(%d): acknowledge buffer received from old client (%d byte)\n",
            cModule, iClient, iAcknSize);

         if (iCliSwap)
            lTemp = pOldHeadSwap->l_bytestosnd;
         else
            lTemp = pOldHead->l_bytestosnd;
         if (lTemp != pOldAckn->l_ack_bytes)
         {
            printf("-E- %s(%d): byte numbers sent (%d) and acknowledged (%d) do not match\n",
                   cModule, iClient, lTemp,
                   pOldAckn->l_ack_bytes);
            goto gEndServ;
         }

         if (iCliSwap)
            lTemp = pOldHeadSwap->l_numbuftosnd;
         else
            lTemp = pOldHead->l_numbuftosnd;
         if (lTemp != pOldAckn->l_ack_buf)
         {
            printf("-E- %s(%d): buffer numbers sent (%d) and acknowledged (%d) do not match\n",
                   cModule, iClient, lTemp,
                   pOldAckn->l_ack_buf);
            goto gEndServ;
         }

         if (iCliSwap)
            swaplw( &(pOldAckn->l_clnt_sts), 1, 0);
         if (pOldAckn->l_clnt_sts == 9)      /* old client finished */
            goto gEndServ;
         else if (pOldAckn->l_clnt_sts != 1)
         {           
            printf(
               "-E- %s(%d): invalid client status %d in acknowledgement\n",
               cModule, iClient,
               pOldAckn->l_clnt_sts);
            goto gEndServ;
         }

         if (iEvtToSent <= 0)
         {
            printf("-W- %s(%d): no more events required, but acknowledge didn't indicate last buffer!\n    - finish client\n",
                   cModule, iClient);
            goto gEndServ;
         }

         if (iDebug) printf(
            "-D- %s(%d): acknowledge buffer okay\n", cModule, iClient);

      } /* (iOldClient) */
      else
      {
         iLock = 0;              /* currently no buffer read locked */
         iBufSend = 0;

         /* receive next communication buffer */
         iRC = ptrevRecv(iClient, iSocket, pCommAll, iCommSize);
         if (iRC)
         {               
            if (iRC < 0)
               printf("-E- %s(%d): receiving communication buffer\n",
                      cModule, iClient);
            goto gEndServ;
         }

         iBufRequ = ntohl(pCommBuf->iBufRequ);
         if (iDebug) printf(
            "-D- %s(%d): comm. buffer received (%d byte): data size %d, mode %d, requ. buffers %d\n",
            cModule, iClient, iCommSize,
            ntohl(pCommBuf->iSize),
            ntohl(pCommBuf->iMode),
            iBufRequ);

         if ( (ntohl(pCommBuf->iMode) != 1) ||
              (ntohl(pCommBuf->iSize) != iCommSize-iint) )
         {
            printf("-E- %s(%d): invalid communication buffer (data %d byte) received: %d, %d\n",
                   cModule, iClient,
                   ntohl(pCommBuf->iSize),
                   ntohl(pCommBuf->iMode),
                   iBufRequ);
            goto gEndServ;
         }

         if (pCommBuf->iIdent != 1)
         {
            swaplw( &(pCommBuf->iIdent), 1, 0);
            if (pCommBuf->iIdent != 1)
            {
               printf("-E- %s(%d): invalid identifier in communication buffer: must be 1, received %d (after swap)\n",
                      cModule, iClient,
                      pCommBuf->iIdent);
               goto gEndServ;
            }

            iCliSwap = 1;
            if (iDebug)
               printf("-D- %s(%d): different endian type of client\n",
                      cModule, iClient);

         } /* (pCommBuf->iIdent != 1) */

         if (iBufRequ == 0) goto gEndServ;
            /* client finished, close socket */

      } /* new client */

      /********************** lock data buffer **********************/

      if (iFirst)
      {
         if (iBufLock == 0)               /* first buffer of client */
         {
            iBufLock = iBufNoLock;/* current buffer (server number) */
            if (iBufNoLock) iBufLock++;
              /* so client gets new buffer after reopen in any case */
         }

         iRC = ptrevReadLockFirst(ptBufLock, iBufLock,
                                  ptClient, ptDummy);
         if (iRC)
         {   
            if (iRC < 0)
            {
               printf("-E- %s(%d): rc=%d ptrevReadLockFirst\n",
                      cModule, iClient, iRC);
               iErrorServ = 1;                       /* stop server */
            }
            goto gEndServ;
         }
         if (iBufLock != iBufNoLock)
            iBufLock = iBufNoLock;
                      /* e.g. slow client: doesn't lock each buffer */
         iLock = 1;                       /* now buffer read locked */

         if (iDebug)
         {
            printf("-D- %s(%d): first buffer (%d) read-locked\n",
                   cModule, iClient, iBufLock);
            if (iOldClient)
              printf("-D- %s(%d): buffer contains %d complete events\n",
                     cModule, iClient, iEventBuf); 
            else
               printf("-D- %s(%d): send %d buffers with %d events\n",
                      cModule, iClient, 
                      iBufRequ, iEventBuf);
         }
         iFirst = 0;                     /* initialization complete */

      }  /* (iFirst) */
      else
      {
         /* slow client doesn't lock each buffer */
         if (iBufNoLock > iBufLock)
            iBufLock = iBufNoLock + 1; /* "+ 1" to avoid dead locks */
         else iBufLock++;            /* next buffer (server number) */
         /* iRC = ptrevReadLock(ptBufLock, iBufLock, ptClient, ptDummy);
            DDD */
         iRC = ptrevReadLockFirst(ptBufLock, iBufLock,
                                  ptClient, ptDummy);
         if (iRC)
         {
            if (iRC < 0)
            {
               printf("-E- %s(%d): rc=%d ptrevReadLock\n",
                      cModule, iClient, iRC);
               iErrorServ = 1;                       /* stop server */
            }
            goto gEndServ;
         }
         iLock = 1;                       /* now buffer read locked */

         if (iDebug)
            printf("-D- %s(%d): buffer %d read-locked\n",
                   cModule, iClient, iBufLock);
      }

      pFirstEvt = (int *) &pcBufMbsCur[iBufHead];    /* first event */
      pFirstEvtSwap = (int *) &pcBufMbsCurSwap[iBufHead];
      if (imbsFragEnd)
      {
         if (iStrSwap) iSizeFrag = (*pFirstEvtSwap+4)/2;
         else iSizeFrag = (*pFirstEvt+4)/2;
                     /* length of fragment (int) at begin of buffer */
         pFirstEvt += iSizeFrag;            /* first complete event */
         pFirstEvtSwap += iSizeFrag;        /* first complete event */

         if (iDebug)
            printf("-D- %s(%d): fragment end skipped (%d byte)\n",
                   cModule, iClient, iSizeFrag*4);
      }

      /*************** send header before MBS buffer ****************/

      if (iOldClient)
      {
         if (iDebug)
            printf("-D- %s(%d): still to send: %d events, in buffer %d events\n",
                   cModule, iClient, iEvtToSent, iEventBuf);

         iByte0 = 0;
         if (iStrSwap) pint = pFirstEvtSwap;
         else pint = pFirstEvt;
         if (iEvtToSent < iEventBuf) iLoop = iEvtToSent;
         else iLoop = iEventBuf;

         if (pOldComm->l_sample_rate > 1) /* reduce event number */
         {
            if (pOldComm->l_sample_rate >= iLoop)
               iLoop = 1;
            else
            {
               if ( (iEvtToSent >= iEventBuf) ||
                    ((iEvtToSent < iEventBuf) &&
                     (iEvtToSent*pOldComm->l_sample_rate >= iEventBuf)))
                  iLoop = iEventBuf/pOldComm->l_sample_rate;
               /* if (iLoop > iEvtToSent) iLoop = iEvtToSent; */
            }
            if (iDebug)
               printf("-D- %s(%d): buffer reduction: send only %d events\n",
                      cModule, iClient, iLoop);
         }

         for (jj=1; jj<=iLoop; jj++)
         {
            iByte = (*pint+4)*2;             /* total event size */
            if (iDebug == 2)
               printf("DDD event %d: size %d\n", jj, iByte);

            iByte0 += iByte;            /* add total event sizes */
            pint += iByte/iint;
         }

         if (iEvtToSent <= iLoop)
         {                      /* last data buffer, send only part */
            iEvtToSent = 0;
            pOldHead->l_buffertype = 9;     /* data and last buffer */
            if (iDebug)
               printf("-D- %s(%d): last buffer\n", cModule, iClient);

         } /* (iEvtToSent < iEventBuf) */
         else
         {
            iEvtToSent -= iLoop;
            pOldHead->l_buffertype = 1;              /* data buffer */
         }

         pOldHead->l_bytestosnd = CLNT__OUTBUFHEAD + iByte0;
                                      /* real data size plus header */
         pOldHead->l_events = iLoop;
         pOldHead->l_clntoutbuf_sdev += iLoop;

         if (pOldHead->l_bytestosnd <= INFOALL)/* send only 1 buffer */
         {
            if (iDebug)
               printf("-D- %s(%d): send %d events (%d byte, total %d) - one (small) buffer\n",
                      cModule, iClient,
                      pOldHead->l_events, iByte0,
                      pOldHead->l_bytestosnd);

            pOldHead->l_numbuftosnd = 1;      /* send only 1 buffer */
            pOldHead->l_bytestosnd = INFOALL;
         }
         else
         {
            pOldHead->l_numbuftosnd = 2;          /* send 2 buffers */

            if (iDebug)
               printf("-D- %s(%d): send %d events (%d byte, total %d) - start with header\n",
                      cModule, iClient,
                      pOldHead->l_events, iByte0,
                      pOldHead->l_bytestosnd);
         }

         /* update header file */
         pOldHead->l_dlen = iByte0;   /* total data size in byte!!! */
         pOldHead->l_free = iBuffer - iByte0;   /* unused data size */
         pOldHead->l_client_con = iClientAct;

         pOldHead->l_inbuf_read_cnt = pOldHeadData->l_inbuf_read_cnt;
                                /* no. of MBS buffers read (ptData) */
         pOldHead->l_inbuf_rdok_cnt = pOldHeadData->l_inbuf_rdok_cnt;
         pOldHead->l_inbuf_byrd_cnt = pOldHeadData->l_inbuf_byrd_cnt;
         pOldHead->l_inbuf_proc_cnt = pOldHeadData->l_inbuf_proc_cnt;
                           /* no. of MBS buffers processed (ptData) */
         pOldHead->l_inbuf_prev_cnt = pOldHeadData->l_inbuf_prev_cnt;
                                         /* no. of processed events */

         pOldHead->l_clntoutbuf_sdbf++;
         if (pOldHead->l_numbuftosnd == 2)
            pOldHead->l_clntoutbuf_sdbf++;
         pOldHead->l_clntoutbuf_sdby += pOldHead->l_dlen; 
                                           /* total number of bytes */
         pOldHead->l_clntoutbuf_prev += pOldHead->l_events; 
         pOldHead->l_clntoutbuf_fltm += pOldHead->l_events; 

         if (iCliSwap)
         {
            swaplw( &(pOldHead->l_testbit), 
                    iOldHead/iint,
                    &(pOldHeadSwap->l_testbit) );
            pchar = (char *) pOldHeadSwap;
         }
         else pchar = (char *) pOldHead;

         iRC = ptrevSends(iClient, iSocket, pchar, iOldHead);
         if (iRC)
         {
            printf("-E- %s(%d): sending buffer header to old client\n",
                   cModule, iClient);
            goto gEndServ;
         }
         if (iDebug)
            printf("-D- %s(%d): header sent to old client (%d byte)\n",
                   cModule, iClient, iOldHead);

      } /* (iOldClient) */
      else
      {
         /*************** send data length to new client ************/

         if (iEOF)    /* file input, not yet handled by old clients */
         {
            if (iDebug)
               printf("-D- %s(%d): EOF found, notify client\n",
                      cModule, iClient);

            iSizeSend = htonl(0);          /* signal EOF to clients */
         }
         else
            iSizeSend = htonl(iBufSizeOut);

         if (ntohl(iSizeSend) <= 0)
            printf("-E- %s(%d): invalid data size %d\n",
                   cModule, iClient, ntohl(iSizeSend));

         /* send data size first (with socket check before) */
         iRC = ptrevSends(iClient, iSocket, (char *) &iSizeSend, iint);
         if (iRC)
         {
            if (iRC < 0)
              printf("-E- %s(%d): error with client connection (length)\n",
                     cModule, iClient);
            else
               printf("-W- %s(%d): client no longer reachable (length), close connection\n",
                      cModule, iClient);

            if (iEOF)
            {
               iErrorServ = 1;                       /* stop server */
               goto gEOFClean;
            }
            goto gEndServ;

         } /* iRC(ptrevSends) */

         if (iEOF)
         {
            if (iDebug)
               printf("-D- %s(%d): signaled EOF to client\n",
                      cModule, iClient);

gEOFClean:
            pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
            if (ptBufLock->iRequestors == 0)
            {
               if (iDebug)
                  printf("-D- %s(%d): last client finished, now shutdown server\n",
                         cModule, iClient);
               iErrorServ = 1;                       /* stop server */

            }
            pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

            goto gEndServ;

         } /* (iEOF) */

         if (iDebug)
            printf("-D- %s(%d): length data buffer %d(%d) sent (total %d byte)\n",
                   cModule, iClient, ii, iBufLock, iint);

      } /* (new client) */

      /********************* now send data **************************/

      if (iOldClient)
      {
         if (iDataSwap) pchar = (char *) pFirstEvtSwap;
         else pchar = (char *) pFirstEvt;
                  /* MBS buffer header and leading fragment skipped */

         if (pOldHead->l_numbuftosnd == 1)    /* send only 1 buffer */
            iSizeData = pOldHead->l_bytestosnd - CLNT__OUTBUFHEAD;
                                       /* (old) header already sent */
         else                                     /* send 2 buffers */
            iSizeData = iByte0;
      }
      else
      {
         iSizeData = iBufSizeOut;
         if (iDataSwap) pchar = pcBufMbsCurSwap;
                                             /* complete MBS buffer */
         else pchar = pcBufMbsCur;

      } /* (new client) */

      iRC = ptrevSends(iClient, iSocket, pchar, iSizeData);
      if (iRC)
      {
         if (iRC < 0)
         {
            printf("-E- %s(%d): error with client connection\n",
                   cModule, iClient);
            iErrorServ = 1;                          /* stop server */
         }
         else
            printf("-W- %s(%d): client no longer reachable, close connection\n",
                   cModule, iClient);
         ii--;
         printf("-W- %s(%d): only %d buffers with %d events sent\n",
                cModule, iClient, ii, ii*iEventBuf);

         goto gEndServ;

      } /* iRC(ptrevSends) */

      if (iDebug)
         printf("-D- %s(%d): data buffer %d(%d) sent (total %d byte)\n",
                cModule, iClient, ii, iBufLock, iSizeData);

      if (ii = iOutBufMax)
      {
         /* iRC = ptrevReadUnlock(ptBufLock, iClient); */
         iRC = ptrevReadUnlockLast(ptBufLock, iClient);
         iLock = 0;

         if (iRC)
         {
            printf("-E- %s(%d): rc=%d ptrevReadUnlock\n",
                   cModule, iClient, iRC);
            iErrorServ = 1;                          /* stop server */
            goto gEndServ;
         }

         if (iDebug)
         {
            printf("-D- %s(%d): buffer %d read-unlocked\n",
                   cModule, iClient, iBufLock);

            printf("-D- %s(%d): keep connection for next request\n",
                   cModule, iClient);
         }

      } /* (ii = iOutBufMax) */
   } /* buffer loop */

   goto gNextServ;

   /*********************** final cleanup **************************/

gEndServ:
   if (iDebug)
      printf("-D- %s(%d): client finished\n",
             cModule, iClient);

   if (iLock)
   {
      iRC = ptrevReadUnlockLast(ptBufLock, iClient);
      if (iRC)
      {
         printf("-E- %s(%d): rc=%d in cleaning action ptrevReadUnlockLast\n",
                cModule, iClient, iRC);
         iErrorServ = 1;                             /* stop server */
      }
   } /* (iLock) */

   iClientAct--;
   iCloseMode = 2;
   if (iSocket)
   {               
      if (iDebug)
         printf("    %s(%d): close(%d) client socket",
                cModule, iClient, iCloseMode);
      rclose(&iSocket, iCloseMode);
      if  (iDebug)
      {
         if (iRC == 0) printf(" - done\n");
         else printf("\n");
      }

      /* provide time stamp */
      iRC = ptrevTime(NULL, cTimeStamp, 256);
      if (iRC)
         printf("-I- %s(%d): client disconnected\n",
             cModule, iClient);
      else
         printf("--> %s, %s(%d): client disconnected\n",
                cTimeStamp, cModule, iClient);
   }                 

   if (iErrorServ == 0) return;

   /* signal main() to stop */
   if (iDebug)
      printf("    %s(%d): signal Exit to main program\n",
             cModule, iClient);
   pthread_mutex_lock(&mutexExit);
   pthread_cond_signal(&condExit);
   pthread_mutex_unlock(&mutexExit);
   return;

}  /* ptrevServ */

/********************************************************************/

void ptrevExit(void)
{
   char cModule[32] = "ptrevExit";

   imySigS = 1;
   printf("\n-I- user specified CTL C:\n");

   /* signal main() to stop */
   if (iDebug)
      printf("    %s: signal Exit to main program\n", cModule);
   pthread_mutex_lock(&mutexExit);
   pthread_cond_signal(&condExit);
   pthread_mutex_unlock(&mutexExit);
   return;

} /* ptrevExit */
