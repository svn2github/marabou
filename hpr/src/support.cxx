#include "TObject.h"
#include "TButton.h"
#include "TCanvas.h"
#include "HTCanvas.h"
#include "TFrame.h"
#include "TLatex.h"
#include "TText.h"
#include "TArc.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
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
#include "SetColor.h"

#include "support.h"
#include "TGMrbInputDialog.h"
#include "TMrbString.h"
#include "TGMrbValuesAndText.h"

#include <iostream>
#include <sstream>
#include <fstream>

TSocket * gSocket = 0;
//_______________________________________________________________________________
   
void AddObjString(Int_t val, TList * list, Int_t type)
{
   Int_t ival = val;
   TString s;
   if (type == kAttCheckB) {
      s+= "CheckButton_";
      if (val == 0) s += "Up";
      else          s += "Down";
   } else if (type == kAttRadioB) {
      s+= "RadioButton_";
      if (val == 0) s += "Up";
      else          s += "Down";
   } else {
      if (type == kAttColor) {
         s+= "ColorSelect_";        
         TColor * color =  GetColorByInd(ival);
         ival = 0;
         if (color) ival = color->GetPixel();
         else       cout << "Invalid color index: " << val << endl;
      }
      if (type == kAttAlign) s+= "AlignSelect_";
      if (type == kAttLineS) s+= "LineSSelect_";
      if (type == kAttFont ) { s+= "CfontSelect_"; ival /= 10; }
      if (type == kAttArrow) s+= "ArrowSelect_";
      if (type == kAttMarker)s+= "Mark_Select_";
      if (type == kAttFillS) s+= "Fill_Select_";
      s += ival;
   }
   list->Add(new TObjString(s.Data()));
} 
//_______________________________________________________________________________
   
void AddObjString(const char * text, TList * list)
{
   list->Add(new TObjString(text));
} 
//_______________________________________________________________________________
   
void AddObjString(Double_t val, TList * list)
{
   ostringstream buf;
   buf << val;
   list->Add(new TObjString(buf.str().c_str()));
//   list->Add(new TObjString(Form("%lf", val)));
} 
//_______________________________________________________________________________

const char * GetText(TList * list, Int_t pos) 
{
   return ((TObjString*)list->At(pos))->GetString();
}   
//_______________________________________________________________________________

Double_t GetDouble(TList * list, Int_t pos) 
{
   TMrbString s;
   s = ((TObjString*)list->At(pos))->GetString();
   Double_t val;
   if (!s.ToDouble(val)) {
      cout << "Illegal doubel: " << s << endl;
      val = 0;
   }
   return val;
}   
//_______________________________________________________________________________
  
Int_t GetInt(TList * list, Int_t pos)
{
   TMrbString s;
   Int_t val;
   Bool_t iscol = kFALSE;
   Bool_t isfont = kFALSE;
   s = ((TObjString*)list->At(pos))->GetString();
   if (s.BeginsWith("CheckButton_") || s.BeginsWith("RadioButton_")) {
      if (s.EndsWith("Down")) val = 1;
      else                    val = 0;
   } else {
      if ( s.BeginsWith("ColorSelect_")) iscol = kTRUE;
      if ( s.BeginsWith("CfontSelect_")) isfont = kTRUE;
      if ( s.BeginsWith("ColorSelect_") || 
           s.BeginsWith("AlignSelect_") ||
           s.BeginsWith("LineSSelect_") ||
           s.BeginsWith("CheckButton_") ||
           s.BeginsWith("Mark_Select_") ||
           s.BeginsWith("Fill_Select_") ||
           s.BeginsWith("CfontSelect_") ||
           s.BeginsWith("ArrowSelect_")) s.Remove(0,12);
      if (!s.ToInteger(val)) {
         cout << "Illegal integer: " << s << endl;
         val = 0;
      } else {
         if (iscol) {
           val =  TColor::GetColor((UInt_t)val);
           if (!GetColorByInd(val)) val = 1;
         }
         if (isfont) val = val * 10 + 2;
      } 
   }    
   return val;
}   
   
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

   Int_t xw = xwid_default;
   Int_t yw = ywid_default;
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
      xw = 40 + maxlen_nam * 12;
   if (Nentries < 25)
      yw = 24 * (Nentries + 1);
   TString pname(fname);
//   pname.Prepend("CWD:");
   HTCanvas *cHCont = new HTCanvas(pname.Data(), pname.Data(),
                                   -xpos, ypos, xw, yw, hpr, 0);
   Int_t item_height = TMath::Min(24, 10000/Nentries);
   cHCont->SetCanvasSize(xw, item_height * Nentries);
   ypos += 50;
   if (ypos > 500) ypos = 5;

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

//   if (Nentries <= 3) 
//      dy = .999 / (Float_t) (Nentries + 1);
//   else               
      dy = .999 / (Float_t) (Nentries);

//   cout << "Nentries, dy " << Nentries << " " <<  dy << endl;
   y0 = 1.;
   y = y0 - dy;
   TString sel;
   TString title;
   for (Int_t i = 0; i < Nentries; i++) {
      Float_t xcmd = xcmd_no_sel;
      CmdListEntry *cle = (CmdListEntry *) fcmdline->At(i);
 //     cout << "new: " << cle->fCmd << endl;
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
//      cout << "y, dy " << y << " " << dy << endl;
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
   cHCont->SetCanvasSize(usedxw, usedyw);
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

void InfoBox(const char *message, TGWindow * win)
{
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), win,
                "Info", message,
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
Bool_t OpenWorkFile(TGWindow * win, TFile ** file)
{
   static const char *fWorkname = 0;
   static TFile *fWorkfile = 0;
   static const char *cfname;
   if (fWorkfile) {
//      WarnBox("Close open workfile");
      fWorkfile->Close();
      fWorkfile = NULL;
      if (file) *file = NULL;
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
   if (!ok) {
      if (file) *file = NULL;
      return kFALSE;
   }
   fname = fWorkname;
   if (!gSystem->AccessPathName(fWorkname, kFileExists))
      fWorkfile = new TFile(fWorkname, "UPDATE", "Output file");
   else
      fWorkfile = new TFile(fWorkname, "RECREATE", "Output file");
   if (file) *file = fWorkfile;
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
Int_t GetObjects(TList & list, TDirectory * rfile, const char * classname)
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


void Canvas2LP(TCanvas * ca, Bool_t to_printer, TGWindow * win)
{
// if opt contains plain: force white background
//                    ps: write to PostScript file  
   const char helpText_PS[] =
"Write picture to PostScript file or send\n\
directly to a printer. With Option\"Force white background\"\n\
background of pads will be white. This is\n\
recommended to save toner of printers.\n\
\n\
Papersize for A4 is X: 20, Y 26, for Letter 20, 24\n\
\n\
Fine tuning of picture size and position on paper\n\
may be done by setting Scale and shift values.\n\
Shift is done prior to scale, so if scale is 0.5\n\
a shift value of 10 will only shift by 5 cm";

   if (!ca) return;
   Int_t *padfs = 0;
   Int_t *padframefs = 0;
   Int_t cafs = 0, caframefs = 0;
   Int_t npads = 0;
   const Int_t maxpads = 1000;

   const char hist_file[] = {"printer_hist.txt"};
   Bool_t ok;
   Int_t itemwidth = 320;
//   ofstream hfile(hist_file);
   static Int_t plain = 1;
   static Double_t xpaper = 20;  // A4 and letter
   static Double_t ypaper = 26;  // A4 24, letter 26
   static Double_t scale  = 1;
   static Double_t xshift = 0;
   static Double_t yshift = 0;
   static Int_t view_ps = 1;

   TList *row_lab = new TList(); 
   TList *values  = new TList();
   row_lab->Add(new TObjString("Force white background"));
   row_lab->Add(new TObjString("Page size X [cm]"));
   row_lab->Add(new TObjString("Page size Y [cm]"));   
   row_lab->Add(new TObjString("Apply scale factor"));
   row_lab->Add(new TObjString("Apply X shift[cm]"));
   row_lab->Add(new TObjString("Apply Y shift[cm]"));
   row_lab->Add(new TObjString("View ps file automatically"));

   AddObjString(plain, values, kAttCheckB);
   AddObjString(xpaper, values);
   AddObjString(ypaper, values);
   AddObjString(scale, values);
   AddObjString(xshift, values);
   AddObjString(yshift, values);
   AddObjString(view_ps, values, kAttCheckB);
  
   TString cmd_name;
   TString prompt;
   TEnv env(".rootrc");
   if (to_printer) {
      prompt = "Printer command";
      cmd_name = "lpr ";
      const char *pc = gSystem->Getenv("PRINTER");
      if (pc) {
         cmd_name += "-P";
         cmd_name += pc;
      }
      const char *ccmd =
          env.GetValue("HistPresent.PrintCommand", cmd_name.Data());
      cmd_name = ccmd;
   } else { 
      cmd_name = ca->GetName();
      cmd_name += ".ps";
      prompt = "PS file name";
   } 
   ok = GetStringExt(prompt.Data(), &cmd_name, itemwidth, win,
                   hist_file, NULL, row_lab, values,
                   NULL, NULL, helpText_PS);
   if (!ok) {
      cout << "Printing canceled" << endl;
      return;
   }
   Int_t vp = 0;
   plain  = GetInt(values,    vp++);
   xpaper = GetDouble(values, vp++);
   ypaper = GetDouble(values, vp++);
   scale  = GetDouble(values, vp++);
   xshift = GetDouble(values, vp++);
   yshift = GetDouble(values, vp++);
   view_ps  = GetInt(values,  vp++);

   if (plain) {
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

   gStyle->SetPaperSize((Float_t) xpaper, (Float_t) ypaper);
   if (scale != 1 || xshift != 0 || yshift != 0) {
      TString extra_ps ;
      if (xshift != 0 || yshift != 0) {
//    convert to points (* 4 to compensate for ROOTs .25)
         Int_t xp = (Int_t) (xshift * 4 * 72. / 2.54);         
         Int_t yp = (Int_t) (yshift * 4 * 72. / 2.54); 
         extra_ps += Form("%d %d  translate ", xp, yp);
      }
      if (scale != 1)extra_ps += Form("%f %f scale ", scale, scale); 
      gStyle->SetHeaderPS(extra_ps.Data());
   }
   TString fname;
   if (to_printer) {
      fname = "temp_pict.ps";
   } else {
      fname = cmd_name;
   }
   ca->SaveAs(fname.Data());
   if (to_printer) {
      TString prtcmd(cmd_name.Data());
      prtcmd += " ";
      prtcmd += fname.Data();
      gSystem->Exec(prtcmd.Data());
   }
	if (view_ps) {
      char * gvcmd = gSystem->Which("/usr/bin:/usr/local/bin", "gv");
      if (gvcmd == NULL)gvcmd = gSystem->Which("/usr/bin:/usr/local/bin", "ggv"); 
      if (gvcmd != NULL) {
         TString cmd(gvcmd);
		   cmd += " ";
		   cmd += fname.Data();
		   cmd += "&";
		   gSystem->Exec(cmd.Data());
         delete gvcmd;
      }
	}
// restore backgrounds 
   if (plain) {
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
   return GetDefaults(hname.Data(), mustexist);
}

//_________________________________________________________________________________________

TEnv *GetDefaults(const char * hname, Bool_t mustexist)
{
   TEnv *lastset = 0;
   TString defname;
   TEnv env(".rootrc");         // inspect ROOT's environment
   defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
//   cout << "Got : " << defname.Data() << endl;
   if (defname.Length() > 0) {
      defname += "_";
      defname += hname;
		Int_t ip = defname.Index(";");
		if (ip > 0) defname.Resize(ip);
      defname += ".def";
//      cout << "Look for : " << defname.Data() << endl;
      if (mustexist && gSystem->AccessPathName(defname.Data()))
         return 0;
//      cout << "Looked for : " << defname.Data() << endl;
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
		cout << name << " " << fname << endl;
   	if (name.BeginsWith(fname)) {
      	Int_t start  = name.Index(" ") + 1;
      	Int_t length = name.Length() - start;
      	name = name(start,length);
			Int_t ip = name.Index(";");
			if (ip > 0) name.Resize(ip);
      	TString question("Delete: ");
      	question += name.Data();
      	question += " from ";
      	question += fname;
      	if (QuestionBox(question.Data(), win) == kMBYes) {      
         	TFile * f = new TFile(fname, "update");
         	if (f->Get(name.Data())) {
               list->Remove(sel);
               Int_t cycle = f->GetKey(name.Data())->GetCycle();
               if (cycle > 0) {
                  name += ";";
                  name += cycle;
               }
               cout << "Deleting: " << name.Data() << endl;
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
   new TCanvas("ct", "Text fonts used by root", 100, 100, 600, 600);
   Float_t x0 = 0.05, y = 0.9, dy = 0.05;
   TText *t1;
   TText *t;
   for (Int_t i = 10; i <= 120; i += 10) {
      t1 = new TText(x0, y, Form("%d", i));
      t = new TText(x0 + 0.1, y, text);
      t1->SetTextSize(0.035);
      t1->SetTextFont(i);
      t->SetTextSize(0.035);
      t->SetTextFont(i);
      t1->Draw();
      t->Draw();
      y -= dy;
   }
   t = new TText(0.45, y, "TextAlign");
   t->SetTextSize(0.04);
   t->SetTextAlign(22);
   t->SetTextFont(60);
   t->Draw();
  
   TPad * pp = new TPad("apad", "apad", 0.3, 0.01, 0.6 ,0.25);
   pp->SetLineWidth(2);
   pp->SetFillColor(11);
   pp->SetFillStyle(1001);
   pp->Draw();
   pp->cd();
   
   t = new TText(0.05, 0.05, "11"); t->SetTextAlign(11);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.05, 0.5, "12");  t->SetTextAlign(12);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.05, 0.95, "13"); t->SetTextAlign(13);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();

   t = new TText(0.5, 0.05, "21");   t->SetTextAlign(21);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.5, 0.5, "22");   t->SetTextAlign(22);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.5, 0.95, "23");   t->SetTextAlign(23);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();

   t = new TText(0.95, 0.05, "31");   t->SetTextAlign(31);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.95, 0.5, "32");   t->SetTextAlign(32);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
   t = new TText(0.95, 0.95, "33");   t->SetTextAlign(33);
   t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
};

//______________________________________________________________________________________
  
void DrawColors() 
{
//   TString hexcol;
   TString scol;
//   TString cmd;
   new TCanvas("rgb_colors", "Predefined RGB colors", -400, 20, 800, 400);
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
 
   new TCanvas("hue_colors", "Hue values", -400,450, 400, 400);
   TArc * a;
   TColor * col; 
   Int_t hue;
   Double_t x1, y1, radius, phi1, phi2;
   Float_t red,green,blue;
   x1= 0.5;
   y1= 0.5;
   radius = 0.4;
   Int_t ind =1001;
   for (hue = 1; hue < 360; hue += 2) {
      phi1 = hue - 1;
      phi2 = hue + 1;
      TColor::HLS2RGB((Float_t)hue, 0.5, 1, red, green, blue);

      col = GetColorByInd(ind);
      if (col) delete col;
      col = new TColor(ind, red,green,blue);
      a = new TArc(x1, y1, radius, phi1, phi2);
 //     col->Print();
      a->SetFillColor(ind);
      a->SetLineColor(0);
      a->SetLineWidth(0);
      ind++;
      a->Draw();
   }
    a = new TArc(x1, y1, radius / 3);
    a->SetFillColor(10);
    a->SetFillStyle(1001);
    a->Draw();
    TText * tt = new TText();
    tt->SetTextAlign(22);
    tt->DrawText(x1 + 0.45, y1,  "0");
    tt->DrawText(x1 , y1 + 0.43, "90");
    tt->DrawText(x1 - 0.45 , y1 , "180");
    tt->DrawText(x1 , y1 - 0.43, "270");
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
   if (env.Lookup("HistPresent.LastSettingsName") == NULL) {
      cout << "Setting defaults dir/name to: " << defname.Data() << endl;
      env.SetValue("HistPresent.LastSettingsName", defname.Data());
      env.SaveLevel(kEnvUser);
   } else {  
      defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
   }
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
//______________________________________________________________________________________

TGraph * FindGraph(HTCanvas * c) 
{
   cout << "FindGraph(HTCanvas * c) " << c << endl;
   TIter next(c->GetListOfPrimitives());
   TObject * obj;
   TGraph * graph = 0;
   while ( (obj = next()) ) {
      obj->Print();
      if (obj->InheritsFrom("TGraph")) {
        if (graph) cout << "Warning: more than 1 TGraph found" << endl;
        else graph = (TGraph *)obj;
      }
   }
   return graph;
}
   
//______________________________________________________________________________________

void WriteGraphasASCII(TGraph * g,  TRootCanvas * mycanvas)
{
   TString fname = g->GetName();
   fname += ".ascii";
   Bool_t ok;
   fname =
       GetString("Write ASCII-file with name", fname.Data(), &ok, mycanvas);
   if (!ok) {
      cout << " Cancelled " << endl;
      return;
   }
   if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
//      cout << fname << " exists" << endl;
      int buttons = kMBOk | kMBDismiss, retval = 0;
      EMsgBoxIcon icontype = kMBIconQuestion;
      new TGMsgBox(gClient->GetRoot(), mycanvas,
                   "Question", "File exists, overwrite?",
                   icontype, buttons, &retval);
      if (retval == kMBDismiss)
         return;
   }
   ofstream outfile;
   outfile.open((const char *) fname);
//   ofstream outfile((const char *)fname);
   Double_t * x = 0;
   Double_t * y = 0;
   Double_t * ex = 0;
   Double_t * ey = 0;
   Double_t * exl = 0;
   Double_t * exh = 0;
   Double_t * eyl = 0;
   Double_t * eyh = 0;

   if (outfile) {
      x = g->GetX();
      y = g->GetY();
      if (g->IsA() == TGraphErrors::Class()) {
         ex = ((TGraphErrors *)g)->GetEX();
         ey = ((TGraphErrors *)g)->GetEY();
      }
      if (g->IsA() == TGraphAsymmErrors::Class()) {
         exl = ((TGraphAsymmErrors *)g)->GetEXlow();
         exh = ((TGraphAsymmErrors *)g)->GetEXhigh();
         eyl = ((TGraphAsymmErrors *)g)->GetEYlow();
         eyh = ((TGraphAsymmErrors *)g)->GetEYhigh();
      }
      for (Int_t i = 0; i < g->GetN(); i++) {
         outfile << x[i] << "\t" << y[i];
         if (ex)  outfile << "\t" << ex[i] << "\t" << ey[i];
         if (exl) outfile << "\t" << exl[i] << "\t" << exh[i] 
                       << "\t" << eyl[i] << "\t" << eyh[i];
         outfile << endl;
      }
      outfile.close();
      cout << g->GetN() << " lines written to: "
          << (const char *) fname << endl;
   } else {
      cout << " Cannot open: " << fname << endl;
   }
}
//______________________________________________________________________________________

void WriteOutGraph(TGraph * g, TRootCanvas * mycanvas)
{
   if (g) {
      TString name = g->GetName();
      Bool_t ok;
      name =
          GetString("Save hist with name", name.Data(), &ok, mycanvas);
      if (!ok)
         return;
      g->SetName(name.Data());
      if (OpenWorkFile(mycanvas)) {
         g->Write();
         CloseWorkFile();
      }
   }
}
//______________________________________________________________________________________

Bool_t fixnames(TFile * * infile, Bool_t checkonly)
{
   TFile * outfile = 0;
   TString outfile_n((*infile)->GetName());
   if (!checkonly) {
      if (outfile_n.EndsWith(".root")) outfile_n.Resize(outfile_n.Length()-5);
      else                             cout << "Warning: " << outfile_n 
                                    << " doesnt end with .root" << endl;
      outfile_n += "_cor.root";
      outfile = new TFile(outfile_n, "RECREATE");
   }
   TIter next((*infile)->GetListOfKeys());
   TKey* key;
   TNamed * obj;
   TString name;
   Bool_t needsfix = kFALSE;
//   cout << "enter fixnames" << endl;
   TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
   while( (key = (TKey*)next()) ){
      (*infile)->cd();
      obj = (TNamed*)key->ReadObj(); 
      name = obj->GetName();
      Bool_t changed = kFALSE;
      Int_t isem = name.Index(";");
		if (isem > 0) {
		   name.Resize(isem);
         changed = kTRUE;
		}
      while (name.Index(notascii) >= 0) {
         name(notascii) = "_";
         changed = kTRUE;
      }
      if (changed) {
         if (checkonly) {
            cout << "Orig name: " << obj->GetName() << endl;
            cout << "New  name: " << name << endl;
         }
         needsfix = kTRUE;
         obj->SetName(name);
      }
      if (!checkonly) {
         outfile->cd();
         obj->Write();
      }
   }
   if (outfile) {
//      cout << "new file ++++++++++++++++++++++++++++++++++++++++++++=" <<endl;
 //     outfile->ls();
      outfile->Close();
      if (needsfix) {
//          cout << "new file: " <<outfile_n<<endl;
         (*infile)->Close();
         *infile = new TFile(outfile_n, "READ");
//         (*infile)->ls();
      }
   }
   return needsfix;
}
//______________________________________________________________________________________

TGraph * PaintArea (TH1 *h, Int_t binl, Int_t binh, Int_t color) 
{
   Int_t nbins = binh - binl + 1;
   cout << "PaintArea " << binl << " " << binh << endl;
   TGraph * pl  = new TGraph(2 * nbins + 3);
   Int_t ip = 0;
   for (Int_t bin = binl; bin <= binh; bin++) {
      pl->SetPoint(ip, h->GetBinLowEdge(bin), h->GetBinContent(bin));
      pl->SetPoint(ip + 1 , h->GetBinLowEdge(bin) + h->GetBinWidth(bin), 
                            h->GetBinContent(bin));
      ip += 2;
   }
   Axis_t ymin = h->GetYaxis()->GetXmin();
   pl->SetPoint(ip, (pl->GetX())[ip-1], ymin);
   pl->SetPoint(ip+1, (pl->GetX())[0],  ymin);   
   pl->SetPoint(ip+2, (pl->GetX())[0],  (pl->GetY())[1]);   
   pl->SetFillColor(color);
   pl->SetFillStyle(1001);
   return pl;
}
//______________________________________________________________________________________

Int_t getcol() 
{
//   TString hexcol;
   TNamed * colobj = new TNamed("colobj", "colobj");
   gROOT->GetListOfSpecials()->Add(colobj);
   colobj->SetUniqueID(0);
   TString scol;
   TString cmd;
   TCanvas * ccol = new TCanvas("colcol", "rgb colors", 400, 20, 800, 400);
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
      cmd = "gROOT->GetListOfSpecials()->FindObject(\"colobj\")->SetUniqueID(";
      cmd += colindex;
      cmd += ");";
      b = new TButton(scol.Data(), cmd.Data(), x, y, x + .9*dx , y + .9*dy );
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
//      if (colindex == 1) b->SetTextColor(10); 
//      else               b->SetTextColor(1);
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
   Int_t ind = 0;
   while ( colobj->GetUniqueID() == 0) {
      if (gSystem->ProcessEvents())
         break;
      gSystem->Sleep(50);
      if (!gROOT->GetListOfCanvases()->FindObject("colcol")) {
         gROOT->GetListOfSpecials()->Remove(colobj);
         delete colobj;
         return 0;
      }
   }
   ind = colobj->GetUniqueID();
   gROOT->GetListOfSpecials()->Remove(colobj);
   delete colobj;
   delete ccol;
   return ind; 
}
//________________________________________________________________
void IncrementIndex(TString * arg)
{
// find number at end of string and increment,
// if no number found add "_0";
   Int_t len = arg->Length();
   if (len < 0) return;
   Int_t ind = len - 1;
   Int_t first_digit = ind;
   TString subs;
   while (ind > 0) {
      subs = (*arg)(ind, len - ind);
      cout << subs << endl;
      if (!subs.IsDigit()) break;
      first_digit = ind;
      ind--;
   }
   if (first_digit == ind) {
     *arg += "_0";
   } else { 
      subs = (*arg)(first_digit, len - first_digit);
      Int_t num = atol(subs.Data());
      arg->Resize(first_digit);
      *arg += (num + 1);
   }
}
//________________________________________________________________

TString * GetStringArg(TString * arg,  Int_t nth)
{
   TRegexp apo("\"");
	TString * result = new TString(*arg);
   for (Int_t i = 0; i < 2*nth; i++) (*result)(apo) = "";
	Int_t ip = result->Index(apo);
	if (ip < 0) {
	  delete result;
	  return NULL;
	}
	result->Remove(0,ip+1);
//	cout << *result << endl;
	ip = result->Index(apo);
	if (ip < 0) {
	  delete result;
	  return NULL;
	}
	result->Resize(ip);
	return result;
}
//________________________________________________________________

TList * BuildList(const char *bp)
{
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
   TCanvas * mcanvas = b->GetCanvas();
//   cp_many title start
   TString ctitle(mcanvas->GetTitle());
   TRegexp cprefix("CP_");
   TRegexp cpostfix(".histlist");
   ctitle(cprefix) = "";
   ctitle(cpostfix) = "";
	TList * hlist = new TList();
	hlist->SetName(ctitle);
//   cout << "Title of mcanvas: " << ctitle.Data() << endl;
//  cp_many title end
   TIter next(mcanvas->GetListOfPrimitives());
   TString cmdline;
	TString entry;
	TString * arg;
   while (  TObject *obj=next()) {
      if (!strncmp(obj->ClassName(), "TButton", 7)) {
         TButton * button = (TButton*)obj;
         cmdline = button->GetMethod();
         if (cmdline.Contains("Select")) {
//            cout << cmdline  << endl;
            arg = GetStringArg(&cmdline, 0);
				if (arg) {
				   entry = *arg;
					delete arg;
			   } else {
				   cout << "Illegal cmd: " << cmdline << endl;
					continue;
			   }
				entry += " ";
            arg = GetStringArg(&cmdline, 2);
				if (arg) {
				   entry += *arg;
					delete arg;
			   } else {
				   cout << "Illegal cmd: " << cmdline << endl;
					continue;
			   }
				entry += " ";
            arg = GetStringArg(&cmdline, 1);
				if (arg) {
				   entry += *arg;
					delete arg;
			   } else {
				   cout << "Illegal cmd: " << cmdline << endl;
					continue;
			   }
//				cout << "|" << entry.Data() << "|" << endl;
            hlist->Add(new TObjString(entry.Data()));
         }
      }
   }
	return hlist;
}
//_______________________________________________________

Int_t FindHistsInFile(const char * rootf, const char * listf) 
{
   ifstream list(listf);
	if (!list.good()) {
	   cout << "Cant open: " << listf << endl;
		return -1;
   } 
	TFile f(rootf);
	if (!f.IsOpen()) {
	   cout << "Cant open: " << rootf << endl;
		return -1;
   } 
	char line[100];
	Int_t nfound = 0;
	while (1) {
	   list >> line;
		if (list.eof()) break;
		TObject * obj = f.Get(line);
		if (obj) nfound ++;
	}
//   cout << "FindHistsInFile " << rootf<< " " << listf << " : " << nfound<< endl;
	return nfound;
}
//__________________________________________________________________________

TH2 * rotate_hist(TH2 * hist, Double_t angle_deg, Int_t serial_nr)
{
   if (!hist) return NULL;
//   cout << "Enter rotate_hist" << endl << flush;
   TString hname(hist->GetName());
   Int_t us = hname.Index("_");
   if (us >= 0) hname.Remove(0, us + 1); // remove all before first underscore
   hname += "_rot";
   hname += serial_nr;
   TString htitle(hist->GetTitle());
   htitle += "_rot";
   htitle += serial_nr;
   TH1 * hold = (TH1*)gROOT->GetList()->FindObject(hname);
   if (hold) {
      delete hold;
//      cout << "rotate_hist,hold: " << hold << endl;
   }
//   cout << "rotate_hist before  hist->Clone()" << endl << flush;
   TH2 * hrot = (TH2*)hist->Clone();
//   cout << "rotate_hist after hist->Clone()" << endl << flush;
   hrot->Reset();
   hrot->SetName(hname);
   hrot->SetTitle(htitle);
//   TH2F * hrot = new TH2F(hname, htitle, nbinx, 0, xa, nbiny, 0, ya);
   Int_t firstx = hist->GetXaxis()->GetFirst();
   Int_t lastx  = hist->GetXaxis()->GetLast();
   Int_t firsty = hist->GetYaxis()->GetFirst();
   Int_t lasty  = hist->GetYaxis()->GetLast();
   Int_t nbinsx = hist->GetXaxis()->GetNbins();
   Int_t nbinsy = hist->GetYaxis()->GetNbins();
//   Axis_t xoff = 0.5 * (Axis_t)(lastx - firstx + 2);
//   Axis_t yoff = 0.5 * (Axis_t)(lasty - firsty + 2);
   Axis_t xoff = 0.5 * (Axis_t)(nbinsx + 1);
   Axis_t yoff = 0.5 * (Axis_t)(nbinsy + 1);
//   cout << xoff << " " << yoff << endl;
   Float_t angle = TMath::Pi() * angle_deg / 180.;
//   Float_t angle = TMath::ATan(tan_a);
   Float_t sina = TMath::Sin(angle);
   Float_t cosa = TMath::Cos(angle);
   for (Int_t binx = firstx; binx <= lastx; binx++) {
      for (Int_t biny = firsty; biny <= lasty; biny++) {
         Axis_t x = (Axis_t)binx - xoff; 
         Axis_t y = (Axis_t)biny - yoff;
         Int_t xr = Int_t(x * cosa - y *sina + xoff);
         Int_t yr = Int_t(x * sina + y *cosa + yoff);
         if (xr >= firstx && xr <= lastx && yr >= firsty && yr <= lasty)
            hrot->SetBinContent(binx, biny, hist->GetBinContent(xr, yr));
      }
   }  
//   cout << "Exit rotate_hist" << endl << flush;
   return hrot;
}
