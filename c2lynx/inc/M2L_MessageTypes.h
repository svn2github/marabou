#ifndef __M2L_MessageTypes_h__
#define __M2L_MessageTypes_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MessageTypes                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//////////////////////////////////////////////////////////////////////////

enum EM2L_MessageType {
	kM2L_MESS_BYE				=	0x1,					// [c] terminate, [s] died
	kM2L_MESS_ACK				=	0x100,					// acknowledge a message
		kM2L_MESS_ACK_OK,									//		[s] ok, no message text
		kM2L_MESS_ACK_MESSAGE,								//		[s] ok + text
		kM2L_MESS_ACK_WARNING,								//		[s] warning
		kM2L_MESS_ACK_ERROR,								//		[s] error
		kM2L_MESS_ACK_END,									// end of acknowledge flags
	kM2L_MESS_VME_REQ			=	0x1000,					// vme request
		kM2L_MESS_VME_CONNECT,								//		[c] connect to module, [s] return module handle
		kM2L_MESS_VME_EXEC_FUNCTION,						//		[c] exec function, [s] return function results
	kM2L_MESS_EXEC_PROG 		=	0x4000 					// [c] execute program
};

const SMrbNamedXShort kMrbLofMessageTypes[] =
	{
		{kM2L_MESS_BYE, 				"terminate|died"},
		{kM2L_MESS_ACK,					"acknowledge"},
		{kM2L_MESS_ACK_OK,					"ok, no message text"},
		{kM2L_MESS_ACK_MESSAGE,				"ok + text"},
		{kM2L_MESS_ACK_WARNING,				"warning"},
		{kM2L_MESS_ACK_ERROR,				"error"},
		{kM2L_MESS_VME_REQ,				"vme request"},
		{kM2L_MESS_VME_CONNECT,				"connect to module|return module handle"},
		{kM2L_MESS_VME_EXEC_FUNCTION,		"exec function|return function results"},
		{kM2L_MESS_EXEC_PROG,			"execute program"},
		{0, 							NULL}
	};

enum EM2L_FunctionType {
	kM2L_FCT_GET_MODULE_INFO	=	0x1, 					// get module info
	kM2L_FCT_CAEN_875			=	0x100,					// caen adc v875
	kM2L_FCT_SIS_3302			=	0x200,					// sis flash adc 3302
};

const SMrbNamedXShort kMrbLofFunctionTypes[] =
	{
		{kM2L_FCT_GET_MODULE_INFO,		"get module info"},
		{kM2L_FCT_CAEN_875, 			"exec function (CAEN V785)"},
		{kM2L_FCT_SIS_3302, 			"exec function (SIS 3302)"},
		{0, 							NULL}
	};
#endif

