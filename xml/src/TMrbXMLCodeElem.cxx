//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/src/TMrbXMLCodeElem.cxx
// Purpose:        MARaBOU XML based code generator: code element
// Description:    Implements class methods to process xml data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXMLCodeElem.cxx,v 1.1 2007-12-20 07:54:41 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TList.h"

#include "TMrbLogger.h"
#include "TMrbXMLCodeElem.h"

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

Bool_t TMrbXMLCodeElem::CheckName(const Char_t * ElemName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CheckName
// Purpose:        Compare name (of end tag) with element name
// Arguments:      Char_t * ElemName   -- element name
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString elemName = this->GetName();
	if (elemName.CompareTo(ElemName) != 0) {
		gMrbLog->Err() << "XML tag out of phase - <" << elemName << ">...</" << ElemName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckName");
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}


Bool_t TMrbXMLCodeElem::CheckParent(const Char_t * ParentName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CheckParent
// Purpose:        Check if we have the right parent
// Arguments:      Char_t * ParentName   -- name of parent element
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	if (fParent) {
		TString elemName = fParent->GetName();
		if (elemName.CompareTo(ParentName) != 0) {
			gMrbLog->Err() << "Wrong parent element - " << elemName << " (should be <" << ParentName << ">)" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckParent");
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << "There is no parent <" << ParentName << "> for root element <" << this->GetName() << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckParent");
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

	if (!this->CheckName(ElemName)) this->SetIndex(kMrbXmlZombie);

	Bool_t sts;

	if (this->GetIndex() != kMrbXmlZombie) {
		switch (this->GetIndex()) {
			case kMrbXml_MrbCode:	sts = this->ProcessElement_MrbCode(); break;
			case kMrbXml_File:		sts = this->ProcessElement_File(); break;
			case kMrbXml_FileHdr:	sts = this->ProcessElement_FileHdr(); break;
			case kMrbXml_Class: 	sts = this->ProcessElement_Class(); break;
			case kMrbXml_ClassHdr:	sts = this->ProcessElement_ClassHdr(); break;
			case kMrbXml_Ctor:		sts = this->ProcessElement_Ctor(); break;
			case kMrbXml_CtorHdr:	sts = this->ProcessElement_CtorHdr(); break;
			case kMrbXml_Method:	sts = this->ProcessElement_Method(); break;
			case kMrbXml_MethodHdr: sts = this->ProcessElement_MethodHdr(); break;
			case kMrbXml_Funct: 	sts = this->ProcessElement_Funct(); break;
			case kMrbXml_FunctHdr:	sts = this->ProcessElement_FunctHdr(); break;
			case kMrbXml_Code:		sts = this->ProcessElement_Code(); break;
			case kMrbXml_Cbody: 	sts = this->ProcessElement_Cbody(); break;
			case kMrbXml_If:		sts = this->ProcessElement_If(); break;
			case kMrbXml_Foreach:	sts = this->ProcessElement_Foreach(); break;
			case kMrbXml_Switch:	sts = this->ProcessElement_Switch(); break;
			case kMrbXml_Case:		sts = this->ProcessElement_Case(); break;
			case kMrbXml_Item:		sts = this->ProcessElement_Item(); break;
			case kMrbXml_Ftype: 	sts = this->ProcessElement_Ftype(); break;
			case kMrbXml_ArgList:	sts = this->ProcessElement_ArgList(); break;
			case kMrbXml_Arg:		sts = this->ProcessElement_Arg(); break;
			case kMrbXml_Atype: 	sts = this->ProcessElement_Atype(); break;
			case kMrbXml_ReturnVal: sts = this->ProcessElement_ReturnVal(); break;
			case kMrbXml_Comment:	sts = this->ProcessElement_Comment(); break;
			case kMrbXml_RootClassList: sts = this->ProcessElement_RootClassList(); break;
			case kMrbXml_MrbClassList:	sts = this->ProcessElement_MrbClassList(); break;
			case kMrbXml_Inheritance:	sts = this->ProcessElement_Inheritance(); break;
			case kMrbXml_ClassRef:	sts = this->ProcessElement_ClassRef(); break;
			case kMrbXml_Tag:		sts = this->ProcessElement_Tag(); break;
			case kMrbXml_Slist: 	sts = this->ProcessElement_Slist(); break;
			case kMrbXml_Subst: 	sts = this->ProcessElement_Subst(); break;
			case kMrbXml_Name:		sts = this->ProcessElement_Name(); break;
			case kMrbXml_Gname: 	sts = this->ProcessElement_Gname(); break;
			case kMrbXml_Cname: 	sts = this->ProcessElement_Cname(); break;
			case kMrbXml_Vname: 	sts = this->ProcessElement_Vname(); break;
			case kMrbXml_Sname: 	sts = this->ProcessElement_Sname(); break;
			case kMrbXml_Purp:		sts = this->ProcessElement_Purp(); break;
			case kMrbXml_Descr: 	sts = this->ProcessElement_Descr(); break;
			case kMrbXml_Author:	sts = this->ProcessElement_Author(); break;
			case kMrbXml_Mail:		sts = this->ProcessElement_Mail(); break;
			case kMrbXml_Url:		sts = this->ProcessElement_Url(); break;
			case kMrbXml_Version:	sts = this->ProcessElement_Version(); break;
			case kMrbXml_Date:		sts = this->ProcessElement_Date(); break;
			case kMrbXml_s: 		sts = this->ProcessElement_s(); break;
			case kMrbXml_S: 		sts = this->ProcessElement_S(); break;
			case kMrbXml_L: 		sts = this->ProcessElement_L(); break;
		}
		return(kTRUE);
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_MrbCode() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_File() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_FileHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Class() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ClassHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ctor() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_CtorHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Method() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_MethodHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Funct() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_FunctHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Code() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Cbody() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_If() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Foreach() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Switch() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Case() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Item() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ftype() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ArgList() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Arg() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Atype() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ReturnVal() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Comment() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_RootClassList() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_MrbClassList() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Inheritance() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ClassRef() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Tag() { return(kTRUE); };

Bool_t TMrbXMLCodeElem::ProcessElement_Slist() { return(kTRUE); };

Bool_t TMrbXMLCodeElem::ProcessElement_Subst() {
	fParent->LofSubst()->SetValue(fCode, "");
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Name() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Gname() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Cname() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Vname() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Sname() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Purp() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Descr() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Author() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Mail() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Url() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Version() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Date() {
	fParent->LofChildren()->SetValue(this->GetName(), fCode);
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_s() {
	fParent->AddCode(Form("$%s", this->GetName()));
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_S() {
	TString n = this->GetName();
	n(0,1).ToUpper();
	fParent->AddCode(Form("$%s", n.Data()));
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_L() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_L
// Purpose:        Line of code: copy code + newline
// Arguments:      --
// Results:        kTRUE
//////////////////////////////////////////////////////////////////////////////

	fCode += "\n";
	this->CopyCodeToParent();
	return(kTRUE);
};

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements which act as decorations in HTML, plain code copy here
// Arguments:      --
// Results:        kTRUE
// Description:    <b>...</b>      boldface
//                 <i> ... </i>    italic
//                 <u> ... </u>    underline
//                 <bx> ... </bx>  boxed
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_B() {
	this->CopyCodeToParent();
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_I() {
	this->CopyCodeToParent();
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_U() {
	this->CopyCodeToParent();
	return(kTRUE);
};

Bool_t TMrbXMLCodeElem::ProcessElement_Bx() {
	this->CopyCodeToParent();
	return(kTRUE);
};

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements which have meaning for HTML only, NOP here
// Arguments:      --
// Results:        kTRUE
// Description:    <m>...</m>      MARaBOU class link
//                 <mm> ... </mm>  MARaBOU method link
//                 <r> ... </r>    ROOT class link
//                 <rm> ... </rm>  ROOT method link
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_M() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mm() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_R() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Rm() { return(kTRUE); };
