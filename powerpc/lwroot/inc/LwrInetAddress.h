#ifndef __LwrInetAddress_h__
#define __LwrInetAddress_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrInetAddress.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TInetAddress
//! 				This class represents an Internet Protocol (IP) address.
//! 				Objects of this class can not be created directly,
//! 				but only via the TSystem GetHostByName(), GetSockName(),
//! 				and GetPeerName() members and via members of the TServerSocket
//! 				and TSocket classes.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"
#include "LwrString.h"

class TInetAddress : public TNamed {

friend class TLynxOsSystem;
friend class TSocket;
friend class TServerSocket;

private:
   TString fHostname;    // full qualified hostname
   UInt_t  fAddress;     // IP address in host byte order
   Int_t   fFamily;      // address family
   Int_t   fPort;        // port through which we are connected

   TInetAddress(const char *host, UInt_t addr, Int_t family, Int_t port = -1);

public:
   TInetAddress();
   TInetAddress(const TInetAddress &adr);
   TInetAddress &operator=(const TInetAddress &rhs);
   virtual ~TInetAddress() { }

   UInt_t      GetAddress() const { return fAddress; }
   UChar_t    *GetAddressBytes() const;
   const char *GetHostAddress() const;
   const char *GetHostName() const { return (const char *) fHostname; }
   Int_t       GetFamily() const { return fFamily; }
   Int_t       GetPort() const { return fPort; }
   Bool_t      IsValid() const { return fFamily == -1 ? kFALSE : kTRUE; }

   inline const Char_t * ClassName() const { return "TInetAddress"; };
};

#endif
