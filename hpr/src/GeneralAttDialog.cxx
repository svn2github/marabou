#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "GeneralAttDialog.h"
#include <iostream>

namespace std {} using namespace std;

Int_t GeneralAttDialog::fForceStyle;
Int_t GeneralAttDialog::fShowPSFile;
Int_t GeneralAttDialog::fUseRegexp;
Int_t GeneralAttDialog::fShowListsOnly;
Int_t GeneralAttDialog::fRememberLastSet;
Int_t GeneralAttDialog::fRememberZoom;
Int_t GeneralAttDialog::fUseAttributeMacro;
Int_t GeneralAttDialog::fMaxListEntries;
Int_t GeneralAttDialog::fContentLowLimit;
Int_t GeneralAttDialog::fVertAdjustLimit;
//_______________________________________________________________________

GeneralAttDialog::GeneralAttDialog(TGWindow * win)
{
static const char helptext[] =
"\n\
___________________________________________________________\n\
Force style, show hist with current style\n\
-----------------------------------------\n\
Use graphics option currently in use instead of options\n\
stored with the histogram on file\n\
___________________________________________________________\n\
Max Ents in Lists\n\
-----------------\n\
Too many entries in histlist (e.g. > 1000) make build up\n\
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
_____________________________________________________________\n\
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

   RestoreDefaults();
   fRow_lab->Add(new TObjString("CheckButton_Force style,show hist with current style"));
   fValp[ind++] = &fForceStyle;
   fRow_lab->Add(new TObjString("CheckButton_Show histlists only"));
   fValp[ind++] = &fShowListsOnly;
   fRow_lab->Add(new TObjString("CheckButton_Show PS file after creation"));
   fValp[ind++] = &fShowPSFile;
   fRow_lab->Add(new TObjString("CheckButton_Remember Expand settings (Marks)"));
   fValp[ind++] = &fRememberLastSet;
   fRow_lab->Add(new TObjString("CheckButton_Remember Zoomings (by left mouse)"));
   fValp[ind++] = &fRememberZoom;
   fRow_lab->Add(new TObjString("CheckButton_Use Attribute Macro"));
   fValp[ind++] = &fUseAttributeMacro;
   fRow_lab->Add(new TObjString("CheckButton_Use Regular expression syntax"));
   fValp[ind++] = &fUseRegexp;
   fRow_lab->Add(new TObjString("PlainIntVal_Max Ents in Lists"));
   fValp[ind++] = &fMaxListEntries;
	fRow_lab->Add(new TObjString("PlainIntVal_Fit2Dim Content Limit"));
	fValp[ind++] = &fContentLowLimit;
	fRow_lab->Add(new TObjString("PlainIntVal_Fit2Dim Vertical Adjust Limit"));
	fValp[ind++] = &fVertAdjustLimit;

   static Int_t ok;
   Int_t itemwidth = 360;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
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
   env.SetValue("GeneralAttDialog.fForceStyle", fForceStyle);
   env.SetValue("GeneralAttDialog.fShowPSFile", fShowPSFile);
   env.SetValue("GeneralAttDialog.fUseRegexp", fUseRegexp);
   env.SetValue("GeneralAttDialog.fShowListsOnly", fShowListsOnly);
   env.SetValue("GeneralAttDialog.fRememberLastSet", fRememberLastSet);
   env.SetValue("GeneralAttDialog.fRememberZoom", fRememberZoom);
   env.SetValue("GeneralAttDialog.fUseAttributeMacro", fUseAttributeMacro);
   env.SetValue("GeneralAttDialog.fMaxListEntries", fMaxListEntries);
   env.SetValue("GeneralAttDialog.fVertAdjustLimit", fVertAdjustLimit);
   env.SetValue("GeneralAttDialog.fContentLowLimit", fContentLowLimit);
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void GeneralAttDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fForceStyle = env.GetValue("GeneralAttDialog.fForceStyle", 1);
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
   fShowPSFile = env.GetValue("GeneralAttDialog.fShowPSFile", 1);
   fUseRegexp = env.GetValue("GeneralAttDialog.fUseRegexp", 0);
   fShowListsOnly = env.GetValue("GeneralAttDialog.fShowListsOnly", 0);
   fRememberLastSet = env.GetValue("GeneralAttDialog.fRememberLastSet", 1);
   fRememberZoom = env.GetValue("GeneralAttDialog.fRememberZoom", 1);
   fUseAttributeMacro = env.GetValue("GeneralAttDialog.fUseAttributeMacro", 0);
   fMaxListEntries =
   env.GetValue("GeneralAttDialog.fMaxListEntries", 333);
   fVertAdjustLimit = env.GetValue("GeneralAttDialog.fVertAdjustLimit", 0);
   fContentLowLimit = env.GetValue("GeneralAttDialog.fContentLowLimit", 0);
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

void GeneralAttDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
}

