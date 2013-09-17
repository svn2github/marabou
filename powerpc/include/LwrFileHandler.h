#ifndef __LwrFileHandler_h__
#define __LwrFileHandler_h__

#include "LwrObject.h"

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrFileHandler.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TFileHandler<br>
//! 				Handles events on file descriptors.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

class TFileHandler : public TObject {

protected:
   int  fFileNum;     //File descriptor
   int  fMask;        //Event interest mask, either bit 1 (read), 2 (write) or both can be set
   int  fReadyMask;   //Readiness mask, either bit 1 (read), 2 (write) or both can be set

   TFileHandler(): fFileNum(-1), fMask(0), fReadyMask(0) { }

public:
   enum { kRead = 1, kWrite = 2 };

   TFileHandler(int fd, int mask);
   virtual ~TFileHandler() { Remove(); }
   int             GetFd() const { return fFileNum; }
   void            SetFd(int fd) { fFileNum = fd; }
   virtual Bool_t  Notify();
   virtual Bool_t  ReadNotify();
   virtual Bool_t  WriteNotify();
   virtual Bool_t  HasReadInterest();
   virtual Bool_t  HasWriteInterest();
   virtual void    SetInterest(Int_t mask);
   virtual void    ResetReadyMask() { fReadyMask = 0; }
   virtual void    SetReadReady() { fReadyMask |= 0x1; }
   virtual void    SetWriteReady() { fReadyMask |= 0x2; }
   virtual Bool_t  IsReadReady() const { return (fReadyMask & 0x1) == 0x1; }
   virtual Bool_t  IsWriteReady() const { return (fReadyMask & 0x2) == 0x2; }
   virtual void    Add();
   virtual void    Remove();

   inline const Char_t * ClassName() const { return "TFileHandler"; };
};

#endif
