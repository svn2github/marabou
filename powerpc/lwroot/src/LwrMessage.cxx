//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrMessage.cxx
//! \brief			Light Weight ROOT: TMessage
//! \details		Class definitions for ROOT under LynxOs: TMessage<br>
//! 				Message buffer class used to send or receive messages.<br>
//! 				Comprises message types raw, string, and array of ints
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-20 08:40:11 $
//////////////////////////////////////////////////////////////////////////////


#ifdef CPU_TYPE_RIO2
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif


#include "LwrTypes.h"
#include "LwrMessage.h"

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a TMessage object of given type
//! \param[in]		What		-- message type
/////////////////////////////////////////////////////////////////////////////

TMessage::TMessage(UInt_t What)
{
	fWhat = What;
	fBuffer = NULL;
	this->AllocBuffer(0);
	this->SetWhat();
}

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a TMessage object, allocates buffer space
//! \param[in]		Buffer			-- buffer addr
//! \param[in]		BufferSize		-- buffer size in bytes
/////////////////////////////////////////////////////////////////////////////

TMessage::TMessage(void * Buffer, Int_t BufferSize)
{
	fBuffer = NULL;
	this->AllocBuffer(BufferSize);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Allocates buffer space
//! \param[in]		BufferSize		-- buffer size in bytes
//////////////////////////////////////////////////////////////////////////////

void * TMessage::AllocBuffer(Int_t BufferSize)
{
	Int_t bsizAligned = ((BufferSize + sizeof(Int_t) - 1) / sizeof(Int_t)) * sizeof(Int_t);
	Int_t toBeAlloc = bsizAligned + sizeof(M2L_MsgHdr);
	if (fBuffer != NULL && (fBytesAllocated < toBeAlloc)) delete [] fBuffer;
	fBuffer = new Char_t[toBeAlloc];
	fBytesAllocated = toBeAlloc;
	memset(fBuffer, 0, toBeAlloc);
	fHeader = (M2L_MsgHdr *) fBuffer;
	fData = (void *) ((Char_t *) fBuffer + sizeof(M2L_MsgHdr));
	this->SetLength(bsizAligned);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers (raw) message data to (user's) buffer
//! \param[out]		Buffer			-- buffer addr
//! \param[in]		Length			-- number of bytes to be read
//////////////////////////////////////////////////////////////////////////////

Int_t TMessage::ReadBuf(Char_t * Buffer, Int_t Length)
{
	if (Length == 0) Length = this->Length();
	Int_t nofBytes = (Length > this->Length()) ? this->Length() : Length;
	memcpy(Buffer, fData, nofBytes);
	return(nofBytes);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers a string to message buffer
//! \param[in]		String			-- string to be sent
//////////////////////////////////////////////////////////////////////////////

void TMessage::WriteString(const Char_t * String)
{
	if (String != NULL) {
		this->AllocBuffer(sizeof(String));
		memcpy(fData, String, sizeof(String));
		this->SetWhat(kMESS_STRING);
	} 
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers (raw) data to message buffer
//! \param[in]		Buffer			-- buffer addr
//! \param[in]		Length			-- number of bytes to be written
//////////////////////////////////////////////////////////////////////////////

void TMessage::WriteBuf(Char_t * Buffer, Int_t Length)
{
	this->AllocBuffer(Length);
	memcpy(fData, Buffer, Length);
	this->SetWhat(kM2L_MESS_RAWBUF);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers message data to string
//! \param[out]		String			-- where to put message data
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMessage::ReadString(TString & String)
{
	String = (Char_t *) fData;
	this->SetWhat(kMESS_STRING);
	return(String.Data());
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers message data to string
//! \param[out]		String		-- where to put message data
//! \param[in]		Max			-- max string length
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMessage::ReadString(Char_t * String, Int_t Max)
{
	if (Max > 1) {
		strncpy(String, (Char_t *) fData, Max - 1);
		*(String + Max) = '\0';
	}
	this->SetWhat(kMESS_STRING);
	return(String);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers (integer) message data to array
//! \param[out]		Array		-- array address
//////////////////////////////////////////////////////////////////////////////

Int_t TMessage::ReadArray(Int_t * Array)
{
	Int_t nofInts = this->Length() / sizeof(Int_t) - 1;
	Int_t * dp = (Int_t *) (fData + sizeof(Int_t));
	for (Int_t i = 0; i < nofInts; i++) *Array++ = *dp++;
	return(nofInts);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Xfers (integer) message data to array
//! \param[out]		Array		-- array address
//! \param[in]		NofInts		-- number of ints to be xferred
//////////////////////////////////////////////////////////////////////////////

Int_t TMessage::ReadFastArray(Int_t * Array, Int_t NofInts)
{
	memset(Array, 0, NofInts * sizeof(Int_t));
	Int_t nofInts = this->Length() / sizeof(Int_t) - 1;
	if (nofInts > NofInts) nofInts = NofInts;
	Int_t * dp = (Int_t *) (fData + sizeof(Int_t));
	for (Int_t i = 0; i < nofInts; i++) *Array++ = *dp++;
	return(nofInts);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Resets message, clear buffer
//! \param[in]		What		-- message type
//////////////////////////////////////////////////////////////////////////////

void TMessage::Reset(UInt_t What)
{
	memset(fBuffer, 0, fBytesAllocated);
	this->SetLength();
	this->SetWhat(What);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets message buffer length
//! \param[in]		Length		-- buffer length in bytes
//////////////////////////////////////////////////////////////////////////////

void TMessage::SetLength(Int_t Length)
{
	if (Length == -1) Length = fBufferSize; else fBufferSize = Length;
	if (fHeader) fHeader->fLength = Length;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets message type
//! \param[in]		What		-- message type
//////////////////////////////////////////////////////////////////////////////

void TMessage::SetWhat(UInt_t What)
{
	if (What == 0) What = fWhat; else fWhat = What;
	if (fHeader) fHeader->fWhat = What;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Swaps data word
//! \param[in]		Data		-- data word
//////////////////////////////////////////////////////////////////////////////

UInt_t TMessage::SwapInt32(UInt_t Data)
{

	union x {
		int l;
		char b[4];
	} x;
	UInt_t d = Data;
	Char_t * ip = (Char_t *) &d;

	x.b[3] = *ip++;
	x.b[2] = *ip++;
	x.b[1] = *ip++;
	x.b[0] = *ip++;
	return(x.l);
}
