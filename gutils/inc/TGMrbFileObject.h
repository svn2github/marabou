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
// Revision:       $Id: TGMrbFileObject.h,v 1.6 2006-11-29 15:10:28 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "TGComboBox.h"

#include "TMrbLofNamedX.h"
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
		Int_t GetSelectionAsString(TString & SelItem, Bool_t FullPath = kTRUE) const;  // get selected file/object -- string
		Int_t GetSelection(TObjArray & SelArr, Bool_t FullPath = kTRUE) const;	// ... -- array

		inline void Associate(const TGWindow * Window) { fCombo->Associate(Window); };	// where to go if combobox
																					// selection changes
		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB;			//!
			
		TGTextEntry * fEntry;				//!
		TGComboBox * fCombo;				//!
		TGPictureButton * fBrowse;	 		//!

		TString fText;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TGMrbFileObjectCombo, 1) 	// [GraphUtils] A file browser showing ROOT objects in a combo box
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox
// Purpose:        Define a labelled file entry and an associated listbox
// Description:    Defines a labelled file entry (TGMrbFileEntry) together
//                 with a listbox showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileObjectListBox: public TGCompositeFrame, public TGMrbObject {

	public:
		enum	{	kBtnBrowse,
					kBtnSingle,
					kBtnRange,
					kBtnClear,
					kBtnApply
				};

	public:
		TGMrbFileObjectListBox(const TGWindow * Parent, const Char_t * Label,				// ctor
							Int_t BufferSize, Int_t EntryId, Int_t ListBoxId,
							Int_t Width, Int_t Height,
							Int_t EntryWidth, Int_t ListBoxWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * EntryGC = NULL,
							TGMrbLayout * BrowseGC = NULL,
							TGMrbLayout * ListBoxGC = NULL,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t EntryOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
							UInt_t ListBoxOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder);

		~TGMrbFileObjectListBox() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGListBox * GetListBox() const { return(fListBox); };

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		Int_t GetSelectionAsString(TString & SelItem, Bool_t FullPath = kTRUE) const;	// get selected file/object -- string
		Int_t GetSelection(TObjArray & SelArr, Bool_t FullPath = kTRUE) const;	// ... -- array

		inline void Associate(const TGWindow * Window) { fListBox->Associate(Window); };	// where to go if combobox
																					// selection changes
		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB1;			//!
		TGHorizontalFrame * fEB2;			//!
			
		TGTextEntry * fEntry;				//!
		TGListBox * fListBox;				//!
		TGPictureButton * fBrowse;	 		//!

		TGRadioButton * fRBSingle;			//!
		TGRadioButton * fRBRange;			//!
		TGTextButton * fTBClear;			//!
		TGTextButton * fTBApply;			//!

		Int_t fStartIndex;
		TMrbLofNamedX fLofListItems;

		TString fText;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TGMrbFileObjectListBox, 1) 	// [GraphUtils] A file browser showing ROOT objects in a list box
};

#endif
