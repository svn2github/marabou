#ifndef __TGMrbMacroBrowser_h__
#define __TGMrbMacroBrowser_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbMacroBrowser.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbMacroBrowserComposite   -- manage list of macros (composite frame)
//                 TGMrbMacroBrowserGroup       -- ... (group frame)
//                 TGMrbMacroBrowserPopup       -- ... (popup menu)
//                 TGMrbMacroBrowserTransient   -- ... (transient window)
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbMacroBrowser.h,v 1.21 2007-08-22 13:43:28 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TList.h"
#include "TObjArray.h"
#include "TEnv.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGMenu.h"
#include "TGFileDialog.h"

#include "TMrbNamedX.h"
#include "TMrbString.h"
#include "TMrbLofNamedX.h"
#include "TMrbLofMacros.h"

#include "TGMrbLayout.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbFileObject.h"
#include "TGMrbRadioButton.h"
#include "TGMrbCheckButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbMessageViewer.h"
#include "TGMrbLofKeyBindings.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroArg
// Purpose:        Display a single macro argument
// Description:    Class to describe one singel macro argument
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroArg : public TObject {

	friend class TGMrbMacroFrame;

	public:
		enum EGMrbMacroEntryType		{
											kGMrbMacroEntryEntry		= BIT(0),
											kGMrbMacroEntryMulti		= BIT(2),
											kGMrbMacroEntryChkBtn		= BIT(3),
											kGMrbMacroEntryRdoBtn		= BIT(4),
											kGMrbMacroEntryBeginEnd		= BIT(6),

											kGMrbMacroEntryPlain		= kGMrbMacroEntryEntry,
											kGMrbMacroEntryPlain2		= kGMrbMacroEntryEntry | kGMrbMacroEntryMulti,
											kGMrbMacroEntryPlainC		= kGMrbMacroEntryEntry | kGMrbMacroEntryChkBtn,
											kGMrbMacroEntryPlainC2		= kGMrbMacroEntryPlainC | kGMrbMacroEntryMulti,

											kGMrbMacroEntryUpDown		= kGMrbMacroEntryEntry | BIT(5),
											kGMrbMacroEntryUpDown2		= kGMrbMacroEntryUpDown | kGMrbMacroEntryMulti,
											kGMrbMacroEntryUpDownC		= kGMrbMacroEntryUpDown | kGMrbMacroEntryChkBtn,
											kGMrbMacroEntryUpDownC2 	= kGMrbMacroEntryUpDownC | kGMrbMacroEntryMulti,

											kGMrbMacroEntryUpDownX		= kGMrbMacroEntryUpDown | kGMrbMacroEntryBeginEnd,
											kGMrbMacroEntryUpDownXC 	= kGMrbMacroEntryUpDownX | kGMrbMacroEntryChkBtn,
											kGMrbMacroEntryUpDownX2 	= kGMrbMacroEntryUpDownX | kGMrbMacroEntryMulti,
											kGMrbMacroEntryUpDownXC2	= kGMrbMacroEntryUpDownXC | kGMrbMacroEntryMulti,

											kGMrbMacroEntryYesNo		= BIT(10),
											kGMrbMacroEntryRadio		= BIT(11),
											kGMrbMacroEntryCheck		= BIT(12),
											kGMrbMacroEntryText 		= BIT(13),
											kGMrbMacroEntryCombo		= BIT(14),
											kGMrbMacroEntryFile 		= BIT(15),
											kGMrbMacroEntryFObjCombo	= BIT(16),
											kGMrbMacroEntryFObjListBox	= BIT(17),

											kGMrbMacroEntryComment		= BIT(28),
											kGMrbMacroEntrySection		= BIT(29),
											kGMrbMacroEntryGroupFrame	= BIT(30),
											kGMrbMacroEntryPad			= BIT(31)
										};

		enum EGMrbMacroArgType		{
											kGMrbMacroArgInt,
											kGMrbMacroArgUInt,
											kGMrbMacroArgFloat,
											kGMrbMacroArgDouble,
											kGMrbMacroArgChar,
											kGMrbMacroArgBool,
											kGMrbMacroArgObjArr
										};

		enum EGMrbMacroArgBase			{
											kGMrbMacroArgBaseDec	=	BIT(0),
											kGMrbMacroArgBaseBin	=	BIT(1),
											kGMrbMacroArgBaseOct	=	BIT(2),
											kGMrbMacroArgBaseHex	=	BIT(3)
										};

		enum EGMrbMacroArgOrientation	{	kGMrbMacroEntryHorizontal	=	BIT(0),
											kGMrbMacroEntryVertical 	=	BIT(1)
										};

		enum							{	kGMrbMacroEntryNo	=	BIT(0)	};
		enum							{	kGMrbMacroEntryYes	=	BIT(1)	};
		enum							{	kGMrbMacroNofFileTypes	=	50	};

	public:
		TGMrbMacroArg() { 					// default ctor
			fNumber = 0;
			fFileInfo = new TGFileInfo();
		};
		TGMrbMacroArg(Int_t Number) {		// ctor
			fNumber = Number;
			fFileInfo = new TGFileInfo();
		};
		virtual ~TGMrbMacroArg() { delete fFileInfo; };						// default dtor

		const Char_t * GetResource(TString & Resource, const Char_t * ResourceName) const;

		inline void SetNumber(Int_t Number) { fNumber = Number; };
		inline Int_t GetNumber() const { return(fNumber); };

		inline void SetName(const Char_t * ArgName) { fName = ArgName; };
		inline const Char_t * GetName() { return(fName.Data()); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fNumber;					// arg number
		TString fName;					// arg name

		TMrbNamedX * fType;				// argument type
		TMrbNamedX * fEntryType; 		// entry type;

		TMrbString fCurrentValue;		// current
		TMrbString fDefaultValue;		// default

		Bool_t fAddLofValues;			// add list of values on exec?

		TGLabel * fComment; 			// gui representation
		TGLabel * fSection;
		TGGroupFrame * fGroup;
		TGMrbLabelEntry * fEntry;
		TGMrbRadioButtonList * fRadio;
		TGMrbCheckButtonList * fCheck;
		TGMrbTextButtonList * fText;
		TGMrbLabelCombo * fCombo;
		TGMrbFileEntry * fFile;
		TGMrbFileObjectCombo * fFObjCombo;
		TGMrbFileObjectListBox * fFObjListBox;

		TMrbLofNamedX fButtons; 		// list of buttons

		Int_t fEntryWidth;				// dimensions
		Int_t fNofEntryFields;
		Double_t fLowerLimit;
		Double_t fUpperLimit;
		Double_t fIncrement;
		Int_t fOrientation;
		Int_t fNofCL;

		TGFileInfo * fFileInfo;			// file info to be used with FileEntry
		TObjArray fFileTypes;
		const Char_t * fPtrFileTypes[2 * TGMrbMacroArg::kGMrbMacroNofFileTypes];

 	ClassDef(TGMrbMacroArg, 0)		// [GraphUtils] Display an argument of a ROOT macro
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame
// Purpose:        Dispay and exec a macro
// Description:    A transient frame to display macro args and exec macro
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroFrame : public TGTransientFrame {

	public:
		// ids to dispatch over X events
		enum EGMrbMacroId				{	kGMrbMacroIdExec,
											kGMrbMacroIdReset,
											kGMrbMacroIdClose,
											kGMrbMacroIdQuit,
											kGMrbMacroIdModifyHeader,
											kGMrbMacroIdModifySource,
											kGMrbMacroIdExecClose,
											kGMrbMacroIdUserButton
										};

		enum							{	kEntryId		=	1000	};
		enum							{	kComboId		=	1100	};
		enum							{	kListBoxId		=	1200	};

		enum							{	kFrameWidth 	=	1200 	};
		enum							{	kLineHeight 	=	20		};

		enum							{	kGMrbMacroNofSubframes	=	3	};

	public:
		TGMrbMacroFrame(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro, UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		~TGMrbMacroFrame() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		virtual void CloseWindow() { delete this; };

		Bool_t ResetMacroArgs();						// reset arguments
		Bool_t ExecMacro(); 	 						// exec macro
		Bool_t ModifyMacroHeader(); 					// modify header
		Bool_t ModifyMacroSource(); 					// modify source

		inline TMrbLofNamedX * GetLofArgTypes() { return(&fLofArgTypes); };
		inline TMrbLofNamedX * GetLofEntryTypes() { return(&fLofEntryTypes); };
		inline TObjArray * GetLofMacroArgs() { return(&fLofMacroArgs); };

		TGMrbMacroArg * FindArgByName(const Char_t * ArgName);
		Bool_t SetArgValue(const Char_t * ArgName, Int_t Value);
		Bool_t SetArgValue(const Char_t * ArgName, Bool_t Value);
		Bool_t SetArgValue(const Char_t * ArgName, Double_t Value);
		Bool_t SetArgValue(const Char_t * ArgName, const Char_t * Value);
		Bool_t GetArgValue(const Char_t * ArgName, Int_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, Bool_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, Double_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, TString & Value, Int_t EntryNo = 0);

		Bool_t SetArgCheck(const Char_t * ArgName, UInt_t Check);
		Bool_t GetArgCheck(const Char_t * ArgName, UInt_t & Check);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t LoadMacro();

	protected:
		TList fHeap;

		TMrbNamedX * fMacro;

		TGGroupFrame * fMacroInfo;
		TGLabel * fMacroName;
		TGLabel * fMacroPath;
		TGLabel * fMacroTitle;
		TGGroupFrame * fMacroArgs;
		TGVerticalFrame * fMacroSubframe[kGMrbMacroNofSubframes];
		TGMrbTextButtonGroup * fAction;

		TMrbLofNamedX fLofArgTypes;
		TMrbLofNamedX fLofEntryTypes;

		TMrbLofNamedX fLofActions;
		TMrbLofNamedX fLofActionsM;

		TObjArray fLofMacroArgs;	// list of arguments

		Bool_t fMacroLoaded;

	ClassDef(TGMrbMacroFrame, 0)	// [GraphUtils] A frame containing definitions of a ROOT macro
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit
// Purpose:        Edit macro layout
// Description:    A transient frame to modify macro args
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroEdit : public TGTransientFrame {

	public:
		// button ids
		enum EGMrbMacroEditButtons		{	kGMrbMacroAclicNone,
											kGMrbMacroAclicPlus,
											kGMrbMacroAclicPlusPlus,
											kGMrbMacroAclicPlusG,
											kGMrbMacroAclicPlusPlusG,
											kGMrbMacroMayModify,
											kGMrbMacroDontModify,
											kGMrbMacroAddGuiPtr,
											kGMrbMacroDontAddGuiPtr
										};

		// ids to dispatch over X events
		enum EGMrbMacroEditId			{	kGMrbMacroEditCurrentArg,
											kGMrbMacroEditIdFirstArg,
											kGMrbMacroEditIdLastArg,
											kGMrbMacroEditIdRemoveArg,
											kGMrbMacroEditIdResetArg,
											kGMrbMacroEditIdNewArgBefore,
											kGMrbMacroEditIdNewArgAfter,

											kGMrbMacroEditIdResetAll,
											kGMrbMacroEditIdResetHeader,
											kGMrbMacroEditIdResetAllArgs,
											kGMrbMacroEditIdSave,
											kGMrbMacroEditIdClose
										};

		enum							{	kEntryId		=	1000	};

		enum							{	kFrameWidth 	=	630 	};
		enum							{	kLineHeight 	=	20		};

		enum							{	kMaxNofArgs 	=	50		};

		enum EGMrbMacroTag	 			{	kMacroTagFile,
											kMacroTagName,
											kMacroTagTitle,
											kMacroTagProc,
											kMacroTagAuthor,
											kMacroTagMailAddr,
											kMacroTagUrl,
											kMacroTagCreationDate,
											kMacroTagSyntax,
											kMacroTagArguments,
											kMacroTagExec,
											kMacroTagIncludeHFiles,
											kMacroTagEnums,
											kMacroTagCode
										};

	public:
		TGMrbMacroEdit(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro, UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		~TGMrbMacroEdit() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		virtual void CloseWindow() { delete this; };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t UpdateArg(Int_t ArgNo = -1);					// update GUI for a given argument
		Bool_t UpdateNofArgs(); 							// update number of arguments in GUI
		Bool_t StoreHeader();								// store header data
		Bool_t StoreArg(Int_t ArgNo = -1);					// store GUI settings for (current) arg
		Bool_t RemoveArg(Int_t ArgNo = -1);					// remove (current) argument and its env defs
		Bool_t InsertArg(Int_t ArgNo);				 		// insert a new argument
		Bool_t RestoreArg(Int_t ArgNo = -1);				// restore original settings for (current) argument
		Bool_t RestoreHeader(); 							// reset header env (Name, Title, Path ...) to original values
		Bool_t RestoreAllArgs();							// reset all args to original values
		Bool_t ChangeEnv(Int_t ArgNo, Bool_t Delete);		// change current environment by deleting/inserting this argument
		Bool_t SaveMacro(const Char_t * FileName);			// save settings to a file
		Int_t ExtractEnums(TMrbLofNamedX & LofEnums, Int_t ArgNo);	// extract enum defs

	protected:
		TList fHeap;

		TMrbNamedX * fMacro;

		TGGroupFrame * fMacroInfo;
		TGMrbLabelEntry * fMacroName;
		TGMrbLabelEntry * fMacroTitle;
		TGMrbLabelEntry * fMacroRcFile;
		TGMrbLabelEntry * fMacroProc;
		TGMrbLabelEntry * fMacroWidth;

		TGMrbRadioButtonList * fMacroAclic;
		TGMrbRadioButtonList * fMacroModify;
		TGMrbRadioButtonList * fMacroAddGuiPtr;

		TGMrbLabelEntry * fMacroNofArgs;

		TGGroupFrame * fMacroArg;
		TGMrbLabelEntry * fArgNumber;
		TGMrbTextButtonList * fArgAction;

		TGGroupFrame * fMacroLayout;
		TGMrbLabelEntry * fArgName;
		TGMrbLabelEntry * fArgTitle;
		TGMrbRadioButtonList * fArgType;
		TGMrbRadioButtonList * fArgEntryType;
		TGMrbLabelEntry * fArgNofEntryFields;
		TGMrbLabelEntry * fArgEntryWidth;
		TGMrbLabelEntry * fArgDefaultValue;
		TGMrbLabelEntry * fArgValues;
		TGMrbRadioButtonList * fArgAddLofValues;
		TGMrbRadioButtonList * fArgBase;
		TGMrbLabelEntry * fArgLowerLimit;
		TGMrbLabelEntry * fArgUpperLimit;
		TGMrbLabelEntry * fArgIncrement;
		TGMrbRadioButtonList * fArgOrientation;
		TGMrbLabelEntry * fArgNofCL;

		TGMrbTextButtonGroup * fAction;

		TMrbLofNamedX fLofAclicModes;
		TMrbLofNamedX fLofModifyModes;
		TMrbLofNamedX fLofGuiModes;

		TMrbLofNamedX fLofArgTypes;
		TMrbLofNamedX fLofEntryTypes;
		TMrbLofNamedX fLofOrientations;
		TMrbLofNamedX fLofIntegerBases;
		TMrbLofNamedX fYesNo;

		TMrbLofNamedX fLofArgActions;
		TMrbLofNamedX fLofActions;

		Int_t fNofArgs; 					// actual number of arguments
		TGMrbMacroArg fCurrentArg;			// current argument number
		TEnv * fOriginalEnv;				// original settings
		TEnv * fCurrentEnv; 				// current settings
		TMrbLofNamedX fLofEnvNames; 		// list of legal env names

	ClassDef(TGMrbMacroEdit, 0)		// [GraphUtils] Edit macro args and layout
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList
// Purpose:        A list of buttons, one per macro
// Description:    A list of text buttons, one per macro,
//                 showing macro name & title
//                 Common subclass to be used by TGMrbBrowserVertical,
//                 TGMrbBrowserGroup, and TGMrbBrowserTransient
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroList : public TGVerticalFrame {

	public:
		TGMrbMacroList(const TGWindow * Parent, TMrbLofMacros * MacroList, UInt_t Width, UInt_t Height,
																TGMrbLayout * FrameGC, TGMrbLayout * ButtonGC,
																UInt_t FrameOptions = kChildFrame | kVerticalFrame,
																UInt_t ButtonOptions = kRaisedFrame | kDoubleBorder);
		~TGMrbMacroList() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		virtual void CloseWindow() { delete this; };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TGMrbTextButtonList * fButtonList;

		TMrbLofMacros * fLofMacros;
		TMrbLofNamedX fLofButtons;

	ClassDef(TGMrbMacroList, 0) 	// [GraphUtils] A list of ROOT macros
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain
// Purpose:        Manage a list of macros
// Description:    A main frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserMain : public TGMainFrame {

	public:
		enum EGMrbMacroMainId	{	kGMrbMacroMenuFileNew,
									kGMrbMacroMenuFileOpen,
									kGMrbMacroMenuFileExit,
									kGMrbMacroMenuViewErrors,
									kGMrbMacroMenuHelpContents,
									kGMrbMacroMenuHelpAbout
								};

	public:
		TGMrbMacroBrowserMain(const TGWindow * Parent, TMrbLofMacros * LofMacros,
												UInt_t Width, UInt_t Height);

		virtual ~TGMrbMacroBrowserMain() { fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		void CloseWindow();

	protected:
		void PopupMessageViewer();
	
	protected:
		TList fHeap;
		TGPopupMenu * fMenuFile;
		TGPopupMenu * fMenuView;
		TGPopupMenu * fMenuHelp;
		TGMenuBar * fMenuBar;

		TGMrbLayout * fFrameGC;
		TGMrbLayout * fButtonGC;

		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

		TGMrbMessageViewer * fMsgViewer;

		TGMrbLofKeyBindings fKeyBindings;
		
	ClassDef(TGMrbMacroBrowserMain,0)		// [GraphUtils] Show ROOT macros in a main frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserVertical
// Purpose:        Manage a list of macros
// Description:    A vertical frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserVertical : public TGVerticalFrame {

	public:
		TGMrbMacroBrowserVertical(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height, UInt_t Options);

		virtual ~TGMrbMacroBrowserVertical() {};

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) { return(kTRUE); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserVertical,0)	// [GraphUtils] Show ROOT macros in a vertical frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Manage a list of macros
// Description:    Group frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserGroup : public TGGroupFrame {

	public:
		TGMrbMacroBrowserGroup(const TGWindow * Parent, const Char_t * Title, TMrbLofMacros * LofMacros, TGMrbLayout * Layout);

		virtual ~TGMrbMacroBrowserGroup() {};

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) { return(kTRUE); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserGroup,0)		// [GraphUtils] Show ROOT macros in a group frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup
// Purpose:        Manage a list of macros
// Description:    Popup menu showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserPopup : public TGPopupMenu {

	public:
		enum EGMrbMacroPopupId	{	kGMrbPopupNew	= 1000,
									kGMrbPopupOpen
								};

	public:
		TGMrbMacroBrowserPopup(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width = 10, UInt_t Height = 10);

		virtual ~TGMrbMacroBrowserPopup() { fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;

	ClassDef(TGMrbMacroBrowserPopup,0)		// [GraphUtils] Show ROOT macros in a popup menu
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserTransient
// Purpose:        Manage a list of macros
// Description:    Transient frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserTransient : public TGTransientFrame {

	public:
		TGMrbMacroBrowserTransient(const TGWindow * Parent, const TGWindow * Main, const Char_t * Title, TMrbLofMacros * LofMacros,
															UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);

		virtual ~TGMrbMacroBrowserTransient() {fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) { return(kTRUE); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserTransient,0)		// [GraphUtils] Show ROOT macros in a transient frame
};

#endif
