#ifndef __TMrbXMLCodeClient_h__
#define __TMrbXMLCodeClient_h__

#include "TString.h"
#include "TEnv.h"
//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xml/inc/TMrbXMLCodeClient.h
// Purpose:        MARaBOU's interface to XML
// Class:          TMrbXMLCodeClient    -- Virtual class to connect to XML code generator
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXMLCodeClient.h,v 1.3 2008-03-05 12:23:44 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;


//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient
// Purpose:        Marabou's XML based code generator
// Description:    Connects to XML code generator
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbXMLCodeClient {

	public:

    	TMrbXMLCodeClient() {};
		virtual ~TMrbXMLCodeClient() {};

		virtual Bool_t ProvideLofItems(const Char_t * Element, const Char_t * Tag, const Char_t * ItemName, TString & LofItems);
		virtual Bool_t ProvideConditionFlag(const Char_t * Element, const Char_t * Tag, const Char_t * FlagName, TString & FlagValue);
		virtual Bool_t ProvideSubst(const Char_t * Element, const Char_t * Tag, const Char_t * ItemName, const Char_t * Item, TEnv * LofSubst);
		virtual Bool_t ProvideCode(const Char_t * Element, const Char_t * Tag, TEnv * LofSubst, const Char_t * ItemName, const Char_t * Item, TString & Code);
		virtual Bool_t ExecuteTask(const Char_t * Element, const Char_t * Tag, const Char_t * Subtag);

		inline Bool_t IsVerbose() { return(fVerboseMode); };

	protected:
		Bool_t fVerboseMode;

};

#endif
