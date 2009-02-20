//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrMonitor.cxx
//! \brief			Light Weight ROOT: TMonitor
//! \details		This class monitors activity on a number of network sockets.<br>
//! 				The actual monitoring is done by TSystem::DispatchOneEvent().<br>
//! 				Typical usage:
//! 				<ul>
//! 				<li>	create a TMonitor object.
//! 				<li>	register a number of TSocket objects
//! 				<li>	call TMonitor::Select()
//! 				</ul>
//! 				Select() returns the socket object which has data waiting.<br>
//! 				TSocket objects can be added, removed, (temporary) enabled or disabled.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-20 08:33:53 $
//////////////////////////////////////////////////////////////////////////////


#include "LwrMonitor.h"
#include "LwrFileHandler.h"
#include "LwrLynxOsSystem.h"

extern TLynxOsSystem * gSystem;

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Socket event handler
//! 				(only used by TMonitor)
/////////////////////////////////////////////////////////////////////////////

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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Socket event handler
//! 				(only used by TMonitor)
//! \param[in]		Mon 		-- TMonitor object
//! \param[in]		Sock		-- socket
//! \param[in]		Interest	-- i/o mode
/////////////////////////////////////////////////////////////////////////////

TSocketHandler::TSocketHandler(TMonitor * Mon, TSocket * Sock, Int_t Interest)
										: TFileHandler(Sock->GetDescriptor(), Interest)
{
	fMonitor = Mon;
	fSocket  = Sock;
	this->Add();
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Notify event
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t TSocketHandler::Notify()
{
	fMonitor->SetReady(fSocket);
	return(kTRUE);
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Create a monitor object
/////////////////////////////////////////////////////////////////////////////

TMonitor::TMonitor()
{
	fActive   = new TList();
	fDeactive = new TList();
	fReady = NULL;
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Cleanup the monitor object.
//! 				Does not delete socket being monitored.
/////////////////////////////////////////////////////////////////////////////

TMonitor::~TMonitor()
{
	fActive->Delete();
	fDeactive->Delete();
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Adds socket to the monitor's active list.<br>
//!					<ul>
//! 				<li>	interest=kRead: we want to monitor the socket for read readiness,<br>
//! 				<li>	interest=kWrite: we monitor the socket for write readiness,<br>
//! 				<li>	interest=kRead|kWrite: we monitor both read and write readiness.
//! 				</ul>
//! \param[in]		Sock		-- socket
//! \param[in]		Interest	-- {r, w, rw}
/////////////////////////////////////////////////////////////////////////////

void TMonitor::Add(TSocket * Sock, Int_t Interest)
{
	fActive->Add(new TSocketHandler(this, Sock, Interest));
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets interest mask for socket.<br>
//!					<ul>
//! 				<li>	interest=kRead: we want to monitor the socket for read readiness,<br>
//! 				<li>	interest=kWrite: we monitor the socket for write readiness,<br>
//! 				<li>	interest=kRead|kWrite: we monitor both read and write readiness.
//! 				</ul>
//! \param[in]		Sock		-- socket
//! \param[in]		Interest	-- {r, w, rw}
/////////////////////////////////////////////////////////////////////////////

void TMonitor::SetInterest(TSocket * Sock, Int_t Interest)
{
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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Remove a socket from the monitor.
//! \param[in]		Sock		-- socket
/////////////////////////////////////////////////////////////////////////////

void TMonitor::Remove(TSocket * Sock)
{
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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Remove all sockets from the monitor.
/////////////////////////////////////////////////////////////////////////////

void TMonitor::RemoveAll()
{
	fActive->Delete();
	fDeactive->Delete();
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Activate a de-activated socket
//! \param[in]		Sock		-- socket
/////////////////////////////////////////////////////////////////////////////

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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Activate all de-activated sockets
/////////////////////////////////////////////////////////////////////////////

void TMonitor::ActivateAll()
{
	TIter next(fDeactive);
	TSocketHandler *s;

	while ((s = (TSocketHandler *) next())) {
		fActive->Add(s);
		s->Add();
	}
	fDeactive->Clear();
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Deactivate a socket
//! \param[in]		Sock		-- socket
/////////////////////////////////////////////////////////////////////////////

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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Deactivate all sockets
/////////////////////////////////////////////////////////////////////////////

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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Select socket for which an event is waiting.<br>
//! 				Waits a maximum of \b timeout milliseconds.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	pointer to socket
//! 				<li>	NULL after timeout or error
//! 				</ul>
//! \param[in]		Timeout		-- timeout in millisecs
/////////////////////////////////////////////////////////////////////////////

TSocket * TMonitor::Select(Long_t Timeout)
{

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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Fills lists with sockets for which events are waiting.<br>
//! 				Waits a maximum of \b timeout milliseconds.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	number of sockets waiting
//! 				<li>	lists with sockets waiting for read and/or write
//! 				<li>	0 after timeout or error
//! 				</ul>
//! \param[out]		Rdready		-- list with sockets waiting to be read
//! \param[out]		Wrready		-- list with sockets ready to write
//! \param[in]		Timeout		-- timeout in millisecs
/////////////////////////////////////////////////////////////////////////////

Int_t TMonitor::Select(TList * Rdready, TList * Wrready, Long_t Timeout)
{
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

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Marks socket being ready
//! 				Called by TSocketHandler::Notify() to signal which socket
//! 				is ready to be read or written.
//! 				User should \b not call this routine.
//! 				The ready socket will be returned via the Select() user function.
//! \param[in]		Sock	-- socket
/////////////////////////////////////////////////////////////////////////////

void TMonitor::SetReady(TSocket * Sock) { fReady = Sock; }

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns number of sockets in the active list
//! \retval 		NofSockets	-- number of active sockets
/////////////////////////////////////////////////////////////////////////////

Int_t TMonitor::GetActive() const { return fActive->GetSize(); }

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns number of deactivated sockets
//! \retval 		NofSockets	-- number of sockets
/////////////////////////////////////////////////////////////////////////////

Int_t TMonitor::GetDeactive() const { return fDeactive->GetSize(); }

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details	 	Checks if socket is active
//! \param[in]		Sock	-- socket
//! \return 		TRUE or FALSE
/////////////////////////////////////////////////////////////////////////////

Bool_t TMonitor::IsActive(TSocket * Sock) const
{
	TIter next(fActive);
	while (TSocketHandler *h = (TSocketHandler*) next())
		if (Sock == h->GetSocket()) return kTRUE;

	// Not found
	return kFALSE;
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns a list with all active sockets.<br>
//! 				This list must be deleted by the user.
//! 				DO NOT call Delete() on this list as it will delete
//! 				the sockets that are still being used by the monitor!
//! \retval 		SockList	-- list of active sockets
/////////////////////////////////////////////////////////////////////////////

TList * TMonitor::GetListOfActives() const
{

	TList * list = new TList();

	TIter next(fActive);
	while (TSocketHandler * h = (TSocketHandler*) next()) list->Add(h->GetSocket());
	return list;
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns a list with all deactivated sockets.<br>
//! 				This list must be deleted by the user.
//! 				DO NOT call Delete() on this list as it will delete
//! 				the sockets that are still being used by the monitor!
//! \retval 		SockList	-- list of deactivated sockets
/////////////////////////////////////////////////////////////////////////////

TList * TMonitor::GetListOfDeactives() const
{
	TList * list = new TList();

	TIter next(fDeactive);
	while (TSocketHandler * h = (TSocketHandler*) next()) list->Add(h->GetSocket());
	return list;
}
