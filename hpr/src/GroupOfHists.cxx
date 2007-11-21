#include <TEnv.h>
#include <TH1.h>
#include <TString.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TGMsgBox.h>
#include "TGMrbInputDialog.h"
#include "support.h"
#include "SetColor.h"
#include "GroupOfHists.h"
#include "Set1DimOptDialog.h"
#include "Set2DimOptDialog.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"


ClassImp (GoHTimer)
ClassImp (GroupOfHists)

GoHTimer::GoHTimer(Long_t ms, Bool_t synch, GroupOfHists * goh):TTimer(ms,synch){
     cout << "init GoHTimer" << endl;
   fGoH = goh;
   gSystem->AddTimer(this);
}

Bool_t GoHTimer::Notify() {
   fGoH->UpdateHists();
   Reset();
   return kTRUE;
};


enum EGoHCommandIds {
   M_AllAsSel,
   M_RebinAll,
   M_CalAllAsSel,
   M_CommonRotate,
   M_AllAsFirst,
   M_SetOptions,
   M_AllAsSelRangeOnly,
   M_ActivateTimer,
   M_Option1Dim,
   M_Option2Dim,
   M_Option2DimCol
};
//________________________________________________________________________

GroupOfHists::GroupOfHists(TList * hlist, HistPresent * hpr, const Char_t *title)
            : fHistPresent(hpr)
{
 //  cout << " ctor GroupOfHists::" << this << endl;
   fCanvas = NULL;
   fTimer = NULL;
   fDialog = NULL;
   fAnyFromSocket = kFALSE;
   RestoreDefaults();
   SetColorModeDialog::RestoreDefaults();
   SetColorModeDialog::SetColorMode();
//
   Int_t nsel = hlist->GetSize();
   Int_t nx = 1, ny =1;
   TH1* hist = 0;
   if (nsel == 0) {
      cout << setred << "No histogram selected" << setblack << endl;
      return;

   } else if (nsel > 32) {
      cout << setred << "Maximum 32 histograms allowed" << setblack << endl;
      return;
   }

//  make a copy of  hlist, add its pointer to list of histlists fHistListList
//  pass its pointer to HTCanvas,
//  destructor of HTCanvas shall delete the list and remove its
//  pointer from fHistListList
   TIter next(hlist);
   while ( TObjString * objs = (TObjString*)next()) {
      fHistList.Add(new TObjString(*objs));
   }
   if (nsel == 2 ) {nx = 1, ny = 2;};
   if (nsel >= 3 ) {nx = 2, ny = 2;};
   if (nsel >= 5 ) {nx = 2, ny = 3;};
   if (nsel >= 7 ) {nx = 3, ny = 3;};
   if (nsel >= 10) {nx = 3, ny = 4;};
   if (nsel >= 13) {nx = 4, ny = 4;};

   if (nsel >= 16) {nx = 4, ny = 5;};
   if (nsel == 21) {nx = 7, ny = 3;};
   if (nsel >  21) {nx = 5, ny = 5;};

   if (nsel > 25) {nx = 6, ny = 5;};
   if (nsel == 32) {nx = 8, ny = 4;};

   if (fArrangeOnTop)      {nx = 1; ny = nsel;};
   if (fArrangeSideBySide) {nx =  nsel; ny = 1;};
   TString buf("cmany_");
   buf += fHistPresent->fSeqNumberMany++;
//   const char * tit = buf.Data();
//   if (title) tit = title;
   buf.Prepend("GroupOfHists_");
   this->SetName(buf.Data());
//   this->SetTitle(tit);
   gROOT->GetList()->Add(this);
   gROOT->GetListOfCleanups()->Add(this);
   fCanvas =  new HTCanvas(buf.Data(), "GroupOfHists",
              WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury, fWindowXWidth, fWindowYWidth);
   fCanvas->SetBit(kMustCleanup);
   WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
   WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;

   fHistPresent->fCanvasList->Add(fCanvas);
   fCanvas->Divide(nx, ny, fMarginX, fMarginY);
//   fCanvas->SetEditable(kTRUE);
   TEnv * lastset = 0;
   TString hname;
   fAnyFromSocket = kFALSE;
   TPad * firstpad = NULL;
   for(Int_t i=0; i<nsel; i++) {
      fCanvas->cd(i+1);
      TPad * p = (TPad *)gPad;
      TString cmd2("((GroupOfHists*)gROOT->GetList()->FindObject(\"");
      cmd2 += GetName();
      cmd2 += "\"))->auto_exec()";
      p->AddExec("ex2", cmd2.Data());
      if (firstpad == NULL) firstpad = p;
      hist = fHistPresent->GetSelHistAt(i, hlist);
      if (!hist) {
//         cout << " Hist not found at: " << i << endl;
         continue;
      }
      TString fname = ((TObjString *)hlist->At(i))->String();
      if (fname.Index("Socket") == 0) fAnyFromSocket = kTRUE;
      hname = hist->GetName();
 //     cout << "Bef: " << hname << endl;

      Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
      if(last_us >0)hname.Remove(last_us);
      TRegexp sem(";");
      hname(sem) ="_";
      hist->SetName(hname);
      lastset = GetDefaults(hname);
//      cout << "GetDefaults: "  << hname << " " << lastset << endl;
      if (lastset) {
//         lastset->Print();
         if (lastset->Lookup("fRangeLowX") )
            hist->GetXaxis()->Set(hist->GetNbinsX(),
            lastset->GetValue("fRangeLowX", 0),
            lastset->GetValue("fRangeUpX",  0));
         if (lastset->Lookup("fBinlx") )
         {
            hist->GetXaxis()->SetRange(
            lastset->GetValue("fBinlx", 0),
            lastset->GetValue("fBinux", 0));
         }
         if (lastset->Lookup("fXtitle") )
           hist->GetXaxis()->SetTitle(lastset->GetValue("fXtitle",""));
         if (lastset->Lookup("fYtitle") )
           hist->GetYaxis()->SetTitle(lastset->GetValue("fYtitle",""));
      }

      if (hist->GetDimension() == 2) {
         hist->Draw(fDrawOpt2Dim);
         if (lastset) {
            if (lastset->GetValue("LogZ", 0) )p->SetLogz();
            if (lastset->Lookup("fRangeLowY") )
               hist->GetXaxis()->Set(hist->GetNbinsY(),
               lastset->GetValue("fRangeLowY", 0),
               lastset->GetValue("fRangeUpY",  0));
            if (lastset->Lookup("fBinly") )
            {
               hist->GetYaxis()->SetRange(
               lastset->GetValue("fBinly", 0),
               lastset->GetValue("fBinuy", 0));
            }
         }
      } else {
         if (lastset && lastset->GetValue("LogY", 0) )p->SetLogy();
         TString drawopt;
         hist->Draw(drawopt.Data());
//         gStyle->SetOptTitle(fHistPresent->GetShowTitle());
         if (fShowContour) drawopt = "hist";
         if (fShowErrors)  drawopt += "e1";
         hist->SetLineColor(fHistLineColor);
         if (fFill1Dim) {
            hist->SetFillStyle(fHistFillStyle);
            hist->SetFillColor(fHistFillColor);
//            cout << "fHistPresent->fHistFillStyle " << fHistPresent->fHistFillStyle << endl;
         } else {
            hist->SetFillStyle(0);
         }
         hist->Draw(drawopt.Data());

         TAxis * xa = hist->GetXaxis();
         TAxis * ya = hist->GetYaxis();
         ya->SetLabelSize(gStyle->GetLabelSize("Y") * 0.5 * ny); // nb y det text size
         xa->SetLabelSize(gStyle->GetLabelSize("X") * 0.5 * ny);
         xa->SetTitleSize(gStyle->GetTitleSize("X") * 0.5 * ny);
         ya->SetTitleSize(gStyle->GetTitleSize("Y") * 0.5 * ny);
         if (nx > 3)xa->SetNdivisions(205);
         if (ny > 3)ya->SetNdivisions(205);

//        hist->SetTitleSize(gStyle->GetTitleSize("C") * 0.8 * ny);
      }
      fHistPresent->SetCurrentHist(hist);
/*
      if (fHistPresent->fUseAttributeMacro && !gSystem->AccessPathName(attrname, kFileExists)) {
         gROOT->LoadMacro(attrname);
         TString cmd = attrname;
         cmd.Remove(cmd.Index("."),100);
         cmd = cmd + "(\"" + gPad->GetName()  + "\",\"" + hist->GetName() + "\")";
//         cout << cmd << endl;
         gROOT->ProcessLine((const char *)cmd);
         gPad->Modified(kTRUE);
         gPad->Update();
      }
*/
   }
   if (!fCanvas->GetAutoExec())
       fCanvas->ToggleAutoExec();
   gPad->Modified(kTRUE);
   gPad->Update();
   BuildMenu();
   if (firstpad) {
      if (fShowAllAsFirst != 0 || (fMarginX <= 0 && fMarginY <= 0))
         ShowAllAsSelected(firstpad, fCanvas, 0, NULL);
      firstpad->cd();
   }
   fCanvas->SetEditable(kTRUE);
}
//________________________________________________________________________

void GroupOfHists::RecursiveRemove(TObject *obj)
{
//   cout <<  "RecursiveRemove,obj " << obj << " "  << obj->GetName() << endl;
   if (obj == fCanvas) {
      fWindowXWidth = fCanvas->GetWindowWidth();
      fWindowYWidth = fCanvas->GetWindowHeight();
      gROOT->GetList()->Remove(this);
      gROOT->GetListOfCleanups()->Remove(this);
 //     cout <<  "RecursiveRemove,fWindowXWidth  " << fWindowXWidth << endl;
      delete this;
   }
}
//________________________________________________________________________

GroupOfHists::~GroupOfHists()
{
//   cout <<"dtor GroupOfHists, fDialog " << fDialog << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fTimer) delete fTimer;
   gROOT->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   SaveDefaults();
}
//________________________________________________________________________

void GroupOfHists::ActivateTimer(Int_t delay)
{
   if(fTimer){
      cout << "Deleting existing timer" << endl;
      delete fTimer;
      fTimer = 0;
   }
   if(delay > 0) fTimer = new GoHTimer(delay, kTRUE, this);
}
//________________________________________________________________________

void GroupOfHists::UpdateHists()
{
//   if (!fHistPresent) return;
//   if (!fHistList)    return;
//   if (fHistPresent->fAnyFromSocket && !(fHistPresent->fComSocket)) return;
//      TList * hlist = fHistPresent->GetSelectedHist();
//   cout << "GroupOfHists::UpdateHists() " << endl;
   Int_t nhist = fHistList.GetSize();
   if(nhist <= 0)return;
   Int_t fx, lx, fy, ly;
   for(Int_t i=0; i<nhist; i++){
      fCanvas->cd(i+1);
//         TPad * p = gPad;
      TH1 * hold  = GetTheHist(gPad);
      fx =  hold->GetXaxis()->GetFirst();
      lx =  hold->GetXaxis()->GetLast();
      TH1 * hist = fHistPresent->GetSelHistAt(i,&fHistList);
      if (!hist) {
         cout << setred << "Cant get histogram, M_analyze stopped? " << endl;
         if(fTimer){
            cout << "Deleting existing timer" << endl;
            delete fTimer;
            fTimer = 0;
//            fHistPresent->fSocketIsOpen = kFALSE;
         }
          cout << setblack << endl;

         return;
      }
//     hist->Print();
      if(hist->GetDimension() == 2){
         fy =  hold->GetYaxis()->GetFirst();
         ly =  hold->GetYaxis()->GetLast();
         hist->Draw(fDrawOpt2Dim);
         hist->GetXaxis()->SetRange(fx, lx);
         hist->GetYaxis()->SetRange(fy, ly);
      } else {
         TString drawopt;
         if(fShowContour)drawopt = "hist";
         if(fShowErrors)drawopt += "e1";
//         if(fFill1Dim){
//            hist->SetFillStyle(1001);
//			hist->SetFillColor(44);
//         } else hist->SetFillStyle(0);
         hist->Draw(drawopt.Data());
         hist->GetXaxis()->SetRange(fx, lx);
      }
   }
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
}
//________________________________________________________________________

void GroupOfHists::BuildMenu()
{
//   cout << "GroupOfHists::BuildMenu() " <<this << endl;
   fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
   TGMenuBar * menubar = fRootCanvas->GetMenuBar();
   TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
   fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Display", fMenu, layoh_left, menubar->GetPopup("Help"));
   fMenu->AddEntry("Show now all as selected, Range only", M_AllAsSelRangeOnly);
   fMenu->AddEntry("Show now all as selected, Range, Min, Max ",  M_AllAsSel);
//   fMenu->AddEntry("Calibrate all as selected",  M_CalAllAsSel);
//   fMenu->AddEntry("Show always all as First", M_AllAsFirst);
//   if (fShowAllAsFirst != 0) fMenu->CheckEntry(M_AllAsFirst);
//   else                 fMenu->UnCheckEntry(M_AllAsFirst);
   fMenu->AddEntry("Rebin all",  M_RebinAll);
   fMenu->AddSeparator();
   fMenu->AddEntry("Activate automatic update",  M_ActivateTimer);
   fMenu->Connect("Activated(Int_t)", "GroupOfHists", this,
                  "HandleMenu(Int_t)");
   fMenu->AddEntry("Activate simultanous rotation", M_CommonRotate);

   fOptionMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Options", fOptionMenu, layoh_left, menubar->GetPopup("Help"));
   fOptionMenu->AddEntry("Specific to this window", M_SetOptions);
   fOptionMenu->AddEntry("How to display a 1-dim histogram", M_Option1Dim);
   fOptionMenu->AddEntry("How to display a 2-dim histogram ", M_Option2Dim);
   fOptionMenu->AddEntry("Color mode of 2-dim histogram", M_Option2DimCol);
   fOptionMenu->Connect("Activated(Int_t)", "GroupOfHists", this,
                        "HandleMenu(Int_t)");
   menubar->MapSubwindows();
   menubar->Layout();
}
//________________________________________________________________________

void GroupOfHists::HandleMenu(Int_t id)
{
   switch (id) {

      case M_ActivateTimer:
         if (fTimer != NULL) {
            ActivateTimer(-1);    // deactivate
            fMenu->UnCheckEntry(M_ActivateTimer);
         } else {
            Int_t tms = 0;
            tms = (Int_t)(1000 * fAutoUpdateDelay);
            if(tms <= 0) tms = 2000;
            cout << "Setting AutoUpdateDelay to " << tms << endl;
            ActivateTimer(tms);    // in milli second
            fMenu->CheckEntry(M_ActivateTimer);
         }
         break;
      case M_RebinAll:
         RebinAll(fCanvas);
         break;
      case M_AllAsSelRangeOnly:
         ShowAllAsSelected(gPad, fCanvas, 0,
                   (TGWindow*)fRootCanvas);
         break;
//      case M_CalAllAsSel:
//         CalibrateAllAsSelected(gPad, fHCanvas, 1);
//         break;

      case M_AllAsSel:
         ShowAllAsSelected(gPad, fCanvas, 1);
         break;
      case M_SetOptions:
         SetOptions();
         break;
      case M_CommonRotate:
         {
         if (fCommonRotate !=0) {
            fCommonRotate = 0;
            fMenu->UnCheckEntry(M_CommonRotate);
         } else {
            fCommonRotate = 1;
            fMenu->CheckEntry(M_CommonRotate);
         }
         TEnv env(".hprrc");		// inspect ROOT's environment
         env.SetValue("GroupOfHists.CommonRotate", fCommonRotate);
         env.SaveLevel(kEnvLocal);
         }
         break;
      case M_AllAsFirst:
         {
         if (fShowAllAsFirst !=0) {
            fShowAllAsFirst = 0;
            fMenu->UnCheckEntry(M_AllAsFirst);
         } else {
            fShowAllAsFirst = 1;
            fMenu->CheckEntry(M_AllAsFirst);
         }
         TEnv env(".hprrc");		// inspect ROOT's environment
         env.SetValue("GroupOfHists.ShowAllAsFirst", fShowAllAsFirst);
         env.SaveLevel(kEnvLocal);
         }
         break;

		case M_Option1Dim:
            new Set1DimOptDialog(fRootCanvas);
			break;

		case M_Option2Dim:
            new Set2DimOptDialog(fRootCanvas);
			break;

		case M_Option2DimCol:
            new SetColorModeDialog(fRootCanvas);
			break;
   }
}
//________________________________________________________________________

void GroupOfHists::auto_exec()
{
   static Double_t phi;
   static Double_t theta;
   Int_t event = gPad->GetEvent();

//   cout << "event "<< event << endl;
   if (event != kButton1Down && event != kMouseMotion && event != kButton1Up) return;
   TObject *select = gPad->GetSelected();
   if(!select) return;
//   cout << "auto_exec_2() selected " << select->GetName() << endl;
   if(gPad == gPad->GetMother()){
    cout << "not in divided" << endl;
      return;
   }
   if (select->InheritsFrom("TPad")) {
      TVirtualPad *pad = (TVirtualPad*)select;
      TH1 * hist = GetTheHist(pad);
      if (hist && hist->GetDimension() == 1 && event == kButton1Down) {
         if (fHistPresent) fHistPresent->ShowHist(hist);
      }
      return;
   }
   if (select->InheritsFrom("TH2")) {
      if (!fCommonRotate) {
         if(event == kButton1Down) {
            if (fHistPresent) {
               TH1 *hist = (TH1*)select;
               fHistPresent->ShowHist(hist);
            }
         }
         return;
      }
   }
   if (fCommonRotate) {
      TList * l = gPad->GetListOfPrimitives();
      TIter next(l);
      TObject * o;
      while ( (o = next()) ){
         if (o->InheritsFrom("TH2")) {
            TH1* h = (TH1*)o;
//            cout << "h->GetDrawOption() " << h->GetDrawOption()<< endl;
            TString dropt = h->GetDrawOption();

            if (dropt.Contains("LEGO",TString::kIgnoreCase) ||
                dropt.Contains("SURF",TString::kIgnoreCase) ) {
            	if (event == kButton1Down) {
               	phi = gPad->GetPhi();
               	theta = gPad->GetTheta();
            	} else if (event == kButton1Up || event == kButton1Motion) {
               	Double_t phi_n = gPad->GetPhi();
               	Double_t theta_n = gPad->GetTheta();
               	if (phi != phi_n || theta != theta_n) {
                  	TList * pl = gPad->GetMother()->GetListOfPrimitives();
                  	TIter nextpad(pl);
                  	TObject * p;
                  	while ( (p = nextpad()) ) {
                     	if (p->InheritsFrom("TPad")) {
                        	TPad* pp = (TPad*)p;
                        	pp->SetPhi(phi_n);
                        	pp->SetTheta(theta_n);
                        	pp->Modified();
                        	pp->Update();
                     	}
                  	}
               	}
            	}
            }
            return;
         }
      }
   }
}
//____________________________________________________________________________

TH1 * GroupOfHists::GetTheHist(TVirtualPad * pad)
{
   TList *l = pad->GetListOfPrimitives();
   TIter next(l);
   TObject *o;
   while ( (o = next()) ) {
      if (o->InheritsFrom("TH1"))
         return (TH1 *) o;
   }
   return NULL;
}
//_______________________________________________________________________________________

void GroupOfHists::ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, Int_t mode, TGWindow * win)
{
// find reference histogram

   TList *l = canvas->GetListOfPrimitives();
   TObject *obj;
   TH1 *href = GetTheHist(pad);
   if (!href) {
//      WarnBox("Selected pad contains no hist,\n please select with middle mouse");
      cout << "No pad selected, using first" << endl;
      TIter next(l);
      while ( (obj = next()) ) {
         if (obj->InheritsFrom(TPad::Class())) {
            TPad *p = (TPad *) obj;
            href = GetTheHist(p);
            if (href) {
               p->cd();
               break;
            }
         }
      }
   }
   if (!href) {
      WarnBox
          ("Selected pad contains no hist,\n please select with middle mouse", win);
      return;
   }
   cout << "ShowAllAsSelected " << href->GetName()<< endl;
   TAxis *xa = href->GetXaxis();
   Axis_t lowedge = xa->GetBinLowEdge(xa->GetFirst());
   Axis_t upedge = xa->GetBinLowEdge(xa->GetLast()) +
       xa->GetBinWidth(xa->GetLast());
   Axis_t min = 0, max = 0;
   if (mode > 0) {
      min = href->GetMinimum();
      max = href->GetMaximum();
   }
   l = canvas->GetListOfPrimitives();
   TIter next(l);
   while ( (obj = next()) ) {
      if (obj->InheritsFrom(TPad::Class())) {
         TPad *p = (TPad *) obj;
         TH1 *hist = GetTheHist(p);
         if (hist) {
            hist->GetXaxis()->SetRangeUser(lowedge, upedge);
            p->SetLogy(pad->GetLogy());
            if (mode > 0 && pad->GetLogy() == 0) {
               hist->SetMinimum(min);
               hist->SetMaximum(max);
            }
            p->Modified(kTRUE);
            p->Update();
         }
      }
   }
   canvas->Modified(kTRUE);
   canvas->Update();
}
//_______________________________________________________________________________________

void GroupOfHists::RebinAll(TCanvas * canvas)
{

   static Int_t ngroup = 2;
   Bool_t ok;
   ngroup = GetInteger("Rebin value", ngroup, &ok);
   if (!ok || ngroup <= 0)
      return;
   TList *l = canvas->GetListOfPrimitives();
   TIter next(l);
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom(TPad::Class())) {
         TPad *p = (TPad *) obj;
         TH1 *hist = GetTheHist(p);
         if (hist) {
//            cout << hist->GetName()<< ", first, last, min, max  "
//            << first << " " << last  << " " << min << " " << max<< endl;
            Int_t first = hist->GetXaxis()->GetFirst();
            Int_t last = hist->GetXaxis()->GetLast();
            hist->Rebin(ngroup);
            first /= ngroup;
            last /= ngroup;
            hist->GetXaxis()->SetRange(first, last);
            p->Modified(kTRUE);
            p->Update();
         }
      }
   }
   canvas->Modified(kTRUE);
   canvas->Update();
}
//_______________________________________________________
void GroupOfHists::SetOptions()
{
static const Char_t helptext[] =
"This menu controls the parameters when\n\
multiple histograms are shown in one canvas.\n\
\"X - Y Width\":  Size of the canvas in pixel\n\
\"X - Y Margin\": The space between the pictures.\n\
Histograms may be updated automatically:\n\
\"AutoUpdateDelay\": determines the interval (sec)\n\
With 3d Views (LEGO, SURF) 2-dim histograms\n\
are rotated simultanously, if \n\
\"Enable CommonRotate\" is activated\n\
\"Always ShowAllAsFirst\": Always use range of first\n\
histogram displayed.\n\
Histograms may be arranged: on top of each other,\n\
side by side or as tiles\n\
";
   fRow_lab = new TList();
	static void *fValp[50];
	Int_t ind = 0;
//	static Int_t dummy = 0;
   fRow_lab->Add(new TObjString("PlainIntVal_Window X Width"));
   fValp[ind++] = &fWindowXWidth;
   fRow_lab->Add(new TObjString("PlainIntVal+Window Y Width"));
   fValp[ind++] = &fWindowYWidth;
   fRow_lab->Add(new TObjString("DoubleValue_X Margin"));
   fValp[ind++] = &fMarginX;
   fRow_lab->Add(new TObjString("DoubleValue+Y Margin"));
   fValp[ind++] = &fMarginY;
   fRow_lab->Add(new TObjString("PlainIntVal_AutoUpdateDelay"));
   fValp[ind++] = &fAutoUpdateDelay;
   fRow_lab->Add(new TObjString("CheckButton+Enable CommonRotate"));
   fValp[ind++] = &fCommonRotate;
   fRow_lab->Add(new TObjString("CheckButton+Always ShowAllAsFirst"));
   fValp[ind++] = &fShowAllAsFirst;
   fRow_lab->Add(new TObjString("RadioButton_ArrangeOnTop"));
   fValp[ind++] = &fArrangeOnTop;
   fRow_lab->Add(new TObjString("RadioButton+ArrangeSideBySide"));
   fValp[ind++] = &fArrangeSideBySide;
   fRow_lab->Add(new TObjString("RadioButton+ArrangeAsTiles"));
   fValp[ind++] = &fArrangeAsTiles;
   Int_t itemwidth = 420;
   static Int_t ok = 0;
   fDialog =
		new TGMrbValuesAndText ("Set Params", NULL, &ok, itemwidth,
								fRootCanvas, NULL, NULL, fRow_lab, fValp,
								NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________
void GroupOfHists::RestoreDefaults()
{
   TEnv env(".hprrc");
   fWindowXWidth = env.GetValue("GroupOfHists.fWindowXWidth", 800);
   fWindowYWidth = env.GetValue("GroupOfHists.fWindowYWidth", 800);
   if (fWindowXWidth < 20 || fWindowXWidth > 1600) fWindowXWidth = 800;
   if (fWindowYWidth < 20 || fWindowYWidth > 1200) fWindowYWidth = 800;
   fMarginX = env.GetValue("GroupOfHists.fMarginX", 0.01);
   fMarginY = env.GetValue("GroupOfHists.fMarginY", 0.01);
   if (fMarginX < 0 || fMarginX > 0.3) fMarginX = 0.01;
   if (fMarginY < 0 || fMarginY > 0.3) fMarginY = 0.01;
   fAutoUpdateDelay = env.GetValue("GroupOfHists.fAutoUpdateDelay", 5);
   fCommonRotate = env.GetValue("GroupOfHists.fCommonRotate", 1);
   fShowAllAsFirst = env.GetValue("GroupOfHists.fShowAllAsFirst", 0);
   fArrangeOnTop = env.GetValue("GroupOfHists.fArrangeOnTop", 0);
   fArrangeSideBySide = env.GetValue("GroupOfHists.fArrangeSideBySide", 0);
   fArrangeAsTiles = env.GetValue("GroupOfHists.fArrangeAsTiles", 1);
   fFill1Dim      = env.GetValue("Set1DimOptDialog.fFill1Dim", 0);
   fHistFillColor = env.GetValue("Set1DimOptDialog.fHistFillColor", 2);
   fHistLineColor = env.GetValue("Set1DimOptDialog.fHistLineColor", 1);
   fHistFillStyle = env.GetValue("Set1DimOptDialog.fHistFillStyle", 0);
   fHistLineStyle = env.GetValue("Set1DimOptDialog.fHistLineStyle", 1);
   fHistLineWidth = env.GetValue("Set1DimOptDialog.fHistLineWidth", 2);
   fShowContour   = env.GetValue("Set1DimOptDialog.fShowContour", 0);
   fShowErrors    = env.GetValue("Set1DimOptDialog.fShowErrors", 0);
   fDrawOpt2Dim  = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
}
//_______________________________________________________
void GroupOfHists::SaveDefaults()
{
   TEnv env(".hprrc");
 //  fWindowXWidth = fCanvas->GetWw();
 //  fWindowYWidth = fCanvas->GetWh();
   env.SetValue("GroupOfHists.fWindowXWidth", fWindowXWidth);
   env.SetValue("GroupOfHists.fWindowYWidth", fWindowYWidth);
   env.SetValue("GroupOfHists.fMarginX", fMarginX);
   env.SetValue("GroupOfHists.fMarginY", fMarginY);
   env.SetValue("GroupOfHists.fAutoUpdateDelay", fAutoUpdateDelay);
   env.SetValue("GroupOfHists.fCommonRotate", fCommonRotate);
   env.SetValue("GroupOfHists.fShowAllAsFirst", fShowAllAsFirst);
   env.SetValue("GroupOfHists.fArrangeOnTop", fArrangeOnTop);
   env.SetValue("GroupOfHists.fArrangeSideBySide", fArrangeSideBySide);
   env.SetValue("GroupOfHists.fArrangeAsTiles", fArrangeAsTiles);
   env.SaveLevel(kEnvLocal);
   if (!fCanvas->GetAutoExec())
       fCanvas->ToggleAutoExec();
}
//_______________________________________________________________________

void GroupOfHists::CloseDown(Int_t)
{
//   cout << "GroupOfHists::CloseDown() fDialog " << fDialog<< endl;
   fDialog = NULL;
}
//_______________________________________________________________________
void GroupOfHists::CRButtonPressed(Int_t widgetId, Int_t buttonId, TObject *obj)
{
//   cout << "GroupOfHists::CRButtonPressed " << widgetId << " " << buttonId << endl;
   if (fMarginX > 0.2) {
      cout << "X Margin > 0.2, setting to 0.01" << endl;
      fMarginX = 0.01;
   }
   if (fMarginY > 0.2) {
      cout << "Y Margin > 0.2, setting to 0.01" << endl;
      fMarginY = 0.01;
   }
   SaveDefaults();
}

