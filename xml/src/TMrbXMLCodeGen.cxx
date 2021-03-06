//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/src/TMrbXMLCodeGen.cxx
// Purpose:        MARaBOU XML based code generator
// Description:    Implements class methods to process xml data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXMLCodeGen.cxx,v 1.6 2008-03-05 12:23:44 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TMrbLogger.h"
#include "TMrbXMLCodeGen.h"
#include "TMrbXMLCodeElem.h"

#include "SetColor.h"

ClassImp(TMrbXMLCodeGen)

extern TMrbLogger * gMrbLog;

ofstream debug;

TMrbXMLCodeGen::TMrbXMLCodeGen(const Char_t * XmlFile, TMrbXMLCodeClient * Client, Bool_t VerboseFlag) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen
// Purpose:        Marabou's SAX parser implementation
// Arguments:      Char_t * XmlFile           -- xml file
//                 TMrbXMLCodeClient * Client -- client class
//                 Bool_t VerboseFlag         -- be verbose if kTRUE
// Results:        --
// Exceptions:
// Description:    Connects to TSAXParser and parses xml data from file
//                 This ctor has to be used for *CONTIGUOUS* mode
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("sax.log");

	fParser = new TSAXParser();
	if (!fParser->IsZombie()) {
		fParser->ConnectToHandler(this->ClassName(), this);
		fClient = Client;
		fRoot = NULL;
		fCurrent = NULL;
		fSelectionTag = "";
		fSelectiveMode = kFALSE;
		fStopParsing = kFALSE;

		fLofSubst = NULL;

		if (fClient)	fVerboseMode = fClient->IsVerbose();
		else			fVerboseMode = gEnv->GetValue("TMrbXML.VerboseMode", VerboseFlag);

		this->Initialize();

		if (XmlFile != NULL && *XmlFile != '\0') {
			if (!this->ParseFile(XmlFile)) this->MakeZombie();
		}
	}
}

TMrbXMLCodeGen::TMrbXMLCodeGen(const Char_t * XmlFile, TMrbXMLCodeClient * Client, const Char_t * Tag, TEnv * LofSubst, Bool_t VerboseFlag) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen
// Purpose:        Marabou's SAX parser implementation
// Arguments:      Char_t * XmlFile           -- xml file
//                 TMrbXMLCodeClient * Client -- client class
//                 Char_t * Tag               -- tag to be selected
//                 TEnv * LofSubst            -- list of substutitions
//                 Bool_t VerboseFlag         -- be verbose if kTRUE
// Results:        TString & Code
// Exceptions:
// Description:    Connects to TSAXParser and parses xml data from file
//                 This ctor has to be used for *SELECTIVE* mode
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("sax.log");

	fParser = new TSAXParser();
	if (!fParser->IsZombie()) {
		fParser->ConnectToHandler(this->ClassName(), this);
		fClient = Client;
		fRoot = NULL;
		fCurrent = NULL;
		fSelectiveMode = kTRUE;
		fSelectionTag = Tag;
		fStopParsing = kFALSE;

		fVerboseMode = fClient->IsVerbose();

		this->Initialize();

		fLofSubst = LofSubst;
		this->ParseFile(XmlFile);
	}
}

void TMrbXMLCodeGen::Initialize() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::Initialize
// Purpose:        Initialization
// Arguments:      --
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	fLofElements.SetName("List of XML elements");
	fLofElements.AddNamedX(kMrbXml_MrbCode, "mrbcode", "<mrbcode>...</mrbcode>");
	fLofElements.AddNamedX(kMrbXml_File, "file", "<file>...</file>");
	fLofElements.AddNamedX(kMrbXml_FileHdr, "fileHdr", "<fileHdr>...</fileHdr>");
	fLofElements.AddNamedX(kMrbXml_Class, "class", "<class>...</class>");
	fLofElements.AddNamedX(kMrbXml_ClassHdr, "classHdr", "<classHdr>...</classHdr>");
	fLofElements.AddNamedX(kMrbXml_Ctor, "ctor", "<ctor>...</ctor>");
	fLofElements.AddNamedX(kMrbXml_CtorHdr, "ctorHdr", "<ctorHdr>...</ctorHdr>");
	fLofElements.AddNamedX(kMrbXml_Method, "method", "<method>...</method>");
	fLofElements.AddNamedX(kMrbXml_MethodHdr, "methodHdr", "<methodHdr>...</methodHdr>");
	fLofElements.AddNamedX(kMrbXml_Funct, "funct", "<funct>...</funct>");
	fLofElements.AddNamedX(kMrbXml_FunctHdr, "functHdr", "<functHdr>...</functHdr>");
	fLofElements.AddNamedX(kMrbXml_Code, "code", "<code>...</code>");
	fLofElements.AddNamedX(kMrbXml_Cbody, "cbody", "<cbody>...</cbody>");
	fLofElements.AddNamedX(kMrbXml_Include, "include", "<include>...</include>");
	fLofElements.AddNamedX(kMrbXml_If, "if", "<if>...</if>");
	fLofElements.AddNamedX(kMrbXml_Foreach, "foreach", "<foreach>...</foreach>");
	fLofElements.AddNamedX(kMrbXml_Switch, "switch", "<switch>...</switch>");
	fLofElements.AddNamedX(kMrbXml_Case, "case", "<case>...</case>");
	fLofElements.AddNamedX(kMrbXml_Task, "task", "<task>...</task>");
	fLofElements.AddNamedX(kMrbXml_Item | kMrbXmlHasOwnText, "item", "<item>...</item>");
	fLofElements.AddNamedX(kMrbXml_Ftype | kMrbXmlHasOwnText, "ftype", "<ftype>...</ftype>");
	fLofElements.AddNamedX(kMrbXml_ArgList, "argList", "<argList>...</argList>");
	fLofElements.AddNamedX(kMrbXml_Arg, "arg", "<arg>...</arg>");
	fLofElements.AddNamedX(kMrbXml_Atype | kMrbXmlHasOwnText, "atype", "<atype>...</atype>");
	fLofElements.AddNamedX(kMrbXml_ReturnVal, "returnVal", "<returnVal>...</returnVal>");
	fLofElements.AddNamedX(kMrbXml_Comment | kMrbXmlHasOwnText, "comment", "<comment>...</comment>");
	fLofElements.AddNamedX(kMrbXml_RootClassList, "rootClassList", "<rootClassList>...</rootClassList>");
	fLofElements.AddNamedX(kMrbXml_MrbClassList, "mrbClassList", "<mrbClassList>...</mrbClassList>");
	fLofElements.AddNamedX(kMrbXml_Inheritance, "inheritance", "<inheritance>...</inheritance>");
	fLofElements.AddNamedX(kMrbXml_ClassRef, "classRef", "<classRef>...</classRef>");
	fLofElements.AddNamedX(kMrbXml_Tag | kMrbXmlHasOwnText, "tag", "<tag>...</tag>");
	fLofElements.AddNamedX(kMrbXml_Subtag | kMrbXmlHasOwnText, "subtag", "<subtag>...</subtag>");
	fLofElements.AddNamedX(kMrbXml_Flag | kMrbXmlHasOwnText, "flag", "<flag>...</flag>");
	fLofElements.AddNamedX(kMrbXml_Fpath | kMrbXmlHasOwnText, "fpath", "<fpath>...</fpath>");
	fLofElements.AddNamedX(kMrbXml_Slist, "slist", "<slist>...</slist>");
	fLofElements.AddNamedX(kMrbXml_Subst, "subst", "<subst>...</subst>");
	fLofElements.AddNamedX(kMrbXml_Clist, "clist", "<clist>...</clist>");
	fLofElements.AddNamedX(kMrbXml_Content, "content", "<content>...</content>");
	fLofElements.AddNamedX(kMrbXml_Xname | kMrbXmlHasOwnText, "xname", "<xname>...</xname>");
	fLofElements.AddNamedX(kMrbXml_Mname | kMrbXmlHasOwnText, "mname", "<mname>...</mname>");
	fLofElements.AddNamedX(kMrbXml_Fname | kMrbXmlHasOwnText, "fname", "<fname>...</fname>");
	fLofElements.AddNamedX(kMrbXml_Gname | kMrbXmlHasOwnText, "gname", "<gname>...</gname>");
	fLofElements.AddNamedX(kMrbXml_Cname | kMrbXmlHasOwnText, "cname", "<cname>...</cname>");
	fLofElements.AddNamedX(kMrbXml_Aname | kMrbXmlHasOwnText, "aname", "<aname>...</aname>");
	fLofElements.AddNamedX(kMrbXml_Vname | kMrbXmlHasOwnText, "vname", "<vname>...</vname>");
	fLofElements.AddNamedX(kMrbXml_Sname | kMrbXmlHasOwnText, "sname", "<sname>...</sname>");
	fLofElements.AddNamedX(kMrbXml_Purp | kMrbXmlHasOwnText, "purp", "<purp>...</purp>");
	fLofElements.AddNamedX(kMrbXml_Descr | kMrbXmlHasOwnText, "descr", "<descr>...</descr>");
	fLofElements.AddNamedX(kMrbXml_Author | kMrbXmlHasOwnText, "author", "<author>...</author>");
	fLofElements.AddNamedX(kMrbXml_Mail | kMrbXmlHasOwnText, "mail", "<mail>...</mail>");
	fLofElements.AddNamedX(kMrbXml_Url | kMrbXmlHasOwnText, "url", "<url>...</url>");
	fLofElements.AddNamedX(kMrbXml_Version | kMrbXmlHasOwnText, "version", "<version>...</version>");
	fLofElements.AddNamedX(kMrbXml_Date | kMrbXmlHasOwnText, "date", "<date>...</date>");
	fLofElements.AddNamedX(kMrbXml_s | kMrbXmlHasOwnText, "s", "<s>...</s>");
	fLofElements.AddNamedX(kMrbXml_S | kMrbXmlHasOwnText, "S", "<S>...</S>");
	fLofElements.AddNamedX(kMrbXml_L | kMrbXmlHasOwnText, "l", "<l>...</l>");
	fLofElements.AddNamedX(kMrbXml_B | kMrbXmlHasOwnText, "b", "<b>...</b>");
	fLofElements.AddNamedX(kMrbXml_I | kMrbXmlHasOwnText, "i", "<i>...</i>");
	fLofElements.AddNamedX(kMrbXml_U | kMrbXmlHasOwnText, "u", "<u>...</u>");
	fLofElements.AddNamedX(kMrbXml_Bx | kMrbXmlHasOwnText, "bx", "<bx>...</bx>");
	fLofElements.AddNamedX(kMrbXml_M | kMrbXmlHasOwnText, "m", "<m>...</m>");
	fLofElements.AddNamedX(kMrbXml_Mm | kMrbXmlHasOwnText, "mm", "<mm>...</mm>");
	fLofElements.AddNamedX(kMrbXml_R | kMrbXmlHasOwnText, "r", "<r>...</r>");
	fLofElements.AddNamedX(kMrbXml_Rm | kMrbXmlHasOwnText, "rm", "<rm>...</rm>");
	fLofElements.AddNamedX(kMrbXml_Nl, "nl", "<nl>...</nl>");

	fDebugMode = gEnv->GetValue("TMrbXML.Debug.Mode", "off");
	if (fDebugMode.IsNull()) fDebugMode = "off";
	fDebugFocusOnElement = gEnv->GetValue("TMrbXML.Debug.FocusOnElement", "any");
	if (fDebugFocusOnElement.IsNull()) fDebugFocusOnElement = "any";
	fDebugFocusOnTag = gEnv->GetValue("TMrbXML.Debug.FocusOnTag", "any");
	if (fDebugFocusOnTag.IsNull()) fDebugFocusOnTag = "any";
	fDebugOutput = gEnv->GetValue("TMrbXML.Debug.Output", "cout");
	if (fDebugOutput.IsNull()) fDebugOutput = "cout";

	if (fDebugMode.CompareTo("on") == 0 && fDebugOutput.CompareTo("cout") != 0) {
		debug.open(fDebugOutput.Data(), ios::out);
		if (!debug.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << fDebugOutput << endl;
			gMrbLog->Flush(this->ClassName(), "Initialize");
			fDebugMode = "off";
		} else {
			gMrbLog->Out() << "Writing DEBUG info to file " << fDebugOutput << endl;
			gMrbLog->Flush(this->ClassName(), "Initialize");
		}
	}
}

Bool_t TMrbXMLCodeGen::ParseFile(const Char_t * XmlFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::ParseFile
// Purpose:        Parse XML data from file
// Arguments:      Char_t * XmlFile   -- filename
//                 Char_t * Prefix    -- prefix to be prepended to file name
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (!gSystem->AccessPathName(XmlFile)) {
		fXmlFile = XmlFile;
		Int_t err = fParser->ParseFile(fXmlFile.Data());	// parse xml data frttom file
		return(err == 0);
	} else {
		gMrbLog->Err() << gSystem->GetError() << " - " << XmlFile << endl;
		gMrbLog->Flush(this->ClassName(), "ParseFile");
		return(kFALSE);
	}
	return(kTRUE);
}

const Char_t * TMrbXMLCodeGen::SelectionTag() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::SelectionTag
// Purpose:        Return tag in selective mode
// Arguments:      --
// Results:        Char_t * Tag    -- selection tag
//////////////////////////////////////////////////////////////////////////////

	if (this->SelectiveMode()) {
		return(fSelectionTag.Data());
	} else {
		gMrbLog->Err() << "Not valid in *CONTIGUOUS* mode" << endl;
		gMrbLog->Flush(this->ClassName(), "GetSelectionTag");
		return(NULL);
	}
}

Bool_t TMrbXMLCodeGen::StopParsing(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::StopParsing
// Purpose:        Set stop flag in selective mode
// Arguments:      Bool_t Flag     -- kTRUE/kFALSE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (this->SelectiveMode()) {
		fStopParsing = Flag;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Not valid in *CONTIGUOUS* mode" << endl;
		gMrbLog->Flush(this->ClassName(), "StopParsing");
		return(kFALSE);
	}
}

void TMrbXMLCodeGen::OnStartElement(const Char_t * ElemName, const TList * LofAttr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnStartElement
// Purpose:        To be called on start of an element
// Arguments:      Char_t * ElemName   -- name of element
//                 TList * LofAttr     -- list of attributes
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	if (fStopParsing) return;

	Int_t nestingLevel = fCurrent ? fCurrent->NestingLevel() + 1 : 0;
	TMrbNamedX * elem = fLofElements.FindByName(ElemName, TMrbLofNamedX::kFindExact);
	if (elem == NULL) {
		gMrbLog->Err() << "No such XML element - <" << ElemName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "OnStartElement");
		elem = new TMrbNamedX(kMrbXmlIsZombie, ElemName);
	}
	elem->AssignObject(fCurrent);
	if (fCurrent && fCurrent->IsZombie()) elem->SetIndex(kMrbXmlIsZombie);
	fCurrent = new TMrbXMLCodeElem(elem, nestingLevel, this);

	fCurrent->SetAttributes(LofAttr);

	if (nestingLevel == 0) {
		fRoot = fCurrent;

		TString type;
		if (!fCurrent->FindAttr("type", type)) {
			gMrbLog->Err() << "[" << ElemName << "] Parsing STOPPED" << endl;
			gMrbLog->Flush(this->ClassName(), "OnStartElement");
			fStopParsing = kTRUE;
			return;
		}

		if (type.CompareTo("contiguous") == 0 && this->SelectiveMode()) {
			gMrbLog->Err() << fCurrent->MsgHdr() << " Wrong XML type - \"contiguous\" (should be \"selective\")" << endl;
			gMrbLog->Flush(this->ClassName(), "OnStartElement");
			gMrbLog->Err() << fCurrent->MsgHdr() << " Parsing STOPPED" << endl;
			gMrbLog->Flush(this->ClassName(), "OnStartElement", setred, kTRUE);
			fStopParsing = kTRUE;
			return;
		} else if (type.CompareTo("selective") == 0  && !this->SelectiveMode()) {
			gMrbLog->Err() << fCurrent->MsgHdr() << " Wrong XML type - \"selective\" (should be \"contiguous\")" << endl;
			gMrbLog->Flush(this->ClassName(), "OnStartElement");
			gMrbLog->Err() << fCurrent->MsgHdr() << " Parsing STOPPED" << endl;
			gMrbLog->Flush(this->ClassName(), "OnStartElement", setred, kTRUE);
			fStopParsing = kTRUE;
			return;
		}
	}

	if (fDebugMode.CompareTo("on") == 0) {
		if (fDebugOutput.CompareTo("cout") == 0) {
			fCurrent->Debug(cout, fDebugFocusOnElement, fDebugFocusOnTag, kTRUE);
		} else {
			fCurrent->Debug(debug, fDebugFocusOnElement, fDebugFocusOnTag, kTRUE);
		}
	}
}

void TMrbXMLCodeGen::OnEndElement(const Char_t * ElemName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnEndElement
// Purpose:        To be called on end of an element
// Arguments:      Char_t * ElemName   -- name of element
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	if (fStopParsing) return;

	fCurrent->ProcessElement(ElemName); 				// process element
	if (fDebugMode.CompareTo("on") == 0) {
		if (fDebugOutput.CompareTo("cout") == 0) {
			fCurrent->Debug(cout, fDebugFocusOnElement, fDebugFocusOnTag, kFALSE);
		} else {
			fCurrent->Debug(debug, fDebugFocusOnElement, fDebugFocusOnTag, kFALSE);
		}
	}

	if (fCurrent != fRoot) {							// preserve root element
		TMrbXMLCodeElem * parent = fCurrent->Parent();	// any other elements go 1 level up
		delete fCurrent;
		fCurrent = parent;
	}
}

void TMrbXMLCodeGen::OnCharacters(const Char_t * Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnCharacters
// Purpose:        To be called on text/code
// Arguments:      Char_t * Text   -- text or code string
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	if (!fCurrent->IsZombie() && fCurrent->HasOwnText()) fCurrent->AddCode(Text);
}

void TMrbXMLCodeGen::OnWarning(const Char_t * Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnWarning
// Purpose:        To be called on warnings
// Arguments:      Char_t * Msg   -- warning message
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	gMrbLog->Wrn() << Msg << endl;
	gMrbLog->Flush(this->ClassName(), "OnWarning");
}

void TMrbXMLCodeGen::OnError(const char * Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnError
// Purpose:        To be called on errors
// Arguments:      Char_t * Msg   -- error message
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	gMrbLog->Err() << Msg;
	gMrbLog->Flush(this->ClassName(), "OnError");
}

void TMrbXMLCodeGen::OnFatalError(const char * Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeGen::OnFatalError
// Purpose:        To be called on fatal errors
// Arguments:      Char_t * Msg   -- error message
// Results:        --
//////////////////////////////////////////////////////////////////////////////

	gMrbLog->Err() << "[Fatal] " << Msg;
	gMrbLog->Flush(this->ClassName(), "OnFatalError");
}
