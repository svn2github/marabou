#ifndef __TMrbTransport_h__
#define __TMrbTransport_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           transport/inc/TMrbTransport.h
// Purpose:        Define transport class for MARaBOU
// Classes:        TMrbTransport  -- base class
// Description:    Class definition to implement a transport mechanism
//                 for MBS event data
// ROOT classes:   TNamed
// Keywords:
// Author:         R. Lutter
// Revision:                  
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "Rtypes.h"
#include "TNamed.h"
#include "TSystem.h"

#include "TMrbLofNamedX.h"

#include "mbsio.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport
// Purpose:        MBS to ROOT transport mechanism
// Methods:        Open         -- open connection (network or LMD file)
//                 Close        -- close connection
//                 NextEvent    -- read next event
//                 NextSubevent -- unpack next subevent
//                 SubeventWC   -- return current wc
//                 SubeventID   -- return current id
//                 GetMbsBase   -- return base addr of MBS data
//                 Show         -- show data
//                 SetShow      -- define what to be shown automatically
//                 ShowStat     -- show event statistics
//                 SetStat      -- define automatic statistics output
//                 SetStream    -- define stream profile
//                 GetMode      -- find a mode index by its name
//                 OpenLogFile  -- open a file for data/error logging
// Description:    Base class to handle data transport from MBS to ROOT.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTransport : public TNamed {

	public:
		enum					{	kSizeOfData	=	0x4000	};
		enum					{	kERROR		=	-1		};
		enum					{	kEOF		=	0		};
		enum					{	kEOE		=	kEOF	};
		enum					{	kMaxErrors	=	10		};

		enum EMrbRunStatus		{	kReady				=	0,
									kRunning			=	0x1,
									kTerminate			=	0x2,
									kError				=	0x4
								};

		enum EMrbTransportMode	{	kUndefined			=	0,
									kSync				=	MBS_CTYPE_SYNC,
									kAsync				=	MBS_CTYPE_ASYNC,
									kFile				=	MBS_CTYPE_FILE,
									kRemote 			=	MBS_CTYPE_REMOTE
								};

		enum EMrbBufferElements	{	kBufferHeader		=	0x1,
									kFileHeader 		=	0x2,
									kEventHeader		=	0x4,
									kSubeventHeader 	=	0x8,
									kSubeventData		=	0x10
								};

	public:

		TMrbTransport(const Char_t *, const Char_t * XptTitle = "");	// create transport
		~TMrbTransport() {};								// default dtor

		Bool_t Open(const Char_t *, const Char_t * Mode = "FILE", Int_t BufferSize = TMrbTransport::kSizeOfData); 	// open connection
		Bool_t Close(); 									// open/close connection
		Bool_t FreeBuffers();								// free data base

		Int_t ReadEvents(Int_t NofEvents = 0);  			// read events from buffer

		UInt_t NextSubevent(UShort_t *);					// unpack next subevent
		const UShort_t * NextSubevent();
		inline UInt_t SubeventWC() { return(fMBSDataIO->sevt_wc); }; 	// return current wc
		inline UInt_t SubeventID() { return(fMBSDataIO->sevt_id); }; 	// return current id

		inline const MBSDataIO * GetMbsBase() { return(fMBSDataIO); };	// return base addr

		Bool_t OpenMEDFile(const Char_t * MEDFile); 		// write data as MBS event data
		Bool_t CloseMEDFile();		
		Bool_t OpenLMDFile(const Char_t * LMDFile); 		// write data to lmd file (GSI format)
		Bool_t CloseLMDFile();		
															// show data (default: S=subevents)
		Bool_t Show(const Char_t * BufElemKey = "Subevents", const Char_t * Output = NULL);

		Bool_t SetShow(const Char_t * BufElemStr = "Events:Subevents", Int_t ScaleDown = 100, const Char_t * Output = NULL);
															// set show profile
															// (default: ES=events+subevents)

		Bool_t ShowStat(const Char_t * Output = NULL); 		// show event statistics

		Bool_t SetStat(Int_t ScaleDown = 100, const Char_t * Output = NULL);
															// set scale down for event statistics
															// (default: every 100 events)

		Bool_t SetStream(Int_t MaxStreams = 0, Int_t SlowDown = 0); // stream profile

		Bool_t SetDumpInterval(Int_t NofRecs = 0);			// dump interval

		virtual Bool_t ProcessEvent(s_vehe *);			// process event data

		Bool_t PrintMbsIoError(const Char_t *);			// print mbsio errors

		inline Bool_t IsError() { return(fErrorFlag); }; 	// test error flag
		inline void ClearError() { fErrorFlag = kFALSE; };	// clear error flag
		inline void SetError() { fErrorFlag = kTRUE; }; 	// set error flag

		Bool_t Version();									// show version
		Bool_t OpenLogFile(const Char_t *);					// open log file

															// public lists of key words:

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbTransport.html&"); };

	public:
		TMrbLofNamedX fLofTransportModes;					// ... transport modes
		TMrbLofNamedX fLofBufferElements;					// ... buffer elements

	protected:

		TString fInputFile;									// input file
		EMrbTransportMode fTransportMode;					// transport mode
		Int_t fBufferSize;									// buffer size
		TString fErrorString;								// error message

		const MBSDataIO *fMBSDataIO;						// i/o data base

		Bool_t fErrorFlag;									// error indicator

	ClassDef(TMrbTransport, 0)		// [M_analyze] Connect to MBS transport manager
};

#endif
