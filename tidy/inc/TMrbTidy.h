#ifndef __TMrbTidy_h__
#define __TMrbTidy_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidy.h
// Purpose:        Interface to Dave Raggett's TIDY library
// Description:    Class definitions to implement an interface to Tidy
//                 Provides wrapper classes for tidy structures
//                    TidyDoc, TidyNode, TidyOption, and TidyAttr
// Author:         R. Lutter
// Revision:       $Id: TMrbTidy.h,v 1.22 2007-08-03 10:11:05 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"

#include "TNamed.h"
#include "TString.h"

#include "TMrbNamedX.h"
#include "TMrbString.h"
#include "TMrbLofNamedX.h"

#include "TMrbString.h"

#include "tidy_protos.h"

class TMrbTidyOption;
class TMrbTidyAttr;
class TMrbTidyNode;
class TMrbTidyDoc;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyOption
// Purpose:        A tidy option
// Description:    Wrapper class for struct TidyOption
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTidyOption : public TMrbNamedX {

 	public:
		TMrbTidyOption() {};
		TMrbTidyOption(TidyOptionId OptId, const Char_t * OptName, TidyOption OptHandle, TMrbTidyDoc * Doc);
		virtual ~TMrbTidyOption() {};

		inline TidyOptionId GetId() { return((TidyOptionId) this->GetIndex()); };
		inline TidyOptionType GetType() { return(fType); };
		inline void SetType(TidyOptionType Type) { fType = Type; };
		inline Bool_t IsReadOnly() { return(tidyOptIsReadOnly(fHandle)); };

		Bool_t GetDefault(TString & String);
		Bool_t GetDefault(Int_t & Value);
		Bool_t GetDefault(Bool_t & Flag);

		Bool_t GetValue(TString & String);
		Bool_t GetValue(Int_t & Value);
		Bool_t GetValue(Bool_t & Flag);

		Bool_t SetValue(Char_t * String);
		Bool_t SetValue(Int_t Value);
		Bool_t SetValue(Bool_t Flag);

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out = cout, Bool_t Verbose = kFALSE);

		void Reset();

		inline TidyOption GetHandle() { return(fHandle); };

		inline TMrbTidyDoc * GetTidyDoc() { return(fTidyDoc); };

	protected:
		TidyOption fHandle; 			// tidy option handle
		TidyOptionType fType;			// option type

		TMrbTidyDoc * fTidyDoc;				// associated tidy document

	ClassDef(TMrbTidyOption, 1) 		// [Utils] Tidy interface: option
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyAttr
// Purpose:        A tidy attribute spec
// Description:    Wrapper class for struct TidyAttr
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTidyAttr : public TMrbNamedX {

 	public:
		TMrbTidyAttr() {};
		TMrbTidyAttr(TidyAttrId AttrId, const Char_t * AttrName, TidyOption OptHandle, TMrbTidyNode * Node);
		virtual ~TMrbTidyAttr() {};

		inline void SetValue(const Char_t * Value) { fValue = Value; };
		inline const Char_t * GetValue() { return(fValue.Data()); };
		Bool_t GetValue(TString & ValStr, TString & CmtStr);

		inline TidyAttrId GetId() { return((TidyAttrId) this->GetIndex()); };

		Bool_t IsEvent();
		Bool_t IsProp();
		Bool_t IsHREF();
		Bool_t IsSRC();
		Bool_t IsID();
		Bool_t IsNAME();
		Bool_t IsSUMMARY();
		Bool_t IsALT();
		Bool_t IsLONGDESC();
		Bool_t IsUSEMAP();
		Bool_t IsISMAP();
		Bool_t IsLANGUAGE();
		Bool_t IsTYPE();
		Bool_t IsVALUE();
		Bool_t IsCONTENT();
		Bool_t IsTITLE();
		Bool_t IsXMLNS();
		Bool_t IsDATAFLD();
		Bool_t IsWIDTH();
		Bool_t IsHEIGHT();
		Bool_t IsFOR();
		Bool_t IsSELECTED();
		Bool_t IsCHECKED();
		Bool_t IsLANG();
		Bool_t IsTARGET();
		Bool_t IsHTTP_EQUIV();
		Bool_t IsREL();
		Bool_t IsOnMOUSEMOVE();
		Bool_t IsOnMOUSEDOWN();
		Bool_t IsOnMOUSEUP();
		Bool_t IsOnCLICK();
		Bool_t IsOnMOUSEOVER();
		Bool_t IsOnMOUSEOUT();
		Bool_t IsOnKEYDOWN();
		Bool_t IsOnKEYUP();
		Bool_t IsOnKEYPRESS();
		Bool_t IsOnFOCUS();
		Bool_t IsOnBLUR();
		Bool_t IsBGCOLOR();
		Bool_t IsLINK();
		Bool_t IsALINK();
		Bool_t IsVLINK();
		Bool_t IsTEXT();
		Bool_t IsSTYLE();
		Bool_t IsABBR();
		Bool_t IsCOLSPAN();
		Bool_t IsROWSPAN();

		inline TidyAttr GetHandle() { return(fHandle); };

	protected:
		TidyAttr fHandle;
		TString fValue; 			// attr value

	ClassDef(TMrbTidyAttr, 1) 		// [Utils] Tidy interface: attribute
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyNode
// Purpose:        A tidy node spec
// Description:    Wrapper class for struct TidyNode
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTidyNode : public TMrbNamedX {

	public:

		enum EMrbTidySubstType	{	kMrbTidySubstUndefined	=	0,
									kMrbTidySubstLocalDef	=	BIT(0),
									kMrbTidySubstInherited 	=	BIT(1),
									kMrbTidySubstInUse	 	=	BIT(2),
									kMrbTidySubstValueSet	=	BIT(3)
								};

 	public:
		TMrbTidyNode() {};
		TMrbTidyNode(TidyTagId NodeId, const Char_t * NodeName, TMrbTidyNode * Parent, TidyNode NodeHandle, TMrbTidyDoc * Doc);
		virtual ~TMrbTidyNode() {};

		inline TidyNodeType GetType() { return(fType); };
		inline void SetType(TidyNodeType Type) { fType = Type; };
		inline TidyTagId GetId() { return((TidyTagId) this->GetIndex()); };

		inline TMrbTidyNode * GetFirst() { return((TMrbTidyNode *) fLofChilds.First()); };
		inline TMrbTidyNode * GetParent() { return(fParent); };
		inline TMrbTidyNode * GetNext(TMrbTidyNode * Child) { return((TMrbTidyNode *) fLofChilds.After((TMrbTidyNode *) Child)); };
		inline TMrbTidyNode * GetPrev(TMrbTidyNode * Child) { return((TMrbTidyNode *) fLofChilds.Before((TMrbTidyNode *) Child)); };

		const Char_t * GetText(TString & Buffer);

		inline void SetParent(TMrbTidyNode * Parent) { fParent = Parent; };

		inline Int_t GetTreeLevel() { return(fTreeLevel); };
		inline void SetTreeLevel(Int_t Level) { fTreeLevel = Level; };
		inline void SetTreeLevelFromParent() { fTreeLevel = fParent ? fParent->GetTreeLevel() + 1 : 1; };

		inline Int_t GetIndentLevel() { return(fIndentLevel); };
		void SetIndentLevel(Int_t Level, Bool_t Recursive = kFALSE);
		Int_t SetIndentation();

		Bool_t HasTextChildsOnly();
		const Char_t * CollectTextFromChilds(TString & Buffer);
		Int_t CollectSubstUsedByChilds(TMrbLofNamedX & LofSubst, Bool_t TextChildsOnly = kTRUE);
		inline TMrbTidyNode * Parent() { return(fParent); };

		void FillTree();
		void DeleteTree();
		Int_t StepTree(TObjArray & LofNodes);
		TMrbTidyNode * ImplantTreeFromFile(const Char_t * File = NULL, const Char_t * Mtag = NULL);

		inline TMrbLofNamedX * GetLofChilds() { return(&fLofChilds); };
		inline TMrbLofNamedX * GetLofAttr() { return(&fLofAttr); };

		Bool_t IsText();
		Bool_t IsProp();
		Bool_t IsHeader();
		Bool_t HasText();
		Bool_t IsHTML();
		Bool_t IsHEAD();
		Bool_t IsTITLE();
		Bool_t IsBASE();
		Bool_t IsMETA();
		Bool_t IsBODY();
		Bool_t IsFRAMESET();
		Bool_t IsFRAME();
		Bool_t IsIFRAME();
		Bool_t IsNOFRAMES();
		Bool_t IsHR();
		Bool_t IsH1();
		Bool_t IsH2();
		Bool_t IsPRE();
		Bool_t IsLISTING();
		Bool_t IsP();
		Bool_t IsUL();
		Bool_t IsOL();
		Bool_t IsDL();
		Bool_t IsDIR();
		Bool_t IsLI();
		Bool_t IsDT();
		Bool_t IsDD();
		Bool_t IsTABLE();
		Bool_t IsCAPTION();
		Bool_t IsTD();
		Bool_t IsTH();
		Bool_t IsTR();
		Bool_t IsCOL();
		Bool_t IsCOLGROUP();
		Bool_t IsBR();
		Bool_t IsAA();
		Bool_t IsLINK();
		Bool_t IsB();
		Bool_t IsI();
		Bool_t IsSTRONG();
		Bool_t IsEM();
		Bool_t IsBIG();
		Bool_t IsSMALL();
		Bool_t IsPARAM();
		Bool_t IsOPTION();
		Bool_t IsOPTGROUP();
		Bool_t IsIMG();
		Bool_t IsMAP();
		Bool_t IsAREA();
		Bool_t IsNOBR();
		Bool_t IsWBR();
		Bool_t IsFONT();
		Bool_t IsLAYER();
		Bool_t IsSPACER();
		Bool_t IsCENTER();
		Bool_t IsSTYLE();
		Bool_t IsSCRIPT();
		Bool_t IsNOSCRIPT();
		Bool_t IsFORM();
		Bool_t IsTEXTAREA();
		Bool_t IsBLOCKQUOTE();
		Bool_t IsAPPLET();
		Bool_t IsOBJECT();
		Bool_t IsDIV();
		Bool_t IsSPAN();
		Bool_t IsINPUT();
		Bool_t IsQ();
		Bool_t IsLABEL();
		Bool_t IsH3();
		Bool_t IsH4();
		Bool_t IsH5();
		Bool_t IsH6();
		Bool_t IsADDRESS();
		Bool_t IsXMP();
		Bool_t IsSELECT();
		Bool_t IsBLINK();
		Bool_t IsMARQUEE();
		Bool_t IsEMBED();
		Bool_t IsBASEFONT();
		Bool_t IsISINDEX();
		Bool_t IsS();
		Bool_t IsSTRIKE();
		Bool_t IsU();
		Bool_t IsMENU();

		inline Bool_t IsRoot() { return(this->GetType() == TidyNode_Root); };
		inline Bool_t IsComment() { return(this->GetType() == TidyNode_Comment); };

		inline TMrbTidyAttr * GetAttribute(TidyAttrId AttrId) { return((TMrbTidyAttr *) fLofAttr.FindByIndex((Int_t) AttrId)); };
		inline TMrbTidyAttr * GetAttrFirst() { return((TMrbTidyAttr *) fLofAttr.First()); };
		inline TMrbTidyAttr * GetAttrNext(TMrbTidyAttr * Attr) { return((TMrbTidyAttr *) fLofAttr.After((TMrbTidyAttr *) Attr)); };

		TMrbTidyAttr * GetAttrHREF();
		TMrbTidyAttr * GetAttrSRC();
		TMrbTidyAttr * GetAttrID();
		TMrbTidyAttr * GetAttrNAME();
		TMrbTidyAttr * GetAttrSUMMARY();
		TMrbTidyAttr * GetAttrALT();
		TMrbTidyAttr * GetAttrLONGDESC();
		TMrbTidyAttr * GetAttrUSEMAP();
		TMrbTidyAttr * GetAttrISMAP();
		TMrbTidyAttr * GetAttrLANGUAGE();
		TMrbTidyAttr * GetAttrTYPE();
		TMrbTidyAttr * GetAttrVALUE();
		TMrbTidyAttr * GetAttrCONTENT();
		TMrbTidyAttr * GetAttrTITLE();
		TMrbTidyAttr * GetAttrXMLNS();
		TMrbTidyAttr * GetAttrDATAFLD();
		TMrbTidyAttr * GetAttrWIDTH();
		TMrbTidyAttr * GetAttrHEIGHT();
		TMrbTidyAttr * GetAttrFOR();
		TMrbTidyAttr * GetAttrSELECTED();
		TMrbTidyAttr * GetAttrCHECKED();
		TMrbTidyAttr * GetAttrLANG();
		TMrbTidyAttr * GetAttrTARGET();
		TMrbTidyAttr * GetAttrHTTP_EQUIV();
		TMrbTidyAttr * GetAttrREL();
		TMrbTidyAttr * GetAttrOnMOUSEMOVE();
		TMrbTidyAttr * GetAttrOnMOUSEDOWN();
		TMrbTidyAttr * GetAttrOnMOUSEUP();
		TMrbTidyAttr * GetAttrOnCLICK();
		TMrbTidyAttr * GetAttrOnMOUSEOVER();
		TMrbTidyAttr * GetAttrOnMOUSEOUT();
		TMrbTidyAttr * GetAttrOnKEYDOWN();
		TMrbTidyAttr * GetAttrOnKEYUP();
		TMrbTidyAttr * GetAttrOnKEYPRESS();
		TMrbTidyAttr * GetAttrOnFOCUS();
		TMrbTidyAttr * GetAttrOnBLUR();
		TMrbTidyAttr * GetAttrBGCOLOR();
		TMrbTidyAttr * GetAttrLINK();
		TMrbTidyAttr * GetAttrALINK();
		TMrbTidyAttr * GetAttrVLINK();
		TMrbTidyAttr * GetAttrTEXT();
		TMrbTidyAttr * GetAttrSTYLE();
		TMrbTidyAttr * GetAttrABBR();
		TMrbTidyAttr * GetAttrCOLSPAN();
		TMrbTidyAttr * GetAttrROWSPAN();

		inline TidyNode GetHandle() { return(fHandle); };

		Bool_t CheckMnode();
		inline Bool_t IsMnode() const { return(fIsMnode); };
		Bool_t CheckEndTag();
		inline Bool_t HasEndTag() const { return(fHasEndTag); };

		TMrbTidyNode * Find(const Char_t * NodeName, const Char_t * NodeAttributes = NULL, Bool_t Recursive = kFALSE);
		TMrbTidyNode * Find(const Char_t * NodeName, TObjArray & LofAttr, Bool_t Recursive = kFALSE);
		Int_t Find(TObjArray & LofNodes, const Char_t * NodeName, const Char_t * NodeAttributes = NULL, Bool_t Recursive = kFALSE);
		Int_t Find(TObjArray & LofNodes, const Char_t * NodeName, TObjArray & LofAttr, Bool_t Recursive = kFALSE);

		TMrbTidyNode * FindByAttr(const Char_t * AttrName, const Char_t * AttrVal, Bool_t Recursive = kFALSE, Bool_t IncludeTopLevel = kFALSE);
		inline TMrbTidyNode * FindByMtag(const Char_t * Tag, Bool_t Recursive = kFALSE, Bool_t IncludeTopLevel = kFALSE) {
							return(this->FindByAttr("mtag", Tag, Recursive, IncludeTopLevel));
		};
		inline TMrbTidyNode * FindByMcase(const Char_t * Tag, Bool_t Recursive = kFALSE, Bool_t IncludeTopLevel = kFALSE) {
							return(this->FindByAttr("mcase", Tag, Recursive, IncludeTopLevel));
		};

		inline Bool_t HasChilds() { return(this->GetLofChilds()->GetEntries() > 0); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out = cout, Bool_t Verbose = kFALSE, Bool_t HtmlFlag = kFALSE);
		void PrintTree(ostream & Out = cout, Bool_t Verbose = kFALSE, Bool_t HtmlFlag = kFALSE);

		void TraceBack(ostream & Out = cout, Int_t NofLevels = 0);

		Bool_t OutputHtml(ostream & Out = cout);
		Bool_t OutputHtmlForMnodes(ostream & Out = cout);
		void OutputHtmlTree(ostream & Out = cout);

		Int_t InitSubstitutions(Bool_t Recursive = kFALSE, Bool_t ReInit = kFALSE);
		Bool_t Substitute(const Char_t * ParamName, const Char_t * ParamValue, Bool_t Recursive = kFALSE, Bool_t Verbose = kFALSE);	// substitute arguments
		Bool_t Substitute(const Char_t * ParamName, Int_t ParamValue, Int_t ParamBase = 10, Bool_t Recursive = kFALSE, Bool_t Verbose = kFALSE);
		Bool_t Substitute(const Char_t * ParamName, Double_t ParamValue, Bool_t Recursive = kFALSE, Bool_t Verbose = kFALSE);
		Bool_t CheckSubstitutions(Bool_t Recursive = kFALSE, Bool_t Verbose = kTRUE);
		void ClearSubstitutions(Bool_t Recursive = kFALSE);
		void PrintSubstitutions(Bool_t Recursive = kFALSE, ostream & Out = cout);
		Int_t GetSubstitutionType(const Char_t * ParamName) const;

		inline TMrbLofNamedX * GetLofSubstitutions() { return(&fLofSubstitutions); };

		Bool_t OutputSubstituted(const Char_t * CaseString = NULL, ostream & Out = cout);
		Bool_t OutputSubstituted(TObjArray & LofCaseStrings, ostream & Out = cout);

		Int_t InitLinks(Bool_t Recursive = kFALSE, Bool_t ReInit = kFALSE);
		inline TMrbLofNamedX * GetLofLinks() { return(&fLofLinks); };

		inline void AddChild(TMrbTidyNode * Child) { fLofChilds.Add(Child); };

		inline TMrbTidyDoc * GetTidyDoc() { return(fTidyDoc); };

	protected:
		Int_t ReadAttr();
		Int_t DecodeAttrString(TObjArray & LofAttr, const Char_t * AttrStr);
		Bool_t CompareAttributes(TObjArray & LofAttr);

		Bool_t OutputHtmlForMB(ostream & Out = cout);
		Bool_t OutputHtmlForMH(ostream & Out = cout);
		Bool_t OutputHtmlForMX(ostream & Out = cout);
		Bool_t OutputHtmlForMC(ostream & Out = cout);
		Bool_t OutputHtmlForMI(ostream & Out = cout);
		Bool_t OutputHtmlForMS(ostream & Out = cout);

		void ProcessMnodeHeader(ostream & Out, const Char_t * CssClass, Int_t Level);
		const Char_t * MarkSubstitutions(TString & Buffer);
		const Char_t * PrepareForHtmlOutput(TString & Buffer);
		const Char_t * PrepareForCodeOutput(TString & Buffer);
		const Char_t * MarkLinks(TString & Buffer);

		const Char_t * Emphasize(TString & String, Bool_t Remove = kFALSE);

		TMrbTidyNode * ScanTidyTree(TidyNode Node, const Char_t * AttrName, const Char_t * AttrString, TMrbTidyDoc * Doc = NULL);

	protected:
		TidyNode fHandle; 					// tidy node handle
		TidyNodeType fType; 				// type
		Int_t fTreeLevel;					// tree level (mnodes)
		Int_t fIndentLevel;					// indent level (code)

		Bool_t fIsMnode; 					// is special marabou node
		Bool_t fHasEndTag;					// kTRUE if end tag needed

		TMrbTidyNode * fParent; 			// parent node
		TMrbTidyDoc * fTidyDoc;					// associated tidy document

		TMrbLofNamedX fLofChilds;			// child nodes
		TMrbLofNamedX fLofAttr; 			// list of attributes

		TMrbLofNamedX fLofSubstitutions;	// current substitutions
		TMrbLofNamedX fLofLinks;			// current links

	ClassDef(TMrbTidyNode, 1) 				// [Utils] Tidy interface: node
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyDoc
// Purpose:        A document tree for tidy
// Description:    Create a tidy document tree
//                 Wrapper class for struct TidyDoc
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTidyDoc : public TNamed {

 	public:
		TMrbTidyDoc();
		TMrbTidyDoc(const Char_t * DocName);
		TMrbTidyDoc(const Char_t * DocName, const Char_t * DocFile, Bool_t Repair = kFALSE, const Char_t * CfgFile = NULL);
		TMrbTidyDoc(const Char_t * DocName, istream & Stream, Bool_t Repair = kFALSE, const Char_t * CfgFile = NULL);
		TMrbTidyDoc(const Char_t * DocName, TString & Buffer, Bool_t Repair = kFALSE, const Char_t * CfgFile = NULL);
		virtual ~TMrbTidyDoc() { this->Reset(kTRUE); };

		void Reset(Bool_t Release = kFALSE);
		inline Int_t GetStatus() const { return(tidyStatus(fHandle)); };
		inline Int_t GetHtmlVersion() const { return(tidyDetectedHtmlVersion(fHandle)); };
		inline Bool_t IsXhtml() const { return(tidyDetectedXhtml(fHandle)); };
		inline Bool_t IsXml() const { return(tidyDetectedGenericXml(fHandle)); };
		inline Int_t GetErrorCount() const { return((Int_t) tidyErrorCount(fHandle)); };
		inline Int_t GetWarningCount() const { return((Int_t) tidyWarningCount(fHandle)); };
		inline Int_t GetAccessWarningCount() const { return((Int_t) tidyAccessWarningCount(fHandle)); };

		inline const Char_t * GetErrorBuffer() { return(fErrorBuffer.bp); };
		inline void PrintDiagnostics(ostream & Out = cout) { Out << this->GetErrorBuffer() << endl; };

		Bool_t LoadConfig(const Char_t * CfgFile);
		inline Int_t GetConfigErrorCount() const { return(tidyConfigErrorCount(fHandle)); };

		inline Bool_t SetOption(TidyOptionId OptionId, Bool_t Flag) {
			TMrbTidyOption * opt = (TMrbTidyOption *) fLofOptions.FindByIndex((Int_t ) OptionId);
			if (opt) return(opt->SetValue(Flag)); else return(kFALSE);
		};
		inline Bool_t SetOption(TidyOptionId OptionId, Int_t Value) {
			TMrbTidyOption * opt = (TMrbTidyOption *) fLofOptions.FindByIndex((Int_t ) OptionId);
			if (opt) return(opt->SetValue(Value)); else return(kFALSE);
		};
		inline Bool_t SetOption(TidyOptionId OptionId, const Char_t * String) {
			TMrbTidyOption * opt = (TMrbTidyOption *) fLofOptions.FindByIndex((Int_t ) OptionId);
			if (opt) return(opt->SetValue(String)); else return(kFALSE);
		};
		inline TMrbLofNamedX * GetLofOptions() { return(&fLofOptions); };

		inline TMrbLofNamedX * GetLofMnodes() { return(&fLofMnodes); };

		Bool_t ParseFile(const Char_t * DocFile, Bool_t Repair = kFALSE);
		Bool_t ParseBuffer(const Char_t * Buffer, Bool_t Repair = kFALSE);
		inline Bool_t ParseBuffer(TString & Buffer, Bool_t Repair = kFALSE) { return(this->ParseBuffer(Buffer.Data(), Repair)); };

		inline Bool_t IsRepaired() { return(fRepair); };
		Bool_t CleanAndRepair();
		Bool_t RunDiagnostics();

		inline Bool_t HasNodes() { return((fTidyRoot != NULL) && fTidyRoot->HasChilds()); };

		inline void StripText(Bool_t Flag = kTRUE) { fStripText = Flag; };
		inline Bool_t TextToBeStripped() { return(fStripText); };

		inline TidyDoc GetHandle() const { return(fHandle); };
		inline const Char_t * GetDocFile() const { return(fDocFile.Length() ? fDocFile.Data() : NULL); };
		inline const Char_t * GetCfgFile() const { return(fCfgFile.Length() ? fCfgFile.Data() : NULL); };

		inline Bool_t ResetOptions() { return(tidyOptResetAllToDefault(fHandle)); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(const Char_t * File, Bool_t Verbose = kFALSE, Bool_t HtmlFlag = kFALSE);
		void Print(ostream & Out = cout, Bool_t Verbose = kFALSE, Bool_t HtmlFlag = kFALSE);

		void PrintOptions(ostream & Out = cout, Bool_t Verbose = kFALSE);

		inline TMrbTidyNode * GetRoot() { return(fTidyRoot); };
		inline TMrbTidyNode * GetHtml() { return(fTidyHtml); };
		inline TMrbTidyNode * GetHead() { return(fTidyHead); };
		inline TMrbTidyNode * GetBody() { return(fTidyBody); };
		inline TMrbTidyNode * GetMbody() { return(fTidyMbody); };

		inline void SetMnodeFlag(Bool_t Flag = kTRUE) { fHasMnodes = Flag; };
		inline Bool_t HasMnodes() { return(fHasMnodes); };

		Bool_t OutputHtml(const Char_t * HtmlFile);
		Bool_t OutputHtml(ostream & Out = cout);

		Bool_t AddToList(); 			// add to list of documents

		const Char_t * GetDocName(Bool_t LongFlag = kFALSE);

	protected:
		Int_t ReadOptions();
		void InitErrorBuffer();
		Bool_t IsFirstChild();
		Bool_t IsLastChild();

	protected:
		TidyDoc fHandle;				// tidy doc handle
		TString fDocFile;				// name of document file
		TString fCfgFile;				// name of configuration file
		Bool_t fRepair; 				// kTRUE if 'clean and repair' is to be called
		Bool_t fStripText;				// kTRUE if <cr>s surrounding text have to be stripped
		Bool_t fHasMnodes;				// kTRUE if special marabou nodes exist
		TMrbTidyNode * fTidyRoot;		// ptr to whole document
		TMrbTidyNode * fTidyHtml;		// ptr to <html>...</html>
		TMrbTidyNode * fTidyHead;		// prt to <head>...</head>
		TMrbTidyNode * fTidyBody;		// ptr to <body>...</body>
		TMrbTidyNode * fTidyMbody;		// ptr to <mb>...</mb>
	
		TMrbLofNamedX fLofOptions;			// list of options
		TMrbLofNamedX fLofMnodes;			// list of special marabou nodes

		TidyBuffer fErrorBuffer;		// internal error buffer

	ClassDef(TMrbTidyDoc, 1) 			// [Utils] Tidy interface: document
};

#endif
