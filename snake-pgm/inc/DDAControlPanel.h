#ifndef __DDAControl_h__
#define __DDAControl_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControl.h
// Purpose:        Class defs for the DDA0816 GUI
// Description:    Defines a main frame for the DDA0816 GUI
// Author:         R. Lutter
// Revision:       $Id: DDAControlPanel.h,v 1.1 2009-03-26 11:13:41 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TApplication.h"
#include "TRint.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGMenu.h"

#include "TMrbLofNamedX.h"

#include "TGMrbRadioButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbPictureButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLayout.h"
#include "TGMrbLofKeyBindings.h"

#include "TSnkDDA0816.h"

//______________________________________________________________[BITS & BYTES]
//////////////////////////////////////////////////////////////////////////////
// Name:           Miscellaneous
// Purpose:        Bits and bytes
// Definitions:    
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

// id words to dispatch over X events in main frame
enum EDDAControlPanelCmdId {	kDDAFileNew,
								kDDAFileOpen,
								kDDAFileClose,
								kDDAFileSave,
								kDDAFileSaveAs,
								kDDAFileReset,
								kDDAFilePrint,
								kDDAFileExit,

								kDDASettingsNormal,
								kDDASettingsVerbose,
								kDDASettingsDebug,
								kDDASettingsOnline,
								kDDASettingsOffline,

								kDDAServerOldDDA,
								kDDAServerNewDDA,

								kDDAHelpContents,
								kDDAHelpAbout,

								kDDASubdevA,
								kDDASubdevB,
								kDDASubdevC,
								kDDASubdevD,

								kDDAEntryChargeState,
								kDDAEntryEnergy,
								kDDAEntryCalibX,
								kDDAEntryCalibY,
								kDDAEntryVoltageX,
								kDDAEntryVoltageY,
								kDDAEntryStepWidthX,
								kDDAEntryStepWidthY,
								kDDAEntryRangeX,
								kDDAEntryRangeY,

								kDDAEntryOffset0,
								kDDAEntryOffset1,
								kDDAOffsetZero0,
								kDDAOffsetZero1,
								kDDAEntryAmplitude0,
								kDDAEntryAmplitude1,
								kDDAEntrySize0,
								kDDAEntrySize1,
								kDDAEntryStopPos0,
								kDDAEntryStopPos1,
								kDDAStopZero0,
								kDDAStopZero1,
								kDDAEntryPacer,
								kDDAEntryScale0,
								kDDAEntryScale1,
								kDDAEntryScanPeriod,
								kDDAEntryCycles,
								kDDAEntryDataFile,

								kDDAControlCheck,
								kDDAControlConnect,
								kDDAControlStartScan,
								kDDAControlStopScan,
								kDDAControlKillServer,

								kDDAPictExit,
								kDDAPictOpen,
								kDDAPictSave
						};

enum EDDASoftScaleId	{		kDDASoftScaleOff	= BIT(0),
								kDDASoftScaleX		= BIT(1),
								kDDASoftScaleY		= BIT(2)
						};
							

// key bindings
enum EDDAKeyAction		{	kDDAKeyActionReset	= TGMrbLofKeyBindings::kGMrbKeyActionUser
						};

// in case of kLHintsExpandX/Y
enum	{	kAutoWidth			= 1 		};
enum	{	kAutoHeight			= 1 		};

// geometry settings
enum	{	kMainFrameWidth 	= 1100	};
enum	{	kMainFrameHeight	= 920   };

enum	{	kHFrameHeight		= (kMainFrameHeight/3)	};
enum	{	kXYEntryWidth		= 100						};
enum	{	kXYEntryHeight		= 20						};
enum	{	kSDEntryWidth		= 100						};
enum	{	kSDEntryHeight		= 20						};
enum	{	kSDButtonWidth		= 400						};
enum	{	kSPEntryWidth		= 100						};
enum	{	kSPEntryHeight		= 20						};
enum	{	kDFEntryWidth		= 100						};
enum	{	kDFEntryHeight		= 20						};
enum	{	kCButtonWidth		= 600						};
enum	{	kCButtonHeight		= 20						};

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel
// Purpose:        Main frame for the MARaBOU config GUI
// Constructors:   
// Methods:        CloseWindow      -- close main frame
//                 ProcessMessage   -- process widget events
// Description:    Defines a main frame for the GUI
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


class DDAControlPanel : public TGMainFrame {

	public:
		DDAControlPanel(const TGWindow *, UInt_t, UInt_t);
		~DDAControlPanel() { fHeap.Delete(); };

		Bool_t OpenScanFile(const Char_t * File = NULL);
		void CloseWindow();

		Bool_t Reset(); 											// reset to defaults
		Bool_t InitializeValues(TSnkDDA0816 * DDA, ESnkScanMode Mode); 	// initialize values according to scan mode
		Bool_t UpdateValues(TSnkDDA0816 * DDA, ESnkScanMode Mode); 	// update values after check
		Bool_t UpdateCalibration(TSnkDDA0816 * DDA); 				// update calibration data
		Bool_t UpdateScanLimits(TSnkDDA0816 * DDA);					// update range
		Bool_t ResetOffset(TSnkDDA0816 * DDA, Int_t Channel);		// rest offset
		Bool_t ResetStopPos(TSnkDDA0816 * DDA, Int_t Channel);		// rest stop position
		Bool_t DDA2Gui(TSnkDDA0816 *);								// set GUI values from DDA0816 object
		Bool_t Gui2DDA(TSnkDDA0816 *, Bool_t SaveFlag = kFALSE);	// set DDA values from GUI

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
		void EntryChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ButtonPressed(Int_t FrameId, Int_t Selection);
		void SetScanMode(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t Action);
		void Activate(Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);


	protected:
		Bool_t OpenDDAFile(const Char_t * File);
		void SetEntryEnabled(TGMrbLabelEntry * Entry, const Char_t * Value = NULL);
		void SetEntryDisabled(TGMrbLabelEntry * Entry, const Char_t * Value = NULL);
		void SetEntrySlave(TGMrbLabelEntry * Entry, const Char_t * Value = NULL);
		void SetFileEnabled(TGMrbFileEntry * Entry, const Char_t * Value = NULL);
		void SetFileDisabled(TGMrbFileEntry * Entry, const Char_t * Value = NULL);

	protected:
		TList fHeap;							// list of objects created on heap

		TGMenuBar * fMenuBar;					// main menu bar
		TGPopupMenu * fMenuFile;				// pulldown menu: file
		TGPopupMenu * fMenuSettings;			// pulldown menu: settings
		TGPopupMenu * fMenuServer;				// pulldown menu: server
		TGPopupMenu * fMenuHelp;				// pulldown menu: help

		TGHorizontalFrame * fCFrame;			// control frame
		TGMrbPictureButtonGroup * fFButtons;	// file buttons: open / save
		TGMrbTextButtonGroup * fCbuttons;		// control buttons
		TGGroupFrame * fSFrame; 				// scan period /cycles
		TGMrbLabelEntry * fScanPeriod;			// ...
		TGMrbLabelEntry * fCycles;				// ...
		TGGroupFrame * fFFrame; 				// file name
		TGMrbFileEntry * fDataFile; 			// ...
		TGMrbRadioButtonGroup * fSubdevs;		// radiobuttons: modules
		TGMrbRadioButtonGroup * fScanModes; 	// radiobuttons: scan modes
		TGVerticalFrame * fHFrame;				// frame to store channel settings for X and Y
		TGGroupFrame * fCalFrame; 				// calibration
		TGHorizontalFrame * fCRow1Frame;		// entries in row 1
		TGHorizontalFrame * fCRow2Frame;		// entries in row 2
		TGMrbLabelEntry * fChargeState;			// entry: charge number
		TGMrbLabelEntry * fEnergy;				// entry: energy
		TGMrbLabelEntry * fCalib[2];			// entry: calibration
		TGMrbLabelEntry * fVoltage[2];			// entry: max voltage
		TGMrbLabelEntry * fStepWidth[2];		// entry: step width
		TGMrbLabelEntry * fRange[2];			// entry: range
		TGGroupFrame * fXYFrame[2]; 			// X and Y frames
		TGMrbRadioButtonGroup * fScanProfiles[2];	// radiobuttons: scan profiles
		TGGroupFrame * fDistFrame[2];			// distance
		TGGroupFrame * fStepFrame[2];			// steps
		TGGroupFrame * fStopPosFrame[2];		// stop position
		TMrbNamedX * fOffsetZeroButton[2];		// zero buttons
		TGMrbLabelEntry * fOffsetD[2];			// offset distance
		TGMrbLabelEntry * fOffsetS[2];			// ... steps
		TGMrbLabelEntry * fAmplitudeD[2];		// amplitude distance
		TGMrbLabelEntry * fAmplitudeS[2];		// ... steps
		TMrbNamedX * fStopZeroButton[2];		// zero buttons
		TGMrbLabelEntry * fStopPosS[2]; 		// stop position distance
		TGMrbLabelEntry * fStopPosD[2];			// ... steps
		TGMrbLabelEntry * fNofPixels[2]; 		// entry: scan size
		TGMrbLabelEntry * fScanLimitsD[2]; 		// entry: actual scan limits (distance)
		TGMrbLabelEntry * fScanLimitsS[2]; 		// ... (steps)
		TGGroupFrame * fFQFrame;			 	// frame to store frequencies & scaledowns
		TGMrbRadioButtonList * fPreScales; 		// radiobuttons: prescaler
		TGHorizontalFrame * fSDFrame;			// frame to store scaledowns
		TGMrbLabelEntry * fPacerClock;			// entry: pacer clock
		TGMrbRadioButtonList * fSoftScale;		// radiobuttons: soft scale
		TGMrbLabelEntry * fScaleValue;			// entry: scale value

		TGMrbLabelEntry * fAmplYScale;			// scaled Y amplitude
		
		TMrbLofNamedX fLofScanProfiles[2];		// list of scan profiles

		TMrbLofNamedX fLofSubdevs; 				// modules
		TMrbLofNamedX fLofPreScales; 			// prescaler values
		TMrbLofNamedX fLofScanModes;	 		// scan modes
		TMrbLofNamedX fLofControlButtons;		// list of control buttons
		TMrbLofNamedX fLofSoftScaleButtons;		// list of softscale buttons
		TMrbLofNamedX fLofFileButtons;			// list of file buttons

		TGMrbLayout * fFrameGC; 				// layout descriptors
		TGMrbLayout * fLabelGC;
		TGMrbLayout * fWhiteEntryGC;
		TGMrbLayout * fYellowEntryGC;
		TGMrbLayout * fGrayButtonGC;
		TGMrbLayout * fBlueButtonGC;

		TGMrbFocusList fFocusList;				// focus handling

		TGFileInfo fDataFileInfo;				// data file

		TString fServerName;					// server name
		Int_t fServerPort;						// ... port
		TString fServerProg;					// ... program

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings

	ClassDef(DDAControlPanel, 1) 				// [SNAKE] Control panel
};

#endif
