#ifndef __TMrbLogger_h__
#define __TMrbLogger_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLogger.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbLogger    -- message/error logging
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TObject.h"
#include "TNamed.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TDatime.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogMessage
// Purpose:        (Error) Message with date, time, and calling info
// Description:    Message bookkeeping
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLogMessage: public TObject {

	public:
		enum EMrbMsgType	{	kMrbMsgMessage	= BIT(0), 		// normal message
								kMrbMsgError	= BIT(1),		// error message
								kMrbMsgWarning	= BIT(2),		// warning
								kMrbMsgAny		= kMrbMsgMessage | kMrbMsgError | kMrbMsgWarning
							};
					
	public:

		TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
										const Char_t * ClassName, const Char_t * Method, const Char_t * Text);
		
		~TMrbLogMessage() {};
		
		inline UInt_t GetDatime() { return(fDatime.Get()); };
		inline EMrbMsgType GetType() { return(fType); };
		inline const Char_t * GetColor() { return(fColor.Data()); };
		inline const Char_t * GetClassName() { return(fClassName.Data()); };
		inline const Char_t * GetMethod() { return(fMethod.Data()); };
		inline const Char_t * GetText() { return(fText.Data()); };
		const Char_t * Get(TString & FmtMsg, const Char_t * ProgName, Bool_t WithDate = kFALSE, Bool_t WithColors = kTRUE);
				
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLogMessage.html&"); };

	protected:
		TDatime fDatime;
		EMrbMsgType fType;
		TString fColor;
		TString fClassName;
		TString fMethod;
		TString fText;
		
	ClassDef(TMrbLogMessage, 1) 	// [Utils] Single (error) message
};	
		
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger
// Purpose:        MARaBOU's message/error logger
// Description:    Methods to output (error) messages to different output channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLogger: public TNamed {

	public:
		enum EMrbMsgRoute		{	kMrbMsgCout 	= BIT(0), 		// stdout/cout
									kMrbMsgCerr 	= BIT(1), 		// stderr/cerr
									kMrbMsgLog		= BIT(2)		// log file
								};

		enum EMrbMsgFileMode	{	kMrbMsgFileNew,
									kMrbMsgFileAppend
								};

	public:
		TMrbLogger(const Char_t * ProgName = "", const Char_t * LogFile = "marabou.log"); 	// ctor

		~TMrbLogger() { Reset(); };						// dtor

		Bool_t Flush(const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL);
		Bool_t Open(const Char_t * LogFile = "marabou.log", const Char_t * Option = "APPEND");
		Bool_t Close();
				
		void SetProgName(const Char_t * ProgName);
		inline const Char_t * GetProgName() { return(fProgName.Data()); };
		
		inline Int_t GetEntries(TObjArray & MsgArr, Int_t Start = 0) { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgAny)); };
		inline Int_t GetMessages(TObjArray & MsgArr, Int_t Start = 0) { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgMessage)); };
		inline Int_t GetErrors(TObjArray & MsgArr, Int_t Start = 0) { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgError)); };
		inline Int_t GetWarnings(TObjArray & MsgArr, Int_t Start = 0) { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgWarning)); };
		
		inline Int_t GetEntriesSinceLastCall(TObjArray & MsgArr) { return(GetEntries(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetMessagesSinceLastCall(TObjArray & MsgArr) { return(GetMessages(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetErrorsSinceLastCall(TObjArray & MsgArr) { return(GetErrors(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetWarningsSinceLastCall(TObjArray & MsgArr) { return(GetWarnings(MsgArr, fIndexOfLastPrinted)); };
		
		Int_t GetEntriesByType(TObjArray & MsgArr, Int_t Start = 0, UInt_t Type = TMrbLogMessage::kMrbMsgAny);
	
		TMrbLogMessage * GetLast(const Char_t * Option = "*");
		TMrbLogMessage * GetLast(UInt_t Type);
		
		inline UInt_t Enable(UInt_t Bits) { fEnabled |= Bits; return(fEnabled); };
		inline UInt_t Disable(UInt_t Bits) { fEnabled &= ~Bits; return(fEnabled); };
		
		inline void SetGUI(TObject * GUI) { fGUI = GUI; };
		
		void Print(Int_t Tail = 0, const Char_t * Option = "*");
		void Print(Int_t Tail, UInt_t Type);
		
		void PrintSinceLastCall(const Char_t * Option = "Error");
		void PrintSinceLastCall(UInt_t Type);
				
		inline void Reset() {
			fLofMessages.Delete();
			fIndexOfLastPrinted = 0;
		};
		
		inline Int_t GetIndexOfLastPrinted() { return(fIndexOfLastPrinted); };
				
		const Char_t * Prefix(const Char_t * Identifier = "*", const Char_t * ProgName = NULL); 	// line prefix
		
		inline ostrstream & Out() { return(*fOut); };
		inline ostrstream & Err() { return(*fErr); };
		inline ofstream & Log() { return(*fLog); };
				
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLogger.html&"); };

	protected:

		TString fLogFile;			// name of log file on disk
		TString fProgName;			// program issuing messages
		TString fPrefix;			// line prefix
				
		UInt_t fEnabled;			// one bit for each stream
		ofstream * fLog;			//!
		ostrstream * fOut;			//!
		ostrstream * fErr;			//!

		Int_t fIndexOfLastPrinted;	// index at last printout
		TObjArray fLofMessages; 	// list of messages
		
		TObject * fGUI; 			//! connect to GUI (class TGMrbMessageViewer)

	ClassDef(TMrbLogger, 1) 		// [Utils] Message/error logger
};

#endif
