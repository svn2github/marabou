#include <TROOT.h>
#include <TEnv.h>
#include <TH1.h>
#include <TLegend.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TRegexp.h>
#include <TGMsgBox.h>
#include "SetColor.h"
#include "TGMrbValuesAndText.h"
#include "WindowSizeDialog.h"
#include "HprStack.h"

ClassImp (HprStack)
static Int_t gSeqNumberStack = 0;

enum EGoHCommandIds {
   M_RememberOptions,
   M_SetOptions,
   M_OptionAxisAtt,
   M_Option1Dim,
   M_Option2Dim,
   M_Option2DimCol
};
//________________________________________________________________________

HprStack::HprStack(TList * hlist, Option_t *opt)
            : fHList(hlist), fOpt(opt)
{
//   cout << " ctor HprStack::" << this << endl;
   fCanvas = NULL;
   fDialog = NULL;
//
   fNhists = fHList->GetSize();
//   TH1* hist = 0;
   if (fNhists == 0) {
      cout << setred << "No histogram selected" << setblack << endl;
      return;

   } else if (fNhists > 32) {
      cout << setred << "Maximum 32 histograms allowed" << setblack << endl;
      return;
   }
   fFill_1Dim = new Int_t[fNhists];
   fFillColor = new Color_t[fNhists];
   fFillStyle = new Style_t[fNhists];
   fLineColor = new Color_t[fNhists];
   fLineStyle = new Style_t[fNhists];
   fLineWidth = new Width_t[fNhists];
   fMarkerColor = new Color_t[fNhists];
   fMarkerStyle = new Style_t[fNhists];
   fMarkerSize = new Size_t[fNhists];
   RestoreDefaults();
//  make a copy of  hlist, add its pointer to list of histlists fHistListList
//  pass its pointer to HTCanvas,
//  destructor of HTCanvas shall delete the list and remove its
//  pointer from fHistListList
//   TList * savelist = new TList();
//   TIter next(hlist);
//   while ( TObjString * objs = (TObjString*)next()) {
//      savelist->Add(new TObjString(*objs));
//   }
   gROOT->GetList()->Add(this);
   gROOT->GetListOfCleanups()->Add(this);
   BuildCanvas();
   BuildMenu();
}
//________________________________________________________________________

void HprStack::BuildCanvas()
{
   TString buf("cstack_");
   buf += gSeqNumberStack++;
   const char * tit = buf.Data();
   fCanvas = new TCanvas(buf.Data(), tit, WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury, fWindowXWidth, fWindowYWidth);
   WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
   WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;
//   cout << "TCanvas *ca = (TCanvas*)" <<  fCanvas << endl;

   TString hname;
   TString stitle("Stacked: ");

//   TString ytitle("");
//   Int_t nbins = 0, binlx = 0, binux = 0, binly = 0, binuy = 0;
   fStack = new THStack("hstack","");
//   cout << "THStack *st = (THStack*)" <<  fStack << endl;
   TH1 *hist;
   for(Int_t i=0; i<fNhists; i++) {
      hist = (TH1*)fHList->At(i);;
      if (!hist) {
//         cout << " Hist not found at: " << i << endl;
         continue;
      }
//      hist->Print();
      hname = hist->GetName();
      Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
      if(last_us >0)hname.Remove(last_us);
      last_us = hname.Last(';');    // chop off version
      if (last_us >0) hname.Remove(last_us);
      hist->SetName(hname);
      fStack->Add(hist);
      stitle += hist->GetName();
      if ( i < fNhists - 1 ) stitle += "_";
   }
//   cout << " fOpt: " << fOpt<< endl;
   fStack->Draw(fOpt);
   SetAttributes();
/*
   if (fRealStack || hist->GetDimension() == 2) hs->Draw();
   else           hs->Draw("nostack");

   if (binlx != 0 && binux != 0) {
      hs->GetXaxis()->SetRange(binlx, binux);
      hs->GetHistogram()->GetXaxis()->SetRange(binlx, binux);
   }
   if (hist->GetDimension() == 2 && binly != 0 && binuy != 0) {
      hs->GetYaxis()->SetRange(binly, binuy);
      hs->GetHistogram()->GetYaxis()->SetRange(binly, binuy);
   }
*/
//   cout << "stitle " << stitle<< endl;
   fStack->SetTitle(stitle);
   fCanvas->BuildLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
   fCanvas->Modified();
   fCanvas->Update();
}
//________________________________________________________________________

void HprStack::RecursiveRemove(TObject *obj)
{
//   cout <<  "HprStack::RecursiveRemove,obj " << obj << " "  << obj->GetName() << endl;
   if (obj == fCanvas) {
      TLegend *leg = (TLegend*)fCanvas->GetListOfPrimitives()->FindObject("TPave");
      if ( leg ) {
         fLegendX1 = leg->GetX1NDC();
//         cout << "fLegendX1 " << fLegendX1<< endl;
         fLegendX2 = leg->GetX2NDC();
         fLegendY1 = leg->GetY1NDC();
         fLegendY2 = leg->GetY2NDC();
      }
      gROOT->GetList()->Remove(this);
      gROOT->GetListOfCleanups()->Remove(this);
 //     cout <<  "RecursiveRemove,fWindowXWidth  " << fWindowXWidth << endl;
      delete this;
   }
}
//________________________________________________________________________

HprStack::~HprStack()
{
//   cout <<"dtor HprStack, fDialog " << fDialog << endl;
   if (fDialog) fDialog->CloseWindowExt();
   gROOT->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   SaveDefaults();
}
//________________________________________________________________________

void HprStack::BuildMenu()
{
//   cout << "HprStack::BuildMenu() " <<this << endl;
   fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
   TGMenuBar * menubar = fRootCanvas->GetMenuBar();
   TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
   fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Display", fMenu, layoh_left, menubar->GetPopup("Help"));
   fMenu->AddEntry("Set Graphics Attributes", M_SetOptions);
   fMenu->AddEntry("Store changed Attributes", M_RememberOptions);
   fMenu->Connect("Activated(Int_t)", "HprStack", this,
                        "HandleMenu(Int_t)");
   menubar->MapSubwindows();
   menubar->Layout();
}
//________________________________________________________________________

void HprStack::HandleMenu(Int_t id)
{
   switch (id) {

		case M_SetOptions:
            SetOptions();
			break;
		case M_RememberOptions:
            CRButtonPressed();
			break;
   }
}
//_______________________________________________________
void HprStack::SetOptions()
{
static const Char_t helptext[] =
"This menu controls the parameters when stacking histograms\n\
";
   fRow_lab = new TList();
	static void *fValp[50];
	Int_t ind = 0;
//	static Int_t dummy = 0;
   fRow_lab->Add(new TObjString("PlainIntVal_Window X Width"));
   fValp[ind++] = &fWindowXWidth;
   fRow_lab->Add(new TObjString("PlainIntVal+Window Y Width"));
   fValp[ind++] = &fWindowYWidth;
   fRow_lab->Add(new TObjString("CheckButton_Show Contour"));
   fValp[ind++] = &fShowContour;
   fRow_lab->Add(new TObjString("CheckButton+Show Errors"));
   fValp[ind++] = &fShowErrors;
   fRow_lab->Add(new TObjString("CheckButton+Draw Markers"));
   fValp[ind++] = &fShowMarkers;
   TString lab;
   for ( Int_t i = 0; i < fNhists; i++ ) {
      lab = "ColorSelect_FillCol[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fFillColor[i];
      lab = "CheckButton+Fill[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fFill_1Dim[i];
      lab = "Fill_Select+fFillSt[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fFillStyle[i];

      lab = "ColorSelect_LineCol[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fLineColor[i];
      lab = "LineSSelect+LineSty[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fLineStyle[i];
      lab = "PlainShtVal+LineWid[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fLineWidth[i];

      lab = "ColorSelect_MarkColor[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fMarkerColor[i];
      lab = "Mark_Select+MarkStyle[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fMarkerStyle[i];
      lab = "Float_Value+fMarkSize[";
      lab += i; lab += "]";
      fRow_lab->Add(new TObjString(lab));
      fValp[ind++] = &fMarkerSize[i];
   }
   Int_t itemwidth = 420;
   static Int_t ok = 0;
   fDialog =
		new TGMrbValuesAndText ("Set Params", NULL, &ok, itemwidth,
								fRootCanvas, NULL, NULL, fRow_lab, fValp,
								NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________
void HprStack::RestoreDefaults()
{
   TEnv env(".hprrc");
   fWindowXWidth = env.GetValue("HprStack.fWindowXWidth", 800);
   fWindowYWidth = env.GetValue("HprStack.fWindowYWidth", 600);
   if (fWindowXWidth < 20 || fWindowXWidth > 1600) fWindowXWidth = 800;
   if (fWindowYWidth < 20 || fWindowYWidth > 1200) fWindowYWidth = 600;
   fLegendX1 = env.GetValue("HprStack.fLegendX1", 0.7);
   fLegendX2 = env.GetValue("HprStack.fLegendX2", 0.95);
   fLegendY1 = env.GetValue("HprStack.fLegendY1", 0.85);
   fLegendY2 = env.GetValue("HprStack.fLegendY2", 0.95);
   TString lab;
   for ( Int_t i = 0; i < fNhists; i++ ) {
      lab = "HprStack.fFill_1Dim["; lab+=i; lab+="]";
      fFill_1Dim[i] = 0;
      if (env.Lookup(lab))
         fFill_1Dim[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fFillColor["; lab+=i; lab+="]";
      fFillColor [i] = i+2;
      if (env.Lookup(lab))
         fFillColor[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fFillStyle["; lab+=i; lab+="]";
      fFillStyle[i] = 3001;
      if (env.Lookup(lab))
         fFillStyle[i]  = env.GetValue(lab, 0);

      lab = "HprStack.fLineColor["; lab+=i; lab+="]";
      fLineColor[i] = i+2;
      if (env.Lookup(lab))
         fLineColor[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fLineStyle["; lab+=i; lab+="]";
      fLineStyle[i] = 1;
      if (env.Lookup(lab))
         fLineStyle[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fLineWidth["; lab+=i; lab+="]";
      fLineWidth[i] = 1;
      if (env.Lookup(lab))
         fLineWidth[i]  = env.GetValue(lab, 0);

      lab = "HprStack.fMarkerColor["; lab+=i; lab+="]";
      fMarkerColor[i] = i+2;
      if (env.Lookup(lab))
         fMarkerColor[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fMarkerStyle["; lab+=i; lab+="]";
      fMarkerStyle[i] = 1;
      if (env.Lookup(lab))
         fMarkerStyle[i]  = env.GetValue(lab, 0);
      lab = "HprStack.fMarkerSize["; lab+=i; lab+="]";
      fMarkerSize[i] = 1;
      if (env.Lookup(lab))
         fMarkerSize[i]  = env.GetValue(lab, 0);
   }
   fShowContour   = env.GetValue("HprStack.fShowContour", 0);
   fShowErrors    = env.GetValue("HprStack.fShowErrors", 0);
   fShowMarkers   = env.GetValue("HprStack.fShowMarkers", 0);
}
//_______________________________________________________

void HprStack::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("HprStack.fWindowXWidth", fWindowXWidth);
   env.SetValue("HprStack.fWindowYWidth", fWindowYWidth);
   env.SetValue("HprStack.fLegendX1", fLegendX1);
   env.SetValue("HprStack.fLegendX2", fLegendX2);
   env.SetValue("HprStack.fLegendY1", fLegendY1);
   env.SetValue("HprStack.fLegendY2", fLegendY2);

   TString lab;
   for ( Int_t i = 0; i < fNhists; i++ ) {
      lab = "HprStack.fFill_1Dim["; lab+=i; lab+="]";
      env.SetValue(lab,fFill_1Dim[i]);
      lab = "HprStack.fFillColor["; lab+=i; lab+="]";
      env.SetValue(lab,fFillColor[i]);
      lab = "HprStack.fFillStyle["; lab+=i; lab+="]";
      env.SetValue(lab,fFillStyle[i]);

      lab = "HprStack.fLineColor["; lab+=i; lab+="]";
      env.SetValue(lab,fLineColor[i]);
      lab = "HprStack.fLineStyle["; lab+=i; lab+="]";
      env.SetValue(lab,fLineStyle[i]);
      lab = "HprStack.fLineWidth["; lab+=i; lab+="]";
      env.SetValue(lab,fLineWidth[i]);

      lab = "HprStack.fMarkerColor["; lab+=i; lab+="]";
      env.SetValue(lab,fMarkerColor[i]);
      lab = "HprStack.fMarkerStyle["; lab+=i; lab+="]";
      env.SetValue(lab,fMarkerStyle[i]);
      lab = "HprStack.fMarkerSize["; lab+=i; lab+="]";
      env.SetValue(lab,fMarkerSize[i]);
  }

   env.SetValue("HprStack.fShowContour", fShowContour);
   env.SetValue("HprStack.fShowErrors",  fShowErrors );
   env.SetValue("HprStack.fShowMarkers", fShowMarkers );
   env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void HprStack::CloseDown(Int_t)
{
//   cout << "HprStack::CloseDown() fDialog " << fDialog<< endl;
   fDialog = NULL;
}
//_______________________________________________________________________
void HprStack::CRButtonPressed(Int_t widgetId, Int_t buttonId, TObject *obj)
{
//   cout << "HprStack::CRButtonPressed " << widgetId << " " << buttonId << endl;
   fWindowXWidth = (Int_t)fCanvas->GetWindowWidth();
   fWindowYWidth = (Int_t)fCanvas->GetWindowHeight();
   TLegend *leg = (TLegend*)fCanvas->GetListOfPrimitives()->FindObject("TPave");
   if ( leg ) {
      fLegendX1 = leg->GetX1NDC();
//      cout << "fLegendX1 " << fLegendX1<< endl;
      fLegendX2 = leg->GetX2NDC();
      fLegendY1 = leg->GetY1NDC();
      fLegendY2 = leg->GetY2NDC();
   }
   SetAttributes();
   SaveDefaults();
   fCanvas->Modified();
   fCanvas->Update();
}
//_______________________________________________________

void HprStack::SetAttributes()
{
   TObjArray * stack = fStack->GetStack();
   TList * orighist = fStack->GetHists();
	TString opt;
	if (fShowContour) opt="HIST";
	if (fShowErrors ) opt+= "E1";
   for(Int_t i=0; i<fNhists; i++) {
      TH1 * hist = (TH1*)stack->At(i);
      TH1 * ohist = (TH1*)orighist->At(i);
      if ( fFill_1Dim[i] ) {
         hist->SetFillStyle(fFillStyle[i]);
         hist->SetFillColor(fFillColor[i]);
         ohist->SetFillStyle(fFillStyle[i]);
         ohist->SetFillColor(fFillColor[i]);
//         cout  << "fFillStyle " << fFillStyle[i]<< " fFillColor " << fFillColor[i] << endl;
      } else {
         hist->SetFillStyle(0);
         ohist->SetFillStyle(0);
     }
      if (hist->GetDimension() == 1) {
         hist->SetLineColor(fLineColor[i]);
         hist->SetLineStyle(fLineStyle[i]);
         hist->SetLineWidth(fLineWidth[i]);
         ohist->SetLineColor(fLineColor[i]);
         ohist->SetLineStyle(fLineStyle[i]);
         ohist->SetLineWidth(fLineWidth[i]);
      }
      if ( fShowMarkers ) {
         hist->SetMarkerStyle(fMarkerStyle[i]);
         hist->SetMarkerColor(fMarkerColor[i]);
         hist->SetMarkerSize(fMarkerSize[i]);
         ohist->SetMarkerStyle(fMarkerStyle[i]);
         ohist->SetMarkerColor(fMarkerColor[i]);
         ohist->SetMarkerSize(fMarkerSize[i]);
      } else {
         hist->SetMarkerSize(0);
      }
   }
   fCanvas->cd();
   opt += fOpt;
//   cout << "DrawOpt: "<< opt << endl;
   fStack->SetDrawOption(opt);
   fCanvas->Modified();
   fCanvas->Update();
}
