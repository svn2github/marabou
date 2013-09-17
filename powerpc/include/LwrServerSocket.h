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
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.6 $     
//! $Date: 2009-02-17 08:02:26 $
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

   TServerSocket(Int_t Port, Bool_t Reuse = kFALSE, Int_t Backlog = kDefaultBacklog);
   TServerSocket(const Char_t * Service, Bool_t Reuse = kFALSE, Int_t Backlog = kDefaultBacklog);
   virtual ~TServerSocket() { Close(); }

   virtual TSocket      *Accept();
   virtual TInetAddress  GetLocalInetAddress();
   virtual Int_t         GetLocalPort();

   inline const Char_t * ClassName() const { return "TServerSocket"; };
};

#endif
