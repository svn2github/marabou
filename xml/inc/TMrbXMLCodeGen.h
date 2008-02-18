#ifndef __TMrbXMLCodeGen_h__
#define __TMrbXMLCodeGen_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/inc/TMrbXMLCodeGen.h
// Purpose:        MARaBOU's interface to XML
// Class:          TMrbXMLCodeGen    -- Marabou's SAX parser implementation
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXMLCodeGen.h,v 1.5 2008-02-18 12:29:03 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <Riostream.h>
#include <TList.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TMrbXMLCodeElem.h"
#include "TMrbXMLCodeClient.h"


//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen
// Purpose:        Marabou's XML based code generator
// Description:    Connects to TSAXParser and parses xml data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbXMLCodeGen: public TObject {

	public:

    	TMrbXMLCodeGen(const Char_t * XmlFile = NULL, TMrbXMLCodeClient * Client = NULL, Bool_t VerboseFlag = kFALSE);
    	TMrbXMLCodeGen(const Char_t * XmlFile, TMrbXMLCodeClient * Client, const Char_t * Tag, TEnv * LofChildren, TEnv * LofSubst, TString & Code, Bool_t VerboseFlag = kFALSE);
		virtual ~TMrbXMLCodeGen() {};

		void OnStartDocument() {};
		void OnEndDocument() {};
		void OnStartElement(const char * ElemName, const TList * LofAttr);
		void OnEndElement(const char * ElemName);
		void OnCharacters(const char * Text);
		void OnComment(const char * Comment) {};
		void OnWarning(const char * Msg);
		void OnError(const char * Msg);
		void OnFatalError(const char * Msg);
		void OnCdataBlock(const char * Data, Int_t Length) {};

		Bool_t ParseFile(const Char_t * XmlFile); 	// parse xml data

		inline TMrbXMLCodeClient * Client() { return(fClient); };
		inline void ConnectToClient(TMrbXMLCodeClient * Client) { fClient = Client; };					// connect to client object

		inline Bool_t SelectiveMode() { return(fSelectiveMode); };
		const Char_t * GetSelectionTag();
		Bool_t SetExtractedCode(const Char_t * Code);
		Bool_t StopParsing(Bool_t Flag = kTRUE);
		inline TEnv * GetLofChildren() { return(fLofChildren); };
		inline TEnv * GetLofSubst() { return(fLofSubst); };
		inline Bool_t ParsingToBeStopped() { return(fStopParsing); };

		inline const Char_t * GetXmlFile() { return(fXmlFile.Data()); };

		inline Bool_t IsVerbose() { return(fVerboseMode); };

	protected:

		void Initialize();							// initialize

	protected:

		Bool_t fVerboseMode;			// kTRUE: be verbose
		Bool_t fSelectiveMode;			// kFALSE: contiguous (=file) mode - entire document will be processed
										// kTRUE: selective mode - extract one given tag only

		Bool_t fStopParsing;			// stop flag

		TString fSelectionTag;			// if in selective mode: tag to be searched for
		TEnv * fLofChildren;			// ... list of xml sub-tags
		TEnv * fLofSubst;				// ... list of substitutions
		TString fExtractedCode;			// ... code extracted due to tag selection

		TSAXParser * fParser;			// connection to SAX parser
		TString fXmlFile;				// xml document
		TMrbXMLCodeElem * fRoot; 		// root element
		TMrbXMLCodeElem * fCurrent; 	// current element

		TMrbLofNamedX fLofElements;		// list of elements

		TString fDebugMode;				// debug mode: on | off
		TString fDebugFocusOnElement; 	// debug focus on element: off | <elem>
		TString fDebugFocusOnTag;		// debug focus on tag: off | <tag>
		TString fDebugOutput;			// debug output: cout | <file>

		TMrbXMLCodeClient * fClient;	// client connect to code generator

	ClassDef(TMrbXMLCodeGen, 1) 		// [XML] code generator
};

#endif
