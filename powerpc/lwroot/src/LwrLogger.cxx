//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLogger.cxx
//! \brief			Light Weight ROOT: TMrbLogger
//! \details		Class definitions for ROOT under LynxOs: TMrbLogger
//!                 A message and error logger
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $     
//! $Date: 2009-05-26 13:07:42 $
//////////////////////////////////////////////////////////////////////////////


namespace std {} using namespace std;

#include "iostream.h"
#include "iomanip.h"

#include "LwrNamedX.h"
#include "LwrLofNamedX.h"
#include "LwrLogger.h"
#include "SetColor.h"

const SMrbNamedXShort kMrbMsgTypes[] =
							{
								{	TMrbLogMessage::kMrbMsgMessage, 			"Message" 		},
								{	TMrbLogMessage::kMrbMsgError,	 			"Error" 		},
								{	TMrbLogMessage::kMrbMsgWarning, 			"Warning" 		},
								{	TMrbLogMessage::kMrbMsgAny, 	 			"Any" 	 		},
								{	TMrbLogMessage::kMrbMsgAny, 	 			"*" 	 		},
								{	0, 											NULL			}
							};

const SMrbNamedXShort kMrbMsgOpenModes[] =
							{
								{	TMrbLogger::kMrbMsgFileNew,					"New"	 		},
								{	TMrbLogger::kMrbMsgFileAppend,				"Append"	 	},
								{	0, 											NULL			}
							};

TMrbLogger * gMrbLog = NULL;			// global access to logging system

//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a message object to be stored in TMrbLogger's data base
//! \param[in]		Type		-- message type
//! \param[in]		Color		-- color to be used for output
//! \param[in]		ClassName	-- calling class
//! \param[in]		Method		-- calling method
//! \param[in]		Text		-- message text
//! \param[in]		Indent		-- if TRUE: don't output class/method but indent text
/////////////////////////////////////////////////////////////////////////////

TMrbLogMessage::TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
									const Char_t * ClassName, const Char_t * Method, const Char_t * Text, Bool_t Indent)
{
	fType = Type;
	if (Color == NULL || *Color == '\0') {
		switch (fType) {
			case kMrbMsgMessage:	fColor = setblack; break;
			case kMrbMsgError:		fColor = setred; break;
			case kMrbMsgWarning:	fColor = setmagenta; break;
			default:				fColor = setblack; break;
		}
	} else fColor = Color;
	fClassName = ClassName ? ClassName : "";
	fMethod = Method ? Method : "";
	fText = Text;
	fIndent = Indent;
};
		
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns formatted message text
//! \param[out]		FmtMsg			-- where to store the message
//! \param[in]		ProgName		-- name of program issuing this message
//! \param[in]		WithDate		-- output date if TRUE
//! \param[in]		WithColors		-- add colors if TRUE
//! \retval 		FmtMsg			-- pointer to string
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMrbLogMessage::Get(TString & FmtMsg, const Char_t * ProgName, Bool_t WithDate, Bool_t WithColors) const
{
	FmtMsg = "";

	if (WithDate) {
		switch (fType) {
			case kMrbMsgMessage:	FmtMsg += "*-["; break;
			case kMrbMsgError:		FmtMsg += "?-["; break;
			case kMrbMsgWarning:	FmtMsg += "!-["; break;
			default:				FmtMsg += "  ["; break;
		}
		FmtMsg += fDatime.AsString();
		FmtMsg += "]- ";
	}

	if (ProgName != NULL && *ProgName != '\0') {
		FmtMsg += ProgName;
		FmtMsg += " <> ";
	}
	if (WithColors) FmtMsg += fColor;

	TString fmt = FmtMsg;

	if (fClassName.Length() > 0) {
		FmtMsg += fClassName;
		FmtMsg += ":";
		if (fMethod.Length() > 0) {
			FmtMsg += ":";
			FmtMsg += fMethod;
			FmtMsg += "(): ";
		} else {
			FmtMsg += " ";
		}
	} else if (fMethod.Length() > 0) {
		FmtMsg += fMethod;
		FmtMsg += "(): ";
	}

	if (fIndent) {
		Int_t l = FmtMsg.Length() - fmt.Length();
		FmtMsg = Form("%s%*s", fmt.Data(), l, "");
 	}

	FmtMsg += fText;
	if (WithColors) FmtMsg += setblack;

	return(FmtMsg.Data());
}
	
//__________________________________________________________________[C++ ctor]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a message logger object
//! \param[in]		ProgName	-- program starting the logger
//! \param[in]		LogFile		-- name of log file
/////////////////////////////////////////////////////////////////////////////

TMrbLogger::TMrbLogger(const Char_t * ProgName, const Char_t * LogFile)
{
	fLog = NULL;
	if (LogFile != NULL && *LogFile != '\0') fLog = new ofstream();

	fOut = new ostrstream();
	fErr = new ostrstream();
	fWrn = new ostrstream();
	fEnabled = TMrbLogger::kMrbMsgCout | TMrbLogger::kMrbMsgCerr;
	if (fLog) fEnabled |= TMrbLogger::kMrbMsgLog;
	fLofMessages.Delete();
	fIndexOfLastPrinted = 0;
	this->SetProgName(ProgName);
	if (fLog) this->Open(LogFile);
	this->SetName((fProgName.Length() > 0) ? fProgName.Data() : fLogFile.Data());
	this->SetTitle("MARaBOU to Lynx: Error & message logger");
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines the program name
//! \param[in]		ProgName		-- program name
//////////////////////////////////////////////////////////////////////////////

void TMrbLogger::SetProgName(const Char_t * ProgName)
{
	fProgName = ProgName;
	if (fProgName.Length() > 0) this->SetName(fProgName.Data());
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a file to store (error) messages
//! \param[in]		LogFile		-- name of log file
//! \param[in]		Option		-- option: \a new or \a append
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbLogger::Open(const Char_t * LogFile, const Char_t * Option)
{
	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	fLogFile = LogFile;

	opt.AddNamedX(kMrbMsgOpenModes);
	nx = opt.FindByName(Option);
	if (nx != NULL && nx->GetIndex() == TMrbLogger::kMrbMsgFileNew) {
			fLog->open(fLogFile.Data(), ios::out);
	} else {
			fLog->open(fLogFile.Data(), ios::app);
	}
	
	if (!fLog->good()) {
		cerr	<< setred
				<< this->ClassName() << "::Open(): Can't open log file - " << fLogFile.Data()
				<< setblack << endl;
		return(kFALSE);
	}
	cout	<< setblue
			<< this->ClassName() << "::Open(): Writing (error) messages to log file " << basename(fLogFile.Data())
			<< setblack << endl;
	return(kTRUE);	
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Closes current log file
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbLogger::Close()
{
	TString logRoot;

	if (fLog && fLog->good()) {
		fLog->close();
		cout	<< setblue
				<< this->ClassName() << "::Close(): Closing log file " << fLogFile.Data()
				<< " (" << fLofMessages.GetEntries() + 1 << " message(s))"
				<< setblack << endl;
		fLofMessages.Delete();
		fIndexOfLastPrinted = 0;	
		return(kTRUE);
	} else return(kFALSE);
}
 
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs any data contained in message buffers
//! 				to log file, cout, and cerr, resp.
//! \param[in]		ClassName		-- calling class
//! \param[in]		Method			-- calling method
//! \param[in]		Color			-- text color to be used
//! \param[in]		Indent			-- if TRUE: don't output class/method but indent text
//! \return 		TRUE or FALSE
/////////////////////////////////////////////////////////////////////////////

Bool_t TMrbLogger::Flush(const Char_t * ClassName, const Char_t * Method, const Char_t * Color, Bool_t Indent)
{
	TString str;
	TMrbLogMessage * msg;
		
	*fOut << ends;
	str = fOut->str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgMessage, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCout) cout << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName, kTRUE, kFALSE) << flush;
	}
	delete fOut;
	fOut = new ostrstream();
	
	*fErr << ends;
	str = fErr->str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgError, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	delete fErr;
	fErr = new ostrstream();

	*fWrn << ends;
	str = fWrn->str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgWarning, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	delete fWrn;
	fWrn = new ostrstream();
		
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs message to log file, cout, and cerr, resp.
//! \param[in]		MsgType 		-- message type (message, error, warning)
//! \param[in]		Msg 			-- message text
//! \param[in]		ClassName		-- calling class
//! \param[in]		Method			-- calling method
//! \param[in]		Color			-- text color to be used
//! \param[in]		Indent			-- if TRUE: don't output class/method but indent text
//! \return 		TRUE or FALSE
/////////////////////////////////////////////////////////////////////////////

Bool_t TMrbLogger::OutputMessage(TMrbLogMessage::EMrbMsgType MsgType, const Char_t * Msg,
						const Char_t * ClassName, const Char_t * Method, const Char_t * Color, Bool_t Indent)
{
	TString str = Msg;
	if (str(str.Length()) != '\n') str += '\n';

	TMrbLogMessage * msg;
		
	if (MsgType == TMrbLogMessage::kMrbMsgMessage) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgMessage, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCout) cout << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName, kTRUE, kFALSE) << flush;
	} else if (MsgType == TMrbLogMessage::kMrbMsgError) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgError, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	} else if (MsgType == TMrbLogMessage::kMrbMsgWarning) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgWarning, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns number of entries
//! \param[in]		Type			-- message type
//! \retval 		NofEntries		-- number of entries found
/////////////////////////////////////////////////////////////////////////////

Int_t TMrbLogger::GetNofEntries(UInt_t Type) const
{
	Int_t nofEntries = 0;
	TMrbLogMessage * msg = (TMrbLogMessage *) fLofMessages.At(0);
	while (msg) {
		if (msg->GetType() & Type) nofEntries++;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
	return(nofEntries);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Extracts messages of given type
//! \param[out]		MsgArr			-- where to store messages
//! \param[in]		Start			-- entry to start with
//! \param[in]		Type			-- message type
//! \retval 		NofEntries		-- number of entries found
/////////////////////////////////////////////////////////////////////////////

Int_t TMrbLogger::GetEntriesByType(TList & MsgArr, Int_t Start, UInt_t Type) const
{
	MsgArr.Clear();
	if (Start > fLofMessages.GetEntries() + 1) return(0);
	if (Start < 0) Start = 0;
	Int_t nofEntries = 0;
	TMrbLogMessage * msg = (TMrbLogMessage *) fLofMessages.At(Start);
	while (msg) {
		if (msg->GetType() & Type) {
			MsgArr.Add(msg);
			nofEntries++;
		}
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	};
	return(nofEntries);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns last message
//! \param[in]		Type			-- message type
//! \retval 		LastMsg 		-- pointer to last message of requested type
/////////////////////////////////////////////////////////////////////////////

TMrbLogMessage * TMrbLogger::GetLast(const Char_t * Type) const
{
	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Type)) != NULL) {
		return(this->GetLast(nx->GetIndex()));
	} else {
		cerr	<< setred
				<< this->ClassName() << "::GetLast(): Illegal message type - " << Type
				<< setblack << endl;
		return(NULL);
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns last message
//! \param[in]		Type			-- message type
//! \retval 		LastMsg 		-- pointer to last message of requested type
/////////////////////////////////////////////////////////////////////////////

TMrbLogMessage * TMrbLogger::GetLast(UInt_t Type) const
{
	TMrbLogMessage * msg;
			
	msg = (TMrbLogMessage *) fLofMessages.At(fLofMessages.GetEntries() - 1);
	if (msg->GetType() & Type) return(msg); else return(NULL);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs (part of) message table
//! \param[in]		Tail			-- output last \a Tail messages only
//! \param[in]		Type			-- message type
/////////////////////////////////////////////////////////////////////////////

void TMrbLogger::Print(Int_t Tail, const Char_t * Type) const
{
	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Type)) != NULL) {
		this->Print(Tail, nx->GetIndex());
	} else {
		cerr	<< setred
				<< this->ClassName() << "::Print(): Illegal message type - " << Type
				<< setblack << endl;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs (part of) message table
//! \param[in]		Tail			-- output last \a Tail messages only
//! \param[in]		Type			-- message type
/////////////////////////////////////////////////////////////////////////////

void TMrbLogger::Print(Int_t Tail, UInt_t Type) const
{
	Int_t idx;
	TMrbLogMessage * msg;
	TString str;
			
	if (Tail >= 0) idx = 0; else idx = fLofMessages.GetEntries() - Tail;
	if (idx < 0) idx = 0;
	
	msg = (TMrbLogMessage *) fLofMessages.At(idx);
	while (msg) {
		if (msg->GetType() & Type) cout << msg->Get(str, "", kTRUE, kTRUE) << flush;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs recently created messages
//! \param[in]		Type			-- message type
/////////////////////////////////////////////////////////////////////////////

void TMrbLogger::PrintSinceLastCall(const Char_t * Type)
{
	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Type)) != NULL) {
		this->PrintSinceLastCall(nx->GetIndex());
	} else {
		cerr	<< setred
				<< this->ClassName() << "::PrintSinceLastCall(): Illegal message type - " << Type
				<< setblack << endl;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Outputs recently created messages
//! \param[in]		Type			-- message type
/////////////////////////////////////////////////////////////////////////////

void TMrbLogger::PrintSinceLastCall(UInt_t Type)
{
	TMrbLogMessage * msg;
	TString str;
			
	msg = (TMrbLogMessage *) fLofMessages.At(fIndexOfLastPrinted);
	while (msg) {
		if (msg->GetType() & Type) cout << msg->Get(str, "", kTRUE, kTRUE) << flush;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
	fIndexOfLastPrinted = fLofMessages.GetEntries() + 1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Formats a line prefix<br>
//! 				Id-[Date & Time]- ProgName
//! \param[in]		Identifier			-- line identifier
//! \param[in]		ProgName			-- calling program
//! \retval 		Prefix				-- line prefix
//////////////////////////////////////////////////////////////////////////////

const Char_t * TMrbLogger::Prefix(const Char_t * Identifier, const Char_t * ProgName)
{

	TDatime dt;
	fPrefix = Identifier;
	fPrefix += "-[";
	fPrefix += dt.AsString();
	fPrefix += "]- ";
	if (ProgName != NULL && *ProgName != '\0') {
		fPrefix += ProgName;
		fPrefix += " <> ";
	}
	return(fPrefix.Data());
}
	
