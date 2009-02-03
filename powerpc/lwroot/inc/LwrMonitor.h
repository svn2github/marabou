#ifndef __LwrMonitor_h__
#define __LwrMonitor_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrMonitor.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMonitor<br>
//! 				This class monitors activity on a number of network sockets.<br>
//! 				The actual monitoring is done by TSystem::DispatchOneEvent().<br>
//! 				Typical usage:
//! 					<li>	create a TMonitor object.
//! 					<li>	Register a number of TSocket objects and call TMonitor::Select().
//!
//! 				Select() returns the socket object which has data waiting.<br>
//! 				TSocket objects can be added, removed, (temporary) enabled or disabled.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrObject.h"
#include "LwrList.h"
#include "LwrSocket.h"


class TMonitor : public TObject {

friend class TSocketHandler;

private:
	TList * fActive;		//!< list of sockets to monitor
	TList * fDeactive;		//!< list of (temporary) disabled sockets
	TSocket * fReady;		//!< socket which is ready to be read or written

	void SetReady(TSocket * Sock);

public:
	enum EInterest { kRead = 1, kWrite = 2 };

public:
	TMonitor();
	virtual ~TMonitor();

	void Add(TSocket * Sock, Int_t Interest = kRead);
	void Remove(TSocket * Sock);
	void RemoveAll();

	void Activate(TSocket * Sock);
	void ActivateAll();
	void Deactivate(TSocket * Sock);
	void DeactivateAll();
	void Ready(TSocket * Sock);

	void SetInterest(TSocket * Sock, Int_t Interest);

	TSocket * Select(Long_t Timeout = 1000);
	Int_t Select(TList * Rdready, TList * Wrready, Long_t timeout = 1000);

	Int_t  GetActive() const;
	Int_t  GetDeactive() const;
	TList * GetListOfActives() const;
	TList * GetListOfDeactives() const;

	Bool_t IsActive(TSocket * Sock) const;

	inline const Char_t * ClassName() const { return "TMonitor"; };
};

#endif
