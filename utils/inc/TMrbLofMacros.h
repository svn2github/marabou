#ifndef __TMrbLofMacros_h__
#define __TMrbLofMacros_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLofMacros.h
// Purpose:        Define utilities to be used with MARaBOU
// Classes:        TMrbLofMacros     -- a list of root macros
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLofMacros.h,v 1.4 2004-11-16 13:30:27 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacros
// Purpose:        Manage a list of root macros
// Description:    Stores a list of macros.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofMacros: public TMrbLofNamedX {

	public:
		enum EMrbMacroEnvNames	{	kMrbMacroName,  		
									kMrbMacroTitle, 		
									kMrbMacroPath, 			
									kMrbMacroWidth, 		
									kMrbMacroNofArgs, 		
									kMrbMacroArgName, 		
									kMrbMacroArgTitle,  	
									kMrbMacroArgType, 		
									kMrbMacroArgEntryType, 	
									kMrbMacroArgEntryWidth, 
									kMrbMacroArgDefaultValue, 
									kMrbMacroArgValues, 	
									kMrbMacroArgAddLofValues, 	
									kMrbMacroArgLowerLimit, 
									kMrbMacroArgUpperLimit, 
									kMrbMacroArgIncrement, 	
									kMrbMacroArgBase, 		
									kMrbMacroArgOrientation, 
									kMrbMacroArgNofCL 		
								};

	public:
		TMrbLofMacros();					 							// default ctor
		TMrbLofMacros(const Char_t * Path); 							// ctor
		~TMrbLofMacros() { this->Delete(); };							// dtor: delete heap objects

		inline void SetPath(const Char_t * Path) { fPath = Path; gSystem->ExpandPathName(fPath); };	// define search path
		inline const Char_t * GetPath() const { return(fPath); };

		Bool_t AddMacro(const Char_t * MacroName); 						// add macro
		Bool_t AskForArgs(const Char_t * MacroName) const {return kTRUE;};					// interactively ask for macro args

		inline TMrbNamedX * FirstMacro() const { return((TMrbNamedX *) First()); };		// loop thru list of macros
		inline TMrbNamedX * NextMacro(TMrbNamedX * Last) const { return((TMrbNamedX *) After(Last)); };

		inline TMrbLofNamedX * GetLofEnvNames() { return(&fLofEnvNames); }; 		// list of legal environment names
		Bool_t CheckEnvName(const Char_t * EnvName, Bool_t IsArgEnv = kTRUE) const;		// check env name

		inline TObjArray * GetLofFilePatterns() { return(&fLofFilePatterns); };

		void PrintMacro(const Char_t * MacroName = "") const ;					// print macro

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLofMacros.html&"); };

	protected:
		TMrbNamedX * ProcessMacro(const Char_t * MacroPath, const Char_t * MdefPath = NULL, Bool_t TestFormat = kTRUE);
		Bool_t CheckMacro(TMrbNamedX * Macro) const;

	protected:
		TString fPath;
		TObjArray fLofFilePatterns;
		TMrbLofNamedX fLofEnvNames;

	ClassDef(TMrbLofMacros, 1)		// [Utils] A list of ROOT macros
};

#endif
