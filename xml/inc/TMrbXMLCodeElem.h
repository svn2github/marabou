#ifndef __TMrbXMLCodeElem_h__
#define __TMrbXMLCodeElem_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/inc/TMrbXMLCodeElem.h
// Purpose:        MARaBOU's XML based code generator
// Class:          TMrbXMLCodeElem    -- XML code elemenet/node
// Description:    Class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXMLCodeElem.h,v 1.8 2008-01-18 13:35:16 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>

#include <Riostream.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TEnv.h>

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TMrbXMLCodeClient.h"

class TMrbXMLCodeGen;

// element indices

enum EMrbXmlElements {
		kMrbXml_MrbCode = 1,
		kMrbXml_File,
		kMrbXml_FileHdr,
		kMrbXml_Class,
		kMrbXml_ClassHdr,
		kMrbXml_Ctor,
		kMrbXml_CtorHdr,
		kMrbXml_Method,
		kMrbXml_MethodHdr,
		kMrbXml_Funct,
		kMrbXml_FunctHdr,
		kMrbXml_Code,
		kMrbXml_Cbody,
		kMrbXml_If,
		kMrbXml_Foreach,
		kMrbXml_Switch,
		kMrbXml_Case,
		kMrbXml_Item,
		kMrbXml_Ftype,
		kMrbXml_ArgList,
		kMrbXml_Arg,
		kMrbXml_Atype,
		kMrbXml_ReturnVal,
		kMrbXml_Comment,
		kMrbXml_RootClassList,
		kMrbXml_MrbClassList,
		kMrbXml_Inheritance,
		kMrbXml_ClassRef,
		kMrbXml_Tag,
		kMrbXml_Flag,
		kMrbXml_Slist,
		kMrbXml_Subst,
		kMrbXml_Xname,
		kMrbXml_Mname,
		kMrbXml_Fname,
		kMrbXml_Aname,
		kMrbXml_Gname,
		kMrbXml_Cname,
		kMrbXml_Vname,
		kMrbXml_Sname,
		kMrbXml_Purp,
		kMrbXml_Descr,
		kMrbXml_Author,
		kMrbXml_Mail,
		kMrbXml_Url,
		kMrbXml_Version,
		kMrbXml_Date,
		kMrbXml_s,
		kMrbXml_S,
		kMrbXml_L,
		kMrbXml_B,
		kMrbXml_I,
		kMrbXml_U,
		kMrbXml_Bx,
		kMrbXml_M,
		kMrbXml_Mm,
		kMrbXml_R,
		kMrbXml_Rm,
		kMrbXml_Nl
};

enum	{	kMrbXmlHasOwnText = 0x1000 };
enum	{	kMrbXmlIsZombie = 0x8000 };

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem
// Purpose:        Describe a XML code element
// Description:    Stores data of an xml node for the code generator
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbXMLCodeElem: public TMrbNamedX {

	public:

    	TMrbXMLCodeElem(TMrbNamedX * Element = NULL, Int_t NestingLevel = 0, TMrbXMLCodeGen * Parser = NULL);
		virtual ~TMrbXMLCodeElem() {};

		inline void AddCode(const Char_t * Code) { fCode += Code; };
		inline const Char_t * GetCode() { return(fCode.Data()); };
		inline void ClearCode() { fCode.Resize(0); };

		inline Int_t NestingLevel() { return(fNestingLevel); };
		inline TMrbXMLCodeElem * Parent() { return(fParent); };

		inline TEnv * LofAttr() { return(&fLofAttr); };
		inline TEnv * LofChildren() { return(&fLofChildren); };
		inline TEnv * LofSubst() { return(&fLofSubst); };

		inline Bool_t IsRootElement() { return(fParent == NULL); };
		inline Bool_t HasOwnText() { return(fHasOwnText); };

		Bool_t SetAttributes(const TList * LofAttr);

		Bool_t ProcessElement(const Char_t * ElemName);

		void Debug(ostream & Out, TString & FocusOnElement, TString & FocusOnTag, Bool_t OnStart);

	protected:

		Bool_t ParentIs(const Char_t * LofParents, Bool_t Verbose = kTRUE);
		Bool_t NameIs(const Char_t * ElemName, Bool_t Verbose = kTRUE);
		Bool_t HasChild(const Char_t * ChildName, Bool_t Verbose = kTRUE);
		Bool_t HasChild(const Char_t * ChildName, TString & ChildCode, Bool_t Verbose = kTRUE);
		TMrbXMLCodeElem * HasAncestor(const Char_t * Ancestor, Bool_t Verbose = kTRUE);
		Bool_t CopyCodeToParent(const Char_t * IndentString = "");
		Bool_t CopyChildToParent(const Char_t * ChildName, const Char_t * ChildCode = NULL);
		Bool_t CopySubstToParent(const Char_t * Sname = NULL, const Char_t * Descr = NULL, const Char_t * Tag = NULL);
		Bool_t GetChildFromParent(const Char_t * ElemName, TString & ElemCode);
		Bool_t FindTag(TString & Tag, Bool_t Verbose = kTRUE);
		Bool_t FindSubst(TString & Subst, Bool_t Verbose = kTRUE);
		Bool_t RequestLofItems(const Char_t * Tag, const Char_t * ItemName, TString & LofItems);
		Bool_t RequestConditionFlag(const Char_t * Tag, const Char_t * FlagName, TString & FlagValue);
		void ClearSubst();
		Bool_t RequestSubst(const Char_t * Tag, const Char_t * ItemName, const Char_t * Item, TEnv * LofSubst);
		Bool_t Substitute(TString & Code, Bool_t Verbose = kFALSE);
		Bool_t ExpandSwitchAndIf() { return(kTRUE); };

		void Indent(ostream & Out);

		Bool_t ProcessElement_MrbCode();
		Bool_t ProcessElement_File();
		Bool_t ProcessElement_FileHdr();
		Bool_t ProcessElement_Class();
		Bool_t ProcessElement_ClassHdr();
		Bool_t ProcessElement_Ctor();
		Bool_t ProcessElement_CtorHdr();
		Bool_t ProcessElement_Method();
		Bool_t ProcessElement_MethodHdr();
		Bool_t ProcessElement_Funct();
		Bool_t ProcessElement_FunctHdr();
		Bool_t ProcessElement_Code();
		Bool_t ProcessElement_Cbody();
		Bool_t ProcessElement_If();
		Bool_t ProcessElement_Foreach();
		Bool_t ProcessElement_Switch();
		Bool_t ProcessElement_Case();
		Bool_t ProcessElement_Item();
		Bool_t ProcessElement_Ftype();
		Bool_t ProcessElement_ArgList();
		Bool_t ProcessElement_Arg();
		Bool_t ProcessElement_Atype();
		Bool_t ProcessElement_ReturnVal();
		Bool_t ProcessElement_Comment();
		Bool_t ProcessElement_RootClassList();
		Bool_t ProcessElement_MrbClassList();
		Bool_t ProcessElement_Inheritance();
		Bool_t ProcessElement_ClassRef();
		Bool_t ProcessElement_Tag();
		Bool_t ProcessElement_Flag();
		Bool_t ProcessElement_Slist();
		Bool_t ProcessElement_Subst();
		Bool_t ProcessElement_Xname();
		Bool_t ProcessElement_Mname();
		Bool_t ProcessElement_Fname();
		Bool_t ProcessElement_Aname();
		Bool_t ProcessElement_Gname();
		Bool_t ProcessElement_Cname();
		Bool_t ProcessElement_Vname();
		Bool_t ProcessElement_Sname();
		Bool_t ProcessElement_Purp();
		Bool_t ProcessElement_Descr();
		Bool_t ProcessElement_Author();
		Bool_t ProcessElement_Mail();
		Bool_t ProcessElement_Url();
		Bool_t ProcessElement_Version();
		Bool_t ProcessElement_Date();
		Bool_t ProcessElement_s();
		Bool_t ProcessElement_S();
		Bool_t ProcessElement_L();
		Bool_t ProcessElement_B();
		Bool_t ProcessElement_I();
		Bool_t ProcessElement_U();
		Bool_t ProcessElement_Bx();
		Bool_t ProcessElement_M();
		Bool_t ProcessElement_Mm();
		Bool_t ProcessElement_R();
		Bool_t ProcessElement_Rm();
		Bool_t ProcessElement_Nl();

	protected:

		Int_t fNestingLevel;		// nesting
		Bool_t fHasOwnText;			// kTRUE if element is not a container but has its own text
		TMrbXMLCodeElem * fParent;	// parent element
		TString fCode;				// code
		TEnv fLofAttr;				// attributes: <tag attr="...">
		TEnv fLofSubst; 			// substitutions: <S>subst</S>, <s>subst</s>
		TEnv fLofChildren;			// code produced by children

		TMrbXMLCodeGen * fParser;	// connect to calling parser object

	ClassDef(TMrbXMLCodeElem, 1) 	// [XML] Code element
};

#endif
