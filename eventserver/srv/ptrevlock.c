/*********************************************************************
 * ptrevlock.c
 * lock functions for multithreaded remote event servers
 * implemented 15.12.98, Horst Goeringer
 *
 ********************************************************************
 * ptrevLockInit: initialize event buffer lock
 * ptrevReadLockFirst: lock first buffer for read
 * ptrevReadLock: lock buffer for read
 * ptrevReadUnlock: unlock buffer after read
 * ptrevReadUnlockLast: unlock last buffer after read
 * ptrevWriteLock: lock buffer for write
 * ptrevWriteUnlock: unlock buffer after write
 * ptrevWait: send dummy buffer to client, if event timeout
 ********************************************************************
 * 15. 2.99, H.G.: move debug output from callers to lock routines
 * 18. 3.99, H.G.: add "buffer number" and "access" to debug output
 * 27. 5.99, H.G.: correct time in SSS message ptrevReadLockFirst
 *                 ptrevReadLockFirst, requ>0: shift requ++ to end
 *  1. 6.99, H.G.: add arg buffer number in ptrevReadLockFirst
 *  7. 6.99, H.G.: ptrevReadLockFirst: check buffer number first
 * 25. 6.99, H.G.: new entry ptrevWait
 * 27. 8.99, H.G.: ptrevWait: mod. arg list, signal OLD after timeout
 *  1.12.99, H.G.: ptrevWait: continue wait loop after error in
 *                 pthread_cond_timedwait
 * 13.12.99, H.G.: ptrevWait: handle dummy buffer for old clients
 ********************************************************************
 */

#include <pthread.h>                    /* should be first include */
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

#include "ptrevlock.h"
#include "ptrevclient.h"
#include "ptrevcomm.h"
#include "ptrevoldcomm.h"

extern time_t t0;
extern int iTimeBuf;  /* if over without new buffer, notify client */
extern int iDebug;

int ptrevLockInit( ptBufLock_t *ptBufLock ) 
{
   ptBufLock->iRequestors = 0;
   ptBufLock->iReaders = 0;
   ptBufLock->iDone = 0;
   ptBufLock->iWriters = 0;
   ptBufLock->iAccess = 0;
   ptBufLock->iBuffer = 0;
   pthread_mutex_init( &(ptBufLock->mutexBuffer), NULL);
   pthread_cond_init( &(ptBufLock->condBufferNew), NULL);
   pthread_cond_init( &(ptBufLock->condBufferOld), NULL);
   return 0;

} /* ptrevLockInit */

/********************************************************************/

int ptrevReadLockFirst( ptBufLock_t *ptBufLock,
                        int iBufn,
                        sptrevClient *pClient,
                        sptrevDummy *pDummy)
{
   char cModule[32] = "ptServ";
   time_t t1;
   int isig = 0;
   int iRC = 0;

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(%d,%d): begin ptrevReadLockFirst(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, pClient->iClient, t1-t0,
             iBufn, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );

   if (ptBufLock->iBuffer < iBufn)           /* buffer already read */
   {
      if (ptBufLock->iRequestors == 0)
      {                                 /* no other clients reading */
         /* request new buffer */
         pthread_cond_signal( &(ptBufLock->condBufferOld) );
         isig = 1;
      }

      while (ptBufLock->iBuffer < iBufn)
      {
         iRC = ptrevWait( ptBufLock, iTimeBuf, pClient, pDummy);
            /* iRC != 0: unlock done in ptrevWait */
         if (iRC < 0) return -11;                    /* stop server */
         if (iRC > 0) return 1;
                     /* client finished via CTL C, or finish client */
      }
      ptBufLock->iRequestors++;         /* mark request immediately */

   }  /* (ptBufLock->iBuffer < iBufn) */
   else
   {  /* requested buffer available */

      ptBufLock->iRequestors++;         /* mark request immediately */
      if (ptBufLock->iRequestors == 1)
      {                                 /* no other clients reading */
         if (ptBufLock->iReaders)
         { 
            pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
            return -1;
            /* no requestor, but reader */
         }
         if (ptBufLock->iDone)
         {
            pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
            return -3;
            /* no requestor, but read unlock */
         }

         /* request first buffer */
         if (ptBufLock->iBuffer == 0)
         {
            pthread_cond_signal( &(ptBufLock->condBufferOld) );
            isig = 1;

            iRC = ptrevWait( ptBufLock, iTimeBuf, pClient, pDummy);
               /* iRC != 0: unlock done in ptrevWait */
            if (iRC < 0) return -12;                 /* stop server */
            if (iRC > 0)
            {        /* client finished via CTL C, or finish client */
               ptBufLock->iRequestors--;  /* cancel previous action */
               return 1;
            }
         }

      } /* no other requestors */
   } /* (ptBufLock->iBuffer >= iBufn) */

   if (ptBufLock->iWriters)
   {
      ptBufLock->iRequestors--;       /* remove request temporarily */
      while (ptBufLock->iWriters)
      {
         iRC = ptrevWait( ptBufLock, iTimeBuf, pClient, pDummy);
            /* iRC != 0: unlock done in ptrevWait */
         if (iRC < 0) return -13;                    /* stop server */
         if (iRC > 0) return 1;
                     /* client finished via CTL C, or finish client */
      }
      ptBufLock->iRequestors++;         /* mark request immediately */
   }

   ptBufLock->iReaders++;

   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      if (isig == 1)
        printf("SSS %s(%d,%d): ReadLockFirst signalled condBufferOld\n",
               cModule, pClient->iClient, t1-t0);

      time(&t1);
      printf("DDD %s(%d,%d): end ptrevReadLockFirst(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, pClient->iClient, t1-t0,
             ptBufLock->iBuffer,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   return 0;

} /* ptrevReadLockFirst */

/********************************************************************/

int ptrevReadLock( ptBufLock_t *ptBufLock,
                   int iBufn,
                   sptrevClient *pClient,
                   sptrevDummy *pDummy)
{
   char cModule[32] = "ptServ";
   time_t t1;
   int iRC = 0;

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(%d,%d): begin ptrevReadLock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, pClient->iClient, t1-t0,
             iBufn, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
   if (ptBufLock->iRequestors == 0)
   {
      /* at least own request should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -1;
   }

   /* avoid several locks of same buffer */
   while (ptBufLock->iBuffer < iBufn)
   {
      iRC = ptrevWait( ptBufLock, iTimeBuf, pClient, pDummy);
         /* iRC != 0: unlock done in ptrevWait */
      if (iRC < 0) return -11;                       /* stop server */
      if (iRC > 0) return 1;
                     /* client finished via CTL C, or finish client */
   }

   /* buffer still write-locked */
   while (ptBufLock->iWriters)
   {
      iRC = ptrevWait( ptBufLock, iTimeBuf, pClient, pDummy);
         /* iRC != 0: unlock done in ptrevWait */
      if (iRC < 0) return -12;                       /* stop server */
      if (iRC > 0) return 1;
                     /* client finished via CTL C, or finish client */
   }

   ptBufLock->iReaders++;

   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(%d,%d): end ptrevReadLock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, pClient->iClient, t1-t0,
             iBufn, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   return 0;

} /* ptrevReadLock */

/********************************************************************/

int ptrevReadUnlock( ptBufLock_t *ptBufLock, int iClient)
{
   char cModule[32] = "ptServ";
   time_t t1;
   int isig = 0;
   int ibuf;

   if (iDebug)
   {
      ibuf = ptBufLock->iBuffer;
      time(&t1);
      printf("DDD %s(%d,%d): begin ptrevReadUnlock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, iClient, t1-t0,
             ibuf, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
   if (ptBufLock->iReaders < 1)
   {
      /* at least own read lock should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -1;
   }
   if (ptBufLock->iRequestors < 1)
   {
      /* at least own request should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -2;
   }
   if (ptBufLock->iDone < 0)
   {
      /* not allowed */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -3;
   }
   if (ptBufLock->iWriters)
   {
      /* no write allowed during read lock */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -4;
   }

   ptBufLock->iDone++;
   if ( (ptBufLock->iReaders == ptBufLock->iRequestors) &&
        (ptBufLock->iDone == ptBufLock->iRequestors) )
   {
      /* buffer read by all clients */
      ptBufLock->iReaders = 0;
      ptBufLock->iDone = 0;
      pthread_cond_signal( &(ptBufLock->condBufferOld) );
      isig = 1;
   }

   ptBufLock->iAccess = 1;

   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(%d,%d): end ptrevReadUnlock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, iClient, t1-t0,
             ibuf, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);

      if (isig == 1)
         printf("SSS %s(%d,%d): ReadUnlock signalled condBufferOld\n",
                cModule, iClient, t1-t0);
   }

   return 0;

} /* ptrevReadUnlock */

/********************************************************************/

int ptrevReadUnlockLast( ptBufLock_t *ptBufLock, int iClient)
{
   char cModule[32] = "ptServ";
   time_t t1;
   int isig = 0;
   int ibuf;

   if (iDebug)
   {
      ibuf = ptBufLock->iBuffer;
      time(&t1);
      printf("DDD %s(%d,%d): begin ptrevReadUnlockLast(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, iClient, t1-t0,
             ibuf, ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
   if (ptBufLock->iReaders < 1)
   {
      /* at least own read lock should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -1;
   }
   if (ptBufLock->iRequestors < 1)
   {
      /* at least own request should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -2;
   }
   if (ptBufLock->iDone < 0)
   {
      /* not allowed */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -3;
   }
   if (ptBufLock->iWriters)
   {
      /* no write allowed during read lock */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -4;
   }

   ptBufLock->iDone++;
   if ( (ptBufLock->iReaders == ptBufLock->iRequestors) &&
        (ptBufLock->iDone == ptBufLock->iRequestors) )
   {
      /* buffer read by all clients */
      ptBufLock->iReaders = 0;
      ptBufLock->iDone = 0;
      if (ptBufLock->iRequestors > 1)
      {
         /* request new buffer for other clients */
         pthread_cond_signal( &(ptBufLock->condBufferOld) );
         isig = 1;
      }
   }
   else
   {
      ptBufLock->iReaders--;
      ptBufLock->iDone--;
   }

   ptBufLock->iAccess = 1;
   ptBufLock->iRequestors--;

   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(%d,%d): end ptrevReadUnlockLast(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, iClient, t1-t0,
             ptBufLock->iBuffer,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);

      if (isig == 1)
         printf("SSS %s(%d,%d): ReadUnlockLast signalled condBufferOld\n",
                cModule, iClient, t1-t0);
   }

   return 0;

} /* ptrevReadUnlockLast */

/********************************************************************/

int ptrevWriteLock( ptBufLock_t *ptBufLock, int iBufn)
{
   char cModule[32] = "ptData";
   time_t t1;

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(0,%d): begin ptrevWriteLock(%d,%d): %d read, %d done, %d write, %d request\n", 
             cModule, t1-t0,
             iBufn,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,   
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
   if (ptBufLock->iWriters)
   {
      /* max one writer allowed */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -1;
   }

   if (ptBufLock->iRequestors)
   {
      while (ptBufLock->iAccess == 0)
      {
         /* wait until at least one client accessed data */
         pthread_cond_wait( &(ptBufLock->condBufferOld),
                            &(ptBufLock->mutexBuffer) );
      }
   }
   else
      pthread_cond_wait( &(ptBufLock->condBufferOld),
                         &(ptBufLock->mutexBuffer) );

   while (ptBufLock->iReaders)
   {      /* buffer again read-locked due to some submission delay */

      pthread_cond_wait( &(ptBufLock->condBufferOld),
                         &(ptBufLock->mutexBuffer) );
   }

   ptBufLock->iWriters++;
   ptBufLock->iBuffer = iBufn;
   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(0,%d): end ptrevWriteLock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, t1-t0,
             ptBufLock->iBuffer,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   return 0;

} /* ptrevWriteLock */

/********************************************************************/

int ptrevWriteUnlock( ptBufLock_t *ptBufLock)
{
   char cModule[32] = "ptData";
   time_t t1;
   int isig = 0;

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(0,%d): begin ptrevWriteUnlock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, t1-t0,
             ptBufLock->iBuffer,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);
   }

   pthread_mutex_lock( &(ptBufLock->mutexBuffer) );
   if (ptBufLock->iWriters == 0)
   {
      /* at least own write lock should be there */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -1;
   }
   if (ptBufLock->iWriters > 1)
   {
      /* more than one write lock exists */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -2;
   }
   if (ptBufLock->iReaders)
   {
      /* not allowed during write lock */
      pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
      return -3;
   }

   ptBufLock->iWriters--;
   pthread_cond_broadcast( &(ptBufLock->condBufferNew) );
   isig = 1;

   ptBufLock->iAccess = 0;

   pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

   if (iDebug)
   {
      time(&t1);
      printf("DDD %s(0,%d): end ptrevWriteUnlock(%d,%d): %d read, %d done, %d write, %d request\n",
             cModule, t1-t0,
             ptBufLock->iBuffer,
             ptBufLock->iAccess,
             ptBufLock->iReaders,
             ptBufLock->iDone,
             ptBufLock->iWriters,
             ptBufLock->iRequestors);

      if (isig == 1)
         printf("SSS %s(0,%d): WriteUnlock broadcasted condBufferNew\n",
                cModule, t1-t0);
   }

   return 0;

} /* ptrevWriteUnlock */

/********************************************************************/

int ptrevWait( ptBufLock_t *ptBufLock,
               int iWaitTime,
               sptrevClient *pClient,
               sptrevDummy *pDummy)
{
   char cModule[32] = "ptServ";
   srevComm sCommBuf, *pCommBuf;
   int iCommSize = sizeof(srevComm);
   srevOldAckn sOldAckn, *pOldAckn;
   int iAcknSize = sizeof(srevOldAckn);
   srevOldHead *pOldHead;

   int iBufRequ;
   int iRC, iRet;
   int inum, inumold = 0;
   int isize = ntohl(pDummy->iSize);
   int iOldHead = 512;
   int iFirst = 1;

   struct timeval now;
   struct timespec timeout;

   pCommBuf = &sCommBuf;
   pOldAckn = &sOldAckn;

   if (pClient->iOld)                             /* old Paw client */ 
      iWaitTime = pClient->iTimeBuf;  /* individually set by client */

   iRC = ETIMEDOUT;
   while (iRC)
   {
gNextWait:
      iRC = gettimeofday(&now,0);
      if (iRC)
      {
         /* lock set by caller */
         pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );
         printf("-E- %s: unexpected rc(gettimeofday) = %d\n",
                cModule, iRC);
         iRet = -1;
      }
      timeout.tv_sec = now.tv_sec + iWaitTime;
      timeout.tv_nsec = 0;

      /* wait for condition for at most iWaitTime sec */
      iRC = pthread_cond_timedwait( &(ptBufLock->condBufferNew),
                                    &(ptBufLock->mutexBuffer),
                                    &timeout);
      if (iRC == ETIMEDOUT)
      {
         /* lock set by caller */
         pthread_mutex_unlock( &(ptBufLock->mutexBuffer) );

         if (pClient->iOld)                       /* old Paw client */ 
         {
            inumold++;
            iBufRequ = 1;              /* no end after flush buffer */
            pOldHead = (srevOldHead *) pDummy;
            if (iDebug)
            {
               printf("-D- %s(%d): after %d sec: send dummy buffer %d to old client (%d byte)\n", 
                      cModule, pClient->iClient,
                      iWaitTime, inumold, iOldHead);
               printf("    %s(%d): client message '%s'\n",
                      cModule, pClient->iClient,
                      pOldHead->c_message);
            }

            iRet = ptrevSend(pClient->iClient,
                             pClient->iSocket,
                             (char *) pDummy,
                             iOldHead);
            if (iRet)
            {
               printf("-E- %s(%d): sending dummy buffer to old client\n",
                      cModule, pClient->iClient);
               return 2;                           /* finish client */
            }
            if (iDebug)
               printf("-D- %s(%d): dummy buffer sent to old client (%d byte)\n",
                      cModule, pClient->iClient, iOldHead);

            /************ Receive acknowledgement buffer ************/
          
            iRet = ptrevRecv(pClient->iClient, 
                             pClient->iSocket,
                             (char *) pOldAckn,
                             iAcknSize);
            if (iRet)
            {              
               if (iRet < 0)
                  printf("-E- %s(%d): receiving acknowledgement buffer\n",
                         cModule, pClient->iClient);
               return 2;                           /* finish client */
            }
            if (iDebug)
               printf("    %s(%d): acknowledgement buffer received from old client (%d byte)\n",
                      cModule, pClient->iClient, iAcknSize);

            if ( (pOldAckn->l_clnt_sts != 1) &&
                 (pOldAckn->l_clnt_sts != 9) )
            {
               swaplw( &(pOldAckn->l_clnt_sts), 1, 0);
               if ( (pOldAckn->l_clnt_sts != 1) &&
                    (pOldAckn->l_clnt_sts != 9) )
               {
                  printf("    %s(%d): invalid acknowledgement buffer received from old client\n",
                         cModule, pClient->iClient);
                  return 2;                        /* finish client */
               }
            }
            else if (pOldAckn->l_clnt_sts == 9)
               return 1;                         /* client finished */

         } /* old Paw client */
         else
         {
            inum = ntohl(pDummy->iBufNo);
            inum++;
            if (iDebug)
               printf("    %s(%d): after %d sec: send dummy buffer %d to client\n",
                      cModule, pClient->iClient, iWaitTime, inum);
            pDummy->iBufNo = htonl(inum);
                                     /* no. of current dummy buffer */

            /* send dummy buffer to client */
            iRet = ptrevSend(pClient->iClient,
                             pClient->iSocket,
                             (char *) pDummy,
                             isize + sizeof(int));
            if (iRet)
            {
               printf("-E- %s(%d): sending dummy buffer to client\n",
                      cModule, pClient->iClient);
               return 2;                           /* finish client */
            }
            if (iDebug)
               printf("    %s(%d): dummy buffer %d (size %d, %d events) sent\n",
                      cModule, pClient->iClient,
                      inum, isize, ntohl(pDummy->iEvtNo));

            /* look if client maintains request */
            iRet = ptrevRecv(pClient->iClient,
                             pClient->iSocket,
                             (char *) pCommBuf,
                             iCommSize);
            if (iRet)
            {
               if (iRet < 0)
               {
                  printf("-E- %s(%d): receiving communication buffer again\n",
                         cModule, pClient->iClient);
                  return 2;                        /* finish client */
               }
               return 1;                /* client closed connection */
            } 

            iBufRequ = ntohl(pCommBuf->iBufRequ);
            if (iDebug)
               printf("-D- %s(%d): comm. buffer received again: %d, %d, %d\n",
                      cModule, pClient->iClient,
                      ntohl(pCommBuf->iSize),
                      ntohl(pCommBuf->iMode),
                      iBufRequ);

            if ( (ntohl(pCommBuf->iMode) != 1) ||
                 (ntohl(pCommBuf->iSize) != iCommSize-sizeof(int)) )
            {
               printf("-E- %s(%d): invalid communication buffer (data %d byte) received: %d, %d\n",
                      cModule, pClient->iClient,
                      ntohl(pCommBuf->iSize),
                      ntohl(pCommBuf->iMode),
                      iBufRequ);
               return 2;                           /* finish client */
            }
         } /* new ROOT client */

         if (iBufRequ <= 0)                      /* client finished */
            return 1;
         else
         {
            /* reestablish lock suspended during network transfer */
            pthread_mutex_lock( &(ptBufLock->mutexBuffer) );

            /* request new buffer to avoid extremely rare deadlock */
            pthread_cond_signal( &(ptBufLock->condBufferOld) );

            goto gNextWait;
         }

      } /* (iRC == ETIMEDOUT) */
      else if ( (iRC) && (iFirst == 1) )
      {
         printf("-W- %s(%d): unexpected rc(pthread_cond_timedwait) = %d\n",
                cModule, pClient->iClient, iRC);
         perror("    ");
         if (iDebug == 0) iFirst = 0;   /* only 1 msg in production */
      }

   } /* while (iRC) */

   return 0;                             /* condBufferNew signalled */

} /* ptrevWait */
