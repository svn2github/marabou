//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbSystem.cxx
// Purpose:        MARaBOU utilities:
//                 Some extensions to TSystem class
// Description:    Implements extended methods for TSystem
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbSystem)

extern TMrbLogger * gMrbLog;			// access to message logging

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem
// Purpose:        Some extensions to TSystem
// Description:    Adds some high-level methods based on TSystem class
// Keywords:       |
//////////////////////////////////////////////////////////////////////////////

TMrbSystem::TMrbSystem() {
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

Int_t TMrbSystem::FindFile(TObjArray & PathList, const Char_t * FileName, const Char_t * SearchPath,
																							Bool_t MultipleMatch) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::FindFile
// Purpose:        Find file(s) along a search path
// Arguments:      TObjArray & PathList  -- resulting path list
//                 Char_t * FileName     -- file name, may be wild-carded
//                 Char_t * SearchPath   -- unix search path (:-separated)
//                 Bool_t MultipleMatch  -- kTRUE if multiple hits requested
// Results:        Int_t NofFiles        -- number of files found
// Exceptions:     NofFiles = -1         -- error
//                 NofFiles = 0          -- no file found
// Description:    Returns a list of file paths for a given file name.
//                 Files will be APPENDED to list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString searchPath, fileName;
	TString dir, dirEnt, filePath;
	void * dp;
	const Char_t * fp;
	Bool_t hasWildCard;
	Int_t nofFiles;

	TObjString * os;
	TString fn;
	Bool_t foundDir, foundFile;

	searchPath = SearchPath;
	gSystem->ExpandPathName(searchPath);
	searchPath = searchPath.Strip(TString::kBoth);

	if (searchPath.Index(":", 0) == -1) {
		searchPath += ":";
	}
	if (searchPath(searchPath.Length() - 1) != ':') searchPath += ":";

	fileName = FileName;
	if (fileName.Index("?", 0) != -1) {
		gMrbLog->Err() << "Illegal wild card \"?\" in file name - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "FindFile");
		return(-1);
	}

	hasWildCard = (fileName.Index("*", 0) != -1);

	n1 = 0;
	n2 = 0;
	nofFiles = 0;
	TRegexp rx(fileName.Data(), kTRUE);
	foundDir = kFALSE;
	while (n2 != -1) {
		n2 = searchPath.Index(":", n1);
		if (n2 == -1) break;
		dir = searchPath(n1, n2 - n1);
		dir = dir.Strip(TString::kBoth);
		if (dir.Length() == 0 || dir.CompareTo(".") == 0) dir = gSystem->WorkingDirectory();
		dp = gSystem->OpenDirectory(dir.Data());
		if (dp != NULL) {
			foundDir = kTRUE;
			while ( (fp = gSystem->GetDirEntry(dp)) ) {
				dirEnt = fp;
				if (dirEnt.Index(rx, 0) != -1) {
					filePath = dir;
					filePath += "/";
					filePath += dirEnt;
					if (!MultipleMatch) {
						foundFile = kFALSE;
						os = (TObjString *) PathList.First();
						while (os) {
							this->GetBaseName(fn, os->GetString().Data());
							if (dirEnt.CompareTo(fn.Data()) == 0) {
								foundFile = kTRUE;
								break;
							}
							os = (TObjString *) PathList.After(os);
						}
					} else foundFile = kFALSE;
					if (!foundFile) {
						PathList.Add(new TObjString(filePath.Data()));
						nofFiles++;
					}
					if (!hasWildCard && !MultipleMatch) {
						gSystem->FreeDirectory(dp);
						return(1);
					}	
				}
			}
		}
		gSystem->FreeDirectory(dp);
		n1 = n2 + 1;
	}
	if (!foundDir) {
		gMrbLog->Err() << "Can't open path name(s) - " << SearchPath << endl;
		gMrbLog->Flush(this->ClassName(), "FindFile");
		return(-1);
	}
	return(nofFiles);
}

const Char_t * TMrbSystem::GetDirName(TString & DirName, const Char_t * FilePath, Bool_t ExpandWD) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetDirName
// Purpose:        Return directory part of file path
// Arguments:      TString & DirName     -- restulting dir name
//                 Char_t * FilePath     -- full path spec
//                 Bool_t ExpandWD       -- if kTRUE, return wirking dir
//                                                    if plain file name
// Results:        Char_t * DirName      -- directory name (same as DirName.Data())
// Exceptions:     
// Description:    Returns path up to (but without) last "/".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FilePath;
	Int_t n1, n2;

	if (fp.Index("/", 0) == -1) {
		DirName = "";
		return(DirName.Data());
	}

	n1 = 0;
	while (1) {
		if ((n2 = fp.Index("/", n1)) == -1) break;
		n1 = n2 + 1;
	}
	if (n1 == 0 && ExpandWD) {
		DirName = gSystem->WorkingDirectory();
	} else {
		DirName = FilePath;
		DirName = DirName(0, n1 - 1);
	}
	return(DirName.Data());
}

const Char_t * TMrbSystem::GetBaseName(TString & BaseName, const Char_t * FilePath) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetBaseName
// Purpose:        Return file name part of file path
// Arguments:      TString & BaseName    -- resulting file name
//                 Char_t * FilePath     -- full path spec
// Results:        Char_t * BaseName     -- file name part (same as BaseName.Data())
// Exceptions:     
// Description:    Returns path behind last "/".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FilePath;
	Int_t n1, n2;

	n1 = 0;
	while (1) {
		if ((n2 = fp.Index("/", n1)) == -1) break;
		n1 = n2 + 1;
	}
	BaseName = FilePath;
	BaseName = BaseName(n1, BaseName.Length() - n1);
	return(BaseName.Data());
}

const Char_t * TMrbSystem::GetExtension(TString & Extension, const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetExtension
// Purpose:        Return extension part of file name
// Arguments:      TString & Extension   -- resulting extension
//                 Char_t * FileName     -- file spec
// Results:        Char_t * Extension    -- extension part (same as Extension.Data())
// Exceptions:     
// Description:    Returns extension (including ".")
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FileName;
	Int_t n1, n2;

	if (fp.Index(".", 0) == -1) {
		Extension = "";
		return(Extension.Data());
	}

	n1 = 0;
	while (1) {
		if ((n2 = fp.Index(".", n1)) == -1) break;
		n1 = n2 + 1;
	}
	Extension = FileName;
	Extension = Extension(n1, Extension.Length() - n1);
	Extension.Prepend(".");
	return(Extension.Data());
}

Bool_t TMrbSystem::CheckExtension(const Char_t * FileName, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::CheckExtension
// Purpose:        Check if file has a given extension
// Arguments:      Char_t * FileName     -- file spec
//                 Char_t * Extension    -- extension
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks file for a given extension.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString ext;

	this->GetExtension(ext, FileName);
	return(ext.CompareTo(Extension) == 0);
}

const Char_t * TMrbSystem::GetRelPath(TString & Path, const Char_t * BaseDir) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetRelPath
// Purpose:        Get relative path name
// Arguments:      TString & Path        -- full path name
//                 Char_t * BaseDir      -- base directory
// Results:        Char_t * RelPath      -- relative path
// Exceptions:     
// Description:    Returns path name as seen from base dir.
//                 Default base dir is current working dir.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (BaseDir == NULL) BaseDir = gSystem->WorkingDirectory();
	TString baseDir = BaseDir;
	if (baseDir(baseDir.Length()) != '/') baseDir += "/";
	if (Path.Index(baseDir, 0) != 0)	return(Path.Data());
	else								return(Path.Data() + baseDir.Length());
}

Bool_t TMrbSystem::CheckType(const Char_t * Path, EMrbFileType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::CheckType
// Purpose:        Check file type
// Arguments:      const Char_t * Path   -- path name
//                 EMrbFileType Type     -- file type
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks file type
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long_t id;
	Long_t size;
	Long_t flags;
	Long_t modtime;
	TString path;
	
	path = gSystem->ExpandPathName(Path);
		
	gSystem->GetPathInfo(path.Data(), &id, &size, &flags, &modtime);
	
	if ((Type == TMrbSystem::kMrbRegularFile) && (flags == TMrbSystem::kMrbRegularFile)) return(kTRUE);
	return((flags & Type) != 0);
}

Bool_t TMrbSystem::CheckAccess(const Char_t * Path, EAccessMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::CheckAccess
// Purpose:        Check file permission
// Arguments:      const Char_t * Path   -- path name
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks file permission bits
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString path;
	
	path = gSystem->ExpandPathName(Path);
		
	return(!gSystem->AccessPathName(path.Data(), Mode));
}
