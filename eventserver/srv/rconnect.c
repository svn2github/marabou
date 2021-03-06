/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt  
 *   Germany
 * created 26. 1.1996 by Horst Goeringer
 *********************************************************************
 * rconnect.c
 * open connection to specified server 
 *********************************************************************
 *  7. 4.1999, H.G.: return error number in case of failure
 *                   pass max time for retries of connect()
 *  3. 3.2000, H.G.: rename iExit to imySigS
 *                   *piMaxTime: meaning of values -1 and 0 exchanged
 *  7. 3.2000, H.G.: renamed form rawConnect to rconnect
 *********************************************************************
 */  
 
#include <stdio.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <socket.h>
#include <errno.h>
#include <netdb.h>

#include <netinet/in.h>

extern int imySigS;                      /* if = 1: CTL C specified */

/********************************************************************/

int rconnect( char *cNode,        /* input:  name of remote host */
              int iPort,          /* input:  port number */
              int *piMaxTime,     /* input:  max time for connect */
                                  /* = -1: try only once */
                                  /* =  0: try until success */
                                  /* >  0: try at most *piMaxTime s */
                                  /* output: time needed for connect */
              int *piSocket)      /* output: socket number */
{
   char cModule[32] = "rconnect";
   int iDebug = 0; 
   int iSocket = 0; 
   int iMaxTime = *piMaxTime; 
   int iTime = 0; 
   int iError;
   int iSleep = 0;
   int iMaxSleep = 10;             /* max sleep time */

   struct hostent *pHE = NULL;
   struct hostent  sHE;
   struct sockaddr_in sSockAddr;
   unsigned long lAddr;
 
   if ( ( pHE = gethostbyname(cNode) ) == NULL ) 
   {
      lAddr = inet_addr(cNode);
      if ( ( pHE = gethostbyaddr( 
               (char *)&lAddr, sizeof(lAddr), AF_INET ) ) == NULL )
      {
         fprintf(stderr,"-E- %s: unknown host %s\n", cModule, cNode );
         iError = -1;
         goto gError;
      }
      if (iDebug) 
         printf("-D- %s: gethostbyaddr succeeded\n", cModule);
   }
   else if (iDebug) 
      printf("-D- %s: gethostbyname succeeded\n", cModule);
   sHE = *pHE;                                        /* safe copy */

gRetryConnect:;

   /* create the socket */
   if ( ( iSocket = socket( 
            AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
   {
      perror("-E- rconnect(socket)");
      iError = errno;
      goto gError;
   }
 
   sSockAddr.sin_family = AF_INET;
   sSockAddr.sin_port   = 0;
   sSockAddr.sin_addr.s_addr = INADDR_ANY;
                                 /* bind a socket */
   if ( bind ( iSocket,
               (struct sockaddr *) &sSockAddr,
               sizeof(sSockAddr) ) == -1 )
   {
      perror("-E- rconnect(bind)");
      iError = errno;
      goto gError;
   }
 
   sSockAddr.sin_family = AF_INET;
   sSockAddr.sin_port   = htons( iPort );
   sSockAddr.sin_addr = * ( (struct in_addr *) sHE.h_addr );
                                 /* contact remote server */
   if ( connect( iSocket,
                 (struct sockaddr *) &sSockAddr,
                 sizeof(sSockAddr) ) == -1 )
   {
      /* if not successful, retry.  Possibly server not yet up. */
      if ( (errno == ECONNREFUSED) &&
           ( (iTime < iMaxTime) || (iMaxTime == -1) ) )
      {
         close(iSocket); 
         iSocket = -1;
         if (imySigS) goto gError;            /* CTL C specified */

         if (iSleep < iMaxSleep) iSleep++;
         iTime += iSleep;
         if (iDebug)
            printf("    still trying (after %d sec) ...\n",iSleep);
         sleep(iSleep);
         goto gRetryConnect;
      }
      perror("-E- rconnect(connect)");
      iError = errno;
      goto gError;
   }

   *piMaxTime = iTime;         /* return time needed for connect */
   *piSocket = iSocket;        /* return socket number */
   return(0);                  /* no error */

gError: 
   if (iSocket)
   {
      shutdown(iSocket, 2);
      close(iSocket);
   }

   *piSocket = -1;
   *piMaxTime = -1;
   return(iError);
}
