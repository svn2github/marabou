#ifndef __TGMrbFileDialog_h__
#define __TGMrbFileDialog_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbFileDialog.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbFileDialog  - some extension to TGFileDialog
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TGFileDialog.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileInfo
// Purpose:        TGFileInfo with static data members
// Methods:        
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileInfo : public TObject {

	public:
		TGMrbFileInfo() {};
		~TGMrbFileInfo() {};
		TGFileInfo * GetInfo();

	public:
		Char_t ** fFileTypes;
		TString fFileName;
		TString fIniDir;

	protected:
		TGFileInfo fInfo;
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileDialog
// Purpose:        Extended TGFileDialog
// Methods:        
// Description:    Some extension to TGFileDialog:
//                 First call to TGFileDialog starts at path = FileInfo->fIniDir,
//                 further calls start with last path used,
//                 exit from TGFileDialog falls back to working dir.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileDialog : public TObject {

	public:
		TGMrbFileDialog(	const TGWindow * Parent,
							const TGWindow * Main,
                			EFileDialogMode DialogType,
							TGMrbFileInfo * FileInfo);			// ctor

		~TGMrbFileDialog() {};								// dtor

		void PopupDialog();												// pop up the orig TGFileDialog window
		inline void SetPath(const Char_t * Path) { fCurrentPath = Path; };	// set path to be used on next call to TGFileDialog
		inline void Reset() { fCurrentPath = fStartPath; };				// reset path to StartPath

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbFileDialog.html&"); };

	protected:
		const TGWindow * fParent;
		const TGWindow * fMain;
		EFileDialogMode fDialogType;
		TGMrbFileInfo * fFileInfo;

		TString fStartPath;
		TString fCurrentPath;

	ClassDef(TGMrbFileDialog, 0)	// [GraphUtils] Some extension to TGObject
};

#endif
