#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"
#include "TKey.h"
#include "TStyle.h"
#include "TSystem.h"
#include "GeneralAttDialog.h"
#include "SetColor.h"
#include <iostream>

namespace std {} using namespace std;

Int_t GeneralAttDialog::fPrependFilenameToName = 1;
Int_t GeneralAttDialog::fPrependFilenameToTitle = 0;
Int_t GeneralAttDialog::fForceStyle = 1;
Int_t GeneralAttDialog::fShowPSFile = 0;
Int_t GeneralAttDialog::fSuppressWarnings = 1;
Int_t GeneralAttDialog::fUseRegexp = 0;
Int_t GeneralAttDialog::fShowListsOnly = 0;
Int_t GeneralAttDialog::fRememberLastSet = 1;
Int_t GeneralAttDialog::fRememberZoom = 1;
Int_t GeneralAttDialog::fRememberStatBox = 0;
Int_t GeneralAttDialog::fRememberLegendBox = 1;
Int_t GeneralAttDialog::fUseAttributeMacro = 0;
Int_t GeneralAttDialog::fMaxListEntries = 333;
Int_t GeneralAttDialog::fContentLowLimit = 0;
Int_t GeneralAttDialog::fVertAdjustLimit =0;
Int_t GeneralAttDialog::fAdjustMinY      =0;
Int_t GeneralAttDialog::fStackedReally = 1;
Int_t GeneralAttDialog::fScaleStack = 1;
Int_t GeneralAttDialog::fStackedNostack = 0;
Int_t GeneralAttDialog::fStackedPads = 0;
TString GeneralAttDialog::fGlobalStyle = "Plain";

//_______________________________________________________________________

GeneralAttDialog::GeneralAttDialog(TGWindow * win)
{
static const char helptext[] =
"\n\
___________________________________________________________\n\
Prepend filename to histo titles\n\
--------------------------------\n\
This is useful when superimposing, stacking or showing histograms\n\
in one canvas with same titles from files e.g. from different runs\n\
since the title is used in legends.\n\
___________________________________________________________\n\
Force style, show hist with current style\n\
-----------------------------------------\n\
Use graphics option currently in use instead of options\n\
stored with the histogram on file\n\
___________________________________________________________\n\
Global style, activate roots style options\n\
------------------------------------------\n\
___________________________________________________________\n\
Max Entries in Lists\n\
--------------------\n\
Too many entries in histlists (e.g. > 1000) make build up\n\
of lists slow and may even crash X on some older systems\n\
With this option one can limit this number\n\
___________________________________________________________\n\
Show lists only\n\
-----------------------------\n\
If very many histograms are used this option allows\n\
to display user defined lists of histograms only.\n\
\n\
Show PS file after creation\n\
---------------------------\n\
Automatically invoke ghostview when a PostScript file is\n\
generated.\n\
____________________________________________________________\n\
Remember Expand settings (Marks) \n\
-----------------------------------\n\
Using marks expanded parts of hists may be shown. This\n\
option allows to remember these settings in later sessions.\n\
____________________________________________________________\n\
Remember Zoomings (by left mouse)\n\
---------------------------------\n\
Pressing the left mouse button in the scale of a histogram\n\
and dragging to the required limit allows to zoom\n\
in the picture. This  option allows to remember these\n\
settings in later sessions.\n\
____________________________________________________________\n\
Remember position of StatBox, LegendBox\n\
--------------------------------------------\n\
Position / size of StatBox, LegendBox can be adjusted from the\n\
menu \"Graphic_defaults\". Using there the command \n\
\"Save as global default\" make these settings persistent\n\
across sessions.\n\
The size will be adjusted according to the number of lines\n\
in the box, e.g. if only name and number of entries of a\n\
histogram should be displayed the box is smaller than\n\
if in addition mean and sigma are to be displayed.\n\
Movements of the boxes by the mouse are not remembered.\n\
\n\
If this option is activated position of the boxes\n\
are remembered when moved with the mouse\n\
\n\
Use Attribute Macro \n\
-------------------\n\
With this option activ each time after a histogram is\n\
displayed a macro (FH_setdefaults.C) is executed.\n\
____________________________________________________________\n\
Use Regular expression syntax\n\
------------------------------\n\
Normally wild card syntax (e.g. ls *.root to list all\n\
files ending with .root) is used in file/histo selection\n\
masks. One may switch to the more powerful Regular expression\n\
For details consult a book on Unix.\n\
____________________________________________________________\n\
";
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
//   static Int_t dummy;
	GetCustomStyles();
	TObject *obj;
   TString style_menu ("ComboSelect_         Set global style");
   TIter next(gROOT->GetListOfStyles());
	while ( (obj = next()) ) {
	   style_menu += ";";
	   style_menu += ((TStyle*)obj)->GetName();
	}
   RestoreDefaults();
   fRow_lab->Add(new TObjString("CheckButton_              Force current style"));
   fValp[ind++] = &fForceStyle;
   fRow_lab->Add(new TObjString(style_menu));
   fGlobalStyleButton = ind;
   fValp[ind++] = &fGlobalStyle;
   fRow_lab->Add(new TObjString("CheckButton_  Prepend filename to histo names"));
   fValp[ind++] = &fPrependFilenameToName;
   fRow_lab->Add(new TObjString("CheckButton_ Prepend filename to histo titles"));
   fValp[ind++] = &fPrependFilenameToTitle;
	fRow_lab->Add(new TObjString("CheckButton_              Show histlists only"));
	fValp[ind++] = &fShowListsOnly;
	fRow_lab->Add(new TObjString("CheckButton_        Suppress warning messages"));
   fValp[ind++] = &fSuppressWarnings;
   fRow_lab->Add(new TObjString("CheckButton_      Show PS file after creation"));
   fValp[ind++] = &fShowPSFile;
   fRow_lab->Add(new TObjString("CheckButton_ Remember Expand settings (Marks)"));
   fValp[ind++] = &fRememberLastSet;
   fRow_lab->Add(new TObjString("CheckButton_Remember Zoomings (by left mouse)"));
   fValp[ind++] = &fRememberZoom;
   fRow_lab->Add(new TObjString("CheckButton_     Remember position of StatBox"));
   fValp[ind++] = &fRememberStatBox;
	fRow_lab->Add(new TObjString("CheckButton_   Remember position of LegendBox"));
	fValp[ind++] = &fRememberLegendBox;
	fRow_lab->Add(new TObjString("CheckButton_              Use Attribute Macro"));
   fValp[ind++] = &fUseAttributeMacro;
   fRow_lab->Add(new TObjString("CheckButton_    Use Regular expression syntax"));
   fValp[ind++] = &fUseRegexp;
   fRow_lab->Add(new TObjString("PlainIntVal_            Max Entriess in Lists"));
   fValp[ind++] = &fMaxListEntries;
	fRow_lab->Add(new TObjString("CheckButton_ Adjust min Y to min cont, (1dim)"));
   fAdjustMinYButton = ind;
	fValp[ind++] = &fAdjustMinY;
	fRow_lab->Add(new TObjString("PlainIntVal_            Fit2Dim Content Limit"));
	fValp[ind++] = &fContentLowLimit;
	fRow_lab->Add(new TObjString("PlainIntVal_    Fit2Dim Vertical Adjust Limit"));
	fValp[ind++] = &fVertAdjustLimit;

   fRow_lab->Add(new TObjString("CommentOnly_Option for stacked hists"));
   fValp[ind++] = &fScaleStack;
   fRow_lab->Add(new TObjString("CheckButton_     Apply scale to stacked hists"));
	fValp[ind++] = &fScaleStack;
	fRow_lab->Add(new TObjString("RadioButton_ Really stack"));
	fValp[ind++] = &fStackedReally;
   fRow_lab->Add(new TObjString("RadioButton+  Superimpose"));
   fValp[ind++] = &fStackedNostack;
   fRow_lab->Add(new TObjString("RadioButton+Separate Pads"));
   fValp[ind++] = &fStackedPads;

   static Int_t ok;
   Int_t itemwidth = 360;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________
void GeneralAttDialog::GetCustomStyles()
{
	if (!gSystem->AccessPathName("hpr_custom_styles.root", kFileExists)) {
	   TFile *cstyle = new TFile("hpr_custom_styles.root");
		TIter n1(gDirectory->GetListOfKeys());
		TKey* key;
		while( (key = (TKey*)n1()) ){
		   if(!strncmp(key->GetClassName(),"TStyle",6)) {
		      TStyle *sty = (TStyle*)cstyle->Get(key->GetName());
				cout << "key->GetName() " << key->GetName()<< endl;
				if (!gROOT->GetListOfStyles()->FindObject(key->GetName()))
			      gROOT->GetListOfStyles()->Add(sty);
		   }
		}
		cstyle->Close();
	}
}
//_______________________________________________________________________

void GeneralAttDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "GeneralAttDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void GeneralAttDialog::CloseDialog()
{
//   cout << "GeneralAttDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void GeneralAttDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("GeneralAttDialog.fGlobalStyle",       fGlobalStyle);
   env.SetValue("GeneralAttDialog.fForceStyle",        fForceStyle);
   env.SetValue("GeneralAttDialog.fShowPSFile",        fShowPSFile);
   env.SetValue("GeneralAttDialog.fSuppressWarnings",  fSuppressWarnings);
   env.SetValue("GeneralAttDialog.fUseRegexp",         fUseRegexp);
   env.SetValue("GeneralAttDialog.fShowListsOnly",     fShowListsOnly);
   env.SetValue("GeneralAttDialog.fRememberLastSet",   fRememberLastSet);
   env.SetValue("GeneralAttDialog.fRememberZoom",      fRememberZoom);
	env.SetValue("GeneralAttDialog.fRememberStatBox",   fRememberStatBox);
	env.SetValue("GeneralAttDialog.fRememberLegendBox", fRememberLegendBox);
	env.SetValue("GeneralAttDialog.fUseAttributeMacro", fUseAttributeMacro);
   env.SetValue("GeneralAttDialog.fMaxListEntries",    fMaxListEntries);
   env.SetValue("GeneralAttDialog.fVertAdjustLimit",   fVertAdjustLimit);
   env.SetValue("GeneralAttDialog.fAdjustMinY",        fAdjustMinY);
   env.SetValue("GeneralAttDialog.fContentLowLimit",   fContentLowLimit);
   env.SetValue("GeneralAttDialog.fStackedReally" ,    fStackedReally );
	env.SetValue("GeneralAttDialog.fScaleStack" ,       fScaleStack );
	env.SetValue("GeneralAttDialog.fStackedNostack",    fStackedNostack);
   env.SetValue("GeneralAttDialog.fStackedPads"   ,    fStackedPads   );
	env.SetValue("GeneralAttDialog.fPrependFilenameToTitle", fPrependFilenameToTitle);
	env.SetValue("GeneralAttDialog.fPrependFilenameToName",  fPrependFilenameToName);
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void GeneralAttDialog::RestoreDefaults()
{
	GetCustomStyles();
   TEnv env(".hprrc");
   fGlobalStyle = env.GetValue("GeneralAttDialog.fGlobalStyle", "Plain");
   fForceStyle = env.GetValue("GeneralAttDialog.fForceStyle", 1);
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
   fShowPSFile = env.GetValue("GeneralAttDialog.fShowPSFile", 1);
   fSuppressWarnings = env.GetValue("GeneralAttDialog.fSuppressWarnings", 1);
   if (fSuppressWarnings) gErrorIgnoreLevel=kError;
   else                   gErrorIgnoreLevel=kWarning;
   fUseRegexp = env.GetValue("GeneralAttDialog.fUseRegexp", 0);
   fShowListsOnly = env.GetValue("GeneralAttDialog.fShowListsOnly", 0);
   fRememberLastSet = env.GetValue("GeneralAttDialog.fRememberLastSet", 1);
   fRememberZoom = env.GetValue("GeneralAttDialog.fRememberZoom", 1);
	fRememberStatBox = env.GetValue("GeneralAttDialog.fRememberStatBox", (Int_t)0);
	fRememberLegendBox = env.GetValue("GeneralAttDialog.fRememberLegendBox", 1);
	fUseAttributeMacro = env.GetValue("GeneralAttDialog.fUseAttributeMacro", 0);
   fMaxListEntries =
   env.GetValue("GeneralAttDialog.fMaxListEntries", 333);
   fVertAdjustLimit = env.GetValue("GeneralAttDialog.fVertAdjustLimit", 0);
   fAdjustMinY      = env.GetValue("GeneralAttDialog.fAdjustMinY",      1);
   fContentLowLimit = env.GetValue("GeneralAttDialog.fContentLowLimit", 0);
   fScaleStack      = env.GetValue("GeneralAttDialog.fScaleStack" ,     0);
	fStackedReally   = env.GetValue("GeneralAttDialog.fStackedReally" , 1);
	fStackedNostack  = env.GetValue("GeneralAttDialog.fStackedNostack", 0);
   fStackedPads     = env.GetValue("GeneralAttDialog.fStackedPads"   , 0);
	fPrependFilenameToTitle = env.GetValue("GeneralAttDialog.fPrependFilenameToTitle"   , 0);
	fPrependFilenameToName = env.GetValue("GeneralAttDialog.fPrependFilenameToName", 1);
	
	if (fAdjustMinY == 0 ) {
		cout << setred
		<< "Warning: Y scale of 1dim hists is forced to 0 (or smaller)"<< endl
		<< "         Be careful when setting log Y scale,"<< endl
		<< "         dont use popup menu with right mouse in canvas"<< endl
		<< "         to set log Y" << setblack << endl;
	}
	cout << "Set style to: " << fGlobalStyle << endl;
	gROOT->SetStyle(fGlobalStyle);
	gStyle->SetPalette(1,NULL);
	// make sure text precission is 2 (not 3)
	Int_t tp = gStyle->GetTextFont();
	if ( tp % 10 != 2 ) {
		cout << "Resetting text font from " << tp;
		tp = tp % 10 + 2;
		gStyle->SetTextFont(tp);
		cout << " to  " << tp << endl;
	}
}
//______________________________________________________________________

void GeneralAttDialog::CloseDown(Int_t wid)
{
   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void GeneralAttDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   if (bid == fGlobalStyleButton) {
	   cout << "Set style to: " << fGlobalStyle << endl;
		gROOT->SetStyle(fGlobalStyle);
		gStyle->SetPalette(1,NULL);
		Int_t tp = gStyle->GetTextFont();
		if ( tp % 10 != 2 ) {
			cout << "Resetting text font from " << tp;
			tp = tp % 10 + 2;
			gStyle->SetTextFont(tp);
			cout << " to  " << tp << endl;
		}
   }
   if (bid == fAdjustMinYButton && fAdjustMinY == 0 ) {
		cout << setred
		<< "Warning: Y scale of 1dim hists is forced to 0 (or smaller)"<< endl
		<< "         Be careful when setting log Y scale,"<< endl
		<< "         dont use popup menu with right mouse in canvas"<< endl
		<< "         to set log Y" << setblack << endl;
	}
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
   if (fSuppressWarnings) gErrorIgnoreLevel=kError;
   else                   gErrorIgnoreLevel=kWarning;
}

