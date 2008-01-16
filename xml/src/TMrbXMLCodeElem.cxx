//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/src/TMrbXMLCodeElem.cxx
// Purpose:        MARaBOU XML based code generator: code element
// Description:    Implements class methods to process xml data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXMLCodeElem.cxx,v 1.6 2008-01-16 07:56:47 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TMrbLogger.h"
#include "TMrbXMLCodeElem.h"
#include "TMrbXMLCodeGen.h"

#include "SetColor.h"

ClassImp(TMrbXMLCodeElem)

extern TMrbLogger * gMrbLog;

TMrbXMLCodeElem::TMrbXMLCodeElem(TMrbNamedX * Element, Int_t NestingLevel, TMrbXMLCodeGen * Parser)
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
	fParser = Parser;
	fParent = (TMrbXMLCodeElem *) Element->GetAssignedObject();
	fNestingLevel = NestingLevel;
	fCode = "";
	if (Element->GetIndex() == kMrbXmlIsZombie) this->MakeZombie();
	fHasOwnText = (Element->GetIndex() & kMrbXmlHasOwnText);
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

Bool_t TMrbXMLCodeElem::NameIs(const Char_t * ElemName, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::NameIs
// Purpose:        Compare name (of end tag) with element name
// Arguments:      Char_t * ElemName   -- element name
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString elemName = this->GetName();
	if (elemName.CompareTo(ElemName) != 0) {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] XML tag out of phase - </" << ElemName << ">" << endl;
			gMrbLog->Flush(this->ClassName(), "NameIs");
		}
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}


Bool_t TMrbXMLCodeElem::ParentIs(const Char_t * LofParents, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ParentIs
// Purpose:        Check if we have the right parent
// Arguments:      Char_t * LofParents   -- name(s) of possible parent element(s)
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TString pname = fParent->GetName();
		TString lofParents = LofParents;
		Int_t from = 0;
		TString pn;
		while (lofParents.Tokenize(pn, from, "|")) {
			if (pname.CompareTo(pn.Data()) == 0) return(kTRUE);
		}
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] Wrong parent element - <" << pname << "> (should be in <" << LofParents << ">)" << endl;
			gMrbLog->Flush(this->ClassName(), "ParentIs");
		}
		return(kFALSE);
	} else {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
			gMrbLog->Flush(this->ClassName(), "ParentIs");
		}
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::HasChild(const Char_t * ChildName, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasChild
// Purpose:        Check if there is a given child element
// Arguments:      Char_t * ChildName   -- name of child element
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString childCode = fLofChildren.GetValue(ChildName, "");
	if (childCode.IsNull()) {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] Child element missing - <" << ChildName << ">" << endl;
			gMrbLog->Flush(this->ClassName(), "HasChild");
		}
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbXMLCodeElem::HasChild(const Char_t * ChildName, TString & ChildCode, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasChild
// Purpose:        Check if there is a given child element
// Arguments:      Char_t * ChildName   -- name of child element
//                 TString & ChildCode  -- where to store its code
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	ChildCode = fLofChildren.GetValue(ChildName, "");
	if (ChildCode.IsNull()) {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] Child element missing - <" << ChildName << ">" << endl;
			gMrbLog->Flush(this->ClassName(), "HasChild");
		}
		return(kFALSE);
	}
	return(kTRUE);
}

TMrbXMLCodeElem * TMrbXMLCodeElem::HasAncestor(const Char_t * Ancestor, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::HasAncestor
// Purpose:        Check if there is a given ancestor element
// Arguments:      Char_t * Ancestor            -- name of ancestor element
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        TMrbXMLCodeElem * Ancestor   -- address of ancestor
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TMrbXMLCodeElem * e = fParent;
		while (e) {
			TString ename = e->GetName();
			if (ename.CompareTo(Ancestor) == 0) return(e);
			e = e->Parent();
		}
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No ancestor <" << Ancestor << "> found" << endl;
			gMrbLog->Flush(this->ClassName(), "HasAncestor");
		}
		return(NULL);
	} else {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
			gMrbLog->Flush(this->ClassName(), "HasAncestor");
		}
		return(NULL);
	}
}

Bool_t TMrbXMLCodeElem::CopyCodeToParent() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopyCodeToParent
// Purpose:        Copy code to parent node
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Copies code to parent element
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		fParent->AddCode(fCode);
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyCodeToParent");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::CopyChildToParent(const Char_t * ChildName, const Char_t * ChildCode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::PassChildToParent
// Purpose:        Xfer child to parent
// Arguments:      Char_t * ChildName    -- child element to be copied
//                 Char_t * ChildCode    -- code
// Results:        kTRUE/kFALSE
// Description:    Copies child code to parent element:
//                 If no 'ChildCode' is given xfers child 'ChildName'
//                 to list of parent childs.
//                 If 'ChildCode' is present creates a new child in the
//                 parent's list of childs
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TString childCode;
		if (ChildCode == NULL) {
			if (!this->HasChild(ChildName, childCode)) return(kFALSE);
		} else {
			childCode = ChildCode;
		}
		fParent->LofChildren()->SetValue(ChildName, childCode);
		return(kTRUE);
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyChildToParent");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::CopySubstToParent(const Char_t * Sname, Bool_t UpperCase, const Char_t * Svalue, const Char_t * Tag) {
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
		if (Sname != NULL) {
			fParent->LofSubst()->SetValue(Form("%s.Name", Sname), (Svalue == NULL ? "" : Svalue));
			fParent->LofSubst()->SetValue(Form("%s.UpperCase", Sname), (UpperCase ? "TRUE" : "FALSE"));
			fParent->LofSubst()->SetValue(Form("%s.RequestFrom", Sname), Form("%s:%s", this->GetName(), (Tag == NULL) ? "NoTag" : Tag));
		} else {
			TList * lsubst = (TList *) fLofSubst.GetTable();
			if (lsubst) {
				TIterator * iter = lsubst->MakeIterator();
				TEnvRec * r;
				while (r = (TEnvRec *) iter->Next()) fParent->LofSubst()->SetValue(r->GetName(), r->GetValue());
			}
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
		ElemCode = fParent->LofChildren()->GetValue(ElemName, "");
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

Bool_t TMrbXMLCodeElem::InheritTag(TString & Tag, Bool_t Verbose) {
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
			Tag = e->LofChildren()->GetValue("tag", "");
			if (!Tag.IsNull()) return(kTRUE);
			e = e->Parent();
		}
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No ancestor found having a tag" << endl;
			gMrbLog->Flush(this->ClassName(), "InheritTag");
		}
		return(kFALSE);
	} else {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
			gMrbLog->Flush(this->ClassName(), "InheritTag");
		}
		return(kFALSE);
	}
}

void TMrbXMLCodeElem::ClearSubst() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ClearSubst
// Purpose:        Clear list of substitutions
// Arguments:      --
// Results:        --
// Description:    Sets any substitions to blank
//////////////////////////////////////////////////////////////////////////////

	TList * lsubst = (TList *) fLofSubst.GetTable();
	if (lsubst) {
		TIterator * iter = lsubst->MakeIterator();
		TEnvRec * r;
		while (r = (TEnvRec *) iter->Next()) fLofSubst.SetValue(r->GetName(), "");
	}
}

Bool_t TMrbXMLCodeElem::RequestLofItems(const Char_t * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::RequestLofItems
// Purpose:        Ask client for a list of items
// Arguments:      Char_t * Tag       -- current tag
//                 Char_t * ItemName  -- name of item
//                 TString & LofItems -- list of item values
// Results:        kTRUE/kFALSE
// Description:    Interface to client object: Call for item values
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		TMrbXMLCodeClient * client = fParser->Client();
		if (client) {
			return(client->ProvideLofItems(this->GetName(), Tag, ItemName, LofItems));
		} else {
			gMrbLog->Err() << "[" << this->GetName() << "] No client connected - can't get item values for tag=\"" << Tag << "\", item=\"" << ItemName << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "RequestLofItems");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No XML parser connected - can't get item values for tag=\"" << Tag << "\", item=\"" << ItemName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "RequestLofItems");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::RequestSubst(const Char_t * Tag, const Char_t * ItemName, const Char_t * Item, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::RequestSubst
// Purpose:        Ask client for a list of substitutions
// Arguments:      Char_t * Tag    -- current tag
//                 Char_t * ItemName  -- name of item
//                 Char_t * Item      -- item value
//                 TEnv * LofSubst    -- list of substitutions
// Results:        kTRUE/kFALSE
// Description:    Interface to client object: Call for substitutions
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		TMrbXMLCodeClient * client = fParser->Client();
		if (client) {
			return(client->ProvideSubst(this->GetName(), Tag, ItemName, Item, LofSubst));
		} else {
			gMrbLog->Err()	<< "[" << this->GetName() << "] No client connected - can't get substitutions for tag=\""
							<< Tag << "\", item \"" << ItemName << "=" << Item << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "RequestSubst");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err()	<< "[" << this->GetName() << "] No XML parser connected - can't get substitutions for tag=\""
						<< Tag << "\", item \"" << ItemName << "=" << Item << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "RequestSubst");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::RequestConditionFlag(const Char_t * Tag, const Char_t * FlagName, TString & FlagValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::RequestConditionFlag
// Purpose:        Ask client for a condition
// Arguments:      Char_t * Tag          -- current tag
//                 Char_t * FlagName     -- name of condition
//                 TString & FlagValue   -- value
// Results:        kTRUE/kFALSE
// Description:    Interface to client object: Call for condition
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		TMrbXMLCodeClient * client = fParser->Client();
		if (client) {
			return(client->ProvideConditionFlag(this->GetName(), Tag, FlagName, FlagValue));
		} else {
			gMrbLog->Err() << "[" << this->GetName() << "] No client connected - can't get condition for tag=\"" << Tag << "\", flag=\"" << FlagName << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "RequestConditionFlag");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err() << "[" << this->GetName() << "] No XML parser connected - can't get condition for tag=\"" << Tag << "\", flag=\"" << FlagName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "RequestConditionFlag");
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

	if (!this->NameIs(ElemName)) this->MakeZombie();

	Bool_t sts;

	if (!this->IsZombie()) {
		switch (this->GetIndex() & 0xFFF) {
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
			case kMrbXml_Flag:			sts = this->ProcessElement_Flag(); break;
			case kMrbXml_Slist: 		sts = this->ProcessElement_Slist(); break;
			case kMrbXml_Subst: 		sts = this->ProcessElement_Subst(); break;
			case kMrbXml_Xname:			sts = this->ProcessElement_Xname(); break;
			case kMrbXml_Mname:			sts = this->ProcessElement_Mname(); break;
			case kMrbXml_Fname:			sts = this->ProcessElement_Fname(); break;
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
			case kMrbXml_M: 			sts = this->ProcessElement_M(); break;
			case kMrbXml_Mm: 			sts = this->ProcessElement_Mm(); break;
			case kMrbXml_R: 			sts = this->ProcessElement_R(); break;
			case kMrbXml_Rm: 			sts = this->ProcessElement_Rm(); break;
			case kMrbXml_Nl: 			sts = this->ProcessElement_Nl(); break;
		}
		return(kTRUE);
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_MrbCode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_MrbCode
// Purpose:        File statement: <mrbcode>...</mrbcode>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    XML root element
//////////////////////////////////////////////////////////////////////////////

	TString xname;
	if (this->HasChild("xname", xname)) {
		gMrbLog->Out() << "[" << this->GetName() << "] XML data parsed from file " << xname << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_MrbCode", setblue);
	}
	return(kTRUE);
}

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

	if (!this->CopyChildToParent("xname")) return(kFALSE);

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
// Parent elems:   <file>        -- file
// Child elems:    <gname>       -- name of generated code file
//                 <purp>        -- purpose
//                 <descr>       -- description
//                 <author>      -- author
//                 <mail>        -- author's mail addr
//                 <url>         -- author's url
//                 <version>     -- cvs version
//                 <date>        -- modification date
//////////////////////////////////////////////////////////////////////////////

	if (!this->ParentIs("file")) return(kFALSE);

	this->CopyChildToParent("xname");
	this->CopyChildToParent("gname");
	fCode =  "//___________________________________________________________[C++ FILE HEADER]\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n";
	fCode += "// Name:           "; fCode += fLofChildren.GetValue("gname", ""); fCode += "\n";
	fCode += "// Purpose:        "; fCode += fLofChildren.GetValue("purp", ""); fCode += "\n";
	fCode += "// Description:    "; fCode += fLofChildren.GetValue("descr", ""); fCode += "\n";
	fCode += "// Author:         "; fCode += fLofChildren.GetValue("author", ""); fCode += "\n";
	fCode += "// Revision:       "; fCode += fLofChildren.GetValue("version", ""); fCode += "\n";
	fCode += "// Date:           "; fCode += fLofChildren.GetValue("date", ""); fCode += "\n";
	fCode += "// URL:            "; fCode += fLofChildren.GetValue("url", ""); fCode += "\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n";
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
// Child elems:    <tag>      -- (opt) identifying tag
//////////////////////////////////////////////////////////////////////////////

	TString tag = fLofChildren.GetValue("tag", "");
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
// Child elems:    <item>      -- list of item to loop on
//////////////////////////////////////////////////////////////////////////////

	TString itemName;
	if (!this->HasChild("item", itemName)) return(kFALSE);
	if (this->HasAncestor("foreach", kFALSE)) {
		gMrbLog->Err() << "[" << this->GetName() << "] Can't be nested" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_Foreach");
		return(kFALSE);
	}
	TString tag;
	if (!this->InheritTag(tag)) return(kFALSE);
	TString lofItems;
	if (!this->RequestLofItems(tag.Data(), itemName.Data(), lofItems)) return(kFALSE);
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
//                 copy all <case> elements to parent and add "@switch|item# to code.
//                 If not, ask for an item value and look for a matching <case>
// Child elems:    <item>      -- item to be compared to <case> values
//////////////////////////////////////////////////////////////////////////////

	TString item;
	if (!this->HasChild("item", item)) return(kFALSE);
	TMrbXMLCodeElem * foreach;
	if (foreach = this->HasAncestor("foreach")) {
		TList * lchild = (TList *) fLofChildren.GetTable();
		if (lchild) {
			TIterator * iter = lchild->MakeIterator();
			TEnvRec * r;
			while (r = (TEnvRec *) iter->Next()) {
				TString rname = r->GetName();
				if (rname.BeginsWith("case")) foreach->LofChildren()->SetValue(r->GetName(), r->GetValue());
			}
		}
		fCode = Form("@switch|%s@\n", item.Data());
		fLofSubst.SetValue(Form("@s|%s@", item.Data()), "");
	} else {
		TString tag;
		if (!this->InheritTag(tag)) return(kFALSE);
		TString itemVal;
		if (!this->RequestLofItems(tag.Data(), item.Data(), itemVal)) return(kFALSE);
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
// Description:    If <if> is child of a <foreach> add "@if|flag# to code.
//                 If not, ask for a flag value and copy code to parent if TRUE
// Child elems:    <flag>      -- boolean flag to be checked
//////////////////////////////////////////////////////////////////////////////

	TString flag;
	if (!this->HasChild("flag", flag)) return(kFALSE);
	if (this->HasAncestor("foreach")) {
		fCode = Form("@if|%s@\n", flag.Data());
		fLofSubst.SetValue(Form("@s|%s@", flag.Data()), "");
	} else {
		TString tag;
		if (!this->InheritTag(tag)) return(kFALSE);
		TString flagVal;
		if (!this->RequestConditionFlag(tag.Data(), flag.Data(), flagVal)) return(kFALSE);
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
// Parent elems:   <switch>   -- switch statement
// Child elems:    <tag>      -- case tag
//////////////////////////////////////////////////////////////////////////////

	if (!this->ParentIs("switch")) return(kFALSE);
	TString caseTag;
	if (!this->HasChild("tag", caseTag)) return(kFALSE);
	
	TString itemName;
	if (!this->GetFromParent("item", itemName)) return(kFALSE);
	Bool_t err = !this->CopyChildToParent(Form("case_%s_%s", itemName.Data(), caseTag.Data()), fCode);
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
// Parent elems:   <argList>    -- list of arguments
//                 <returnVal>  -- return value
// Child elems:    <aname>      -- argument name
//                 <atype>      -- type
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("argList|returnVal")) {
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
		fCode += "\n";
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
// Parent elems:   <inheritance>    -- class inheritance
//                 <rootClassList>  -- list of ROOT classes used
//                 <mrbClassList>   -- list of MARaBOU classes used
// Child elems:    <cname>          -- class name
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("inheritance|rootClassList|mrbClassList")) {
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
// Parent elems:   <code>    -- tagged code block
//                 <case>    -- case statement within <switch>
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("code|case")) {
		return(this->CopyChildToParent(this->GetName(), fCode));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Flag() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Flag
// Purpose:        Tag: <flag>...</flag>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes flag to parent
// Parent elems:   <if>    -- if clause
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("if")) {
		return(this->CopyChildToParent(this->GetName(), fCode));
	} else {
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
// Description:    Wraps "@s|...@" (substitution, lower case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	TString tag;
	this->InheritTag(tag, kFALSE);
	Bool_t err = !this->CopySubstToParent(fCode, kFALSE, "", tag.Data());
	fCode = Form("@s|%s@", fCode.Data());
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
// Description:    Wraps "@S|...@" (substitution, upper case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	TString tag;
	this->InheritTag(tag, kFALSE);
	Bool_t err = !this->CopySubstToParent(fCode, kTRUE, "", tag.Data());
	fCode = Form("@S|%s@", fCode.Data());
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

Bool_t TMrbXMLCodeElem::ProcessElement_ArgList() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_ReturnVal() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Inheritance() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Xname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Fname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Aname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Gname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Cname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Vname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Sname() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Purp() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Descr() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Author() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mail() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Url() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Version() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Date() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Atype() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Item() { return(this->CopyChildToParent(this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ftype() { return(this->CopyChildToParent(this->GetName(), fCode)); };

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
//                 <nl>            new line
//                 Code will be copied to parent
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_B() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_I() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_U() { return(this->CopyCodeToParent()); };
Bool_t TMrbXMLCodeElem::ProcessElement_Bx() { return(this->CopyCodeToParent()); };

Bool_t TMrbXMLCodeElem::ProcessElement_Nl() { fCode = "\n"; return(this->CopyCodeToParent()); };

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

void TMrbXMLCodeElem::Debug(ostream & Out, TString & FocusOnElement, TString & FocusOnTag, Bool_t OnStart) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::Debug
// Purpose:        Debugger
//////////////////////////////////////////////////////////////////////////////

	if (FocusOnElement.CompareTo("any") != 0) {
		if (FocusOnElement.CompareTo(this->GetName()) != 0) return;
	}
	if (FocusOnTag.CompareTo("any") != 0) {
		TString tag;
		if (!this->HasChild("tag", tag, kFALSE)) return;
		if (FocusOnTag.CompareTo(tag.Data()) != 0) return;
	}

	TString x, s;
	if (OnStart) {
		x = "<xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		s = Form("[Start <%s>]", this->GetName());
		x(2, s.Length()) = s;
		this->Indent(Out); Out << x.Data() << endl;
	} else {
		x = "/==========================================================================================";
		s = Form("[End <%s>]", this->GetName());
		x(2, s.Length()) = s;
		this->Indent(Out); Out << x.Data() << endl;
		x = "|------------------------------------------------------------------------------------------";
		TList * l = (TList *) fLofChildren.GetTable();
		if (l) {
			this->Indent(Out); Out << "| List of child elements:" << endl;
			TIterator * iter = l->MakeIterator();
			TEnvRec * r;
			while (r = (TEnvRec *) iter->Next()) {
				TString name = r->GetName();
				TString code = r->GetValue();
				Int_t from = 0;
				while (code.Tokenize(code, from, "\n")) {
					this->Indent(Out); Out << Form("|   %-15s  |%s|", name.Data(), code.Data()) << endl;
					name = "";
				}
			}
			this->Indent(Out); Out << x.Data() << endl;
		}
		l = (TList *) fLofSubst.GetTable();
		if (l) {
			this->Indent(Out); Out << "| List of substitutions:" << endl;
			TIterator * iter = l->MakeIterator();
			TEnvRec * r;
			while (r = (TEnvRec *) iter->Next()) {
				TString name = r->GetName();
				TString code = r->GetValue();
				Int_t from = 0;
				while (code.Tokenize(code, from, "\n")) {
					this->Indent(Out); Out << Form("|   %-15s  |%s|", name.Data(), code.Data()) << endl;
					name = "";
				}
			}
			this->Indent(Out); Out << x.Data() << endl;
		}
		Int_t from = 0;
		TString line;
		TString name = "Code:";
		while (fCode.Tokenize(line, from, "\n")) {
			this->Indent(Out); Out << Form("| %-17s  |%s|", name.Data(), line.Data()) << endl;
			name = "";
		}
		x(0) = '\\';
		this->Indent(Out); Out << x.Data() << endl;
	}
}

void TMrbXMLCodeElem::Indent(ostream & Out) {
	Out << Form("[%2d]", fNestingLevel);
	for (Int_t i = 0; i < fNestingLevel; i++) Out << "  ";
}
