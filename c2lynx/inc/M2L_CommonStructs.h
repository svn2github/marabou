#ifndef __M2L_CommonStructs_h__
#define __M2L_CommonStructs_h__

///_____________________________________________[C/C++ STRUCT DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           CommonStructs.h
// Purpose:        MARaBOU to Lynx: communication
// Description:    Structure definitions for M2L server
// Author:         R. Lutter
// Revision:       $Id: M2L_CommonStructs.h,v 1.5 2011-07-26 08:41:50 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

enum {	kM2L_L_NAME 		=	32	};
enum {	kM2L_L_TYPE 		=	32	};
enum {	kM2L_L_TEXT 		=	512	};

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_MsgHdr
// Purpose:        Message header
// Members:        Int_t fLength        -- total message length (header + data)
//                 UInt_t fWhat         -- message type
// Used by:        Client and server (part of any message)
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	Int_t fLength;
	UInt_t fWhat;
} M2L_MsgHdr;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_MsgXhdr
// Purpose:        Message header (exec function)
// Members:        M2L_MsgHdr fHdr      -- header
//                 UInt_t fHandle       -- module handle
//                 UInt_t fCode         -- function code
// Used by:        Client and server (part of any message)
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	UInt_t fHandle;
	UInt_t fCode;
} M2L_MsgXhdr;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_MsgData
// Purpose:        Message data
// Members:        Int_t fWc            -- word count
//                 Int_t fData[]        -- (integer) data
// Used by:        Client and server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	Int_t fWc;
	Int_t fData[1];
} M2L_MsgData;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Connect
// Purpose:        Connect to VME module
// Members:        M2L_MsgHdr fHdr      -- header
//                 Char_t fModuleName[] -- module name
//                 Char_t fModuleType[] -- module type
//                 UInt_t fBaseAddr     -- phys address
//                 Int_t fNofChannels   -- number of channels to be used
// Used by:        Client
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	Char_t fModuleName[kM2L_L_NAME];
	Char_t fModuleType[kM2L_L_TYPE];
	UInt_t fBaseAddr;
	Int_t fNofChannels;
} M2L_VME_Connect;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Return_Handle
// Purpose:        Return module identifier
// Members:        M2L_MsgHdr fHdr      -- header
//                 UInt_t fHandle       -- handle to be used for further access
// Used by:        Server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	UInt_t fHandle;
} M2L_VME_Return_Handle;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_Acknowledge
// Purpose:        Return acknowledge message
// Members:        M2L_MsgHdr fHdr      -- header
//                 Char_t fText[]       -- opt. message text
// Used by:        Server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	Char_t fText[kM2L_L_TEXT];
} M2L_Acknowledge;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Exec_Function
// Purpose:        Execute function
// Members:        M2L_MsgXhdr fXhdr    -- header (exec function)
//                 M2L_MsgData fData    -- (integer) data
// Used by:        Client
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgXhdr fXhdr;
	M2L_MsgData fData;
} M2L_VME_Exec_Function;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Return_Results
// Purpose:        Return acknowledge message
// Members:        M2L_MsgHdr fHdr      -- header
//                 M2L_MsgData fData    -- (integer) data
// Used by:        Server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	M2L_MsgData fData;
} M2L_VME_Return_Results;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Return_Module_Info
// Purpose:        Return module info data
// Members:        M2L_MsgHdr fhdr      -- header
//                 Int_t fBoardId       -- board id
//                 Int_t fSerial        -- serial number
//                 Int_t fMajorVersion  -- major version
//                 Int_t fMinorVersion  -- minor version
// Used by:        Server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	Int_t fBoardId;
	Int_t fSerial;
	Int_t fMajorVersion;
	Int_t fMinorVersion;
} M2L_VME_Return_Module_Info;

//___________________________________________________[C/C++ STRUCT DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           M2L_VME_Return_Module_Addr
// Purpose:        Return module address
// Members:        M2L_MsgHdr fhdr      -- header
//                 UInt_t fAddress      -- vme address
//                 Int_t fAddrSpace     -- address space (full or reduced)
// Used by:        Server
//////////////////////////////////////////////////////////////////////////////

typedef struct {
	M2L_MsgHdr fHdr;
	UInt_t fAddress;
	Int_t fAddrSpace;
} M2L_VME_Return_Module_Addr;

#endif
