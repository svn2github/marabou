#ifndef __TMrbLofUserVars_h__
#define __TMrbLofUserVars_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLofUserVars.h
// Purpose:        Define named variables and windows
// Class:          TMrbLofUserVars -- a list of user-defined variables and windows
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TNamed.h"
#include "TDirectory.h"
#include "TObjArray.h"
#include "TSystem.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVar.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars
// Purpose:        Defines a list of user-defined variables and windows
// Description:    Defines a list of user variables.
//                 This class is mainly used to keep track of all vars and
//                 windows defined in the application (global gMrbLofUserVars).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbLofUserVars: public TNamed {

	public:
		TMrbLofUserVars() {};		// default ctor

		~TMrbLofUserVars() { 		// dtor
			fLofVars.Clear();			// empty list - but don't delete objects
			gDirectory->GetList()->Remove(this);
		};

		TMrbLofUserVars(const Char_t * Name);				// explicit ctor

		TMrbLofUserVars(const TMrbLofUserVars &) {};		// default copy ctor

		Bool_t Append(TObject * VarObj);					// add a new object
		inline void Remove(TObject * VarObj) {				// remove object from list
			fLofVars.Remove(VarObj);
			fLofVars.Compress();
		};
		Bool_t Initialize(Char_t * Option = "Default");		// set all vars/windows to initial values
		Bool_t ReadFromFile(const Char_t * FileName);		// read definitions and settings from file
		virtual void Print(Option_t * Option = "Default");	// output settings to cout

		TObject * Find(const Char_t * VarName, UInt_t VarType = kVarOrWindow);	// find variable
		TObject * First(UInt_t  VarType = kVarOrWindow);							// get first entry
		TObject * After(TObject * VarPtr, UInt_t VarType = kVarOrWindow); 		// get next entry

		inline UInt_t GetEntries() { return(fLofVars.GetLast() + 1); };		// get number of entries
		inline TObject * Get(UInt_t Index) { return(fLofVars[Index]); };	// get object at position index
		inline UInt_t IndexOf(TObject * VarObj) { return(fLofVars.IndexOf(VarObj)); };	// get index
		inline void Replace(TObject * OldVar, TObject * NewVar) { fLofVars.AddAt(NewVar, IndexOf(OldVar)); };

		void Draw();										// draw windows

		void SetGlobalAddress();						// set global address gMrbLofUserVars

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLofUserVars.html&"); };

	protected:
		Int_t fNofVars;					// total number of vars/windows
		TObjArray fLofVars; 				// list of variables

	ClassDef(TMrbLofUserVars, 1)	// [Utils] A list of user-defined variables/windows
};

#endif
