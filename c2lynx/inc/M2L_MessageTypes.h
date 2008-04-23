#ifndef __M2L_MessageTypes_h__
#define __M2L_MessageTypes_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MessageTypes                                                         //
//                                                                      //
// System predefined message types. Message types are constants that    //
// indicate what kind of message it is. Make sure your own message      //
// types don't clash whith the ones defined in this file. ROOT reserves //
// all message ids between 0 - 10000. Make sure your message            //
// id < 200000000.                                                      //
//                                                                      //
//**//////////////////////////////////////////////////////////////////**//
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//////////////////////////////////////////////////////////////////////////

enum EMessageTypes_M2L {
#ifdef R__LYNXOS
//---- message types predefined by ROOT (see $ROOTSYS/include/MessageTypes.h)
//     have to be defined in case of LynxOs
	kMESS_ACK			= 0x10000000, 			//OR with kMESS_ACK to force each message to be acknowledged
	kMESS_ANY			= 0,					//generic message type
	kMESS_OK,									//everything OK
	kMESS_NOTOK,								//things are NOT OK
	kMESS_STRING,								//string follows
	kMESS_OBJECT,								//object follows
#endif

//---- MARABOU to Lynx extensions
	kM2L_MESS_AWD			= 0x08000000,		// OR with kM2L_MESS_AWD means "acknowledge when done"

	kM2L_MESS_USER			= 0x10000,			// start of user defined message types
	kM2L_MESS_BYE,								// terminate
	kM2L_MESS_RAWBUF,							// raw data transmission
	kM2L_MESS_ARRAY,							// array of integers
	kM2L_MESS_EXEC_PROG, 						// execute program
	kM2L_MESS_SETUP_CAMAC,						// start esone part of server (connect to vc32/cc32)
	kM2L_MESS_SETUP_VME,						// start communication with VME modules
	kM2L_MESS_EXEC_CAMAC,						// execute camac command
	kM2L_MESS_EXEC_VME,						// execute vme command
};

#endif
