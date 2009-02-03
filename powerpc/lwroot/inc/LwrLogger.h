#ifndef __LwrLogger_h__
#define __LwrLogger_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLogger.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMrbLogger
//! 				A (error) message logger class.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"
#include "fstream.h"
#include "strstream.h"

#include "LwrObject.h"
#include "LwrNamed.h"
#include "LwrLynxOsSystem.h"
#include "LwrDatime.h"
#include "LwrList.h"

extern TLynxOsSystem * gSystem;

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

		TMrbLogMessage() {};
		TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
										const Char_t * ClassName, const Char_t * Method, const Char_t * Text, Bool_t Indent = kFALSE);
		
		~TMrbLogMessage() {};
		
		inline UInt_t GetDatime() const { return(fDatime.Get()); };
		inline EMrbMsgType GetType() const { return(fType); };
		inline const Char_t * GetColor() const { return(fColor.Data()); };
		inline const Char_t * GetClassName() const { return(fClassName.Data()); };
		inline const Char_t * GetMethod() const { return(fMethod.Data()); };
		inline const Char_t * GetText() const { return(fText.Data()); };
		const Char_t * Get(TString & FmtMsg,
									const Char_t * ProgName = NULL,
									Bool_t WithDate = kFALSE,
									Bool_t WithColors = kTRUE) const;
				
	protected:
		Bool_t fIndent;
		TDatime fDatime;
		EMrbMsgType fType;
		TString fColor;
		TString fClassName;
		TString fMethod;
		TString fText;
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
		TMrbLogger(const Char_t * ProgName = "", const Char_t * LogFile = "c2lynx.log"); 	// ctor

		~TMrbLogger() { Reset(); };						// dtor

		Bool_t Flush(const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL, Bool_t Indent = kFALSE);
		Bool_t Open(const Char_t * LogFile = "marabou.log", const Char_t * Option = "APPEND");
		Bool_t Close();
				
		void SetProgName(const Char_t * ProgName);
		inline const Char_t * GetProgName() const { return(fProgName.Data()); };
		
		Int_t GetNofEntries(UInt_t Type = TMrbLogMessage::kMrbMsgAny) const; 	// number of entries
		inline Int_t GetEntries(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgAny)); };
		inline Int_t GetMessages(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgMessage)); };
		inline Int_t GetErrors(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgError)); };
		inline Int_t GetWarnings(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgWarning)); };
		
		inline Int_t GetEntriesSinceLastCall(TList & MsgArr) const { return(GetEntries(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetMessagesSinceLastCall(TList & MsgArr) const { return(GetMessages(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetErrorsSinceLastCall(TList & MsgArr) const { return(GetErrors(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetWarningsSinceLastCall(TList & MsgArr) const { return(GetWarnings(MsgArr, fIndexOfLastPrinted)); };
		
		Int_t GetEntriesByType(TList & MsgArr, Int_t Start = 0, UInt_t Type = TMrbLogMessage::kMrbMsgAny) const;
	
		TMrbLogMessage * GetLast(const Char_t * Option = "*") const;
		TMrbLogMessage * GetLast(UInt_t Type) const;
		
		inline UInt_t Enable(UInt_t Bits) { fEnabled |= Bits; return(fEnabled); };
		inline UInt_t Disable(UInt_t Bits) { fEnabled &= ~Bits; return(fEnabled); };
		
		inline void SetGUI(TObject * GUI) { fGUI = GUI; };
		
		void Print(Int_t Tail = 0, const Char_t * Option = "*") const;
		void Print(Int_t Tail, UInt_t Type) const;
		
		void PrintSinceLastCall(const Char_t * Option = "Error");
		void PrintSinceLastCall(UInt_t Type);
				
		inline void Reset() {
			fLofMessages.Delete();
			fIndexOfLastPrinted = 0;
		};
		
		inline Int_t GetIndexOfLastPrinted() const { return(fIndexOfLastPrinted); };
				
		const Char_t * Prefix(const Char_t * Identifier = "*", const Char_t * ProgName = NULL); 	// line prefix
		
		inline Bool_t Out(const Char_t * Msg, const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgMessage, Msg, ClassName, Method, Color));
		};
		inline Bool_t Err(const Char_t * Msg, const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgError, Msg, ClassName, Method, Color));
		};
		inline Bool_t Wrn(const Char_t * Msg, const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgWarning, Msg, ClassName, Method, Color));
		};
	
		inline ostrstream & Out() { return(*fOut); };
		inline ostrstream & Err() { return(*fErr); };
		inline ostrstream & Wrn() { return(*fWrn); };
		inline ofstream & Log() { return(*fLog); };

		inline const Char_t * ClassName() const { return "TMrbLogger"; };

	protected:
		Bool_t OutputMessage(TMrbLogMessage::EMrbMsgType MsgType, const Char_t * Msg,
									const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL, Bool_t Indent = kFALSE);

	protected:

		TString fLogFile;			// name of log file on disk
		TString fProgName;			// program issuing messages
		TString fPrefix;			// line prefix
				
		UInt_t fEnabled;			// one bit for each stream
		ofstream * fLog;			//!
		ostrstream * fOut;			//!
		ostrstream * fErr;			//!
		ostrstream * fWrn;			//!

		Int_t fIndexOfLastPrinted;	// index at last printout
		TList fLofMessages; 		// list of messages
		
		TObject * fGUI; 			//! connect to GUI (class TGMrbMessageViewer)
};

#endif
