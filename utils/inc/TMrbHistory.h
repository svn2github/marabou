#ifndef __TMrbHistory_h__
#define __TMrbHistory_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbHistory.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbHistory    -- (inverse) history buffer
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "TObjArray.h"
#include "TObjString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory
// Purpose:        Keep track of actions performed so far
// Description:    Stores commands in an array of strings which may be
//                 accessed later on.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbHistory: public TObjArray {

	public:
		TMrbHistory() {};					// default ctor
		~TMrbHistory() {};					// default dtor

		TObjString * AddEntry(TObject * Obj, const Char_t * Command); 	// add entry
		const Char_t * GetEntry(Int_t Index);					// get history entry
		Bool_t RemoveEntry(Int_t Index);							// remove entry
		Bool_t Exec(Int_t Index); 							// execute command at entry(i)
		Bool_t Restore(Bool_t ReverseOrder = kTRUE); 			// exec all history entries
						
		void Print(ostream & Out);
		inline void Print() { Print(cout); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbHistory.html&"); };

	protected:

	ClassDef(TMrbHistory, 1) 	// [Utils] Some extensions to TSystem class
};

#endif
