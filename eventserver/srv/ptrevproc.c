/********************************************************************
 * ptrevproc.c
 * utility functions for multithreaded remote event server prototype
 * implemented 17. 2.99, Horst Goeringer
 *******************************************************************
 * ptrevRecv:     receive data buffer via socket
 * ptrevSend:     send data buffer via socket
 * ptrevSends:    send data via socket, check before if socket ready
 * ptrevTime:     provides string with actual time and date
 *******************************************************************
 */

#include <pthread.h>                    /* should be first include */
#include <stdio.h>
#include <socket.h>
#include <select.h>
#include <time.h>
#include <locale.h>

#include <netinet/in.h>


extern int iDebug;
extern int iTimeOut;

/********************************************************************/

int ptrevRecv(int iClient, int iSocket, char *pcBuffer, int iBufSize)
{
   char cModule[32] = "ptrevRecv";
   char *pcBuf;
   char cMsg[256] = "";
   int iSize;
   int iRC;

   pcBuf = pcBuffer;
   iSize = iBufSize;
   while (iSize > 0)
   {
      iRC = recv(iSocket, pcBuf, iSize, 0);
      if (iRC < 0)
      {
         sprintf(cMsg, "-E- %s(%d): receiving data: rc = %d",
                 cModule, iClient, iRC);
         perror(cMsg);
         return -1;
      }
      if (iRC == 0)
      {
         if (iDebug)
         {
            printf("-D- %s(%d): sender closed connection\n",
                cModule, iClient);
            printf("    %s(%d): %d bytes requested, %d received\n",
                   cModule, iClient,
                   iBufSize, iBufSize-iSize);
         }
         return 1;
      }
      iSize -= iRC;
      pcBuf += iRC;

   } /* while (iSize > 0) */

   return 0;                                      /* okay */

} /* ptrevRecv */

/********************************************************************/

int ptrevSend(int iClient, int iSocket, char *pcBuffer, int iBufSize)
{
   char cModule[32] = "ptrevSend";
   char *pcBuf;
   char cMsg[256] = "";
   int iSize;
   int iRC;

   pcBuf = pcBuffer;
   iSize = iBufSize;
   while (iSize > 0)
   {
      iRC = send(iSocket, pcBuf, iSize, 0);
      if (iRC < 0)
      {
         sprintf(cMsg, "-E- %s(%d): sending data: rc = %d",
                 cModule, iClient, iRC);
         perror(cMsg);
         return -1;
      }
      if (iRC == 0) break;
      iSize -= iRC;
      pcBuf += iRC;
   } /* while (iSize > 0) */

   if (iSize)
   {
      printf("-E- %s(%d): %d requested, but %d sent\n",
             cModule, iClient,
             iBufSize, iBufSize-iSize);
      return -1;
   }

   return 0;                                      /* okay */

} /* ptrevSend */

/********************************************************************/

int ptrevSends(int iClient, int iSocket, char *pcBuffer, int iBufSize)
{
   char cModule[32] = "ptrevSends";
   char *pcBuf;
   char cMsg[256] = "";
   int iSize;
   int iRC;
   int iLoop = 0;

   fd_set iRead, iExcept, iWrite;
   struct timeval sTimeOut;

   if (iDebug)
      printf("XXX %s(%d): timeout %d s\n", cModule, iClient, iTimeOut);

   FD_ZERO(&iRead);
   FD_ZERO(&iExcept);
   FD_ZERO(&iWrite);

   pcBuf = pcBuffer;
   iSize = iBufSize;
   while (iSize > 0)
   {
      iLoop++;
      sTimeOut.tv_sec = iTimeOut;
      sTimeOut.tv_usec = 0;

      FD_SET(iSocket, &iWrite);
      iRC = select(32, NULL, &iWrite, NULL, &sTimeOut);
      if (iRC < 0)
      {
         sprintf(cMsg, "-E- %s(%d): checking socket for write: rc = %d",
                 cModule, iClient, iRC);
         perror(cMsg);
         return -1;
      }
      if (iRC == 0)
      {
        printf("-W- %s(%d): socket not ready for write (within %d s)\n",
               cModule, iClient, iTimeOut);
        return 1;
      }
      if (iDebug)
      {
         printf("\nXXX %s(%d): select(%d) okay, rc = %d, ",
                cModule, iClient, iLoop, iRC);
         fflush(stdout);
      }

      iRC = send(iSocket, pcBuf, iSize, 0);
      if (iDebug)
      {
         printf(" (%d) ", iRC);
         fflush(stdout);
      }
      if (iRC < 0)
      {
         sprintf(cMsg, "-E- %s(%d): sending data: rc = %d",
                 cModule, iClient, iRC);
         perror(cMsg);
         return -1;
      }
      if (iRC == 0) break;
      else if (iDebug)
      {
         printf(":%d", iRC);
         fflush(stdout);
      }

      iSize -= iRC;
      pcBuf += iRC;
   } /* while (iSize > 0) */

   if (iSize)
   {
      printf("-E- %s(%d): %d bytes requested, but %d sent\n",
             cModule, iClient,
             iBufSize, iBufSize-iSize);
      return -1;
   }

   if (iDebug)
      printf("\nXXX %s(%d), send okay\n", cModule, iClient);
   return 0;                                      /* okay */

} /* ptrevSends */

/********************************************************************/

int ptrevTime(time_t *tTime, char *pcBuffer, int iSize)
{
   char cModule[32] = "ptrevTime";
   char cMsg[256];
   int iRC;
   time_t timeReq;
   int itmSize;
   struct tm *ptm0, tmStr, *ptmStr;      /* time and date structure */

   if (iDebug)
      printf("-D- %s: provide current time\n", cModule);      

   ptmStr = &tmStr;
   itmSize = sizeof(tmStr);
   setlocale(LC_ALL, "C");
   timeReq = time(tTime);             /* tTime = NULL: current time */
   ptm0 = localtime(&timeReq);       /* tm structure with req. time */
   memcpy(ptmStr, ptm0, itmSize);       /* try to make it reentrant */

   /* format time string */
   iRC = strftime(pcBuffer, iSize, "%T, %A, %B %d, %Y", ptmStr);
   if (iRC == 0)
   {
      sprintf(cMsg, "-E- %s: strftime", cModule);
      perror(cMsg);
      return -1;
   }

   if (iDebug)
      printf("-D- %s\n", pcBuffer);

   return 0;

} /* ptrevTime */
