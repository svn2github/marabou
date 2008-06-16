//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMessage                                                             //
//                                                                      //
// Message buffer class used for serializing objects and sending them   //
// over a network. This class inherits from TBuffer the basic I/O       //
// serializer.                                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrTypes.h"
#include "LwrMessage.h"

//______________________________________________________________________________
TMessage::TMessage(UInt_t What)
{
	fWhat = What;
	fBuffer = NULL;
	this->AllocBuffer(0);
	this->SetWhat();
}

//______________________________________________________________________________
TMessage::TMessage(void * Buffer, Int_t BufferSize)
{
	fBuffer = NULL;
	this->AllocBuffer(BufferSize);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
Int_t TMessage::ReadBuf(Char_t * Buffer, Int_t Length)
{
	if (Length == 0) Length = this->Length();
	Int_t nofBytes = (Length > this->Length()) ? this->Length() : Length;
	memcpy(Buffer, fData, nofBytes);
	return(nofBytes);
}

//______________________________________________________________________________
void TMessage::WriteString(const Char_t * String)
{
	if (String != NULL) {
		this->AllocBuffer(sizeof(String));
		memcpy(fData, String, sizeof(String));
		this->SetWhat(kMESS_STRING);
	} 
}

//______________________________________________________________________________
void TMessage::WriteBuf(Char_t * Buffer, Int_t Length)
{
	this->AllocBuffer(Length);
	memcpy(fData, Buffer, Length);
	this->SetWhat(kM2L_MESS_RAWBUF);
}

//______________________________________________________________________________
const Char_t * TMessage::ReadString(TString & String)
{
	String = (Char_t *) fData;
	this->SetWhat(kMESS_STRING);
	return(String.Data());
}

//______________________________________________________________________________
const Char_t * TMessage::ReadString(Char_t * String, Int_t Max)
{
	if (Max > 1) {
		strncpy(String, (Char_t *) fData, Max - 1);
		*(String + Max) = '\0';
	}
	this->SetWhat(kMESS_STRING);
	return(String);
}

//______________________________________________________________________________
Int_t TMessage::ReadArray(Int_t * Array)
{
	Int_t nofInts = this->Length() / sizeof(Int_t) - 1;
	Int_t * dp = (Int_t *) (fData + sizeof(Int_t));
	for (Int_t i = 0; i < nofInts; i++) *Array++ = *dp++;
	return(nofInts);
}

//______________________________________________________________________________
Int_t TMessage::ReadFastArray(Int_t * Array, Int_t NofInts)
{
	memset(Array, 0, NofInts * sizeof(Int_t));
	Int_t nofInts = this->Length() / sizeof(Int_t) - 1;
	if (nofInts > NofInts) nofInts = NofInts;
	Int_t * dp = (Int_t *) (fData + sizeof(Int_t));
	for (Int_t i = 0; i < nofInts; i++) *Array++ = *dp++;
	return(nofInts);
}

//______________________________________________________________________________
void TMessage::Reset(UInt_t What)
{
	memset(fBuffer, 0, fBytesAllocated);
	this->SetLength();
	this->SetWhat(What);
}

//______________________________________________________________________________
void TMessage::SetLength(Int_t Length)
{
	if (Length == -1) Length = fBufferSize; else fBufferSize = Length;
	if (fHeader) fHeader->fLength = Length;
}

//______________________________________________________________________________
void TMessage::SetWhat(UInt_t What)
{
	if (What == 0) What = fWhat; else fWhat = What;
	if (fHeader) fHeader->fWhat = What;
}

//______________________________________________________________________________
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
