/*********************************************************************
 * ptrevlock.h
 * contains the resources to be locked
 * created 21. 1.99, Horst Goeringer
 *********************************************************************
 * 27. 5.1999, H.G.: comments added
 *********************************************************************
 */

/* structure controlling access to the shared event buffer */
typedef struct
{
   int iRequestors;          /* no. of requestors for event buffer */
   int iReaders;                              /* no. of read locks */
   int iDone;                               /* no. of read unlocks */
   int iWriters;                             /* no. of write locks */
   int iAccess;              /* buffer read by at least one thread */
   int iBuffer;                              /* current buffer no. */
   pthread_mutex_t mutexBuffer;
   pthread_cond_t condBufferNew;   /* signalled after write unlock */
   pthread_cond_t condBufferOld; /* signalled after last read unlock */
} ptBufLock_t;

