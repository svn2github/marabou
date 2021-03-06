#ifndef __VMEServerPanel_h__
#define __VMEServerPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEServerPanel.h
// Purpose:        Class defs for VME modules
// Class:          VMEServerPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMEServerPanel.h,v 1.6 2010-03-10 12:08:11 Rudolf.Lutter Exp $
// Date:           $Date: 2010-03-10 12:08:11 $
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTextView.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

#include "TMrbC2Lynx.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEServerPanel
// Purpose:        Tab frame to connect to LynxOs server
// Constructors:
// Description:    A dialog window to connect to server
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMEServerPanel : public TGCompositeFrame {

	public:
		// colors
		enum EVMEServerColorId 	{
									kVMEServerColorBlack	=	0x000000,
									kVMEServerColorRed		=	0xFF0000,
									kVMEServerColorGreen	=	0x00FF00,
									kVMEServerColorYellow	=	0xFFFF00,
									kVMEServerColorBlue 	=	0x0000FF,
									kVMEServerColorMagenta	=	0xFF00FF,
								};

		// cmd ids to dispatch over X events in this panel
		enum EVMEServerCmdId 	{
									kVMEServerHost,
									kVMEServerTcpPort,
									kVMEServerServerPath,
									kVMEServerServerTypes,
									kVMEServerButtonConnect,
									kVMEServerButtonAbort,
									kVMEServerButtonRestart
								};

		enum					{	kVMEServerWaitForConnection	=	10	};

		enum EVMEServerLog		{	kVMEServerLogNone	=	TMrbC2Lynx::kC2LServerLogNone,
									kVMEServerLogCout	=	TMrbC2Lynx::kC2LServerLogCout,
									kVMEServerLogXterm	=	TMrbC2Lynx::kC2LServerLogXterm,
		};

	public:
		VMEServerPanel(TGCompositeFrame * TabFrame);
		virtual ~VMEServerPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		Bool_t Connect();							// connect to lynxos server
		Bool_t ActionButton(Int_t, Int_t);			// will be called on 'button pressed'

	protected:
		TList fHeap;								//! list of objects created on heap

		Bool_t fAbortConnection;					// kTRUE if connection to be aborted

		TGGroupFrame * fServerFrame; 				// server
		TGMrbLabelCombo * fSelectHost;  			//		host
		TGMrbLabelEntry * fSelectPort;				//		port
		TGMrbLabelEntry * fServerPathFileEntry;		//		path
		TGMrbLabelCombo * fSelectLogType;  			//		log type
		TGMrbTextButtonGroup * fServerButtonFrame;	// buttons
		TGGroupFrame * fLogFrame; 					// server log

		TGFileInfo fServerPathFileInfo;

		TMrbTail * fServerLog;						// stream to output server logs

		TMrbLofNamedX fServerActions;
		TMrbLofNamedX fServerColors;
		TMrbLofNamedX fServerLogTypes;

		TGMrbFocusList fFocusList;

	ClassDef(VMEServerPanel, 0) 	// [VMEControl] Connect ot LynxOs server
};

#endif
