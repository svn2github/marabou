/*********************************************************************
 * ptrevclient.h
 * client control structure passed to server threads
 * created 22.12.1998, Horst Goeringer
 *********************************************************************
 * 10.12.1999, H.G.: enhancements for old clients
 *********************************************************************
 */

typedef struct
{
   int iClient;                            /* number current client */
   int iSocket;                     
   int iOld;                               /* = 1: old (Paw client) */
   int iTimeBuf;     /* max wait time for events (old clients only) */
} sptrevClient;
