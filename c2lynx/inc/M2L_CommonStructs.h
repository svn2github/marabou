#ifndef __M2L_CommonStructs_h__
#define __M2L_CommonStructs_h__

enum {	kM2L_L_NAME 		=	32	};
enum {	kM2L_L_TYPE 		=	32	};
enum {	kM2L_L_TEXT 		=	512	};

typedef struct {
	Int_t fLength;
	UInt_t fWhat;
} M2L_MsgHdr;

typedef struct {
	M2L_MsgHdr fHdr;
	Char_t fModuleName[kM2L_L_NAME];
	Char_t fModuleType[kM2L_L_TYPE];
	UInt_t fBaseAddr;
	Int_t fNofChannels;
} M2L_VME_Connect;

typedef struct {
	M2L_MsgHdr fHdr;
	UInt_t fHandle;
} M2L_VME_Return_Handle;

typedef struct {
	M2L_MsgHdr fHdr;
	UInt_t fModuleHandle;
	UInt_t fOffset;
} M2L_VME_Exec_Funct;

typedef struct {
	M2L_MsgHdr fHdr;
	Char_t fText[kM2L_L_TEXT];
} M2L_Acknowledge;

#endif
