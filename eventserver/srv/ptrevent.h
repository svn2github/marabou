/*********************************************************************
 * ptrevent.h
 * entry declarations of ptrevlock and ptrevproc
 * created 22.12.98, Horst Goeringer
 *
 * 15. 2.1999, H.G.: add client number as argument
 * 25. 6.1999, H.G.: new entry ptrevWait, new arg: ptr sptrevDummy
 *  5. 7.1999, H.G.: new entry ptrevTime
 * 17. 7.1999, H.G.: new entry f_ut_gen_evt
 *********************************************************************
 */
int f_ut_gen_evt(int *, int, int, int, int);
   /* generate event */

int ptrevTime(time_t *, char *, int);
   /* send buffer to client */

int ptrevSend(int, int, char *, int);
   /* send buffer to client */

int ptrevWait( ptBufLock_t *, int, sptrevClient *, sptrevDummy *);
   /* send dummy buffer to client, if wait times out */

int ptrevLockInit( ptBufLock_t *);
   /* initialize event buffer lock */

int ptrevReadLockFirst( ptBufLock_t *, int, sptrevClient *, sptrevDummy *); 
   /* lock first buffer of request for read */

int ptrevReadLock( ptBufLock_t *, int, sptrevClient *, sptrevDummy *);
   /* lock further buffer for read */

int ptrevReadUnlock( ptBufLock_t *, int);
   /* unlock buffer after read */

int ptrevReadUnlockLast( ptBufLock_t *, int);
   /* unlock last buffer of request after read */

int ptrevWriteLock( ptBufLock_t *, int);
   /* lock buffer for write */

int ptrevWriteUnlock( ptBufLock_t *);
   /* unlock buffer after write */
