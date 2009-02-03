#ifndef __LwrServerSocket_h__
#define __LwrServerSocket_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrServerSocket.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TServerSocket<br>
//! 				This class implements server sockets.<br>
//! 				A server socket waits for requests to come in over the network.<br>
//! 				It performs some operation based on that request and
//! 				then possibly returns a result to the requester.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"
#include "LwrSocket.h"

class TServerSocket : public TSocket {

private:
   TServerSocket() { fIsServerSocket = kFALSE; };
   TServerSocket(const TServerSocket &);
   void operator=(const TServerSocket &);

public:
   enum { kDefaultBacklog = 10 };

   TServerSocket(Int_t port, Bool_t reuse = kFALSE, Int_t backlog = kDefaultBacklog);
   TServerSocket(const char *service, Bool_t reuse = kFALSE, Int_t backlog = kDefaultBacklog);
   virtual ~TServerSocket() { Close(); }

   virtual TSocket      *Accept();
   virtual TInetAddress  GetLocalInetAddress();
   virtual Int_t         GetLocalPort();

   inline const Char_t * ClassName() const { return "TServerSocket"; };
};

#endif
