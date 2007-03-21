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
#include "HistPresent.h"

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
   M_AllAsSelRangeOnly,
   M_ActivateTimer
};
//________________________________________________________________________

GroupOfHists::GroupOfHists(TList * hlist, HistPresent * hpr, const Char_t *title) 
            : fHistPresent(hpr) 
{
   cout << " ctor GroupOfHists::" << this << endl;
   fCanvas = NULL;
   fTimer = NULL;
   if (hpr) fAutoUpdateDelay = hpr->GetAutoUpdateDelay();
   else     fAutoUpdateDelay = 5;
   fCommonRotate = kFALSE;
   fAnyFromSocket = kFALSE;
   fShowAllAsFirst = kFALSE;
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

 	TString wwhx = "HistPresent.WindowXWidth_";
	wwhx += nx;
	wwhx += "x";
	wwhx += ny;
	TString wwhy = "HistPresent.WindowYWidth_";
	wwhy += nx;
	wwhy += "x";
	wwhy += ny;
   TEnv env(".rootrc");		// inspect ROOT's environment
	Int_t wwx   =   env.GetValue(wwhx.Data(), 0);
	if (wwx == 0) wwx   =   env.GetValue("HistPresent.WindowXWidth_many", 800);
	Int_t wwy   =   env.GetValue(wwhy.Data(), 0);
	if (wwy == 0) wwy   =   env.GetValue("HistPresent.WindowYWidth_many", 800);

   TString arrange("no"); 
   arrange =env.GetValue("HistPresent.WindowArrange_many",arrange.Data());
   if (arrange.Contains("top")) {nx = 1; ny = nsel;}
   if (arrange.Contains("side")) {nx =  nsel; ny = 1;}

   TString buf("cmany_");
   buf += fHistPresent->fSeqNumberMany++;
   const char * tit = buf.Data();
   if (title) tit = title;
   buf.Prepend("GroupOfHists_");
   this->SetName(buf.Data());
   this->SetTitle(tit);
   gROOT->GetList()->Add(this);
   gROOT->GetListOfCleanups()->Add(this);
   fCanvas =  new HTCanvas(buf.Data(), tit, 
              fHistPresent->fWincurx, fHistPresent->fWincury, wwx, wwy);
   fHistPresent->fWincurx = fHistPresent->fWintopx;
   fHistPresent->fWincury += fHistPresent->fWinshifty;

   fHistPresent->fCanvasList->Add(fCanvas);
   fCanvas->Divide(nx, ny, fHistPresent->fDivMarginX, fHistPresent->fDivMarginY);
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
//      cout << "Aft: "  << hname << endl;
      lastset = GetDefaults(hname);
      if (lastset) {
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
         hist->Draw(fHistPresent->fDrawOpt2Dim->Data());
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
         gStyle->SetOptTitle(fHistPresent->GetShowTitle());
         if (fHistPresent->fShowContour) drawopt = "hist";
         if (fHistPresent->fShowErrors)  drawopt += "e1";
         hist->SetLineColor(fHistPresent->fHistLineColor);
         if (fHistPresent->fFill1Dim) {
            hist->SetFillStyle(fHistPresent->fHistFillStyle);
            hist->SetFillColor(fHistPresent->fHistFillColor);
            cout << "fHistPresent->fHistFillStyle " << fHistPresent->fHistFillStyle << endl;
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
   gPad->Modified(kTRUE);
   gPad->Update();
   BuildMenu();
   if (firstpad) { 
      if (fShowAllAsFirst || (fHistPresent->fDivMarginX <= 0 && fHistPresent->fDivMarginY <= 0)) 
//         ShowAllAsSelected(firstpad, fCanvas, 0, NULL);
      firstpad->cd();
   }
   fCanvas->SetEditable(kTRUE);
}
//________________________________________________________________________

void GroupOfHists::RecursiveRemove(TObject *obj)
{
   if (obj == fCanvas) delete this;
}
//________________________________________________________________________

GroupOfHists::~GroupOfHists()
{
   if (fTimer) delete fTimer;
   gROOT->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
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
         hist->Draw(fHistPresent->fDrawOpt2Dim->Data());
         hist->GetXaxis()->SetRange(fx, lx);
         hist->GetYaxis()->SetRange(fy, ly);
      } else { 
         TString drawopt;
         if(fHistPresent->fShowContour)drawopt = "hist";
         if(fHistPresent->fShowErrors)drawopt += "e1";
         if(fHistPresent->fFill1Dim){
            hist->SetFillStyle(1001);
			hist->SetFillColor(44);
         } else hist->SetFillStyle(0);
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
   cout << "GroupOfHists::BuildMenu() " <<this << endl;
   fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
   TGMenuBar * menubar = fRootCanvas->GetMenuBar();
   TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
   fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Display", fMenu, layoh_left, menubar->GetPopup("Help"));
   fMenu->AddEntry("Show now all as selected, Range only", M_AllAsSelRangeOnly);
   fMenu->AddEntry("Show now all as selected, Range, Min, Max ",  M_AllAsSel);
   fMenu->AddEntry("Calibrate all as selected",  M_CalAllAsSel);
   fMenu->AddEntry("Show always all as First", M_AllAsFirst);
   if (fShowAllAsFirst) fMenu->CheckEntry(M_AllAsFirst);
   else                 fMenu->UnCheckEntry(M_AllAsFirst);
   fMenu->AddEntry("Rebin all",  M_RebinAll);
   fMenu->AddSeparator();
   fMenu->AddEntry("Activate automatic update",  M_ActivateTimer);
   fMenu->AddEntry("Activate simultanous rotation", M_CommonRotate);
   fMenu->Connect("Activated(Int_t)", "GroupOfHists", this,
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
      case M_CommonRotate:
         if (fCommonRotate) {
            fCommonRotate = kFALSE;
            fMenu->UnCheckEntry(M_CommonRotate);
         } else {
            fCommonRotate = kTRUE;
            fMenu->CheckEntry(M_CommonRotate);
         }
         break;
      case M_AllAsFirst:
         if (fShowAllAsFirst) {
            fShowAllAsFirst = kFALSE;
            fMenu->UnCheckEntry(M_AllAsFirst);
         } else {
            fShowAllAsFirst = kTRUE;
            fMenu->CheckEntry(M_AllAsFirst);
         }
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
 //           cout << "h->GetDrawOption() " << h->GetDrawOption()<< endl;
            if (!strncmp(h->GetDrawOption(), "lego", 4)) { 
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



