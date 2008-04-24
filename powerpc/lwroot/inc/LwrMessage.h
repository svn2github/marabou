#ifndef __LwrMessage_h__
#define __LwrMessage_h__

#include "LwrObject.h"
#include "LwrString.h"
#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

class TMessage : public TObject {

friend class TSocket;

protected:
	TMessage(void * Buffer, Int_t BufferSize);   // only called by TSocket::Recv()
	void * AllocBuffer(Int_t BufferSize);
	void SetLength(Int_t Length = -1);

public:
	TMessage(UInt_t What = kMESS_ANY);
	virtual ~TMessage() {};

	void			Reset(UInt_t What = 0);
	void			SetWhat(UInt_t What = 0);
	void			WriteString(const Char_t * String);
	const			Char_t * ReadString(TString & String);
	const			Char_t * ReadString(Char_t * String, Int_t Max);
	Int_t			ReadBuf(Char_t * Buffer, Int_t Max = 0);
	Int_t			ReadArray(Int_t * Array);
	Int_t			ReadFastArray(Int_t * Array, Int_t NofInts);

	inline UInt_t   What() { return fWhat; }
	inline Char_t * Buffer() { return fBuffer; };
	inline void *	Data() { return fData; };
	inline Int_t	Length() { return fBufferSize; };

	inline const Char_t * ClassName() const { return "TMessage"; };

protected:
	UInt_t	fWhat;  			// message type
	Int_t	fBytesAllocated;	// bytes allocated so far
	Int_t	fBufferSize;		// current buffer size (w/o header)
	Char_t	*fBuffer;			// message buffer
	M2L_MsgHdr *fHeader;		// header section
	void *	fData;				// start of data section

};

#endif
