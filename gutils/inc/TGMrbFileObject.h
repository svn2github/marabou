#ifndef __TGMrbFileObject_h__
#define __TGMrbFileObject_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbFileObject.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbFileObjectCombo   -- a file entry together with
//                                           a combo box containing file objects
//                 TGMrbFileObjectList    -- a file entry together with
//                                           a list box containing file objects
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
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "TGComboBox.h"

#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo
// Purpose:        Define a labelled file entry and an associated combo box
// Description:    Defines a labelled file entry (TGMrbFileEntry) together
//                 with a combo box showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileObjectCombo: public TGCompositeFrame, public TGMrbObject {

	public:
		TGMrbFileObjectCombo(const TGWindow * Parent, const Char_t * Label,				// ctor
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

		~TGMrbFileObjectCombo() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		const Char_t * GetSelection(TString & SelItem, Bool_t FullPath = kTRUE) const;	// get selected file/object

		inline void Associate(const TGWindow * Window) { fCombo->Associate(Window); };	// where to go if combobox
																					// selection changes
		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbFileObjectCombo.html&"); };

	protected:
		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB;			//!
			
		TGTextEntry * fEntry;				//!
		TGComboBox * fCombo;				//!
		TGPictureButton * fBrowse;	 		//!

		TString fText;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TGMrbFileObjectCombo, 1) 	// [GraphUtils] A labelled entry field to browse file names
};

#endif
