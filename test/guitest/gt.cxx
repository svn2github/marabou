#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TGFrame.h"
#include "TRint.h"
#include "TEnv.h"

#include "TGLayout.h"
#include "TGButtonGroup.h"

#include "TNGMrbProfile.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFileEntry.h"
#include "TNGMrbLabelCombo.h"
#include "TNGMrbFileObject.h"
#include "TNGMrbTextButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbCheckButton.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"

extern TNGMrbLofProfiles * gMrbLofProfiles;

static TNGMrbProfile * stdProfile;
static TNGMrbProfile * yellowProfile;
static TNGMrbProfile * blueProfile;


void gt(const Char_t * Type, Int_t Width, Int_t Height);
void testIt(const Char_t * Type, TGMainFrame * Wdw);

const char * fileTypes[] = {	"All files",		"*",
                            	NULL,				NULL
							};

TGFileInfo info;

Int_t main(Int_t argc, Char_t * argv[]) {
	const Char_t * type;
	Int_t width, height;
	TRint theApp("DGFControl", &argc, argv);
	switch (argc) {
		case 1:
			type = "*";
			width = 800;
			height = 1100;
			break;
		case 2:
			type = argv[1];
			width = 800;
			height = 1100;
			break;
		case 3:
			type = argv[1];
			width = atoi(argv[2]);
			height = 1100;
			break;
		case 4:
		default:
			type = argv[1];
			width = atoi(argv[2]);
			height = atoi(argv[3]);
			break;
	}
	gt(type, width, height);
	theApp.Run();
}

void gt(const Char_t * Type, Int_t Width, Int_t Height) {
	TGMainFrame * w = new TGMainFrame(gClient->GetRoot(), Width, Height);

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
	stdProfile = gMrbLofProfiles->GetDefault();
	yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

	testIt(Type, w);

	w->SetWindowName("MARaBOU Widgets");
	w->MapSubwindows();
	w->Resize(w->GetDefaultSize());
	w->Resize(Width, Height);
	w->MapWindow();
}

void testIt(const Char_t * Type, TGMainFrame * w) {
	TGVerticalFrame * ef = new TGVerticalFrame(w);
	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "PE") == 0) {
		TNGMrbLabelEntry * e1 = new TNGMrbLabelEntry(ef, "Plain number entry", stdProfile,
													1, 500, 30, 200, TNGMrbLabelEntry::kGMrbEntryIsNumber);
		ef->AddFrame(e1, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}
	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NE") == 0) {
		TNGMrbLabelEntry * e2 = new TNGMrbLabelEntry(ef, "Numbers + up/down", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons);
		ef->AddFrame(e2, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NEUD") == 0) {
		TNGMrbLabelEntry * e3 = new TNGMrbLabelEntry(ef, "Numbers, up/down, begin/end", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons |
													TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
		e3->SetFormat(TGNumberEntry::kNESHex);
		e3->SetLimits(TGNumberEntry::kNELLimitMinMax, 0, 0x2000);
		ef->AddFrame(e3, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NEBE") == 0) {
		TMrbNamedX * action = new TMrbNamedX(0, "clr", "Action", w);
		TNGMrbLabelEntry * e4 = new TNGMrbLabelEntry(ef, "Same, but + action button", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons |
													TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
		e4->SetLimits(TGNumberEntry::kNELLimitMinMax, 200, 400);
		ef->AddFrame(e4, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NET") == 0) {
		TNGMrbLabelEntry * e5 = new TNGMrbLabelEntry(ef, "Time", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons |
													TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
		e5->SetFormat(TGNumberEntry::kNESHourMinSec);
		e5->SetLimits(TGNumberEntry::kNELLimitMinMax);
		e5->SetTimeLimit(TGNumberEntry::kNELLimitMin, 10, 0);
		e5->SetTimeLimit(TGNumberEntry::kNELLimitMax, 15, 30);
		ef->AddFrame(e5, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NED") == 0) {
		TNGMrbLabelEntry * e6 = new TNGMrbLabelEntry(ef, "Degree", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons |
													TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
		e6->SetFormat(TGNumberEntry::kNESDegree);
		e6->SetLimits(TGNumberEntry::kNELLimitMinMax);
		e6->SetDegreeLimit(TGNumberEntry::kNELLimitMin, 90, 30);
		e6->SetDegreeLimit(TGNumberEntry::kNELLimitMax, 360);
		ef->AddFrame(e6, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "LE") == 0 || strcmp(Type, "NEDT") == 0) {
		TNGMrbLabelEntry * e7 = new TNGMrbLabelEntry(ef, "Date", stdProfile,
													1, 500, 30, 200,
													TNGMrbLabelEntry::kGMrbEntryIsNumber |
													TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons |
													TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
		e7->SetFormat(TGNumberEntry::kNESDayMYear);
		e7->SetLimits(TGNumberEntry::kNELLimitMinMax);
		e7->SetDateLimit(TGNumberEntry::kNELLimitMin, 2004, 9, 1);
		e7->SetDateLimit(TGNumberEntry::kNELLimitMax, 2004, 12, 31);
		ef->AddFrame(e7, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "FB") == 0) {
 		info.fFileTypes = (const Char_t **) fileTypes;
		TNGMrbFileEntry * e8 = new TNGMrbFileEntry(ef, "FileBrowser",
												&info, kFDOpen, stdProfile,
												1, 500, 30, 300, 0);
		ef->AddFrame(e8, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "C") == 0) {
		TMrbNamedX c1(100, "red", "color red");
		TMrbNamedX c2(2, "green", "color green");
		TMrbNamedX c3(30, "blue", "color blue");
		TMrbLofNamedX * l = new TMrbLofNamedX();
		l->Sort(kTRUE);
		l->AddNamedX(&c1);
		l->AddNamedX(&c2);
		l->AddNamedX(&c3);
		l->Print();
		TNGMrbLabelCombo * e9 = new TNGMrbLabelCombo(ef, "Combo", l, stdProfile,
													1, 1, 500, 30, 200,
													TNGMrbLabelCombo::kGMrbComboHasUpDownButtons |
													TNGMrbLabelCombo::kGMrbComboHasBeginEndButtons);
		ef->AddFrame(e9, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "FOC") == 0) {
		TNGMrbFileObjectCombo * e10 = new TNGMrbFileObjectCombo(ef, "FileBrowser + ObjectCombo", stdProfile,
													1, 1, 500, 60, 300, 200,
													TNGMrbFileObjectCombo::kGMrbComboHasUpDownButtons |
													TNGMrbFileObjectCombo::kGMrbComboHasBeginEndButtons);
		ef->AddFrame(e10, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	w->AddFrame(ef);

	TGHorizontalFrame * tf = new TGHorizontalFrame(w);

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "TB") == 0 || strcmp(Type, "TBH") == 0) {
		TNGMrbTextButtonGroup * e11h = new TNGMrbTextButtonGroup(tf, "text buttons (horizontal layout)", "new:open|open file:close:save:exit",
												stdProfile, 1, 0);
		tf->AddFrame(e11h, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "TB") == 0 || strcmp(Type, "TBV") == 0) {
		TNGMrbTextButtonGroup * e11v = new TNGMrbTextButtonGroup(tf, "text buttons (vertical layout)", "new:open:close:save:exit",
												stdProfile, 0, 1);
		tf->AddFrame(e11v, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "TB") == 0 || strcmp(Type, "TBM") == 0) {
		TNGMrbTextButtonGroup * e11m = new TNGMrbTextButtonGroup(tf, "text buttons (matrix layout)", "new:open:close:save:exit",
												stdProfile, 3, 2);
		tf->AddFrame(e11m, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	w->AddFrame(tf);

	TGHorizontalFrame * rf = new TGHorizontalFrame(w);

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "RB") == 0 || strcmp(Type, "RBH") == 0) {
		TNGMrbRadioButtonGroup * e12h = new TNGMrbRadioButtonGroup(rf, "radio buttons (horizontal layout)", "Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 1, 0);
		rf->AddFrame(e12h, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "RB") == 0 || strcmp(Type, "RBV") == 0) {
		TNGMrbRadioButtonGroup * e12v = new TNGMrbRadioButtonGroup(rf, "radio buttons (vertical layout)", "Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 0, 1);
		e12v->SetPadLeft(10);
		rf->AddFrame(e12v, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "RB") == 0 || strcmp(Type, "RBM") == 0) {
		TNGMrbRadioButtonGroup * e12m = new TNGMrbRadioButtonGroup(rf, "radio buttons (matrix layout)", "Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 2, 4);
		rf->AddFrame(e12m, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	w->AddFrame(rf);

	TObjArray * lsb = new TObjArray();
	lsb->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x0fffffff, "cbutton_all.xpm"));
	lsb->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	lsb->Add(new TNGMrbSpecialButton(0x20000000, "even", "Select EVEN", 0x05555555, "cbutton_even.xpm"));
	lsb->Add(new TNGMrbSpecialButton(0x10000000, "odd", "Select ODD", 0x0aaaaaaa, "cbutton_odd.xpm"));

	TGVerticalFrame * cfv = new TGVerticalFrame(w);

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "CB") == 0 || strcmp(Type, "CBH") == 0) {
		TNGMrbCheckButtonGroup * e13h = new TNGMrbCheckButtonGroup(cfv, "check buttons (horizontal layout)",
												"Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 1, 0);
		e13h->SetIndivColumnWidth();
		cfv->AddFrame(e13h, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "CB") == 0 || strcmp(Type, "CBS") == 0) {
		TNGMrbCheckButtonGroup * e13s = new TNGMrbCheckButtonGroup(cfv, "check buttons (+ special buttons)",
												"Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 1, 0, 0, lsb);
		e13s->SetIndivColumnWidth();
		cfv->AddFrame(e13s, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	w->AddFrame(cfv);

	TGHorizontalFrame * cfh = new TGHorizontalFrame(w);

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "CB") == 0 || strcmp(Type, "CBV") == 0) {
		TNGMrbCheckButtonGroup * e13v = new TNGMrbCheckButtonGroup(cfh, "check buttons (vertical layout)",
												"Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 0, 1);
		e13v->SetIndivColumnWidth();
		cfh->AddFrame(e13v, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "CB") == 0 || strcmp(Type, "CBM") == 0) {
		TNGMrbCheckButtonGroup * e13m = new TNGMrbCheckButtonGroup(cfh, "check buttons (matrix)",
												"Mon:Tue:Wed:Thu:Fri:Sat:Sun",
												stdProfile, 3, 3);
		e13m->SetIndivColumnWidth();
		cfh->AddFrame(e13m, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	if (*Type == '*' || strcmp(Type, "B") == 0 || strcmp(Type, "RB") == 0  || strcmp(Type, "YN") == 0) {
		TNGMrbYesNoButtonGroup * e14 = new TNGMrbYesNoButtonGroup(cfh, "yes/no", stdProfile);
		cfh->AddFrame(e14, new TGLayoutHints(kLHintsNormal, 10, 0, 10, 0));
	}

	w->AddFrame(cfh);
}
