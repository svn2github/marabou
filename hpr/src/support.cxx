#include "TObject.h"
#include "TButton.h"
#include "TCanvas.h"
#include "HTCanvas.h"
#include "TFrame.h"
#include "TLatex.h"
#include "TText.h"
#include "TH1.h"
#include "TF1.h"
#include "TRegexp.h"
#include "TKey.h"
#include "TMapFile.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TGMsgBox.h"
#include "TRootHelpDialog.h"
#include "TVirtualPad.h"
#include "TString.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TList.h"
#include "TRandom.h"

#include "CmdListEntry.h"
#include "FitHist.h"
// #include "Table.h"
#include "SetColor.h"

#include "support.h"
#include "TGMrbInputDialog.h"

#include <iostream>
#include <sstream>
#include <fstream>

TSocket * gSocket = 0;
//extern HistPresent *hp;
//----------------------------------------------------------------------- 
//  a command button

int GetBPars(const char *cmd, TObject * cc, int xpos, int ypos, float dx,
             float dy, char *tmp, float *x0, float *x1, float *y0,
             float *y1)
{
   if (xpos < 1 || xpos > 8 || ypos < 1 || ypos > 8) {
      printf("Illegal button position: %d %d\n", xpos, ypos);
      return 0;
   }
   strcpy(tmp, cc->GetName());
   strcat(tmp, "->");
   strcat(tmp, cmd);
   strcat(tmp, "();");

   *x0 = (float) ((xpos - 1) * dx);
   *x1 = *x0 + 0.97 * dx;
   *y0 = (float) (1 - ypos * dy);
   *y1 = *y0 + dy;
   return 1;
};

//----------------------------------------------------------------------- 
//  a command button

TButton *CButton(const char *cmd, const char *title, TObject * cc,
                 int xpos, int ypos, float dy, float dx)
{
   char tmp[50];
   float x0, x1, y0, y1;
   if (GetBPars(cmd, cc, xpos, ypos, dx, dy, tmp, &x0, &x1, &y0, &y1)) {
      TButton *b = new TButton((char *) title, tmp, x0, y0, x1, y1);
      b->SetTextFont(60);
      b->Draw();
      return b;
   } else
      return 0;
};

//----------------------------------------------------------------------- 
//  a toggle button

TButton *SButton(const char *cmd, const char *title, TObject * cc,
                 int xpos, int ypos, float dy, int onoff, float dx)
{
   char tmp[50];
   float x0, x1, y0, y1;
   if (GetBPars(cmd, cc, xpos, ypos, dx, dy, tmp, &x0, &x1, &y0, &y1)) {
      int col = 2;
      if (onoff)
         col = 3;
      TButton *b = new TButton((char *) title, tmp, x0, y0, x1, y1);
      b->SetFillColor(col);
      b->Draw();
      return b;
   } else
      return 0;
};

//----------------------------------------------------------------------- 
//  a help button

void HButton(const char *cmd, TObject * cc, int xpos, int ypos,
             float dy, float dx)
{
   char tmp[50];
   float x0, x1, y0, y1;
   if (GetBPars(cmd, cc, xpos, ypos, dx, dy, tmp, &x0, &x1, &y0, &y1)) {
      x0 += dx;
      x1 = x0 + dx * 0.25;
      TButton *b = new TButton((char *) "?", tmp, x0, y0, x1, y1);
      b->Draw();
   }
};

//----------------------------------------------------------------------- 

TLatex *GetPadLatex(TButton * pad)
{
   TIter next(pad->GetListOfPrimitives());
   while (TObject * obj = next()) {
      const char *name = obj->ClassName();
      if (!strncmp(name, "TLatex", 5))
         return (TLatex *) obj;
   }
   return NULL;
}

//----------------------------------------------------------------------- 
Bool_t is2dim(TH1 * hist)
{
   return hist->InheritsFrom("TH2");
};

//----------------------------------------------------------------------- 

Bool_t is3dim(TH1 * hist)
{
   return hist->InheritsFrom("TH3");
};
//____________________________________________________________________________

Bool_t is_a_list(TObject * obj)
{
   return obj->InheritsFrom("TList");
}

//____________________________________________________________________________

Bool_t is_a_function(TObject * obj)
{
   return obj->InheritsFrom("TF1");
}

//____________________________________________________________________________

Bool_t is_a_hist(TObject * obj)
{
   return obj->InheritsFrom("TH1");
}

//------------------------------------------------------ 
Bool_t is_a_window(TObject * obj)
{
   return (!strncmp(obj->ClassName(), "TMrbWindow", 10));
};

//------------------------------------------------------ 
Bool_t is_a_cut(TObject * obj)
{
   return (!strncmp(obj->ClassName(), "TCutG", 5));
};
Bool_t is_a_tree(TObject * obj)
{
   return (!strncmp(obj->ClassName(), "TTree", 5));
};

//------------------------------------------------------ 

Bool_t is_memory(const char *name)
{
   return (!strcmp(name, "Memory"));
}

//------------------------------------------------------ 

Bool_t is_a_file(const char *name)
{
   if (strstr(name, ".root") != 0)
      return kTRUE;
   else
      return kFALSE;
}

//------------------------------------------------------ 

Int_t CheckList(TList * list, const char *name)
{
   if (!list)
      return 0;
   Int_t entries, old_entries = -10;
   TObject *obj;
   Int_t lname = strlen(name);
 loop:
   entries = list->GetSize();
   if (entries == old_entries) {
      cout << "Infinite loop: " << entries << endl;
//      list->Print();
      return entries;
   }
   if (entries > 0) {
      old_entries = entries;
//      cout << "CheckList: entries: " << entries << endl;
      for (Int_t i = 0; i < entries; i++) {
         obj = list->At(i);
//         cout    << "at[i] " << i << " obj: " <<  obj << endl;
         if (!obj->TestBit(TObject::kNotDeleted) ||
             obj->TestBit(0xf0000000)) {
            cout << " remove deleted " << obj << " name: " << name <<
                " at " << i << " in " << list << endl;
//            obj->Dump();
            list->RemoveAt(i);
           
            goto loop;
         } else {
            if (lname > 0) {
               if (strncmp(obj->ClassName(), name, lname)) {
                  cout << "remove wrong Class " << obj->
                      ClassName() << endl;
                  list->RemoveAt(i);
                  goto loop;
               }
            }
         }
      }
   }
   return entries;
}

//------------------------------------------------------ 

TButton *CommandButton(TString & cmd, TString & tit,
                       Float_t x0, Float_t y, Float_t x1, Float_t y1,
                       Bool_t selected)
{
   TButton *button = new TButton((const char *) tit, "", x0, y, x1, y1);
   TRegexp brace(")");
   TString newcmd(cmd);
   TString buf;
   if (cmd.Contains("\""))
      buf = ",";
   buf += Form("\"%x\")", button);
   newcmd(brace) = buf.Data();
//   cout << "CommandButton: " << newcmd << endl;
   button->SetBit(kCommand);
   if (selected) {
      button->SetFillColor(3);
      button->SetBit(kSelected);
   } else {
      button->SetFillColor(18);
      button->ResetBit(kSelected);
   }
   button->SetMethod((char *) newcmd.Data());
   button->SetTextAlign(12);
//   button->SetTextFont(101);
   button->SetTextFont(100);
   button->SetTextSize(0.8);
//   if(selected)button->SetFillColor(3);
//   if(gROOT->GetVersionInt() <= 22308){
//      TText *text = GetPadText(button);
//      if(text)text->SetX(0.03);
   //  } else {
   TLatex *text = GetPadLatex(button);
   if (text)
      text->SetX(0.03);
//   }
   button->Draw();
   return button;
}

//________________________________________________________________________________

TString *GetTitleString(TObject * obj)
{
   TString *btitle = new TString(obj->GetName());
   *btitle = *btitle + " C: ";
   if (is_a_hist(obj)) {
      char ccont[9];
      TH1 *hist = (TH1 *) obj;
      sprintf(ccont, "%6.0f", hist->GetSumOfWeights());
      *btitle = *btitle + ccont;
   }
   *btitle = *btitle + " T: " + obj->GetTitle();

   if (!strncmp(obj->ClassName(), "TCutG", 5)) {
      btitle->Prepend("Cut:");
   }
   return btitle;
}

//________________________________________________________________________________

void SelectButton(TString & cmd,
                  Float_t x0, Float_t y, Float_t x1, Float_t y1,
                  Bool_t selected)
{
//   cout << ctitle << endl;
   TButton *button = new TButton("", "", x0, y, x1, y1);
   TRegexp brace(")");
   TString newcmd(cmd);

   TString buf;
   if (cmd.Contains("\""))
      buf = ",";
   buf += Form("\"%x\")", button);
   newcmd(brace) = buf.Data();

   button->SetMethod((char *) newcmd.Data());
   button->SetBit(kSelection);
//   cout << "SelectButton: " << newcmd << endl;
   if (selected) {
      button->SetFillColor(3);
      button->SetBit(kSelected);
   } else {
      button->SetFillColor(15);
      button->ResetBit(kSelected);
   }
   button->Draw();
}

//______________________________________________________________________________________

HTCanvas *CommandPanel(const char *fname, TList * fcmdline,
                       Int_t xpos, Int_t ypos, HistPresent * hpr, Int_t xwid)
{   
   Int_t xwid_default = 250;
   Int_t ywid_default = 600;

   Int_t xw = xwid_default, yw = ywid_default;
   if (xwid > 0) xwid_default = xwid;
   Int_t Nentries = fcmdline->GetSize();
   Int_t maxlen_tit = 0;
   Int_t maxlen_nam = 0;
   Bool_t anysel = kFALSE;
   for (Int_t i = 0; i < Nentries; i++) {
      CmdListEntry *cle = (CmdListEntry *) fcmdline->At(i);
      if (cle->fNam.Length() > maxlen_nam)
         maxlen_nam = cle->fNam.Length();
      if (cle->fTit.Length() > maxlen_tit)
         maxlen_tit = cle->fTit.Length();
      if (cle->fSel.Length() > 0)
         anysel = kTRUE;
//      if(cle->fSel != "NoOp") anysel=kTRUE;
   }
   if (maxlen_nam > 20)
      xw = 50 + maxlen_nam * 11;
   if (Nentries < 25)
      yw = 24 * (Nentries + 1);
   TString pname(fname);
   pname.Prepend("CP_");
   HTCanvas *cHCont = new HTCanvas(pname.Data(), pname.Data(),
                                   -xpos, ypos, xw, yw, hpr, 0);
   ypos += 50;
   if (ypos > 500)
      ypos = 5;
//    Float_t expandx = (Float_t)(maxlen_tit + maxlen_nam + 1)/19.;
//    Float_t expandx = (Float_t)(maxlen_nam + 1)/22.;
//   if(expandx < 1.) expandx=1.;
   Float_t expandx = xw / 250.;
   if(expandx < 1.) expandx=1.;
   Float_t expandy = (Float_t) Nentries / 25;

   Float_t x1, y0, y, dy;
   Float_t xcmd_with_sel = 0.08 / expandx;
   Float_t xcmd_no_sel   = 0.01 / expandx;
//   if (anysel);
//      x0 = 0.08;
//   else
//      x0 = 0.01;
//   x0 = x0 / expandx;
//   x1 = 1.;
   x1 = 0.99;

   if (Nentries <= 3) dy = .99 / (Float_t) (Nentries + 1);
   else               dy = .99 / (Float_t) (Nentries);

//   cout << "Nentries, dy " << Nentries << " " <<  dy << endl;
   y0 = 1.;
   y = y0 - dy;
   TString sel;
   TString title;
   for (Int_t i = 0; i < Nentries; i++) {
      Float_t xcmd = xcmd_no_sel;
      CmdListEntry *cle = (CmdListEntry *) fcmdline->At(i);
//      cout << "new: " << cle->fCmd << endl;
      if (anysel) {
         sel = cle->fSel;
         if (sel != "NoOp" && sel.Length() > 0) {
            Bool_t selected;
            if (sel.Index(" YES") > 0) {
               selected = kTRUE;
               sel.Remove(sel.Index(" YES"), sel.Length());
            } else {
               selected = kFALSE;
            }
            xcmd = xcmd_with_sel;
            SelectButton(sel, xcmd_no_sel, y, xcmd_with_sel, y + dy, selected);
         } else {
            xcmd = xcmd_no_sel;
         }
      }
      title = cle->fNam;
      title.Resize(maxlen_nam + 1);
//      title += cle->fTit;

      CommandButton(cle->fCmd, title, xcmd, y, x1, y + dy, kFALSE);
//      if((cle->fTit).Length()>0)b->SetToolTipText((const char *)cle->fTit,500);

//      b->SetName((const char *)cle->fNam); 
      y -= dy;
   }
   Int_t usedxw = cHCont->GetWw();
   Int_t usedyw = cHCont->GetWh();
//   Int_t newxw = (Int_t) ((Float_t) usedxw * expandx);
   Int_t newxw = usedxw;
   Int_t newyw;
   if (Nentries > 25)
      newyw = (Int_t) ((Float_t) usedyw * expandy);
   else
      newyw = usedyw;
//   cHCont->SetCanvas(newxw, newyw);
//   if (newyw > usedyw)
//      newyw -= 16;
   if (xwid < 0) {
      newxw = TMath::Max(xwid_default, newxw);
      if (newxw > xwid_default) newxw += 20;
   } else {
      newxw = xwid_default;
   } 
   cHCont->SetWindowSize(newxw, TMath::Min(ywid_default+10, newyw+20));
   cHCont->SetCanvasSize(usedxw, newyw);
   cHCont->SetEditable(kFALSE);
   cHCont->Update();
   return cHCont;
};

//______________________________________________________________________________________

int GetPosition(TVirtualPad * pad, UInt_t * xp, UInt_t * yp)
{
   *xp = 1000;
   *yp = 650;
   return 1;
};

//--------------------------------------------------
Int_t XBinNumber(TH1 * hist, Axis_t xval)
{
   TAxis *xaxis = hist->GetXaxis();
   Axis_t xmin = xaxis->GetXmin();
   Axis_t xmax = xaxis->GetXmax();
   Int_t nxbins = hist->GetNbinsX();
   Axis_t xbinwidth = (xmax - xmin) / nxbins;
   Int_t binlx = (Int_t) ((xval - xmin) / xbinwidth + 1.);
   if (binlx < 1)
      binlx = 1;
   if (binlx > nxbins)
      binlx = nxbins;
   return binlx;
}

//--------------------------------------------------
Int_t YBinNumber(TH1 * hist, Axis_t yval)
{
   TAxis *yaxis = hist->GetYaxis();
   Axis_t ymin = yaxis->GetXmin();
   Axis_t ymax = yaxis->GetXmax();
   Int_t nybins = hist->GetNbinsY();
   Axis_t ybinwidth = (ymax - ymin) / nybins;
   Int_t binly = (Int_t) ((yval - ymin) / ybinwidth + 1.);
   if (binly < 1)
      binly = 1;
   if (binly > nybins)
      binly = nybins;
   return binly;
}

//__________________________________________________________________

Stat_t Content(TH1 * hist, Axis_t xlow, Axis_t xup,
               Stat_t * mean, Stat_t * sigma)
{
   if (!hist) {
      cout << "Content: NULL pointer to histogram" << endl;
      return -1;
   }
   Double_t x = 0, cont = 0;
   Double_t sum = 0;
   Double_t sumx = 0;
   Double_t sumx2 = 0;
   Double_t sigi = 0, mi = 0;
   for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
      x = hist->GetBinCenter(i);
      if (x >= xlow && x < xup) {
         cont = hist->GetBinContent(i);
         sum += cont;
         sumx += x * cont;
         sumx2 += x * x * cont;
      }
   }
   if (sum > 0.) {
      mi = sumx / sum;
      sigi = sqrt(sumx2 / sum - mi * mi);
   }
   *mean = mi;
   *sigma = sigi;
   return sum;
}

//__________________________________________________________________

void WarnBox(const char *message, TGWindow * win)
{
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), win,
                "Warning", message,
                kMBIconExclamation, kMBDismiss, &retval);
}

//__________________________________________________________________

Bool_t QuestionBox(const char *message, TGWindow * win)
{
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), win,
                "Question", message,
                kMBIconQuestion, kMBYes | kMBNo, &retval);
   if (retval == kMBNo)
      return kFALSE;
   else
      return kTRUE;
}
//------------------------------------------------------   
void CloseWorkFile(TGWindow * win)
{
   OpenWorkFile(win);
//   if(fWorkfile){
//      fWorkfile->Close();
//      fWorkfile = NULL;
//  }
   gDirectory = gROOT;
}

//------------------------------------------------------   
Bool_t OpenWorkFile(TGWindow * win)
{
   static const char *fWorkname = 0;
   static TFile *fWorkfile = 0;
   static const char *cfname;
   if (fWorkfile) {
//      WarnBox("Close open workfile");
      fWorkfile->Close();
      fWorkfile = NULL;
      return kFALSE;
   }
   static TString fname = "workfile.root";
   TEnv env(".rootrc");         // inspect ROOT's environment
   if (!fWorkname) {
      cfname = env.GetValue("HistPresent.WorkFileName", fname.Data());
      fname = cfname;
//      cout << " GetValue " << cfname << endl;
   }
//   else {
//      cfname = fWorkname;
//   }
//   cout << " defval " << cfname << endl;
   Bool_t ok;
   fWorkname = GetString("Use File:", fname.Data(), &ok, win);
   if (!ok)
      return kFALSE;
   fname = fWorkname;
   if (!gSystem->AccessPathName(fWorkname, kFileExists))
      fWorkfile = new TFile(fWorkname, "UPDATE", "Output file");
   else
      fWorkfile = new TFile(fWorkname, "RECREATE", "Output file");
   return kTRUE;
}

//------------------------------------------------------   
Int_t GetUsedSize(TMapFile * mfile)
{
   if (!mfile) {
      cout << "GetUsedSize: NULL pointer" << endl;
      return -1;
   }
   TMapRec *mr = mfile->GetFirst();
   if (!mr) {
      cout << "GetUsedSize: no records" << endl;
      return -2;
   }
   Int_t size = 0;
   while (mfile->OrgAddress(mr)) {
      if (!mr)
         break;
      size += mr->GetBufSize();
      mr = mr->GetNext();
   }
   return size;
}
//------------------------------------------------------   
Int_t GetObjects(TList & list, TFile * rfile, const char * classname)
{
   Int_t maxkey = 0;
   if (!rfile)
      return maxkey;
   TIter next(rfile->GetListOfKeys());
//   TList *list = 0;
   TKey *key;
   TString cn;
   while ( (key = (TKey *) next()) ) {
      cn = key->GetClassName();
//      cout << "GetObjects: " << cn << endl;
      if (cn.BeginsWith(classname)) {
         TString kn(key->GetName());
         kn += ";";
         kn += key->GetCycle();
         maxkey = TMath::Max(maxkey, (Int_t)key->GetCycle());
         list.Add(new TObjString(kn.Data()));
      }
   }
   return maxkey;
}

//------------------------------------------------------   
Int_t contains_filenames(const char *lname)
{
   if (gSystem->AccessPathName(lname)) {
      cout << "Cant find: " << lname << endl;
      return -1;
   }
   ifstream wstream;
   wstream.open(lname, ios::in);
   if (!wstream.good()) {
      cout << "Error in open file: "
          << gSystem->GetError() << " - " << lname << endl;
      return -1;
   }
   TString line;
   Int_t ok = 0;
   while (1) {
      line.ReadLine(wstream, kFALSE);
      if (wstream.eof())
         break;
      line.Strip();
      if (line.Length() <= 0)
         continue;
      if (line[0] == '#')
         continue;
      Int_t pp = line.Index(" ");
      if (pp <= 0)
         break;
      line.Resize(pp);
      if (line.Contains(".root") || line.Contains(".map"))
         ok = 1;
      break;
   }
   wstream.close();
   return ok;
}

//_______________________________________________________________________________________

TH1 *GetTheHist(TVirtualPad * pad)
{
   TList *l = pad->GetListOfPrimitives();
   TIter next(l);
   TObject *o;
   while ( (o = next()) ) {
      if (is_a_hist(o))
         return (TH1 *) o;
   }
   return NULL;
}

//_______________________________________________________________________________________

void ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, Int_t mode, TGWindow * win)
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

//_______________________________________________________________________________________

void CalibrateAllAsSelected(TVirtualPad * pad, TCanvas * canvas,
                            Int_t mode)
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
          ("Selected pad contains no hist,\n please select with middle mouse");
      return;
   }
   HistPresent *hp = (HistPresent *) gROOT->FindObject("mypres");
   if (!hp) {
      WarnBox("Cant locate HistPresent object");
      return;
   }
   TEnv env(".rootrc");         // inspect ROOT's environment
   TString defprefix;
   TString refname;
   defprefix =
       env.GetValue("HistPresent.LastSettingsName", defprefix.Data());
   if (defprefix.Length() > 0) {
      defprefix += "_";
      refname = defprefix;
      refname += href->GetName();
      Int_t last_us = refname.Last('_');	// chop off us added by GetSelHistAt
      refname.Remove(last_us);
      refname += ".def";
      cout << refname << endl;
      if (gSystem->AccessPathName(refname.Data())) {
         TString question = refname;
         question += " does not exist";
         WarnBox(question.Data());
         return;
      }
   } else {
      cout << setred << "Cant locate file with default settings" << endl;
      cout << "check your .rootrc for HistPresent.LastSettingsName"
          << setblack << endl;
      WarnBox("Cant locate file with default settings");
      return;
   }
   TString copycmd;
   l = canvas->GetListOfPrimitives();
   TIter next(l);
   while (( obj = next()) ) {
      if (obj->InheritsFrom(TPad::Class())) {
         TPad *p = (TPad *) obj;
         TH1 *hist = GetTheHist(p);
         if (hist && hist != href) {
            copycmd = "cp -v ";
            copycmd += refname;
            copycmd += " ";
            copycmd += defprefix;
            copycmd += hist->GetName();
            Int_t last_us = copycmd.Last('_');	// chop off us added by GetSelHistAt
            copycmd.Remove(last_us);
            copycmd += ".def";
            cout << copycmd << endl;
            gSystem->Exec(copycmd.Data());
//            cout << hist->GetName()<< ", first, last, min, max  " 
         }
      }
   }
}

//_______________________________________________________________________________________

void RebinAll(TVirtualPad * pad, TCanvas * canvas, Int_t mode)
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

//_______________________________________________________________________________________


void Canvas2LP(TCanvas * ca, const Char_t * opt, TGWindow * win,
               Bool_t autops)
{
   const char helpText_PS[] =
"Write picture to PostScript file, With \"white bg\"\n\
the background is forced to be white. This is\n\
recommended to save toner of printers. Use\n\
command with \"as it is\" to preserve background\n\
colors. If option \"Reduce size\" is activated\n\
the picture size is halfed";

   const char helpText_LP[] =
"Send picture directly to Printer, With \"white bg\"\n\
the background is forced to be white. This is\n\
recommended to save toner of printers. Use\n\
command with \"as it is\" to preserve background\n\
colors. If option \"Reduce size\" is activated\n\
the picture size is halfed";

   if (!ca) return;
   Int_t *padfs = 0;
   Int_t *padframefs = 0;
   Int_t cafs = 0, caframefs = 0;
   Int_t npads = 0;
   const Int_t maxpads = 64;
   TString option = opt;
   if (option.Contains("plain")) {
      TList *l = ca->GetListOfPrimitives();
      TIter next(l);
      TObject *obj;
      padfs = new Int_t[maxpads];
      padframefs = new Int_t[maxpads];
      while (( obj = next()) ) {
         if (obj->InheritsFrom(TPad::Class())) {
            TPad *p = (TPad *) obj;
            if (npads < maxpads) {
               padfs[npads] = p->GetFillStyle();
               padframefs[npads] = p->GetFrame()->GetFillStyle();
               npads++;
            }
            p->SetFillStyle(0);
            p->GetFrame()->SetFillStyle(0);
         }
      }
      cafs = ca->GetFillStyle();
      caframefs = ca->GetFrame()->GetFillStyle();
      ca->SetFillStyle(0);
      ca->GetFrame()->SetFillStyle(0);
      ca->SetBorderMode(0);
   }
   TEnv env(".rootrc");         // inspect ROOT's environment
   Int_t xpaper = env.GetValue("HistPresent.PaperSizeX", 20);
   Int_t ypaper = env.GetValue("HistPresent.PaperSizeX", 26);
//   gStyle->SetPaperSize(19.,28.);
   gStyle->SetPaperSize((Float_t) xpaper, (Float_t) ypaper);
   static Bool_t reduce = kFALSE;
   if (option.Contains("ps")) {
      TString hname = ca->GetName();
      TString title = "Write PS-file with Name";
      if (option.Contains("plain"))
         title += " (white backgound)";
      else
         title += " (as it is)";
      hname += ".ps";
      Bool_t ok;
      hname = GetString(title.Data(), hname.Data(), &ok, win,
                        "Reduced Size", &reduce, helpText_PS);
      if (!ok) {
         cout << "Printing cancelled " << endl;
         return;
      }
      if (reduce)
         gStyle->SetPaperSize((Float_t) xpaper * 0.5,
                              (Float_t) ypaper * 0.5);
      ca->SaveAs(hname.Data());

      if (autops) {
         TString cmd("gv ");
         cmd += hname.Data();
         cmd += "&";
         gSystem->Exec(cmd.Data());
      }
   } else {
      TString cmd = "lpr ";
      const char *pc = gSystem->Getenv("PRINTER");
      if (pc) {
         cmd += "-P";
         cmd += pc;
      }
//                                                                                                                                                                                                                                                                TEnv env(".rootrc");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       // inspect ROOT's environment
      const char *ccmd =
          env.GetValue("HistPresent.PrintCommand", cmd.Data());
      cmd = ccmd;
      Bool_t ok;
      cmd = GetString("Printer command: ", cmd.Data(), &ok, win,
                      "Reduced Size", &reduce, helpText_LP);
      if (!ok) {
         cout << "Printing cancelled " << endl;
         return;
      }
      if (reduce)
         gStyle->SetPaperSize((Float_t) xpaper * 0.5,
                              (Float_t) ypaper * 0.5);
      ca->SaveAs("temp_pict.ps");
      TString prcmd = cmd;
      prcmd += " ";
      prcmd += "temp_pict.ps";
      gSystem->Exec(prcmd.Data());
      if (cmd.Contains("lpr ")) {
         cmd(2, 1) = "q";
         gSystem->Exec(cmd.Data());
      }
   }
   if (option.Contains("plain")) {
      TList *l = ca->GetListOfPrimitives();
      TIter next(l);
      TObject *obj;
      while ( (obj = next()) ) {
         if (obj->InheritsFrom(TPad::Class())) {
            TPad *p = (TPad *) obj;
            if (npads < maxpads) {
               p->SetFillStyle(padfs[npads]);

               p->GetFrame()->SetFillStyle(padframefs[npads]);
               npads++;
            }
         }
      }
      ca->SetFillStyle(cafs);
      ca->GetFrame()->SetFillStyle(caframefs);
      ca->Modified(kTRUE);
      ca->Update();
      delete[]padfs;
      delete[]padframefs;
   }
}

//_______________________________________________________________________________________
void Canvas2RootFile(TCanvas * canvas, TGWindow * win)
{
   TString hname = canvas->GetName();
   hname += ".canvas";
   Bool_t ok;
   TObject *obj;
   hname = GetString("Save canvas with name", hname.Data(), &ok, win);
   if (!ok)
      return;
   if ((obj = gROOT->FindObject(hname.Data()))) {
      WarnBox("Object with this name already exists", win);
      cout << setred << "Object with this name already exists: "
          << obj->ClassName() << setblack << endl;
      return;
   }
   if (OpenWorkFile(win)) {
      if (canvas->GetAutoExec())
         canvas->ToggleAutoExec();
      TCanvas *nc =
          new TCanvas(hname.Data(), canvas->GetTitle(), 50, 500, 720, 500);
      TIter next(canvas->GetListOfPrimitives());
      while ((obj = next())) {
         gROOT->SetSelectedPad(nc);
         nc->GetListOfPrimitives()->Add(obj->Clone());
      }
      if (nc->GetAutoExec())
         nc->ToggleAutoExec();
      nc->Write();
      CloseWorkFile();
      delete nc;
   }
}

//_________________________________________________________________________________________

TEnv *GetDefaults(TString & hname, Bool_t mustexist)
{
   TEnv *lastset = 0;
   TString defname;
   TEnv env(".rootrc");         // inspect ROOT's environment
   defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
   if (defname.Length() > 0) {
      defname += "_";
      defname += hname;
      defname += ".def";
//      cout << "Look for : " << defname.Data() << endl;
      if (mustexist && gSystem->AccessPathName(defname.Data()))
         return 0;
//      cout << "Look for : " << defname.Data() << endl;
      lastset = new TEnv(defname.Data());	// inspect ROOT's environment
   }
   return lastset;
}

//___________________________________________________________________________________

TH1 *gethist(const char *hname, TSocket * sock)
{
   if (!sock) return 0;
   Int_t nobs;
   TH1 * hist = 0;
   TMessage *message;
   TString mess("M_client gethist ");
   mess += hname;
   sock->Send(mess);            // send message

   while ((nobs = sock->Recv(message)))	// receive next message
   {
      if (nobs <= 0)
         break;
//     cout << "nobs " << nobs << endl;
      if (message->What() == kMESS_STRING) {
         char *str0 = new char[nobs];
         message->ReadString(str0, nobs);
         cout << str0 << endl;
         delete str0;
         break;
      } else if (message->What() == kMESS_OBJECT) {

         hist = (TH1 *) message->ReadObject(message->GetClass());
         break;
      } else {
         cout << "Unknown message type" << endl;
      }
   }
   if (message) delete message;
   gDirectory = gROOT;
   return hist;
}

//_______________________________________________________________________________

TMrbStatistics *getstat(TSocket * sock)
{
   if (!sock) return 0;
   Int_t nobs;
   TMrbStatistics *stat = 0;
   TMessage *message;
   TString mess("M_client getstat ");

   sock->Send(mess);            // send message

   while ((nobs = sock->Recv(message)))	// receive next message
   {
      if (nobs <= 0)
         break;
//     cout << "nobs " << nobs << endl;
      if (message->What() == kMESS_STRING) {
         char *str0 = new char[nobs];
         message->ReadString(str0, nobs);
         cout << str0 << endl;
         delete str0;
         break;
      } else if (message->What() == kMESS_OBJECT) {
         stat =
             (TMrbStatistics *) message->ReadObject(message->GetClass());
         break;
      } else {
         cout << "Unknown message type" << endl;
      }
   }
   if (message) delete message;
   gDirectory = gROOT;
   return stat;
}
//___________________________________________________________________________

TColor * GetColorByInd(Int_t index) {   
   TIter next(gROOT->GetListOfColors());
   while (TColor * c = (TColor *)next()) {
      if (c->GetNumber() == index) {
         return c;
      }
   }
   return NULL;
}
//___________________________________________________________________________

void SetUserPalette(Int_t startindex, TArrayI * pixels) 
{
   Int_t ncont = pixels->GetSize();
   if (ncont <= 0) return;
	TColor * c;
   
	TArrayI colind(ncont);
	Float_t r=0, g=0, b=0;
	for (Int_t i = 0; i < ncont; i++) {
   	colind[i] = i + startindex;
   	c = GetColorByInd(i + startindex); 
   	if (c) delete c;
   	c = new TColor(i + startindex, r, g, b);
   	c->Pixel2RGB((*pixels)[i], r, g, b);
   	c->SetRGB(r, g, b);
   	gStyle->SetPalette(ncont, colind.GetArray());  
	}
}
//___________________________________________________________________________
void AdjustMaximum(TH1 * h2, TArrayD * xyvals)
{
   Int_t nv = xyvals->GetSize();
   if (nv <= 0) return;
   Double_t highest_cont = (*xyvals)[0];
   for (Int_t i = 0; i < nv; i++) {
      if ((*xyvals)[i] > highest_cont) highest_cont = (*xyvals)[i] ;
   }
   if (highest_cont > h2->GetMaximum()) {
      h2->SetMaximum(highest_cont);
      cout << "SetMaximum to highest contour level: " << highest_cont << endl; 
   }     
}
//___________________________________________________________________________

Int_t DeleteOnFile(const char * fname, TList* list, TGWindow * win)
{
   Int_t ndeleted = 0;
   TString name;
   TIter next(list);
   TObjString * sel;
   while ( (sel = (TObjString*)next()) ) {
   	name = sel->GetString();
   	if (name.BeginsWith(fname)) {
      	Int_t start  = name.Index(" ") + 1;
      	Int_t length = name.Length() - start;
      	name = name(start,length);
      	TString question("Delete: ");
      	question += name.Data();
      	question += " from ";
      	question += fname;
      	if (QuestionBox(question.Data(), win) == kMBYes) {      
         	TFile * f = new TFile(fname, "update");
         	if (f->Get(name.Data())) {
               list->Remove(sel);
            	f->Delete(name.Data());
            	ndeleted++;
         	} else {
            	cout << name.Data() << " not found  on " << fname << endl;
         	}
         	f->Close();
      	}
   	}
   }
   return ndeleted;
}
//___________________________________________________________________________

TH1 * calhist(TH1 * hist, TF1 * calfunc,
              Int_t  nbin_cal, Axis_t low_cal, Axis_t binw_cal,
              const char * origname)
{
   TString hname_cal;
   if (origname) hname_cal = origname;
   else          hname_cal = hist->GetName();
   hname_cal += "_cal";
   TString title_cal(hist->GetTitle());
   title_cal += "_calibrated";
   title_cal += ";Energy[KeV];Events[";
   title_cal += Form("%4.2f", binw_cal);
   title_cal += " KeV]";
   TH1 * hist_cal;
   if      (!strcmp(hist->ClassName(), "TH1F"))
   	hist_cal = new TH1F(hname_cal, title_cal,nbin_cal, 
                     low_cal, low_cal + binw_cal * nbin_cal);
   else if (!strcmp(hist->ClassName(), "TH1D"))
   	hist_cal = new TH1D(hname_cal, title_cal,nbin_cal, 
                     low_cal, low_cal + binw_cal * nbin_cal);
   else if (!strcmp(hist->ClassName(), "TH1S"))
   	hist_cal = new TH1S(hname_cal, title_cal,nbin_cal, 
                     low_cal, low_cal + binw_cal * nbin_cal);
   else
   	hist_cal = new TH1C(hname_cal, title_cal,nbin_cal, 
                     low_cal, low_cal + binw_cal * nbin_cal);

//   under - overflows of origin hist are taken as they are
   hist_cal->SetBinContent(0, hist->GetBinContent(0));
   hist_cal->SetBinContent(hist_cal->GetNbinsX()+1, 
                           hist->GetBinContent(hist->GetNbinsX()+1));
//  update number of entries
   hist_cal->SetEntries(hist->GetBinContent(0) + 
                        hist->GetBinContent(hist->GetNbinsX()+1));
// shuffle bins
   for (Int_t bin = 1; bin <= hist->GetNbinsX(); bin++) {
      Axis_t binw = hist->GetBinWidth(bin);
      Axis_t bcent = hist->GetBinCenter(bin);
      for (Int_t cnt = 0; cnt < hist->GetBinContent(bin); cnt++) {
         Axis_t bcent_r = bcent + binw  * (gRandom->Rndm() - 0.5);
         Axis_t bcent_cal = calfunc->Eval(bcent_r);
         hist_cal->Fill(bcent_cal);
      }
   }
   return hist_cal;
}
//_______________________________________________________________________________________

void PrintGraph(TGraphErrors * gr) 
{

	cout << endl << "Graph Object, Npoints: " << gr->GetN()<< endl;
   gr->Print();
/*
	cout << "           X" << "    Error(X)" 
   	  << "           Y" << "    Error(Y)" << endl;
	for (Int_t i = 0; i < gr->GetN(); i++) {
   	cout << setw(12) << (gr->GetX())[i] << setw(12) << (gr->GetEX())[i]
      	  << setw(12) << (gr->GetY())[i] << setw(12) << (gr->GetEY())[i]
      	  << endl;
	}
*/
	TIter next(gr->GetListOfFunctions());
	while (TObject * obj = next()) {
   	if (obj->InheritsFrom("TF1")) {
      	TF1 * f = (TF1*)obj;
      	cout << endl << "Fitted function name: " << f->GetName()
         	  << " Type: " << f->GetTitle() 
         	  << " Chi2/NDF: " << f->GetChisquare() 
         	  << "/" << f->GetNDF() << endl << endl;

      	for (Int_t i = 0; i < f->GetNpar(); i++) {
         	cout << "Par_" << i << setw(12) << f->GetParameter(i)
            	  << " +- " << setw(12) << f->GetParError(i)  << endl;
      	}
      	cout << endl;
   	}
	}
}
//_______________________________________________________________________________________

Bool_t IsInsideFrame(TCanvas * c, Int_t px, Int_t py)
{
//   Bool_t inside = kFALSE;
   TIter next(c->GetListOfPrimitives());
   TObject * obj =0;
   Axis_t x = gPad->AbsPixeltoX(px);
   Axis_t y = gPad->AbsPixeltoY(py);
   while ((obj = next())) {
      if (obj->IsA() == TFrame::Class()) {
         TFrame * fr = (TFrame*)obj; 
         if (x < fr->GetX1())return kFALSE;
         if (x > fr->GetX2())return kFALSE;
         if (y < fr->GetY1())return kFALSE;
         if (y > fr->GetY2())return kFALSE;
         return kTRUE;
      }
   }
   return kFALSE;
}
//_______________________________________________________________________________________

TGraph * FindGraph(TCanvas * ca)
{
   if (!ca) return NULL;
   TIter next(ca->GetListOfPrimitives());
   while (TObject * obj = next()) {
      if (obj->InheritsFrom("TGraph")) { 
          TGraph * g = (TGraph*)obj;
          return g;
      }
   }
   return NULL;
};

//__________________________________________________________________

void Show_Fonts()
{
   const char text[] = "The big brown fox jumps over the lazy dog.";
   new TCanvas("ct", "Text fonts used by root", 100, 100, 600, 400);
   Float_t x0 = 0.05, y = 0.9, dy = 0.07;
   TText *t1;
   TText *t;
   for (Int_t i = 10; i <= 120; i += 10) {
      t1 = new TText(x0, y, Form("%d", i));
      t = new TText(x0 + 0.1, y, text);
      t1->SetTextSize(0.05);
      t1->SetTextFont(i);
      t->SetTextSize(0.05);
      t->SetTextFont(i);
      t1->Draw();
      t->Draw();
      y -= dy;
   }
};

//______________________________________________________________________________________
  
void DrawColors() 
{
//   TString hexcol;
   TString scol;
//   TString cmd;
   new TCanvas("colors", "rgb colors", 400, 20, 800, 400);
   Float_t dx = 1./10.2 , dy = 1./10.2 ;
   Float_t x0 = 0.1 * dx,  y0 =0.1 *  dy; 
   Float_t x = x0, y = y0;;
   TButton * b;
   TColor  * c;
   Int_t maxcolors = 100;
   Int_t basecolor = 1;
   Int_t colindex = basecolor;
//   Int_t palette = new Int_t[maxcolors];
   for (Int_t i=basecolor; i<= maxcolors; i++) {
      scol = Form("%d", colindex);
      b = new TButton(scol.Data(), scol.Data(), x, y, x + .9*dx , y + .9*dy );
      b->SetFillColor(colindex);
      b->SetTextAlign(22);
      b->SetTextFont(100);
      b->SetTextSize(0.8);
      c = GetColorByInd(colindex);
      if (c) {
         if ( c->GetRed() + c->GetBlue() + c->GetGreen() < 1.5 ) b->SetTextColor(0);
         else                   b->SetTextColor(1);
      } else {
         cout << "color not found " << colindex << endl;
      }
      if ( (colindex++ >= maxcolors + basecolor) ) {
         cout << "Too many colors " << maxcolors << endl;
         break;
      }
      b->Draw();
      y += dy;
      if ( y >= 1 - dy ){
         y = y0;
         x+= dx;
      }
   }
}
//______________________________________________________________________________________
  
void DrawFillStyles() 
{
   Int_t styles[30];
   styles[0] = 0; 
   styles[1] = 1001; 
   styles[2] = 2001; 
   styles[3] = 4000; 
   for (Int_t i=1; i<= 25; i++) styles[3 + i] = i +3000;

   TString scol;
//   TString cmd;
   new TCanvas("fillstyles", "fillstyles", 400, 20, 800, 400);
   Float_t dx = 1./7.2 , dy = 1./5.2 ;
   Float_t x0 = 0.1 * dx,  y0 = 1 - 1.1 *  dy; 
   Float_t x = x0, y = y0;
   TPaveText * b;
//   Int_t palette = new Int_t[maxcolors];

   for (Int_t i=0; i< 29 ; i++) {
      scol = Form("%d", styles[i]);
      if      (i==0) scol = Form("%d \n hollow", styles[i]);
      else if (i==1) scol += "\n solid";
   
      b = new TPaveText(x, y, x + .9*dx , y + .9*dy );
      b->AddText(Form("%d", styles[i]));
      if (i==0) {
         b->AddText("hollow");
         b->SetFillColor(0);
      } else if (i==1) {
          b->AddText("solid");
          b->SetFillColor(1);
          b->SetTextColor(10);
      } else if (i==2) {
          b->AddText("hatch");
          b->SetFillColor(1);
          b->SetTextColor(10);
      } else if (i==3) {
         b->SetFillColor(0);
         b->AddText("trans-");
         b->AddText("parent");
      } else {
          b->SetTextColor(1);
          b->SetFillColor(1);
      }
      b->SetFillStyle(styles[i]);
      b->SetTextAlign(22);
      b->SetTextFont(100);
      b->SetTextSize(0.06);
      b->Draw();
      x += dx;
      if ( x >= 1 - dx ){
         y -= dy;
         x = x0;
      }
   }
}
//______________________________________________________________________________________
  
void DrawLineStyles() 
{
   Int_t nstyles = 4;
   new TCanvas("linestyles", "linestyles", 400, 20, 500, 300);
   Float_t dy = 1./ (nstyles + 2) ;
   Float_t y = 1 - 1.1 *  dy; 
   Float_t ts = 0.1;
   TLine * l;
   TText * t;
   for (Int_t i = 1; i <= nstyles; i++) {
      t = new TText(0.1, y - 0.3 * ts , Form("%d", i));
      l = new TLine(0.15, y, 0.8, y);
      l->SetLineStyle(i);
      l->SetLineWidth(2);
      t->SetTextSize(ts);
      t->Draw();
      l->Draw();
      y -= dy;
   }
}
//______________________________________________________________________________________
  
Bool_t CreateDefaultsDir(TRootCanvas * mycanvas, Bool_t checkonly) 
{
   TString defname("defaults/Last");
   Bool_t fok = kFALSE;
   TEnv env(".rootrc");         // inspect ROOT's environment
   defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
//   if (defname.Length() <= 0) {
//      WarnBox("Setting defaults dir/name to defaults/Last");
//      defname = "defaults/Last";
//      env.SetValue("HistPresent.LastSettingsName", defname.Data());
//   }    
   env.SaveLevel(kEnvUser);
   fok = kTRUE;
   Int_t lslash = defname.Last('/');
   if (lslash > 0) {
      TString dirname = defname;
      dirname.Remove(lslash, 100);
      if (gSystem->AccessPathName(dirname.Data())) {
         if (checkonly) return kFALSE;
         fok = kFALSE;
         TString question = dirname;
         question += " does not exist, create it?";
         int buttons = kMBYes | kMBNo, retval = 0;
         EMsgBoxIcon icontype = kMBIconQuestion;
         new TGMsgBox(gClient->GetRoot(), mycanvas,
                      "Warning", (const char *) question,
                      icontype, buttons, &retval);
         if (retval == kMBYes) {
            if (gSystem->MakeDirectory((const char *) dirname) == 0) {
               fok = kTRUE;
            } else {
               dirname.Prepend("Error creating ");
               dirname.Append(": ");
               dirname.Append(gSystem->GetError());
               WarnBox(dirname.Data());
            }
         }
      } else {
         return kTRUE;
      }
   }
   return fok;
}





