//////////////////////////////////////////////////////////////////////
// Copyright:
//   GSI, Gesellschaft fuer Schwerionenforschung mbH
//   Planckstr. 1
//   D-64291 Darmstadt
//   Germany  
//   created in Feb 1999 by Horst Goeringer
//////////////////////////////////////////////////////////////////////
// MRevCli.C
//   get specified no. of events from MBS remote event server
//   pass no. of events to macro
//   use int pointers for event access
//////////////////////////////////////////////////////////////////////
// 29. 4.1999, H.G.: measure data rates
//  6. 5.1999, H.G.: request loop
// 21. 6.1999, H.G.: buffer flush
//  6. 3.2000, H.G.: use function RevStatus
//////////////////////////////////////////////////////////////////////

Int_t MRevCli(Int_t iEvt=11)
{
   char *pcNode = "mowgli";  // server node name 
   Int_t iPort = 0;          // take default port no. MBS event server
   Int_t iDebug = 3;         // debug level
   Int_t iOpen = 1;          // no. of event requests
   Int_t iFlush = 1;         // = 1: flush current event buffer

   Int_t iStatus = -1;       // status information
   Int_t iOut = 0;           // no status output to stdout by default
   Int_t iData = 0;          // = 1: print event parameters from macro
   Int_t iError = 0;
   Int_t ii, jj, ifl;
   Int_t iBufsize;
   Int_t iEvtsize;
   Int_t iEvthead = 4;           // MBS event header (2-byte words)

   Float_t rsum = 0, rate = 0;
   Double_t dRTime;
   Double_t dCTime;

   TStopwatch pClock;
   TSocket *pSocket;
   int *piEvent;
   MRevBuffer *buf = new MRevBuffer(iDebug);

   if (iDebug == 1) iOut = 1;            // status output to stdout
   for (jj=1; jj<=iOpen;jj++)           // request loop
   {
      ii = 0;
      rsum = 0;
      if (iOpen > 1) printf("\n**** request %d:", jj);
      else printf("   ");
      if (iFlush)
      {
         ifl = 1;                               // skip current buffer
         printf(" skip current buffer\n");
      }
      else
      {
         ifl = 0;
         printf("\n");
      }

      iStatus = buf->RevStatus (iOut);
      pSocket = buf->RevOpen (pcNode, iPort, iEvt);
      iStatus = buf->RevStatus (iOut);
      if (!pSocket) return 8;
      
      pClock.Start(kTRUE);
      for (;;)                                          // event loop
      {
         if (ii > 0) ifl = 0;    // skip buffer only once per request
         piEvent = buf->RevGetI (pSocket, ifl);  // ptr to next event
         iStatus = buf->RevStatus (iOut);
         if (!piEvent)
         {
            pClock.Stop();
            if (iStatus == 3)         // currently no events from DAQ
            {
               ifl = 0;         // new buffer, needs not be specified
               continue;
            }

            if (ii != iEvt)
            {
               if (iEvt == 0)
                  printf("-I- %d events received\n", ii);
               else
               { 
                  iError = 1;
                  printf("-E- %d events expected, %d events received\n",
                         iEvt, ii);
               }
            }
            else
               printf("-I- all %d events received:\n", iEvt);

            break;
         } // (!piEvent)

         iEvtsize = (piEvent[0] + iEvthead)*2;
         rsum += iEvtsize;
         ii++;
         if (ii == 1) iBufsize = buf->RevBufsize();

         if (iData)
         {
            printf("-D- event %d: no. %d, length %d\n", ii,
                   piEvent[3],
                   piEvent[0]);
            for (Int_t kk=0; kk<(iEvtsize/4); kk++)
               printf("%d: %d\n", kk+1, piEvent[kk]);
         }
      } // event loop

      dRTime = pClock.RealTime();
      dCTime = pClock.CpuTime();
      if (dRTime)
      {
         rate = rsum/(dRTime*1024);
         if (rsum >= 1.e+10)
            printf("    %.0g Bytes in %.0f sec, rate %.1f kB/sec\n",
                  rsum, dRTime, rate);
         else
            printf("    %.0f Bytes in %.0f sec, rate %.1f kB/sec\n",
                  rsum, dRTime, rate);
      }  
      else
         printf("    buffer size %d, evt size %d, time: real %d s, CPU %d s\n",
                iBufsize, iEvtsize, dRTime, dCTime);

      if (iError) break;

   } // request loop

   buf->RevClose (pSocket);
   iStatus = buf->RevStatus (iOut);
   buf->~MRevBuffer();    // calls also destructor for event

   if (iError) return 4;
   else return 0;

}
