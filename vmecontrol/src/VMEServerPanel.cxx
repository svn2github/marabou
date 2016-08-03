//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEServerPanel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEServerPanel.cxx,v 1.15 2011-02-28 11:52:12 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFInstrumentPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TObjString.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"

#include "TGMrbProgressBar.h"

#include "VMEControlData.h"
#include "VMEServerPanel.h"

#include "SetColor.h"

const SMrbNamedX kVMEServerButtons[] =
			{
				{VMEServerPanel::kVMEServerButtonConnect,		"Connect to LynxOs", 	"Connect to LynxOs server"		},
				{VMEServerPanel::kVMEServerButtonAbort, 		"Abort connection",	"Abort connection in progress"	},
				{VMEServerPanel::kVMEServerButtonRestart,		"Restart server",	"Restart LynxOs server"			},
				{0, 											NULL,	NULL								}
			};

const SMrbNamedXShort kVMEServerColors[] =
			{
				{VMEServerPanel::kVMEServerColorRed,		(Char_t *) setred		},
				{VMEServerPanel::kVMEServerColorBlue,		(Char_t *) setblue  	},
				{VMEServerPanel::kVMEServerColorGreen,		(Char_t *) setgreen 	},
				{VMEServerPanel::kVMEServerColorMagenta,	(Char_t *) setmagenta	},
				{VMEServerPanel::kVMEServerColorYellow,		(Char_t *) setyellow	},
				{VMEServerPanel::kVMEServerColorBlack,		(Char_t *) setblack 	},
				{0, 										NULL		}
			};

const SMrbNamedX kVMEServerLogTypes[] =
			{
				{VMEServerPanel::kVMEServerLogNone, 	"none",		"No server output at all"	},
				{VMEServerPanel::kVMEServerLogCout, 	"cout",		"Server output to cout/cerr"	},
				{VMEServerPanel::kVMEServerLogXterm, 	"xterm",	"Server output to XTERM window"		},
				{0, 									NULL,			NULL						}
			};

static Char_t * kVMEServerFileTypes[]	=	{	"All files",			"*",
												NULL,					NULL
											};

extern TMrbC2Lynx * gMrbC2Lynx;
extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMEServerPanel)

VMEServerPanel::VMEServerPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEServerPanel
// Purpose:        Connect to LynxOs server
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorWhite);	HEAP(entryGC);

// geometry
	Int_t frameWidth = this->GetWidth();

//	Initialize several lists
	fServerActions.SetName("Server actions");
	fServerActions.AddNamedX(kVMEServerButtons);

	fServerColors.SetName("Server colors");
	fServerColors.AddNamedX(kVMEServerColors);

	fServerLogTypes.SetName("Server log types");
	fServerLogTypes.AddNamedX(kVMEServerLogTypes);

	this->ChangeBackground(gVMEControlData->fColorGreen);

	TGLayoutHints * loXpndX = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loNormal);

	frameGC->SetLH(loXpndX);
	groupGC->SetLH(loXpndX);
	labelGC->SetLH(loXpndX);
	entryGC->SetLH(loXpndX);
	comboGC->SetLH(loNormal);

	TGLayoutHints * lh = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
	HEAP(lh);

	TGVerticalFrame * vframe = new TGVerticalFrame(this);
	HEAP(vframe);
	this->AddFrame(vframe, lh);
	vframe->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * vdmy = new TGVerticalFrame(this);
	HEAP(vdmy);
	this->AddFrame(vdmy, lh);
	vdmy->ChangeBackground(gVMEControlData->fColorGreen);

	fServerFrame = new TGGroupFrame(vframe, "LynxOs Server", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fServerFrame);
	vframe->AddFrame(fServerFrame, groupGC->LH());

// VME host
	TMrbLofNamedX lofHosts;
	TString hostNames;
	gVMEControlData->Vctrlrc()->Get(hostNames, ".PPCNames", "");
	if (hostNames.IsNull()) gVMEControlData->Vctrlrc()->Get(hostNames, ".HostName", "");
	if (hostNames.IsNull()) gVMEControlData->Rootrc()->Get(hostNames, ".HostName", "<undef>");
	lofHosts.AddNamedX(hostNames.Data());
	fSelectHost = new TGMrbLabelCombo(fServerFrame, "VME Host",	&lofHosts,
																kVMEServerHost, 1,
																frameWidth/3, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fServerFrame->AddFrame(fSelectHost, frameGC->LH());

// TCP port
	fSelectPort = new TGMrbLabelEntry(fServerFrame, "TCP Port",	50, kVMEServerTcpPort,
																	frameWidth/3, kLEHeight, frameWidth/8,
																	frameGC, labelGC, entryGC, labelGC);
	fServerFrame->AddFrame(fSelectPort, frameGC->LH());
	Int_t tcpPort = gVMEControlData->Vctrlrc()->Get(".TcpPort", -1);
	if (tcpPort == -1) tcpPort = gVMEControlData->Rootrc()->Get(".TcpPort", 9010);
	fSelectPort->SetText(tcpPort);
	fSelectPort->SetType(TGMrbLabelEntry::kGMrbEntryTypeCharInt);
	fSelectPort->SetRange(9000, 9020);
	fSelectPort->AddToFocusList(&fFocusList);

// Server path
	fServerPathFileEntry = new TGMrbLabelEntry(fServerFrame, "Server Path",
																50, kVMEServerServerPath,
																frameWidth/4, kLEHeight, frameWidth/3,
																frameGC, labelGC, entryGC);
	HEAP(fServerPathFileEntry);
	fServerFrame->AddFrame(fServerPathFileEntry, frameGC->LH());
	TString spath;
	gVMEControlData->Vctrlrc()->Get(spath, ".ServerName", "");
	if (spath.IsNull()) gVMEControlData->Rootrc()->Get(spath, ".ServerName", "/nfs/marabou/bin/mrbLynxOsSrv");
	fServerPathFileEntry->SetText(spath.Data());

// Server log types
	fSelectLogType = new TGMrbLabelCombo(fServerFrame, "Server Output",		&fServerLogTypes,
																		kVMEServerServerTypes, 1,
																		frameWidth/4, kLEHeight, frameWidth/10,
																		frameGC, labelGC, comboGC, labelGC);
	HEAP(fSelectLogType);
	fServerFrame->AddFrame(fSelectLogType, frameGC->LH());
	fSelectLogType->Select(kVMEServerLogNone);

//	buttons
	fServerButtonFrame = new TGMrbTextButtonGroup(vframe, "Actions", &fServerActions, 0, 1, groupGC, buttonGC);
	HEAP(fServerButtonFrame);
	vframe->AddFrame(fServerButtonFrame, buttonGC->LH());
	((TGMrbButtonFrame *) fServerButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButton(Int_t, Int_t)");

	TabFrame->AddFrame(this, loXpndX);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t VMEServerPanel::ActionButton(Int_t FrameId, Int_t ButtonId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESystemPanel::ActionButton
// Purpose:        Action taken on 'button pressed'
// Arguments:      Int_t FrameId      -- framek id
//                 Int_t ButtonId     -- button id
// Results:        --
// Exceptions:
// Description:    Handles action buttons.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (ButtonId) {
		case kVMEServerButtonConnect:	return(this->Connect());
		case kVMEServerButtonAbort: 	fAbortConnection = kTRUE;
										return(kTRUE);
		case kVMEServerButtonRestart:	return(kTRUE);
		default:
			gMrbLog->Err()	<< "Illegal button id - " << ButtonId << endl;
			gMrbLog->Flush(this->ClassName(), "ActionButton");
			return(kFALSE);
	}
}

Bool_t VMEServerPanel::Connect() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESystemPanel::Connect
// Purpose:        Connect to lynxOs server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads GUI entries and connects to server.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gVMEControlData->IsOffline()) {
		gVMEControlData->MsgBox(this, "Connect", "OFFLINE", "Running in OFFLINE mode - no access to LynxOs");
		return(kTRUE);
	}

	if (gMrbC2Lynx) {
		gVMEControlData->MsgBox(this, "Connect", "Already connected ...",
						Form("Already connected to LynxOs server @ %s:%d", gMrbC2Lynx->GetHost(), gMrbC2Lynx->GetPort()));
		return(kTRUE);
	}

	TString ppc = fSelectHost->GetText();
	if (ppc.Contains("ppc-")) {
		TString pn = ppc(4, 2);
	} else {
		gMrbLog->Err()	<< "Illegal host name - " << ppc << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}

	TString srv = fServerPathFileEntry->GetText();
	TString port = fSelectPort->GetText();
	Int_t portNo = port.Atoi();

	TString cpu, lynx, slog;
	slog = ((TMrbNamedX *) fServerLogTypes.FindByIndex(fSelectLogType->GetSelected()))->GetName();
	TMrbC2Lynx * c2l = new TMrbC2Lynx(ppc.Data(), srv.Data(), "c2lynx.log", portNo, slog, kFALSE);
	if (c2l->IsZombie()) {
		gMrbLog->Err()	<< "Unable to start linux <-> lynx connection" << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}

	if (!c2l->Connect(kFALSE)) return(kFALSE);

	if (!c2l->WaitForConnection()) {
		TGMrbProgressBar * pgb = new TGMrbProgressBar(gClient->GetRoot(), this,
								Form("Connecting to LynxOs server @ %s:%d ...", ppc.Data(), portNo), 400, "blue", NULL, kTRUE);
		pgb->SetRange(0, kVMEServerWaitForConnection);
		fAbortConnection = kFALSE;
		for (Int_t wait = 0; wait < kVMEServerWaitForConnection; wait++) {
			pgb->Increment(1, Form("%d out of %d", wait + 1, kVMEServerWaitForConnection));
			gSystem->ProcessEvents();
			if (fAbortConnection) {
				gVMEControlData->MsgBox(this, "Connect", "Abort", "Pending connection to LynxOs server aborted");
				break;
			}
			if (c2l->WaitForConnection()) break;
			sleep(1);
		}
//		pgb->DeleteWindow();
		delete pgb;

	}

	if (c2l->IsConnected()) {
		if (c2l->GetServerLog()->GetIndex() == TMrbC2Lynx::kC2LServerLogNone)
					gVMEControlData->MsgBox(this, "Connect", "Info", Form("Connected to LynxOs server @ %s:%d", ppc.Data(), portNo), kMBIconAsterisk, kMBDismiss);
		return(kTRUE);
	} else {
		gVMEControlData->MsgBox(this, "Connect", "Abort", Form("Unable to connect to LynxOs server @ %s:%d", ppc.Data(), portNo), kMBIconStop);
		return(kFALSE);
	}

	return(kTRUE);
}


