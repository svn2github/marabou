//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidy
// Purpose:        MARaBOU utilities: tidy interface
// Description:    Implements class methods to interface the tidy library
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbTidy.cxx,v 1.34 2007-08-03 10:11:05 Rudolf.Lutter Exp $       
// Date:           
//Begin_Html
/*
<b>Example of a HTML document (file "tidy.html")</b><br>
<pre>
&lt;!DOCTYPE HTML PUBLIC "-// IETF/DTD HTML 2.0// EN"&gt;
&lt;html&gt;
&lt;!-- Author: Marabou team (marabou@physik.uni-muenchen.de)  --&gt;
&lt;head&gt;&lt;title&gt;TMrbTidyDoc.html&lt;/title&gt;&lt;/head&gt;
&lt;body bgcolor="#ebfcb0"&gt;
&lt;h1&gt;TMrbTidyDoc: A MARaBOU class interfacing Dave Raggett's TidyLib&lt;/h1&gt;
&lt;/body&gt;
&lt;/html&gt;
</body>
<p>
<b>ROOT script to invoke TIDY:</b>
<pre>
{
&nbsp;&nbsp;&nbsp;&nbsp;gSystem->Load("$MARABOU/lib/libTMrbUtils.so");
&nbsp;&nbsp;&nbsp;&nbsp;gSystem->Load("$MARABOU/lib/libTMrbTidy.so");
&nbsp;&nbsp;&nbsp;&nbsp;gSystem->Load("$MARABOU/lib/libTidy.so");
&nbsp;&nbsp;&nbsp;&nbsp;TMrbTidyDoc * tdoc = new TMrbTidyDoc("demo", "tidy.html");
&nbsp;&nbsp;&nbsp;&nbsp;tdoc->Print();
}
</pre>
<b>Output from method TMrbTidyDoc::Print():</b>
<pre>
Document demo (file tidy.html): structure as analyzed by D. Raggett's TIDY
----------------------------------------------------------------------------------------------
[ 0] node root: type=Root(0) parent=root
[ 1] . node HTML: type=DOCTYPE(1) parent=root text='&lt;!DOCTYPE HTML&gt;&lt;cr&gt;&lt;cr&gt;' PUBLIC=
[ 1] . node html: type=StartTag(5) tagid=HTML(48) parent=root
[ 2] .. node comment: type=Comment(2) parent=html text='&lt;!-- Author: Marabou team (marabou@physik.uni-muenchen.de)  --&gt;&lt;cr&gt;&lt;cr&gt;'
[ 2] .. node head: type=StartTag(5) tagid=HEAD(46) parent=html
[ 3] ... node title: type=StartTag(5) tagid=TITLE(111) parent=head
[ 4] .... node text: type=Text(4) parent=title text='TMrbTidyDoc.html&lt;cr&gt;'
[ 2] .. node body: type=StartTag(5) tagid=BODY(16) parent=html bgcolor=#EBFCB0
[ 3] ... node h1: type=StartTag(5) tagid=H1(40) parent=body
[ 4] .... node text: type=Text(4) parent=h1 text='TMrbTidyDoc: A MARaBOU class interfacing Dave Raggett's TidyLib&lt;cr&gt;'
</pre>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TROOT.h"
#include "TObjString.h"
#include "SetColor.h"
#include "TMrbTidy.h"
#include "TMrbTidyCommon.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

ClassImp(TMrbTidyDoc)
ClassImp(TMrbTidyOption)
ClassImp(TMrbTidyNode)
ClassImp(TMrbTidyAttr)

TString tabEquiv("   ");

TMrbLofNamedX * gMrbLofTidyDocs = NULL;

extern TMrbLogger * gMrbLog;			// global access to logging system

TMrbTidyDoc::TMrbTidyDoc() : TNamed("TidyDoc", "Tidy document") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        Create a tidy document
// Arguments:      --
// Description:    Ctor to instantiate a tidy document
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	this->Reset();
	fHandle = tidyCreate();
	tidySetErrorBuffer(fHandle, &fErrorBuffer);
	this->AddToList();
}
		
TMrbTidyDoc::TMrbTidyDoc(const Char_t * DocName) : TNamed(DocName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        Create a tidy document
// Arguments:      Char_t * DocName       -- name of document
// Description:    Ctor to instantiate a tidy document
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	this->Reset();
	fHandle = tidyCreate();
	this->InitErrorBuffer();					// init error buffer
	this->AddToList();
}		
		
TMrbTidyDoc::TMrbTidyDoc(const Char_t * DocName, const Char_t * DocFile, Bool_t Repair, const Char_t * CfgFile) : TNamed(DocName, DocFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        Create a tidy document
// Arguments:      Char_t * DocName       -- name of document
//                 Char_t * DocFile       -- file containing document
//                 Bool_t Repair          -- clean and repair
//                 Char_t * CfgFile       -- Tidy's config file
// Description:    Ctor to instantiate a tidy document
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	this->Reset();								// reset
	fHandle = tidyCreate(); 					// init tidy system
	this->InitErrorBuffer();					// init error buffer
	if (!this->LoadConfig(CfgFile)) this->MakeZombie();	// load config
	if (!this->IsZombie()) {
		this->ReadOptions();					// read options
		this->SetOption(TidyXhtmlOut, kTRUE);	// convert to xhtml
		if (!this->ParseFile(DocFile, Repair)) this->MakeZombie();	// parse file
	}
	if (!this->IsZombie()) this->AddToList();
}		
		
TMrbTidyDoc::TMrbTidyDoc(const Char_t * DocName, istream & Stream, Bool_t Repair, const Char_t * CfgFile) : TNamed(DocName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        Create a tidy document
// Arguments:      Char_t * DocName       -- name of document
//                 istream & Stream       -- stream to read from
//                 Bool_t Repair          -- clean and repair
//                 Char_t * CfgFile       -- Tidy's config file
// Description:    Ctor to instantiate a tidy document
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	this->Reset();								// reset
	fHandle = tidyCreate(); 					// init tidy system
	this->InitErrorBuffer();					// init error buffer
	if (!this->LoadConfig(CfgFile)) this->MakeZombie();	// load config
	if (!this->IsZombie()) {
		this->ReadOptions();					// read options
		this->SetOption(TidyXhtmlOut, kTRUE);	// convert to xhtml
		TString tidyBuffer;
		tidyBuffer.ReadFile(Stream);			// read data to buffer
		this->ParseBuffer(tidyBuffer, Repair);	// parse buffer
		this->AddToList();
	}
}		
		
TMrbTidyDoc::TMrbTidyDoc(const Char_t * DocName, TString & Buffer, Bool_t Repair, const Char_t * CfgFile) : TNamed(DocName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        Create a tidy document
// Arguments:      Char_t * DocName       -- name of document
//                 TString & Buffer       -- buffer containing html data
//                 Bool_t Repair          -- clean and repair
//                 Char_t * CfgFile       -- Tidy's config file
// Description:    Ctor to instantiate a tidy document
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	this->Reset();								// reset
	fHandle = tidyCreate(); 					// init tidy system
	this->InitErrorBuffer();					// init error buffer
	if (!this->LoadConfig(CfgFile)) this->MakeZombie();	// load config
	if (!this->IsZombie()) {
		this->ReadOptions();					// read options
		this->SetOption(TidyXhtmlOut, kTRUE);	// convert to xhtml
		this->ParseBuffer(Buffer, Repair);		// parse buffer
		this->AddToList();
	}
}		

TMrbTidyOption::TMrbTidyOption(TidyOptionId OptId, const Char_t * OptName, TidyOption OptHandle, TMrbTidyDoc * Doc)
																		: TMrbNamedX((Int_t) OptId, OptName) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyOption
// Purpose:        A tidy option
// Arguments:      TidyOptionId OptId  	  -- option id
//                 Char_t * OptName       -- name
//                 TidyOption OptHandle   -- ptr to tidy struct
//                 TMrbTidyDoc * Doc          -- link to document
// Description:    Ctor to instantiate a tidy option
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fType = TidyString;
	fHandle = OptHandle;
	fTidyDoc = Doc;
	fType = tidyOptGetType(OptHandle);
}

TMrbTidyNode::TMrbTidyNode(TidyTagId NodeId, const Char_t * NodeName, TMrbTidyNode * Parent, TidyNode NodeHandle, TMrbTidyDoc * Doc)
																		: TMrbNamedX((Int_t) NodeId, NodeName) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode
// Purpose:        A tidy node
// Arguments:      TidyTagId NodeId  	  -- node id
//                 Char_t * NodeName      -- name
//                 TMrbTidyNode * Parent  -- link to parent node
//                 TidyOption NodeHandle  -- ptr to tidy struct
//                 TMrbTidyDoc * Doc          -- link to document
// Description:    Ctor to instantiate a tidy node
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fHandle = NodeHandle;
	fParent = Parent;
	fTidyDoc = Doc;
	fLofChilds.Delete();
	fLofChilds.SetName("Child Nodes");
	fLofAttr.Delete();
	fLofAttr.SetName("Node Attributes");
	fIndentLevel = 0;
	this->SetTreeLevelFromParent();
	this->ReadAttr();
	this->CheckMnode();
}

TMrbTidyAttr::TMrbTidyAttr(TidyAttrId AttrId, const Char_t * AttrName, TidyAttr AttrHandle, TMrbTidyNode * Node)
																	: TMrbNamedX((Int_t) AttrId, AttrName) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyAttr
// Purpose:        A tidy attribute
// Arguments:      TidyAttrId AttrId  	  -- attr id
//                 Char_t * AttrName      -- name
//                 TidyAttr AttrHanlde    -- ptr to tidy struct
//                 TMrbTidyNode * Node    -- node attr belongs to
// Description:    Ctor to instantiate a tidy attribute
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fHandle = AttrHandle;
	fValue.Resize(0);
	this->AssignObject(Node);
}

void TMrbTidyDoc::Reset(Bool_t Release) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::Reset
// Purpose:        Reset
// Arguments:      Bool_t Release  -- kTRUE -> release
// Results:        --
// Exceptions:
// Description:    Resets to initial values
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Release && fHandle) tidyRelease(fHandle);	// free tidy allocs
	fRepair = kFALSE;								// don't run reapir step
	fStripText = kFALSE;							// don't strip text
	fDocFile.Resize(0); 							// no doc file
	fCfgFile.Resize(0); 							// no config
	if (Release && fTidyRoot) {						// delete document tree
		fTidyRoot->DeleteTree();
		delete fTidyRoot;
	}
	fTidyRoot = NULL;								// no html data present
	fTidyHtml = NULL;
	fTidyHead = NULL;
	fTidyBody = NULL;
	fTidyMbody = NULL;

	fLofOptions.Delete();							// empty list of options
	fLofOptions.SetName("Tidy Options");

	fLofMnodes.Delete();								// empty list of marabou nodes
	fLofMnodes.SetName("Special MARaBOU Tags");
	fLofMnodes.AddNamedX(kMrbTidyMnodeIds); 			// define special nodes
}

void TMrbTidyDoc::InitErrorBuffer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::InitErrorBuffer
// Purpose:        Initialize tidy's error buffer
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets internal error buffer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	memset(&fErrorBuffer, 0, sizeof(TidyBuffer));
	if (fHandle) tidySetErrorBuffer(fHandle, &fErrorBuffer);
}

Bool_t TMrbTidyDoc::LoadConfig(const Char_t * CfgFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::LoadConfig
// Purpose:        Load Tidy's config file
// Arguments:      Char_t * CfgFile    -- config file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads config data from file (via tidyLoadConfig())
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fCfgFile.Resize(0); 							// no config
	if (CfgFile) {
		TString cfgFile = CfgFile;
		gSystem->ExpandPathName(cfgFile);
		if (gSystem->AccessPathName(cfgFile.Data())) { 	// check if file exists
			gMrbLog->Err()	<< "[" << this->GetName() << "] Can't access config file - " << CfgFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadConfig");
			return(kFALSE);
		}
		fCfgFile = cfgFile;
		tidyLoadConfig(fHandle, (Char_t *) cfgFile.Data());			// load config data
	}
	return(kTRUE);
}

Int_t TMrbTidyDoc::ReadOptions() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::ReadOptions
// Purpose:        Read tidy options
// Arguments:      --
// Results:        Int_t NofOptions   -- number of options read
// Exceptions:
// Description:    Reads options via tidyGetOptionsList()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofOptions.Delete();				// clear option list
	TidyIterator itOpt = tidyGetOptionList(fHandle);	// iterate over tidy's option list
	while (itOpt)
	{
  		TidyOption optHandle = tidyGetNextOption(fHandle, &itOpt);	// get handle to tidy option
		TMrbTidyOption * opt = new TMrbTidyOption(tidyOptGetId(optHandle), tidyOptGetName(optHandle), optHandle, this);
		fLofOptions.Add(opt);			// add to list
  	}
	return(fLofOptions.GetEntriesFast());
}

void TMrbTidyDoc::PrintOptions(ostream & Out, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::PrintOptions
// Purpose:        Print tidy options
// Arguments:      Bool_t Verbose  -- verbose
// Results:        --
// Exceptions:
// Description:    Prints tidy options.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fLofOptions.GetEntriesFast() > 0) {
		Out 	<< endl << "Document " << this->GetName();
		if (fDocFile.Length()) Out 	<< " (file " << fDocFile << ")";
		Out 	<< ": list of TIDY options" << endl
				<< "-------------------------------------------------------------------------" << endl
				<< " Id" << setw(30) << "Name" << setw(10) << "Value" << setw(10) << "Default" << endl
				<< "-------------------------------------------------------------------------" << endl;
		TMrbTidyOption * opt = (TMrbTidyOption *) fLofOptions.First();
		while (opt) {
			opt->Print(Out, Verbose);
			opt = (TMrbTidyOption *) fLofOptions.After(opt);
		}
	}
}

Bool_t TMrbTidyDoc::ParseFile(const Char_t * DocFile, Bool_t Repair) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::ParseFile
// Purpose:        Parse html data from file
// Arguments:      Char_t * DocFile     -- html document to be processed
//                 Bool_t Repair        -- kTRUE if tidy's repair mechanism has to be called
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Processes html data from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetTitle(DocFile);
	fDocFile.Resize(0); 							// no document
	TString docFile = DocFile;
	gSystem->ExpandPathName(docFile);
	if (gSystem->AccessPathName(docFile.Data())) { 		// check if file exists
		gMrbLog->Err()	<< "[" << this->GetName() << "] Can't access document file - " << docFile << endl;
		gMrbLog->Flush(this->ClassName(), "ParseFile");
		return(kFALSE);
	}
	ifstream docStrm(docFile.Data(), ios::in); 			// try to open file
	if (!docStrm.good()) {							// some error
		gMrbLog->Err() << "[" << this->GetName() << "] " << gSystem->GetError() << " - " << docFile << endl;
		gMrbLog->Flush(this->ClassName(), "ParseFile");
		return(kFALSE);
	}
	fDocFile = docFile;
	TString tidyBuffer; 							// fill buffer
	tidyBuffer.ReadFile(docStrm);					// from file
	docStrm.close();
	return(this->ParseBuffer(tidyBuffer.Data(), Repair));	// parse buffer
}

Bool_t TMrbTidyDoc::ParseBuffer(const Char_t * Buffer, Bool_t Repair) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::ParseBuffer
// Purpose:        Parse html data from buffer
// Arguments:      Char_t * Buffer      -- buffer with html data
//                 Bool_t Repair        -- kTRUE if tidy's repair mechanism has to be called
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Processes html data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->HasNodes()) {					// document tree has to be empty
		gMrbLog->Err() << "[" << this->GetName() << "] Document tree already filled - can't proceed" << endl;
		gMrbLog->Flush(this->ClassName(), "ParseBuffer");
		return(kFALSE);
	}

	tidyParseString(fHandle, (Char_t *) Buffer);			// parse html data
	fRepair = Repair;
	if (Repair) tidyCleanAndRepair(fHandle);				// try to repair things
	TidyNode node = tidyGetRoot(fHandle);
	fTidyRoot = new TMrbTidyNode(tidyNodeGetId(node), "root", NULL, node, this);
	fTidyRoot->SetType(TidyNode_Root);
	fTidyRoot->CheckEndTag();
	fTidyRoot->FillTree();
	fTidyHtml = (TMrbTidyNode *) fTidyRoot->GetLofChilds()->FindByIndex((Int_t) TidyTag_HTML);
	if (fTidyHtml) {
		fTidyHead = (TMrbTidyNode *) fTidyHtml->GetLofChilds()->FindByIndex((Int_t) TidyTag_HEAD);
		fTidyBody = (TMrbTidyNode *) fTidyHtml->GetLofChilds()->FindByIndex((Int_t) TidyTag_BODY);
	}
	if (fTidyBody) {
		fTidyMbody = (TMrbTidyNode *) fTidyBody->GetLofChilds()->FindByName("mb");
		if (fTidyMbody) {
			fTidyMbody->InitSubstitutions(kTRUE, kTRUE);
			fTidyMbody->InitLinks(kTRUE, kTRUE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbTidyDoc::CleanAndRepair() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::CleanAndRepair
// Purpose:        Start tidy's repair mechanism
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Repair inconsistencies via tidyCleanAndRepair().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;
	if (this->HasNodes()) {
		tidyCleanAndRepair(fHandle);
		fRepair = kTRUE;
		ok = kTRUE;
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] Document tree is empty - can't proceed" << endl;
		gMrbLog->Flush(this->ClassName(), "CleanAndRepair");
		ok = kFALSE;
	}
	return(ok);
}

Bool_t TMrbTidyDoc::RunDiagnostics() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::RunDiagnostics
// Purpose:        Perform tidy's diagnostic step
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Runs diagnostics via tidyRunDiagnostics().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;
	if (this->IsRepaired()) {
		if (this->HasNodes()) {
			tidyRunDiagnostics(fHandle);
			ok = kTRUE;
		} else {
			gMrbLog->Err() << "[" << this->GetName() << "] Document tree is empty - can't proceed" << endl;
			gMrbLog->Flush(this->ClassName(), "RunDiagnostics");
			ok = kFALSE;
		}
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't perform diagnostics step - call method \"CleanAndRepair()\" first" << endl;
		gMrbLog->Flush(this->ClassName(), "RunDiagnostics");
		ok = kFALSE;
	}
	return(ok);
}

Bool_t TMrbTidyDoc::AddToList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::AddToList
// Purpose:        Add to list of documents
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds this tidy object to list of documents
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLofTidyDocs == NULL) {
		gMrbLofTidyDocs = new TMrbLofNamedX();
		gMrbLofTidyDocs->Delete();
		gMrbLofTidyDocs->SetName("List of TIDY documents");
	}

	if (gMrbLofTidyDocs->FindByName(this->GetName())) {
		gMrbLog->Err() << "[" << this->GetName() << "] Document already in list - ignored" << endl;
		gMrbLog->Flush(this->ClassName(), "AddToList");
		return(kFALSE);
	}

	Int_t n = gMrbLofTidyDocs->GetEntriesFast();
	gMrbLofTidyDocs->AddNamedX(n, this->GetName(), fDocFile.Data(), this);
	return(kTRUE);
}

const Char_t * TMrbTidyDoc::GetDocName(Bool_t LongFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::GetDocName
// Purpose:        Return name
// Arguments:      Bool_t LongFlag    -- long output
// Results:        Char_t * DocName   -- name
// Exceptions:
// Description:    Runs diagnostics via tidyRunDiagnostics().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString docName;
	if (LongFlag && fDocFile.Length() > 0) {
		TMrbSystem ux;
		ux.GetBaseName(docName, fDocFile.Data());
		docName.Prepend(":");
		docName.Prepend(this->GetName());
	} else {
		docName = this->GetName();
	}
	return(docName.Data());
}

const Char_t * TMrbTidyNode::GetText(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::GetText()
// Purpose:        Get text from node
// Arguments:      TString & Buffer  -- where to store text
// Results:        Char_t * BufPtr   -- ptr to buffer data
// Exceptions:
// Description:    Reads text portion from node.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Buffer.Resize(0);			// cleanup
	Bool_t hasText = (this->IsText() || this->HasText());
	if (hasText) {		// is there any text in this node?
		TidyBuffer buf = {'\0'};
		tidyNodeGetText(fTidyDoc->GetHandle(), fHandle, &buf);	// fetch it
		Buffer = buf.bp;		// fill buffer
		if (fTidyDoc->TextToBeStripped() && (Buffer(0) == '\n') && (Buffer(Buffer.Length() - 1) == '\n')) {
			Buffer.Resize(Buffer.Length() - 1);
			Buffer = Buffer(1, Buffer.Length() - 1);
		}
	}
	return(hasText ? Buffer.Data() : NULL);
}

Bool_t TMrbTidyNode::HasTextChildsOnly() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::HasTextChildsOnly()
// Purpose:        Check if node has only text in it
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks node childs to be text nodes only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->HasChilds()) {
		TMrbTidyNode * child = (TMrbTidyNode *) fLofChilds.First();
		while (child) {
			if (!child->IsText()) return(kFALSE);
			child = (TMrbTidyNode *) fLofChilds.After(child);
		}
		return(kTRUE);
	}
	return(kFALSE);
}

const Char_t *  TMrbTidyNode::CollectTextFromChilds(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CollectTextFromChilds()
// Purpose:        Collect text from childs
// Arguments:      TString & Buffer   -- where to store text
// Results:        Char_t * BufPtr    -- ptr to buffer data
// Exceptions:
// Description:    Collect text from childs if it is a pure text node.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Buffer.Resize(0);
	Bool_t isPureText = this->HasTextChildsOnly();
	if (isPureText) {
		TString text;
		TMrbTidyNode * child = (TMrbTidyNode *) fLofChilds.First();
		while (child) {
			Buffer += child->GetText(text);
			child = (TMrbTidyNode *) fLofChilds.After(child);
		}
	}
	return(isPureText ? Buffer.Data() : NULL);
}

Int_t TMrbTidyNode::CollectSubstUsedByChilds(TMrbLofNamedX & LofSubst, Bool_t TextChildsOnly) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CollectSubstUsedByChilds()
// Purpose:        Collect substitutions from childs
// Arguments:      TMrbLofNamedX & LofSubst   -- list of substitutions
// Results:        Int_t NofSubst             -- number of subst in list
// Exceptions:
// Description:    Collects substitutions used by childs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	LofSubst.Delete();
	TMrbTidyNode * child = (TMrbTidyNode *) fLofChilds.First();
	while (child) {
		if (!TextChildsOnly || child->IsText()) {
			TMrbNamedX * nx = (TMrbNamedX *) child->GetLofSubstitutions()->First();
			while (nx) {
				if (nx->GetIndex() & kMrbTidySubstInUse) LofSubst.AddNamedX(nx);
				nx = (TMrbNamedX *) child->GetLofSubstitutions()->After(nx);
			}
		}
		child = (TMrbTidyNode *) fLofChilds.After(child);
	}
	return(LofSubst.GetEntriesFast());
}

void TMrbTidyNode::FillTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::FillTree()
// Purpose:        Fill document tree
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Fills document tree recursively.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofChilds.Delete();
	for (TidyNode child = tidyGetChild(fHandle); child; child = tidyGetNext(child)) {
		const Char_t * nName = tidyNodeGetName(child);
		TidyNodeType ty = tidyNodeGetType(child);
		TString nodeName;
		if (nName == NULL) {
			if (ty == TidyNode_Text)			nodeName = "text";
			else if (ty == TidyNode_Comment)	nodeName = "comment";
			else nodeName = "n.a";
		} else {
			nodeName = nName;
		}
		TMrbTidyNode * node = new TMrbTidyNode(tidyNodeGetId(child), nodeName, this, child, this->GetTidyDoc());
		node->SetType(ty);
		node->CheckEndTag();
		if (node->GetIndex() == TidyTag_MNODE_MI) {
			TMrbTidyNode * iNode = node->ImplantTreeFromFile();
			if (iNode) {
				node->AddChild(iNode);
				iNode->FillTree();
			}
		} else {
			node->FillTree();
		}
		this->AddChild(node);
	}
}

TMrbTidyNode * TMrbTidyNode::ImplantTreeFromFile(const Char_t * File, const Char_t * Mtag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::IncludeExternalNode()
// Purpose:        Insert node from external file
// Arguments:      Char_t * File       -- name of html file
//                 Char_t * Mtag       -- marabou tag
// Results:        TMrbTidyNode * Node -- root node of implanted tree
// Exceptions:
// Description:    Opens a html file and searches for a node with given marabou tag.
//                 Implants tree starting with this node into local tree.
//                 If File and/or Mtag args are missing value will be taken
//                 from mfile= and mtag= attributes, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofChilds.Delete();
	if (tidyGetChild(fHandle) > 0) {
		gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] No childs allowed for <mi> nodes" << endl;
		gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
		this->TraceBack(cerr, 3);
		return(NULL);
	}

	TString fileStr;
	if (File == NULL || *File == '\0') {
		TMrbTidyAttr * mfile = (TMrbTidyAttr *) fLofAttr.FindByName("mfile");
		if (mfile == NULL) {
			gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Tag missing in <mi> node - \"mfile=...\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
			this->TraceBack(cerr, 3);
			return(NULL);
		}
		fileStr = mfile->GetValue();
		fileStr = fileStr.Strip(TString::kBoth);
		if (fileStr.IsNull()) {
			gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Empty tag in <mi> node - \"mfile=...\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
			this->TraceBack(cerr, 3);
			return(NULL);
		}
	} else {
		fileStr = File;
	}

	TString tagStr;
	if (Mtag == NULL || *Mtag == '\0') {
		TMrbTidyAttr * mtag = (TMrbTidyAttr *) fLofAttr.FindByName("mtag");
		if (mtag == NULL) {
			gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Tag missing in <mi> node - \"mtag=...\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
			this->TraceBack(cerr, 3);
			return(NULL);
		}
		tagStr = mtag->GetValue();
		tagStr = tagStr.Strip(TString::kBoth);
		if (tagStr.IsNull()) {
			gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Empty tag in <mi> node - \"mtag=...\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
			this->TraceBack(cerr, 3);
			return(NULL);
		}
	} else {
		tagStr = Mtag;
	}

	TMrbSystem ux;
	TString iName;
	ux.GetBaseName(iName, fileStr.Data());
	TMrbNamedX * nx = (TMrbNamedX *) gMrbLofTidyDocs->FindByName(iName.Data());
	TMrbTidyDoc * iDoc;
	if (nx) {
		iDoc = (TMrbTidyDoc *) nx->GetAssignedObject();
	} else {
		iDoc = new TMrbTidyDoc(iName.Data(), fileStr.Data(), kFALSE, this->GetTidyDoc()->GetCfgFile());
	}
	TMrbTidyNode * node = this->ScanTidyTree(iDoc->GetRoot()->GetHandle(), "mtag", tagStr.Data(), iDoc);	// low level scan method
	if (node == NULL) {
		gMrbLog->Err() << "[" << fileStr << "] No such attr - mtag=\"" << tagStr << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "ImplantTreeFromFile");
		this->TraceBack(cerr, 3);
	}
	return(node);
}

TMrbTidyNode * TMrbTidyNode::ScanTidyTree(TidyNode Node, const Char_t * AttrName, const Char_t * AttrString, TMrbTidyDoc * Doc) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::ScanTidyTree()
// Purpose:        Scan original tidy tree for a given attribute
// Arguments:      TidyNode Node       -- tidy node to start with
//                 Char_t * AttrName   -- name of attribute
//                 Char_t * AttrString -- attr value
//                 TMrbTidyDoc * Doc   -- document
// Results:        TMrbTidyNode * Node -- node with given attr
// Exceptions:
// Description:    Low-level method to scan original tidy tree.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyDoc * doc = Doc ? Doc : this->GetTidyDoc();
	TMrbTidyNode * node;
	for (TidyNode child = tidyGetChild(Node); child; child = tidyGetNext(child)) {
		TidyAttr attrHandle;
		for (attrHandle = tidyAttrFirst(child); attrHandle; attrHandle = tidyAttrNext(attrHandle)) {
			TString aName = tidyAttrName(attrHandle);
			TString aValue = tidyAttrValue(attrHandle);	// get value
			if (aName.CompareTo(AttrName) == 0 && aValue.CompareTo(AttrString) == 0) {
				node = new TMrbTidyNode(tidyNodeGetId(child), tidyNodeGetName(child), this, child, doc);
				node->SetType(tidyNodeGetType(child));
				node->CheckEndTag();
				return(node);
			}
		}
		node = this->ScanTidyTree(child, AttrName, AttrString, doc);
		if (node) return(node);
	}
	return(NULL);
}

void TMrbTidyNode::DeleteTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::DeleteTree()
// Purpose:        Delete document tree
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Removes any nodes from document tree.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyNode * node = (TMrbTidyNode *) fLofChilds.First();
	while (node) {
		node->DeleteTree();
		node = (TMrbTidyNode *) fLofChilds.After(node);
	}
	fLofChilds.Delete();
}

Int_t TMrbTidyNode::ReadAttr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::ReadAttr
// Purpose:        Read tidy attributes
// Arguments:      --
// Results:        Int_t NofAttr   -- number of attributes read
// Exceptions:
// Description:    Reads node attributes via tidyAttrFirst()/Next()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofAttr.Delete();				// clear attr list
	TidyAttr attrHandle;
	for (attrHandle = tidyAttrFirst(fHandle); attrHandle; attrHandle = tidyAttrNext(attrHandle)) {
		TMrbTidyAttr * attr = new TMrbTidyAttr(tidyAttrGetId(attrHandle), tidyAttrName(attrHandle), attrHandle, this);
		fLofAttr.Add(attr);				// add to list
		attr->SetValue(tidyAttrValue(attrHandle));	// get value
  	}
	return(fLofAttr.GetEntriesFast());
}

Bool_t TMrbTidyNode::CheckMnode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CheckMnode
// Purpose:        Check if special marabou node
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if node is one of special marabou nodes (<mh>, <mb>, ...)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetIndex() == TidyTag_UNKNOWN) {
		TMrbNamedX * nx = this->GetTidyDoc()->GetLofMnodes()->FindByName(this->GetName());
		if (nx == NULL) {
			fIsMnode = kFALSE;
		} else {
			fIsMnode = kTRUE;
			this->ChangeIndex(nx->GetIndex());
			this->GetTidyDoc()->SetMnodeFlag();
		}
	} else {
		fIsMnode = kFALSE;
	}
	return(fIsMnode);
}
	
Bool_t TMrbTidyNode::CheckEndTag() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CheckEndTag
// Purpose:        Check if node has end tag
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if node needs to have an end tag
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetType() == TidyNode_StartEnd) {
		fHasEndTag = kFALSE;
	} else if (this->IsText()) {
		fHasEndTag = kFALSE;
	} else if (this->IsComment()) {
		fHasEndTag = kFALSE;
	} else if (this->IsMETA()) {
		fHasEndTag = kFALSE;
	} else if (this->IsLINK()) {
		fHasEndTag = kFALSE;
	} else if (this->IsHR()) {
		fHasEndTag = kFALSE;
	} else {
		fHasEndTag = kTRUE;
	}
	return(fHasEndTag);
}
	
void TMrbTidyOption::Print(ostream & Out, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyOption::Print
// Purpose:        Print tidy option
// Arguments:      ostream & Out   -- output stream
//                 Bool_t Verbose  -- verbose
// Results:        --
// Exceptions:
// Description:    Prints tidy option.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Verbose)	Out << this->GetName() << ": id=" << this->GetIndex();
	else			Out << setw(3) << this->GetIndex() << setw(30) << this->GetName();

	switch (fType) {
		case TidyString:
			{
				TString optVal, optDef;
				this->GetDefault(optDef);
				if (this->GetValue(optVal)) {
					if (Verbose)	Out << " value=" << optVal << " default=" << optDef << endl;
					else			Out << setw(10) << optVal << setw(10) << optDef << endl;
				}
			}
			break;
		case TidyInteger:
			{
				Int_t optVal, optDef;
				this->GetDefault(optDef);
				if (this->GetValue(optVal)) {
					if (Verbose)	Out << " value=" << optVal << " default=" << optDef << endl;
					else			Out << setw(10) << optVal << setw(10) << optDef << endl;
				}
			}
			break;
		case TidyBoolean:
			{
				Bool_t optVal, optDef;
				this->GetDefault(optDef);
				if (this->GetValue(optVal)) {
					if (Verbose)	Out << " value=" << (optVal ? "TRUE" : "FALSE")
										<< " default=" << (optDef ? "TRUE" : "FALSE") << endl;
					else			Out << setw(10) << (optVal ? "TRUE" : "FALSE")
										<< setw(10) << (optDef ? "TRUE" : "FALSE") << endl;
				}
			}
			break;
	}
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyOption::GetValue()/SetValue()
// Purpose:        Get/set tidy options
// Description:    Interface to tidyOptGetXXX()/tidyOptSetXXX()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbTidyOption::GetDefault(TString & String) {
	if (fType == TidyString) String = tidyOptGetDefault(fHandle);
	return(fType == TidyString);
};

Bool_t TMrbTidyOption::GetDefault(Int_t & Value) {
	if (fType == TidyInteger) Value = tidyOptGetDefaultInt(fHandle);
	return(fType == TidyInteger);
};

Bool_t TMrbTidyOption::GetDefault(Bool_t & Flag) {
	if (fType == TidyBoolean) Flag = tidyOptGetDefaultBool(fHandle);
	return(fType == TidyBoolean);
};

Bool_t TMrbTidyOption::GetValue(TString & String) {
	if (fType == TidyString) String = tidyOptGetValue(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex());
	return(fType == TidyString);
};

Bool_t TMrbTidyOption::GetValue(Int_t & Value) {
	if (fType == TidyInteger) Value = tidyOptGetInt(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex());
	return(fType == TidyInteger);
};

Bool_t TMrbTidyOption::GetValue(Bool_t & Flag) {
	if (fType == TidyBoolean) Flag = tidyOptGetBool(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex());
	return(fType == TidyBoolean);
};

Bool_t TMrbTidyOption::SetValue(Char_t * String) {
	if (fType == TidyString) tidyOptSetValue(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex(), String);
	return(fType == TidyString);
};

Bool_t TMrbTidyOption::SetValue(Int_t Value) {
	if (fType == TidyInteger) tidyOptSetInt(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex(), Value);
	return(fType == TidyInteger);
};

Bool_t TMrbTidyOption::SetValue(Bool_t Flag) {
	if (fType == TidyBoolean) tidyOptSetBool(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex(), Flag);
	return(fType == TidyBoolean);
};

void TMrbTidyOption::Reset()  { tidyOptResetToDefault(fTidyDoc->GetHandle(), (TidyOptionId) this->GetIndex()); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::IsXXX()
// Purpose:        Query node type
// Description:    Interface to tidyNodeIsXXX()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbTidyNode::IsText() { return(tidyNodeIsText(fHandle)); };
Bool_t TMrbTidyNode::IsProp() { return(tidyNodeIsProp(fTidyDoc->GetHandle(), fHandle)); };
Bool_t TMrbTidyNode::IsHeader() { return(tidyNodeIsHeader(fHandle)); };
Bool_t TMrbTidyNode::HasText() { return(tidyNodeHasText(fTidyDoc->GetHandle(), fHandle)); };
Bool_t TMrbTidyNode::IsHTML() { return(tidyNodeIsHTML(fHandle)); };
Bool_t TMrbTidyNode::IsHEAD() { return(tidyNodeIsHEAD(fHandle)); };
Bool_t TMrbTidyNode::IsTITLE() { return(tidyNodeIsTITLE(fHandle)); };
Bool_t TMrbTidyNode::IsBASE() { return(tidyNodeIsBASE(fHandle)); };
Bool_t TMrbTidyNode::IsMETA() { return(tidyNodeIsMETA(fHandle)); };
Bool_t TMrbTidyNode::IsBODY() { return(tidyNodeIsBODY(fHandle)); };
Bool_t TMrbTidyNode::IsFRAMESET() { return(tidyNodeIsFRAMESET(fHandle)); };
Bool_t TMrbTidyNode::IsFRAME() { return(tidyNodeIsFRAME(fHandle)); };
Bool_t TMrbTidyNode::IsIFRAME() { return(tidyNodeIsIFRAME(fHandle)); };
Bool_t TMrbTidyNode::IsNOFRAMES() { return(tidyNodeIsNOFRAMES(fHandle)); };
Bool_t TMrbTidyNode::IsHR() { return(tidyNodeIsHR(fHandle)); };
Bool_t TMrbTidyNode::IsH1() { return(tidyNodeIsH1(fHandle)); };
Bool_t TMrbTidyNode::IsH2() { return(tidyNodeIsH2(fHandle)); };
Bool_t TMrbTidyNode::IsPRE() { return(tidyNodeIsPRE(fHandle)); };
Bool_t TMrbTidyNode::IsLISTING() { return(tidyNodeIsLISTING(fHandle)); };
Bool_t TMrbTidyNode::IsP() { return(tidyNodeIsP(fHandle)); };
Bool_t TMrbTidyNode::IsUL() { return(tidyNodeIsUL(fHandle)); };
Bool_t TMrbTidyNode::IsOL() { return(tidyNodeIsOL(fHandle)); };
Bool_t TMrbTidyNode::IsDL() { return(tidyNodeIsDL(fHandle)); };
Bool_t TMrbTidyNode::IsDIR() { return(tidyNodeIsDIR(fHandle)); };
Bool_t TMrbTidyNode::IsLI() { return(tidyNodeIsLI(fHandle)); };
Bool_t TMrbTidyNode::IsDT() { return(tidyNodeIsDT(fHandle)); };
Bool_t TMrbTidyNode::IsDD() { return(tidyNodeIsDD(fHandle)); };
Bool_t TMrbTidyNode::IsTABLE() { return(tidyNodeIsTABLE(fHandle)); };
Bool_t TMrbTidyNode::IsCAPTION() { return(tidyNodeIsCAPTION(fHandle)); };
Bool_t TMrbTidyNode::IsTD() { return(tidyNodeIsTD(fHandle)); };
Bool_t TMrbTidyNode::IsTH() { return(tidyNodeIsTH(fHandle)); };
Bool_t TMrbTidyNode::IsTR() { return(tidyNodeIsTR(fHandle)); };
Bool_t TMrbTidyNode::IsCOL() { return(tidyNodeIsCOL(fHandle)); };
Bool_t TMrbTidyNode::IsCOLGROUP() { return(tidyNodeIsCOLGROUP(fHandle)); };
Bool_t TMrbTidyNode::IsBR() { return(tidyNodeIsBR(fHandle)); };
Bool_t TMrbTidyNode::IsAA() { return(tidyNodeIsA(fHandle)); };
Bool_t TMrbTidyNode::IsLINK() { return(tidyNodeIsLINK(fHandle)); };
Bool_t TMrbTidyNode::IsB() { return(tidyNodeIsB(fHandle)); };
Bool_t TMrbTidyNode::IsI() { return(tidyNodeIsI(fHandle)); };
Bool_t TMrbTidyNode::IsSTRONG() { return(tidyNodeIsSTRONG(fHandle)); };
Bool_t TMrbTidyNode::IsEM() { return(tidyNodeIsEM(fHandle)); };
Bool_t TMrbTidyNode::IsBIG() { return(tidyNodeIsBIG(fHandle)); };
Bool_t TMrbTidyNode::IsSMALL() { return(tidyNodeIsSMALL(fHandle)); };
Bool_t TMrbTidyNode::IsPARAM() { return(tidyNodeIsPARAM(fHandle)); };
Bool_t TMrbTidyNode::IsOPTION() { return(tidyNodeIsOPTION(fHandle)); };
Bool_t TMrbTidyNode::IsOPTGROUP() { return(tidyNodeIsOPTGROUP(fHandle)); };
Bool_t TMrbTidyNode::IsIMG() { return(tidyNodeIsIMG(fHandle)); };
Bool_t TMrbTidyNode::IsMAP() { return(tidyNodeIsMAP(fHandle)); };
Bool_t TMrbTidyNode::IsAREA() { return(tidyNodeIsAREA(fHandle)); };
Bool_t TMrbTidyNode::IsNOBR() { return(tidyNodeIsNOBR(fHandle)); };
Bool_t TMrbTidyNode::IsWBR() { return(tidyNodeIsWBR(fHandle)); };
Bool_t TMrbTidyNode::IsFONT() { return(tidyNodeIsFONT(fHandle)); };
Bool_t TMrbTidyNode::IsLAYER() { return(tidyNodeIsLAYER(fHandle)); };
Bool_t TMrbTidyNode::IsSPACER() { return(tidyNodeIsSPACER(fHandle)); };
Bool_t TMrbTidyNode::IsCENTER() { return(tidyNodeIsCENTER(fHandle)); };
Bool_t TMrbTidyNode::IsSTYLE() { return(tidyNodeIsSTYLE(fHandle)); };
Bool_t TMrbTidyNode::IsSCRIPT() { return(tidyNodeIsSCRIPT(fHandle)); };
Bool_t TMrbTidyNode::IsNOSCRIPT() { return(tidyNodeIsNOSCRIPT(fHandle)); };
Bool_t TMrbTidyNode::IsFORM() { return(tidyNodeIsFORM(fHandle)); };
Bool_t TMrbTidyNode::IsTEXTAREA() { return(tidyNodeIsTEXTAREA(fHandle)); };
Bool_t TMrbTidyNode::IsBLOCKQUOTE() { return(tidyNodeIsBLOCKQUOTE(fHandle)); };
Bool_t TMrbTidyNode::IsAPPLET() { return(tidyNodeIsAPPLET(fHandle)); };
Bool_t TMrbTidyNode::IsOBJECT() { return(tidyNodeIsOBJECT(fHandle)); };
Bool_t TMrbTidyNode::IsDIV() { return(tidyNodeIsDIV(fHandle)); };
Bool_t TMrbTidyNode::IsSPAN() { return(tidyNodeIsSPAN(fHandle)); };
Bool_t TMrbTidyNode::IsINPUT() { return(tidyNodeIsINPUT(fHandle)); };
Bool_t TMrbTidyNode::IsQ() { return(tidyNodeIsQ(fHandle)); };
Bool_t TMrbTidyNode::IsLABEL() { return(tidyNodeIsLABEL(fHandle)); };
Bool_t TMrbTidyNode::IsH3() { return(tidyNodeIsH3(fHandle)); };
Bool_t TMrbTidyNode::IsH4() { return(tidyNodeIsH4(fHandle)); };
Bool_t TMrbTidyNode::IsH5() { return(tidyNodeIsH5(fHandle)); };
Bool_t TMrbTidyNode::IsH6() { return(tidyNodeIsH6(fHandle)); };
Bool_t TMrbTidyNode::IsADDRESS() { return(tidyNodeIsADDRESS(fHandle)); };
Bool_t TMrbTidyNode::IsXMP() { return(tidyNodeIsXMP(fHandle)); };
Bool_t TMrbTidyNode::IsSELECT() { return(tidyNodeIsSELECT(fHandle)); };
Bool_t TMrbTidyNode::IsBLINK() { return(tidyNodeIsBLINK(fHandle)); };
Bool_t TMrbTidyNode::IsMARQUEE() { return(tidyNodeIsMARQUEE(fHandle)); };
Bool_t TMrbTidyNode::IsEMBED() { return(tidyNodeIsEMBED(fHandle)); };
Bool_t TMrbTidyNode::IsBASEFONT() { return(tidyNodeIsBASEFONT(fHandle)); };
Bool_t TMrbTidyNode::IsISINDEX() { return(tidyNodeIsISINDEX(fHandle)); };
Bool_t TMrbTidyNode::IsS() { return(tidyNodeIsS(fHandle)); };
Bool_t TMrbTidyNode::IsSTRIKE() { return(tidyNodeIsSTRIKE(fHandle)); };
Bool_t TMrbTidyNode::IsU() { return(tidyNodeIsU(fHandle)); };
Bool_t TMrbTidyNode::IsMENU() { return(tidyNodeIsMENU(fHandle)); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::GetAttrXXX()
// Purpose:        Query node attributes
// Description:    Interface to tidyAttrGetXXX()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

TMrbTidyAttr * TMrbTidyNode::GetAttrHREF() { return(this->GetAttribute(TidyAttr_HREF)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrSRC() { return(this->GetAttribute(TidyAttr_SRC)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrID() { return(this->GetAttribute(TidyAttr_ID)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrNAME() { return(this->GetAttribute(TidyAttr_NAME)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrSUMMARY() { return(this->GetAttribute(TidyAttr_SUMMARY)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrALT() { return(this->GetAttribute(TidyAttr_ALT)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrLONGDESC() { return(this->GetAttribute(TidyAttr_LONGDESC)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrUSEMAP() { return(this->GetAttribute(TidyAttr_USEMAP)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrISMAP() { return(this->GetAttribute(TidyAttr_ISMAP)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrLANGUAGE() { return(this->GetAttribute(TidyAttr_LANGUAGE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrTYPE() { return(this->GetAttribute(TidyAttr_TYPE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrVALUE() { return(this->GetAttribute(TidyAttr_VALUE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrCONTENT() { return(this->GetAttribute(TidyAttr_CONTENT)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrTITLE() { return(this->GetAttribute(TidyAttr_TITLE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrXMLNS() { return(this->GetAttribute(TidyAttr_XMLNS)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrDATAFLD() { return(this->GetAttribute(TidyAttr_DATAFLD)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrWIDTH() { return(this->GetAttribute(TidyAttr_WIDTH)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrHEIGHT() { return(this->GetAttribute(TidyAttr_HEIGHT)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrFOR() { return(this->GetAttribute(TidyAttr_FOR)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrSELECTED() { return(this->GetAttribute(TidyAttr_SELECTED)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrCHECKED() { return(this->GetAttribute(TidyAttr_CHECKED)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrLANG() { return(this->GetAttribute(TidyAttr_LANG)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrTARGET() { return(this->GetAttribute(TidyAttr_TARGET)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrHTTP_EQUIV() { return(this->GetAttribute(TidyAttr_HTTP_EQUIV)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrREL() { return(this->GetAttribute(TidyAttr_REL)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnMOUSEMOVE() { return(this->GetAttribute(TidyAttr_OnMOUSEMOVE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnMOUSEDOWN() { return(this->GetAttribute(TidyAttr_OnMOUSEDOWN)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnMOUSEUP() { return(this->GetAttribute(TidyAttr_OnMOUSEUP)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnCLICK() { return(this->GetAttribute(TidyAttr_OnCLICK)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnMOUSEOVER() { return(this->GetAttribute(TidyAttr_OnMOUSEOVER)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnMOUSEOUT() { return(this->GetAttribute(TidyAttr_OnMOUSEOUT)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnKEYDOWN() { return(this->GetAttribute(TidyAttr_OnKEYDOWN)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnKEYUP() { return(this->GetAttribute(TidyAttr_OnKEYUP)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnKEYPRESS() { return(this->GetAttribute(TidyAttr_OnKEYPRESS)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnFOCUS() { return(this->GetAttribute(TidyAttr_OnFOCUS)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrOnBLUR() { return(this->GetAttribute(TidyAttr_OnBLUR)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrBGCOLOR() { return(this->GetAttribute(TidyAttr_BGCOLOR)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrLINK() { return(this->GetAttribute(TidyAttr_LINK)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrALINK() { return(this->GetAttribute(TidyAttr_ALINK)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrVLINK() { return(this->GetAttribute(TidyAttr_VLINK)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrTEXT() { return(this->GetAttribute(TidyAttr_TEXT)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrSTYLE() { return(this->GetAttribute(TidyAttr_STYLE)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrABBR() { return(this->GetAttribute(TidyAttr_ABBR)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrCOLSPAN() { return(this->GetAttribute(TidyAttr_COLSPAN)); };
TMrbTidyAttr * TMrbTidyNode::GetAttrROWSPAN() { return(this->GetAttribute(TidyAttr_ROWSPAN)); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyAttr::IsXXX()
// Purpose:        Query attribute type
// Description:    Interface to tidyAttrIsXXX()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbTidyAttr::IsEvent() { return(tidyAttrIsEvent(fHandle)); };
Bool_t TMrbTidyAttr::IsProp() { return(tidyAttrIsProp(fHandle)); };
Bool_t TMrbTidyAttr::IsHREF() { return(tidyAttrIsHREF(fHandle)); };
Bool_t TMrbTidyAttr::IsSRC() { return(tidyAttrIsSRC(fHandle)); };
Bool_t TMrbTidyAttr::IsID() { return(tidyAttrIsID(fHandle)); };
Bool_t TMrbTidyAttr::IsNAME() { return(tidyAttrIsNAME(fHandle)); };
Bool_t TMrbTidyAttr::IsSUMMARY() { return(tidyAttrIsSUMMARY(fHandle)); };
Bool_t TMrbTidyAttr::IsALT() { return(tidyAttrIsALT(fHandle)); };
Bool_t TMrbTidyAttr::IsLONGDESC() { return(tidyAttrIsLONGDESC(fHandle)); };
Bool_t TMrbTidyAttr::IsUSEMAP() { return(tidyAttrIsUSEMAP(fHandle)); };
Bool_t TMrbTidyAttr::IsISMAP() { return(tidyAttrIsISMAP(fHandle)); };
Bool_t TMrbTidyAttr::IsLANGUAGE() { return(tidyAttrIsLANGUAGE(fHandle)); };
Bool_t TMrbTidyAttr::IsTYPE() { return(tidyAttrIsTYPE(fHandle)); };
Bool_t TMrbTidyAttr::IsVALUE() { return(tidyAttrIsVALUE(fHandle)); };
Bool_t TMrbTidyAttr::IsCONTENT() { return(tidyAttrIsCONTENT(fHandle)); };
Bool_t TMrbTidyAttr::IsTITLE() { return(tidyAttrIsTITLE(fHandle)); };
Bool_t TMrbTidyAttr::IsXMLNS() { return(tidyAttrIsXMLNS(fHandle)); };
Bool_t TMrbTidyAttr::IsDATAFLD() { return(tidyAttrIsDATAFLD(fHandle)); };
Bool_t TMrbTidyAttr::IsWIDTH() { return(tidyAttrIsWIDTH(fHandle)); };
Bool_t TMrbTidyAttr::IsHEIGHT() { return(tidyAttrIsHEIGHT(fHandle)); };
Bool_t TMrbTidyAttr::IsFOR() { return(tidyAttrIsFOR(fHandle)); };
Bool_t TMrbTidyAttr::IsSELECTED() { return(tidyAttrIsSELECTED(fHandle)); };
Bool_t TMrbTidyAttr::IsCHECKED() { return(tidyAttrIsCHECKED(fHandle)); };
Bool_t TMrbTidyAttr::IsLANG() { return(tidyAttrIsLANG(fHandle)); };
Bool_t TMrbTidyAttr::IsTARGET() { return(tidyAttrIsTARGET(fHandle)); };
Bool_t TMrbTidyAttr::IsHTTP_EQUIV() { return(tidyAttrIsHTTP_EQUIV(fHandle)); };
Bool_t TMrbTidyAttr::IsREL() { return(tidyAttrIsREL(fHandle)); };
Bool_t TMrbTidyAttr::IsOnMOUSEMOVE() { return(tidyAttrIsOnMOUSEMOVE(fHandle)); };
Bool_t TMrbTidyAttr::IsOnMOUSEDOWN() { return(tidyAttrIsOnMOUSEDOWN(fHandle)); };
Bool_t TMrbTidyAttr::IsOnMOUSEUP() { return(tidyAttrIsOnMOUSEUP(fHandle)); };
Bool_t TMrbTidyAttr::IsOnCLICK() { return(tidyAttrIsOnCLICK(fHandle)); };
Bool_t TMrbTidyAttr::IsOnMOUSEOVER() { return(tidyAttrIsOnMOUSEOVER(fHandle)); };
Bool_t TMrbTidyAttr::IsOnMOUSEOUT() { return(tidyAttrIsOnMOUSEOUT(fHandle)); };
Bool_t TMrbTidyAttr::IsOnKEYDOWN() { return(tidyAttrIsOnKEYDOWN(fHandle)); };
Bool_t TMrbTidyAttr::IsOnKEYUP() { return(tidyAttrIsOnKEYUP(fHandle)); };
Bool_t TMrbTidyAttr::IsOnKEYPRESS() { return(tidyAttrIsOnKEYPRESS(fHandle)); };
Bool_t TMrbTidyAttr::IsOnFOCUS() { return(tidyAttrIsOnFOCUS(fHandle)); };
Bool_t TMrbTidyAttr::IsOnBLUR() { return(tidyAttrIsOnBLUR(fHandle)); };
Bool_t TMrbTidyAttr::IsBGCOLOR() { return(tidyAttrIsBGCOLOR(fHandle)); };
Bool_t TMrbTidyAttr::IsLINK() { return(tidyAttrIsLINK(fHandle)); };
Bool_t TMrbTidyAttr::IsALINK() { return(tidyAttrIsALINK(fHandle)); };
Bool_t TMrbTidyAttr::IsVLINK() { return(tidyAttrIsVLINK(fHandle)); };
Bool_t TMrbTidyAttr::IsTEXT() { return(tidyAttrIsTEXT(fHandle)); };
Bool_t TMrbTidyAttr::IsSTYLE() { return(tidyAttrIsSTYLE(fHandle)); };
Bool_t TMrbTidyAttr::IsABBR() { return(tidyAttrIsABBR(fHandle)); };
Bool_t TMrbTidyAttr::IsCOLSPAN() { return(tidyAttrIsCOLSPAN(fHandle)); };
Bool_t TMrbTidyAttr::IsROWSPAN() { return(tidyAttrIsROWSPAN(fHandle)); };

void TMrbTidyDoc::Print(const Char_t * File, Bool_t Verbose, Bool_t HtmlFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::Print
// Purpose:        Print data
// Arguments:      Char_t * File    -- file name
//                 Bool_t Verbose   -- kTRUE: print full text strings
//                 Bool_t HtmlFlag  -- kTRUE: output html format
// Results:        --
// Exceptions:
// Description:    Prints document data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ofstream of(File, ios::out);
	if (!of.good()) {
		gMrbLog->Err() << "[" << this->GetName() << "] " << gSystem->GetError() << " - " << File << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
	} else {
		this->Print(of, Verbose, HtmlFlag);
		of.close();
	}
}

void TMrbTidyDoc::Print(ostream & Out, Bool_t Verbose, Bool_t HtmlFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
//                 Bool_t Verbose   -- kTRUE: print full text strings
//                 Bool_t HtmlFlag  -- kTRUE: output html format
// Results:        --
// Exceptions:
// Description:    Prints document data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->HasNodes()) {
		if (HtmlFlag) {
			Out << "<html>" << endl;
			Out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"mrb-style.css\">" << endl;
			Out << "<body bgcolor=\"beige\">" << endl;
			Out 	<< "<H2 align=\"center\">Document " << this->GetName() << "</H2>" << endl;
			if (fDocFile.Length()) Out 	<< "<H4 align=\"center\">File: " << fDocFile << "</H4>" << endl;
			Out 	<< "<H4 align=\"center\">(structure as analyzed by D. Raggett's TIDY)</H4><HR>" << endl;
		} else {
			Out 	<< endl << "Document " << this->GetName();
			if (fDocFile.Length()) Out 	<< " (file " << fDocFile << ")";
			Out 	<< ": structure as analyzed by D. Raggett's TIDY" << endl
					<< "----------------------------------------------------------------------------------------------" << endl;
		}
		fTidyRoot->PrintTree(Out, Verbose, HtmlFlag);
		if (HtmlFlag) Out << "</body></html>" << endl;
	}
}

Bool_t TMrbTidyDoc::OutputHtml(const Char_t * HtmlFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::OutputHtml
// Purpose:        Output HTML code
// Arguments:      Char_t * HtmlFile    -- output file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs html code to file
//                 including special code for marabou nodes
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;
	if (this->HasNodes()) {
		TString htmlFile = HtmlFile;
		gSystem->ExpandPathName(htmlFile);
		if (this->HasMnodes()) {
			ofstream of(htmlFile.Data(), ios::out);
			if (!of.good()) {
				gMrbLog->Err() << "[" << this->GetName() << "] " << gSystem->GetError() << " - " << HtmlFile << endl;
				gMrbLog->Flush(this->ClassName(), "OutputHtml");
				ok = kFALSE;
			} else {
				fTidyRoot->OutputHtmlTree(of);
				of.close();
				ok = kTRUE;
			}
		} else {
			tidySaveFile(fHandle, (Char_t *) htmlFile.Data());				
		}
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] Document tree empty - no html code to process" << endl;
		gMrbLog->Flush(this->ClassName(), "OutputHtml");
		ok = kFALSE;
	}
	return(ok);
}

Bool_t TMrbTidyDoc::OutputHtml(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc::OutputHtml
// Purpose:        Output HTML code
// Arguments:      ostream & Out    -- output stream
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs html code to stream
//                 including special code for marabou nodes
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;
	if (this->HasNodes()) {
		if (this->HasMnodes()) {
			fTidyRoot->OutputHtmlTree(Out);
			ok = kTRUE;
		} else {
			TidyBuffer tbuf = {0};
			tidySaveBuffer(fHandle, &tbuf);				
			Out << tbuf.bp << endl;
		}
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] Document tree empty - no html code to process" << endl;
		gMrbLog->Flush(this->ClassName(), "OutputHtml");
		ok = kFALSE;
	}
	return(ok);
}

void TMrbTidyNode::PrintTree(ostream & Out, Bool_t Verbose, Bool_t HtmlFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::PrintTree
// Purpose:        Print data recursively
// Arguments:      ostream & Out    -- output stream
//                 Bool_t Verbose   -- kTRUE: print full text strings
//                 Bool_t HtmlFlag  -- kTRUE: output html format
// Results:        --
// Exceptions:
// Description:    Prints node data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->Print(Out, Verbose, HtmlFlag);
	if (this->HasChilds() && HtmlFlag) Out << "<UL>" << endl;
	TMrbTidyNode * node = (TMrbTidyNode *) fLofChilds.First();
	while (node) {
		node->PrintTree(Out, Verbose, HtmlFlag);
		node = (TMrbTidyNode *) fLofChilds.After(node);
	}
	if (this->HasChilds() && HtmlFlag) Out << "</UL>" << endl;
}

Int_t TMrbTidyNode::StepTree(TObjArray & LofNodes) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::StepTree
// Purpose:        Step thru tree and collect nodes
// Arguments:      TObjArray & LofNodes  -- where to store nodes
// Results:        Int_t NofNodes        -- number of nodes
// Exceptions:
// Description:    Steps thru node tree and stores nodes in array
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyNode * node = (TMrbTidyNode *) fLofChilds.First();
	Int_t nofNodes = 0;
	while (node) {
		LofNodes.Add(node);
		nofNodes++;
		nofNodes += node->StepTree(LofNodes);
		node = (TMrbTidyNode *) fLofChilds.After(node);
	}
	return(nofNodes);
}

void TMrbTidyNode::Print(ostream & Out, Bool_t Verbose, Bool_t HtmlFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Print
// Purpose:        Print data
// Arguments:      ostream & Out    -- output stream
//                 Bool_t Verbose   -- kTRUE: print full text strings
//                 Bool_t HtmlFlag  -- kTRUE: output html format
// Results:        --
// Exceptions:
// Description:    Prints node data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX lofNodeTypes;
	lofNodeTypes.AddNamedX(kMrbTidyNodeTypes);
	TMrbLofNamedX lofTagIds;
	lofTagIds.AddNamedX(kMrbTidyTagIds);

	TString liTag;
	if (HtmlFlag) {
		liTag = "<LI>";
	} else {
		liTag = "";
		Out << "[" << setw(2) << fTreeLevel << "] ";
		for (Int_t lev = 0; lev < fTreeLevel; lev++) Out << ".";
		if (fTreeLevel > 0) Out << " ";
	}
	TString parentName = fParent ? fParent->GetName() : "root";
	TMrbNamedX * ty = (TMrbNamedX *) lofNodeTypes.FindByIndex((Int_t) this->GetType());
	TString tyStr;
	if (ty) tyStr = Form("%s(%d)", ty->GetName(), ty->GetIndex()); else tyStr = this->GetType();
	TMrbNamedX * tag = (TMrbNamedX *) lofTagIds.FindByIndex(this->GetIndex());
	TString tagStr;
	if (tag) {
		tagStr = Form("%s(%d)", tag->GetName(), tag->GetIndex());
	} else if (this->IsMnode()) {
		tagStr = Form("MNODE(%d)", this->GetIndex());
	} else {
		tagStr = this->GetIndex();
	}
	if (HtmlFlag) {
		Out << "<em>" << this->GetName() << "</em>";
		Out << "<UL>";
	} else {
		Out << "<" << this->GetName() << ">";
	}
	Out << liTag << " type=" << tyStr.Data();
	if (this->GetIndex() != TidyTag_UNKNOWN) Out << liTag << " tag=" << tagStr.Data();
	if (!HtmlFlag) Out << " parent=" << parentName.Data();
	if (this->IsMnode()) Out << liTag << " indent=" << fIndentLevel;
	TString text;
	if (this->GetText(text)) {
		if (text.Length() == 0) {
			Out << liTag << " text=<empty>";
		} else if (Verbose) {
			text.ReplaceAll("\n", "<cr>");
			Out << liTag << " text='" << text << "'";
		} else {
			text = text(0, 10);
			text.ReplaceAll("\n", "<cr>");
			Out << liTag << " text='" << text << "...'";
		}
	}
	TMrbTidyAttr * a = (TMrbTidyAttr *) fLofAttr.First();
	while (a) {
		TString aValue = a->GetValue();
		TString aQuote = (aValue.Index("\"", 0) >= 0) ? "'" : "\"";
		TString aName = a->GetName();
		if (Verbose || aName.CompareTo("string") != 0) {
			Out << liTag << " " << a->GetName() << "=" << aQuote << aValue << aQuote;
		} else {
			aValue = aValue(0, 10);
			Out << liTag << " " << a->GetName() << "=" << aQuote << aValue << "..." << aQuote;
		}
		a = (TMrbTidyAttr *) fLofAttr.After(a);
	}
	if (HtmlFlag) Out << "</UL>";
	Out << endl;
}

void TMrbTidyNode::TraceBack(ostream & Out, Int_t NofLevels) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::TraceBack
// Purpose:        Output backtrace
// Arguments:      ostream & Out    -- output stream
//                 Int_t NofLevels  -- number of levels
// Results:        --
// Exceptions:
// Description:    Prints backtrace data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << "At node " << ends; this->Print(Out, kTRUE);
	TMrbTidyNode * parent = this->GetParent();
	TString indent = "";
	if (NofLevels == 0) NofLevels = 1000000;
	for (Int_t l = 0; l < NofLevels; l++) {
		indent += "  ";
		Out << indent << "child of " << ends; parent->Print(Out, kTRUE);
		parent = parent->GetParent();
		if (parent == NULL) return;
	}		
}

void TMrbTidyNode::OutputHtmlTree(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlTree
// Purpose:        Output html data recursively
// Arguments:      ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs node data in html format
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t popUp = kFALSE;
	if (!this->IsRoot()) popUp = this->OutputHtml(Out);
	if (!popUp) {
		TMrbTidyNode * node = (TMrbTidyNode *) fLofChilds.First();
		while (node) {
			node->OutputHtmlTree(Out);
			node = (TMrbTidyNode *) fLofChilds.After(node);
		}
	}
	if (!this->IsRoot() && !this->IsMnode() && this->HasEndTag()) Out << "</" << this->GetName() << ">" << endl;
}

Bool_t TMrbTidyNode::OutputHtml(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtml
// Purpose:        Output html data
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data.
//                 Special treatment for marabou nodes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t popUp = kFALSE;

	TMrbLofNamedX lofNodeTypes;
	lofNodeTypes.AddNamedX(kMrbTidyNodeTypes);
	TMrbLofNamedX lofTagIds;
	lofTagIds.AddNamedX(kMrbTidyTagIds);

	if (this->IsComment()) {
		TString text;
		Out << this->GetText(text) << endl;
	} else if (this->IsText()) {
		TString text;
		this->GetText(text);
		if (fParent->IsMnode()) {
			TString t = text.Strip(TString::kBoth);
			if (t.IsNull()) {
				Out << "<br>" << endl;
			} else {
				for (Int_t i = 0; i < fIndentLevel; i++) Out << tabEquiv;
				Out << "<pre class=\"code\">" << this->PrepareForHtmlOutput(text) << "</pre><br>" << endl;
			}
		} else {
			Out << text << endl;
		}
	} else if (this->IsMnode()) {
		popUp = this->OutputHtmlForMnodes(Out);
	} else {
		Out << "<" << this->GetName();
		TMrbTidyAttr * a = (TMrbTidyAttr *) fLofAttr.First();
		while (a) {
			TString aValue = a->GetValue();
			TString aQuote = (aValue.Index("\"", 0) >= 0) ? "'" : "\"";
			Out << " " << a->GetName() << "=" << aQuote << a->GetValue() << aQuote;
			a = (TMrbTidyAttr *) fLofAttr.After(a);
		}
		if (this->GetType() == TidyNode_StartEnd) Out << " /";
		Out << ">" << endl;
	}
	return(popUp);
}

Bool_t TMrbTidyNode::OutputHtmlForMnodes(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMnodes
// Purpose:        Special html output for marabou nodes
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for marabou nodes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t popUp = kFALSE;
	if (this->GetIndex() == TidyTag_MNODE_MB) {
		popUp = this->OutputHtmlForMB(Out);
	} else if (this->GetIndex() == TidyTag_MNODE_MH) {
		popUp = this->OutputHtmlForMH(Out);
	} else if (this->GetIndex() == TidyTag_MNODE_MX) {
		popUp = this->OutputHtmlForMX(Out);
	} else if (this->GetIndex() == TidyTag_MNODE_MC) {
		popUp = this->OutputHtmlForMC(Out);
	} else if (this->GetIndex() == TidyTag_MNODE_MI) {
		popUp = this->OutputHtmlForMI(Out);
	} else if (this->GetIndex() == TidyTag_MNODE_MS) {
		popUp = this->OutputHtmlForMS(Out);
	}
	return(popUp);
}

Bool_t TMrbTidyNode::OutputHtmlForMB(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMB
// Purpose:        Special html output for node <mb>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <mb>...</mb> [body]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->ProcessMnodeHeader(Out, "header0", 0);
	return(kFALSE);
}

Bool_t TMrbTidyNode::OutputHtmlForMH(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMH
// Purpose:        Special html output for node <mh>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <mh>...</mh> [header]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->ProcessMnodeHeader(Out, "header1", 0);
	TString hdata;
	if (this->CollectTextFromChilds(hdata)) {
		hdata.ReplaceAll("\t", tabEquiv.Data());
		Out << "<pre class=\"code\">" << this->PrepareForHtmlOutput(hdata) << "</pre><br>" << endl;
	} else {
		Out << "<br>" << endl;
	}
	return(kTRUE);
}

Bool_t TMrbTidyNode::OutputHtmlForMX(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMX
// Purpose:        Special html output for node <mx>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <mx> [expand]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName = this->GetParent()->GetName();
	TString h;
	if (pName.CompareTo("ms") == 0) h = "header3"; else h = "header2";

	this->ProcessMnodeHeader(Out, h.Data(), fTreeLevel - 4);

	this->SetIndentation();

	TMrbTidyAttr * aString = (TMrbTidyAttr *) fLofAttr.FindByName("string");
	if (aString) {
		TString buf = aString->GetValue();
		buf.ReplaceAll("\t", tabEquiv.Data());
		buf.ReplaceAll("&", "&amp;");
		buf = buf.Strip(TString::kBoth);
		for (Int_t i = 0; i < fIndentLevel; i++) buf.Prepend(tabEquiv.Data());
		Out << "<pre class=\"code\">" << this->PrepareForHtmlOutput(buf) << "</pre><br>" << endl;
	} else {
		Out << "<br>" << endl;
	}
	return(kTRUE);
}

Bool_t TMrbTidyNode::OutputHtmlForMC(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMC
// Purpose:        Special html output for node <mc>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <mc>...</mc> [code]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pName = this->GetParent()->GetName();
	TString h;
	if (pName.CompareTo("ms") == 0) h = "header3"; else h = "header2";

	this->ProcessMnodeHeader(Out, h.Data(), fTreeLevel - 4);
	return(kFALSE);
}

Bool_t TMrbTidyNode::OutputHtmlForMS(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMS
// Purpose:        Special html output for node <ms>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <ms>...</ms> [switch]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->ProcessMnodeHeader(Out, "header2", fTreeLevel - 4);
	return(kFALSE);
}

Bool_t TMrbTidyNode::OutputHtmlForMI(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputHtmlForMI
// Purpose:        Special html output for node <mi>
// Arguments:      ostream & Out    -- output stream
// Results:        Bool_t PopUp     -- kTRUE if there is no need to process childs
// Exceptions:
// Description:    Outputs html data for <mi> [include]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->ProcessMnodeHeader(Out, "header2", fTreeLevel - 4);
	return(kFALSE);
}

void TMrbTidyNode::ProcessMnodeHeader(ostream & Out, const Char_t * CssClass, Int_t Level) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::ProcessMnodeHeader
// Purpose:        Special html output for node <mc>
// Arguments:      ostream & Out     -- output stream
//                 Char_t * CssClass -- class name in css style
//                 Int_t Level       -- tree level as compared to <mb> tag
// Results:        --
// Exceptions:
// Description:    Outputs html header for marabou nodes
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cssClass = CssClass;
	Out << "<div class=\"" << cssClass << "\">" << endl;
	TMrbNamedX * nx = this->GetTidyDoc()->GetLofMnodes()->FindByName(this->GetName());
	if (nx) {
		Out << "<b>[" << fTreeLevel << "] &lt;" << this->GetName() << "&gt; [" << nx->GetTitle() << "]</b><br>" << endl;
	} else {
		Out << "<b>&lt;" << this->GetName() << "&gt;</b><br>" << endl;
	}
	Out << "<table summary=\"" << this->GetName() << "\"><tr><td width=\"200\"></td><td width=\"150\"></td><td width=\"150\"></td><td></td></tr>" << endl;
	TMrbTidyAttr * aType = (TMrbTidyAttr *) fLofAttr.FindByName("mtype");
	TString mType = "";
	if (aType) {
		mType = aType->GetValue();
		Out << "<tr><td>Type:</td><td colspan=\"3\"><pre class=\"mtag\">" << mType << "</pre></td></tr>" << endl;
	}
	Bool_t isMbody = (this->GetIndex() == TidyTag_MNODE_MB);
	if (isMbody) {
		Out << "<tr><td>Input from:</td><td colspan=\"3\">" << this->GetTidyDoc()->GetDocFile() << "</td></tr>" << endl;
		Out << "<tr><td>Tidy config from:</td><td colspan=\"3\">" << this->GetTidyDoc()->GetCfgFile() << "</td></tr>" << endl;
	}
	TString method = "";
	Bool_t isMethod = (mType.CompareTo("C++ METHOD") == 0);
	if (isMethod) {
		TMrbTidyAttr * attr = (TMrbTidyAttr *) fLofAttr.FindByName("name");
		TMrbString astr;
		if (attr) {
			astr = attr->GetValue();
			Out << "<tr><td>Name:</td><td colspan=\"3\"><pre class=\"mtag\">" << astr << "</pre></td></tr>" << endl;
			method = astr;
			method += "(";
		}
		attr = (TMrbTidyAttr *) fLofAttr.FindByName("purp");
		if (attr) {
			astr = attr->GetValue();
			Out << "<tr><td>Purpose:</td><td colspan=\"3\">" << astr  << "</td></tr>" << endl;
		}
		attr = (TMrbTidyAttr *) fLofAttr.FindByName("args");
		if (attr) {
			astr = attr->GetValue();
			TString a = "Arguments:";
			TString arg;
			Int_t from = 0;
			Int_t nargs = 0;
			while (astr.Tokenize(arg, from, ",")) {
				nargs++;
				TObjArray * sarg = arg.Tokenize(":");
				Int_t n = sarg->GetEntries();
				if (n > 0) {
					Out << "<tr>";
					if (n >= 1) Out << "<td>" << a << "</td><td><pre class=\"mtag\">" << ((TObjString *) sarg->At(0))->GetString() << "</pre></td>";
					if (n >= 2) Out << "<td><pre class=\"mtag\">" << ((TObjString *) sarg->At(1))->GetString() << "</pre></td>";
					if (n >= 3) Out << "<td>" << ((TObjString *) sarg->At(2))->GetString() << "</td>";
					Out << "</tr>" << endl;
					a = "";
					if (nargs > 1) method += ", ";
					method += ((TObjString *) sarg->At(0))->GetString();
					method += " ";
					method += ((TObjString *) sarg->At(1))->GetString();
				}
				delete sarg;
			}
		}
		method += ")";
		attr = (TMrbTidyAttr *) fLofAttr.FindByName("return");
		if (attr) {
			astr = attr->GetValue();
			TObjArray * sstr = astr.Tokenize(":");
			Int_t n = sstr->GetEntries();
			if (n > 0) {
				Out << "<tr>";
				if (n >= 1) Out << "<td>Return:</td><td><pre class=\"mtag\">" << ((TObjString *) sstr->At(0))->GetString() << "</pre></td>";
				if (n >= 2) Out << "<td><pre class=\"mtag\">" << ((TObjString *) sstr->At(1))->GetString() << "</pre></td>";
				if (n >= 3) Out << "<td>" << ((TObjString *) sstr->At(2))->GetString() << "</td>";
				Out << "</tr>" << endl;
				method.Prepend(" ");
				method.Prepend(((TObjString *) sstr->At(0))->GetString());
			}
			delete sstr;
		}
	}
	TMrbTidyAttr * aDescr = (TMrbTidyAttr *) fLofAttr.FindByName("descr");
	if (aDescr) {
		TMrbString dstr = aDescr->GetValue();
		dstr.ReplaceAll("\t", tabEquiv.Data());
		dstr = dstr.Strip(TString::kBoth);
		Out << "<tr><td>Description:</td><td colspan=\"3\">" << this->Emphasize(dstr) << "</td></tr>" << endl;
	}
	TMrbTidyAttr * aCmt = (TMrbTidyAttr *) fLofAttr.FindByName("comment");
	if (aCmt) {
		TMrbString cmt = aCmt->GetValue();
		if (cmt.BeginsWith("//")) cmt = cmt(2, 1000);
		cmt.ReplaceAll("\t", tabEquiv.Data());
		cmt.ReplaceAll("&", "&amp;");
		cmt = cmt.Strip(TString::kBoth);
		Out << "<tr><td>Comment:</td><td colspan=\"3\">" << this->Emphasize(cmt) << "</td></tr>" << endl;
	}
	TMrbTidyAttr * fileTag = (TMrbTidyAttr *) fLofAttr.FindByName("mfile");
	if (fileTag) {
		TString fileStr;
		fileStr = fileTag->GetValue();
		Out << "<tr><td>Included from file:</td><td colspan=\"3\"><pre class=\"mtag\">" << fileStr << "</pre></td></tr>" << endl;
	} else {
		TMrbTidyAttr * stdTag = (TMrbTidyAttr *) fLofAttr.FindByName("mtag");
		if (stdTag) {
			TMrbTidyAttr * altTag = (TMrbTidyAttr *) fLofAttr.FindByName("atag");
			TString tagStr;
			tagStr = stdTag->GetValue();
			if (altTag) {
				tagStr += " (";
				tagStr += altTag->GetValue();
				tagStr += ")";
			}
			Out << "<tr><td>Tag:</td><td colspan=\"3\"><pre class=\"mtag\">" << tagStr << "</pre></td></tr>" << endl;
		}
	}
	TMrbTidyAttr * aCase = (TMrbTidyAttr *) fLofAttr.FindByName("mcase");
	if (aCase) {
		if (fParent->GetIndex() != TidyTag_MNODE_MS) {
			gMrbLog->Wrn()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Attr \"mcase\" outside of <ms> tag" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMnodeHeader");
			this->TraceBack(cerr, 3);
		}
		TString aVal, aCmt;
		if (aCase->GetValue(aVal, aCmt)) {
			Out << "<tr><td>Case:</td><td><pre class=\"mtag\">" << aVal
				<< "</pre></td><td><pre class=\"mtag\">" << aCmt << "</pre></td></tr>" << endl;
		} else {
			Out << "<tr><td>Case:</td><td><pre class=\"mtag\">" << aVal
				<< "</pre></td></tr>" << endl;
		}
	} else if (fParent->GetIndex() == TidyTag_MNODE_MS) {
		gMrbLog->Wrn()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Attr \"mcase\" missing inside <ms> tag" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessMnodeHeader");
		this->TraceBack(cerr, 3);
	}

	if (this->HasTextChildsOnly()) {
		TMrbLofNamedX lofSubst;
		this->CollectSubstUsedByChilds(lofSubst);
		nx = (TMrbNamedX *) lofSubst.First();
		while (nx) {
			TMrbNamedX * mx = fLofSubstitutions.FindByName(nx->GetName());
			if (mx) mx->ChangeIndex(mx->GetIndex() | kMrbTidySubstInUse);
			nx = (TMrbNamedX *) lofSubst.After(nx);
		}
	}
	nx = (TMrbNamedX *) fLofSubstitutions.First();
	Bool_t firstSubst = kTRUE;
	TString sTitle = "Substitutions ";
	sTitle += isMbody ? " (global)" : " (local)";
	while (nx) {
		if (nx->GetIndex() & kMrbTidySubstLocalDef) {
			if (firstSubst) Out << "<tr><td>" << sTitle << ":</td>"; else Out << "<tr><td></td>";
			firstSubst = kFALSE;
			TString sdescr = nx->GetTitle();
			if (sdescr.IsNull()) {
				Out << "<td><pre class=\"arg1\">" << nx->GetName() << "</pre></td></tr>" << endl;
			} else {
				Out << "<td><pre class=\"arg1\">" << nx->GetName() << "</pre></td><td>" << sdescr << "</td></tr>" << endl;
			}
		}
		nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
	}
	sTitle = firstSubst ? "Substitutions (inherited)" : "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(inherited)";
	nx = (TMrbNamedX *) fLofSubstitutions.First();
	firstSubst = kTRUE;
	while (nx) {
		if (nx->GetIndex() & kMrbTidySubstInUse) {
			if (nx->GetIndex() & kMrbTidySubstInherited) {
				if (firstSubst) Out << "<tr><td>" << sTitle << ":</td>"; else Out << "<tr><td></td>";
				firstSubst = kFALSE;
				TString sdescr = nx->GetTitle();
				if (sdescr.IsNull()) {
					Out << "<td><pre class=\"arg1\">" << nx->GetName() << "</pre></td></tr>" << endl;
				} else {
					Out << "<td><pre class=\"arg1\">" << nx->GetName() << "</pre></td><td>" << sdescr << "</td></tr>" << endl;
				}
			}
		}
		nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
	}
	TMrbTidyAttr * aIter = (TMrbTidyAttr *) fLofAttr.FindByName("iter");
	if (aIter) Out << "<tr><td>Iteration:</td><td colspan=\"3\">" << aIter->GetValue() << "</td></tr>" << endl;
	Out << "</table></div>" << endl;

	this->SetIndentation();

	if (isMethod) {
		this->MarkLinks(method);
		Out << "<br><pre class=\"method\">" << method << " {" << "</pre>" << endl;
	}
}

const Char_t * TMrbTidyNode::Emphasize(TString & String, Bool_t Remove) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Emphasize
// Purpose:        Replace special chars by <b>, <i>, <u> etc.
// Arguments:      TString String       -- string containing meta chars
//                 Bool_t Remove        -- kTRUE -> remove meta chars
// Results:        Char_t * String      -- string after replacement
// Exceptions:
// Description:    Searches for pairs of           replaces by
//                          **                     <b></b>
//                          !!                     <i></i>
//                          __                     <u></u>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray * emph;
	Int_t nEmph;

	TString str = String;
	str.ReplaceAll("**", '\001');				// bold
	emph = str.Tokenize('\001');
	nEmph = emph->GetEntries();
	if (nEmph % 2) {
		String = "";
		if (Remove) {
			for (Int_t i = 0; i < nEmph; i++) String += ((TObjString *) emph->At(i))->GetString();
		} else {
			for (Int_t i = 0; i < nEmph; i++) {
				if (i & 1) String += "<b>";
				String += ((TObjString *) emph->At(i))->GetString();
				if (i & 1) String += "</b>";
			}
		}
	}
	delete emph;

	str = String;
	str.ReplaceAll("!!", '\001');					// italic
	emph = str.Tokenize('\001');
	nEmph = emph->GetEntries();
	if (nEmph % 2) {
		String = "";
		if (Remove) {
			for (Int_t i = 0; i < nEmph; i++) String += ((TObjString *) emph->At(i))->GetString();
		} else {
			for (Int_t i = 0; i < nEmph; i++) {
				if (i & 1) String += "<i>";
				String += ((TObjString *) emph->At(i))->GetString();
				if (i & 1) String += "</i>";
			}
		}
	}
	delete emph;

	str = String;
	str.ReplaceAll("__", '\001');					// underline
	emph = str.Tokenize('\001');
	nEmph = emph->GetEntries();
	if (nEmph % 2) {
		String = "";
		if (Remove) {
			for (Int_t i = 0; i < nEmph; i++) String += ((TObjString *) emph->At(i))->GetString();
		} else {
			for (Int_t i = 0; i < nEmph; i++) {
				if (i & 1) String += "<u>";
				String += ((TObjString *) emph->At(i))->GetString();
				if (i & 1) String += "</u>";
			}
		}
	}
	delete emph;

	str = String;
	str.ReplaceAll("||", '\001');					// box
	emph = str.Tokenize('\001');
	nEmph = emph->GetEntries();
	if (nEmph % 2) {
		String = "";
		if (Remove) {
			for (Int_t i = 0; i < nEmph; i++) String += ((TObjString *) emph->At(i))->GetString();
		} else {
			for (Int_t i = 0; i < nEmph; i++) {
				if (i & 1) String += "<bx>";
				String += ((TObjString *) emph->At(i))->GetString();
				if (i & 1) String += "</bx>";
			}
		}
	}
	return(String.Data());
}

Int_t TMrbTidyNode::SetIndentation() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::SetIndentation
// Purpose:        Set indentation level
// Arguments:      --
// Results:        Int_t IndLevel   -- indentation level
// Exceptions:
// Description:    Calculates indentation from indent attr:
//                     indent="+n"  increment by n
//                     indent="-n"  decrement by n
//                     indent="n"   set indentation to n
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t indentLevel = fIndentLevel;
	TMrbTidyAttr * aIndent = (TMrbTidyAttr *) fLofAttr.FindByName("indent");
	if (aIndent) {
		TString aInd = aIndent->GetValue();
		aInd.ReplaceAll("\t", tabEquiv.Data());
		aInd = aInd.Strip(TString::kBoth);
		switch (aInd(0)) {
			case '+':	aInd = aInd(1, 1000); indentLevel += atoi(aInd.Data()); break;
			case '-':	aInd = aInd(1, 1000); indentLevel -= atoi(aInd.Data()); break;
			default:	indentLevel = atoi(aInd.Data()); break;
		}
		this->SetIndentLevel(indentLevel, kTRUE);
	}
	return(indentLevel);
}

void TMrbTidyNode::SetIndentLevel(Int_t IndentLevel, Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::SetIndentLevel
// Purpose:        Set indent level down the tree
// Arguments:      --
// Results:        Int_t IndLevel   -- indentation level
//                 Bool_t Recursive -- set recursively if kTRUE
// Exceptions:
// Description:    Sets text indentation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fIndentLevel = IndentLevel;
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			node->SetIndentLevel(IndentLevel, kTRUE);
			node = this->GetNext(node);
		}
	}
}

Int_t TMrbTidyNode::InitSubstitutions(Bool_t Recursive, Bool_t ReInit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::InitSubstitutions
// Purpose:        Fill substitution buffer
// Arguments:      Bool_t Recursive -- step down the tree if kTRUE
//                 Bool_t ReInit    -- re-initialize if set
// Results:        Int_t NofSubst   -- number of substitutions found
// Exceptions:
// Description:    Resets subst buffer and fills it
//                 with items from "subst=..." string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nSubst = fLofSubstitutions.GetEntriesFast();
	if (ReInit || nSubst == 0) {
		TMrbTidyAttr * aSubst = (TMrbTidyAttr *) fLofAttr.FindByName("subst");
		nSubst = 0;
		fLofSubstitutions.Delete();
		if (aSubst) {
			TString subst = aSubst->GetValue();
			Int_t from = 0;
			TString sstr;
			while (subst.Tokenize(sstr, from, ",")) {
				sstr.ReplaceAll("\t", tabEquiv.Data());
				TString sdescr = "";
				Int_t n = sstr.Index(":", 0);
				if (n > 0) {
					sdescr = sstr(n + 1, 1000);
					sdescr = sdescr.Strip(TString::kBoth);
					sstr.Resize(n);
					sstr = sstr.Strip(TString::kBoth);
				}
				fLofSubstitutions.AddNamedX(kMrbTidySubstLocalDef, sstr.Data(), sdescr.Data(), new TObjString(""));
			}
		}
		TMrbLofNamedX * lofParentSubst = this->Parent()->GetLofSubstitutions();
		TMrbNamedX * nx = (TMrbNamedX *) lofParentSubst->First();
		while (nx) {
			if (!fLofSubstitutions.FindByName(nx->GetName())) {
				fLofSubstitutions.AddNamedX(kMrbTidySubstInherited, nx->GetName(), nx->GetTitle(), nx->GetAssignedObject());
			}
			nx = (TMrbNamedX *) lofParentSubst->After(nx);
		}
		Bool_t ok;
		TString buf;
		TMrbTidyAttr * aString = (TMrbTidyAttr *) fLofAttr.FindByName("string");
		if (aString) {
			buf = aString->GetValue();
			ok = kTRUE;
		} else if (this->IsText()) {
			this->GetText(buf);
			ok = kTRUE;
		}
		if (ok) {
			nx = (TMrbNamedX *) fLofSubstitutions.First();
			while (nx) {
				Bool_t inUse = kFALSE;
				TString param = nx->GetName();		// (1) #...&1U# -> upper case first char
				param.Prepend("#");
				param += "&1U#";
				inUse = (buf.Index(param.Data(), 0) >= 0);
				if (!inUse) {
					param = nx->GetName();
					param.Prepend("#");
					param += "&amp;1U#";
					inUse = (buf.Index(param.Data(), 0) >= 0);
				}
				if (!inUse) {
					param = nx->GetName();			// (2) #...&UC# -> upper case
					param.Prepend("#");
					param += "&UC#";
					inUse = (buf.Index(param.Data(), 0) >= 0);
				}
				if (!inUse) {
					param = nx->GetName();
					param.Prepend("#");
					param += "&amp;UC#";
					inUse = (buf.Index(param.Data(), 0) >= 0);
				}
				if (!inUse) {
					param = nx->GetName();
					param.Prepend("#");
					param += "#";
					inUse = (buf.Index(param.Data(), 0) >= 0);
				}
				if (inUse) nx->ChangeIndex(nx->GetIndex() | kMrbTidySubstInUse);
				nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
			}
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			node->InitSubstitutions(Recursive, ReInit);
			node = this->GetNext(node);
		}
	}
	return(fLofSubstitutions.GetEntriesFast());
}

Int_t TMrbTidyNode::InitLinks(Bool_t Recursive, Bool_t ReInit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::InitLinks
// Purpose:        Fill link buffer
// Arguments:      Bool_t Recursive -- step down the tree if kTRUE
//                 Bool_t ReInit    -- re-initialize if set
// Results:        Int_t NofLinks   -- number of links found
// Exceptions:
// Description:    Resets link buffer and fills it
//                 with items from "xxx-links=..." string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nLinks = fLofLinks.GetEntriesFast();
	if (ReInit || nLinks == 0) {
		TMrbLofNamedX lofLinkTypes;
		lofLinkTypes.AddNamedX(kMrbTidyLinkTypes);
		fLofLinks.Delete();
		TMrbNamedX * nx = (TMrbNamedX *) lofLinkTypes.First();
		while (nx) {
			TMrbTidyAttr * aLinks = (TMrbTidyAttr *) fLofAttr.FindByName(nx->GetName());
			if (aLinks) {
				TString links = aLinks->GetValue();
				links.ReplaceAll("\t", tabEquiv.Data());
				Int_t from = 0;
				TString lstr1;
				TString lstr2;
				while (links.Tokenize(lstr1, from, ",")) {
					lstr2 = "";
					Int_t n = lstr1.Index(":", 0);
					if (n > 0) {
						lstr2 = lstr1(n + 1, 1000);
						lstr2 = lstr2.Strip(TString::kBoth);
						lstr1.Resize(n);
					}
					lstr1 = lstr1.Strip(TString::kBoth);
					if (lstr2.IsNull()) lstr2 = lstr1;
					fLofLinks.AddNamedX(nx->GetIndex(), lstr1.Data(), lstr2.Data());
				}
			}
			nx = (TMrbNamedX *) lofLinkTypes.After(nx);
		}
		TMrbLofNamedX * lofParentLinks = this->Parent()->GetLofLinks();
		nx = (TMrbNamedX *) lofParentLinks->First();
		while (nx) {
			if (!fLofLinks.FindByName(nx->GetName())) {
				fLofLinks.AddNamedX(nx->GetIndex(), nx->GetName(), nx->GetTitle());
			}
			nx = (TMrbNamedX *) lofParentLinks->After(nx);
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			node->InitLinks(Recursive, ReInit);
			node = this->GetNext(node);
		}
	}
	return(fLofLinks.GetEntriesFast());
}

void TMrbTidyNode::PrintSubstitutions(Bool_t Recursive, ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::PrintSubstitutions
// Purpose:        Output substitutions
// Arguments:      Bool_t Recursive -- step down the tree if kTRUE
//                 ostream & Out    -- output stream
// Results:        --
// Exceptions:
// Description:    Outputs subst names (and values) to stream Out.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nSubst = this->InitSubstitutions(kFALSE, kFALSE);
	if (nSubst > 0) {
		TString nodeName = this->GetName();
		if (nodeName.CompareTo("mb") == 0) {
			Out << Form("%-10s%5s  %-30s%-6s%-15s%-12s%-6s%-15s%s",	"Node",
																		"level",
																		"mtag",
																		"  #s",
																		"param",
																		"mode",
																		"used",
																		"value",
																		"description") << endl;
			Out << "--------------------------------------------------------------------------------------------------------------" << endl;
		}
																		
		TMrbTidyAttr * aTag = (TMrbTidyAttr *) fLofAttr.FindByName("mtag");
		TString tag = aTag ? aTag->GetValue() : "";
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.First();
		Bool_t firstSubst = kTRUE;
		while (nx) {
			TString mode = "undef";
			if (nx->GetIndex() & kMrbTidySubstLocalDef) mode = "local";
			else if (nx->GetIndex() & kMrbTidySubstInherited) mode = "inherited";
			TString used;
			if (nx->GetIndex() & kMrbTidySubstInUse) used = "yes";
			else if (nx->GetIndex() & kMrbTidySubstInUse) used = "somewhere";
			TString sValue = ((nx->GetIndex() & kMrbTidySubstValueSet) == 0) ? "<n.a>" : ((TObjString *) nx->GetAssignedObject())->GetString().Data();
			if (firstSubst) {
				Out << Form("%-10s %3d   %-30s %3d  %-15s%-12s%-6s%-15s%s",	nodeName.Data(),
																				this->GetTreeLevel(),
																				tag.Data(),
																				nSubst,
																				nx->GetName(),
																				mode.Data(),
																				used.Data(),
																				sValue.Data(),
																				nx->GetTitle()) << endl;
			} else {
				Out << Form("%-10s%7s%-30s%6s%-15s%-12s%-6s%-15s%s",			"",
																				"",
																				"",
																				"",
																				nx->GetName(),
																				mode.Data(),
																				used.Data(),
																				sValue.Data(),
																				nx->GetTitle()) << endl;
			}
			firstSubst = kFALSE;
			nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			node->PrintSubstitutions(Recursive, Out);
			node = this->GetNext(node);
		}
	}
}

void TMrbTidyNode::ClearSubstitutions(Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::ClearSubstitutions
// Purpose:        Clear current substitution settings
// Arguments:      Bool_t Recursive -- step down the tree if kTRUE
// Results:        --
// Exceptions:
// Description:    Resets all substitutions to "n.a" (empty).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->InitSubstitutions(kFALSE, kFALSE);
	TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.First();
	while (nx) {
		nx->ChangeIndex(nx->GetIndex() & ~kMrbTidySubstValueSet);
		((TObjString *) nx->GetAssignedObject())->SetString("");
		nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			node->ClearSubstitutions(Recursive);
			node = this->GetNext(node);
		}
	}
}

Bool_t TMrbTidyNode::CheckSubstitutions(Bool_t Recursive, Bool_t VerboseMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CheckSubstitutions
// Purpose:        Check if substitutions are ok
// Arguments:      Bool_t Recursive     -- step down the tree if kTRUE
//                 Bool_t VerboseMode   -- error/warning message if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if all substitutions have values assigned.
//                 If CheckCode=kTRUE it searches for #param# in text nodes
//                 and compares with entries in subst table.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;
	if (this->InitSubstitutions(kFALSE, kFALSE) > 0) {
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.First();
		Bool_t ok = kTRUE;
		while (nx) {
			if ((nx->GetIndex() & kMrbTidySubstValueSet) == 0) {
				if (VerboseMode) {
					if (ok) {
						gMrbLog->Err()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Node \"" << this->GetName() << "\" (level " << this->GetTreeLevel()
										<< "): Subst missing for param(s) >> ";
					} else {
						gMrbLog->Err()	<< ",";
					}
					gMrbLog->Err() << nx->GetName();
				}
				ok = kFALSE;
			}
			nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
		}
		if (VerboseMode && !ok) {
			gMrbLog->Err() << " <<" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckSubstitutions");
			this->TraceBack(cerr, 3);
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			if (!node->CheckSubstitutions(Recursive, VerboseMode)) ok = kFALSE;
			node = this->GetNext(node);
		}
	}
	return(ok);
}

Int_t TMrbTidyNode::GetSubstitutionType(const Char_t * ParamName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::GetSubstitutionType
// Purpose:        Return type of substitution
// Arguments:      Char_t * ParamName     -- name of subst parameter
// Results:        EMrbTidySubstType Type -- type: local, parent, cleared
// Exceptions:
// Description:    Returns type of a substitution.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.FindByName(ParamName);
	return(nx ? nx->GetIndex() : kMrbTidySubstUndefined);
}

Bool_t TMrbTidyNode::Substitute(const Char_t * ParamName, const Char_t * ParamValue, Bool_t Recursive, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Substitute
// Purpose:        Substitute param with value
// Arguments:      Char_t * ParamName    -- param name
//                 Char_t * ParamValue   -- value to be substituted
//                 Bool_t Recursive      -- step down the tree if kTRUE
//                 Bool_t Verbose        -- kTRUE if verbose mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Substitutes given param (#param#) with string.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;
	if (this->InitSubstitutions(kFALSE, kFALSE) > 0) {
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.FindByName(ParamName);
		if (nx) {
			nx->ChangeIndex(nx->GetIndex() | kMrbTidySubstValueSet);
			((TObjString *) nx->GetAssignedObject())->SetString(ParamValue);
			ok = kTRUE;
		} else if (Verbose) {
			gMrbLog->Err()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Node \"" << this->GetName()
							<< "\" (level " << this->GetTreeLevel()
							<< "): Param not found - " << ParamName << endl;
			gMrbLog->Flush(this->ClassName(), "Substitute");
			this->TraceBack(cerr, 3);
			ok = kFALSE;
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			if ((node->GetSubstitutionType(ParamName) & kMrbTidySubstLocalDef) == 0) {
				if(!node->Substitute(ParamName, ParamValue, Recursive, Verbose)) ok = kFALSE;
			}
			node = this->GetNext(node);
		}
	}
	return(ok);
}

Bool_t TMrbTidyNode::Substitute(const Char_t * ParamName, Int_t ParamValue, Int_t ParamBase, Bool_t Recursive, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Substitute
// Purpose:        Substitute param with value
// Arguments:      Char_t * ParamName    -- param name
//                 Int_t ParamValue      -- value to be substituted
//                 Int_t ParamBase       -- num base to be used for conversion
//                 Bool_t Recursive      -- step down the tree if kTRUE
//                 Bool_t Verbose        -- kTRUE if verbose mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Substitutes given param (#param#) with int.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;
	if (this->InitSubstitutions(kFALSE, kFALSE) > 0) {
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.FindByName(ParamName);
		if (nx) {
			nx->ChangeIndex(nx->GetIndex() | kMrbTidySubstValueSet);
			TMrbString val;
			val.FromInteger(ParamValue, 0, ParamBase, kFALSE, kTRUE);
			((TObjString *) nx->GetAssignedObject())->SetString(val.Data());
			ok = kTRUE;
		} else if (Verbose) {
			gMrbLog->Err()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Node \"" << this->GetName()
							<< "\" (level " << this->GetTreeLevel()
							<< "): Param not found - " << ParamName << endl;
			gMrbLog->Flush(this->ClassName(), "Substitute");
			this->TraceBack(cerr, 3);
			ok = kFALSE;
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			if ((node->GetSubstitutionType(ParamName) & kMrbTidySubstLocalDef) == 0) {
				if(!node->Substitute(ParamName, ParamValue, ParamBase, Recursive, Verbose)) ok = kFALSE;
			}
			node = this->GetNext(node);
		}
	}
	return(ok);
}

Bool_t TMrbTidyNode::Substitute(const Char_t * ParamName, Double_t ParamValue, Bool_t Recursive, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Substitute
// Purpose:        Substitute param with value
// Arguments:      Char_t * ParamName    -- param name
//                 Double_t ParamValue   -- value to be substituted
//                 Bool_t Recursive      -- step down the tree if kTRUE
//                 Bool_t Verbose        -- kTRUE if verbose mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Substitutes given param (#param#) with double.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;
	if (this->InitSubstitutions(kFALSE, kFALSE) > 0) {
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.FindByName(ParamName);
		if (nx) {
			nx->ChangeIndex(nx->GetIndex() | kMrbTidySubstValueSet);
			TMrbString val;
			val.FromDouble(ParamValue);
			((TObjString *) nx->GetAssignedObject())->SetString(val.Data());
			ok = kTRUE;
		} else if (Verbose) {
			gMrbLog->Err()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Node \"" << this->GetName()
							<< "\" (level " << this->GetTreeLevel()
							<< "): Param not found - " << ParamName << endl;
			gMrbLog->Flush(this->ClassName(), "Substitute");
			this->TraceBack(cerr, 3);
			ok = kFALSE;
		}
	}
	if (Recursive) {
		TMrbTidyNode * node = this->GetFirst();
		while (node) {
			if ((node->GetSubstitutionType(ParamName) & kMrbTidySubstLocalDef) == 0) {
				if(!node->Substitute(ParamName, ParamValue, Recursive, Verbose)) ok = kFALSE;
			}
			node = this->GetNext(node);
		}
	}
	return(ok);
}

Bool_t TMrbTidyNode::OutputSubstituted(const Char_t * CaseString, ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputSubstituted
// Purpose:        Output text with substitutions
// Arguments:      Char_t * CaseString    -- attribute "case=..."
//                 ostream & Out          -- output stream
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Replaces params by their substitutions and outputs text.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString caseString = CaseString;
	TObjArray * lofCaseStrings = caseString.Tokenize(":");
	if (lofCaseStrings->GetEntries() > 0) lofCaseStrings->Print();
	Bool_t sts = this->OutputSubstituted(*lofCaseStrings, Out);
	delete lofCaseStrings;
	return(sts);
}

Bool_t TMrbTidyNode::OutputSubstituted(TObjArray & LofCaseStrings, ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::OutputSubstituted
// Purpose:        Output text with substitutions
// Arguments:      TObjArray & LofCaseStrings	 -- array containing attributes "case=..."
//                 ostream & Out                 -- output stream
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Replaces params by their substitutions and outputs text.
//                 Works for pure text nodes as well as for
//                 nodes with attr "string=..."
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyAttr * aCase = (TMrbTidyAttr *) fLofAttr.FindByName("mcase");
	if (aCase != NULL && LofCaseStrings.GetEntriesFast() > 0) {
		TString caseString = ((TObjString *) LofCaseStrings[0])->GetString();
		TString aVal, aCmt;
		aCase->GetValue(aVal, aCmt);
		if (caseString.CompareTo(aVal.Data()) != 0) return(kFALSE);
	}

	TString buf;
	Bool_t ok = kFALSE;

	TMrbTidyAttr * aCmt = (TMrbTidyAttr *) fLofAttr.FindByName("comment");
	if (aCmt) {
		TString cmt = aCmt->GetValue();
		if (cmt.BeginsWith("//")) Out << cmt << endl;
	}

	TMrbTidyAttr * aString = (TMrbTidyAttr *) fLofAttr.FindByName("string");
	if (aString) {
		buf = aString->GetValue();
		ok = kTRUE;
	} else if (this->IsText()) {
		this->GetText(buf);
		ok = kTRUE;
	}
	if (ok) {
		TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.First();
		while (nx) {
			if (nx->GetIndex() & kMrbTidySubstInUse) {
				if (nx->GetIndex() & kMrbTidySubstValueSet) {
					TString param = nx->GetName();		// (1) #...&1U# -> upper case first char
					param.Prepend("#");
					param += "&1U#";
					TString subst = ((TObjString *) nx->GetAssignedObject())->GetString();
					subst(0,1).ToUpper();
					buf.ReplaceAll(param, subst);
					param = nx->GetName();				// (2) #...&UC# -> upper case
					param.Prepend("#");
					param += "&UC#";
					subst = ((TObjString *) nx->GetAssignedObject())->GetString();
					subst.ToUpper();
					buf.ReplaceAll(param, subst);
					param = nx->GetName();				// (3) #...# -> normal substitution
					param.Prepend("#");
					param += "#";
					subst = ((TObjString *) nx->GetAssignedObject())->GetString();
					buf.ReplaceAll(param, subst);
				} else {
					gMrbLog->Err()	<< "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Node \"" << this->GetName() << "\" (level "
									<< this->GetTreeLevel() << "): param \"" << nx->GetName() << "\" used but not substituted" << endl;
					gMrbLog->Flush(this->ClassName(), "OutputSubstituted");
					this->TraceBack(cerr, 3);
				}
			}
			nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
		}
		Out << this->PrepareForCodeOutput(buf) << endl;
	}

	TMrbTidyNode * node = this->GetFirst();
	TObjArray lofCaseStrings;
	lofCaseStrings.Delete();
	for (Int_t i = (aCase ? 1 : 0); i < LofCaseStrings.GetEntriesFast(); i++) lofCaseStrings.Add(LofCaseStrings[i]);
	while (node) {
		node->OutputSubstituted(lofCaseStrings, Out);
		node = this->GetNext(node);
	}

	return(ok);
}

const Char_t * TMrbTidyNode::PrepareForCodeOutput(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::PrepareForCodeOutput
// Purpose:        Prepare buffer to be output as plain code
// Arguments:      TString & Buffer          -- buffer containing text
// Results:        Char_t * BufPtr           -- points to 'Buffer'
// Exceptions:
// Description:    Do some finish before starting real code output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Buffer.ReplaceAll("&lt;", "<");
	Buffer.ReplaceAll("&gt;", ">");
	Buffer.ReplaceAll("&amp;", "&");
	return(Buffer.Data());
}

const Char_t * TMrbTidyNode::PrepareForHtmlOutput(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::PrepareForHtmlOutput
// Purpose:        Prepare buffer to be output as html
// Arguments:      TString & Buffer          -- buffer containing text
// Results:        Char_t * BufPtr           -- points to 'Buffer'
// Exceptions:
// Description:    Do some finish before starting html output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Buffer.ReplaceAll("\t", tabEquiv.Data());
	this->MarkLinks(Buffer);
	this->MarkSubstitutions(Buffer);
	return(Buffer.Data());
}

const Char_t * TMrbTidyNode::MarkSubstitutions(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::MarkSubstitutions
// Purpose:        Mark substitutions for html output
// Arguments:      TString & Buffer          -- buffer containing text
// Results:        Char_t * BufPtr           -- points to 'Buffer'
// Exceptions:
// Description:    Marks (= changes background color) for all substitutions in text
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofSubstitutions.First();
	while (nx) {
		if (nx->GetIndex() & kMrbTidySubstInUse) {
			TString param = nx->GetName();			// (1) #...&1U# -> upper case first char
			TString title = "<em title=\"";
			title += nx->GetTitle();
			title += " (1st char upper case)\">";
			param.Prepend("#");
			param += "&1U#";
			TString subst = nx->GetName();
			subst(0,1).ToUpper();
			subst.Prepend(title.Data()); 
			subst += "</em>";
			Buffer.ReplaceAll(param, subst);
			param = nx->GetName();
			param.Prepend("#");
			param += "&amp;1U#";
			Buffer.ReplaceAll(param, subst);
			param = nx->GetName();				// (2) #...&UC# -> upper case
			title = "<em title=\"";
			title += nx->GetTitle();
			title += " (upper case)\">";
			param.Prepend("#");
			param += "&UC#";
			subst = nx->GetName();
			subst.ToUpper();
			subst.Prepend(title.Data()); 
			subst += "</em>";
			Buffer.ReplaceAll(param, subst);
			param = nx->GetName();
			param.Prepend("#");
			param += "&amp;UC#";
			Buffer.ReplaceAll(param, subst);
			param = nx->GetName();			// (3) #...# -> normal substitution
			title = "<em title=\"";
			title += nx->GetTitle();
			title += "\">";
			param.Prepend("#");
			param += "#";
			subst = nx->GetName();
			subst.Prepend(title.Data()); 
			subst += "</em>";
			Buffer.ReplaceAll(param, subst);
		}
		nx = (TMrbNamedX *) fLofSubstitutions.After(nx);
	}
	return(Buffer.Data());
}

const Char_t * TMrbTidyNode::MarkLinks(TString & Buffer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::MarkLinks
// Purpose:        Mark links for html output
// Arguments:      TString & Buffer          -- buffer containing text
// Results:        Char_t * BufPtr           -- points to 'Buffer'
// Exceptions:
// Description:    Marks (= inserts href tags) for all links in buffer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX lofLinkTypes;
	lofLinkTypes.AddNamedX(kMrbTidyLinkTypes);
	TMrbNamedX * nx = (TMrbNamedX *) fLofLinks.First();
	while (nx) {
		TString link = nx->GetName();
		TString href = nx->GetTitle();
		Bool_t mFlag = kFALSE;
		Int_t hl = href.Length();
		if (href(hl - 1) == '#') {
			mFlag = kTRUE;
			href.Resize(hl - 1);
		}
		TString hrefStr = "<a href=\"";
		TMrbNamedX * hr = lofLinkTypes.FindByIndex(nx->GetIndex());
		if (hr) hrefStr += hr->GetTitle(); else hrefStr += "http://undef.reference.somewhere/";
		hrefStr += href;
		hrefStr += ".html";
		if (mFlag) {
			hrefStr += "#";
			hrefStr += href;
			hrefStr += ":";
			hrefStr += nx->GetName();
		}
		hrefStr += "\">";
		hrefStr += nx->GetName();
		hrefStr += "</a>";
		Buffer.ReplaceAll(nx->GetName(), hrefStr);
		nx = (TMrbNamedX *) fLofLinks.After(nx);
	}
	return(Buffer.Data());
}

TMrbTidyNode * TMrbTidyNode::Find(const Char_t * NodeName, const Char_t * NodeAttributes, Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Find
// Purpose:        Find a specified node
// Arguments:      Char_t * NodeName         -- name
//                 Char_t * NodeAttributes   -- attributes
//                 Bool_t Recursive          -- kTRUE if to be searched recursively
// Results:        TMrbTidyNode * Node       -- resulting node
// Exceptions:
// Description:    Searches for a specified node
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray lofAttr;
	TMrbTidyNode * node;
	this->DecodeAttrString(lofAttr, NodeAttributes);
	if (Recursive) {
		TString nodeName = NodeName;
		node = this->GetFirst();
		while (node) {
			if (nodeName.CompareTo(node->GetName()) == 0 && node->CompareAttributes(lofAttr)) return(node);
			TMrbTidyNode * child = node->Find(NodeName, lofAttr, Recursive);
			if (child) return(child);
			node = this->GetNext(node);
		}
	} else {
		node = (TMrbTidyNode *) fLofChilds.FindByName(NodeName);
		if (node != NULL && node->CompareAttributes(lofAttr)) return(node);
	}
	return(NULL);
}

TMrbTidyNode * TMrbTidyNode::Find(const Char_t * NodeName, TObjArray & LofAttr, Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Find
// Purpose:        Find a specified node
// Arguments:      Char_t * NodeName         -- name
//                 TObjArray & LofAttr       -- attributes
//                 Bool_t Recursive          -- kTRUE if to be searched recursively
// Results:        TMrbTidyNode * Node       -- resulting node
// Exceptions:
// Description:    Searches for a specified node
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyNode * node;
	if (Recursive) {
		TString nodeName = NodeName;
		node = this->GetFirst();
		while (node) {
			if (nodeName.CompareTo(node->GetName()) == 0 && node->CompareAttributes(LofAttr)) return(node);
			TMrbTidyNode * child = node->Find(NodeName, LofAttr, Recursive);
			if (child) return(child);
			node = this->GetNext(node);
		}
	} else {
		node = (TMrbTidyNode *) fLofChilds.FindByName(NodeName);
		if (node != NULL && node->CompareAttributes(LofAttr)) return(node);
	}
	return(NULL);
}

Int_t TMrbTidyNode::Find(TObjArray & LofNodes, const Char_t * NodeName, const Char_t * NodeAttributes, Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Find
// Purpose:        Find a specified node
// Arguments:      TObjArray & LofNodes      -- list of nodes
//                 Char_t * NodeName         -- name
//                 Char_t * NodeAttributes   -- attributes
//                 Bool_t Recursive          -- kTRUE if to be searched recursively
// Results:        Int_t NofNodes            -- number of nodes found
// Exceptions:
// Description:    Searches for a specified node
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray lofAttr;
	TMrbTidyNode * node;
	this->DecodeAttrString(lofAttr, NodeAttributes);
	if (Recursive) {
		TString nodeName = NodeName;
		node = this->GetFirst();
		while (node) {
			if (nodeName.CompareTo(node->GetName()) == 0 && node->CompareAttributes(lofAttr)) LofNodes.Add(node);
			node->Find(LofNodes, NodeName, lofAttr, Recursive);
			node = this->GetNext(node);
		}
	} else {
		node = (TMrbTidyNode *) fLofChilds.FindByName(NodeName);
		if (node != NULL && node->CompareAttributes(lofAttr)) LofNodes.Add(node);
	}
	return(LofNodes.GetEntriesFast());
}

Int_t TMrbTidyNode::Find(TObjArray & LofNodes, const Char_t * NodeName,  TObjArray & LofAttr, Bool_t Recursive) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::Find
// Purpose:        Find a specified node
// Arguments:      TObjArray & LofNodes      -- list of nodes
//                 Char_t * NodeName         -- name
//                 TObjArray & LofAttr       -- attributes
//                 Bool_t Recursive          -- kTRUE if to be searched recursively
// Results:        Int_t NofNodes            -- number of nodes found
// Exceptions:
// Description:    Searches for a specified node
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyNode * node;
	if (Recursive) {
		TString nodeName = NodeName;
		node = this->GetFirst();
		while (node) {
			if (nodeName.CompareTo(node->GetName()) == 0 && node->CompareAttributes(LofAttr)) LofNodes.Add(node);
			node->Find(LofNodes, NodeName, LofAttr, Recursive);
			node = this->GetNext(node);
		}
	} else {
		node = (TMrbTidyNode *) fLofChilds.FindByName(NodeName);
		if (node != NULL && node->CompareAttributes(LofAttr)) LofNodes.Add(node);
	}
	return(LofNodes.GetEntriesFast());
}

TMrbTidyNode * TMrbTidyNode::FindByAttr(const Char_t * AttrName, const Char_t * AttrValue, Bool_t Recursive, Bool_t IncludeTopLevel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::FindByAttr
// Purpose:        Find a node by its attr
// Arguments:      const Char_t * AttrName   -- attr name
//                 const Char_t * AttrValue  -- ... value
//                 Bool_t Recursive          -- kTRUE if to be searched recursively
//                 Bool_t IncludeTopLevel    -- start search from caller level
// Results:        TMrbTidyNode * Node       -- resulting node
// Exceptions:
// Description:    Searches for a specified node
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbTidyNode * node;
	TString aValue = AttrValue;
	if (IncludeTopLevel) {
		TMrbTidyAttr * attr = (TMrbTidyAttr *) fLofAttr.FindByName(AttrName);
		if (attr && aValue.CompareTo(attr->GetValue()) == 0) return(node);
	}
	node = this->GetFirst();
	while (node) {
		TMrbTidyAttr * attr = (TMrbTidyAttr *) (node->GetLofAttr())->FindByName(AttrName);
		if (attr && aValue.CompareTo(attr->GetValue()) == 0) return(node);
		if (Recursive) {
			TMrbTidyNode * child = node->FindByAttr(AttrName, AttrValue, Recursive, kFALSE);
			if (child) return(child);
		}
		node = this->GetNext(node);
	}
	return(NULL);
}

Int_t TMrbTidyNode::DecodeAttrString(TObjArray & LofAttr, const Char_t * NodeAttributes) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::DecodeAttrString
// Purpose:        Decode string of attributes
// Arguments:      TObjArray & LofAttr       -- attributes
//                 Char_t * NodeAttributes   -- string of attributes
// Results:        Int_t NofAttr             -- number of attributes found
// Exceptions:
// Description:    Decodes a string of attributes.
//                 Format: "attr1=val1 attr2=val2 ..."
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	LofAttr.Delete();
	if (NodeAttributes != NULL) {
		TString str = NodeAttributes;
		Int_t from = 0;
		TString attr;
		while (str.Tokenize(attr, from, " ")) {
			attr.ReplaceAll("\t", tabEquiv.Data());
			attr = attr.Strip(TString::kBoth);
			if (attr.Length() > 0) {
				Int_t n = attr.Index("=", 0);
				if (n <= 0) {
					gMrbLog->Err() << "[" << this->GetTidyDoc()->GetDocName(kTRUE) << "] Syntax error in attr string - " << attr << " (ignored)" << endl;
					gMrbLog->Flush(this->ClassName(), "DecodeAttrString");
					this->TraceBack(cerr, 3);
					continue;
				}
				TString attrVal = attr(n + 1, attr.Length() - n - 1);
				attrVal.ReplaceAll("\t", tabEquiv.Data());
				attrVal = attrVal.Strip(TString::kBoth);
				if (attrVal(0) == '"') attrVal = attrVal(1, attrVal.Length() - 1);
				if (attrVal(attrVal.Length()) == '"') attrVal.Resize(attrVal.Length() - 1);
				attr.Resize(n);
				LofAttr.Add(new TNamed(attr.Data(), attrVal.Data()));
			}
		}
	}
	return(LofAttr.GetEntriesFast());
}

Bool_t TMrbTidyNode::CompareAttributes(TObjArray & LofAttr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode::CompareAttributes
// Purpose:        Compare attributes
// Arguments:      TObjArray & LofAttr       -- attributes
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Compares attributes in list with node attributes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = 0; i < LofAttr.GetEntriesFast(); i++) {
		TNamed * attrStr = (TNamed *) LofAttr[i];
		TString attrName = attrStr->GetName();
		TString attrVal	= attrStr->GetTitle();
		if (attrName.CompareTo("parent") == 0) {
			TString parentName = fParent ? fParent->GetName() : "root";
			if (attrVal.CompareTo(parentName.Data()) != 0) return(kFALSE);
		} else {
			TMrbTidyAttr * attr = (TMrbTidyAttr *) fLofAttr.FindByName(attrName.Data());
			if (attr == NULL) return(kFALSE);
			if (attrVal.CompareTo(attr->GetValue()) != 0) return(kFALSE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbTidyAttr::GetValue(TString & ValStr, TString & CmtStr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyAttr::GetValue
// Purpose:        Return attr value splitted
// Arguments:      TString & ValStr   -- where to store attr value
// Results:        Char_t * Value
// Exceptions:
// Description:    Splits attr value at ":" if present
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ValStr = this->GetValue();
	CmtStr = "";
	Int_t n = ValStr.Index(":", 0);
	if (n > 0) {
		CmtStr = ValStr(n + 1, 1000);
		ValStr.Resize(n);
	}
	return(n > 0);
}
