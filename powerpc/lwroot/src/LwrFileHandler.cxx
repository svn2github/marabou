//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSysEvtHandler                                                       //
//                                                                      //
// Abstract base class for handling system events.                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include "LwrFileHandler.h"
#include "LwrLynxOsSystem.h"

TLynxOsSystem * gSystem;

//______________________________________________________________________________
TFileHandler::TFileHandler(int fd, int mask)
{
   // Create a file descriptor event handler. If mask=kRead then we
   // want to monitor the file for read readiness, if mask=kWrite
   // then we monitor the file for write readiness, if mask=kRead|kWrite
   // then we monitor both read and write readiness.

	fFileNum = fd;
	if (!mask) mask = kRead;
	fMask = mask;
	fReadyMask = 0;
}

//______________________________________________________________________________
Bool_t TFileHandler::Notify()
{
   // Notify when event occured on descriptor associated with this handler.

   return kFALSE;
}

//______________________________________________________________________________
Bool_t TFileHandler::ReadNotify()
{
   // Notify when something can be read from the descriptor associated with
   // this handler.

   return kFALSE;
}

//______________________________________________________________________________
Bool_t TFileHandler::WriteNotify()
{
   // Notify when something can be written to the descriptor associated with
   // this handler.

   return kFALSE;
}

//______________________________________________________________________________
Bool_t TFileHandler::HasReadInterest()
{
   // True if handler is interested in read events.

   return (fMask & 1);
}

//______________________________________________________________________________
Bool_t TFileHandler::HasWriteInterest()
{
   // True if handler is interested in write events.

   return (fMask & 2);
}

//______________________________________________________________________________
void TFileHandler::SetInterest(Int_t mask)
{
   // Set interest mask to 'mask'.

   if (!mask)
      mask = kRead;
   fMask = mask;
}

//______________________________________________________________________________
void TFileHandler::Add()
{
   // Add file event handler to system file handler list.

	if (gSystem && fFileNum != -1)  gSystem->AddFileHandler(this);
}

//______________________________________________________________________________
void TFileHandler::Remove()
{
   // Remove file event handler from system file handler list.

   if (gSystem && fFileNum != -1) gSystem->RemoveFileHandler(this);
}
