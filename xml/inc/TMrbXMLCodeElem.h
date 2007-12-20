#ifndef __TMrbXMLCodeElem_h__
#define __TMrbXMLCodeElem_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/inc/TMrbXMLCodeElem.h
// Purpose:        MARaBOU's XML based code generator
// Class:          TMrbXMLCodeElem    -- XML code elemenet/node
// Description:    Class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXMLCodeElem.h,v 1.1 2007-12-20 07:54:41 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <Riostream.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TEnv.h>

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

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
		kMrbXml_Slist,
		kMrbXml_Subst,
		kMrbXml_Name,
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
		kMrbXml_Rm
};

enum	{	kMrbXmlZombie = -1 };

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem
// Purpose:        Describe a XML code element
// Description:    Stores data of an xml node for the code generator
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbXMLCodeElem: public TMrbNamedX {

	public:

    	TMrbXMLCodeElem(TMrbNamedX * Element = NULL, Int_t NestingLevel = 0);
		virtual ~TMrbXMLCodeElem() {};

		inline const Char_t * Tag() { return(fTag.Data()); };

		inline void AddCode(const Char_t * Code) { fCode += Code; };
		inline const Char_t * GetCode() { return(fCode.Data()); };
		inline void ClearCode() { fCode.Resize(0); };

		inline NestingLevel() { return(fNestingLevel); };
		inline TMrbXMLCodeElem * Parent() { return(fParent); };

		inline TEnv * LofAttr() { return(&fLofAttr); };
		inline TEnv * LofChildren() { return(&fLofChildren); };

		inline TEnv * LofSubst() { return(&fLofSubst); };

		inline IsRootElement() { return(fParent == NULL); };

		Bool_t SetAttributes(const TList * LofAttr);

		Bool_t ProcessElement(const Char_t * ElemName);

	protected:

		inline void SetTag(const Char_t * Tag) { fTag = Tag; };
		Bool_t CheckParent(const Char_t * ParentName);
		Bool_t CheckName(const Char_t * ElemName);
		inline Bool_t CopyCodeToParent() { fParent->AddCode(fCode); return(kTRUE); };
		Bool_t CopySubstToParent();

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
		Bool_t ProcessElement_Slist();
		Bool_t ProcessElement_Subst();
		Bool_t ProcessElement_Name();
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

	protected:

		Int_t fNestingLevel;		// nesting
		TString fTag;				// tag code
		TMrbXMLCodeElem * fParent;	// parent element
		TEnv fAttr; 				// attributes
		TString fCode;				// code
		TEnv fLofAttr;				// attributes: <tag attr="...">
		TEnv fLofChildren;			// (code produced by) children
		TEnv fLofSubst; 			// substitutions: <S>subst</S>, <s>subst</s>

	ClassDef(TMrbXMLCodeElem, 1) 	// [XML] Code element
};

#endif
