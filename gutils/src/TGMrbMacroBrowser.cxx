//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbMacroBrowser.cxx
// Purpose:        MARaBOU graphic utilities: browser to manage root macros
// Description:    Implements class methods to browse & execute root macros
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbMacroBrowser.cxx,v 1.12 2006-07-14 08:02:52 Rudolf.Lutter Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbMacroBrowser.gif>
*/
//End_Html
// Generated Code:
//Begin_Html
/*
<img src=gutils/TGMrbMacroBrowserCode.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

#include <sstream>

#include "TEnv.h"
#include "TApplication.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"
#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "TGMsgBox.h"

#include "TGMrbMacroBrowser.h"

#include "SetColor.h"

ClassImp(TGMrbMacroBrowserMain)
ClassImp(TGMrbMacroBrowserVertical)
ClassImp(TGMrbMacroBrowserGroup)
ClassImp(TGMrbMacroBrowserPopup)
ClassImp(TGMrbMacroBrowserTransient)
ClassImp(TGMrbMacroFrame)
ClassImp(TGMrbMacroEdit)
ClassImp(TGMrbMacroArg)
ClassImp(TGMrbMacroList)

const SMrbNamedX kGMrbMacroLofActions[] =
			{
				{TGMrbMacroFrame::kGMrbMacroIdModifyHeader, "Modify header",	"Modify macro layout"				},
				{TGMrbMacroFrame::kGMrbMacroIdModifySource, "Modify source",	"Modify macro source"				},
				{TGMrbMacroFrame::kGMrbMacroIdExec, 		"Execute",			"Execute macro"						},
				{TGMrbMacroFrame::kGMrbMacroIdExecClose, 	"Exec + Close", 	"Execute macro & close window"		},
				{TGMrbMacroFrame::kGMrbMacroIdReset,		"Reset",			"Reset arguments to default values"	},
				{TGMrbMacroFrame::kGMrbMacroIdClose,		"Close",			"Close window" 				 		},
				{0, 										NULL,				NULL								}
			};

const SMrbNamedX kGMrbMacroLofEditArgActions[] =
			{
				{TGMrbMacroEdit::kGMrbMacroEditIdRemoveArg, 	"Remove", 		"Remove argument"					},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetArg, 		"Reset",		"Reset argument defs to defaults" 	},
				{TGMrbMacroEdit::kGMrbMacroEditIdNewArgBefore,	"New before",	"Insert argument BEFORE current one"	},
				{TGMrbMacroEdit::kGMrbMacroEditIdNewArgAfter,	"New after",	"Insert argument AFTER current one"	},
				{0, 											NULL,			NULL								}
			};

const SMrbNamedX kGMrbMacroLofEditActions[] =
			{
				{TGMrbMacroEdit::kGMrbMacroEditIdResetAll, 		"Reset ALL",	"Reset header AND arguments"		},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetHeader, 	"Reset header",	"Reset header only" 				},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetAllArgs, 	"Reset args",	"Reset arguments only"				},
				{TGMrbMacroEdit::kGMrbMacroEditIdSave, 			"Save macro",	"Save macro to file"				},
				{TGMrbMacroEdit::kGMrbMacroEditIdClose,			"Close",		"Close window"						},
				{0, 											NULL,			NULL								}
			};

const SMrbNamedX kGMrbMacroLofArgTypes[] =
				{
					{TGMrbMacroArg::kGMrbMacroArgInt, 		"Int_t",		"Integer value"					},
					{TGMrbMacroArg::kGMrbMacroArgUInt, 		"UInt_t",		"Unsigned integer value"		},
					{TGMrbMacroArg::kGMrbMacroArgFloat, 	"Float_t",		"Floating point value"			},
					{TGMrbMacroArg::kGMrbMacroArgDouble,	"Double_t", 	"Double precision value"		},
					{TGMrbMacroArg::kGMrbMacroArgChar,		"Char_t *", 	"String of characters"			},
					{TGMrbMacroArg::kGMrbMacroArgBool,		"Bool_t",		"Boolean value" 				},
					{0, 									NULL,			NULL							}
				};

const SMrbNamedX kGMrbMacroLofEntryTypes[] =
				{
					{TGMrbMacroArg::kGMrbMacroEntryPlain, 	"Entry",		"Plain entry (text or numeric)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryUpDown,	"UpDown",		"Entry (numeric) with Up/Down buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownX,	"UpDown-X", 	"Entry (numeric) with Begin/Up/Down/End buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryYesNo, 	"YesNo",		"Yes/No button" 							},
					{TGMrbMacroArg::kGMrbMacroEntryRadio,	"Radio",		"List of (exclusive) radio buttons" 				},
					{TGMrbMacroArg::kGMrbMacroEntryCheck,	"Check",		"List of (inclusive) check buttons" 				},
					{TGMrbMacroArg::kGMrbMacroEntryCombo,	"Combo",		"Combo box" 							},
					{TGMrbMacroArg::kGMrbMacroEntryFile,	"File", 		"File dialog box"						},
					{TGMrbMacroArg::kGMrbMacroEntryFObjCombo,"FObjCombo",	"Combo box for ROOT objects"			},
					{0, 									NULL,			NULL									}
				};

const SMrbNamedXShort kGMrbMacroLofOrientations[] =
							{
								{TGMrbMacroArg::kGMrbMacroEntryHorizontal, 	"horizontal"	},
								{TGMrbMacroArg::kGMrbMacroEntryVertical,	"vertical"		},
								{0, 										NULL			}
							};

const SMrbNamedXShort kGMrbMacroLofIntegerBases[] =
							{
								{TGMrbMacroArg::kGMrbMacroArgBaseDec,	 	"dec"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseBin,		"bin"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseOct,		"oct"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseHex,		"hex"			},
								{0, 										NULL			}
							};

const SMrbNamedXShort kGMrbMacroYesNoButtons[] =
							{
								{TGMrbMacroArg::kGMrbMacroEntryNo,		"No" 		},
								{TGMrbMacroArg::kGMrbMacroEntryYes, 	"Yes"		},
								{0, 									NULL		}
							};


const SMrbNamedXShort kGMrbMacroLofTags[] =
							{
								{TGMrbMacroEdit::kMacroTagFile,				"MACRO_FILE"				},
								{TGMrbMacroEdit::kMacroTagName,				"MACRO_NAME"				},
								{TGMrbMacroEdit::kMacroTagTitle, 			"MACRO_TITLE"				},
								{TGMrbMacroEdit::kMacroTagAuthor,			"AUTHOR"					},
								{TGMrbMacroEdit::kMacroTagCreationDate,		"CREATION_DATE" 			},
								{TGMrbMacroEdit::kMacroTagSyntax,			"MACRO_SYNTAX"				},
								{TGMrbMacroEdit::kMacroTagArguments, 		"MACRO_ARGUMENTS"			},
								{TGMrbMacroEdit::kMacroTagExec,				"MACRO_EXEC"				},
								{TGMrbMacroEdit::kMacroTagIncludeHFiles,	"MACRO_INCLUDE_H_FILES"		},
								{TGMrbMacroEdit::kMacroTagCode,				"MACRO_CODE"				},
								{0, 											NULL					}
							};

// file types
const char * macroFileTypes[] =	{	"ROOT macros", 	"*.C",
									"All files",	"*",
                            		NULL,			NULL
								};

extern TMrbLogger * gMrbLog;		// access to message logger 

TGMrbMacroBrowserMain::TGMrbMacroBrowserMain(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height) : TGMainFrame(Parent, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain
// Purpose:        Browse & execute root macros in a main frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Displays & executes root macros in a main frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fMsgViewer = NULL;

//	Initialize colors
	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fButtonGC = new TGMrbLayout(font, black, gray);
	HEAP(fButtonGC);
	fFrameGC = new TGMrbLayout(font, black, blue);
	HEAP(fFrameGC);

	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);

	fMenuFile->AddEntry("New ... Ctrl-n", kGMrbMacroMenuFileNew);
	fMenuFile->AddEntry("Open ... Ctrl-o", kGMrbMacroMenuFileOpen);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Exit ... Ctrl-q", kGMrbMacroMenuFileExit);
	fMenuFile->Associate(this);

	fMenuView = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuView);

	fMenuView->AddEntry("&Errors", kGMrbMacroMenuViewErrors);
	fMenuView->Associate(this);

	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuHelp);

	fMenuHelp->AddEntry("&Contents", kGMrbMacroMenuHelpContents);
	fMenuHelp->AddEntry("&About", kGMrbMacroMenuHelpAbout);
	fMenuHelp->Associate(this);

	fMenuBar = new TGMenuBar(this, 500, 20, kHorizontalFrame | kRaisedFrame);
	HEAP(fMenuBar);

	TGLayoutHints * menuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,0, 4, 0, 0);
	HEAP(menuBarItemLayout);
	fMenuBar->AddPopup("&File", fMenuFile, menuBarItemLayout);
	fMenuBar->AddPopup("&View", fMenuView, menuBarItemLayout);
	TGLayoutHints * menuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
	HEAP(menuBarHelpLayout);
	fMenuBar->AddPopup("&Help", fMenuHelp, menuBarHelpLayout);

	TGLayoutHints * menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
	HEAP(menuBarLayout);
	this->AddFrame(fMenuBar, menuBarLayout);

	fMenuBar->ChangeBackground(gray);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	fFrameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	fButtonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros, Width - 10, Height - 10, fFrameGC, fButtonGC);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, fFrameGC->LH());

	this->ChangeBackground(blue);

	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-n", TGMrbLofKeyBindings::kGMrbKeyActionNew);
	fKeyBindings.BindKey("Ctrl-o", TGMrbLofKeyBindings::kGMrbKeyActionOpen);
	
	if (gEnv->GetValue("MacroBrowser.ViewMessages", kFALSE) == 1
	||	gEnv->GetValue("TGMrbMessageViewer.Popup", kFALSE) == 1) this->PopupMessageViewer();

	SetWindowName("MacroBrowser: Display & exec ROOT macros");

	MapSubwindows();

	Resize(GetDefaultSize());
	Int_t w = fMenuBar->GetDefaultWidth();
	if (w < (Int_t) fMacroList->GetDefaultWidth()) w = fMacroList->GetDefaultWidth();
	Resize(w + 10, fMenuBar->GetDefaultHeight() + fMacroList->GetDefaultHeight() + 5);

	MapWindow();
}

Bool_t TGMrbMacroBrowserMain::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain::ProcessMessage
// Purpose:        Message handler
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroBrowserMain.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;

	TGFileInfo fi;

	TMrbLofMacros lofMacros;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_MENU:
					switch (Param1) {
						case TGMrbMacroBrowserMain::kGMrbMacroMenuFileNew:
							macro = new TMrbNamedX(0, "macro.C");
							macroEnv = new TEnv(".new");
							macroEnv->SetValue("NofArgs=1");
							macro->AssignObject(macroEnv);
							new TGMrbMacroEdit(fClient->GetRoot(), this, macro, 100, 100);
							break;
						case TGMrbMacroBrowserMain::kGMrbMacroMenuFileOpen:
							fi.fFileTypes = (const char **) macroFileTypes;
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
							if (fi.fFilename != NULL && *fi.fFilename != '\0') {
								if (lofMacros.AddMacro(fi.fFilename)) {
									macro = lofMacros.FirstMacro();
									if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
								}
							}
							break;
						case TGMrbMacroBrowserMain::kGMrbMacroMenuViewErrors:
							if (fMsgViewer != NULL) delete fMsgViewer;
							this->PopupMessageViewer();
							break;
						case TGMrbMacroBrowserMain::kGMrbMacroMenuFileExit:
							this->CloseWindow();
							break;
					}
					break;

					default:
						break;
				}
				break;
				
		case kC_KEY:
			switch (Param1) {
				case TGMrbLofKeyBindings::kGMrbKeyActionExit:
					gApplication->Terminate(0);
					break;
				case TGMrbLofKeyBindings::kGMrbKeyActionNew:
					macro = new TMrbNamedX(0, "macro.C");
					macroEnv = new TEnv(".new");
					macroEnv->SetValue("NofArgs=1");
					macro->AssignObject(macroEnv);
					new TGMrbMacroEdit(fClient->GetRoot(), this, macro, 100, 100);
					break;
				case TGMrbLofKeyBindings::kGMrbKeyActionOpen:
					fi.fFileTypes = (const char **) macroFileTypes;
					new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
					if (fi.fFilename != NULL && *fi.fFilename != '\0') {
						if (lofMacros.AddMacro(fi.fFilename)) {
							macro = lofMacros.FirstMacro();
							if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
						}
					}
					break;
			}
			break;
	}
	return(kTRUE);
}

void TGMrbMacroBrowserMain::CloseWindow() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain::CloseWindow
// Purpose:        Close window and terminate application
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Got close message for this MainFrame.
//                 Calls parent's CloseWindow()
//                 (which destroys the window) and terminates the application.
//                 The close message is generated by the window manager when
//                 its close window menu item is selected.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   TGMainFrame::CloseWindow();
   gApplication->Terminate(0);
}

void TGMrbMacroBrowserMain::PopupMessageViewer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain::PopupMessageViewer
// Purpose:        Create a message viewer window
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


	fMsgViewer = new TGMrbMessageViewer(fClient->GetRoot(), this,
									"MacroBrowser: View (error) messages", 800, 300, gMrbLog, kTRUE, NULL);
	fMsgViewer->Disable(TMrbLogMessage::kMrbMsgMessage);
	fMsgViewer->Enable(TMrbLogMessage::kMrbMsgError);
	fMsgViewer->AddEntries();
}

TGMrbMacroBrowserVertical::TGMrbMacroBrowserVertical(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height, UInt_t Options) :
															TGVerticalFrame(Parent, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserVertical
// Purpose:        Browse & execute root macros in a vertical frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
//                 UInt_t Options                  -- frame options
// Results:        
// Exceptions:     
// Description:    Displays & executes root macros in a vertical frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, blue);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros, Width - 20, Height - 20, frameGC, buttonGC, Options);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroBrowserGroup::TGMrbMacroBrowserGroup(const TGWindow * Parent, const Char_t * Title, TMrbLofMacros * LofMacros, TGMrbLayout * Layout)
							: TGGroupFrame(Parent, Title, kVerticalFrame, Layout->GC(), Layout->Font(), Layout->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Browse & execute root macros in a group frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Title                  -- group title
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 TGMrbLayout * Layout            -- layout (GC, font, background)
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a group frame
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, blue);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros,	this->GetDefaultWidth() - 20,
														this->GetDefaultHeight() - 20,
														frameGC, buttonGC);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroBrowserPopup::TGMrbMacroBrowserPopup(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height) : TGPopupMenu(Parent, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup
// Purpose:        Browse & execute root macros in a popup menu
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a popup menu
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;
	TString macroName;
	TString macroTitle;

	fLofMacros = LofMacros;

	macro = fLofMacros->FirstMacro();
	this->AddEntry("&New", TGMrbMacroBrowserPopup::kGMrbPopupNew);
	this->AddEntry("&Open", TGMrbMacroBrowserPopup::kGMrbPopupOpen);
	this->AddSeparator();
	while (macro) {
		macroEnv = (TEnv *) macro->GetAssignedObject();
		macroName = macro->GetName();
		macroName += ": ";
		macroTitle = macroEnv->GetValue("Title", "");
		macroName += macroTitle;
		this->AddEntry(macroName.Data(), macro->GetIndex());
		macro = fLofMacros->NextMacro(macro);
	}
}

Bool_t TGMrbMacroBrowserPopup::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup::ProcessMessage
// Purpose:        Message handler
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroBrowserPopup.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;

	TGFileInfo fi;
	TString ifile;

	TMrbLofMacros lofMacros;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_MENU:
					switch (Param1) {
						case TGMrbMacroBrowserPopup::kGMrbPopupNew:
							macro = new TMrbNamedX(0, "macro.C");
							macroEnv = new TEnv(".new");
							macroEnv->SetValue("NofArgs=1");
							macro->AssignObject(macroEnv);
							new TGMrbMacroEdit(fClient->GetRoot(), this, macro, 100, 100);
							break;
						case TGMrbMacroBrowserPopup::kGMrbPopupOpen:
							fi.fFileTypes = (const char **) macroFileTypes;
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
							if (fi.fFilename != NULL && *fi.fFilename != '\0') {
								if (lofMacros.AddMacro(fi.fFilename)) {
									macro = lofMacros.FirstMacro();
									if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
								}
							}
							break;
						default:
							macro = fLofMacros->FindByIndex(Param1);
							new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
							break;
					}
					break;

				default:	break;
			}
			break;

		default:	break;
	}
	return(kTRUE);
}

TGMrbMacroBrowserTransient::TGMrbMacroBrowserTransient(const TGWindow * Parent, const TGWindow * Main,
									const Char_t * Title, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height, UInt_t Options)
														: TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Browse & execute root macros in a group frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TGWindow * Main                 -- main window
//                 Char_t * Title                  -- title bar
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a group frame
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, blue);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros,	this->GetDefaultWidth() - 20,
														this->GetDefaultHeight() - 20,
														frameGC, buttonGC, Options);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroList::TGMrbMacroList(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height,
														TGMrbLayout * FrameGC, TGMrbLayout * ButtonGC,
														UInt_t FrameOptions, UInt_t ButtonOptions) :
											TGVerticalFrame(Parent, Width, Height, FrameOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList
// Purpose:        A text button list containing one button per macro
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
//                 TGMrbLayout * FrameGC           -- frame layout
//                 TGMrbLayout * ButtonGC          -- button layout
//                 UInt_t FrameOptions             -- frame options
//                 UInt_t ButtonOptions            -- button options
// Results:        
// Exceptions:     
// Description:    Displays a list of buttons, one per macro.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;
	TString macroTitle;

	fLofMacros = LofMacros;

	fLofButtons.Delete();
	macro = fLofMacros->FirstMacro();
	while (macro) {
		macroEnv = (TEnv *) macro->GetAssignedObject();
		macroTitle = macroEnv->GetValue("Title", "");
		fLofButtons.AddNamedX(new TMrbNamedX(macro->GetIndex(), macro->GetName(), macroTitle));
		macro = fLofMacros->NextMacro(macro);
	}
	fButtonList = new TGMrbTextButtonList(this, NULL, &fLofButtons, -1, 1, Width, Height,
														FrameGC, NULL, ButtonGC, FrameOptions, ButtonOptions);
	HEAP(fButtonList);
	this->AddFrame(fButtonList, FrameGC->LH());
	fButtonList->Associate(this);

	this->ChangeBackground(FrameGC->BG());
}

Bool_t TGMrbMacroList::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList::ProcessMessage
// Purpose:        Message handler
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroList.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					macro = fLofMacros->FindByIndex(Param1);
					new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
					break;

				default:	break;
			}
			break;

		default:	break;
	}
	return(kTRUE);
}

TGMrbMacroFrame::TGMrbMacroFrame(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro,
										UInt_t Width, UInt_t Height, UInt_t Options) :
													TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame
// Purpose:        A fransient frame to display macro args 
// Arguments:      TGWindow * Parent     -- parent window
//                 TGWindow * Main       -- main window
//                 TMrbNamedX * Macro    -- macro specs
//                 UInt_t Width          -- width
//                 UInt_t Height         -- height
//                 UInt_t Options        -- options
// Results:        
// Exceptions:     
// Description:    Displays macro arguments and execs macro.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TEnv * macroEnv;
	TGMrbMacroArg * macroArg;

	Int_t frameWidth;

	Int_t nofArgs;

	TString macroName, macroPath, titleBar;
	TString argName, argTitle, argType, argEntryType, addLofValues;
	TMrbString argString;
	TMrbString argValue;
	TString prefix;
	TMrbNamedX * button;

	Int_t defaultEntryWidth;
	Int_t intBase;
	TString currentValue;
	TString defaultValue;
	TString value;
	TString cintString;

	TGMrbLayout * updownGC;
	Bool_t xUpDown;

	TObjArray lofSubstrings;

	TObjArray * lofSpecialButtons;
	
	TString inputFile;
	Int_t nft;

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(font, black, blue);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, blue);
	HEAP(frameGC);
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * entryGC = new TGMrbLayout(font, black, white);
	HEAP(entryGC);
	TGMrbLayout * comboGC = new TGMrbLayout(font, black, white);
	HEAP(comboGC);

//	Initialize several lists
	fLofArgTypes.SetName("Argument types");
	fLofArgTypes.AddNamedX(kGMrbMacroLofArgTypes);
	fLofEntryTypes.SetName("Entry types");
	fLofEntryTypes.AddNamedX(kGMrbMacroLofEntryTypes);
	fLofActions.SetName("Action buttons");
	fLofActions.AddNamedX(kGMrbMacroLofActions);

	fMacro = Macro; 		// save pointer to macro

	macroEnv = (TEnv *) Macro->GetAssignedObject();
	nofArgs = macroEnv->GetValue("NofArgs", 1);

	frameWidth = macroEnv->GetValue("Width", -1);
	if (frameWidth == -1) frameWidth = TGMrbMacroFrame::kFrameWidth;

	macroName = Macro->GetName();
	titleBar = "ROOT Macro Browser: ";
	titleBar += macroName;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);
	fMacroInfo = new TGGroupFrame(this, "Info", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroInfo);
	this->AddFrame(fMacroInfo, frameGC->LH());

	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 80, 1, 3, 3);
	HEAP(labelLayout);
	labelGC->SetLH(labelLayout);
	fMacroName = new TGLabel(fMacroInfo, Macro->GetName(), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroName);
	fMacroName->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroName, labelGC->LH());

	fMacroTitle = new TGLabel(fMacroInfo, macroEnv->GetValue("Title", ""), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroTitle);
	fMacroTitle->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroTitle, labelGC->LH());

	macroPath = "loaded from ";
	macroPath += macroEnv->GetValue("Path", "");
	fMacroPath = new TGLabel(fMacroInfo, macroPath.Data(), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroPath);
	fMacroPath->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroPath, labelGC->LH());

	fMacroArgs = new TGGroupFrame(this, "Arguments", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroArgs);
	this->AddFrame(fMacroArgs, frameGC->LH());

	TGLayoutHints * entryLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(entryLayout);
	entryGC->SetLH(entryLayout);

	TGLayoutHints * comboLayout = new TGLayoutHints(kLHintsExpandY, 5, 1, 3, 3);
	HEAP(comboLayout);
	comboGC->SetLH(comboLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	labelGC->SetBG(gray);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
	for (Int_t i = 0; i < nofArgs; i++) {
		macroArg = new TGMrbMacroArg(i + 1);
		fLofMacroArgs.Add(macroArg);
		argTitle = macroEnv->GetValue(macroArg->GetResource(argName, "Title"), "");
		argType = macroEnv->GetValue(macroArg->GetResource(argName, "Type"), "Char_t *");
		if ((macroArg->fType = fLofArgTypes.FindByName(argType.Data(), TMrbLofNamedX::kFindExact)) == NULL) {
			gMrbLog->Err() << Macro->GetName() << ": Illegal argument type - " << argType << endl;
			gMrbLog->Flush(this->ClassName());
			macroArg->fType = fLofArgTypes.FindByIndex(TGMrbMacroArg::kGMrbMacroArgChar);
		}
		argEntryType = macroEnv->GetValue(macroArg->GetResource(argName, "EntryType"), "Entry");
		if ((macroArg->fEntryType =fLofEntryTypes.FindByName(argEntryType.Data(), TMrbLofNamedX::kFindExact)) == NULL) {
			gMrbLog->Err() << Macro->GetName() << ": Illegal entry type - " << argEntryType	<< endl;
			gMrbLog->Flush(this->ClassName());
			macroArg->fEntryType = fLofEntryTypes.FindByIndex(TGMrbMacroArg::kGMrbMacroEntryPlain);
		}

		addLofValues = macroEnv->GetValue(macroArg->GetResource(argName, "AddLofValues"), "No");
		macroArg->fAddLofValues = (addLofValues.CompareTo("Yes") == 0);

		defaultEntryWidth = (macroArg->fType->GetIndex() == TGMrbMacroArg::kGMrbMacroArgChar) ? 250 : 150;
		if (macroArg->fEntryType->GetIndex() == TGMrbMacroArg::kGMrbMacroEntryFile) defaultEntryWidth = 300;
		macroArg->fEntryWidth = macroEnv->GetValue(macroArg->GetResource(argName, "Width"), defaultEntryWidth);

		currentValue = macroEnv->GetValue(macroArg->GetResource(argName, "Current"), "");
		currentValue = currentValue.Strip(TString::kBoth);
		macroArg->fCurrentValue = currentValue;

		defaultValue = macroEnv->GetValue(macroArg->GetResource(argName, "Default"), "");
		defaultValue = defaultValue.Strip(TString::kBoth);
		if (defaultValue(0) == '<') {
			defaultValue = defaultValue(1, defaultValue.Length() - 1);
			defaultValue = gEnv->GetValue(defaultValue.Data(), "");
		} else if (defaultValue(0) == '@') {
			defaultValue = defaultValue(1, defaultValue.Length() - 1);
			switch (macroArg->fType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroArgInt:		cintString = "macroArgInt = "; break;
				case TGMrbMacroArg::kGMrbMacroArgUInt:		cintString = "macroArgUInt = "; break;
				case TGMrbMacroArg::kGMrbMacroArgFloat:		cintString = "macroArgFloat = "; break;
				case TGMrbMacroArg::kGMrbMacroArgDouble:	cintString = "macroArgDouble = "; break;
				case TGMrbMacroArg::kGMrbMacroArgBool:		cintString = "macroArgBool = "; break;
				default:									cintString = "macroArgString = "; break;
			}
			cintString += defaultValue.Data();
			gROOT->ProcessLine(cintString.Data());
		}

		macroArg->fDefaultValue = defaultValue;
		gSystem->ExpandPathName(macroArg->fDefaultValue);

		Int_t n = macroArg->fEntryType->GetIndex();
		if (n == TGMrbMacroArg::kGMrbMacroEntryPlain ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDown ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDownX) {
			xUpDown = (n == TGMrbMacroArg::kGMrbMacroEntryUpDownX);
			updownGC = (n == TGMrbMacroArg::kGMrbMacroEntryUpDown || xUpDown) ? labelGC : NULL;
			macroArg->fEntry = new TGMrbLabelEntry(fMacroArgs, argTitle.Data(),
														40, TGMrbMacroFrame::kEntryId + i + 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														macroArg->fEntryWidth,
														frameGC, labelGC, entryGC, updownGC, xUpDown);
			HEAP(macroArg->fEntry);
			fMacroArgs->AddFrame(macroArg->fEntry, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			macroArg->fEntry->GetEntry()->SetText(value.Data());

			TGMrbLabelEntry::EGMrbEntryType at = TGMrbLabelEntry::kGMrbEntryTypeCharInt;
			switch (macroArg->fType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroArgInt:
				case TGMrbMacroArg::kGMrbMacroArgUInt:
				case TGMrbMacroArg::kGMrbMacroArgBool:		at = TGMrbLabelEntry::kGMrbEntryTypeInt; break;
				case TGMrbMacroArg::kGMrbMacroArgFloat:
				case TGMrbMacroArg::kGMrbMacroArgDouble:	at = TGMrbLabelEntry::kGMrbEntryTypeDouble; break;
			}
			intBase = macroEnv->GetValue(macroArg->GetResource(argName, "Base"), 10);
			macroArg->fEntry->SetType(at, 0, intBase);

			if (n == TGMrbMacroArg::kGMrbMacroEntryUpDown || n == TGMrbMacroArg::kGMrbMacroEntryUpDownX) {
				macroArg->fLowerLimit = macroEnv->GetValue(macroArg->GetResource(argName, "LowerLimit"), 0.0);
				macroArg->fUpperLimit = macroEnv->GetValue(macroArg->GetResource(argName, "UpperLimit"), 1000000.0);
				macroArg->fIncrement = macroEnv->GetValue(macroArg->GetResource(argName, "Increment"), 1.0);
				macroArg->fEntry->SetRange(macroArg->fLowerLimit, macroArg->fUpperLimit);
				macroArg->fEntry->SetIncrement(macroArg->fIncrement);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			macroArg->fButtons.Delete();
			macroArg->fButtons.SetPatternMode();
			macroArg->fButtons.AddNamedX(kGMrbMacroYesNoButtons);
			macroArg->fRadio = new TGMrbRadioButtonList(fMacroArgs, argTitle.Data(),
														&macroArg->fButtons, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
			HEAP(macroArg->fRadio);
			fMacroArgs->AddFrame(macroArg->fRadio, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			if (value.CompareTo("Yes") == 0) {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryYes, kButtonDown);
			} else {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryRadio
						|| n == TGMrbMacroArg::kGMrbMacroEntryCheck
						|| n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			argString = macroEnv->GetValue(macroArg->GetResource(argName, "Orientation"), "");
			if (argString.CompareTo("vertical") == 0)	macroArg->fOrientation = kVerticalFrame;
			else								macroArg->fOrientation = kHorizontalFrame;
			macroArg->fNofCL = macroEnv->GetValue(macroArg->GetResource(argName, "NofCL"), 1);
			argString = macroEnv->GetValue(macroArg->GetResource(argName, "Values"), "");
			argString = argString.Strip(TString::kBoth);
			if (argString.Length() == 0) {
				gMrbLog->Err()	<< Macro->GetName() << ": " << macroArg->fEntryType->GetName()
								<< ": Values missing" << endl;
				gMrbLog->Flush(this->ClassName());
			} else {
				Int_t m = 1;
				macroArg->fButtons.Delete();
				macroArg->fButtons.SetPatternMode();
				lofSubstrings.Delete();
				Int_t nstr = argString.Split(lofSubstrings);
				for (Int_t nn = 0; nn < nstr; nn++) {
					TMrbString str = ((TObjString *) lofSubstrings[nn])->GetString().Data();
					intBase = macroEnv->GetValue(macroArg->GetResource(argName, "Base"), 10);
					Int_t nsep = str.Index("=", 0);
					TString tip = "";
					if (nsep > 0) {
						UInt_t at = macroArg->fType->GetIndex();
						if (at == TGMrbMacroArg::kGMrbMacroArgInt || at == TGMrbMacroArg::kGMrbMacroArgUInt) {
							str.SplitOffInteger(prefix, m, intBase);
							str.Resize(nsep);
						} else {
							tip = str(nsep + 1, str.Length());
							str.Resize(nsep);
						}
					}
					macroArg->fButtons.AddNamedX(new TMrbNamedX(m, str.Data(), (tip.Length() > 0) ? tip.Data() : NULL));
					m <<= 1;
				}
				if (n == TGMrbMacroArg::kGMrbMacroEntryRadio) {
					macroArg->fRadio = new TGMrbRadioButtonList(fMacroArgs, argTitle.Data(),
														&macroArg->fButtons, -1, macroArg->fNofCL,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC, macroArg->fOrientation);
					HEAP(macroArg->fRadio);
					fMacroArgs->AddFrame(macroArg->fRadio, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					if ((button = macroArg->fButtons.FindByName(value.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
						macroArg->fRadio->SetState(button->GetIndex(), kButtonDown);
					}
				} else if (n == TGMrbMacroArg::kGMrbMacroEntryCheck) {
					macroArg->fCheck = new TGMrbCheckButtonList(fMacroArgs, argTitle.Data(),
														&macroArg->fButtons, -1, macroArg->fNofCL,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC,
														lofSpecialButtons,
														macroArg->fOrientation);
					HEAP(macroArg->fCheck);
					fMacroArgs->AddFrame(macroArg->fCheck, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					UInt_t pattern = macroArg->fButtons.FindPattern(value.Data());
					if (pattern != TMrbLofNamedX::kIllIndexBit) {
						macroArg->fCheck->SetState(pattern, kButtonDown);
					}
				} else if (n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
					macroArg->fCombo = new TGMrbLabelCombo(fMacroArgs, argTitle.Data(), &macroArg->fButtons,
														TGMrbMacroFrame::kEntryId + i + 1, -1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														macroArg->fEntryWidth,
														frameGC, labelGC, comboGC);
					HEAP(macroArg->fCombo);
					fMacroArgs->AddFrame(macroArg->fCombo, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					if ((button = macroArg->fButtons.FindByName(value.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
						macroArg->fCombo->GetComboBox()->Select(button->GetIndex());
						macroArg->fCombo->SetText(button->GetName());
					}
				}
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFile) {
			memset(macroArg->fPtrFileTypes, 0, 2 * TGMrbMacroArg::kGMrbMacroNofFileTypes * sizeof(Char_t *));
			argString = macroEnv->GetValue(macroArg->GetResource(argName, "Values"), "");
			argString = argString.Strip(TString::kBoth);
			nft = 0;
			if (argString.Length() != 0) {
				macroArg->fFileTypes.Delete();
				nft = argString.Split(macroArg->fFileTypes);
				for (Int_t nn = 0; nn < nft; nn++) macroArg->fPtrFileTypes[nn] = ((TObjString *) macroArg->fFileTypes[nn])->GetString();
			}
			if (nft % 2) {
				gMrbLog->Err()	<< Macro->GetName() << ": " << macroArg->fEntryType->GetName()
								<< ": File types have to be pairs of strings" << endl;
				gMrbLog->Flush(this->ClassName());
				nft = 0;
			}
			macroArg->fPtrFileTypes[nft++] = "All files";
			macroArg->fPtrFileTypes[nft] = "*";
			macroArg->fFileInfo->fIniDir = (Char_t *) gSystem->WorkingDirectory();
			macroArg->fFileInfo->fFileTypes = (const Char_t **) macroArg->fPtrFileTypes;
			macroArg->fFile = new TGMrbFileEntry(fMacroArgs, argTitle.Data(), 100,
												TGMrbMacroFrame::kEntryId + i + 1,
												frameWidth - 20,
												TGMrbMacroFrame::kLineHeight,
												macroArg->fEntryWidth,
												macroArg->fFileInfo, kFDOpen,
												frameGC, labelGC, entryGC, labelGC);
			HEAP(macroArg->fFile);
			fMacroArgs->AddFrame(macroArg->fFile, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			macroArg->fFile->GetEntry()->SetText(value.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			macroArg->fFObjCombo = new TGMrbFileObjectCombo(fMacroArgs, argTitle.Data(), 100,
												TGMrbMacroFrame::kEntryId + i + 1,
												TGMrbMacroFrame::kComboId + i + 1,
												frameWidth - 20,
												TGMrbMacroFrame::kLineHeight,
												macroArg->fEntryWidth,
												macroArg->fEntryWidth,
												frameGC, labelGC, entryGC, labelGC, entryGC);
			HEAP(macroArg->fFObjCombo);
			fMacroArgs->AddFrame(macroArg->fFObjCombo, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			macroArg->fFObjCombo->SetFileEntry(value.Data());
		}
	}

	TGLayoutHints * actionLayout = new TGLayoutHints(kLHintsExpandX, 5, 1, 3, 1);
	HEAP(actionLayout);
	buttonGC->SetLH(actionLayout);

	fAction = new TGMrbTextButtonGroup(this, "Action", &fLofActions, -1, 2, frameGC, buttonGC);
	HEAP(fAction);
	this->AddFrame(fAction, frameGC->LH());
	fAction->Associate(this);

	this->ChangeBackground(blue);
	this->SetWindowName(titleBar.Data());

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() + 10), 0, ax, ay, wdum);
	this->Move(ax, ay);

	this->MapSubwindows();
	this->Resize(this->GetDefaultSize());

	this->Resize(frameWidth, fMacroInfo->GetDefaultHeight()
								+ fMacroArgs->GetDefaultHeight()
								+ fAction->GetDefaultHeight() + 20);

	this->MapWindow();

	gClient->WaitFor(this);
}

Bool_t TGMrbMacroFrame::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ProcessMessage
// Purpose:        Message handler
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroFrame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case TGMrbMacroFrame::kGMrbMacroIdModifyHeader:
							this->ModifyMacroHeader();
							break;
						case TGMrbMacroFrame::kGMrbMacroIdModifySource:
							this->ModifyMacroSource();
							break;
						case TGMrbMacroFrame::kGMrbMacroIdClose:
							this->CloseWindow();
							break;
						case TGMrbMacroFrame::kGMrbMacroIdReset:
							this->ResetMacroArgs();
							break;
						case TGMrbMacroFrame::kGMrbMacroIdExec:
							this->ExecMacro();
							break;
						case TGMrbMacroFrame::kGMrbMacroIdExecClose:
							this->ExecMacro();
							this->CloseWindow();
							break;
					}
					break;

				default:	break;
			}
			break;

		default:	break;
	}
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ResetMacroArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ResetMacroArgs
// Purpose:        Reset arguments
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Resets macro arguments
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg * macroArg;

	TMrbNamedX *button;

	macroArg = (TGMrbMacroArg *) fLofMacroArgs.First();
	while (macroArg) {
		Int_t n = macroArg->fEntryType->GetIndex();
		if (n == TGMrbMacroArg::kGMrbMacroEntryPlain ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDown ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDownX) {
			macroArg->fEntry->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			if (macroArg->fDefaultValue.CompareTo("Yes") == 0) {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryYes, kButtonDown);
			} else {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryRadio) {
			if ((button = macroArg->fButtons.FindByName(macroArg->fDefaultValue.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
				macroArg->fRadio->SetState(button->GetIndex(), kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCheck) {
			UInt_t pattern = macroArg->fButtons.FindPattern(macroArg->fDefaultValue.Data());
			macroArg->fCheck->SetState(0xFFFFFFFF, kButtonUp);
			if (pattern != TMrbLofNamedX::kIllIndexBit) {
				macroArg->fCheck->SetState(pattern, kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			if ((button = macroArg->fButtons.FindByName(macroArg->fDefaultValue.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
				macroArg->fCombo->GetComboBox()->Select(button->GetIndex());
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFile) {
			macroArg->fFile->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			macroArg->fFObjCombo->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		}
		macroArg = (TGMrbMacroArg *) fLofMacroArgs.After(macroArg);
	}
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ExecMacro() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ExecMacro
// Purpose:        Execute macro
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes macro with given arguments
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg * macroArg;

	UInt_t buttonBits;
	TMrbString argString;
	TString cmd, delim;
	TEnv * macroEnv;
	TMrbString currentValue, cVal;
	UInt_t argType;

	ostringstream * cmdString = new ostringstream();

	macroEnv = (TEnv *) fMacro->GetAssignedObject();
	gROOT->LoadMacro(macroEnv->GetValue("Path", "UNKNOWN"));

	cmd = fMacroName->GetText()->GetString();
	cmd = cmd(0, cmd.Index(".C"));
	*cmdString << cmd;
	delim = "(";

	macroArg = (TGMrbMacroArg *) fLofMacroArgs.First();
	while (macroArg) {
		Int_t n = macroArg->fEntryType->GetIndex();
		if (n == TGMrbMacroArg::kGMrbMacroEntryPlain ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDown ||
			n == TGMrbMacroArg::kGMrbMacroEntryUpDownX) {
			argString = macroArg->fEntry->GetEntry()->GetText();
			currentValue = argString;
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			buttonBits = macroArg->fRadio->GetActive();
			currentValue = macroArg->fButtons.FindByIndex(buttonBits)->GetName();
			if (currentValue.CompareTo("Yes") == 0) {
				argString = "kTRUE";
			} else {
				argString = "kFALSE";
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryRadio) {
			buttonBits = macroArg->fRadio->GetActive();
			currentValue = macroArg->fButtons.FindByIndex(buttonBits)->GetName();
			argType = macroArg->fType->GetIndex();
			if (argType == TGMrbMacroArg::kGMrbMacroArgInt || argType == TGMrbMacroArg::kGMrbMacroArgUInt) {
				argString.FromInteger(buttonBits, 0, 16, kTRUE, kTRUE);
			} else {
				argString = currentValue;
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCheck) {
			buttonBits = macroArg->fCheck->GetActive();
			argString.FromInteger(buttonBits, 0, 16, kTRUE, kTRUE);
			macroArg->fButtons.Pattern2String(currentValue, buttonBits);
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			argString = ((TGTextLBEntry *) macroArg->fCombo->GetComboBox()->GetSelectedEntry())->GetText()->GetString();
			currentValue = argString;
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFile) {
			argString = macroArg->fFile->GetEntry()->GetText();
			currentValue = argString;
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			argString = macroArg->fFObjCombo->GetSelection(argString);
			currentValue = argString;
		}
		if (macroArg->fType->GetIndex() == TGMrbMacroArg::kGMrbMacroArgChar) {
			argString.Prepend("\"");
			argString += "\"";
		}
		*cmdString << delim << argString;

		macroArg->GetResource(cVal, "Current");
		cVal += "=";
		cVal += currentValue;
		macroEnv->SetValue(cVal, kEnvChange);
		delim = ", ";

		if (macroArg->fAddLofValues) {
			argString = macroEnv->GetValue(macroArg->GetResource(argString, "Values"), "");
			*cmdString << delim << "\"" << argString << "\"";
		}
		macroArg = (TGMrbMacroArg *) fLofMacroArgs.After(macroArg);
	}
	*cmdString << ");" << ends;
	gROOT->ProcessLine(cmdString->str().c_str());
//	cmdString->rdbuf()->freeze(0);
	delete cmdString;
	G__unloadfile((Char_t *) macroEnv->GetValue("Path", "UNKNOWN"));
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ModifyMacroSource() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ModifyMacroSource
// Purpose:        Modify macro source
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Calls "nedit" to modifiy macro source.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cmd;
	TEnv * macroEnv;

	macroEnv = (TEnv *) fMacro->GetAssignedObject();
	cmd = "nedit ";
	cmd += macroEnv->GetValue("Path", "UNKNOWN");
	cmd += " &";
	gSystem->Exec(cmd.Data());
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ModifyMacroHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ModifyMacroHeader
// Purpose:        Modify macro args & layout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Modifies macro arguments and layout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	new TGMrbMacroEdit(fClient->GetRoot(), this, fMacro, 100, 100);
	this->CloseWindow();
	return(kTRUE);
}

TGMrbMacroEdit::TGMrbMacroEdit(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro,
										UInt_t Width, UInt_t Height, UInt_t Options) :
													TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit
// Purpose:        A fransient frame to edit macro args 
// Arguments:      TGWindow * Parent     -- parent window
//                 TGWindow * Main       -- main window
//                 TMrbNamedX * Macro    -- macro specs
//                 UInt_t Width          -- width
//                 UInt_t Height         -- height
//                 UInt_t Options        -- options
// Results:        
// Exceptions:     
// Description:    Edits macro arguments and layout.
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=gif/TGMrbMacroEdit.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

	TString macroName, macroPath, titleBar;
	TMrbString argValue;


	UInt_t frameWidth;

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
//	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(font, black, gray);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, blue);
	HEAP(frameGC);
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * entryGC = new TGMrbLayout(font, black, white);
	HEAP(entryGC);
	TGMrbLayout * comboGC = new TGMrbLayout(font, black, white);
	HEAP(comboGC);

//	Initialize several lists
	fLofArgTypes.SetName("Argument types");
	fLofArgTypes.AddNamedX(kGMrbMacroLofArgTypes);
	fLofEntryTypes.SetName("Entry types");
	fLofEntryTypes.AddNamedX(kGMrbMacroLofEntryTypes);
	fLofArgActions.SetName("Action buttons");
	fLofArgActions.AddNamedX(kGMrbMacroLofEditArgActions);
	fLofActions.SetName("Action buttons");
	fLofActions.AddNamedX(kGMrbMacroLofEditActions);
	fLofOrientations.SetName("Entry orientations");
	fLofOrientations.AddNamedX(kGMrbMacroLofOrientations);
	fLofIntegerBases.SetName("Integer base values");
	fLofIntegerBases.AddNamedX(kGMrbMacroLofIntegerBases);
	fYesNo.SetName("Yes or No");
	fYesNo.AddNamedX(kGMrbMacroYesNoButtons);

	fMacro = Macro; 		// save pointer to macro

	TMrbLofMacros * m = new TMrbLofMacros();			// get list of env names
	fLofEnvNames.AddNamedX(m->GetLofEnvNames());
	delete m;

	fOriginalEnv = (TEnv *) fMacro->GetAssignedObject(); // original settings
	fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
	fCurrentEnv = new TEnv(".listOfArgs");
	this->RestoreHeader();								// copy from original to current
	this->RestoreAllArgs();

	frameWidth = TGMrbMacroEdit::kFrameWidth;

	macroName = Macro->GetName();
	titleBar = "Edit ROOT Macro: ";
	titleBar += macroName;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 3);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);
	fMacroInfo = new TGGroupFrame(this, "Info", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroInfo);
	this->AddFrame(fMacroInfo, frameGC->LH());

	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 80, 1, 3, 3);
	HEAP(labelLayout);
	labelGC->SetLH(labelLayout);

	TGLayoutHints * entryLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(entryLayout);
	entryGC->SetLH(entryLayout);

	TGLayoutHints * comboLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 1, 3, 3);
	HEAP(comboLayout);
	comboGC->SetLH(comboLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroName = new TGMrbLabelEntry(fMacroInfo, "Name", 80, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fMacroName);
	fMacroInfo->AddFrame(fMacroName, frameGC->LH());
	fMacroName->GetEntry()->SetText(Macro->GetName());

	fMacroTitle = new TGMrbLabelEntry(fMacroInfo, "Title", 80, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fMacroTitle);
	fMacroInfo->AddFrame(fMacroTitle, frameGC->LH());
	fMacroTitle->GetEntry()->SetText(fOriginalEnv->GetValue("Title", ""));

	fMacroWidth = new TGMrbLabelEntry(fMacroInfo, "Width", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC, labelGC);
	HEAP(fMacroWidth);
	fMacroInfo->AddFrame(fMacroWidth, frameGC->LH());
	fMacroWidth->SetRange(0, 1000);
	fMacroWidth->SetIncrement(100);
	fMacroWidth->GetEntry()->SetText(fOriginalEnv->GetValue("Width", ""));

	fMacroNofArgs = new TGMrbLabelEntry(fMacroInfo, "Number of arguments", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fMacroNofArgs);
	fMacroInfo->AddFrame(fMacroNofArgs, frameGC->LH());

	fMacroArg = new TGGroupFrame(this, "Argument", kHorizontalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroArg);
	this->AddFrame(fMacroArg, frameGC->LH());

	TGLayoutHints * curargLayout = new TGLayoutHints(kLHintsLeft, 5, 1, 3, 3);
	HEAP(curargLayout);

	fArgNumber = new TGMrbLabelEntry(fMacroArg, "Current", 40, TGMrbMacroEdit::kGMrbMacroEditCurrentArg,
														(frameWidth - 20) / 5,
														TGMrbMacroEdit::kLineHeight, 120,
														frameGC, labelGC, entryGC, labelGC, kTRUE);
	HEAP(fArgNumber);
	fMacroArg->AddFrame(fArgNumber, curargLayout);
	fArgNumber->Associate(this);
	fArgNumber->SetRange(1, TGMrbMacroEdit::kMaxNofArgs);
	fArgNumber->SetIncrement(1);

	fArgAction = new TGMrbTextButtonList(fMacroArg, NULL, &fLofArgActions, -1, 1, frameWidth / 2, TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgAction);
	fMacroArg->AddFrame(fArgAction, frameGC->LH());
	fArgAction->Associate(this);

	fMacroLayout = new TGGroupFrame(this, "Layout", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroLayout);
	this->AddFrame(fMacroLayout, frameGC->LH());

	fArgName = new TGMrbLabelEntry(fMacroLayout, "Name", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgName);
	fMacroLayout->AddFrame(fArgName, frameGC->LH());

	fArgTitle = new TGMrbLabelEntry(fMacroLayout, "Title", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgTitle);
	fMacroLayout->AddFrame(fArgTitle, frameGC->LH());

	fArgType = new TGMrbRadioButtonList(fMacroLayout, "Type",
														&fLofArgTypes, -1, 3,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgType);
	fMacroLayout->AddFrame(fArgType, frameGC->LH());

	fArgEntryType = new TGMrbRadioButtonList(fMacroLayout, "Entry type",
														&fLofEntryTypes, -1, 3,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgEntryType);
	fMacroLayout->AddFrame(fArgEntryType, frameGC->LH());

	fArgEntryWidth = new TGMrbLabelEntry(fMacroLayout, "Entry width", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC, labelGC);
	HEAP(fArgEntryWidth);
	fMacroLayout->AddFrame(fArgEntryWidth, frameGC->LH());
	fArgEntryWidth->SetRange(0, 1000);
	fArgEntryWidth->SetIncrement(100);

	fArgDefaultValue = new TGMrbLabelEntry(fMacroLayout, "Default value", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgDefaultValue);
	fMacroLayout->AddFrame(fArgDefaultValue, frameGC->LH());

	fArgValues = new TGMrbLabelEntry(fMacroLayout, "Values", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgValues);
	fMacroLayout->AddFrame(fArgValues, frameGC->LH());

	fArgAddLofValues = new TGMrbRadioButtonList(fMacroLayout, "Add list of values",
														&fYesNo, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgAddLofValues);
	fMacroLayout->AddFrame(fArgAddLofValues, frameGC->LH());

	fArgLowerLimit = new TGMrbLabelEntry(fMacroLayout, "Lower limit", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fArgLowerLimit);
	fMacroLayout->AddFrame(fArgLowerLimit, frameGC->LH());

	fArgUpperLimit = new TGMrbLabelEntry(fMacroLayout, "Upper limit", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fArgUpperLimit);
	fMacroLayout->AddFrame(fArgUpperLimit, frameGC->LH());

	fArgIncrement = new TGMrbLabelEntry(fMacroLayout, "Increment", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fArgIncrement);
	fMacroLayout->AddFrame(fArgIncrement, frameGC->LH());

	fArgBase = new TGMrbRadioButtonList(fMacroLayout, "Base",
														&fLofIntegerBases, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgBase);
	fMacroLayout->AddFrame(fArgBase, frameGC->LH());

	fArgOrientation = new TGMrbRadioButtonList(fMacroLayout, "Orientation",
														&fLofOrientations, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgOrientation);
	fMacroLayout->AddFrame(fArgOrientation, frameGC->LH());

	fCurrentArg.SetNumber(1);
	argValue = 1;
	fArgNumber->GetEntry()->SetText(argValue.Data());
	this->UpdateArg();
	this->UpdateNofArgs();

	TGLayoutHints * actionLayout = new TGLayoutHints(kLHintsExpandX, 5, 1, 3, 1);
	HEAP(actionLayout);
	buttonGC->SetLH(actionLayout);

	fAction = new TGMrbTextButtonGroup(this, "Macro action", &fLofActions, -1, 1, frameGC, buttonGC);
	HEAP(fAction);
	this->AddFrame(fAction, frameGC->LH());
	fAction->Associate(this);

	this->ChangeBackground(blue);
	this->SetWindowName(titleBar.Data());

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() + 10), 0, ax, ay, wdum);
	this->Move(ax, ay);

	this->MapSubwindows();
	this->Resize(this->GetDefaultSize());

	this->Resize(frameWidth, fMacroInfo->GetDefaultHeight()
								+ fMacroArg->GetDefaultHeight()
								+ fMacroLayout->GetDefaultHeight()
								+ fAction->GetDefaultHeight()
								+ 20);

	this->MapWindow();

	gClient->WaitFor(this);
}

Bool_t TGMrbMacroEdit::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::ProcessMessage
// Purpose:        Message handler
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroEdit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argValue;
	Int_t intVal;

	TGFileInfo fo;

	TMrbLofMacros lofMacros;
	TMrbNamedX * macro;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case kGMrbMacroEditIdRemoveArg:
							this->RemoveArg();
							this->UpdateNofArgs();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdResetArg:
							this->RestoreArg();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdNewArgBefore:
							this->StoreArg();
							this->InsertArg(fCurrentArg.GetNumber());
							this->UpdateNofArgs();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdNewArgAfter:
							this->StoreArg();
							this->InsertArg(fCurrentArg.GetNumber() + 1);
							this->UpdateNofArgs();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdResetAll:
							this->RestoreHeader();
							fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
							this->UpdateNofArgs();
							this->RestoreAllArgs();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdResetHeader:
							this->RestoreHeader();
							break;
						case kGMrbMacroEditIdResetAllArgs:
							this->RestoreAllArgs();
							fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
							this->UpdateNofArgs();
							this->UpdateArg();
							break;
						case kGMrbMacroEditIdSave:
							fo.fFileTypes = (const char **) macroFileTypes;
							new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fo);
							if (fo.fFilename != NULL && *fo.fFilename != '\0') {
								if (this->SaveMacro(fo.fFilename)) {
									if (lofMacros.AddMacro(fo.fFilename)) {
										macro = lofMacros.FirstMacro();
										fMacro->GetAssignedObject()->Delete();
										fMacro->AssignObject(macro->GetAssignedObject());
										this->CloseWindow();
									}
								}
							}
							break;
						case kGMrbMacroEditIdClose:
							this->CloseWindow();
							break;
					}
					break;

				default:	break;
			}
			break;

		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_TEXTCHANGED:
				case kTE_ENTER:
					this->StoreArg();
					argValue = fArgNumber->GetEntry()->GetText();
					if (argValue.ToInteger(intVal)) {
						if (intVal < 1) {
							this->StoreArg();
							fCurrentArg.SetNumber(1); 
							argValue = 1;
							fArgNumber->GetEntry()->SetText(argValue.Data());
							this->UpdateArg();
						} else if (intVal > fNofArgs) {
							this->StoreArg();
							fCurrentArg.SetNumber(fNofArgs); 
							argValue = fNofArgs;
							fArgNumber->GetEntry()->SetText(argValue.Data());
							this->UpdateArg();
						} else {
							this->StoreArg();
							fCurrentArg.SetNumber(intVal); 
							this->UpdateArg();
						}
					}
					break;
			}
			break;
		default:	break;
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::UpdateArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::UpdateArg
// Purpose:        Update GUI for an argument
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Updates GUI fields for a given argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argValue, argType, argEntryType, argBase, argOrientation;

	TMrbNamedX * nx;
	Int_t idx;

	TGMrbMacroArg thisArg;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetNumber();
	thisArg.SetNumber(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateArg");
		return(kFALSE);
	}


	fArgName->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Name"), ""));
	fArgTitle->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Title"), ""));

	argType = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Type"), "");
	nx = fLofArgTypes.FindByName(argType.Data());
	idx = nx ? nx->GetIndex() : 0;
	fArgType->SetState(idx, kButtonDown);

	argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "EntryType"), "");
	nx = fLofEntryTypes.FindByName(argEntryType.Data());
	idx = nx ? nx->GetIndex() : 0;
	fArgEntryType->SetState(idx, kButtonDown);

	fArgEntryWidth->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Width"), ""));
	fArgDefaultValue->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Default"), ""));
	fArgValues->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Values"), ""));
	argValue = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "AddLofValues"), "No");
	nx = fYesNo.FindByName(argValue.Data());
	if (nx) fArgAddLofValues->SetState(nx->GetIndex(), kButtonDown);
	else	fArgAddLofValues->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
	fArgLowerLimit->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "LowerLimit"), ""));
	fArgUpperLimit->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "UpperLimit"), ""));
	fArgIncrement->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Increment"), ""));

	argBase = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Base"), "");
	nx = fLofEntryTypes.FindByName(argBase.Data());
	idx = nx ? nx->GetIndex() : 1;
	fArgBase->SetState(idx, kButtonDown);

	argOrientation = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Orientation"), "");
	nx = fLofEntryTypes.FindByName(argOrientation.Data());
	idx = nx ? nx->GetIndex() : 1;
	fArgOrientation->SetState(idx, kButtonDown);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::StoreHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::StoreHeader
// Purpose:        Xfer entry data from GUI to environment
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stores GUI data to environment.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue;

	argValue = fMacroName->GetEntry()->GetText();
	fCurrentEnv->SetValue("Name", argValue.Data(), kEnvChange);
	argValue = fMacroTitle->GetEntry()->GetText();
	fCurrentEnv->SetValue("Title", argValue.Data(), kEnvChange);
	argValue = fMacroWidth->GetEntry()->GetText();
	fCurrentEnv->SetValue("Width", argValue.Data(), kEnvChange);
	argValue = fMacroNofArgs->GetEntry()->GetText();
	fCurrentEnv->SetValue("NofArgs", argValue.Data(), kEnvChange);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::StoreArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::StoreArg
// Purpose:        Xfer entry data from GUI to environment
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stores GUI data to environment.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argName, argValue, argType, argEntryType, argBase, argOrientation;

	TMrbNamedX * nx;
	Int_t idx;

	TGMrbMacroArg thisArg;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetNumber();
	thisArg.SetNumber(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "StoreArg");
		return(kFALSE);
	}

	argValue = fArgName->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Name"), argValue.Data(), kEnvChange);
	argValue = fArgTitle->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Title"), argValue.Data(), kEnvChange);

	idx = fArgType->GetActive();
	nx  = fLofArgTypes.FindByIndex(idx);
	argType = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Type"), argType.Data(), kEnvChange);

	idx = fArgEntryType->GetActive();
	nx = fLofEntryTypes.FindByIndex(idx);
	argEntryType = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "EntryType"), argEntryType.Data(), kEnvChange);

	argValue = fArgEntryWidth->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Width"), argValue.Data(), kEnvChange);
	argValue = fArgDefaultValue->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Default"), argValue.Data(), kEnvChange);
	argValue = fArgValues->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Values"), argValue.Data(), kEnvChange);
	nx = fYesNo.FindByIndex(fArgAddLofValues->GetActive());
	if (nx) argValue = nx->GetName(); else argValue = "No";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "AddLofValues"), argValue.Data(), kEnvChange);
	argValue = fArgLowerLimit->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "LowerLimit"), argValue.Data(), kEnvChange);
	argValue = fArgUpperLimit->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "UpperLimit"), argValue.Data(), kEnvChange);
	argValue = fArgIncrement->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Increment"), argValue.Data(), kEnvChange);

	idx = fArgBase->GetActive();
	nx = fLofIntegerBases.FindByIndex(idx);
	argBase = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Base"), argBase.Data(), kEnvChange);

	idx = fArgOrientation->GetActive();
	nx = fLofOrientations.FindByIndex(idx);
	argOrientation = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Orientation"), argOrientation.Data(), kEnvChange);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RemoveArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RemoveArg
// Purpose:        Remove an argument and its environment definitions
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Removes a given argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argValue;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetNumber();

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "RemoveArg");
		return(kFALSE);
	}

	if (this->ChangeEnv(ArgNo, kTRUE)) {
		fNofArgs--;
		if (fNofArgs <= 0) {
			this->InsertArg(1);
			fCurrentArg.SetNumber(1);
			argValue = fNofArgs;
			fArgNumber->GetEntry()->SetText(argValue.Data());
		} else if (fCurrentArg.GetNumber() > fNofArgs) {
			fCurrentArg.SetNumber(fNofArgs);
			argValue = fNofArgs;
			fArgNumber->GetEntry()->SetText(argValue.Data());
		}
		return(kTRUE);
	} else return(kFALSE);
}

Bool_t TGMrbMacroEdit::InsertArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::InsertArg
// Purpose:        Insert an argument and create empty environment definitions
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Inserts a new argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argValue;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetNumber();

	if (ArgNo < 1 || ArgNo > fNofArgs + 1) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs + 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "InsertArg");
		return(kFALSE);
	}

	if (this->ChangeEnv(ArgNo, kFALSE)) {
		fNofArgs++;
		fCurrentArg.SetNumber(ArgNo);
		argValue = ArgNo;
		fArgNumber->GetEntry()->SetText(argValue.Data());
		return(kTRUE);
	} else return(kFALSE);
}

Bool_t TGMrbMacroEdit::RestoreArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreArg
// Purpose:        Restore argument settings from original environment
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores an argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TGMrbMacroArg thisArg;
	TString envName, envVal, argEnv;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetNumber();
	thisArg.SetNumber(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreArg");
		return(kFALSE);
	}

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == 0) {
			envName = envName(4, envName.Length() - 4);
			thisArg.GetResource(argEnv, envName.Data());
			envVal = fOriginalEnv->GetValue(argEnv.Data(), "");
			fCurrentEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RestoreHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreHeader
// Purpose:        Restore header settings from original environment
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores header data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TString envName, envVal;

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == -1) {
			envVal = fOriginalEnv->GetValue(envName.Data(), "");
			fCurrentEnv->SetValue(envName.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RestoreAllArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreAllArgs
// Purpose:        Restore argument settings from original environment
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores all arguments.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = 1; i <= fNofArgs; i++) this->RestoreArg(i);
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::UpdateNofArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::UpdateNofArgs
// Purpose:        Update GUI entry "number of arguments"
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores all arguments.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString argValue = fNofArgs;
	fMacroNofArgs->GetEntry()->SetText(argValue.Data());
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::ChangeEnv(Int_t ArgNo, Bool_t Delete) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::ChangeEnv
// Purpose:        Change environment settings
// Arguments:      Int_t ArgNo      -- argument number
//                 Bool_t Delete    -- if kTRUE -> delete argument
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Change environment: delete / insert an argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg thisArg, nextArg;
	TString argEnv, envName, envVal;
	TMrbString argName;
	TEnv * newEnv;
	TMrbNamedX * nx;

	Int_t nofArgs = (Delete) ? fNofArgs : fNofArgs + 1;

	if (ArgNo < 1 || ArgNo > nofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << nofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ChangeEnv");
		return(kFALSE);
	}

	newEnv = new TEnv(".listOfArgs");

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == -1) {
			envVal = fCurrentEnv->GetValue(envName.Data(), "");
			newEnv->SetValue(envName.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}

	if (Delete) {
		for (Int_t i = 1; i < ArgNo; i++) {
			thisArg.SetNumber(i);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
		for (Int_t i = ArgNo + 1; i <= fNofArgs; i++) {
			thisArg.SetNumber(i);
			nextArg.SetNumber(i - 1);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(nextArg.GetResource(argEnv, envName.Data()), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
	} else {
		for (Int_t i = 1; i < ArgNo; i++) {
			thisArg.SetNumber(i);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
		thisArg.SetNumber(ArgNo);
		nx = (TMrbNamedX *) fLofEnvNames.First();
		while (nx) {
			envName = nx->GetName();
			if (envName.Index("Arg.", 0) == 0) {
				envName = envName(4, envName.Length() - 4);
				thisArg.GetResource(argEnv, envName.Data());
				newEnv->SetValue(argEnv.Data(), "", kEnvChange);
			}
			nx = (TMrbNamedX *) fLofEnvNames.After(nx);
		}
		for (Int_t i = ArgNo; i <= fNofArgs; i++) {
			thisArg.SetNumber(i);
			nextArg.SetNumber(i + 1);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(nextArg.GetResource(argEnv, envName.Data()), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
	}

	delete fCurrentEnv;
	fCurrentEnv = newEnv;
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::SaveMacro(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::SaveMacro
// Purpose:        Save macro settings
// Arguments:      Char_t * FileName   -- file name (.C)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Write macro definitions to a file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString templatePath;
	TString macroTemplateFile;
	ofstream macroStrm;
	ifstream orgStrm;
	TMrbTemplate macroTmpl;
	TMrbNamedX * macroTag;
	TMrbLofNamedX lofMacroTags;
	TString line;
	TGMrbMacroEdit::EGMrbMacroTag tagIdx;
	TString fileName, orgFile, macroName, macroType;
	TMrbSystem ux;
	TDatime dt;
	TGMrbMacroArg thisArg;
	TMrbNamedX * nx;

	Bool_t fileIsNew;
	Int_t yesno;

	TString argEnv, argName, argTitle, argType, argEntryType, argDefault, argDel, addLofValues;
	TString envName, envVal;
	TMrbString argSpace, envSpace, msgSpace;

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", "");
	if (templatePath.Length() == 0) templatePath = ".:$(MARABOU)/templates/macros";
	gSystem->ExpandPathName(templatePath);

	fileIsNew = kTRUE;
	if (!gSystem->AccessPathName(FileName)) {
		TString msg = this->ClassName();
		msg += "::SaveMacro()";
		new TGMsgBox(fClient->GetRoot(), this, msg, "File exists. Overwrite?", kMBIconQuestion, kMBYes | kMBNo, &yesno);
		if (yesno != 1) return(kFALSE);
		fileIsNew = kFALSE;
	}

	if (!fileIsNew) {
		orgFile = FileName;
		orgFile += ".org";
		gSystem->Unlink(orgFile.Data());
		gSystem->Rename(FileName, orgFile.Data());
	}

	ux.GetBaseName(fileName, FileName);
	macroName = fileName(0, fileName.Index("."));

	macroStrm.open(FileName, ios::out);
	if (!macroStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << FileName << endl;
		gMrbLog->Flush(this->ClassName(), "SaveMacro");
		return(kFALSE);
	}

	TString fileSpec;
	ux.Which(fileSpec, templatePath.Data(), "MacroBrowserTemplate.C");
	if (fileSpec.IsNull()) {
		gMrbLog->Err() << "No such file - " << templatePath << "/" << "MacroBrowserTemplate.C"	<< endl;
		gMrbLog->Flush(this->ClassName(), "Savemacro");
		return(kFALSE);
	}

	this->StoreHeader();
	this->StoreArg();

	macroTemplateFile = fileSpec;

	lofMacroTags.AddNamedX(kGMrbMacroLofTags);
	if (!macroTmpl.Open(macroTemplateFile, &lofMacroTags)) return(kFALSE);

	for (;;) {
		macroTag = macroTmpl.Next(line);
		if (macroTmpl.IsEof()) break;
		if (macroTmpl.IsError()) continue;
		if (macroTmpl.Status() & TMrbTemplate::kNoTag) {
			if (line.Index("//-") != 0) macroStrm << line << endl;
		} else {
			switch (tagIdx = (TGMrbMacroEdit::EGMrbMacroTag) macroTag->GetIndex()) {
				case TGMrbMacroEdit::kMacroTagFile:
					macroStrm << macroTmpl.Encode(line, fileName) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagName:
					macroStrm << macroTmpl.Encode(line, macroName) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagTitle:
					macroStrm << macroTmpl.Encode(line, fCurrentEnv->GetValue("Title", "")) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagAuthor:
					macroStrm << macroTmpl.Encode(line, gSystem->Getenv("USER")) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagCreationDate:
					macroStrm << macroTmpl.Encode(line, dt.AsString()) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagSyntax:
					for (i = 1; i <= fNofArgs; i++) {
						if (i == 1) macroTmpl.InitializeCode("%ARG1%");
						else		macroTmpl.InitializeCode("%ARGN%");
						thisArg.SetNumber(i);
						argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
						argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
						argSpace.Resize(0); argSpace.Fill(fileName.Length() + 5);
						macroTmpl.Substitute("$macroFile", fileName);
						macroTmpl.Substitute("$argName", argName);
						macroTmpl.Substitute("$argSpace", argSpace);
						if (argType.Index("Char", 0) == 0) argType.Prepend("const ");
						macroTmpl.Substitute("$argType", argType);
						argDel = (i == fNofArgs) ? ")" : ",";
						macroTmpl.Substitute("$argDel", argDel);
						macroTmpl.WriteCode(macroStrm);
					}
					break;
				case TGMrbMacroEdit::kMacroTagArguments:
					for (i = 1; i <= fNofArgs; i++) {
						if (i == 1) macroTmpl.InitializeCode("%ARG1%");
						else		macroTmpl.InitializeCode("%ARGN%");
						thisArg.SetNumber(i);
						argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
						argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
						argTitle = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Title"), "");
						argSpace.Resize(0); argSpace.Fill(25 - (argType.Length() + argName.Length() + 1));
						macroTmpl.Substitute("$argName", argName);
						macroTmpl.Substitute("$argType", argType);
						macroTmpl.Substitute("$argTitle", argTitle);
						macroTmpl.Substitute("$argSpace", argSpace);
						macroTmpl.WriteCode(macroStrm);
					}
					break;
				case TGMrbMacroEdit::kMacroTagExec:
					macroStrm << "//+Exec __________________________________________________[ROOT MACRO BROWSER]" << endl;
					nx = (TMrbNamedX *) fLofEnvNames.First();
					while (nx) {
						envName = nx->GetName();
						if (envName.Index("Arg.", 0) == -1) {
							if (envName.CompareTo("Path") != 0) {
								envVal = fCurrentEnv->GetValue(envName.Data(), "");
								envSpace.Resize(0); envSpace.Fill(20 - envName.Length());
								macroTmpl.InitializeCode();
								macroTmpl.Substitute("$envName", envName);
								macroTmpl.Substitute("$envVal", envVal);
								macroTmpl.Substitute("$envSpace", envSpace);
								macroTmpl.WriteCode(macroStrm);
							}
						}
						nx = (TMrbNamedX *) fLofEnvNames.After(nx);
					}
					for (i = 1; i <= fNofArgs; i++) {
						thisArg.SetNumber(i);
						nx = (TMrbNamedX *) fLofEnvNames.First();
						while (nx) {
							envName = nx->GetName();
							if (envName.Index("Arg.", 0) == 0) {
								envName = envName(4, envName.Length() - 4);
								thisArg.GetResource(argEnv, envName.Data());
								envVal = fCurrentEnv->GetValue(argEnv.Data(), "");
								if (envVal.Length() > 0) {
									envSpace.Resize(0); envSpace.Fill(20 - argEnv.Length());
									macroTmpl.InitializeCode();
									macroTmpl.Substitute("$envName", argEnv);
									macroTmpl.Substitute("$envVal", envVal);
									macroTmpl.Substitute("$envSpace", envSpace);
									macroTmpl.WriteCode(macroStrm);
								}
							}
							nx = (TMrbNamedX *) fLofEnvNames.After(nx);
						}
					}
					macroStrm << "//-Exec" << endl;
					break;
				case TGMrbMacroEdit::kMacroTagIncludeHFiles:
					if (fileIsNew) {
						macroTmpl.InitializeCode("%NEW%");
						macroTmpl.WriteCode(macroStrm);
					} else {
						orgStrm.open(orgFile.Data(), ios::in);
						if (!orgStrm.good()) {	
							gMrbLog->Err() << gSystem->GetError() << " - " << orgFile << endl;
							gMrbLog->Flush(this->ClassName(), "SaveMacro");
						} else {
							for (;;) {
								line.ReadLine(orgStrm, kFALSE);
								if (orgStrm.eof()) break;
								if (line.Index("#include", 0) == 0) macroStrm << line << endl;
							}
							orgStrm.close();
						}
					}
					break;
				case TGMrbMacroEdit::kMacroTagCode:
					macroType = "void";
					for (i = 1; i <= fNofArgs; i++) {
						thisArg.SetNumber(i);
						argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
						argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
						argDefault = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Default"), "");
						if (i == 1) {
							argSpace.Resize(0);
							if (argDefault.Length() > 0) {
								if (argType.Index("Char", 0) == 0)	macroTmpl.InitializeCode("%ARG1DC%");
								else								macroTmpl.InitializeCode("%ARG1D%");
							} else									macroTmpl.InitializeCode("%ARG1%");
						} else {
							argSpace.Resize(0); argSpace.Fill(macroType.Length() + macroName.Length() + 2);
							if (argDefault.Length() > 0) {
								if (argType.Index("Char", 0) == 0)	macroTmpl.InitializeCode("%ARGNDC%");
								else								macroTmpl.InitializeCode("%ARGND%");
							} else									macroTmpl.InitializeCode("%ARGN%");
						}
						macroTmpl.Substitute("$macroType", macroType);
						macroTmpl.Substitute("$macroName", macroName);
						macroTmpl.Substitute("$argName", argName);
						if (argType.Index("Char", 0) == 0) argType.Prepend("const ");
						macroTmpl.Substitute("$argType", argType);
						argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
						if (argEntryType.CompareTo("YesNo") == 0) {
							if (argDefault.CompareTo("Yes") == 0)	macroTmpl.Substitute("$argDefault", "kTRUE");
							else									macroTmpl.Substitute("$argDefault", "kFALSE");
						} else										macroTmpl.Substitute("$argDefault", argDefault);
						addLofValues = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "AddLofValues"), "No");
						macroTmpl.Substitute("$argSpace", argSpace);
						argDel = (i == fNofArgs) ? ")" : ",";
						if (addLofValues.CompareTo("Yes") == 0) argDel = ",";
						macroTmpl.Substitute("$argDel", argDel);
						macroTmpl.WriteCode(macroStrm);
						if (addLofValues.CompareTo("Yes") == 0) {
							argSpace.Resize(0); argSpace.Fill(macroType.Length() + macroName.Length() + 2);
							macroTmpl.InitializeCode("%ARGNDC%");
							macroTmpl.Substitute("$argSpace", argSpace);
							macroTmpl.Substitute("$argType", "const Char_t *");
							macroTmpl.Substitute("$argName", argName + "Values");
							macroTmpl.Substitute("$argDefault",
									fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Values"), ""));
							argDel = (i == fNofArgs) ? ")" : ",";
							macroTmpl.Substitute("$argDel", argDel);
							macroTmpl.WriteCode(macroStrm);
						}
					}
					if (fileIsNew) {
						macroTmpl.InitializeCode("%NEW%");
						macroTmpl.WriteCode(macroStrm);
					} else {
						Bool_t found = kFALSE;
						orgStrm.open(orgFile.Data(), ios::in);
						if (!orgStrm.good()) {	
							gMrbLog->Err() << gSystem->GetError() << " - " << orgFile << endl;
							gMrbLog->Flush(this->ClassName(), "SaveMacro");
						} else {
							for (;;) {
								line.ReadLine(orgStrm, kFALSE);
								if (orgStrm.eof()) break;
								if (found) {
									macroStrm << line << endl;
								} else if (line.Index("//>>__", 0) == 0) {
									macroStrm << line << endl;
									found = kTRUE;
								}
							}
							orgStrm.close();
						}
						if (!found) {
							msgSpace.Resize(0); msgSpace.Fill(strlen(this->ClassName()) + strlen("::SaveMacro(): "));
							gMrbLog->Err()	<< "Can't locate USER CODE in file " << orgFile << endl
											<< msgSpace << "Copy code MANUALLY please ..." << endl;
							gMrbLog->Flush(this->ClassName(), "SaveMacro");
							macroTmpl.InitializeCode("%NEW%");
							macroTmpl.WriteCode(macroStrm);
						}
					}
					break;
			}
		}
	}	
	macroStrm.close();
	gMrbLog->Out() << "Definitions saved to file " << FileName << endl;
	gMrbLog->Flush(this->ClassName(), "SaveMacro", setblue);
	if (!fileIsNew) {
		msgSpace.Resize(0); msgSpace.Fill(strlen(this->ClassName()) + strlen("::SaveMacro(): "));
		gMrbLog->Out() << msgSpace << "(Original version saved to " << orgFile << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveMacro", setblue);
	}
	return(kTRUE);
}

const Char_t * TGMrbMacroArg::GetResource(TString & Resource, const Char_t * ResourceName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroArg::GetResource
// Purpose:        Build resource name
// Arguments:      TString & Resource      -- where to put results
//                 Char_t * ResourceName   -- resource name (trailing part)
// Results:        Char_t * Resource       -- same as arg #1
// Exceptions:
// Description:    Builds resource names "ArgN.ResourceName"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString arg;

	arg = "Arg";
	arg += fNumber;
	arg += ".";
	arg += ResourceName;
	Resource = arg;
	return(Resource.Data());
}
