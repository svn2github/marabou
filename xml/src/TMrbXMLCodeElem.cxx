//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/src/TMrbXMLCodeElem.cxx
// Purpose:        MARaBOU XML based code generator: code element
// Description:    Implements class methods to process xml data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXMLCodeElem.cxx,v 1.10 2008-03-05 12:23:44 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TMrbLogger.h"
#include "TMrbSystem.h"
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
	if (fParser)	fVerboseMode = fParser->IsVerbose();
	else			fVerboseMode = gEnv->GetValue("TMrbXML.VerboseMode", kFALSE);
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
// Arguments:      Char_t * ElemName     -- element name
//                 Bool_t Verbose        -- output error if kTRUE
// Results:        kTRUE/kFALSE
//////////////////////////////////////////////////////////////////////////////

	TString elemName = this->GetName();
	if (elemName.CompareTo(ElemName) != 0) {
		if (Verbose) {
			gMrbLog->Err() << this->MsgHdr() << "Wrong element (should be <" << ElemName << ">)" << endl;
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
			gMrbLog->Err() << this->MsgHdr() << "Wrong parent element - <" << pname << "> (should be one of these: " << LofParents << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ParentIs");
		}
		return(kFALSE);
	} else {
		if (Verbose) {
			gMrbLog->Err() << this->MsgHdr() << "No parent element found" << endl;
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
			gMrbLog->Err() << this->MsgHdr() << "Child element missing - <" << ChildName << ">" << endl;
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
			gMrbLog->Err() << this->MsgHdr() << "Child element missing - <" << ChildName << ">" << endl;
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
			gMrbLog->Err() << this->MsgHdr() << "No ancestor <" << Ancestor << "> found" << endl;
			gMrbLog->Flush(this->ClassName(), "HasAncestor");
		}
		return(NULL);
	} else {
		if (Verbose) {
			gMrbLog->Err() << this->MsgHdr() << "No parent element found" << endl;
			gMrbLog->Flush(this->ClassName(), "HasAncestor");
		}
		return(NULL);
	}
}

Bool_t TMrbXMLCodeElem::CopyCodeToAncestor(TMrbXMLCodeElem * Ancestor, const Char_t * IndentString) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopyCodeToAncestor
// Purpose:        Copy code to ancestor node
// Arguments:      TMrbXMLCodeElem * Ancestor   -- ancestor
//                 Char_t * IndentString        -- string to be prepended
// Results:        kTRUE/kFALSE
// Description:    Copies code to ancestor element
//////////////////////////////////////////////////////////////////////////////

	if (Ancestor) {
		if (IndentString == NULL || *IndentString == '\0') {
			Ancestor->AddCode(fCode);
		} else {
			TString line;
			Int_t from = 0;
			while (fCode.Tokenize(line, from, "\n")) {
				if (line.IsNull()) {
					line = "\n";
				} else {
					if (line.BeginsWith("^")) line = line(1, line.Length());
					else if (!line.BeginsWith("//")) line.Prepend(IndentString);
				}
				line += "\n";
				Ancestor->AddCode(line);
			}
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << this->MsgHdr() << "No ancestor given" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyCodeToAncestor");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::CopyChildToAncestor(TMrbXMLCodeElem * Ancestor, const Char_t * ChildName, const Char_t * ChildCode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopyChildToAncestor
// Purpose:        Xfer child to ancestor element
// Arguments:      TMrbXMLCodeElem * Ancestor   -- ancestor
//                 Char_t * ChildName           -- child element to be copied
//                 Char_t * ChildCode           -- code
// Results:        kTRUE/kFALSE
// Description:    Copies child code to ancestor element:
//                 If no 'ChildCode' is given xfers child 'ChildName'
//                 to list of parent childs.
//                 If 'ChildCode' is present creates a new child in the
//                 ancestor's list of childs
//////////////////////////////////////////////////////////////////////////////

	if (Ancestor) {
		TString childCode;
		if (ChildCode == NULL) {
			if (!this->HasChild(ChildName, childCode)) return(kFALSE);
		} else {
			childCode = ChildCode;
		}
		Ancestor->LofChildren()->SetValue(ChildName, childCode);
		return(kTRUE);
	} else {
		gMrbLog->Err() << this->MsgHdr() << "No ancestor element given" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyChildToAncestor");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::CopySubstToAncestor(TMrbXMLCodeElem * Ancestor, const Char_t * Sname, const Char_t * Descr, const Char_t * Tag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::CopySubstToAncestor
// Purpose:        Copy substitutions to ancestor node
// Arguments:      TMrbXMLCodeElem * Ancestor   -- ancestor
//                 Char_t * Sname               -- name of subst
//                 Char_t * Descr               -- description
//                 Char_t * tag                 -- associated tag
// Results:        kTRUE/kFALSE
// Description:    Copies substitutions (<subst>...</subst>) to ancestor node
//////////////////////////////////////////////////////////////////////////////

	if (Ancestor) {
		if (Sname != NULL) {
			Ancestor->LofSubst()->SetValue(Form("%s.Value", Sname), "");
			Ancestor->LofSubst()->SetValue(Form("%s.Descr", Sname), (Descr == NULL) ? "" : Descr);
			Ancestor->LofSubst()->SetValue(Form("%s.Tag", Sname), Tag);
		} else {
			TList * lsubst = (TList *) fLofSubst.GetTable();
			if (lsubst) {
				TIterator * iter = lsubst->MakeIterator();
				TEnvRec * r;
				while (r = (TEnvRec *) iter->Next()) Ancestor->LofSubst()->SetValue(r->GetName(), r->GetValue());
			}
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << this->MsgHdr() << "No ancestor element given" << endl;
		gMrbLog->Flush(this->ClassName(), "CopySubstToAncestor");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::GetChildFromAncestor(TMrbXMLCodeElem * Ancestor, const Char_t * ElemName, TString & ElemCode, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::GetChildFromAncestor
// Purpose:        Get a child element from ancestor
// Arguments:      Char_t * ElemName  -- element name
//                 TString & ElemCode -- code
// Results:        kTRUE/kFALSE
// Description:    Passes a element from ancestor to child
//////////////////////////////////////////////////////////////////////////////

	ElemCode = "";
	if (Ancestor) {
		ElemCode = Ancestor->LofChildren()->GetValue(ElemName, "");
		if (ElemCode.IsNull()) {
			if (Verbose) {
				gMrbLog->Err()	<< this->MsgHdr() << "Couldn't get element <" << ElemName
								<< "> from ancestor <" << Ancestor->GetName() << ">" << endl;
				gMrbLog->Flush(this->ClassName(), "GetChildFromAncestor");
			}
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err() << this->MsgHdr() << "No ancestor element given" << endl;
		gMrbLog->Flush(this->ClassName(), "GetChildFromAncestor");
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::FindTag(TString & Tag, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::FindTag
// Purpose:        Search for a tag
// Arguments:      TString & Tag  -- tag name
// Results:        kTRUE/kFALSE
// Description:    Searches for a given tag thru the list of ancestors
//////////////////////////////////////////////////////////////////////////////

	Tag = "";

	if (this->HasChild("tag", Tag, kFALSE)) return(kTRUE);

	if (fParent) {
		TMrbXMLCodeElem * e = fParent;
		while (e) {
			Tag = e->LofChildren()->GetValue("tag", "");
			if (!Tag.IsNull()) return(kTRUE);
			e = e->Parent();
		}
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No tag found, neither as child nor in list of ancestors" << endl;
			gMrbLog->Flush(this->ClassName(), "FindTag");
		}
		return(kFALSE);
	} else {
		if (Verbose) {
			gMrbLog->Err() << "[" << this->GetName() << "] No parent element found" << endl;
			gMrbLog->Flush(this->ClassName(), "FindTag");
		}
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::FindSubst(const Char_t * Sname, TString & Svalue, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::FindSubst
// Purpose:        Search for a given substitution
// Arguments:      Char_t * Sname    -- subst name
//                 TString & Svalue  -- value
// Results:        kTRUE/kFALSE
// Description:    Searches for a given substitution
//////////////////////////////////////////////////////////////////////////////

	Svalue = "";
	TList * lsubst = (TList *) fLofSubst.GetTable();
	if (lsubst) {
		TIterator * iter = lsubst->MakeIterator();
		TEnvRec * r;
		TString sn = Form("%s.Value", Sname);
		while (r = (TEnvRec *) iter->Next()) {
			TString s = r->GetName();
			if (s.CompareTo(sn.Data()) == 0) {
				Svalue = r->GetValue();
				return(kTRUE);
			}
		}
	}
	if (Verbose) {
		gMrbLog->Err() << "[" << this->GetName() << "] No such subst - " << Sname << endl;
		gMrbLog->Flush(this->ClassName(), "FindSubst");
	}
	return(kFALSE);
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
		while (r = (TEnvRec *) iter->Next()) {
			TString rname = r->GetName();
			if (rname(0) != '@') fLofSubst.SetValue(r->GetName(), "");
		}
	}
}

Bool_t TMrbXMLCodeElem::Substitute(TString & Code, TEnv * LofSubst, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::Substitute
// Purpose:        Replace substitutions
// Arguments:      TString & Code   -- code containing substitutions
//                 TEnv * LofSubst  -- list of substitutions
//                 Bool_t Verbose   -- verbose flag
// Results:        kTRUE/kFALSE
// Description:    Searches for string '@#@' and replaces it
//                 by entry value found in substitution table
//                 @#@switch|...@ and @#@if|...@ will be handled separately
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts = kTRUE;
	Int_t idx1 = 0;

	Int_t hdrl = strlen("@#@");
	while ((idx1 = Code.Index("@#@", idx1)) >= 0) {
		idx1 += hdrl;
		Int_t idx2 = Code.Index("@", idx1);
		if (idx2 < 0) {
			gMrbLog->Err() << this->MsgHdr() << "Missing '@' at char pos " << idx1 << "+N - cant' resolve \"@#@|...@\"" << endl;
			gMrbLog->Flush(this->ClassName(), "Substitute");
			sts = kFALSE;
		} else {
			TString subst = Code(idx1, idx2 - idx1);
			Int_t idx3 = subst.Index("|", 0);
			if (idx3 < 0) {
				gMrbLog->Err() << this->MsgHdr() << "Missing '|' at char pos " << idx1 << "+N - cant' resolve \"@#@|...@\"" << endl;
				gMrbLog->Flush(this->ClassName(), "Substitute");
				sts = kFALSE;
			} else {
				TString attr = subst(0, idx3);
				idx3 += strlen("|");
				TString sname = subst(idx3, subst.Length() - idx3);
				TString res = Form("%s.Value", sname.Data());
				TString sval = LofSubst ? LofSubst->GetValue(res.Data(), "undef") : fLofSubst.GetValue(res.Data(), "undef");
				sval = sval.Strip(TString::kBoth);
				if (sval.CompareTo("undef") == 0) {
					if (Verbose) {
						gMrbLog->Err() << this->MsgHdr() << "No such subst - " << sname << " (attr=" << attr << ")" << endl;
						gMrbLog->Flush(this->ClassName(), "Substitute");
						sts = kFALSE;
					}
				} else {
					if (attr.CompareTo("switch") == 0 || attr.CompareTo("if") == 0) {
						Code(idx1 + idx3, idx2 - idx3 - idx1) = Form("%s|%s", sname.Data(), sval.Data());
					} else if (attr.CompareTo("S") == 0) {
						if (sval.IsNull()) {
							Code.Remove(idx1 - hdrl, idx2 - idx1 + hdrl + 1);
						} else {
							sval(0,1).ToUpper();
							Code(idx1 - hdrl, idx2 - idx1 + hdrl + 1) = sval;
						}
					} else if (attr.CompareTo("s") == 0) {
						if (sval.IsNull()) {
							Code.Remove(idx1 - hdrl, idx2 - idx1 + hdrl + 1);
						} else {
							Code(idx1 - hdrl, idx2 - idx1 + hdrl + 1) = sval;
						}
					}
					idx1 -= hdrl;
				}
			}
		}
	}
	return(sts);
}

Bool_t TMrbXMLCodeElem::FindAttr(const Char_t * Aname, TString & Avalue, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::FindAttr
// Purpose:        Search for a given attribute
// Arguments:      Char_t * Aname    -- attr name
//                 TString & Avalue  -- value
// Results:        kTRUE/kFALSE
// Description:    Searches for a given attribute
//////////////////////////////////////////////////////////////////////////////

	Avalue = "";
	TList * lattr = (TList *) fLofAttr.GetTable();
	if (lattr) {
		TIterator * iter = lattr->MakeIterator();
		TEnvRec * r;
		while (r = (TEnvRec *) iter->Next()) {
			TString s = r->GetName();
			if (s.CompareTo(Aname) == 0) {
				Avalue = r->GetValue();
				return(kTRUE);
			}
		}
	}
	if (Verbose) {
		gMrbLog->Err() << "[" << this->GetName() << "] No such attribute - " << Aname << endl;
		gMrbLog->Flush(this->ClassName(), "FindAttr");
	}
	return(kFALSE);
}

Bool_t TMrbXMLCodeElem::ExpandSwitch(TString & Code) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ExpandSwitch
// Purpose:        Expand <switch> inserts
// Arguments:      TString & Code   -- code to be scanned
// Results:        kTRUE/kFALSE
// Description:    Searches for '@#@switch|...' and substitutes <case> code.
//////////////////////////////////////////////////////////////////////////////

	if (!this->NameIs("foreach")) return(kFALSE);

	Bool_t sts = kTRUE;
	Int_t idx1 = 0;
	while ((idx1 = Code.Index("@#@switch|", idx1)) >= 0) {
		idx1 += sizeof("@#@switch|") - 1;
		Int_t idx2 = Code.Index("@", idx1);
		if (idx2 < 0) {
			gMrbLog->Err() << "[" << this->GetName() << "] Missing '@' at char pos " << idx1 << "+N - cant' resolve \"@#@switch|...@\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ExpandSwitch");
			sts = kFALSE;
		} else {
			TString cname = Code(idx1, idx2 - idx1);
			Int_t idx3 = cname.Index("|", 0);
			if (idx3 == -1) {
				gMrbLog->Err() << "[" << this->GetName() << "] Missing '|' in switch statement - " << cname << endl;
				gMrbLog->Flush(this->ClassName(), "ExpandSwitch");
				sts = kFALSE;
			} else {
				TString cval = cname(idx3 + 1, cname.Length() - idx3);
				cname = cname(0, idx3);
				TString child = Form("case_%s_%s", cname.Data(), cval.Data());
				TString ccode = fLofChildren.GetValue(child.Data(), "");
				if (ccode.IsNull()) {
					gMrbLog->Err() << "[" << this->GetName() << "] Can't resolve case - case_" << cname << "_" << cval << endl;
					gMrbLog->Flush(this->ClassName(), "ExpandSwitch");
					sts = kFALSE;
					ccode = Form(">>> %s::ExpandSwitch(): unresolved - %s <<<", this->ClassName(), child.Data());
				}
				Code(idx1 - sizeof("@#@switch|") + 1, idx2 - idx1 + sizeof("@#@switch|")) = ccode;
			}
		}
	}
	return(sts);
}

Bool_t TMrbXMLCodeElem::ExpandIf(TString & Code) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ExpandIf
// Purpose:        Expand <if> inserts
// Arguments:      TString & Code   -- code to be scanned
// Results:        kTRUE/kFALSE
// Description:    Searches for '@#@if|...' and substitutes <flag> code.
//////////////////////////////////////////////////////////////////////////////

	if (!this->NameIs("foreach")) return(kFALSE);

	Bool_t sts = kTRUE;
	Int_t idx1 = 0;
	while ((idx1 = Code.Index("@#@if|", idx1)) >= 0) {
		idx1 += sizeof("@#@if|") - 1;
		Int_t idx2 = Code.Index("@", idx1);
		if (idx2 < 0) {
			gMrbLog->Err() << "[" << this->GetName() << "] Missing '@' at char pos " << idx1 << "+N - cant' resolve \"@#@if|...@\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ExpandIf");
			sts = kFALSE;
		} else {
			TString cname = Code(idx1, idx2 - idx1);
			Int_t idx3 = cname.Index("|", 0);
			if (idx3 == -1) {
				gMrbLog->Err() << "[" << this->GetName() << "] Missing '|' in if statement - " << cname << endl;
				gMrbLog->Flush(this->ClassName(), "ExpandIf");
				sts = kFALSE;
			} else {
				TString cval = cname(idx3 + 1, cname.Length() - idx3);
				cname = cname(0, idx3);
				TString child = Form("if_%s_%s", cname.Data(), cval.Data());
				TString ccode = fLofChildren.GetValue(child.Data(), "");
				if (ccode.IsNull()) {
					TString child0 = Form("if_%s_0", cname.Data());
					if (child.CompareTo(child0.Data()) != 0) {
						gMrbLog->Err() << "[" << this->GetName() << "] Can't resolve flag - if_" << cname << "_" << cval << endl;
						gMrbLog->Flush(this->ClassName(), "ExpandIf");
						sts = kFALSE;
						ccode = Form(">>> %s::ExpandIf(): unresolved - %s <<<", this->ClassName(), child.Data());
					}
				}
				Code(idx1 - sizeof("@#@if|") + 1, idx2 - idx1 + sizeof("@#@if|")) = ccode;
			}
		}
	}
	return(sts);
}

Bool_t TMrbXMLCodeElem::ExpandInclude(const Char_t * Tag, const Char_t * ItemName, const Char_t * Item, TString & Code) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ExpandInclude
// Purpose:        Expand <include> inserts
// Arguments:      Char_t * Tag      -- tag
//                 Char_t * Item     -- item name
//                 TString & Code    -- code to be scanned
// Results:        kTRUE/kFALSE
// Description:    Searches for '@#@include|...' and substitutes <item> code.
//////////////////////////////////////////////////////////////////////////////

	if (!this->NameIs("foreach")) return(kFALSE);

	Int_t hdrl = strlen("@#@include|");
	Bool_t sts = kTRUE;
	Int_t idx1 = 0;
	while ((idx1 = Code.Index("@#@include|", idx1)) >= 0) {
		cout << "@@@0 idx1=" << idx1 << " code=" << Code << endl;
		idx1 += hdrl;
		Int_t idx2 = Code.Index("@", idx1);
		if (idx2 < 0) {
			gMrbLog->Err() << "[" << this->GetName() << "] Missing '@' at char pos " << idx1 << "+N - cant' resolve \"@#@include|...@\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ExpandInclude");
			sts = kFALSE;
		} else {
			TString code = "";
			TString item = Code(idx1, idx2 - idx1);
			cout << "@@@0 idx2=" << idx2 << " item=" << item << endl;
			Int_t idx3 = item.Index("|", 0);
			getchar();
			if (idx3 < 0) {
				gMrbLog->Err() << this->MsgHdr() << "Missing '|' at char pos " << idx1 << "+N - cant' resolve \"@#@include|...@\"" << endl;
				gMrbLog->Flush(this->ClassName(), "ExpandInclude");
				sts = kFALSE;
			} else {
				idx3 += strlen("|");
				TString codeFile = item(idx3, item.Length() - idx3);
				cout << "@@@ " << codeFile << " " << Tag << endl;
				getchar();
				gSystem->ExpandPathName(codeFile);
				TString xmlPath = gEnv->GetValue("TMrbConfig.XMLPath", ".:config:$(MARABOU)/xml/config");
				gSystem->ExpandPathName(xmlPath);

				TString xmlFile = "";
				TMrbSystem ux;
				ux.Which(xmlFile, xmlPath.Data(), codeFile.Data());
				if (xmlFile.IsNull()) {
					gMrbLog->Err() << this->MsgHdr() << "XML file not found - " << codeFile << endl;
					gMrbLog->Flush(this->ClassName(), "ExpandInclude");
					gMrbLog->Err()	<< "Searching on path " << xmlPath << endl;
					gMrbLog->Flush(this->ClassName(), "ExpandInclude", setred, kTRUE);
				} else {
					TMrbXMLCodeGen * codeGen = new TMrbXMLCodeGen(xmlFile.Data(), fParser->Client(), Tag, &fLofSubst, this->IsVerbose());
					if (codeGen->IsZombie()) {
						gMrbLog->Err() << this->MsgHdr() << "Can't process XML file - " << codeFile << endl;
						gMrbLog->Flush("TMrbConfig", "ExpandInclude");
						sts = kFALSE;
					} else {
						TString code = codeGen->GetCode();
					}
					delete codeGen;
					codeGen = NULL;
				}
			}
			if (code.IsNull()) {
				Code.Remove(idx1 - hdrl, idx2 - idx1 + hdrl + 1);
			} else {
				Code(idx1 - hdrl, idx2 - idx1 + hdrl + 1) = code;
			}
		}
		cout << "@@@1 code=|" << Code << "|" << endl;
		getchar();
	}
 	return(sts);
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

Bool_t TMrbXMLCodeElem::RequestSubst(const Char_t * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::RequestSubst
// Purpose:        Ask client for a list of substitutions
// Arguments:      Char_t * Tag    -- current tag
//                 Char_t * ItemName  -- name of item
//                 Char_t * ItemData  -- item value
//                 TEnv * LofSubst    -- list of substitutions
// Results:        kTRUE/kFALSE
// Description:    Interface to client object: Call for substitutions
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		TMrbXMLCodeClient * client = fParser->Client();
		if (client) {
			LofSubst->SetValue("@Request.OK", kFALSE);
			return(client->ProvideSubst(this->GetName(), Tag, ItemName, ItemData, LofSubst));
		} else {
			gMrbLog->Err()	<< "[" << this->GetName() << "] No client connected - can't get substitutions for tag=\""
							<< Tag << "\", item \"" << ItemName << "=" << ItemData << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "RequestSubst");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err()	<< "[" << this->GetName() << "] No XML parser connected - can't get substitutions for tag=\""
						<< Tag << "\", item \"" << ItemName << "=" << ItemData << "\"" << endl;
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

Bool_t TMrbXMLCodeElem::RequestTask(const Char_t * Tag, const Char_t * Subtag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::RequestTask
// Purpose:        Request task execution
// Arguments:      Char_t * Tag             -- current tag
//                 Char_t * Subtag          -- subtag
// Results:        kTRUE/kFALSE
// Description:    Interface to client object: Execute task
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		TMrbXMLCodeClient * client = fParser->Client();
		if (client) {
			return(client->ExecuteTask(this->GetName(), Tag, Subtag));
		} else {
			gMrbLog->Err()	<< "[" << this->GetName() << "] No client connected - can't execute task for tag=\""
							<< Tag << "\", subtag=\"" << Subtag << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "RequestTask");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err()	<< "[" << this->GetName() << "] No XML parser connected - can't execute task for tag=\""
							<< Tag << "\", subtag=\"" << Subtag << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "RequestTask");
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

	if (!this->NameIs(ElemName)) {
		gMrbLog->Err() << "[" << this->GetName() << "] XML tag out of phase - </" << ElemName << ">" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement");
		this->MakeZombie();
	}

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
			case kMrbXml_Include: 		sts = this->ProcessElement_Include(); break;
			case kMrbXml_If:			sts = this->ProcessElement_If(); break;
			case kMrbXml_Foreach:		sts = this->ProcessElement_Foreach(); break;
			case kMrbXml_Switch:		sts = this->ProcessElement_Switch(); break;
			case kMrbXml_Case:			sts = this->ProcessElement_Case(); break;
			case kMrbXml_Task:			sts = this->ProcessElement_Task(); break;
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
			case kMrbXml_Subtag:		sts = this->ProcessElement_Subtag(); break;
			case kMrbXml_Flag:			sts = this->ProcessElement_Flag(); break;
			case kMrbXml_FlagValue:		sts = this->ProcessElement_FlagValue(); break;
			case kMrbXml_Fpath:			sts = this->ProcessElement_Fpath(); break;
			case kMrbXml_Slist: 		sts = this->ProcessElement_Slist(); break;
			case kMrbXml_Subst: 		sts = this->ProcessElement_Subst(); break;
			case kMrbXml_Clist: 		sts = this->ProcessElement_Clist(); break;
			case kMrbXml_Content: 		sts = this->ProcessElement_Content(); break;
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
			case kMrbXml_B: 			sts = this->ProcessElement_B(); break;
			case kMrbXml_I: 			sts = this->ProcessElement_I(); break;
			case kMrbXml_U: 			sts = this->ProcessElement_U(); break;
			case kMrbXml_Bx: 			sts = this->ProcessElement_Bx(); break;
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
	if (this->IsVerbose() && this->HasChild("xname", xname, kFALSE)) {
		gMrbLog->Out() << "[" << this->GetName() << "] XML data parsed from file " << xname << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_MrbCode");
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

	if (!this->ParentIs("mrbcode")) return(kFALSE);

	TString gname;
	if (!this->HasChild("gname", gname)) return(kFALSE);

	TString tag;
	if (!this->HasChild("tag", tag)) return(kFALSE);

	if (!this->CopyChildToAncestor(fParent, "xname")) return(kFALSE);

	Bool_t err = !this->RequestSubst(tag.Data(), NULL, NULL, this->LofSubst());
	if (this->SubstOk()) {
		err |= !this->Substitute(gname, NULL, kTRUE);
		err |= !this->Substitute(fCode, NULL, kTRUE);
	}

	ofstream file(gname.Data(), ios::out);
	if (!file.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << gname << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_File");
		return(kFALSE);
	}
	file << fCode << endl;					// write code to file
	file.close();

	this->CopyCodeToAncestor(fParent);		// pass code to root element, too

	if (this->IsVerbose()) {
		gMrbLog->Out() << "[" << this->GetName() << "] Generated code written to file " << gname << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessElement_File");
	}
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

	this->CopyChildToAncestor(fParent, "xname");
	this->CopyChildToAncestor(fParent, "gname");
	fCode =  "//___________________________________________________________[C++ FILE HEADER]\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n";
	fCode += "// Name:           "; fCode += fLofChildren.GetValue("gname", ""); fCode += "\n";
	fCode += "// Purpose:        "; fCode += fLofChildren.GetValue("purp", ""); fCode += "\n";
	TString hdrLine;
	fCode += this->FormatHeaderLine(hdrLine, "// Description:    ", fLofChildren.GetValue("descr", ""));
	fCode += "// Author:         "; fCode += fLofChildren.GetValue("author", ""); fCode += "\n";
	fCode += "// Mail:           "; fCode += fLofChildren.GetValue("mail", ""); fCode += "\n";
	fCode += "// Revision:       "; fCode += fLofChildren.GetValue("version", ""); fCode += "\n";
	fCode += "// Date:           "; fCode += fLofChildren.GetValue("date", ""); fCode += "\n";
	fCode += "// URL:            "; fCode += fLofChildren.GetValue("url", ""); fCode += "\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n\n";
	Bool_t err = !this->CopyCodeToAncestor(fParent);
	err |= !this->CopySubstToAncestor(fParent);
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Method() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Method
// Purpose:        C++ method: <method>...</method>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Generates code for a C++ method
//////////////////////////////////////////////////////////////////////////////

	TString tag;
	Bool_t err = kFALSE;
	if (this->HasChild("tag", tag, kFALSE)) {
		this->ClearSubst();

		if (fParser && fParser->SelectiveMode()) {
			if (this->SubstOk(this->Parser()->LofSubst())) {
				err |= !this->Substitute(fCode, this->Parser()->LofSubst());
				if (tag.CompareTo(fParser->SelectionTag()) == 0) {
					fParser->StopParsing();
					fParser->FoundMatchingTag();
				} else {
					return(!err);
				}
			} else if (tag.CompareTo(fParser->SelectionTag()) == 0) {
				fParser->StopParsing();
				fParser->FoundMatchingTag();
				return(!err);
			}
		} else {
			err |= !this->RequestSubst(tag.Data(), NULL, NULL, this->LofSubst());
			if (!this->SubstOk()) return(kTRUE);
		}
	}

	err |= !this->CopyCodeToAncestor(fParent, "\t");
	fCode = "}";
	this->CopyCodeToAncestor(fParent);
	return(!err);
};

Bool_t TMrbXMLCodeElem::ProcessElement_MethodHdr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_MethodHdr
// Purpose:        File statement: <methodHdr>...</methodHdr>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Creates a standard c++ method header
// Parent elems:   <method>      -- c++ class method
// Child elems:    <mname>       -- name of method
//                 <cname>       -- class name
//                 <purp>        -- purpose
//                 <argList>     -- list of arguments
//                 <returnVal>   -- return value
//                 <descr>       -- description
//////////////////////////////////////////////////////////////////////////////

	if (!this->ParentIs("method")) return(kFALSE);

	this->CopyChildToAncestor(fParent, "mname");
	this->CopyChildToAncestor(fParent, "cname");


	fCode = "\n^";
	TString str = fLofChildren.GetValue("returnVal", "");
	if (str.IsNull())	fCode += "void";
	else				fCode += str(0, str.Index("|", 0));
	fCode += Form(" %s::%s(", fLofChildren.GetValue("cname", ""), fLofChildren.GetValue("mname", ""));

	TString argList = fLofChildren.GetValue("argList", "");
	Int_t argn = 0;
	Int_t from = 0;
	while (argList.Tokenize(str, from, ":")) {
		TObjArray * a = str.Tokenize("|");
		Int_t n = a->GetEntriesFast();
		TString atype = (n >= 1) ? ((TObjString *) a->At(0))->GetString() : "";
		TString aname = (n >= 2) ? ((TObjString *) a->At(1))->GetString() : "";
		TString adescr = (n >= 3) ? ((TObjString *) a->At(2))->GetString() : "";
		if (argn > 0) fCode += ", ";
		fCode += Form("%s %s", atype.Data(), aname.Data());
		delete a;
	}
	fCode += ") {\n";

	fCode +=  "//________________________________________________________________[C++ METHOD]\n";
	fCode += "//////////////////////////////////////////////////////////////////////////////\n";
	fCode += "// Name:           "; fCode += fLofChildren.GetValue("cname", ""); fCode += "::";
									fCode += fLofChildren.GetValue("mname", ""); fCode += "\n";
	fCode += "// Purpose:        "; fCode += fLofChildren.GetValue("purp", ""); fCode += "\n";

	argList = fLofChildren.GetValue("argList", "");
	TString head = "// Arguments:      ";
	from = 0;
	while (argList.Tokenize(str, from, ":")) {
		TObjArray * a = str.Tokenize("|");
		Int_t n = a->GetEntriesFast();
		TString atype = (n >= 1) ? ((TObjString *) a->At(0))->GetString() : "";
		TString aname = (n >= 2) ? ((TObjString *) a->At(1))->GetString() : "";
		TString adescr = (n >= 3) ? ((TObjString *) a->At(2))->GetString() : "";
		aname = Form("%s %s", atype.Data(), aname.Data());
		fCode += Form("%s%-20s -- %s\n", head.Data(), aname.Data(), adescr.Data());
		head = "                   ";
		delete a;
	}

	head = "// Results:        ";
	str = fLofChildren.GetValue("returnVal", "");
	if (str.EndsWith(":")) str = str(0, str.Length() - 1);
	TObjArray * a = str.Tokenize("|");
	Int_t n = a->GetEntriesFast();
	TString atype = (n >= 1) ? ((TObjString *) a->At(0))->GetString() : "";
	TString aname = (n >= 2) ? ((TObjString *) a->At(1))->GetString() : "";
	TString adescr = (n >= 3) ? ((TObjString *) a->At(2))->GetString() : "";
	aname = Form("%s %s", atype.Data(), aname.Data());
	fCode += Form("%s%-20s -- %s\n", head.Data(), aname.Data(), adescr.Data());
	delete a;

	TString hdrLine;
	fCode += this->FormatHeaderLine(hdrLine, "// Description:    ", fLofChildren.GetValue("descr", ""));
	fCode += "//////////////////////////////////////////////////////////////////////////////\n\n";

	Bool_t err = !this->CopyCodeToAncestor(fParent);
	err |= !this->CopySubstToAncestor(fParent);
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_Class() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_ClassHdr() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ctor() { return(kTRUE); };
Bool_t TMrbXMLCodeElem::ProcessElement_CtorHdr() { return(kTRUE); };
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

	Bool_t err = kFALSE;
	TString tag;
	if (this->HasChild("tag", tag, kFALSE)) {

		this->ClearSubst();

		if (fParser && fParser->SelectiveMode()) {
			if (this->SubstOk(this->Parser()->LofSubst())) {
				err |= !this->Substitute(fCode, this->Parser()->LofSubst());
				if (tag.CompareTo(fParser->SelectionTag()) == 0) {
					fParser->StopParsing();
					fParser->FoundMatchingTag();
				} else {
					return(!err);
				}
			} else if (tag.CompareTo(fParser->SelectionTag())) {
				fParser->StopParsing();
				fParser->FoundMatchingTag();
				return(!err);
			}
		} else {
			err |= !this->RequestSubst(tag.Data(), NULL, NULL, this->LofSubst());
			if (this->SubstOk())	err |= !this->Substitute(fCode);
			else					return(kTRUE);
		}
	}

	err |= !this->CopyCodeToAncestor(fParent);
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

	return(this->CopyCodeToAncestor(fParent));
}

Bool_t TMrbXMLCodeElem::ProcessElement_Task() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Task
// Purpose:        Task execution: <task>...</task>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Execute external task
// Child elems:    <subtag>      -- (opt) identifying subtag
//                 <descr>       -- (opt) description
//////////////////////////////////////////////////////////////////////////////

	TString tag;
	if (!this->FindTag(tag)) return(kFALSE);

	TString subtag;
	this->HasChild("subtag", subtag, kFALSE);

	return(this->RequestTask(tag.Data(), subtag.Data()));
}

Bool_t TMrbXMLCodeElem::ProcessElement_Foreach() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Foreach
// Purpose:        Loop statement: <foreach>...</foreach>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Loops over list of <item>s and copies code to parent
//                 <comment> within <foreach> will be printed only once
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
	if (!this->FindTag(tag)) return(kFALSE);
	TString lofItems;
	if (!this->RequestLofItems(tag.Data(), itemName.Data(), lofItems)) return(kFALSE);
	TString origCode = fCode;
	Int_t from = 0;
	TString item;
	Bool_t err = kFALSE;
	Bool_t first = kTRUE;
	while (lofItems.Tokenize(item, from, ":")) {
		fLofSubst.SetValue("@Request.First", first);
		fCode = "";
		Int_t from = 0;
		TString line;
		while (origCode.Tokenize(line, from, "\n")) {
			if (line.BeginsWith("//*")) {
				if (!first) {
					if (fCode.EndsWith("\n")) fCode.Resize(fCode.Length() - 1);
					continue;
				}
				line(2) = ' ';
			}
			fCode += line;
			fCode += "\n";
		} 
		this->ClearSubst();
		err |= !this->RequestSubst(tag.Data(), itemName.Data(), item.Data(), this->LofSubst());
		if (this->SubstOk()) err |= !this->Substitute(fCode);
		err |= !this->ExpandSwitch(fCode);
		err |= !this->ExpandIf(fCode);
		err |= !this->ExpandInclude(tag.Data(), itemName.Data(), item.Data(), fCode);
		err |= !this->Substitute(fCode);
		err |= !this->CopyCodeToAncestor(fParent);
		first = kFALSE;
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
//                 copy all <case> elements to parent and add "@#@switch|item@ to code.
//                 If not, ask for an item value and look for a matching <case>
// Child elems:    <item>      -- item to be compared to <case> values
//////////////////////////////////////////////////////////////////////////////

	TString item;
	if (!this->HasChild("item", item)) return(kFALSE);
	TMrbXMLCodeElem * foreach;
	if (foreach = this->HasAncestor("foreach")) {
		TString subst;
		if (!foreach->FindSubst(item.Data(), subst)) {
			gMrbLog->Err() << "[" << this->GetName() << "] Need item value - " << item << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessElement_Switch");
			return(kFALSE);
		}
		TList * lchild = (TList *) fLofChildren.GetTable();
		if (lchild) {
			TIterator * iter = lchild->MakeIterator();
			TEnvRec * r;
			while (r = (TEnvRec *) iter->Next()) {
				TString rname = r->GetName();
				if (rname.BeginsWith("case")) this->CopyChildToAncestor(foreach, r->GetName());
			}
		}
		fCode = Form("@#@switch|%s@\n", item.Data());
	} else {
		TString tag;
		if (!this->FindTag(tag)) return(kFALSE);
		TString itemVal;
		if (!this->RequestLofItems(tag.Data(), item.Data(), itemVal)) return(kFALSE);
		if (!this->HasChild(Form("case_%s_%s", item.Data(), itemVal.Data()), fCode)) return(kFALSE);
	}
	Bool_t err = !this->CopyCodeToAncestor(fParent);
	err |= !this->CopySubstToAncestor(fParent);
	return(!err);
}

Bool_t TMrbXMLCodeElem::ProcessElement_If() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_If
// Purpose:        Condition statement: <if>...</if>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    If <if> is child of a <foreach> add "@#@if|flag# to code.
//                 If not, ask for a flag value and copy code to parent if TRUE
// Child elems:    <flag>      -- boolean flag to be checked
//////////////////////////////////////////////////////////////////////////////

	TString flag;
	if (!this->HasChild("flag", flag)) return(kFALSE);
	TString flagValue;
	if (!this->HasChild("flagValue", flagValue, kFALSE)) flagValue = "1";
	TMrbXMLCodeElem * foreach;
	if (foreach = this->HasAncestor("foreach")) {
		TString subst;
		if (!foreach->FindSubst(flag.Data(), subst)) {
			gMrbLog->Err() << "[" << this->GetName() << "] Need flag value - " << flag << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessElement_If");
			return(kFALSE);
		}
		this->CopyChildToAncestor(foreach, Form("if_%s_%s", flag.Data(), flagValue.Data()), fCode);
		fCode = Form("@#@if|%s@\n", flag.Data());
	} else {
		TString tag;
		if (!this->FindTag(tag)) return(kFALSE);
		TString val;
		if (!this->RequestConditionFlag(tag.Data(), flag.Data(), val)) return(kFALSE);
		if (val.CompareTo(flagValue.Data()) != 0) return(kFALSE);
	}
	Bool_t err = !this->CopyCodeToAncestor(fParent);
	err |= !this->CopySubstToAncestor(fParent);
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
	if (!this->GetChildFromAncestor(fParent, "item", itemName)) return(kFALSE);
	if (fCode.EndsWith("\n")) fCode.Resize(fCode.Length() - 1);
	Bool_t err = !this->CopyChildToAncestor(fParent, Form("case_%s_%s", itemName.Data(), caseTag.Data()), fCode);
	err |= !this->CopySubstToAncestor(fParent);
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
		TString descr;
		this->HasChild("descr", descr, kFALSE);
		fCode = Form("%s|%s|%s:", atype.Data(), aname.Data(), descr.Data());
		return(this->CopyCodeToAncestor(fParent));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Include() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Include
// Purpose:        Include statement: <include>...</include>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Includes a piece of code
// Child elems:    <fpath>      -- path to external file
//                 <tag>        -- tag
//                 <item>       -- item
//////////////////////////////////////////////////////////////////////////////

	TString fpath;
	if (!this->HasChild("fpath", fpath)) return(kFALSE);

	if (this->ParentIs("foreach", kFALSE)) {
		TString tag;
		if (!this->FindTag(tag)) return(kFALSE);
		TString item;
		if (!this->GetChildFromAncestor(fParent, "item", item)) return(kFALSE);
		fCode = Form("@#@include|%s|%s@\n", item.Data(), fpath.Data());
		return(this->CopyCodeToAncestor(fParent));
	} else {
		TString tag;
		Bool_t hasTag = this->HasChild("tag", tag, kFALSE);

		gSystem->ExpandPathName(fpath);
		TString xmlPath = gEnv->GetValue("TMrbConfig.XMLPath", ".:config:$(MARABOU)/xml/config");
		gSystem->ExpandPathName(xmlPath);

		TString xmlFile = "";
		TMrbSystem ux;
		ux.Which(xmlFile, xmlPath.Data(), fpath.Data());
		if (xmlFile.IsNull()) {
			gMrbLog->Err() << this->MsgHdr() << "XML file not found - " << fpath << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessElement_Include");
			gMrbLog->Err()	<< "Searching on path " << xmlPath << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessElement_Include", setred, kTRUE);
			return(kFALSE);
		}

		fCode = "";
		TMrbXMLCodeGen * codeGen;
		if (hasTag) {
			codeGen = new TMrbXMLCodeGen(xmlFile, fParser->Client(), tag.Data(), &fLofSubst, this->IsVerbose());
			if (!codeGen->TagMatched()) {
				gMrbLog->Err() << this->MsgHdr() << "No such tag - " << tag << " (file " << gSystem->BaseName(xmlFile.Data()) << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessElement_Include");
			}
		} else {
			codeGen = new TMrbXMLCodeGen(xmlFile, fParser->Client(), this->IsVerbose());
		}

		if (codeGen->IsZombie()) {
			gMrbLog->Err() << this->MsgHdr() << "Can't process XML file - " << xmlFile << endl;
			gMrbLog->Flush("TMrbConfig", "ProcessElement_Include");
			return(kFALSE);
		}

		fCode = codeGen->GetCode();
		delete codeGen;
		codeGen = NULL;
		return(this->CopyCodeToAncestor(fParent));
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
//                 Marks <comment> within <foreach> to be printed only once
//////////////////////////////////////////////////////////////////////////////

	TString type = fLofAttr.GetValue("type", "");
	TString cmt = this->HasAncestor("foreach", kFALSE) ? "\n//* " : "\n// ";
	if (type.CompareTo("code") == 0) {
		fCode.Prepend(cmt.Data());
		fCode += "\n";
		return(this->CopyCodeToAncestor(fParent));
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
		return(this->CopyCodeToAncestor(fParent));
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
// Parent elems:   <file>    -- file
//                 <class>   -- c++ class
//                 <ctor>    -- c++ constructor
//                 <method>  -- c++ method
//                 <funct>   -- function
//                 <includr> -- include code (from file)
//                 <foreach> -- loop
//                 <code>    -- tagged code block
//                 <case>    -- case statement within <switch>
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("file|class|ctor|method|funct|include|foreach|code|case")) {
		return(this->CopyChildToAncestor(fParent, this->GetName(), fCode));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Subtag() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Tag
// Purpose:        Tag: <tag>...</tag>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes tag to parent
// Parent elems:   <task>    -- execute external task
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("task")) {
		return(this->CopyChildToAncestor(fParent, this->GetName(), fCode));
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
		return(this->CopyChildToAncestor(fParent, this->GetName(), fCode));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_FlagValue() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_FlagValue
// Purpose:        Tag: <flagValue>...</flagValue>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Passes flag to parent
// Parent elems:   <if>    -- if clause
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("if")) {
		return(this->CopyChildToAncestor(fParent, this->GetName(), fCode));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Fpath() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Fpath
// Purpose:        Tag: <fpath>...</fpath>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Defines file path
// Parent elems:   <include>    -- include file
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("include")) {
		return(this->CopyChildToAncestor(fParent, this->GetName(), fCode));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Slist() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Slist
// Purpose:        Substitute: <slist>...</slist>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Copies own list of substitutions to parent
//////////////////////////////////////////////////////////////////////////////

	if (this->ParentIs("file|foreach|code")) {
		return(this->CopySubstToAncestor(fParent));
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbXMLCodeElem::ProcessElement_Subst() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Subst
// Purpose:        Substitute: <subst>...</subst>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Adds a substitution item to list of substitutions
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	if (!this->ParentIs("slist")) return(kFALSE);
	TString sname;
	if (!this->HasChild("sname", sname)) return(kFALSE);
	TString descr;
	this->HasChild("descr", descr, kFALSE);
	TString tag;
	this->FindTag(tag, kFALSE);
	return(this->CopySubstToAncestor(fParent, sname.Data(), descr.Data(), tag.Data()));
};

Bool_t TMrbXMLCodeElem::ProcessElement_Clist() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Clist
// Purpose:        List of contents: <clist>...</clist>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    --
// Parent elems:   <include>    -- include file
//////////////////////////////////////////////////////////////////////////////

	return(this->ParentIs("include"));
}

Bool_t TMrbXMLCodeElem::ProcessElement_Content() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_Content
// Purpose:        Content element: <content>...</content>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    --
// Parent elems:   <clist>    -- list of contents
//////////////////////////////////////////////////////////////////////////////

	return(this->ParentIs("clist"));
}

Bool_t TMrbXMLCodeElem::ProcessElement_s() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_s
// Purpose:        Substitute: <s>...</s>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Wraps "@#@s|...@" (substitution, lower case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	fCode = Form("@#@s|%s@", fCode.Data());
	return(this->CopyCodeToAncestor(fParent));
};

Bool_t TMrbXMLCodeElem::ProcessElement_S() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_S
// Purpose:        Substitute: <S>...</S>
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Wraps "@#@S|...@" (substitution, upper case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	fCode = Form("@#@S|%s@", fCode.Data());
	return(this->CopyCodeToAncestor(fParent));
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
	Bool_t err = !this->CopyCodeToAncestor(fParent);
	err |= !this->CopySubstToAncestor(fParent);
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

Bool_t TMrbXMLCodeElem::ProcessElement_ArgList() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_ReturnVal() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Inheritance() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Xname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Fname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Aname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Gname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Cname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Vname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Sname() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Purp() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Descr() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Author() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mail() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Url() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Version() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Date() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Atype() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Item() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Ftype() { return(this->CopyChildToAncestor(fParent, this->GetName(), fCode)); };

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
//                 Code will be copied to parent after slight modification
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbXMLCodeElem::ProcessElement_B() { fCode = Form("*%s*", fCode.Data()); return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_I() { fCode = Form("//%s//", fCode.Data()); return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_U() { fCode = Form("__%s__", fCode.Data()); return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Bx() { fCode = Form("||%s||", fCode.Data()); return(this->CopyCodeToAncestor(fParent)); };

Bool_t TMrbXMLCodeElem::ProcessElement_Nl() { fCode = "\n"; return(this->CopyCodeToAncestor(fParent)); };

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

Bool_t TMrbXMLCodeElem::ProcessElement_M() { return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Mm() { return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_R() { return(this->CopyCodeToAncestor(fParent)); };
Bool_t TMrbXMLCodeElem::ProcessElement_Rm() { return(this->CopyCodeToAncestor(fParent)); };

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::ProcessElement_<XXX>
// Purpose:        Elements which have meaning for HTML only, NOP here
// Arguments:      --
// Results:        kTRUE
// Description:    <mrbClassList>...</mrbClassList>    list of MARaBOU classes
//                 <rootClassList>...</rootClassList>  list of ROOT classes
//////////////////////////////////////////////////////////////////////////////

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

const Char_t * TMrbXMLCodeElem::MsgHdr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::MsgHdr
// Purpose:        Format message header
// Arguments:      --    
// Description:    Format header for (erro) output
//////////////////////////////////////////////////////////////////////////////

	if (fParser) {
		return(Form("[%s <%s>] ", gSystem->BaseName(fParser->GetXmlFile()), this->GetName()));
	} else {
		return(Form("[<%s>] ", this->GetName()));
	}
}


const Char_t * TMrbXMLCodeElem::FormatHeaderLine(TString & FmtText, const Char_t * LineHdr, const Char_t * Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::FormatHeaderLine
// Purpose:        Format a header line
// Arguments:      TString & FmtText    -- where to write formatted output
//                 Char_t * LineHdr     -- start of header line
//                 Char_t * Text        -- text to be formatted
// Results:        Char_t * FmtText     -- ptr to formatted output     
// Description:    Wraps "@#@s|...@" (substitution, lower case) around code
//                 then adds code to list of substitutions
//////////////////////////////////////////////////////////////////////////////

	FmtText = "";
	Int_t lng = strlen(LineHdr) - 2;
	Int_t from = 0;
	TString line;
	TString text = Text;
	while (text.Tokenize(line, from, "\n")) {
		line = line.Strip(TString::kBoth);
		if (line.IsNull()) continue;
		line.Prepend(LineHdr);
		LineHdr = Form("//%*s", lng, " ");
		FmtText += line;
		FmtText += "\n";
	}
	return(FmtText.Data());
}

void TMrbXMLCodeElem::Indent(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeElem::Indent
// Purpose:        Indent text
// Arguments:      ostream & Out     -- output stream
// Results:        -- 
// Description:    Indents text according to nesting level
//////////////////////////////////////////////////////////////////////////////

	Out << Form("[%2d]", fNestingLevel);
	for (Int_t i = 0; i < fNestingLevel; i++) Out << "  ";
}
