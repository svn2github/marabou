//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMonitor                                                             //
//                                                                      //
// This class monitors activity on a number of network sockets.         //
// The actual monitoring is done by TSystem::DispatchOneEvent().        //
// Typical usage: create a TMonitor object. Register a number of        //
// TSocket objects and call TMonitor::Select(). Select() returns the    //
// socket object which has data waiting. TSocket objects can be added,  //
// removed, (temporary) enabled or disabled.                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include "LwrMonitor.h"
#include "LwrFileHandler.h"
#include "LwrLynxOsSystem.h"

extern TLynxOsSystem * gSystem;

//---- Socket event handler ----------------------------------------------------
//
// This utility class is only used by TMonitor.
//

class TSocketHandler : public TFileHandler {

private:
	TMonitor * fMonitor; 	//monitor to which this handler belongs
	TSocket * fSocket;		//socket being handled

public:
	TSocketHandler(TMonitor * Mon, TSocket *Sock, Int_t Interest);
	Bool_t Notify();
	Bool_t ReadNotify() { return Notify(); }
	Bool_t WriteNotify() { return Notify(); }
	TSocket * GetSocket() const { return fSocket; }
};

TSocketHandler::TSocketHandler(TMonitor * Mon, TSocket * Sock, Int_t Interest)
										: TFileHandler(Sock->GetDescriptor(), Interest)
{
	fMonitor = Mon;
	fSocket  = Sock;
	this->Add();
}

Bool_t TSocketHandler::Notify()
{
	fMonitor->SetReady(fSocket);
	return(kTRUE);
}

//______________________________________________________________________________
TMonitor::TMonitor()
{
   // Create a monitor object. If mainloop is true the monitoring will be
   // done in the main event loop.

	fActive   = new TList();
	fDeactive = new TList();
	fReady = NULL;
}

//______________________________________________________________________________
TMonitor::~TMonitor()
{
   // Cleanup the monitor object. Does not delete socket being monitored.

	fActive->Delete();
	fDeactive->Delete();
}

//______________________________________________________________________________
void TMonitor::Add(TSocket * Sock, Int_t Interest)
{
   // Add socket to the monitor's active list. If interest=kRead then we
   // want to monitor the socket for read readiness, if interest=kWrite
   // then we monitor the socket for write readiness, if interest=kRead|kWrite
   // then we monitor both read and write readiness.

	fActive->Add(new TSocketHandler(this, Sock, Interest));
}
//______________________________________________________________________________
void TMonitor::SetInterest(TSocket * Sock, Int_t Interest)
{
   // Set interest mask for socket sock to interest. If the socket is not
   // in the active list move it or add it there.
   // If interest=kRead then we want to monitor the socket for read readiness,
   // if interest=kWrite then we monitor the socket for write readiness,
   // if interest=kRead|kWrite then we monitor both read and write readiness.

	TSocketHandler * s = 0;

	if (!Interest) Interest = kRead;

   // Check first the activated list ...
	TIter next(fActive);
	while ((s = (TSocketHandler *) next())) {
		if (Sock == s->GetSocket()) {
			s->SetInterest(Interest);
			return;
		}
	}

	// Check now the deactivated list ...
	TIter next1(fDeactive);
	while ((s = (TSocketHandler *) next1())) {
		if (Sock == s->GetSocket()) {
			fDeactive->Remove(s);
			fActive->Add(s);
			s->SetInterest(Interest);
			return;
		}
	}

	// The socket is not in our lists: just add it
	fActive->Add(new TSocketHandler(this, Sock, Interest));
}

//______________________________________________________________________________
void TMonitor::Remove(TSocket * Sock)
{
   // Remove a socket from the monitor.

	TIter next(fActive);
	TSocketHandler * s;

	while ((s = (TSocketHandler *) next())) {
		if (Sock == s->GetSocket()) {
			fActive->Remove(s);
			delete s;
			return;
		}
	}

	TIter next1(fDeactive);

	while ((s = (TSocketHandler *) next1())) {
		if (Sock == s->GetSocket()) {
			fDeactive->Remove(s);
			delete s;
			return;
		}
	}
}

//______________________________________________________________________________
void TMonitor::RemoveAll()
{
   // Remove all sockets from the monitor.

	fActive->Delete();
	fDeactive->Delete();
}

//______________________________________________________________________________
void TMonitor::Activate(TSocket * Sock)
{
   // Activate a de-activated socket.

	TIter next(fDeactive);
	TSocketHandler *s;

	while ((s = (TSocketHandler *) next())) {
		if (Sock == s->GetSocket()) {
			fDeactive->Remove(s);
			fActive->Add(s);
			s->Add();
			return;
		}
	}
}

//______________________________________________________________________________
void TMonitor::ActivateAll()
{
	// Activate all de-activated sockets.

	TIter next(fDeactive);
	TSocketHandler *s;

	while ((s = (TSocketHandler *) next())) {
		fActive->Add(s);
		s->Add();
	}
	fDeactive->Clear();
}

//______________________________________________________________________________
void TMonitor::Deactivate(TSocket * Sock)
{
	// De-activate a socket.

	TIter next(fActive);
	TSocketHandler *s;

	while ((s = (TSocketHandler *) next())) {
		if (Sock == s->GetSocket()) {
			fActive->Remove(s);
			fDeactive->Add(s);
			s->Remove();
			return;
		}
	}
}

//______________________________________________________________________________
void TMonitor::DeactivateAll()
{
   // De-activate all activated sockets.

	TIter next(fActive);
	TSocketHandler *s;

	while ((s = (TSocketHandler *) next())) {
		fDeactive->Add(s);
		s->Remove();
	}
	fActive->Clear();
}

//______________________________________________________________________________
TSocket * TMonitor::Select(Long_t Timeout)
{
   // Return pointer to socket for which an event is waiting.
   // Wait a maximum of timeout milliseconds.
   // If return is due to timeout it returns (TSocket *)-1.
   // Return 0 in case of any other error situation.

	TSocketHandler * h;
	if (gSystem->Select(fActive, Timeout) > 0) {
		TIter next(fActive);
		while ((h = (TSocketHandler *)next())) {
			if (h->IsReadReady()) return(h->GetSocket());
			if (h->IsWriteReady()) return(h->GetSocket());
		}
	}
	return(NULL);
}

//______________________________________________________________________________
Int_t TMonitor::Select(TList * Rdready, TList * Wrready, Long_t Timeout)
{
   // Return numbers of sockets that are ready for reading or writing.
   // Wait a maximum of timeout milliseconds.
   // Return 0 if timed-out. Return < 0 in case of error.
   // If rdready and/or wrready are not 0, the lists of sockets with
   // something to read and/or write are also returned.

	Int_t nr = -2;

	nr = gSystem->Select(fActive, Timeout);

	TSocketHandler * h;
	if (nr > 0 && (Rdready || Wrready)) {
		// Clear the lists
		if (Rdready) Rdready->Clear();
		if (Wrready) Wrready->Clear();
		TIter next(fActive);
		while ((h = (TSocketHandler *)next())) {
			if (Rdready && h->IsReadReady()) Rdready->Add(h->GetSocket());
			if (Wrready && h->IsWriteReady()) Wrready->Add(h->GetSocket());
		}
	}

	return nr;
}

//______________________________________________________________________________
void TMonitor::SetReady(TSocket * Sock)
{
   // Called by TSocketHandler::Notify() to signal which socket is ready
   // to be read or written. User should not call this routine. The ready
   // socket will be returned via the Select() user function.
   // Ready(TSocket *sock) signal is emitted.

	fReady = Sock;
}

//______________________________________________________________________________
Int_t TMonitor::GetActive() const
{
	// Return number of sockets in the active list.

	return fActive->GetSize();
}

//______________________________________________________________________________
Int_t TMonitor::GetDeactive() const
{
	// Return number of sockets in the de-active list.

	return fDeactive->GetSize();
}

//______________________________________________________________________________
Bool_t TMonitor::IsActive(TSocket * Sock) const
{
   // Check if socket 's' is in the active list. Avoids the duplication
   // of active list via TMonitor::GetListOfActives().

	TIter next(fActive);
	while (TSocketHandler *h = (TSocketHandler*) next())
		if (Sock == h->GetSocket()) return kTRUE;

	// Not found
	return kFALSE;
}

//______________________________________________________________________________
TList *TMonitor::GetListOfActives() const
{
   // Returns a list with all active sockets. This list must be deleted
   // by the user. DO NOT call Delete() on this list as it will delete
   // the sockets that are still being used by the monitor.

	TList * list = new TList();

	TIter next(fActive);
	while (TSocketHandler * h = (TSocketHandler*) next()) list->Add(h->GetSocket());
	return list;
}

//______________________________________________________________________________
TList *TMonitor::GetListOfDeactives() const
{
   // Returns a list with all de-active sockets. This list must be deleted
   // by the user. DO NOT call Delete() on this list as it will delete
   // the sockets that are still being used by the monitor.

	TList * list = new TList();

	TIter next(fDeactive);
	while (TSocketHandler * h = (TSocketHandler*) next()) list->Add(h->GetSocket());
	return list;
}
