#ifndef __DGFRestoreModuleSettingsPanel_h__
#define __DGFRestoreModuleSettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRestoreModuleSettingsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <iostream>
#include <iomanip>

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbCheckButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbPictureButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFocusList.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel
// Purpose:        Main frame to restore module settings
// Constructors:   
// Description:    A dialog window to restore module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFRestoreModuleSettingsPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFRestoreModuleSettingsActionsId 	{	kDGFRestoreModuleSettingsRestore,
														kDGFRestoreModuleSettingsClose,
														kDGFRestoreModuleSettingsSelectAll,
														kDGFRestoreModuleSettingsSelectNone,
														kDGFRestoreModuleSettingsSelectColumn
													};
				
		// geometry settings
		enum					{	kFrameWidth 			= 750					};
		enum					{	kFrameHeight 			= 380					};

		enum					{	kLEWidth				= kAutoWidth			};
		enum					{	kEntryWidth				= 100					};
		enum					{	kNumEntryWidth			= 80					};
		enum					{	kLEHeight				= 20					};
		enum					{	kButtonWidth			= 400					};

	public:
		DGFRestoreModuleSettingsPanel(const TGWindow * Parent, UInt_t Width, UInt_t Height,
                									UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~DGFRestoreModuleSettingsPanel() { fHeap.Delete(); };

		DGFRestoreModuleSettingsPanel(const DGFRestoreModuleSettingsPanel & f) : TGMainFrame(f) {};	// default copy ctor

		inline virtual void CloseWindow() { delete this; };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t LoadDatabase(Bool_t LoadPSA);		// restore dgf settings from file

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFRestoreModuleSettingsPanel, 0)		// [DGFControl] Restore settings
};

#endif
