#ifndef __TGMrbObjectCombo_h__
#define __TGMrbObjectCombo_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbObjectCombo.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbObjectCombo       -- a file entry together with
//                                           a combo box containing file objects
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGFileDialog.h"
#include "TGTextEntry.h"
#include "TGComboBox.h"

#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbObjectCombo
// Purpose:        Define a labelled file entry and an associated combo box
// Description:    Defines a labelled file entry (TGMrbFileEntry) together
//                 with a combo box showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbObjectCombo: public TGCompositeFrame, public TGMrbObject {

	public:
		TGMrbObjectCombo(TGWindow * Parent, const Char_t * Label,				// ctor
							Int_t BufferSize, Int_t EntryId, Int_t ComboId,
							Int_t Width, Int_t Height,
							Int_t EntryWidth, Int_t ComboWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * EntryGC = NULL,
							TGMrbLayout * BrowseGC = NULL,
							TGMrbLayout * ComboGC = NULL,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t EntryOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
							UInt_t ComboOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder);

		~TGMrbObjectCombo() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		const Char_t * GetSelection(TString & SelItem, Bool_t FullPath = kTRUE) const;	// get selected file/object

		inline void Associate(TGWindow * Window) { fCombo->Associate(Window); };	// where to go if combobox
																					// selection changes
		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbObjectCombo.html&"); };

	protected:
		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB;			//!
			
		TGTextEntry * fEntry;				//!
		TGComboBox * fCombo;				//!
		TGPictureButton * fBrowse;	 		//!

		TString fText;

		TGFileInfo * fFileInfo; 			//!

	ClassDef(TGMrbObjectCombo, 1) 	// [GraphUtils] A labelled entry field to browse file names
};

#endif
