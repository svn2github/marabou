#ifndef __LwrMonitor_h__
#define __LwrMonitor_h__

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

#include "LwrObject.h"
#include "LwrList.h"
#include "LwrSocket.h"


class TMonitor : public TObject {

friend class TSocketHandler;

private:
	TList * fActive;		// list of sockets to monitor
	TList * fDeactive;		// list of (temporary) disabled sockets
	TSocket * fReady;		// socket which is ready to be read or written

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
