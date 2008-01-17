#ifndef __TMrbXMLCodeGen_h__
#define __TMrbXMLCodeGen_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/inc/TMrbXMLCodeGen.h
// Purpose:        MARaBOU's interface to XML
// Class:          TMrbXMLCodeGen    -- Marabou's SAX parser implementation
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXMLCodeGen.h,v 1.4 2008-01-17 09:26:13 Rudolf.Lutter Exp $       
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

    	TMrbXMLCodeGen(const Char_t * XmlFile = NULL, TMrbXMLCodeClient * Client = NULL);
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

	protected:

		void Initialize();							// initialize

	protected:

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
