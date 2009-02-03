#ifndef __LwrLogger_h__
#define __LwrLogger_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLogger.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TMrbLogger<br>
//! 				A (error) message logger class.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-03 13:30:30 $
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
// \class			TMrbLogMessage
// \details			(Error) Message with date, time, and calling info
//////////////////////////////////////////////////////////////////////////////


class TMrbLogMessage: public TObject {

	public:
		enum EMrbMsgType	{	kMrbMsgMessage	= BIT(0), 		// normal message
								kMrbMsgError	= BIT(1),		// error message
								kMrbMsgWarning	= BIT(2),		// warning
								kMrbMsgAny		= kMrbMsgMessage | kMrbMsgError | kMrbMsgWarning
							};
					
	public:

		//! Default constructor
		TMrbLogMessage() {};

		//! Constructor
		//! \param[in]	Type		-- message type	(normal, warning, error)
		//! \param[in]	Color		-- string to colorify things
		//! \param[in]	ClassName	-- calling class
		//! \param[in]	Method		-- calling method
		//! \param[in]	Text		-- message text
		//! \param[in]	Indent		-- kTRUE if text has to be indented
		TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
								const Char_t * ClassName, const Char_t * Method,
								const Char_t * Text, Bool_t Indent = kFALSE);
		
		//! Destructor
		~TMrbLogMessage() {};
		
		//! Get date and time of message
		inline UInt_t GetDatime() const { return(fDatime.Get()); };

		//! Get message type
		inline EMrbMsgType GetType() const { return(fType); };

		//! Get message color
		inline const Char_t * GetColor() const { return(fColor.Data()); };

		//! Get name of calling class
		inline const Char_t * GetClassName() const { return(fClassName.Data()); };

		//! Get name of calling method
		inline const Char_t * GetMethod() const { return(fMethod.Data()); };

		//! Get message text
		inline const Char_t * GetText() const { return(fText.Data()); };

		const Char_t * Get(TString & FmtMsg,
									const Char_t * ProgName = NULL,
									Bool_t WithDate = kFALSE,
									Bool_t WithColors = kTRUE) const;
				
	protected:
		Bool_t fIndent; 		//!< kTRUE if text is to be indented
		TDatime fDatime;		//!< time stamp
		EMrbMsgType fType;		//!< message type
		TString fColor; 		//!< prefix to colorify text
		TString fClassName; 	//!< name of calling class
		TString fMethod;		//!< method name
		TString fText;			//!< message text
};	
		
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger
// Purpose:        MARaBOU's message/error logger
// Description:    Methods to output (error) messages to different output channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// \class			TMrbLogger
// \details			Message logger
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
		//! Constructor
		//! \param[in]	ProgName		-- name of calling program
		//! \param[in]	LogFile 		-- name of log file
		TMrbLogger(const Char_t * ProgName = "", const Char_t * LogFile = "c2lynx.log"); 	// ctor

		//! Destructor
		~TMrbLogger() { Reset(); };						// dtor

		Bool_t Flush(const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL, Bool_t Indent = kFALSE);
		Bool_t Open(const Char_t * LogFile = "marabou.log", const Char_t * Option = "APPEND");
		Bool_t Close();
				
		void SetProgName(const Char_t * ProgName);

		//! Get name of calling program
		inline const Char_t * GetProgName() const { return(fProgName.Data()); };
		
		Int_t GetNofEntries(UInt_t Type = TMrbLogMessage::kMrbMsgAny) const;

		//! Get list of messages (of any type)
		//! \param[out] MsgArr	-- where to store messages
		//! \param[in]	Start	-- message number to start with
		inline Int_t GetEntries(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgAny)); };

		//! Get list of (normal) messages
		//! \param[out] MsgArr	-- where to store messages
		//! \param[in]	Start	-- message number to start with
		inline Int_t GetMessages(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgMessage)); };

		//! Get list of error messages
		//! \param[out] MsgArr	-- where to store messages
		//! \param[in]	Start	-- message number to start with
		inline Int_t GetErrors(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgError)); };

		//! Get list of warning messages
		//! \param[out] MsgArr	-- where to store messages
		//! \param[in]	Start	-- message number to start with
		inline Int_t GetWarnings(TList & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgWarning)); };
		
		//! Get list of messages (of any type) since last call
		//! \param[out] MsgArr	-- where to store messages
		inline Int_t GetEntriesSinceLastCall(TList & MsgArr) const { return(GetEntries(MsgArr, fIndexOfLastPrinted)); };

		//! Get list of (normal) messages since last call
		//! \param[out] MsgArr	-- where to store messages
		inline Int_t GetMessagesSinceLastCall(TList & MsgArr) const { return(GetMessages(MsgArr, fIndexOfLastPrinted)); };

		//! Get list of error messages since last call
		//! \param[out] MsgArr	-- where to store messages
		inline Int_t GetErrorsSinceLastCall(TList & MsgArr) const { return(GetErrors(MsgArr, fIndexOfLastPrinted)); };

		//! Get list of warning messages since last call
		//! \param[out] MsgArr	-- where to store messages
		inline Int_t GetWarningsSinceLastCall(TList & MsgArr) const { return(GetWarnings(MsgArr, fIndexOfLastPrinted)); };
		
		Int_t GetEntriesByType(TList & MsgArr, Int_t Start = 0, UInt_t Type = TMrbLogMessage::kMrbMsgAny) const;
	
		TMrbLogMessage * GetLast(const Char_t * Option = "*") const;
		TMrbLogMessage * GetLast(UInt_t Type) const;
		
		//! Enable output of messages of given type
		//! \param[in]	Bits	-- type bits
		inline UInt_t Enable(UInt_t Bits) { fEnabled |= Bits; return(fEnabled); };
		
		//! Disable output of messages of given type
		//! \param[in]	Bits	-- type bits
		inline UInt_t Disable(UInt_t Bits) { fEnabled &= ~Bits; return(fEnabled); };
		
		void Print(Int_t Tail = 0, const Char_t * Option = "*") const;
		void Print(Int_t Tail, UInt_t Type) const;
		
		void PrintSinceLastCall(const Char_t * Option = "Error");
		void PrintSinceLastCall(UInt_t Type);
				
		//! Reset message lists
		inline void Reset() {
			fLofMessages.Delete();
			fIndexOfLastPrinted = 0;
		};
		
		//! Return index of last message printed
		inline Int_t GetIndexOfLastPrinted() const { return(fIndexOfLastPrinted); };
				
		const Char_t * Prefix(const Char_t * Identifier = "*", const Char_t * ProgName = NULL); 	// line prefix
		
		//! Output a (normal) message
		//! \param[in]	Msg 		-- message text
		//! \param[in]	ClassName	-- name of calling class
		//! \param[in]	Method		-- calling method
		//! \param[in]	Color		-- prefix to apply color
		inline Bool_t Out(const Char_t * Msg, const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgMessage, Msg, ClassName, Method, Color));
		};
		
		//! Output an error message
		//! \param[in]	Msg 		-- message text
		//! \param[in]	ClassName	-- name of calling class
		//! \param[in]	Method		-- calling method
		//! \param[in]	Color		-- prefix to apply color
		inline Bool_t Err(const Char_t * Msg, const Char_t * ClassName = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgError, Msg, ClassName, Method, Color));
		};
		
		//! Output a warning message
		//! \param[in]	Msg 		-- message text
		//! \param[in]	ClassName	-- name of calling class
		//! \param[in]	Method		-- calling method
		//! \param[in]	Color		-- prefix to apply color
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

		TString fLogFile;			//!< name of log file on disk
		TString fProgName;			//!< program issuing messages
		TString fPrefix;			//!< line prefix
				
		UInt_t fEnabled;			//!< one bit for each stream
		ofstream * fLog;
		ostrstream * fOut;
		ostrstream * fErr;
		ostrstream * fWrn;

		Int_t fIndexOfLastPrinted;	//!< index at last printout
		TList fLofMessages; 		//!< list of messages
};

#endif
