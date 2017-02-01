//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvSocket.cxx
//! \brief			MARaBOU to Lynx: Socket communication
//! \details		Implements class methods for socket communication
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2011-05-20 12:21:03 $
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "LwrMonitor.h"
#include "SrvSocket.h"
#include "SrvVMEModule.h"
#include "SrvSis3302.h"
#include "SrvSis3302_Layout.h"
#include "SrvVulomTB.h"
#include "SrvVulom_Layout.h"
#include "SrvCaen785.h"
#include "SrvUtils.h"
#include "SetColor.h"

#include "vmelib.h"

extern Bool_t gSignalTrap;

extern TMrbLofNamedX * gLofVMEModules;			// list of actual modules

SrvSocket::SrvSocket(const Char_t * Name, const Char_t * Path, Int_t Port, Bool_t NonBlocking, Bool_t VerboseMode) : TNamed(Name, Path) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			SrvSocket	SrvSocket.h
//! \brief			Create a server socket
//! \details		Class to define a server socket
//////////////////////////////////////////////////////////////////////////////

	fSocket = new TServerSocket(Port, 1);
	if(!fSocket->IsValid()){
		this->MakeZombie();
		cerr << setred << this->ClassName() << ": Can't create server socket: error code " << fSocket->GetErrorCode() << setblack << endl;
	} else {
		fPort = Port;
		fNonBlocking = NonBlocking;
		fVerboseMode = VerboseMode;
		fMsgTypes.AddNamedX(kMrbLofMessageTypes);
		fFctTypes.AddNamedX(kMrbLofFunctionTypes);
		fSocket->SetOption(kNoBlock, fNonBlocking ? 1 : 0);
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a socket and listens for messages
//////////////////////////////////////////////////////////////////////////////

void SrvSocket::Listen() {
	static TSocket * primaryClient = NULL;

	TMonitor * mon = new TMonitor();
	mon->Add(this->GetSocket());

	TSocket * sock;
	while (1) {
		sock = mon->Select();
		if (sock == NULL) continue;
		if (sock->IsServerSocket()) {
			TSocket * s = ((TServerSocket *) sock)->Accept();
			mon->Add(s);
			TString p;
			if (primaryClient == NULL) {
				p = "primary";
				primaryClient = s;
			} else {
				p = "secondary";
			}
			cout << this->ClassName() << "::Listen(): [Monitor] New (" << p.Data() << ") client added to selection list" << endl;
			continue;
		}

		Int_t errCnt = 0;
		Char_t buffer[1024];
		M2L_MsgHdr * hdr = (M2L_MsgHdr *) buffer;
		sock->RecvRaw(buffer, sizeof(M2L_MsgHdr));
		swapHdr(hdr);
		Int_t nofBytes = sock->RecvRaw(buffer + sizeof(M2L_MsgHdr), hdr->fLength - sizeof(M2L_MsgHdr));
		TMrbNamedX * mtype = this->CheckMessageType(hdr, "Listen", kTRUE, kFALSE);
		if (mtype) {
			if (hdr->fWhat == kM2L_MESS_VME_CONNECT) {
				errCnt = 0;
				M2L_VME_Connect * connect = (M2L_VME_Connect *) buffer;
				swapData((Int_t *) &connect->fBaseAddr);
				swapData(&connect->fSegSize);
				swapData(&connect->fNofChannels);
				swapData((Int_t *) &connect->fMapping);
				SrvVMEModule * vmeModule = new SrvVMEModule(connect->fModuleName, connect->fModuleType,
						connect->fBaseAddr, connect->fSegSize, connect->fNofChannels, connect->fMapping);
				if (vmeModule->IsZombie()) {
					this->Error(sock);
				} else {
					switch (vmeModule->GetID()) {
						case SrvVMEModule::kModuleCaen785:
							{
								SrvCaen785 * proto = (SrvCaen785 *) vmeModule->GetPrototype();
								if (proto->TryAccess(vmeModule)) break;
								this->Error(sock);
								continue;
							}
						case SrvVMEModule::kModuleSis3302:
							{
								SrvSis3302 * proto = (SrvSis3302 *) vmeModule->GetPrototype();
								if (proto->TryAccess(vmeModule)) break;
								this->Error(sock);
								continue;
							}
						case SrvVMEModule::kModuleVulomTB:
							{
								SrvVulomTB * proto = (SrvVulomTB *) vmeModule->GetPrototype();
								if (proto->TryAccess(vmeModule)) break;
								this->Error(sock);
								continue;
							}
					}
					vmeModule->Print();
					vmeModule->Append();
					M2L_VME_Return_Handle * handle = (M2L_VME_Return_Handle *) buffer;
					initMessage((M2L_MsgHdr *) handle, sizeof(M2L_VME_Return_Handle), kM2L_MESS_VME_CONNECT);
					handle->fHandle = swapIt((UInt_t) vmeModule);
					this->CheckMessageType((M2L_MsgHdr *) handle, "Listen", kFALSE, kTRUE);
					sock->SendRaw(handle, sizeof(M2L_VME_Return_Handle));
				}
			} else if (hdr->fWhat == kM2L_MESS_VME_EXEC_FUNCTION) {
				errCnt = 0;
				M2L_VME_Exec_Function * exec = (M2L_VME_Exec_Function *) buffer;
				swapData((Int_t *) &exec->fXhdr.fHandle);
				swapData((Int_t *) &exec->fXhdr.fCode);
				TMrbNamedX * ftype = fFctTypes.FindByIndex(exec->fXhdr.fCode);
				if (ftype) {
					SrvVMEModule * vmeModule = (SrvVMEModule *) exec->fXhdr.fHandle;
					M2L_MsgHdr * res;
					switch (vmeModule->GetID()) {
						case SrvVMEModule::kModuleCaen785:
							{
								SrvCaen785 * proto = (SrvCaen785 *) vmeModule->GetPrototype();
								res = proto->Dispatch(vmeModule, ftype, &exec->fData);
								if (res) break;
								this->Error(sock);
								continue;
							}
						case SrvVMEModule::kModuleSis3302:
							{
								SrvSis3302 * proto = (SrvSis3302 *) vmeModule->GetPrototype();
								res = proto->Dispatch(vmeModule, ftype, &exec->fData);
								if (res) break;
								this->Error(sock);
								continue;
							}
						case SrvVMEModule::kModuleVulomTB:
							{
								SrvVulomTB * proto = (SrvVulomTB *) vmeModule->GetPrototype();
								res = proto->Dispatch(vmeModule, ftype, &exec->fData);
								if (res) break;
								this->Error(sock);
								continue;
							}
					}
					this->CheckMessageType(res, "Listen", kFALSE, kTRUE);
					sock->SendRaw(res, swapIt(res->fLength));
				} else {
					cerr << setred << this->ClassName() << "::Listen(): Unknown function code - " << setbase(16) << exec->fXhdr.fCode << setbase(10) << setblack << endl;
					this->Error(sock);
				}
			} else if (hdr->fWhat == kM2L_MESS_BYE) {
				TIterator * iter = gLofVMEModules->MakeIterator();
				TMrbNamedX * module;
				while (module = (TMrbNamedX *) iter->Next()) {
					SrvVMEModule * vmeModule = (SrvVMEModule *) module->GetAssignedObject();
					switch (vmeModule->GetID()) {
						case SrvVMEModule::kModuleCaen785:
						{
							SrvCaen785 * proto = (SrvCaen785 *) vmeModule->GetPrototype();
							continue;
						}
						case SrvVMEModule::kModuleSis3302:
						{
							SrvSis3302 * proto = (SrvSis3302 *) vmeModule->GetPrototype();
							proto->Release(vmeModule);
							continue;
						}
						case SrvVMEModule::kModuleVulomTB:
						{
							SrvVulomTB * proto = (SrvVulomTB *) vmeModule->GetPrototype();
							continue;
						}
					}
				}
				delete iter;
				if (sock == primaryClient) {
					errCnt = 0;
					cout << this->ClassName() << "::Listen(): Primary client has disconnected - shutting down server" << endl;
					this->Message(sock);
					return;
				} else {
					cout << this->ClassName() << "::Listen(): Secondary client has disconnected" << endl;
					this->Message(sock);
					mon->Remove(sock);
					continue;
				}
			}
		} else if (hdr->fWhat == 0 && nofBytes == 0 && errCnt > 2) {
			cerr << this->ClassName() << "::Listen(): Giving up ..." << setblack << endl;
			this->Error(sock);
			return;
		} else {
			errCnt++;
			cerr << this->ClassName() << "::Listen(): Unknown message code - " << setbase(16) << hdr->fWhat
							<< ", message length = " << nofBytes << " byte(s)" << setbase(10) << setblack << endl;
			this->Error(sock);
		}
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Informs message sender about successful processing
//! \param[in]		Sock		-- socket
//! \param[in]		Type		-- type of acknowedgement
//! \param[in]		Text		-- message text
//////////////////////////////////////////////////////////////////////////////

void SrvSocket::Acknowledge(TSocket * Sock, EM2L_MessageType Type, const Char_t * Text) {

	M2L_Acknowledge ack;
	initMessage((M2L_MsgHdr *) &ack, sizeof(M2L_Acknowledge), Type);
	if (Text != NULL) strncpy(ack.fText, Text, kM2L_L_TEXT - 1);
	this->CheckMessageType((M2L_MsgHdr *) &ack, "Acknowledge", kFALSE, kTRUE);
	Sock->SendRaw(&ack, sizeof(M2L_Acknowledge));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Checks message type and writes to log if verbose mode
//! \param[in]		Hdr 		-- message header
//! \param[in]		Method		-- calling method
//! \param[in]		RecvFlag	-- kTRUE if 'receive'
//! \param[in]		SwapFlag	-- swap header if kTRUE
//////////////////////////////////////////////////////////////////////////////

TMrbNamedX * SrvSocket::CheckMessageType(M2L_MsgHdr * Hdr, const Char_t * Method, Bool_t RecvFlag, Bool_t SwapFlag) {

	UInt_t what = SwapFlag ? swapIt(Hdr->fWhat) : Hdr->fWhat;
	Int_t length = SwapFlag ? swapIt(Hdr->fLength) : Hdr->fLength;

	TMrbNamedX * mtype = fMsgTypes.FindByIndex(what);
	if (!mtype) return(NULL);
	
	TString rs;
	if (this->IsVerbose()) {
		TString t = mtype->GetName();
		Int_t x = t.Index("|", 0);
		if (RecvFlag) {
			rs = "Recv";
			if (x > 0) t.Resize(x);
		} else {
			rs = "Send";
			if (x > 0) t = t(x + 1, 1000);
		}
		cout	<< this->ClassName() << "::" << Method << "(): "<< "[" << rs.Data() << "] "
						<< t.Data()
						<< " (what=0x" << setbase(16) << what << ", "
						<< setbase(10) << length << " byte(s))" << endl;
	}
	return(mtype);
}
