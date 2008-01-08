//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/src/TMrbXMLCodeElem.cxx
// Purpose:        MARaBOU XML based code generator: code element
// Description:    Implements class methods to process xml data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXMLCodeElem.cxx,v 1.2 2008-01-08 15:09:21 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TList.h"

#include "TMrbLogger.h"
#include "TMrbXMLCodeElem.h"

#include "SetColor.h"

ClassImp(TMrbXMLCodeElem)

extern TMrbLogger * gMrbLog;

TMrbXMLCodeElem::TMrbXMLCodeElem(TMrbNamedX * Element, Int_t NestingLevel)
				: TMrbNamedX(Element ? Element->GetIndex() : 0, Element ? Element->GetName() : "") {
									
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem
// Purpose:        Create a code element
// Arguments:      Char_t * ElemName        -- element name
//                 Int_t NestingLevel       -- nesting
//                 TMrbXMLCodeElem * Parent  -- parent element
// Results:        --
// Exceptions:
// Description:    Handles code elements for the code generator
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("sax.log");
	fParent = (TMrbXMLCodeElem *) Element->GetAssignedObject();
	fNestingLevel = NestingLevel;
}

Bool_t TMrbXMLCodeElem::SetAttributes(const TList * LofAttr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::SetAttributes
// Purpose:        Store attributes
// Arguments:      TList * LofAttr  -- list of attributes
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TXMLAttr * attr;
	TIterator * iter = LofAttr->MakeIterator();
	while (attr = (TXMLAttr *) iter->Next()) fLofAttr.SetValue(attr->GetName(), attr->GetValue());
	return(kTRUE);
}

Bool_t TMrbXMLCodeElem::NameIs(const Char_t * ElemName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::NameIs
// Purpose:        Compare name (of end tag) with element name
// Arguments:      Char_t * ElemName   -- element name
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString elemName = this->GetName();
	if (elemName.CompareTo(ElemName) != 0) {
		gMrbLog->Err() << "[" << this->GetName() << "] XML tag out of phase - </" << ElemName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "NameIs");
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}


Bool_t TMrbXMLCodeElem::ParentIs(const Char_t * ParentName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ParentIs
// Purpose:        Check if we have the right parent
// Arguments:      Char_t * ParentName   -- name of parent element
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TString elemName = fParent->GetName();
		if (elemName.CompareTo(ParentName) != 0) {
			gMrbLog->Err() << "[" << this->GetName() << "] Wrong parent element - " << elemName << " (should be <" << ParentName << ">)" << endl;
			gMrbLog->Flush(this->ClassName(), "ParentIs");
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "ParentIs");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::HasChild(const Char_t * ChildName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasChild
// Purpose:        Check if there is a given child element
// Arguments:      Char_t * ChildName   -- name of child element
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString childCode = fLofChildCodes.GetValue(ChildName, "");
	if (childCode.IsNull()) {
		gMrbLog->Err() << "[" << this->GetName() << "] Child element missing - <" << ChildName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "HasChild");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbXMLCodeElem::HasChild(const Char_t * ChildName, TString & ChildCode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasChild
// Purpose:        Check if there is a given child element
// Arguments:      Char_t * ChildName   -- name of child element
//                 TString & ChildCode  -- where to store its code
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	ChildCode = fLofChildCodes.GetValue(ChildName, "");
	if (ChildCode.IsNull()) {
		gMrbLog->Err() << "[" << this->GetName() << "] Child element missing - <" << ChildName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "HasChild");
		return(kFALSE);
	}
	return(kTRUE);
}

TMrbXMLCodeElem * TMrbXMLCodeElem::HasAncestor(const Char_t * Ancestor) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasAncestor
// Purpose:        Check if there is a given ancestor element
// Arguments:      Char_t * Ancestor            -- name of ancestor element
// Results:        TMrbXMLCodeElem * Ancestor   -- address of ancestor
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TMrbXMLCodeElem * e = fParent;
		while (e) {
			TString ename = e->GetName();
			if (ename.CompareTo(Ancestor) == 0) return(e);
			e = e->Parent();
		}
		gMrbLog->Err() << "[" << this->GetName() << "] No ancestor <" << Ancestor << "> found" << endl;
		gMrbLog->Flush(this->ClassName(), "HasAncestor");
		return(NULL);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "HasAncestor");
		return(NULL);
	}
}

Bool_t TMrbXMLCodeElem::CopyCodeToParent(const Char_t * Ename) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopyCodeToParent
// Purpose:        Copy code to parent node
// Arguments:      Char_t * Ename    -- element name to be used
// Results:        kTRUE/kFALSE
// Description:    Copies code to parent element
//                 either to parent's buffer
//                 or to a separate list to be used for subsequent formatting
//                 depending on Ename
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		if (Ename == NULL || *Ename != '\0') {
			fParent->AddCode(fCode);
		} else {
			fParent->LofChildCodes()->SetValue(Ename, fCode);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyCodeToParent");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::CopySubstToParent(const Char_t * Sname, const Char_t * Sval) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopySubstToParent
// Purpose:        Copy substitutions to parent node
// Arguments:      Char_t * Sname    -- name of subst
//                 Char_t * Sval     -- value
// Results:        kTRUE/kFALSE
// Description:    Copies substitutions (<s>...</s> and <S>...</S>) to parent node
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		if (Sname != NULL && *Sname != '\0') {
			fParent->LofSubst()->SetValue(Sname, Sval);
		} else {
			TIterator * iter = ((TList *) fLofSubst.GetTable())->MakeIterator();
			TEnvRec * r;
			while (r = (TEnvRec *) iter->Next()) fParent->LofSubst()->SetValue(r->GetName(), r->GetValue());
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "CopySubstToParent");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::GetFromParent(const Char_t * ElemName, TString & ElemCode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::GetFromParent
// Purpose:        Get a sub-element from parent
// Arguments:      Char_t * ElemName  -- element name
//                 TString & ElemCode -- code
// Results:        kTRUE/kFALSE
// Description:    Passes a element from parent to child
//////////////////////////////////////////////////////////////////////////////

	ElemCode = "";
	if (fParent) {
		ElemCode = fParent->LofChildCodes()->GetValue(ElemName, "");
		if (ElemCode.IsNull()) {
			gMrbLog->Err()	<< "[" << this->GetName() << "] Couldn't get element <" << ElemName
							<< "> from parent <" << fParent->GetName() << ">" << endl;
			gMrbLog->Flush(this->ClassName(), "GetFromParent");
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "GetFromParent");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::InheritTag(TString & Tag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::InheritTag
// Purpose:        Ask ancestors for a tag
// Arguments:      TString & Tag  -- tag name
// Results:        kTRUE/kFALSE
// Description:    Passes a element from parent to child
//////////////////////////////////////////////////////////////////////////////

	Tag = "";
	if (fParent) {
		TMrbXMLCodeElem * e = fParent;
		while (e) {
			Tag = e->LofChildCodes()->GetValue("tag", "");
			if (!Tag.IsNull()) return(kTRUE);
			e = e->Parent();
		}
		gMrbLog->Err() << "[" << this->GetName() << "] No ancestor found having a tag" << endl;
		gMrbLog->Flush(this->ClassName(), "InheritTag");
		return(kFALSE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "InheritTag");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement(const Char_t * ElemName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement
// Purpose:        Operations to be done on end tag
// Arguments:      Char_t * ElemName   -- element name
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (!this->NameIs(ElemName)) this->SetIndex(kMrbXmlZombie);

	Bool_t sts;

	if (this->GetIndex() != kMrbXmlZombie) {
		switch (this->GetIndex()) {
			case kMrbXml_MrbCode:		sts = this->ProcessElement_MrbCode(); break;
			case kMrbXml_File:			sts = this->ProcessElement_File(); break;
			case kMrbXml_FileHdr:		sts = this->ProcessElement_FileHdr(); break;
			case kMrbXml_Class: 		sts = this->ProcessElement_Class(); break;
			case kMrbXml_ClassHdr:		sts = this->ProcessElement_ClassHdr(); break;
			case kMrbXml_Ctor:			sts = this->ProcessElement_Ctor(); break;
			case kMrbXml_CtorHdr:		sts = this->ProcessElement_CtorHdr(); break;
			case kMrbXml_Method:		sts = this->ProcessElement_Method(); break;
			case kMrbXml_MethodHdr: 	sts = this->ProcessElement_MethodHdr(); break;
			case kMrbXml_Funct: 		sts = this->ProcessElement_Funct(); break;
			case kMrbXml_FunctHdr:		sts = this->ProcessElement_FunctHdr(); break;
			case kMrbXml_Code:			sts = this->ProcessElement_Code(); break;
			case kMrbXml_Cbody: 		sts = this->ProcessElement_Cbody(); break;
			case kMrbXml_If:			sts = this->ProcessElement_If(); break;
			case kMrbXml_Foreach:		sts = this->ProcessElement_Foreach(); break;
			case kMrbXml_Switch:		sts = this->ProcessElement_Switch(); break;
			case kMrbXml_Case:			sts = this->ProcessElement_Case(); break;
			case kMrbXml_Item:			sts = this->ProcessElement_Item(); break;
			case kMrbXml_Ftype: 		sts = this->ProcessElement_Ftype(); break;
			case kMrbXml_ArgList:		sts = this->ProcessElement_ArgList(); break;
			case kMrbXml_Arg:			sts = this->ProcessElement_Arg(); break;
			case kMrbXml_Atype: 		sts = this->ProcessElement_Atype(); break;
			case kMrbXml_ReturnVal: 	sts = this->ProcessElement_ReturnVal(); break;
			case kMrbXml_Comment:		sts = this->ProcessElement_Comment(); break;
			case kMrbXml_RootClassList: sts = this->ProcessElement_RootClassList(); break;
			case kMrbXml_MrbClassList:	sts = this->ProcessElement_MrbClassList(); break;
			case kMrbXml_Inheritance:	sts = this->ProcessElement_Inheritance(); break;
			case kMrbXml_ClassRef:		sts = this->ProcessElement_ClassRef(); break;
			case kMrbXml_Tag:			sts = this->ProcessElement_Tag(); break;
			case kMrbXml_Slist: 		sts = this->ProcessElement_Slist(); break;
			case kMrbXml_Subst: 		sts = this->ProcessElement_Subst(); break;
			case kMrbXml_Name:			sts = this->ProcessElement_Name(); break;
			case kMrbXml_Aname:			sts = this->ProcessElement_Aname(); break;
			case kMrbXml_Gname: 		sts = this->ProcessElement_Gname(); break;
			case kMrbXml_Cname: 		sts = this->ProcessElement_Cname(); break;
			case kMrbXml_Vname: 		sts = this->ProcessElement_Vname(); break;
			case kMrbXml_Sname: 		sts = this->ProcessElement_Sname(); break;
			case kMrbXml_Purp:			sts = this->ProcessElement_Purp(); break;
			case kMrbXml_Descr: 		sts = this->ProcessElement_Descr(); break;
			case kMrbXml_Author:		sts = this->ProcessElement_Author(); break;
			case kMrbXml_Mail:			sts = this->ProcessElement_Mail(); break;
			case kMrbXml_Url:			sts = this->ProcessElement_Url(); break;
			case kMrbXml_Version:		sts = this->ProcessElement_Version(); break;
			case kMrbXml_Date:			sts = this->ProcessElement_Date(); break;
			case kMrbXml_s: 			sts = this->ProcessElement_s(); break;
			case kMrbXml_S: 			sts = this->ProcessElement_S(); break;
			case kMrbXml_L: 			sts = this->ProcessElement_L(); break;
		}
		return(kTRUE);
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_MrbCode() { return(kTRUE); };

Bool_t TMrbXMLCodeElem::ProcessElement_File() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_File
// Purpose:        File statement: <file>...</file>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Writes code to file given by <gname>.
//////////////////////////////////////////////////////////////////////////////

	TString gname;
	if (!this->HasChild("gname", gname)) return(kFALSE);
	ofstream file(gname.Data(), ios::out);
	if (!file.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << gname << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_File");
		return(kFALSE);
	}
	file << fCode << endl;
	file.close();
	gMrbLog->Out() << "[" << this->GetName() << "] Generated code written to file " << gname << endl;
	gMrbLog->Flush(this->ClassName(), "ProcessElement_File", setblue);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_FileHdr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_FileHdr
// Purpose:        File statement: <fileHdr>...</fileHdr>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Creates a standard file header
// Elements:       <gname>       -- name of generated code file
//                 <purp>        -- purpose
//                 <descr>       -- description
//                 <author>      -- author
//                 <mail>        -- author's mail addr
//                 <url>         -- author's url
//                 <version>     -- cvs version
//                 <date>        -- modification date
//////////////////////////////////////////////////////////////////////////////

	fCode =  "//___________________________________________________________[C++ FILE HEADER]\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n";
	fCode += "// Name:           "; fCode += fLofChildCodes.GetValue("gname", ""); fCode += "\n";
	fCode += "// Purpose:        "; fCode += fLofChildCodes.GetValue("purp", ""); fCode += "\n";
	fCode += "// Description:    "; fCode += fLofChildCodes.GetValue("descr", ""); fCode += "\n";
	fCode += "// Author:         "; fCode += fLofChildCodes.GetValue("author", ""); fCode += "\n";
	fCode += "// Revision:       "; fCode += fLofChildCodes.GetValue("version", ""); fCode += "\n";
	fCode += "// Date:           "; fCode += fLofChildCodes.GetValue("date", ""); fCode += "\n";
	fCode += "// URL:            "; fCode += fLofChildCodes.GetValue("url", ""); fCode += "\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////";
	this->CopyCodeToParent();
	return(kTRUE);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Class() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ClassHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ctor() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_CtorHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Method() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_MethodHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Funct() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_FunctHdr() { return(kTRUE); };

Bool_t TMrbXMLCodeElem::ProcessElement_Code() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Code
// Purpose:        Code statement: <code>...</code>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Main code container. If tagged performs substsitutions.
//                 Then copies code to parent.
// Elements:       <tag>      -- (opt) identifying tag
//////////////////////////////////////////////////////////////////////////////

	TString tag = fLofChildCodes.GetValue("tag", "");
	Bool_t err = kFALSE;
	if (!tag.IsNull()) {
		this->ClearSubst();
		err |= !this->RequestSubst(tag.Data(), NULL, NULL, this->LofSubst());
		err |= !this->Substitute();
	}
	err |= !this->CopyCodeToParent();
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Cbody() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Cbody
// Purpose:        Code statement: <cbody>...</cbody>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Container for code elements,
//                 passes code to parent only
//////////////////////////////////////////////////////////////////////////////

	return(this->CopyCodeToParent());
}

Bool_t TMrbXMLCodeElem::ProcessElement_Foreach() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Foreach
// Purpose:        Loop statement: <foreach>...</foreach>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Loops over list of <item>s and copies code to parent
// Elements:       <item>      -- list of item to loop on
//////////////////////////////////////////////////////////////////////////////

	TString itemName;
	if (!this->HasChild("item", itemName)) return(kFALSE);
	if (this->HasAncestor("foreach")) {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't be nested" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_Foreach");
		return(kFALSE);
	}
	TString tag;
	if (!this->InheritTag(tag)) return(kFALSE);
	TString lofItems;
	if (!this->RequestItemValue(tag.Data(), itemName.Data(), lofItems)) return(kFALSE);
	TString origCode = fCode;
	Int_t from = 0;
	TString item;
	Bool_t err = kFALSE;
	while (lofItems.Tokenize(item, from, ":")) {
		fCode = origCode;
		this->ClearSubst();
		err |= !this->RequestSubst(tag.Data(), itemName.Data(), item.Data(), this->LofSubst());
		err |= !this->Substitute();
		err |= !this->ExpandSwitchAndIf();
		err |= !this->CopyCodeToParent();
	}
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Switch() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Switch
// Purpose:        Switch statement: <switch>...</switch>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    If <switch> is child of a <foreach>
//                 copy all <case> elements to parent and add "#switch|item# to code.
//                 If not, ask for an item value and look for a matching <case>
// Elements:       <item>      -- item to be compared to <case> values
//////////////////////////////////////////////////////////////////////////////

	TString item;
	if (!this->HasChild("item", item)) return(kFALSE);
	TMrbXMLCodeElem * foreach;
	if (foreach = this->HasAncestor("foreach")) {
		TIterator * iter = ((TList *) fLofChildCodes.GetTable())->MakeIterator();
		TEnvRec * r;
		while (r = (TEnvRec *) iter->Next()) {
			TString rname = r->GetName();
			if (rname.BeginsWith("case")) foreach->LofChildCodes()->SetValue(r->GetName(), r->GetValue());
		}
		fCode = Form("#switch|%s#\n", item.Data());
		fLofSubst.SetValue(Form("#s|%s#\n", item.Data()), "");
	} else {
		TString tag;
		if (!this->InheritTag(tag)) return(kFALSE);
		TString itemVal;
		if (!this->RequestItemValue(tag.Data(), item.Data(), itemVal)) return(kFALSE);
		if (!this->HasChild(Form("case_%s_%s", item.Data(), itemVal.Data()), fCode)) return(kFALSE);
	}
	Bool_t err = !this->CopyCodeToParent();
	err |= !this->CopySubstToParent();
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_If() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_If
// Purpose:        Condition statement: <if>...</if>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    If <if> is child of a <foreach> add "#if|flag# to code.
//                 If not, ask for a flag value and copy code to parent if TRUE
// Elements:       <flag>      -- boolean flag to be checked
//////////////////////////////////////////////////////////////////////////////

	TString flag;
	if (!this->HasChild("flag", flag)) return(kFALSE);
	if (this->HasAncestor("foreach")) {
		fCode = Form("#if|%s#\n", flag.Data());
		fLofSubst.SetValue(Form("#s|%s#\n", flag.Data()), "");
	} else {
		TString tag;
		if (!this->InheritTag(tag)) return(kFALSE);
		TString flagVal;
		if (!this->RequestFlag(tag.Data(), flag.Data(), flagVal)) return(kFALSE);
		if (flagVal.CompareTo("TRUE") != 0) return(kFALSE);
	}
	Bool_t err = !this->CopyCodeToParent();
	err |= !this->CopySubstToParent();
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Case() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Case
// Purpose:        Case statement: <case>...</case>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes code and substitutions to parent element <switch>
// Elements:       <tag>      -- case tag
//////////////////////////////////////////////////////////////////////////////

	if (!this->ParentIs("switch")) return(kFALSE);
	TString caseTag;
	if (!this->HasChild("tag", caseTag)) return(kFALSE);
	
	TString itemName;
	if (!this->GetFromParent("item", itemName)) return(kFALSE);
	Bool_t err = !this->CopyCodeToParent(Form("case_%s_%s", itemName.Data(), caseTag.Data()));
	err |= !this->CopySubstToParent();
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Arg() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Arg
// Purpose:        Argument: <arg>...</arg>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes argument to parent <argList>
//                 arguments will be stored in a :-separated string
// Elements:       <aname>      -- argument name
//                 <atype>      -- type
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("argList")) {
		TString aname;
		if (!this->HasChild("aname", aname)) return(kFALSE);
		TString atype;
		if (!this->HasChild("atype", atype)) return(kFALSE);
		fCode = Form("%s|%s:", atype.Data(), aname.Data());
		return(this->CopyCodeToParent());
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Comment() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Comment
// Purpose:        Comment: <comment>...</comment>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    If attribute type=code creates a c/c++ like comment
//                 NOP otherwise
//////////////////////////////////////////////////////////////////////////////

	TString type = fLofAttr.GetValue("type", "");
	if (type.CompareTo("code") == 0) {
		fCode.Prepend("// ");
		return(this->CopyCodeToParent());
	}
	return(kTRUE);
}

Bool_t TMrbXMLCodeElem::ProcessElement_ClassRef() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_ClassRef
// Purpose:        Class reference: <classRef>...</classRef>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes class reference to parent <inheritance>
//                 class refs will be stored in a :-separated string
// Elements:       <cname>      -- class name
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("inheritance")) {
		TString cname;
		if (!this->HasChild("cname", cname)) return(kFALSE);
		fCode = Form("%s:", cname.Data());
		return(this->CopyCodeToParent());
	}
	return(kFALSE);
}


Bool_t TMrbXMLCodeElem::ProcessElement_Tag() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Tag
// Purpose:        Tag: <tag>...</tag>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes tag to parent
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		return(this->CopyCodeToParent(this->GetName()));
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_Tag");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_s() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_s
// Purpose:        Substitute: <s>...</s>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Wraps "#s|...#" (substitution, lower case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	fCode = Form("#s|%s#", fCode.Data());
	Bool_t err = !this->CopySubstToParent(fCode, "");
	err |= !this->CopyCodeToParent();
	return(!err);
};

Bool_t TMrbXMLCodeElem::ProcessElement_S() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_S
// Purpose:        Substitute: <S>...</S>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Wraps "#S|...#" (substitution, upper case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	fCode = Form("#S|%s#", fCode.Data());
	Bool_t err = !this->CopySubstToParent(fCode, "");
	err |= !this->CopyCodeToParent();
	return(!err);
};

Bool_t TMrbXMLCodeElem::ProcessElement_L() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_L
// Purpose:        One line of code: <l>...</l>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Appends a "\n", then copies code to parent
//                 Any substitutions found will also be transferred to parent
//////////////////////////////////////////////////////////////////////////////

	fCode += "\n";
	Bool_t err = !this->CopyCodeToParent();
	err |= !this->CopySubstToParent();
	return(!err);
};

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements which pass code to parent for subsequent formatting
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Adds code to a separate list to be used later on by parent
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_ArgList() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_ReturnVal() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Inheritance() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Name() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Aname() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Gname() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Cname() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Vname() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Sname() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Purp() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Descr() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Author() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mail() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Url() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Version() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Date() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Atype() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Item() { return(this->CopyCodeToParent(this->GetName())); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ftype() { return(this->CopyCodeToParent(this->GetName())); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Pure HTML tags: copy plain code to parent
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    <b>...</b>      boldface
//                 <i> ... </i>    italic
//                 <u> ... </u>    underline
//                 <bx> ... </bx>  boxed
//                 Code will be copied to parent
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_B() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_I() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_U() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_Bx() { return(this->CopyCodeToParent()); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements used for HTML output only: copy plain code to parent
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    <m>...</m>      MARaBOU class link
//                 <mm> ... </mm>  MARaBOU method link
//                 <r> ... </r>    ROOT class link
//                 <rm> ... </rm>  ROOT method link
//                 Code will be copied to parent
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_M() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mm() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_R() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_Rm() { return(this->CopyCodeToParent()); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements which have meaning for HTML only, NOP here
// Arguments:      --
// Results:        kTRUE
// Description:    <slist>...</slist>                  MARaBOU substitution
//                 <subst>...</subst>                  ...
//                 <mrbClassList>...</mrbClassList>    list of MARaBOU classes
//                 <rootClassList>...</rootClassList>  list of ROOT classes
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_Slist() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Subst() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_RootClassList() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_MrbClassList() { return(kTRUE); };

