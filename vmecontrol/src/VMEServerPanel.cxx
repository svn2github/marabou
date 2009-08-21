//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEServerPanel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEServerPanel.cxx,v 1.9 2009-08-21 10:02:32 Rudolf.Lutter Exp $
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
#include "TMrbC2Lynx.h"

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

static Char_t * kVMEServerFileTypes[]	=	{	"All files",			"*",
												NULL,					NULL
											};

extern TMrbC2Lynx * gMrbC2Lynx;
extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMEServerPanel)

VMEServerPanel::VMEServerPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
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

	this->ChangeBackground(gVMEControlData->fColorGreen);

	TGLayoutHints * loXpndX = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loNormal);

	frameGC->SetLH(loXpndX);
	groupGC->SetLH(loXpndX);
	labelGC->SetLH(loXpndX);
	entryGC->SetLH(loXpndX);
	comboGC->SetLH(loNormal);

	fServerFrame = new TGGroupFrame(this, "LynxOs Server", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fServerFrame);
	this->AddFrame(fServerFrame, groupGC->LH());

// VME host
	TMrbLofNamedX lofHosts;
	TString hostNames;
	Int_t selIdx = -1;
	TString lynxOs = "";
	TString vmeHost = gVMEControlData->Vctrlrc()->Get(".HostName", "ppc-0");
	for (Int_t i = 0; i < kVMENofPPCs; i++) {
		if (i > 0) hostNames += ":";
		TString ppc = Form("ppc-%d", i);
		if (ppc.CompareTo(vmeHost.Data()) == 0) {
			selIdx = i + 1;
			lynxOs = (i >= 10) ? "3.1" : "2.5";
		}
		hostNames += ppc;
	}
	lofHosts.AddNamedX(hostNames.Data());
	fSelectHost = new TGMrbLabelCombo(fServerFrame, "VME Host",	&lofHosts,
																kVMEServerHost, selIdx,
																frameWidth/4, kLEHeight, frameWidth/4,
																frameGC, labelGC, comboGC, labelGC);
	fServerFrame->AddFrame(fSelectHost, frameGC->LH());

// TCP port
	fSelectPort = new TGMrbLabelEntry(fServerFrame, "TCP Port",	50, kVMEServerTcpPort,
																	frameWidth/4, kLEHeight, frameWidth/4,
																	frameGC, labelGC, entryGC, labelGC);
	fServerFrame->AddFrame(fSelectPort, frameGC->LH());
	Int_t tcpPort = gVMEControlData->Vctrlrc()->Get(".TcpPort", 9010);
	fSelectPort->SetText(tcpPort);
	fSelectPort->SetType(TGMrbLabelEntry::kGMrbEntryTypeCharInt);
	fSelectPort->SetRange(9000, 9020);
	fSelectPort->AddToFocusList(&fFocusList);

// Server path
	fServerPathFileEntry = new TGMrbLabelEntry(fServerFrame, "Server Path (as seen from LynxOs)",
																50, kVMEServerServerPath,
																frameWidth/4, kLEHeight, frameWidth/4,
																frameGC, labelGC, entryGC);
	HEAP(fServerPathFileEntry);
	fServerFrame->AddFrame(fServerPathFileEntry, frameGC->LH());
	TString spath = "/nfs/marabou/bin/mrbLynxOsSrv";
	fServerPathFileEntry->SetText(gVMEControlData->Vctrlrc()->Get(".ServerName", spath.Data()));

//	buttons
	fServerButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fServerActions, 0, 1, groupGC, buttonGC);
	HEAP(fServerButtonFrame);
	this->AddFrame(fServerButtonFrame, buttonGC->LH());
	((TGMrbButtonFrame *) fServerButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButton(Int_t, Int_t)");

//	server log
	fLogFrame = new TGGroupFrame(this, "Server Log", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fLogFrame);
	this->AddFrame(fLogFrame, groupGC->LH());

	fTextView = new TGTextView(fLogFrame, frameWidth, 200);
	HEAP(fTextView);
	fLogFrame->AddFrame(fTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
	fTextView->SetFont(gClient->GetFont(gVMEControlData->FixedFont())->GetFontStruct());

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
	Int_t ppcNo = -1;
	if (ppc.Contains("ppc-")) {
		TString pn = ppc(4, 2);
		ppcNo = pn.Atoi();
	} else {
		gMrbLog->Err()	<< "Illegal host name - " << ppc << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}

	TString srv = fServerPathFileEntry->GetText();
	TString port = fSelectPort->GetText();
	Int_t portNo = port.Atoi();

	TString cpu, lynx;
	TMrbC2Lynx * c2l = new TMrbC2Lynx(ppc.Data(), srv.Data(), "c2lynx.log", portNo, "Pipe", kFALSE);
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
		pgb->DeleteWindow();
		delete pgb;
	}

	if (!c2l->IsConnected()) {
		gVMEControlData->MsgBox(this, "Connect", "Abort", Form("Unable to connect to LynxOs server @ %s:%d", ppc.Data(), portNo), kMBIconStop);
		return(kFALSE);
	}

	fPipe = c2l->GetPipe();
	if (fPipe) {
		fServerLog = new TMrbTail("mrb2lynx", fPipe);
		if (fServerLog->IsZombie()) {
			gMrbLog->Wrn()	<< "Connection to server log failed - no output" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		} else {
			fServerLog->SetOutput(this->ClassName(), this, "FillTextView()");
			fServerLog->Start();
		}
	} else {
		fTextView->AddLine("Connecting to running server - no server log possible");
		fTextView->AdjustWidth();
		gMrbLog->Wrn()	<< "Connecting to running server - no server log possible" << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
	}

	return(kTRUE);
}

void VMEServerPanel::FillTextView() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESystemPanel::FillTextView
// Purpose:        Fill server messages in text view window
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Called by signakl "NewDataArrived()"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t buf[512];
	if (fgets(buf, 512, fPipe))	{
		TString str = buf;
		str.Resize(str.Length() - 1);
		TIterator * iter = fServerColors.MakeIterator();
		TMrbNamedX * nx;
		Int_t color = 0;
		Int_t idx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			if (color == 0 && str.Contains(nx->GetName()))  color = nx->GetIndex();
		}
		idx = 0;
		while ((idx = str.Index("\033", idx)) >= 0) str.Remove(idx, 5);
		str = str.Strip(TString::kBoth);
		str.Prepend("   ");
		str += "   ";
		fTextView->SetForegroundColor(color);
		fTextView->AddLine(str.Data());
		fTextView->AdjustWidth();
		fTextView->ShowBottom();
	} else {
		cout << endl;
	}
}


