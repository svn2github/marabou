#ifndef __TMrbSystem_h__
#define __TMrbSystem_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbSystem.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbSystem    -- extension to TSystem class
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjArray.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem
// Purpose:        Some extensions to TSystem
// Description:    Some high-level methods based on TSystem class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSystem: public TObject {

	public:
		enum EMrbFileType		{	kMrbRegularFile		= 0,
									kMrbExecutableFile	= BIT(0),
									kMrbDirectory 		= BIT(1),
									kMrbSpecialFile		= BIT(2)
								};
	
	public:
		TMrbSystem();						// default ctor
		~TMrbSystem() {};					// default dtor

		Int_t FindFile(TObjArray & PathList,
				const Char_t * FileName, const Char_t * SearchPath = "", Bool_t MultipleMatch = kFALSE) const;

		const Char_t * GetDirName(TString & DirName, const Char_t * FilePath, Bool_t ExpandWD = kFALSE) const;
		const Char_t * GetBaseName(TString & FileName, const Char_t * FilePath) const;
		const Char_t * GetExtension(TString & Extension, const Char_t * FileName) const;
		Bool_t CheckExtension(const Char_t * FileName, const Char_t * Extension) const;
		const Char_t * GetRelPath(TString & Path, const Char_t * BaseDir = NULL) const;

		inline const Char_t * WorkingDirectory() const { return(gSystem->WorkingDirectory()); };
		inline Bool_t ChangeDirectory(const Char_t * Path) const { return(gSystem->ChangeDirectory(Path)); };
		
																						// check file type
		inline Bool_t IsRegular(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbRegularFile)); };
		inline Bool_t IsExecutable(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbExecutableFile)); };
		inline Bool_t IsDirectory(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbDirectory)); };
		inline Bool_t IsSpecial(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbSpecialFile)); };

		inline Bool_t Exists(const Char_t * Path) const { return(this->CheckAccess(Path, kFileExists)); };
		inline Bool_t HasReadPermission(const Char_t * Path) const { return(this->CheckAccess(Path, kReadPermission)); };
		inline Bool_t HasWritePermission(const Char_t * Path) const { return(this->CheckAccess(Path, kWritePermission)); };
		inline Bool_t HasExecutePermission(const Char_t * Path) const { return(this->CheckAccess(Path, kExecutePermission)); };
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSystem.html&"); };

	protected:
		Bool_t CheckType(const Char_t * Path, EMrbFileType Type) const;
		Bool_t CheckAccess(const Char_t * Path, EAccessMode Mode) const;
	
	ClassDef(TMrbSystem, 1) 	// [Utils] Some extensions to TSystem class
};

#endif
