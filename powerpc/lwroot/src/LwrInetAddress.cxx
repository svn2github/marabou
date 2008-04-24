//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TInetAddress                                                         //
//                                                                      //
// This class represents an Internet Protocol (IP) address.             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrInetAddress.h"

//______________________________________________________________________________
TInetAddress::TInetAddress()
{
   // Default ctor. Used in case of unknown host. Not a valid address.

   fHostname = "UnknownHost";
   fAddress  = 0;
   fFamily   = -1;
   fPort     = -1;
}

//______________________________________________________________________________
TInetAddress::TInetAddress(const char *host, UInt_t addr, Int_t family, Int_t port)
{
   // Create TInetAddress. Private ctor. TInetAddress objects can only
   // be created via the friend classes TSystem, TServerSocket and TSocket.
   // Use the IsValid() method to check the validity of a TInetAddress.

   fAddress = addr;
   if (!strcmp(host, "????"))
      fHostname = GetHostAddress();
   else
      fHostname = host;
   fFamily = family;
   fPort   = port;
}

//______________________________________________________________________________
TInetAddress::TInetAddress(const TInetAddress &adr)
{
   // TInetAddress copy ctor.

   fHostname = adr.fHostname;
   fAddress  = adr.fAddress;
   fFamily   = adr.fFamily;
   fPort     = adr.fPort;
}

//______________________________________________________________________________
TInetAddress& TInetAddress::operator=(const TInetAddress &rhs)
{
   // TInetAddress assignment operator.

   if (this != &rhs) {
      fHostname = rhs.fHostname;
      fAddress  = rhs.fAddress;
      fFamily   = rhs.fFamily;
      fPort     = rhs.fPort;
   }
   return *this;
}

//______________________________________________________________________________
UChar_t *TInetAddress::GetAddressBytes() const
{
   // Returns the raw IP address in host byte order. The highest
   // order byte position is in addr[0]. To be prepared for 64-bit
   // IP addresses an array of bytes is returned.

   static UChar_t addr[4];

   addr[0] = (UChar_t) ((fAddress >> 24) & 0xFF);
   addr[1] = (UChar_t) ((fAddress >> 16) & 0xFF);
   addr[2] = (UChar_t) ((fAddress >> 8) & 0xFF);
   addr[3] = (UChar_t) (fAddress & 0xFF);

   return addr;
}

//______________________________________________________________________________
const char *TInetAddress::GetHostAddress() const
{
   // Returns the IP address string "%d.%d.%d.%d".
   // Copy string immediately, it will be reused.

   return Form("%d.%d.%d.%d", (fAddress >> 24) & 0xFF,
                              (fAddress >> 16) & 0xFF,
                              (fAddress >>  8) & 0xFF,
                               fAddress & 0xFF);
}
