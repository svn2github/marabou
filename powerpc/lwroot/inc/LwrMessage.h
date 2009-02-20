#ifndef __LwrMessage_h__
#define __LwrMessage_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrMessage.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMessage<br>
//! 				Base class to provide a message structure
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-20 08:33:52 $
//////////////////////////////////////////////////////////////////////////////

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
	void			WriteBuf(Char_t * Buffer, Int_t Length);
	Int_t			ReadArray(Int_t * Array);
	Int_t			ReadFastArray(Int_t * Array, Int_t NofInts);
	UInt_t			SwapInt32(UInt_t Data);

	//! Return message type
	inline UInt_t   What() { return fWhat; }

	//! Return ptr to message buffer
	inline Char_t * Buffer() { return fBuffer; };

	//! Return ptr to data section
	inline void *	Data() { return fData; };

	//! Return message length
	inline Int_t	Length() { return fBufferSize; };

	inline const Char_t * ClassName() const { return "TMessage"; };

protected:
	UInt_t	fWhat;  			//!< message type
	Int_t	fBytesAllocated;	//!< bytes allocated so far
	Int_t	fBufferSize;		//!< current buffer size (w/o header)
	Char_t	*fBuffer;			//!< message buffer
	M2L_MsgHdr *fHeader;		//!< header section
	void *	fData;				//!< start of data section

};

#endif
