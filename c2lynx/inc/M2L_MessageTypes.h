#ifndef __M2L_MessageTypes_h__
#define __M2L_MessageTypes_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MessageTypes                                                         //
//                                                                      //
//**//////////////////////////////////////////////////////////////////**//
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//////////////////////////////////////////////////////////////////////////

enum EM2L_MessageType {
	kM2L_MESS_ACK				=	1,						// acknowledge a message
	kM2L_MESS_BYE,											// terminate
	kM2L_MESS_VME_REQ			=	0x1000,					// vme request
		kM2L_MESS_VME_CONNECT 	=	kM2L_MESS_VME_REQ | 1,	//		connect to module
		kM2L_MESS_VME_RETURN_HANDLE,						//		return module handle
	kM2L_MESS_CAMAC_REQ 		=	0x2000,					// camac request
	kM2L_MESS_EXEC_PROG 		=	0x4000 					// execute program
};

#endif
