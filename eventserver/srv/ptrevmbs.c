/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 17. 2.1999 by Horst Goeringer
 ********************************************************************
 * ptrevmbs.c
 *    mbs specific functions for multithreaded remote event server
 *******************************************************************
 * ptrevGetEvtno: get number of events in MBS buffer
 *******************************************************************
 * 10.12. 2001, H.G.: socket.h included 
 *******************************************************************
 */

#include <pthread.h>                    /* should be first include */
#include <socket.h>

#include <netinet/in.h>

extern int iDebug;

/********************************************************************/

int ptrevGetEvtno(int *piBuf, int iSwap)
{
   char cModule[32] = "ptrevGetEvtno";
   int iEvtno;
   int iRC;

   iEvtno = piBuf[4];
   if (iSwap)
      iRC = (int) swaplw( &iEvtno, 1, 0);

   if (iDebug)
      printf("-D- %s: %d buffer elements\n", cModule, iEvtno);

   return iEvtno;

} /* ptrevGetEvtno */
