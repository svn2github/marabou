#include "TROOT.h"
#include "TEnv.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TFile.h"
#include "TMapFile.h"
#include "TKey.h"
#include "TCutG.h"
#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TGMsgBox.h"
#include "TObject.h"
#include "TTree.h"
#include "TBranch.h"
#include "TBranchObject.h"
#include "TLeafObject.h"
#include "TProfile.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TVirtualPad.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TApplication.h"
#include "TDiamond.h"
#include "TCanvas.h"
//#include "TArrayI.h"
//#include "TArrayF.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TFrame.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjectTable.h"
#include "TList.h"
#include "TMessage.h"
#include "TGraphErrors.h"
#include "CmdListEntry.h"
#include "TGMrbInputDialog.h"
#include "HistPresent.h"
#include "FitHist.h"
#include "FhContour.h"
#include "FitHist_Help.h"
#include "support.h"
//#include "HprDefMacro.cxx"
#include "TGMrbTableFrame.h"
#include "TMrbStatistics.h"
#include "HTCanvas.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbHelpBrowser.h" 

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>

static const char  *fHlistSuffix=".histlist";
Int_t nHists;
extern TH1 * gHpHist;
//_____________________________________________________________________________________

const char AttrTemplate[]=
"FH_setdefaults(const char *cname, const char* hname) \n\
{\n\
\n\
// This is a template attributes file\n\
\n\
// this macro is called when FitHist draws / expands a histogram\n\
// please read the documentation (http://root.cern.ch) for all methods\n\
// defined for histograms and canvases and styles\n\
\n\
// Methods of HistPresent itself may be used via the pointer mypres.\n\
// This can be used to get a histograms from any file  and overlay it\n\
// See line with GetHist()\n\
\n\
//  gROOT->Reset();\n\
// find pointer to HistPresent object\n\
  class HistPresent;\n\
  HistPresent * mypres = (HistPresent*)gROOT->FindObject(\"mypres\");\n\
  if (!mypres) { \n\
     cout << \"histogram presenter object not found\" << endl;\n\
     return 0;\n\
  }\n\
  TCanvas* canvas = (TCanvas*)gROOT->FindObject(cname);\n\
  TH1* hist = 0;\n\
  if (gROOT->GetVersionInt() >= 22306) \n\
     TH1* hist = mypres->GetCurrentHist();   // assure backward comp\n\
  else    hist = (TH1*)gROOT->FindObject(hname);\n\
\n\
  if (!hist || !canvas) {\n\
     cout << \"hist or canvas not found\" << endl;\n\
     return 0;\n\
  }\n\
\n\
//   cout << \" Setting defaults for histogram\" <<  endl;    // tell what we are doing\n\
//   cout << \" Name:  \" << hist->GetName() <<  endl;\n\
   TString title = hist->GetTitle();\n\
   TString name = hist->GetName();\n\
\n\
//  global attributes\n\
\n\
   gStyle->SetOptStat(1001111);    // draw a statistics box\n\
                                   // Integral,(overf, underf,) mean, rms, entries, name \n\
//   gStyle->SetTitleYSize(0.08);\n\
//   gStyle->SetStatH(0.3);\n\
//   gStyle->SetStatW(0.3);\n\
//   gStyle->SetPadLeftMargin(0.07);\n\
//   gStyle->SetPadRightMargin(0.02);\n\
//   gStyle->SetPadTopMargin(0.08);\n\
//   gStyle->SetPadBottomMargin(0.05);\n\
\n\
//   canvas specific attributes\n\
\n\
//    gPad->GetFrame()->SetFillStyle(1001);       // colored background \n\
//    gPad->GetFrame()->SetFillColor(38);         // sea blue\n\
//    gPad->GetFrame()->SetFillStyle(0);   // empty background\n\
//    gPad->SetFillStyle(0);\n\
\n\
//   histogram specific attributes\n\
//   if it should apply for 1 or 2 dim put inside if (  )\n\
\n\
//   hist->SetLabelSize(0.06, \"Y\");\n\
//   hist->SetLabelSize(0.06, \"X\");\n\
//   hist->SetNdivisions(505, \"Y\");\n\
//   hist->SetNdivisions(505, \"X\");\n\
//   hist->SetDrawOption(\"hist\");\n\
//   hist->SetFillStyle(1001);\n\
//   hist->SetFillColor(42);\n\
\n\
//   Example how to use regular expressions \n\
\n\
//   TRegexp end_with_w=\"w$\";\n\
//   if (name.Index(end_with_w) >= 0) \n\
//     cout <<  \"Hist name ends with w\"<< endl;\n\
//   TRegexp start_with_e=\"^e\";\n\
//   if (name.Index(start_with_e) >= 0) \n\
//      cout <<  \"Hist name starts with e\" << endl;\n\
\n\
    gROOT->ForceStyle();            // overwrite attributes store with hists\n\
\n\
   if (hist->InheritsFrom(\"TH3\"))  // for 3 dim hists\n\
   {\n\
//      if (title == \"mprer\")hist->SetMaximum(200);\n\
//      hist->SetMinimum(1000);\n\
      hist->SetDrawOption(\"box\");\n\
//      hist->SetFillStyle(1001);\n\
//      hist->SetFillColor(46);\n\
      canvas->Modified(kTRUE);\n\
//      canvas->SetLogz();                   // log scale \n\
   } else if (hist->InheritsFrom(\"TH2\"))  // for 2 dim hists\n\
   {\n\
//      if (title == \"mprer\")hist->SetMaximum(200);\n\
//      hist->SetMinimum(1000);\n\
      hist->SetDrawOption(\"box\");\n\
      hist->SetFillStyle(1001);\n\
      hist->SetFillColor(46);\n\
      canvas->Modified(kTRUE);\n\
//      canvas->SetLogz();                   // log scale \n\
   } else {                          // options for 1 dim \n\
\n\
//     overlay another histogram if name matches hsimple_hpx\n\
\n\
//      if (name == \"hsimple_hpx\") {\n\
//     get histogam named hpx1 from file hsimple.root\n\
//         TH1 * hs = mypres->GetHist(\"hsimple.root\",\"hpx1\");\n\
//         if (hs) {\n\
//            draw and set line color to blue\n\
//            hs->Draw(\"same\");\n\
//            hs->SetLineColor(4);\n\
//         }\n\
//      }\n\
//      hist->GetXaxis()->SetRange(50,2050); // draw only bins 50-2050 \n\
      hist->SetDrawOption(\"hist\");\n\
//      hist->SetFillStyle(0);               // hollow\n\
//      hist->SetLineColor(6);\n\
      hist->SetFillStyle(1001);      // solid\n\
      hist->SetFillColor(45);\n\
//      hist->SetMaximum(1000);\n\
   }\n\
}\n\
";
//_____________________________________________________________________________________

ClassImp(HistPresent)

// constructor

HistPresent::HistPresent(const Text_t *name, const Text_t *title)
            :TNamed(name,title)
{
//   cout<< "enter HistPresent ctor "  <<endl;
//   TDirectory *fDirectory;
//   TList *fFunctions;
   fOpfac= 1.;
   fRebin = 2;
   fRMethod = 0;
   fSeqNumberMany = 0;

   fNwindows=   0;

//   cout<< "HistPresent ctor , fWintopx " <<  fWintopx  <<endl;
   fRootFile = NULL;
   activeFile= NULL;
   activeHist= NULL;
   filelist = NULL;
   fControlBar = NULL;
   maincanvas=0;
   lastcanvas=0;
   
   fByTitle=kFALSE;

   fCanvasList = new TList();
   fAllCuts = new TList();
   fAllWindows = new TList();
   fAllFunctions = new TList();
   fHistLists = new TList();
   fSelectHist = new TList();
   fHistListList = new TList();
   fSelectCut = new TList();
   fSelectWindow = new TList();
   fSelectLeaf = new TList();
   fSelectContour = new TList();
   fSelectGraph = new TList();
   fAllContours = new TList();

   gROOT->GetListOfCleanups()->Add(this);

   fGraphCut = new TString();
   fCutVarX = new TString();
   fCutVarY = new TString();
   fLeafCut = new TString();
   fExpression = new TString();
   fFileSelMask = new TString();
   fHistSelMask = new TString();
   fHistSelMask_1 = new TString();
   fHistSelMask_2 = new TString();
   fDrawOpt2Dim = new TString("COLZ");
   f2DimColorPalette = new TString("REGB");
//   fAutoExecName_1 = new TString();
//   fAutoExecName_2 = new TString();
   fHelpDir        = new TString();
   fHostToConnect  = new TString("localhost");
   fComSocket       = 0;
   fSocketToConnect = 9090;
   fConnectedOnce = kFALSE;
   fHelpBrowser = NULL;
   fUseHist = kFALSE;
   fApplyGraphCut = kFALSE;
   fApplyLeafCut = kFALSE;
   fApplyExpression = kFALSE;

   RestoreOptions();
   fCmdLine = new TList();
   if (gDirectory) {
      HistPresent *hold = (HistPresent*)gDirectory->GetList()->FindObject(GetName());
      if (hold) {
//         Warning("Build","Replacing existing : %s",GetName());
         delete hold;
      }
//      AppendDirectory();
      gDirectory->Append(this);
   }
};

//________________________________________________________________
// destructor

HistPresent::~HistPresent()
{
   cout<< "enter HistPresents dtor" <<endl;
   if (fComSocket) {
      fComSocket->Send("M_client exit");
   }
   SaveOptions();
//   CloseAllCanvases();
   gDirectory->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   if (cHPr )delete cHPr;
};
//________________________________________________________________
void HistPresent::RecursiveRemove(TObject * obj)
{
   fCanvasList->Remove(obj);
   fAllCuts->Remove(obj);
   fAllWindows->Remove(obj);
   fAllFunctions->Remove(obj);
   fHistLists->Remove(obj);
   fSelectHist->Remove(obj);
   fHistListList->Remove(obj);
   fSelectCut->Remove(obj);
   fSelectWindow->Remove(obj);
   fSelectLeaf->Remove(obj);
}
//________________________________________________________________
void HistPresent::ShowMain()
{
   nHists=0;
   cHPr = new HTCanvas("cHPr", "Histogram Presenter",5,5, 250, 400, this, 0);
   cHPr->cd();
   maincanvas = GetMyCanvas();

   Float_t  dy=0.068, y=1. - 6.5*dy - 0.001, x0=0.01, x1=0.97;
   TButton *b;
   Int_t hint_delay=1000;
//   SButton("ByTitle","ByName",this,1,1,dy,1, 0.8);
//   CButton("OpenWorkFile",      "Open WorkFile",this,1,1,dy,0.5);
   b = CButton("CloseHistLists",    "Close HistLists",this,1,1,dy,0.5);
   b = CButton("CloseAllCanvases",  "Close Windows",this,2,1,dy,0.5);
   b = CButton("ListSelect",        "List Hists,Cuts",this,1,2,dy,0.5);
   b = CButton("ClearSelect",       "Clear Select",this,2,2,dy,0.5);
   b = CButton("Editrootrc",        "Edit .rootrc",this,1,3,dy,0.5);
   b->SetToolTipText("Edit resource file .rootrc in CWD or HOME",hint_delay);
   b = CButton("EditAttrFile",      "Edit AttrMacro",this,2,3,dy,0.5);
   b->SetToolTipText("Edit a macro used to customize drawing options",hint_delay);
//   b = CButton("DiffHist",          "Diffte",this,1,3,dy,0.25);
//   b = CButton("FoldSquareWave",    "Fpeaks",this,2,3,dy,0.25);
   b = CButton("RebinHist",         "Rebin",this,1,4,dy,0.25);
   b->SetToolTipText("Rebin a selected histogram",hint_delay);
   fRebin2 = SButton("SetRebinValue(2);", "2",this,3,4,dy,1,0.125);
   fRebin2->SetToolTipText("Set rebin value to 2",hint_delay);
   fRebin4 = SButton("SetRebinValue(4);", "4",this,4,4,dy,0,0.125);
   fRebin4->SetToolTipText("Set rebin value to 4",hint_delay);
//   SButton("SetRebinValue(8);", "8",this,5,4,dy,0,0.1);
   fRebinOth = SButton("SetRebinValue(0);", "oth",this,3,4,dy,0,0.25);
   fRebinOth->SetToolTipText("Ask for rebin value",hint_delay);
   fRebinSumAvg = CButton("SetRebinMethod",    "Sum",this,4,4,dy,0.25);
   fRebinSumAvg->SetToolTipText("Use either sum or avarage when rebinning",hint_delay);
   b = CButton("OperateHist(1);",   "+",this,1,5,dy,0.1);
   b->SetToolTipText("Add 2 or more histograms",hint_delay);
   b = CButton("OperateHist(2);",   "-",this,2,5,dy,0.1);
   b->SetToolTipText("Subtract histogram from another",hint_delay);
   b = CButton("OperateHist(3);",   "*",this,3,5,dy,0.1);
   b->SetToolTipText("Multiply histograms, if only one is selected scale it by factor",hint_delay);
   b = CButton("OperateHist(4);",   "/",this,4,5,dy,0.1);
   b->SetToolTipText("Divide histograms ",hint_delay);
//   fValButton = 0;
   fValButton = CButton("SetOperateVal",     "1.",this,3,5,dy,0.25);
   fValButton->SetToolTipText("Factor used in operations (e.g. h1 + fac*h2)",hint_delay);

   TString cmd("mypres->ShowFiles(\"A\")");
   TString tit("Show FileList (alphab)");
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Show files ending .root, .map or .histlist in CWD in alphabetical order",hint_delay);
   y-=dy;
//
//   cmd="mypres->ShowFiles(\"R\")";
//   tit="Show FileList (reverse)";
//   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
//   b->SetToolTipText(
//   "Show files ending .root, .map or .histlist in CWD in reverse alphabetical order",hint_delay);
//
//   y-=dy;
   cmd="mypres->ShowFiles(\"D\")";
   tit="Show FileList (bydate)";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Show files ending .root, .map or .histlist in CWD ordered by date",hint_delay);
   y-=dy;

   cmd="mypres->ShowContents(\"Memory\",\"\")";
   tit="List Objects in Memory";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Show objects (hists, windows, functions currently in memory",hint_delay);
   y-=dy;

   cmd="mypres->ShowContents(\"Socket\",\"\")";
   tit="Hists from M_analyze";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Get hists from running M_analyze via TcpIp",hint_delay);
   y-=dy;

   cmd="mypres->GetFileSelMask()";
   tit="File Selection mask";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Define a regular expression used file selection criterion",hint_delay);
   y-=dy;

   cmd="mypres->GetHistSelMask()";
   tit="Histo Selection mask";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Define a regular expression for selection of a subset of histograms",hint_delay);
   y-=dy;
/*
   cmd = "mypres->DefineHist()";
   tit = "Define new histogram";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Create a histogram optionally to be used when projecting tree entries",hint_delay);
   y-=dy;
*/
   cmd = "mypres->ListMacros()";
   tit = "List Macros";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "List files ending with .C (root macros) in CWD with first line starting with //Hint:",hint_delay);
   y-=dy;

   cmd = "mypres->StackSelectedHists()";
   tit = "Stack selected hists";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Stackselected histograms",hint_delay);
   y-=dy;

   cmd = "mypres->ShowSelectedHists()";
   tit = "Show selected hists";
   b = CommandButton(cmd,tit,x0,y,x1,y+dy);
   b->SetToolTipText(
   "Show selected histograms in one single canvas",hint_delay);

   cHPr->SetEditable(kFALSE);
   cHPr->Update();
   CreateDefaultsDir(maincanvas); 
//   FitHist *dummy= new FitHist("dummy","dummy", 0);
//   delete dummy;
}
//________________________________________________________________________________________

void HistPresent::Editrootrc()
{
   TString EditCmd = ".rootrc";
   Bool_t fok = kFALSE;
   if (!gSystem->AccessPathName(EditCmd.Data())) {
      cout << "Using .rootrc in cwd" << endl;
      fok = kTRUE;
   } else {
      EditCmd.Prepend("/");
      EditCmd.Prepend(gSystem->HomeDirectory());
      if (!gSystem->AccessPathName(EditCmd.Data())) {
         cout << "Using " << EditCmd.Data() << endl;
         fok = kTRUE;
      }   
   }
   if (fok) { 
      EditCmd.Prepend("nedit ");
      gSystem->Exec(EditCmd.Data()); EditCmd += "&";
   }
   else   cout << "Cant find .rootrc" << endl;

   cout << setred << "Warning: values might be overwriten by current settings"
        << setblack << endl;
}
//________________________________________________________________________________________

void HistPresent::EditAttrFile()
{
   if (gSystem->AccessPathName(attrname)) {
      ofstream attrfile(attrname, ios::out);
      if (!attrfile) {
         cout << "Cant open new " << attrname << endl;
         return;
      }
      attrfile << AttrTemplate;
      attrfile.close();
   }
   TString EditCmd = "nedit ";
   EditCmd += attrname; EditCmd += "&";
   gSystem->Exec(EditCmd.Data());
}
//________________________________________________________________________________________

void HistPresent::ShowFiles(const char *how, const char *bp)
{
   if (filelist) {
      if (gROOT->FindObject("Filelist")) delete filelist; 
      activeFile=NULL; 
      filelist=NULL;
   }
   const char *fname;
   void* dirp=gSystem->OpenDirectory(".");
   TRegexp endwithroot("\\.root$");
   TRegexp endwithmap("\\.map$");
   TRegexp endwithlist("\\.histlist$");
   while ( (fname=gSystem->GetDirEntry(dirp)) ) {
      TString sname(fname);
      if (sname.Index(endwithroot) >= 0 || sname.Index(endwithlist) >= 0 ||
         sname.Index(endwithmap) >= 0) {
         if (sname.Index(endwithlist) >= 0 &&
            contains_filenames(fname) <= 0) continue;
         if (fFileSelMask->Length() > 0) {
            TRegexp re((const char *)*fFileSelMask, !fUseRegexp);
            if (sname.Index(re) <0) continue;
         }
         Long_t  id, flags, modtime;
         Long64_t size;
         gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
//         cout << fname << " " << modtime << endl;
         TString cmd = "mypres->Show";
         if (sname.Index(endwithlist) >= 0) {
            cmd = cmd + "List(\"\",\"" + fname + "\")";
         } else {
            cmd = cmd + "Contents(\"" + fname + "\", \"\" )";
         }
        
         TString nam=fname;
         TString tit;
         TString sel;

         if      (!strcmp(how,"A")) 
            fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel));
         else if (!strcmp(how,"R")) 
            fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel, kTRUE));
         else
            fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel,modtime));
      }
   }
   if (fCmdLine->GetSize() <= 0) {
      WarnBox("No files found, check File Selection Mask");
   } else {
      fCmdLine->Sort();
      filelist = CommandPanel("Filelist",fCmdLine, 5, 430);
   }
//   fCmdLine->Print();
   fCmdLine->Delete();
//   fcmd->Delete();
//   ftit->Delete();
};
//________________________________________________________________________________________

void HistPresent::ListMacros(const char *bp)
{
//   if (fControlBar) {
//      if (fControlBar->TestBit(TObject::kNotDeleted) &&
//          !fControlBar->TestBit(0xf0000000)) delete fControlBar; 
//      fControlBar=0;
//   }
   Bool_t first = kTRUE;
   const char *fname;
   void* dirp=gSystem->OpenDirectory(".");
   ifstream wstream;
   TString wline;
   TRegexp dotC = "\\.C$";   
   while ( (fname=gSystem->GetDirEntry(dirp)) ) {
      TString sname(fname);
      if (sname.Index(dotC)>0) {
//         cout << fname << endl;
         wstream.open(fname, ios::in);
	      if (!wstream.good()) {
		      cerr	<< "ListMacros: "<< gSystem->GetError() << " - " << fname
				   << endl;
            wstream.close();
		      continue;
	      }
	      wline.ReadLine(wstream, kFALSE);
	      wline.Strip();
         Int_t len =wline.Length();
	      Int_t n = wline.Index("Hint:");
         if (len > 0 && n >= 0) { 
            n+=5;
//            if (!fControlBar)
            if (first) {
               fControlBar = new TControlBar("vertical", "Macros", 5,380);
               first = kFALSE;
            }
            TString cmd =".x "; cmd += fname;
            TString hint = wline(n,len);  
            fControlBar->AddButton(fname, cmd.Data(), hint.Data());
         }
         wstream.close();
      }
   }
   if (fControlBar) {fControlBar->Show(); 


gROOT->SaveContext();}
   else           cout << " No macro containing a hint found " << endl;
};
//________________________________________________________________________________________

void HistPresent::TurnButtonGreen(TVirtualPad ** pad)
{
   if (*pad && (*pad)->TestBit(TObject::kNotDeleted) 
           && !((*pad)->TestBit(0xf0000000)) )
   { 
      if (strcmp((*pad)->ClassName(), "TButton")) {
         cout << "No longer a TButton, but " << (*pad)->ClassName() << endl;
      } else {
         (*pad)->SetFillColor(0); 
         (*pad)->Modified(kTRUE);
         (*pad)->Update();
      } 
   } 
   gPad->SetFillColor(3);
//   cout << " Green" << setbase(16) << gPad << endl;
   *pad=gPad;
}
//________________________________________________________________________________________

void HistPresent::ShowContents(const char *fname, const char * dir, const char* bp)
{
   static Int_t ycanvas=5;
   TMrbStatistics * st = NULL;
   TList lofF;
   TList lofW1;
   TList lofW2;
   TList lofT;
   TList lofC;
   TList lofUc;
   TList lofG;
   Int_t nstat=0;
   Bool_t ok;
   TString cmd;
   TString title;
   TString hint;
   TString sel;
   Int_t maxkey = 0;
   if (fMaxListEntries > 500) {
      cout << "Warning: Max number of entries in list of histograms set to: 500" 
           << endl;
      fMaxListEntries = 500;
   }
//   TurnButtonGreen(&activeFile);

   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
      if (b) {
        b->SetBit(kSelected);
        b->SetFillColor(3);
        b->Modified(kTRUE);b->Update();
      }
   }
   void* dirp=gSystem->OpenDirectory(".");
   const char * fn;
   TString suffix(fHlistSuffix);
   suffix += "$";
   TRegexp rsuf(suffix);
   while ( (fn=gSystem->GetDirEntry(dirp)) ) {
      hint = fn;
      if (hint.Index(rsuf) > 0) {
			if (contains_filenames(fn) > 0) continue;
		   if (strstr(fname,".root")) {
			   if (FindHistsInFile(fname, fn) <= 0) {
//				   cout << "No hist of: " << fn << " found in: " << fname << endl;
					continue;
				}
			}
         hint(rsuf) = "";
         cmd = "mypres->ShowList(\"";
         cmd = cmd + fname + "\",\"" + hint + "\")";
         title = "ShowList "; title += hint;
         sel.Resize(0);
         fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
      }
   }
   if (fCmdLine->GetSize() > 0) fCmdLine->Sort();

   if (fRootFile) fRootFile->Close();

   Int_t anything_to_delete = 0;
   if (strstr(fname,".root")) {
      TFile * rfile = NULL;
      rfile = new TFile(fname);
      if ( (fixnames(&rfile, kTRUE) )) {
         if (QuestionBox("File contains objects with illegal names\n\
(see transcript output)\n\
Should we create a new file with corrected names?", maincanvas)) { 
            fixnames(&rfile, kFALSE);
//               rfile->ls();
            TString mess("A new file: ");
            mess += rfile->GetName();
            mess += "\nhas been created\n Please redisplay filelist";
            InfoBox(mess.Data(), maincanvas); 
            rfile->Close();
            return;
         }
      }   
      if (fShowListsOnly > 0) {  
         st = NULL;
         nstat = 0;
         cout << "Skip hists in file, show hist lists only" << endl;
      } else {
         if (strlen(dir) > 0) rfile->cd(dir);
//         gDirectory->ls();
      	st = (TMrbStatistics*)gDirectory->Get("TMrbStatistics");
      	if (!st) {
         	st=new TMrbStatistics(fname);
         	nstat = st->Fill(gDirectory);
      	} else nstat = st->GetListOfEntries()->GetSize();
         // look for directory entries
   		TIter next(gDirectory->GetListOfKeys());
   		TKey* key;
   		while( (key = (TKey*)next()) ){
      		if(!strncmp(key->GetClassName(),"TDirectory",10)){
            	cmd = "mypres->ShowContents(\"";
            	cmd = cmd + fname + "\",\"";
               if (strlen(dir) > 0) cmd = cmd + "/" + dir;
               cmd = cmd + key->GetName() + "\")";
            	sel = "";
            	title = "Dir: ";
               title += key->GetName();
               hint = title;
            	fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            }
         }
      }
      maxkey = TMath:: Max(GetObjects(lofT, gDirectory, "TTree"),        maxkey);
      maxkey = TMath:: Max(GetObjects(lofT, gDirectory, "TNtuple"),      maxkey);
      maxkey = TMath:: Max(GetObjects(lofF, gDirectory, "TF1"),          maxkey);
      maxkey = TMath:: Max(GetObjects(lofC, gDirectory, "TCanvas"),      maxkey);
      maxkey = TMath:: Max(GetObjects(lofC, gDirectory, "HTCanvas"),      maxkey);
      maxkey = TMath:: Max(GetObjects(lofG, gDirectory, "TGraph"),       maxkey);
      maxkey = TMath:: Max(GetObjects(lofUc, gDirectory, "FhContour"),   maxkey);
      maxkey = TMath:: Max(GetObjects(lofW1, gDirectory, "TMrbWindowF"), maxkey);
      maxkey = TMath:: Max(GetObjects(lofW1, gDirectory, "TMrbWindowI"), maxkey);
      maxkey = TMath:: Max(GetObjects(lofW2, gDirectory, "TMrbWindow2D"),maxkey);
      rfile->Close();
   } else if (strstr(fname,"Socket")) {
      if (!fComSocket) {
         if (!fConnectedOnce) {
            *fHostToConnect = GetString("Host to connect", fHostToConnect->Data()
                        ,&ok,maincanvas);
            fSocketToConnect = GetInteger("Socket to connect",fSocketToConnect
                        ,&ok,maincanvas);
            fConnectedOnce = kTRUE;
         }
         fComSocket = new TSocket(*fHostToConnect, fSocketToConnect);
         if (!fComSocket->IsValid()) {
            fComSocket->Close();
            fComSocket = 0;
            cout << setred << 
            "Cant connect to remote M_analyze" << setblack << endl;
            return;
         } else {  
//           Wait till we get the start message
            TMessage * mess;
            fComSocket->Recv(mess);
            if ( mess->What() == kMESS_STRING ) {
            	char str[30];
            	mess->ReadString(str, 25);
            	TString  smess = str; smess = smess.Strip(TString::kBoth);
            	if (smess(0,6) != "accept") {               
               	cerr << "Connection not accepted, message: " 
			      	<< setred << smess << setblack << endl;
               	fComSocket->Close();
               	fComSocket = 0;   
               	if (mess) delete mess;
               	return;
            	}
            }
         }
      }
      if (fShowListsOnly <= 0) {  
      	st = getstat(fComSocket);
      	if (!st) {
         	cout << setred << "Cant get stat, Connection lost?" << setblack << endl;
         	fComSocket->Close("force");
         	fComSocket = NULL;
         	return;
      	} else {
         	nstat = st->GetListOfEntries()->GetSize();
      	}
      } else {
         st = NULL;
         nstat = 0;
          cout << " Skip getstat(fComSocket)" << endl;
      }
   } else if (strstr(fname,"Memory")) {
      st=new TMrbStatistics();
      nstat = st->Fill();
   } else if (strstr(fname,".map")) {
      TMapFile * mfile =0;
      mfile = TMapFile::Create(fname);
      st = (TMrbStatistics*)mfile->Get("TMrbStatistics");
      if (!st) {
         st=new TMrbStatistics(fname);
         nstat = st->Fill(mfile);
      } else nstat = st->GetListOfEntries()->GetSize();
//      lofF = GetFunctions(mfile);
//      lofT = GetTrees(rfile);
//      lofW = GetWindows(mfile);
     mfile->Close();


     if (nstat > 0) {
         cmd = "mypres->SaveMap(\"";
         cmd = cmd + fname + "\")";
         hint = "Save as ROOT file ";
         title = "Save";
         fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
      }
        
   } else {
       WarnBox("Unknown file");
       return;
   } 
//   if (nstat > fMaxListEntries) { 
//      cout << setred << "Too many entries in list: " << nstat << endl;
//      cout << "this might crash X, please use selection mask"<< endl;
//      cout << "to reduce number of entries below: " <<  fMaxListEntries  << endl;
 //     cout << "WARNING: Only histlists will be shown" << setblack << endl;
//   } else if (nstat <= 0) {
//      WarnBox("Nothing found, Check Hist Selection Mask");
//   }
//  list of 1 and 2 dim, histograms
//   if (nstat > 0 && nstat <= fMaxListEntries) { 
   Int_t not_shown = 0;
   if (nstat > 0) { 
      
      TMrbStatEntry * stent;
      TIter nextentry(st->GetListOfEntries());
      while ( (stent = (TMrbStatEntry*)nextentry()) ) {
         if (fHistSelMask->Length() > 0) {
            TString tn(stent->GetName());
            if        (fHistSelOp == kHsOp_None) {
               TRegexp re((const char *)*fHistSelMask, !fUseRegexp);
               if (tn.Index(re) <0) continue;
            } else if (fUseRegexp) {
               TRegexp re1((const char *)*fHistSelMask_1);
               TRegexp re2((const char *)*fHistSelMask_2);
               if (fHistSelOp == kHsOp_And) {
                  if (tn.Index(re1) < 0 || 
                     tn.Index(re2) < 0) continue;
               } else if (fHistSelOp == kHsOp_Or) {
                  if (tn.Index(re1) < 0 && 
                     tn.Index(re2) < 0) continue;
               } else if (fHistSelOp == kHsOp_Not) {
                  if (( fHistSelMask_1->Length() > 0 &&
                     tn.Index(re1) < 0) || 
                    (tn.Index(re2) >=0)) continue;
               }
            } else {           
               if (fHistSelOp == kHsOp_And) {
                  if (!tn.Contains(fHistSelMask_1->Data()) || 
                     !tn.Contains(fHistSelMask_2->Data())) continue;
               } else if (fHistSelOp == kHsOp_Or) {
                  if (!tn.Contains(fHistSelMask_1->Data()) && 
                     !tn.Contains(fHistSelMask_2->Data())) continue;
               } else if (fHistSelOp == kHsOp_Not) {
                  if (( fHistSelMask_1->Length() > 0 &&
                    !tn.Contains(fHistSelMask_1->Data())) || 
                     (tn.Contains(fHistSelMask_2->Data()))) continue;
               }
            }
         }
         cmd = fname;
         cmd = cmd + "\",\"";
         cmd += dir;  
         cmd = cmd + "\",\"" + stent->GetName();
         if (stent->GetCycle() > 0) {
            cmd += ";";
            cmd += stent->GetCycle();
         } 
         cmd += "\")";
         sel = cmd; 
         cmd.Prepend("mypres->ShowHist(\"");
         sel.Prepend("mypres->SelectHist(\"");

//         cout << cmd << endl;

         title = stent->GetName();
         if (stent->GetCycle() > 1) {
            title += ";";
            title += stent->GetCycle();
         }   
         hint = title;
         if     ( stent->GetDimension() == 1  )title.Prepend("1d ");
         else if (stent->GetDimension() == 2 )title.Prepend("2d ");
         else                               title.Prepend("3d ");
         title +=  " " ;
         title +=  (Int_t)stent->GetEntries();

         hint = stent->GetName();
         if (stent->GetDimension() == 1)     hint += " 1d hist Ent: ";
         else if (stent->GetDimension() == 2 )hint += " 2d hist Ent: ";
         else                               hint += " 3d hist Ent: ";
         hint += " "; 
         hint += (Int_t)stent->GetEntries(); 
         hint += " Title: ";
         hint += stent->GetTitle();
         TString tmp = fname;
         tmp = tmp + " " + stent->GetName();
//         TObjString tobjs(tmp.Data());
         if (fSelectHist->Contains(tmp.Data())) {
            cout << "Already selected: " << tmp.Data() << endl;
            sel += " YES";
         }
         if (fCmdLine->GetSize() < fMaxListEntries) { 
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         } else {
            if (not_shown <= 0){
               cout << setred << "Too many entries in list: " << nstat << endl;
               cout << "this might crash X, please use selection mask"<< endl;
               cout << "to reduce number of entries below: " <<  fMaxListEntries  << endl;
               cout << "On your own risk you may increase value: " << fMaxListEntries << endl;
               cout << "WARNING: not all hists will be shown" << setblack << endl;
            }
            not_shown++;
//            cout << "Not shown: " << stent->GetName() << endl;
         }
      }
   }
   if (not_shown > 0) cout << "Another: " << not_shown << " hists are not shown" << endl;
   if (fHistSelMask->Length() <=0) {
//  windows
      if (lofW1.GetSize() > 0) {
         TIter next(&lofW1);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            TString name(title); 
            cmd = fname;
            cmd = cmd + "\",\"" + name.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->PrintWindow(\"");
            sel.Prepend("mypres->LoadWindow(\"");
            title.Prepend("w1 ");
            hint =  title;
            hint+=" 1-dim window";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         }
      }
      if (lofW2.GetSize() > 0) {
         TIter next(&lofW2);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            TString name(title); 
            cmd = fname;
            cmd = cmd + "\",\"" + name.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->PrintWindow(\"");
            sel.Prepend("mypres->LoadWindow(\"");
            title.Prepend("w2 ");
            hint =  title;
            hint+=" 2-dim window";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
         }
         title = "2D Cuts to ASCII";
         hint = "Write " + title;
         cmd = "mypres->CutsToASCII(\"";
         cmd = cmd + fname + "\")";
         sel = "";
         fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
         anything_to_delete++; 
      }
   //  functions
      if (lofF.GetSize() > 0) {
         TIter next(&lofF);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            cmd = fname;
            cmd = cmd + "\",\"" + title.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->ShowFunction(\"");
            sel.Prepend("mypres->LoadFunction(\"");
   //         sel.Resize(0);
            title.Prepend("f ");
            hint =  title;
            hint+=" function";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         }
      }
   //  canvases
      if (lofC.GetSize() > 0) {
         TIter next(&lofC);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            cmd = fname;
            cmd = cmd + "\",\"" + title.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->ShowCanvas(\"");
            sel.Resize(0);
            title.Prepend("c ");
            hint =  title;
            hint+=" canvas";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         }
      }
   //  user contours
      if (lofUc.GetSize() > 0) {
         TIter next(&lofUc);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            cmd = fname;
            cmd = cmd + "\",\"" + title.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->ShowContour(\"");
            sel.Prepend("mypres->SelectContour(\"");
//            sel.Resize(0);
            title.Prepend("U_C ");
            hint =  title;
            hint+=" contour";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         }
      }
   //  graphs
      if (lofG.GetSize() > 0) {
         TIter next(&lofG);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            cmd = fname;
            cmd = cmd + "\",\"" + title.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->ShowGraph(\"");
            sel.Prepend("mypres->SelectGraph(\"");
//            sel.Resize(0);
            title.Prepend("Graph ");
            hint =  title;
            hint+=" graph";
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
            anything_to_delete++; 
         }
      }
   //  trees
      if (lofT.GetSize() > 0) {
         TIter next(&lofT);
         TObjString * objs;
         while ( (objs = (TObjString*)next())) {
            title = objs->String();
            cmd = fname;
            cmd = cmd + "\",\"" + title.Data() + "\")";
            sel = cmd;
            cmd.Prepend("mypres->ShowTree(\"");
            sel.Resize(0);;
            hint =  title;
            title.Prepend("Tree: ");
            fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
         }
      }
   }
//
   if (anything_to_delete > 0) {
   	if (maxkey > 1) {
   		cmd = "mypres->PurgeEntries(\"";
   		cmd = cmd + fname +  "\")";
   		title = "Purge Entries";
   		hint = "Purge: delete entries, keep last cycl";
   		sel.Resize(0);
   		fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
   	}
   	cmd = "mypres->DeleteSelectedEntries(\"";
   	cmd = cmd + fname +  "\")";
   	title = "Delete Sel Entries";
   	hint = "Delete selected entries";
   	sel.Resize(0);
   	fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
   }
   cmd = "mypres->ShowStatOfAll(\"";
   cmd = cmd + fname + "\",\"" + dir + "\")";
   title = "Show Stats of all";
   hint = "Show statistics of all histograms and save to file";
   sel.Resize(0);
   fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
   if (strstr(fname,"Socket")) { 
      cmd = "mypres->SaveFromSocket(\"";
      cmd = cmd + fname + "\")";
      hint = "Save all hists to a local ROOT file ";
      title = "Save to local ROOT file";
      fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
   }
//
   if (fCmdLine->GetSize() > 0) {
      if (fCmdLine->GetSize() > 3) {
         cmd = "mypres->ComposeList()";
         title = "Compose list";
         hint = "Select histograms to be added to list first";
         sel.Resize(0);
         fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
      }
//      CheckList(fHistLists, "HTCanvas");
      TObject *obj =  fHistLists->FindObject(fname);
      if (obj) {
         cout << "Delete canvas: " << fname << endl;
         fHistLists->Remove(obj);
         delete obj;
      }
         ycanvas = 5 + 50 * fHistLists->GetSize();
         TString cmd_title(fname);
         if (strlen(dir) > 0) cmd_title = cmd_title + "_" + dir;
         HTCanvas *ccont = CommandPanel(cmd_title.Data(), fCmdLine, 
                           260, ycanvas, this, fWinwidx_hlist);
         if (fHistLists)fHistLists->Add(ccont);
   }
   fCmdLine->Delete();
   if (st) delete st;
   gDirectory=gROOT;
}
//________________________________________________________________________________________

void HistPresent::PurgeEntries(const char * fname, const char * bp)
{
   TString question(fname);
   question.Prepend("Purge file: ");
   if (QuestionBox(question.Data(), GetMyCanvas()) == kMBYes) {
      TFile * f = new TFile(fname, "update");
      f->Purge();
      f->Close();
   }
}
//________________________________________________________________________________________

void HistPresent::DeleteSelectedEntries(const char * fname, const char * bp)
{
   Int_t ndeleted = 0;
   ndeleted += DeleteOnFile(fname, fSelectCut, GetMyCanvas());
   ndeleted += DeleteOnFile(fname, fSelectHist, GetMyCanvas());
   ndeleted += DeleteOnFile(fname, fSelectWindow, GetMyCanvas());
   ndeleted += DeleteOnFile(fname, fSelectContour, GetMyCanvas());
   ndeleted += DeleteOnFile(fname, fAllFunctions, GetMyCanvas());
   ndeleted += DeleteOnFile(fname, fSelectGraph, GetMyCanvas());
   cout << ndeleted << " object";
   if (ndeleted !=1) cout << "s";
   cout << " deleted on " << fname << endl;
}
 //________________________________________________________________________________________

void HistPresent::ShowStatOfAll(const char * fname, const char * dir, const char * bp)
{
  Int_t nstat;
   TString sname(fname);
   TMrbStatistics * st = 0;
   TRegexp rname("\\.root");
   TRegexp mname("\\.map");

   if (sname == "Socket") {
      if (!fComSocket) return;
      st = getstat(fComSocket);
      if (!st) {
         WarnBox(" cant get stat(fComSocket)");
         cout << setred << "Cant get stat, Connection lost?" << setblack << endl;
         fComSocket->Close("force");
         fComSocket = NULL;
         return;
      }
   } else if (sname.Index(rname) > 0) {
      sname(rname) = "";      
      TFile * rfile =0;
      rfile = new TFile(fname);

      if (strlen(dir) > 0) rfile->cd(dir);
      st = (TMrbStatistics*)gDirectory->Get("TMrbStatistics");
      if (!st) {
         TString sname(fname);
         if (strlen(dir) > 0) sname = sname + " Dir: " + dir;
         st=new TMrbStatistics(sname);
         nstat = st->Fill(gDirectory);
      } else nstat = st->GetListOfEntries()->GetSize();
      if (rfile )rfile->Close();
   } else if (sname.Index(mname) > 0) {
      sname(mname) = "";      
      TMapFile * mfile =0;
      mfile = TMapFile::Create(fname);
      st = (TMrbStatistics*)mfile->Get("TMrbStatistics");
      if (!st) {
         st=new TMrbStatistics(fname);
         nstat = st->Fill(mfile);
      } else nstat = st->GetListOfEntries()->GetSize();
   } else {
       cout << "Unkown suffix in : " << fname << endl;
       return;
   }
   if (st) {
      sname += ".stat";
      ofstream output(sname.Data());
      st->Print(output);
      output.close();
      TString EditCmd("nedit ");
      EditCmd += sname.Data();
      EditCmd += "&";sname.Data();
      gSystem->Exec(EditCmd.Data());
      cout << "Statistic saved  to: " << sname.Data() << endl;
      delete st;
   }
   gDirectory = gROOT;
}
//________________________________________________________________________________________
  
void HistPresent::ComposeList(const char* bp)
{
   Int_t nselect=fSelectHist->GetSize();
   if (nselect < 1) {
      WarnBox("No selection");
      return;
   }
   TString fname  = ((TObjString *)fSelectHist->At(0))->String();
   Int_t pp = fname.Index(" ");
   fname.Resize(pp); 
   Bool_t put_file = kFALSE;
   TString listname  =  fname;
   if      (fname.Contains(".root")) {
      TRegexp rname("\\.root");
      listname(rname) = fHlistSuffix;
   } else if (fname.Contains(".map")) {
      TRegexp rname("\\.map");
      listname(rname) = fHlistSuffix;
   }
   Bool_t ok;
   listname=GetString("Name of list:",listname.Data(), &ok, GetMyCanvas());
   if (!ok) return;
//   listname += fHlistSuffix;
   if (!gSystem->AccessPathName(listname)) {
      TString question=listname;
      question += " exists, Overwrite?";

      if (QuestionBox(question.Data(), GetMyCanvas()) == kMBNo) return;
   }
   ofstream wstream;
   wstream.open(listname.Data(), ios::out);
	if (!wstream.good()) {
   cout	<< "Error in open file: "
		<< gSystem->GetError() << " - " << listname.Data()
		 << endl;
	   return;
	}
   for(Int_t i = 0; i < nselect; i++) {
      TString hname  = ((TObjString *)fSelectHist->At(i))->String();
//		cout << hname << endl;
      Int_t pp = hname.Index(" ");
      hname.Remove(0,pp+1);
		TRegexp vers(";[0-9]*");
		hname(vers) = "";
      if (put_file) wstream << fname.Data() << " "; 
      wstream << hname.Data() << endl;
   }
   wstream.close();
};
//_______________________________ShowList_________________________________________________________
// Show List 
  
void HistPresent::ShowList(const char* fcur, const char* lname, const char* bp)
{
   static Int_t ycanvas=20;
   TString sl = lname;
   if (!sl.Contains(fHlistSuffix)) sl += fHlistSuffix;
   Int_t status = 1;
   Bool_t cfn = kTRUE;
   status = contains_filenames(sl.Data());
   if (status == -1) {
      cout << "Cant open: " << sl.Data() << endl;
      return;
   } else if (status == 0) cfn = kFALSE;

   TString fname = fcur;    
   ifstream wstream;
   wstream.open(sl.Data(), ios::in);
   TH1 *hist=0;
   TMapFile *mfile = 0;
   TString cmd; TString tit; TString sel;
   if (strstr(fname, "Socket")) 
      cout << "Warning: Validity of entries in list are not checked" << endl;
   while ( 1) {
      TString line;
      line.ReadLine(wstream, kFALSE);
	   if (wstream.eof()) {
			wstream.close();
			break;
		}
		line.Strip();
		if (line.Length() <= 0) continue;
		if (line[0] == '#') continue;
      if (cfn) {
         Int_t pp = line.Index(" ");
         if (pp <= 0) continue;
         fname = line(0,pp);
         line.Remove(0,pp+1);             
      }

      cmd = "mypres->ShowHist(\"";
//      if (is_a_file(fname))cmd += "ShowHist(\"";
//      else                cmd += "ShowMap(\"";
      cmd += fname; 
		TString hname(line);
		TString dname;
		Int_t pp = line.Index(" ");
		if (pp > 0) {
		   dname = line;
			hname.Resize(pp);
			dname.Remove(0,pp+1);
		}
		cmd = cmd + "\",\"" + dname + "\",\"";
//      cmd +=  "\",\"\",\""; 
      sel = "mypres->SelectHist";
//      sel = sel + "(\"" + fname + "\",\"\",\"";
      sel = sel + "(\"" + fname + "\",\"" + dname + "\",\"";

      cmd = cmd + hname + "\")";
      sel = sel + hname + "\")";
//		cout << cmd << endl;
//      TString empty;
      tit = line;
      if (is_a_file(fname)) {
//      cout << "fn: " << fname  << " dn: " << dname  << " hn: " << hname << endl;
         if (fRootFile) fRootFile->Close();
         fRootFile=new TFile(fname);
			if (dname.Length() > 0)gDirectory->cd(dname);
         hist = (TH1*)gDirectory->Get(hname);
      } else if  (strstr(fname, ".map")) {
         mfile = TMapFile::Create(fname);
         hist=0;
         hist    = (TH1 *) mfile->Get(hname, hist);

      } else if (strstr(fname, "Socket")) {
//
      }
      if (hist) { 
         if (is2dim(hist))   line.Prepend("2d ");
         else                line.Prepend("1d ");
         line +=  " " ;
         line +=  (Int_t)hist->GetEntries();
         if (is2dim(hist)) tit += " 2d hist Ent: ";
         else                tit += " 1d hist Ent: ";
         tit +=  " " ;
         tit +=  (Int_t)hist->GetEntries();
      } else {
         line.Prepend("?d ");
         line +=  " ??" ;
         tit += " ?d hist Ent: ??";
      }
//      if (fShowTitle) {
//        if (strlen(hist->GetTitle()) > 0)
//        tit = tit + " Title: " + hist->GetTitle();
//      }
      fCmdLine->Add(new CmdListEntry(cmd, line, tit, sel));
      if (is_a_file(fname)) {
         if (fRootFile) fRootFile->Close();
      } else {
         if (hist) delete hist;
         if (mfile) mfile->Close();
      }
   }
   if (fCmdLine->GetSize() <= 0) {
      cout << "No Hists found" << endl;
   } else {
      cmd = "mypres->ShowInOneCanvas()";
      tit = "ShowAllInOneCanvas";
      TString hint = "Show all hists in list in one Canvas";
      sel.Resize(0);
      fCmdLine->AddFirst(new CmdListEntry(cmd, tit, hint, sel));
      cmd = "mypres->StackInOneCanvas()";
      tit = "StackAll";
      hint = "Stack all hists in list in one Canvas";
      sel.Resize(0);
      fCmdLine->AddFirst(new CmdListEntry(cmd, tit, hint, sel));

//      TString cname = fname;
//      cname = cname + "_" +lname; 
      TCanvas *ccont = CommandPanel(sl.Data(), fCmdLine, 260, ycanvas, this);
      fHistLists->Add(ccont);
      ycanvas += 50;
      if (ycanvas >= 500) ycanvas=5;
   } 
   fCmdLine->Delete(); 
   gDirectory=gROOT;
}
//________________________________________________________________________________________
// Show List 
  
void HistPresent::StackInOneCanvas(const char *bp)
{
   TList * hlist = BuildList(bp);
	if (hlist) {
      StackSelectedHists(hlist, hlist->GetName());
      hlist->Delete();
   }
}
//________________________________________________________________________________________
// Show List 
  
void HistPresent::ShowInOneCanvas(const char *bp)
{
   TList * hlist = BuildList(bp);
	if (hlist) {
      ShowSelectedHists(hlist, hlist->GetName());
      hlist->Delete();
   }
}
//________________________________________________________________________________________
// Show Tree 
  
void HistPresent::ShowTree(const char* fname, const char* tname, const char* bp)
{
  static Int_t ycanvas=5;
//  const Int_t MAXLEAF=33;
  if (fRootFile) fRootFile->Close();
  fRootFile=new TFile(fname);
  fRootFile->ls();
  TTree *tree = (TTree*)fRootFile->Get(tname);
   TObjArray *leaves = tree->GetListOfLeaves();
   Int_t nleaves = leaves->GetEntriesFast();
   if (nleaves <=0) {
       cout << "No leaves" << endl;
       return;
   }

   fSelectLeaf->Delete();
   *fLeafCut="";
//   TButton *button;
   Int_t len, l;
//   TLeaf *leafcount;
   char *bname;
   char branchname[100];
   TString cmd;
   TString nam;
   TString empty;
   TString sel;

   cmd = "mypres->EditExpression()";
   nam = "Edit expression";
   sel = "mypres->ToggleExpression()";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));

   cmd = "mypres->EditLeafCut()";
   nam = "Edit formula cut";
   sel = "mypres->ToggleLeafCut()";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));


   cmd = "mypres->DefineGraphCut()";
   nam = "Define graphical cut";
   sel = "mypres->ToggleGraphCut()";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));

   cmd = "mypres->UseHist()";
   nam = "Use selected hist";
   sel = "mypres->ToggleUseHist()";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));

   cmd = "mypres->ShowLeaf(\"";
   cmd = cmd + fname + "\",\"" + tname  + "\",\"\")";
   nam = "Show leafs";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, empty));

   TString cmd_base("mypres->ShowLeaf(\"");
   TString sel_base("mypres->SelectLeaf(\"");
   cmd_base = cmd_base + fname + "\",\"" + tname + "\",\"";
   for (l=0;l<nleaves;l++) {
      TLeaf *leaf = (TLeaf*)leaves->UncheckedAt(l);
      TBranch *branch = leaf->GetBranch();
      strcpy(branchname,branch->GetName());
      bname = branchname;
      while ( *bname) { if (*bname == '.') *bname='_'; bname++; }
      if (branch->IsA() == TBranchObject::Class()) {
      } else {
         len = leaf->GetLen();
 //        leafcount =leaf->GetLeafCount();
         if (len > 1) {
           for (Int_t ix = 0; ix < len; ix++) {

 //             sprintf(lname,"%s[%d]", branchname,ix);
              cmd = cmd_base + Form("%s[%d]",leaf->GetName(), ix) + "\")";
              sel = sel_base + Form("%s[%d]",leaf->GetName(), ix) + "\")";
              nam = Form("%s[%d]",leaf->GetName(), ix);
              fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));
           }
         } else {
//            if (len > 1) sprintf(lname,"%s[%d]",     branchname,len);
//            else         sprintf(lname,"%s",         branchname);
//            sprintf(lname,"%s",         branchname);
            cmd = cmd_base + leaf->GetName() + "\")";
            sel = sel_base + leaf->GetName() + "\")";
            nam = leaf->GetName();
            fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));

         }
//         cmd = "mypres->ShowLeaf(\"";
//         cmd = cmd + fname + "\",\"" + tname + "\",\"" + lname + "\")";
//         cout << cmd << endl;
//         sel = "mypres->SelectLeaf(\"";
//         sel = sel  + lname + "\")";
//         cmd = cmd_base + lname + "\")";
 //        sel = sel_base + lname + "\")";
 //        nam = lname;
 //        fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));
     }
   }
   fRootFile->Close();
   CommandPanel("TreeList", fCmdLine, 245, ycanvas, this);
   ycanvas += 50;
   if (ycanvas >= 500) ycanvas=5;
   fCmdLine->Delete(); 

}
//________________________________________________________________________________________
  
void HistPresent::SelectLeaf(const char* lname, const char* bp)
{
   if (!bp) return;
   cout << " Adding leaf " << lname << endl;
   TString sel = lname;
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
   if (b->TestBit(kSelected)) {
      TObjString tobjs((const char *)sel);
      cout << "Remove leaf";
      tobjs.Print(" ");
      fSelectLeaf->Remove(&tobjs);
      b->SetFillColor(16);
      b->ResetBit(kSelected);
   } else {
      cout << " Add leaf " << lname << endl;
      fSelectLeaf->Add(new TObjString((const char *)sel));     
      b->SetFillColor(3);
      b->SetBit(kSelected);
   }
   b->Modified(kTRUE);b->Update();
}
//________________________________________________________________________________________

void HistPresent::ToggleExpression(const char* bp)
{
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
      if (b->TestBit(kSelected)) {
         cout << "set use expression =kFALSE"<< endl;
         fApplyExpression = kFALSE;
         b->SetFillColor(16);     
         b->ResetBit(kSelected);
      } else {
         cout << "set use expression =kTRUE"<< endl;
         fApplyExpression = kTRUE;
         b->SetFillColor(3);
         b->SetBit(kSelected);
      }
   }
}
//________________________________________________________________________________________
  
void HistPresent::UseHist(const char* bp)
{
   ShowContents("Memory", "");
   if (!fUseHist) ToggleUseHist();
}
//________________________________________________________________________________________
  
void HistPresent::ToggleUseHist(const char* bp)
{
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
      if (b->TestBit(kSelected)) {
         cout << "set UseHist = kFALSE"<< endl;
         fUseHist = kFALSE;
         b->SetFillColor(16);     
         b->ResetBit(kSelected);
      } else {
         cout << "set UseHist= kTRUE"<< endl;
         fUseHist = kTRUE;
         b->SetFillColor(3);
         b->SetBit(kSelected);
      }
   }
}
//________________________________________________________________________________________
  
void HistPresent::ToggleLeafCut(const char* bp)
{
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
      if (b->TestBit(kSelected)) {
         cout << "set formula cut =kFALSE"<< endl;
         fApplyLeafCut = kFALSE;
         b->SetFillColor(16);     
         b->ResetBit(kSelected);
      } else {
         if (fApplyGraphCut) {
            WarnBox("Cant have formula and graphical cut simultanously");
            return;
         }         
         cout << "set formula cut =kTRUE "<< endl;
         fApplyLeafCut = kTRUE;
         b->SetFillColor(3);
         b->SetBit(kSelected);
      }
   }
}
//________________________________________________________________________________________
  
void HistPresent::ToggleGraphCut(const char* bp)
{
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
      if (b->TestBit(kSelected)) {
         cout << "set graphical cut =kFALSE"<< endl;
         fApplyGraphCut = kFALSE;
         b->SetFillColor(16);     
      } else {
         if (fApplyLeafCut) {
            WarnBox("Cant have formula and graphical cut simultanously");
            return;
         }         
         cout << "set graphical cut =kTRUE"<< endl;
         fApplyGraphCut = kTRUE;
         b->SetFillColor(3);
      }
   }
}
//________________________________________________________________________________________
  
void HistPresent::GetFileSelMask(const char* bp)
{
    Bool_t ok;
    *fFileSelMask = GetString(
    "Edit File Selection Mask",(const char *)*fFileSelMask, &ok, maincanvas);
}
//________________________________________________________________________________________
  
void HistPresent::GetHistSelMask(const char* bp)
{

    Bool_t yesno = kFALSE;
    if (fUseRegexp) yesno=kTRUE;
    Bool_t ok;
    *fHistSelMask=GetString(
                  "Edit Hist Selection Mask",(const char *)*fHistSelMask, &ok,
                  maincanvas, "Use Regexp syntax", &yesno, 
                  Help_SelectionMask_text);
     if (!ok) return;
     if (yesno) fUseRegexp = 1;
     else         fUseRegexp = 0;
//    if (!fUseRegexp) {
      if      (fHistSelMask->Contains("&")) {
         fHistSelOp = kHsOp_And;
         Int_t indop = fHistSelMask->Index("&");
         *fHistSelMask_1 = (*fHistSelMask)(0, indop);
         *fHistSelMask_2 = (*fHistSelMask)(indop+1, fHistSelMask->Length());         
      } else if (fHistSelMask->Contains("|")) {
         fHistSelOp = kHsOp_Or;
         Int_t indop = fHistSelMask->Index("|");
         *fHistSelMask_1 = (*fHistSelMask)(0, indop);
         *fHistSelMask_2 = (*fHistSelMask)(indop+1, fHistSelMask->Length());         
      } else if (fHistSelMask->Contains("!")) {
         fHistSelOp = kHsOp_Not;
         Int_t indop = fHistSelMask->Index("!");
         *fHistSelMask_1 = (*fHistSelMask)(0, indop);
         *fHistSelMask_2 = (*fHistSelMask)(indop+1, fHistSelMask->Length());         
      } else                                 fHistSelOp = kHsOp_None;
      *fHistSelMask_1 = fHistSelMask_1->Strip(TString::kBoth);
      *fHistSelMask_2 = fHistSelMask_2->Strip(TString::kBoth);
//      cout << fHistSelOp  << " " << "|" <<
//      fHistSelMask_1->Data() << "|" << " "<< "|"  << fHistSelMask_2->Data() << "|" << endl;
//   }   
}
//________________________________________________________________________________________
  
void HistPresent::EditExpression(const char* bp)
{
   Bool_t ok;
   *fExpression=GetString("Edit expression",(const char *)*fExpression,
                          &ok, GetMyCanvas());
   if (!ok) return;
   if (strlen(*fExpression) > 1) {
      cout << *fExpression<< endl;
      cout << "Please press grey button to activate" << endl;
//       if (!fApplyExpression)ToggleExpression();
    }   
}
//________________________________________________________________________________________
  
void HistPresent::EditLeafCut(const char* bp)
{

   Bool_t ok;
    *fLeafCut=GetString("Edit formula cut",(const char *)*fLeafCut,
                         &ok, GetMyCanvas());
   if (!ok) return;
    if (strlen(*fLeafCut) > 1) {
       
//       if (!fApplyLeafCut)ToggleLeafCut();
       cout << *fLeafCut << endl;
       cout << "Please press grey button to activate" << endl;
    } 
}
//________________________________________________________________________________________
  
void HistPresent::DefineGraphCut(const char* bp)
{
   Bool_t ok;
   *fGraphCut=GetString("Cut name",(const char *)*fGraphCut, &ok, GetMyCanvas());
   if (!ok) return;
   if (strlen(*fGraphCut) > 1) {
      TCutG* cc=(TCutG*)gROOT->FindObject(fGraphCut->Data());
      if (!cc) {
         WarnBox("Cut not found in memory");
         return;
      }
      *fCutVarX = cc->GetVarX();
      *fCutVarX=GetString("X-value name",fCutVarX->Data(), &ok, GetMyCanvas());
      cc->SetVarX(fCutVarX->Data());
      *fCutVarY = cc->GetVarY();
      *fCutVarY=GetString("Y-value name",fCutVarY->Data(), &ok, GetMyCanvas());
      cc->SetVarY(fCutVarY->Data());
      if (!fApplyGraphCut) ToggleGraphCut();
      cout << "Enable: " << *fGraphCut << endl;
   } else {
      if (fApplyGraphCut) ToggleGraphCut();
   }   
//  svalues->Delete();
//  delete svalues;
}
//________________________________________________________________________________________

void HistPresent::ShowLeaf( const char* fname, const char* tname,
                            const char* leafname, const char* bp)
{

   TString leaf0;
   TString leaf1;
   TString leaf2;
   TString cmd;
   TString hname = "hist_";
   Int_t nent;
   static Double_t nbin[3] = {0, 0, 0};
   static Double_t vmin[3] = {0, 0, 0};
   static Double_t vmax[3] = {0, 0, 0};
   cout << "leafname " << leafname << " " << strlen(leafname) 
    << " nent " << fSelectLeaf->GetSize() << endl;
   if (strlen(leafname)>0) {
      leaf0=leafname;
      cmd=leaf0;
      nent = 1;
   } else {
      if (fSelectLeaf->IsEmpty() && !fApplyExpression) {
         WarnBox("Please select a leaf");
         return;
      }
      nent = fSelectLeaf->GetSize();
      if (nent > 3) {
         WarnBox("More the 3 leafs selected");
         return;
      }
      if (nent > 0) {
         TObjString *objs;
         TIter next(fSelectLeaf);
         objs = (TObjString *)next();
         cmd = objs->String();
         leaf0 = cmd;
         if (nent > 1) {
            objs = (TObjString *)next();
            leaf1 = objs->String();
   //         cmd += ":";
   //         cmd  += leaf1;
            cmd.Prepend(":");
            cmd.Prepend(leaf1.Data());
         }
         if (nent == 3) {
            objs = (TObjString *)next();
            leaf2 = objs->String();
   //         cmd += ":";
   //         cmd += leaf2;
            cmd.Prepend(":");
            cmd.Prepend(leaf2.Data());
         }
      }
   }
   const char * lname[3];
   lname[0] = leaf0.Data();
   lname[1] = leaf1.Data();
   lname[2] = leaf2.Data();
   const char * cchname; 
//   TString hname = "hist_";
    cout << "nent " << nent << endl;
   if (nent > 0) hname += leaf0;
   else hname += "userdef";
   if (nent > 1) hname += "_"; hname += leaf1;
   if (nent > 2) hname += "_"; hname += leaf2;

   if (fApplyExpression) {
      cmd = *fExpression;
      TRegexp a1("\\$1");
      while (cmd.Index(a1)>=0) {cmd(a1)=leaf0.Data();}
      TRegexp a2("\\$2");
      if (cmd.Index(a2)>=0) {
         if (nent <2) {
            WarnBox("2 args expected, only 1 given");
            return;
         } else {while (cmd.Index(a2)>=0) {cmd(a2)=leaf1.Data();}}
      }  
      TRegexp a3("\\$3");
      if (cmd.Index(a3)>=0) {
        if (nent <3) {
           WarnBox("3 args expected");
           return;
        } else {while (cmd.Index(a3)>=0) {cmd(a3)=leaf2.Data();}}
      }  
   }
   TString option = "goff";
   cmd += ">>";
//  look if a histogram is defined to be used instead of a default
   TH1* hist1 =0;
   if (fUseHist && nent !=3) {
      Int_t nselect=fSelectHist->GetSize();
      if (nselect != 1) {
         WarnBox("Please select exactly 1 histogram");
         return;
      }
      hist1 = GetSelHistAt(0);
      if (!hist1) {WarnBox("Histogram not found");return;};
      if (is2dim(hist1) && nent != 2) {
         WarnBox("Histogram must be 2-dim");
         return;
      }
      if (!is2dim(hist1) && nent != 1) {
         WarnBox("Histogram must be 1-dim");
         return;
      }
      hname = hist1->GetName();
      cmd += hname;
      option += "same";

   } else {
      cmd += hname;
      gDirectory=gROOT;
      cchname= (const char *)hname;
      cout << "look for:" << cchname << ":" << endl;
      TObject *obj = (TObject *)gROOT->FindObject(cchname);
      TH1* hist;
      if (obj) {
         hist = (TH1*)obj;
         cout << " deleting " << hname << endl;
         delete hist;
      }
   }

   gDirectory=gROOT;
//   if (nent==3) new TCanvas("ctemp","ctemp",800,800);

   if (fRootFile) fRootFile->Close();
   fRootFile=new TFile(fname);
   TTree *tree = (TTree*)fRootFile->Get(tname);

   gDirectory=gROOT;

   TEnv * env = 0;
   Bool_t limits_defined = kFALSE;
   TString tag = leaf0.Data(); 
   if (!hist1 && nent > 0 && fRememberTreeHists) {
      if (gSystem->AccessPathName("ntuplerc")) {
         ofstream rcfile("ntuplerc", ios::out);
         if (!rcfile) {
           cout << "Cant open new ntuplerc" << endl;
         }
         rcfile << "ntuple.resourcefile: ntuplerc" << endl;;
         cout <<  "New file: ntuplerc" << endl;
         rcfile.close();
      }
      env = new TEnv("ntuplerc");
//     look if at least one of nbin, min max is defined;
      if (env) {
         limits_defined = kTRUE;
         for(Int_t i = 0; i < nent; i++) {
            Int_t nb;
            tag = lname[i]; tag += ".nbin";
            if (env->Lookup(tag.Data())) {
               nb = (Int_t)nbin[i]; 
               nb = env->GetValue(tag, nb); nbin[i] = nb;
            } else { 
               limits_defined = kFALSE;
            }
            tag = lname[i]; tag += ".min";
            if (env->Lookup(tag.Data())) {
               vmin[i] = atof(env->GetValue(tag.Data(), Form("%f",vmin[i])));
            }else {
               limits_defined = kFALSE;
            }

            tag = lname[i]; tag += ".max";
            if (env->Lookup(tag.Data())) {
               vmax[i] = atof( env->GetValue(tag.Data(), Form("%f",vmax[i])));
            }else {
               limits_defined = kFALSE;
            }

         }
      }
   }
//  look if we need to redefine limits
   Bool_t find_limits = kFALSE;
   for(Int_t i = 0; i < nent; i++) {
     if (nbin[i] == 0) find_limits = kTRUE;
   } 
   if (find_limits) limits_defined = kFALSE;

   if (find_limits || fAlwaysNewLimits) {
      for(Int_t i = 0; i < nent; i++) {
         TString bname;
         bname = lname[i];
         Int_t ib = bname.Index('[');
         if (ib > 0)bname.Resize(ib);
         vmin[i] = tree->GetMinimum(bname);
         vmax[i] = tree->GetMaximum(bname);
         nbin[i] = 100;
      }
   }
   if (!limits_defined) {
      TOrdCollection *row_lab = new TOrdCollection(); 
      TOrdCollection *col_lab = new TOrdCollection();
      col_lab->Add(new TObjString("Nbins"));
      col_lab->Add(new TObjString("Min"));
      col_lab->Add(new TObjString("Max"));
      row_lab->Add(new TObjString(leaf0.Data()));
      if (nent > 1 ) row_lab->Add(new TObjString(leaf1.Data()));
      if (nent == 3) row_lab->Add(new TObjString(leaf2.Data()));
      TArrayD xyvals(3*nent);
      Int_t p = 0;
      for(Int_t i = 0; i < nent; i++) { xyvals[p] = nbin[i]; p++;}
      for(Int_t i = 0; i < nent; i++) { xyvals[p] = vmin[i]; p++;}
      for(Int_t i = 0; i < nent; i++) { xyvals[p] = vmax[i]; p++;}
// show values to caller and let edit
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
      TRootCanvas * pwin = NULL;
      if (b) {
 //        TCanvas * cc = b->GetCanvas();
         pwin = (TRootCanvas *)(b->GetCanvas()->GetCanvasImp());
      }
      cout << "pwin " << pwin << endl;

      Int_t ret,  itemwidth=120, precission = 5; 
      TGMrbTableOfDoubles(pwin, &ret, "Set axis ranges", 
                        itemwidth,3, nent, xyvals, precission,
                       col_lab, row_lab);
      delete row_lab; delete col_lab;
      if (ret < 0) {
 //        delete [] vmin; delete [] vmax; delete [] nbin; 
//         delete [] xyvals;
         return;
      }
      p = 0;
      for(Int_t i = 0; i < nent; i++) { nbin[i] = xyvals[p]; p++;}
      for(Int_t i = 0; i < nent; i++) { vmin[i] = xyvals[p]; p++;}
      for(Int_t i = 0; i < nent; i++) { vmax[i] = xyvals[p]; p++;}
//      delete [] xyvals;
      if (fRememberTreeHists) {
         Int_t buttons= kMBYes | kMBNo, retval;
         EMsgBoxIcon icontype = kMBIconQuestion;
         new TGMsgBox(gClient->GetRoot(), GetMyCanvas(),
          "Qustion","Save values to ntuplerc",
          icontype, buttons, &retval);
         if (retval == kMBYes) {
            for(Int_t i = 0; i < nent; i++) {
               tag = lname[i]; tag += ".nbin";
               env->SetValue(tag.Data(),(Int_t)nbin[i]);
               tag = lname[i]; tag += ".min";
               env->SetValue(tag.Data(),vmin[i]);
               tag = lname[i]; tag += ".max";
               env->SetValue(tag.Data(),vmax[i]);
            }
            env->SaveLevel(kEnvLocal);
         }
      }
   }

   if (nent==1) new TH1F(hname.Data(),hname.Data(),
               	  (Int_t)nbin[0],vmin[0], vmax[0]); 
   if (nent==2) new TH2F(hname.Data(),hname.Data(),
               	 (Int_t)nbin[0],vmin[0], vmax[0], 
               	 (Int_t)nbin[1],vmin[1], vmax[1]); 
   if (nent==3) new TH3F(hname.Data(),hname.Data(),
               	 (Int_t)nbin[0],vmin[0], vmax[0], 
               	 (Int_t)nbin[1],vmin[1], vmax[1], 
               	 (Int_t)nbin[2],vmin[2], vmax[2]); 

//   delete [] vmin; delete [] vmax; delete [] nbin;
   if (env) delete env;

   
	 TEnv rootenv(".rootrc");		// inspect ROOT's environment
    Int_t max_events = 10000000;
    max_events = rootenv.GetValue("HistPresent.MaxEvents", max_events);
//   cout << cmd << endl;
   if (fApplyLeafCut) {
      TString cut = *fLeafCut;
      TRegexp a1("\\$1");
      if (cut.Index(a1)>=0) {while (cut.Index(a1)>=0) {cut(a1)=leaf0.Data();}}
      TRegexp a2("\\$2");
      if (cut.Index(a2)>=0) {
         if (nent < 2) {
            WarnBox("2 args expected, only 1 given");
            return;
         } else { while (cut.Index(a2)>=0) {cut(a2)=leaf1.Data();}}
      }  
      TRegexp a3("\\$3");
      if (cut.Index(a3)>=0) {
        if (nent < 3) {
           WarnBox("3 args expected");
           return;
        } else { while (cut.Index(a3)>=0) {cut(a3)=leaf2.Data();}}
      }  
      cout << "Execute: " << cmd << " Cut: " << cut << endl;
      tree->Draw((const char*)cmd, cut.Data(),option.Data(),max_events,0);
   } else if (fApplyGraphCut && nent == 2) {
      tree->Draw((const char*)cmd, fGraphCut->Data(),option.Data(),max_events,0); 
      cout << "Execute: " << cmd << endl;
      cout << "Apply graphical cut: " <<  fGraphCut->Data() << 
      " with X :" << fCutVarX->Data() <<   
      " and Y :" << fCutVarX->Data() <<  endl;  
   } else {
      cout << "Execute: " << cmd << endl;
      tree->Draw((const char*)cmd,"",option.Data(),max_events,0);
   }
   fRootFile->Close();
   fRootFile=NULL;
   cchname = (const char *)hname;
   TObject *obj = (TObject *)gROOT->FindObject(cchname);
//  if (!obj)WarnBox("No Object"); 
   TH1* hist = (TH1*)obj;
   if (hist) {
//      if (nent < 3) {
         hist->GetXaxis()->SetTitle(lname[0]);
         if (nent >= 2) hist->GetYaxis()->SetTitle(lname[1]);
         if (nent >= 3) hist->GetZaxis()->SetTitle(lname[2]);
         ShowHist(hist);
//      } else {

//         TString name3d("H3d_");
//         name3d+=hname.Data();
//         HTCanvas * h3d = new HTCanvas(name3d, name3d,400, 400, 800,800, this, 0);
//         hist->Draw();
//         h3d->Modified(); 
//         h3d->Update();
//         fCanvasList->Add(h3d);
//      }
   } else WarnBox("No hist");    
}

//________________________________________________________________________________________
// Save histograms from mapped file
  
void HistPresent::SaveMap(const char* mapname, const char* bp)
{
   TMapFile *mfile;
   mfile = TMapFile::Create(mapname);
   if (!mfile) {
      cout << "MapFile not found: " << mapname << endl;
      gDirectory=gROOT;
      return;
   }   
   TString fname = mapname;
   Int_t pp = fname.Index(".");
   if (pp) fname.Resize(pp);
   fname+=".root";

   Bool_t ok;
   const char * foutname=GetString("File Name",(const char *)fname, 
                                   &ok, GetMyCanvas());
   if (!ok) return;
   if (!gSystem->AccessPathName((const char *)foutname, kFileExists)) {
      TString question=foutname;
      question += " already exists, overwrite?";
      if (!QuestionBox(question.Data(), GetMyCanvas())) return;
   }
   TFile *f = new TFile(foutname,"RECREATE");
   const char * name;
   TMapRec *mr = mfile->GetFirst();
   if (mr) {
      while (mfile->OrgAddress(mr)) {
         if (!mr) break;
         name=mr->GetName();
         TH1 *hist=0;
         hist    = (TH1 *) mfile->Get(name, hist);
         if (hist) hist->Write();
         cout << "Writing: " << name << endl;
         mr=mr->GetNext();         
      }
   }
   if (f) f->Close();
   cout << "SaveMap done" << endl;
   gDirectory=gROOT;
}
//________________________________________________________________________________________
// Save histograms from mapped file
  
void HistPresent::SaveFromSocket(const char * name, const char* bp)
{
   if (!fComSocket) return;
   TString fname ("hists_XX.root");

   Bool_t ok;
   const char * foutname=GetString("File Name",(const char *)fname, 
                                   &ok, GetMyCanvas());
   if (!ok) return;
   if (!gSystem->AccessPathName((const char *)foutname, kFileExists)) {
      TString question=foutname;
      question += " already exists, overwrite?";
      if (!QuestionBox(question.Data(), GetMyCanvas())) return;
   }

   TMrbStatistics * st = getstat(fComSocket);
   if (!st) {
       cout << " cant get stat(fComSocket)" << endl;
       fComSocket->Close("force");
       fComSocket = NULL;
       return;
   } else {
//         st->Print();
   }
   TFile *f = new TFile(foutname,"RECREATE");
   TH1 *hist=0;
   Int_t nhists = 0;
   TMrbStatEntry * stent;
   TIter nextentry(st->GetListOfEntries());
   while ( (stent = (TMrbStatEntry*)nextentry()) ) {
      hist = (TH1 *) gethist(stent->GetName(), fComSocket);
      if (!hist){
         cout << setred << "Cant get hist, Connection lost?" << setblack << endl;
         fComSocket->Close("force");
         fComSocket = NULL;
         break;
      }
      f->cd();
      if (hist) hist->Write();
//           cout << "Writing: " << stent->GetName()<< endl;
      nhists++;
   }
   if (f) f->Close();
   if (st) delete st;
   cout << nhists << " histograms saved to " << foutname << endl;
   gDirectory=gROOT;
}

//________________________________________________________________________________________
// Show Function
  
void HistPresent::ShowFunction(const char* fname, const char* name, const char* bp)
{
   TF1* func;
   if (strstr(fname,".root")) {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      func = (TF1*)fRootFile->Get(name);
//      func->SetDirectory(gROOT);
//      fRootFile->Close();
   } else {
      func=(TF1*)gROOT->FindObject(name);
   }
//  gROOT->ls();
   if (func) {
      new TCanvas("cccc","cccc", 400, 40, 200, 200);
      func->Draw();   
   } else     WarnBox("Function not found");
   if (fRootFile) fRootFile->Close();
}
//________________________________________________________________________________________
// Show user contour
  
void HistPresent::ShowContour(const char* fname, const char* name, const char* bp)
{
   FhContour * co;
   if (strstr(fname,".root")) {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      co = (FhContour*)fRootFile->Get(name);
//      func->SetDirectory(gROOT);
      fRootFile->Close();
   } else {
      co = (FhContour*)gROOT->FindObject(name);
   }
//  gROOT->ls();
   if (co) {
      TArrayI colsav(*(co->GetColorArray()));
      TArrayD levsav(*(co->GetLevelArray()));

      Int_t result = co->Edit(GetMyCanvas()); 

      if (result >= 0){
         Bool_t changed = kFALSE;
         for (Int_t i = 0l; i < co->GetColorArray()->GetSize(); i++) {
            if (colsav[i] !=   (*(co->GetColorArray()))[i] ) {
               changed = kTRUE;
               break;
            }
            if (levsav[i] !=   (*(co->GetLevelArray()))[i]){
               changed = kTRUE;
               break;
            }
         }
         if (changed) {
            cout << "Changed" << endl;
            int buttons = kMBOk | kMBDismiss, retval = 0;
            EMsgBoxIcon icontype = kMBIconQuestion;
            new TGMsgBox(gClient->GetRoot(), GetMyCanvas(),
                   "Question", "Save modified contour?",
                   icontype, buttons, &retval);
             if (retval == kMBOk) {
                fRootFile=new TFile(fname, "UPDATE");
                co->Write();
                fRootFile->Close();
                fRootFile = 0;
            }
         }
      }
   } else     WarnBox("Contour not found");

//   if (fRootFile) fRootFile->Close();
}
//________________________________________________________________________________________
// Show user contour
  
void HistPresent::ShowGraph(const char* fname, const char* name, const char* bp)
{
   TGraphErrors * graph;
   if (strstr(fname,".root")) {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      graph = (TGraphErrors*)fRootFile->Get(name);
//      func->SetDirectory(gROOT);
//      fRootFile->Close();
   } else {
      graph = (TGraphErrors*)gROOT->FindObject(name);
   }
   if (graph) {
      TString cname = name;
      cname.Prepend("C_");
      if (fNwindows>0) {       // not the 1. time
         if (fWinshiftx != 0 && fNwindows%2 != 0) fWincurx += fWinshiftx;
         else   {fWincurx = fWintopx; fWincury += fWinshifty;}
      }
      fNwindows++;
      HTCanvas * cg = new HTCanvas(cname, cname, fWincurx, fWincury,
                                 fWinwidx_1dim, fWinwidy_1dim, this, 0, 0, graph);
      fCanvasList->Add(cg);
//      graph->SetName(hname);
//      graph->SetTitle(htitle);
      graph->Draw(fDrawOptGraph);
      graph->GetHistogram()->SetStats(kFALSE);
   } else {
      WarnBox("Graph not found");
   }
   if (fRootFile) fRootFile->Close();
}
//________________________________________________________________________________________
// set rebin val 
  
void HistPresent::SetRebinValue(Int_t val) 
{
   Bool_t ok; 
   if (val == 0) {
      Int_t i = GetInteger("Rebin value", fRebin, &ok, maincanvas);
      if (!ok || i <= 0) return;
      fRebinOth->SetTitle(Form("%d", i));
      fRebinOth->SetFillColor(3);
      fRebin2->SetFillColor(2);
      fRebin4->SetFillColor(2);
      fRebin=i;
   }else {
      fRebin=val;
      fRebinOth->SetFillColor(2);
      if (val == 2) {fRebin4->SetFillColor(2); fRebin2->SetFillColor(3);}
      if (val == 4) {fRebin4->SetFillColor(3); fRebin2->SetFillColor(2);}
   }
   fRebinOth->Modified(kTRUE); 
   fRebin2->Modified(kTRUE);
   fRebin4->Modified(kTRUE);
}
//________________________________________________________________________________________
// set rebin val 
  
void HistPresent::SetRebinMethod() 
{
   if (fRMethod == 1) {
      fRebinSumAvg->SetTitle("Sum");
      fRMethod=0;
   } else {
      fRebinSumAvg->SetTitle("Avg");
      fRMethod=1;
   }
   fRebinSumAvg->Modified(kTRUE); fRebinSumAvg->Update();
}
//____________________________________________________________________________________ 
  
void HistPresent::SetOperateVal() 
{
   Bool_t ok; 
   Float_t fac = GetFloat("Factor", fOpfac, &ok, maincanvas);
   fOpfac = fac;
   fValButton->SetTitle(Form("%lg", fac));
}
//________________________________________________________________________________________
// Operate on  histograms 
  
void HistPresent::OperateHist(Int_t op) 
{
   Int_t nselect=fSelectHist->GetSize();
   if (nselect < 1) {
      WarnBox("No selection");
      return;
   }
//   if (nselect < 2 && op == 1) {
//      WarnBox("Less than 2 selections for Add, to scale choose *");     
//      return;
//   }
   if (nselect > 2 && op != 1) {
      WarnBox("More than 2 selections, canceled");
//      nselect = 2;     
      return;     
   }
   if (nselect == 1 && op == 3) {
     cout << "OperateHist: Scale errors linearly" << endl;
   }
   TH1* hist1 = GetSelHistAt(0);
   if (!hist1) {WarnBox("Histogram not found");return;};
   TString name1 = hist1->GetName();
   cout << "Do: " << name1;
   TString nameop = "empty";
   TString name2;
   TH1* hresult =  (TH1*)hist1->Clone();
   TH1* hist2;
//   if (nselect == 2 || op == 1) {
   if (nselect == 2) {
      for(Int_t i = 1; i < nselect; i++) {
         hist2 = GetSelHistAt(i);
         if (!hist2) {WarnBox("Histogram not found");return;};
         name2 = hist2->GetName();
         switch (op) {
            case 1: 
               hresult->Add(hist2, fOpfac);
               nameop="_plus_";
               cout << " + " <<  fOpfac << " * " << name2;
               break;
            case 2: 
               hresult->Add(hist2, -1.*fOpfac);
               nameop="_minus_";
               cout << " - "<<  fOpfac << " * "  << name2;
               break;
            case 3: 
               hresult->Multiply(hist1, hist2, 1., fOpfac);
               nameop="_times_";
               cout << " * "<<  fOpfac << " * " << name2;
               break;
            case 4: 
               hresult->Divide(hist1, hist2, 1., fOpfac);
               nameop="_divby_";
               cout << " / "<<  fOpfac << " * " << name2;
               break;
         }
      }  
   } else {
      Float_t fac = fOpfac;
      Int_t nbinsx;
      switch (op) {
         case 2:                       // subtract
            fac = - fOpfac;
//            break;
         case 1:                       // add
            nameop="_biased";
             nbinsx = hresult->GetNbinsX();
            if (hresult->GetDimension() == 1) {
               for(Int_t binx = 1; binx <= nbinsx; binx ++) {
                 Stat_t cont = hresult->GetBinContent(binx);
                  hresult->SetBinContent(binx, cont + fac);
               }
            } else if (hresult->GetDimension() == 2) {
               Int_t nbinsy = hresult->GetNbinsY();
               for(Int_t binx = 1; binx <= nbinsx; binx ++) {
                  for(Int_t biny = 1; biny <= nbinsy; biny ++) {
                    Stat_t cont = hresult->GetBinContent(binx, biny);
                     hresult->SetBinContent(binx, biny, cont + fac);
                  }
               }
            } else {
               Int_t nbinsy = hresult->GetNbinsY();
               Int_t nbinsz = hresult->GetNbinsZ();
               for(Int_t binx = 1; binx <= nbinsx; binx ++) {
                  for(Int_t biny = 1; biny <= nbinsy; biny ++) {
                     for(Int_t binz = 1; binz <= nbinsz; binz ++) {
                        Stat_t cont = hresult->GetBinContent(binx, biny, binz);
                        hresult->SetBinContent(binx, biny, binz, cont + fac);
                     }
                  }
               }
            }
            cout<< " + " << fac;
            break;
         case 4:                       // divide 
            if (fOpfac != 0.) fac = 1./fOpfac;
            else                fac = 1.;
//            break;
         case 3:                        // multiply
            hresult->Sumw2();
            hresult->Scale(fac);
//            for( Int_t i=1; i <= hresult->GetNbinsX(); i++) {
//               hresult->SetBinError(i, fOpfac * hist1->GetBinError(i));
//            }
            nameop="_scaled";
            cout<< " * " << fac;
            break;
      }
   }
   name1 = name1 + nameop + name2;
   if (name1.Length() > 64) {
//      TDatime td;
//      name1 = "hist_";
//      ostrstream buf;
      
 //     buf << setfill('0') << setw(6) << td.GetTime()<< '\0';
 //     name1 += buf.str();
 //     buf.rdbuf()->freeze(0);
      WarnBox("Name for result hist got too long, watch terminal");
   }
   hresult->SetName((const char *)name1);
   cout << " ->> " << name1  << endl;
   ShowHist(hresult);
}
//________________________________________________________________________________________
// Differentiate histogram
  
void HistPresent::DiffHist() 
{
   TH1* hist = GetSelHistAt(0);
   if (!hist) return;
   if (fSelectHist->GetSize() > 1) {
      WarnBox("More than 1 selection, take first");
   } 
   if (is2dim(hist)) {
      WarnBox("2 dim not supported yet");
      return;
   }       
//  gROOT->ls();
   if (hist) {
      // create a clone of the old histogram
      TH1F* hnew = (TH1F*)hist->Clone();

      // change name and axis specs
      TString name = hist->GetName();
      name += "_diffed";
      hnew->SetName((const char*)name);
      TAxis *xold   = hist->GetXaxis();
      Int_t nbins   = xold->GetNbins();

      // copy merged bin contents (ignore under/overflows)
      Int_t bin;
      Stat_t diff;
      for (bin = 1;bin<=nbins-1;bin++) {
         diff = hist->GetBinContent(bin) - hist->GetBinContent(bin+1) ;
         hnew->SetBinContent(bin,diff);
      }
      hnew->SetEntries(nbins);
      ShowHist(hnew);
   } else {
      WarnBox("No hist found"); 
   }
   
}
//________________________________________________________________________________________
// Rebin histograms 
  
void HistPresent::RebinHist() 
{
   TH1* hist = GetSelHistAt(0);
   if (!hist) {WarnBox("No hist found"); return;}
   if (fSelectHist->GetSize() > 1) {
      WarnBox("More than 1 selection, take first");
   } 
   if (is2dim(hist)) {
      WarnBox("2 dim not supported yet");
      return;
   } 
   TString buf(hist->GetName());
   buf += "_rebinby_";  buf += fRebin;  
//   TString name = hist->GetName();
//   name += "_rebin_";
   TH1* hold=(TH1*)gROOT->FindObject(buf.Data());
   if (hold) {
//      cout << "Delete existing " <<  buf.str()<< endl;
      delete hold;
   }
   TH1F *hnew = (TH1F*)hist->Rebin(fRebin,buf.Data());
   if (fRMethod == 1) hnew->Scale(1./(Float_t)fRebin);
   ShowHist(hnew);
}
//________________________________________________________________________________________
// Rebin histograms 
  
TH1* HistPresent::GetSelHistAt(Int_t pos, TList * hl, Bool_t try_memory) 
{
   TList * hlist;
   if (hl) hlist = hl;
   else   hlist = fSelectHist;
   if (hlist->IsEmpty()) {
      WarnBox("Please select a hist");
      return NULL;
   }
   if (hlist->GetSize() < pos) {
      WarnBox("Requested non existing entry");
      return NULL;
   } 
 //  Bool_t ok;
   TObjString * obj = (TObjString *)hlist->At(pos);

   TString fname = obj->String();
//	cout << "|" << fname << "|" << endl;
   Int_t pp = fname.Index(" ");
   if (pp <= 0) {cout << "Not file name in: " << obj->String() << endl; return NULL;};
   fname.Resize(pp);

   TString hname = obj->String();
   hname.Remove(0,pp+1);
   TString dname = hname.Data();
   pp = hname.Index(" ");
   if (pp <= 0) {cout << "No histogram name in: " << obj->String()<< endl; return NULL;};
   hname.Resize(pp);
   dname.Remove(0,pp+1);
//   cout << fname << "|" << hname << "|" << dname << "|" << endl;

   hname = hname.Strip(TString::kBoth);
   dname = dname.Strip(TString::kBoth);
   TH1* hist;
   hist = (TH1*)gROOT->GetList()->FindObject(hname);
	if (!hist && try_memory) {
	   TString newname(fname.Data());
		pp = newname.Index(".");
		if (pp>0) newname.Resize(pp);
		newname += "_";
		newname += hname.Data();
	   hist = (TH1*)gROOT->GetList()->FindObject(newname);
      cout << "Use hist in memory: " << hname << " Fn: " << fname << " Nn: " << newname << endl;
	}
//   if (hist) hist->Print();
   if (hist && (fname == "Memory" || try_memory)) return hist;
   if (!(fname == "Memory")) {
      hist=(TH1*)gROOT->GetList()->FindObject(hname);
//     gROOT->ls();
      if (hist) {
//         WarnBox("Deleting existing histogram");
         hist->Delete();
//         WarnBox("Using existing histogram");
//         return hist;
      } 
   } 
   if (strstr(fname,"Socket")) {
      if(!fComSocket){
          cout << setred << "No connection open"  << setblack << endl;
          return NULL;
      }
      hist = gethist(hname.Data(), fComSocket);
      if (!hist){
         cout << setred << "Cant get hist, Connection lost?" << setblack << endl;
         fComSocket->Close("force");
         fComSocket = NULL;
      }
      return hist;
   }
// watch out: is it .map or .root or in memory

   if (fname.Index(".map") >= 0) {
      TMapFile *mfile;
      mfile = TMapFile::Create(fname);
      hist=NULL;
      hist    = (TH1 *) mfile->Get(hname, hist);
      mfile->Close();
   } else {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile((const char *)fname);
      if (dname.Length() > 0)fRootFile->cd(dname);
      hist = (TH1*)gDirectory->Get(hname);
      if (hist) hist->SetDirectory(gROOT);
      else      cout << "Histogram: " << hname << " not found" << endl;
      if (fRootFile) {fRootFile->Close(); fRootFile=NULL;};
   }
   gDirectory=gROOT;
//  gROOT->ls();
   TString newname(hname.Data());
   newname += "_",
   newname += pos;
   if (hist) hist->SetName(newname.Data());
   return hist;
}
//________________________________________________________________________________________
// Get Ccut  
  
void HistPresent::SelectCut(const char* fname, const char* cname, const char* bp)
{
   if (!gPad) cout << "Cut: Cant find Pad, call debugger" << endl;
   else      gPad->SetFillColor(3);

   if (fRootFile) fRootFile->Close();
   fRootFile=new TFile(fname);
   fRootFile->Get(cname);
//   TCutG * cut= (TCutG*)fRootFile->Get(cname);
 //  if (cut) cut->SetDirectory(gROOT);
   fRootFile->Close();
   gDirectory=gROOT;
}
//_______________________________________________________________________

void HistPresent::CloseHistLists() 
{  
   ClearSelect();
   fHistLists->Delete();
}
//________________________________________________________________________________________
// Select
  
void HistPresent::SelectHist(const char* fname, const char* dir, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   sel = sel + " " + hname + " " + dir;
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
      if (b->TestBit(kSelected)) {
         TObjString tobjs((const char *)sel);
   //      tobjs.Print(" ");
         fSelectHist->Remove(&tobjs);
         b->SetFillColor(16);
         b->ResetBit(kSelected);
   //      gPad->SetFillColor(16);
   //     case not already selected
      } else {
         fSelectHist->Add(new TObjString((const char *)sel));
         b->SetFillColor(3);
         b->SetBit(kSelected);
   //      gPad->SetFillColor(3);
      }
      b->Modified(kTRUE);b->Update();
   }
}
//________________________________________________________________________________________
// Select
  
void HistPresent::SelectContour(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   sel = sel + " " + hname;
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
      if (b->TestBit(kSelected)) {
         TObjString tobjs((const char *)sel);
   //      tobjs.Print(" ");
         fSelectContour->Remove(&tobjs);
         b->SetFillColor(16);
         b->ResetBit(kSelected);
   //      gPad->SetFillColor(16);
   //     case not already selected
      } else {
/*
         if (fRootFile) fRootFile->Close();
         fRootFile=new TFile(fname);
         FhContour * co = (FhContour*)fRootFile->Get(hname);
         co->SetDirectory(gROOT);
         fAllContours->Add(co);
         fRootFile->Close();
         gDirectory=gROOT;
*/
         fSelectContour->Add(new TObjString((const char *)sel));
         b->SetFillColor(3);
         b->SetBit(kSelected);
   //      gPad->SetFillColor(3);
      }
      b->Modified(kTRUE);b->Update();
   }
}
//________________________________________________________________________________________
// Select
  
void HistPresent::SelectGraph(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   sel = sel + " " + hname;
   if (bp) {
      TButton * b;
      b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
      if (b->TestBit(kSelected)) {
         TObjString tobjs((const char *)sel);
         fSelectGraph->Remove(&tobjs);
         b->SetFillColor(16);
         b->ResetBit(kSelected);
      } else {
         b->SetFillColor(3);
         b->SetBit(kSelected);
         fSelectGraph->Add(new TObjString((const char *)sel));
   //      gPad->SetFillColor(3);
      }
      b->Modified(kTRUE);b->Update();
   }
}
//________________________________________________________________________________________

void HistPresent::PrintCut(const char* fname, const char* hname, const char* bp)
{
   TCutG* cut;
   if (is_memory(fname)) {
      cut = (TCutG*)gROOT->FindObject(hname);
   } else {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      cut = (TCutG*)fRootFile->Get(hname);
      if (fRootFile) fRootFile->Close();
      gDirectory=gROOT;
   }
   if (cut) {
      cout << cut->GetName() << ":"<<endl;
      cout<< "fVarX:" << cut->GetVarX() <<endl;
      cout<< "fVarY:" << cut->GetVarY() <<endl;
      int n = cut->GetN();
      for(int i=0; i<n; i++)  {
         Double_t x,y;
        cut->GetPoint(i,x,y);
        cout << "x: " << x << " \ty: " << y << endl;
      }
      if (strncmp("Memory",fname,6)) delete cut;
   }
}
//________________________________________________________________________________________

void HistPresent::CutsToASCII(const char* name, const char* bp)
{
   if (fAllCuts->GetSize() <= 0) {
      WarnBox("No cuts selected");
      return;
   }
   const char helpText[] = "Write values of cuts (Window2D) as ASCII file";
   TRegexp rname("root");
   TString fname = name;
   fname(rname) ="wdw2D";
   Bool_t ok;
   fname = GetString("Write ASCII-file with name",fname.Data(),  &ok, maincanvas,
   0, 0, helpText);
   if (!ok) {
      cout << " Canceled " << endl;
      return; 
   } 
   if (!gSystem->AccessPathName((const char *)fname, kFileExists)) {
//      cout << fname << " exists" << endl;
      int buttons= kMBOk | kMBDismiss, retval=0;
      EMsgBoxIcon icontype = kMBIconQuestion;
      new TGMsgBox(gClient->GetRoot(), GetMyCanvas(),
       "Question", "File exists, overwrite?",
       icontype, buttons, &retval);
      if (retval == kMBDismiss) return;
   }
   ofstream outfile;
   outfile.open((const char *)fname);
   if (!outfile) {
      WarnBox("Cant open file");
      return;
   }
   TIter next(fAllCuts);
   while ( TMrbWindow2D * cut = (TMrbWindow2D *)next()) {
      Int_t n = cut->GetN();
      outfile << cut->GetName() <<endl;
      outfile << n << endl;
      for(Int_t i=0; i < n; i++) {
         Double_t x,y;
         cut->GetPoint(i,x,y);
         outfile << x<< "\t" << y<< endl;
      }
//      cut->Print();
   }
   outfile.close();
}
//________________________________________________________________________________________
void HistPresent::CutsFromASCII(TGWindow * win) 
{
   const char helpText[] = "Read values of cuts (Window2D) from ASCII file";
   TRegexp endwithwdw2D("wdw2D$");
   TString fname = "xxx.wdw2D";
   void* dirp=gSystem->OpenDirectory(".");
   while (const char * name=gSystem->GetDirEntry(dirp)) {
      TString sname(name);
      if (sname.Index(endwithwdw2D) >= 0) fname = sname;
   }
   Bool_t ok;
   fname = GetString("Filename",fname.Data(), &ok, win,0, 0, helpText);
   if (!ok) {
      cout << " Canceled " << endl;
      return; 
   } 
   ifstream infile;
   infile.open((const char *)fname, ios::in);
	if (!infile.good()) {
	cerr	<< "CutsFromASCII: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return;
	}
	TString cutname; 
   Int_t npoints;
   Float_t x,y;
   while ( 1) {
	   cutname.ReadLine(infile, kFALSE);
	   if (infile.eof()) {
			infile.close();
			return;
		}
      cutname = cutname.Strip();
		if (cutname.Length() <= 0) continue;
      infile >> npoints;
      cout << endl;
      cout << cutname.Data() << " " << npoints << endl;
      if (npoints <= 0) {WarnBox("npoints <= 0"); return;}
         
      TMrbWindow2D * wdw = new TMrbWindow2D((const char *)cutname, npoints);
      for(Int_t i = 0; i < npoints; i++) {
         infile >> x; infile >> y;
         wdw->SetPoint(i, x, y); 
      }
      fAllCuts->Add(wdw);
   }
}
//________________________________________________________________________________________
void HistPresent::LoadCut(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   sel = sel + " " + hname;
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
   if (!b->TestBit(kSelected)) {
//   if (color != 3) {
      if (!strncmp("Memory",fname,6)) {
         cout << " Cut was removed" << endl;
      } else { 
         if (fRootFile) fRootFile->Close();
         fRootFile=new TFile(fname);
         TCutG* cut= (TCutG*)fRootFile->Get(hname);
//         cout << "TCutG, class " << cut->ClassName() << endl;
         TMrbWindow2D * wdw2d= 
         new TMrbWindow2D(cut->GetName(),cut->GetN(),cut->GetX(),cut->GetY());
         fAllCuts->Add(wdw2d);
//  cut->SetDirectory(gROOT);
         fRootFile->Close();
         gDirectory=gROOT;
         fSelectCut->Add(new TObjString((const char *)sel));
//     case not already selected
         b->SetFillColor(3);
         b->SetBit(kSelected);
      }
   } else {
      TMrbWindow2D* cc = (TMrbWindow2D*)fAllCuts->FindObject(hname);
      TList *tl=gDirectory->GetList();
      TIter next(tl);
      while ( TObject* obj=next()) {
         if (!(strncmp(obj->ClassName(),"FitHist",7))) {
            FitHist *fhist=(FitHist*)obj;
            if (fhist->UseCut(cc)) {
               cout << "Cut: " << hname << " used by fithist" << endl;
               return;
            }
         }
      }
      TObjString tobjs((const char *)sel);
      cout << "remove from list: ";
      tobjs.Print(" ");
//      TCutG* cc=(TCutG*)gROOT->FindObject(hname);
      if (cc) {
         cout << "remove from memory: " << hname << endl;
         fAllCuts->Remove(cc);        
         delete cc;
      }
      fSelectCut->Remove(&tobjs);    
      b->SetFillColor(16);
      b->ResetBit(kSelected);
   }
   b->Modified(kTRUE);
   b->Update();
}
//________________________________________________________________________________________
void HistPresent::PrintWindow(const char* fname, const char* hname, const char* bp)
{
   TMrbWindow *wdw;
   TObject * obj = NULL;
   TString sel = fname;
   if (is_memory(fname)) {
      wdw = (TMrbWindow*)gROOT->FindObject(hname);
   } else if (sel.Contains(".map")) {
       TMapFile *mfile;
       mfile = TMapFile::Create(fname);
       obj   = (TObject*) mfile->Get(hname, 0);
       mfile->Close();
   } else {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      obj = (TObject*)fRootFile->Get(hname);
   }
   if (obj) {
       TNamed* nobj=(TNamed*)obj;
       cout << " " << endl;
       cout << "---   Window: " << nobj->GetName() << " -----------" << endl;
       obj->Print(); delete obj; obj = 0;
   }
   if (fRootFile) {fRootFile->Close(); fRootFile = 0;}
   gDirectory=gROOT;
}
//________________________________________________________________________________________

void HistPresent::LoadFunction(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   TObject * fun = 0;
   sel = sel + " " + hname;
	TObjString tobjs((const char *)sel);
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
   if (!b->TestBit(kSelected)) {
//   if (color != 3) {
      if (!strncmp("Memory",fname,6)) {
         cout << " Function was removed??" << endl;
      } else if (sel.Contains(".map")) {
          TMapFile *mfile;
          mfile = TMapFile::Create(fname);
          fun=NULL;
          fun   = (TObject*) mfile->Get(hname, 0);
          mfile->Close();
      } else {
         if (fRootFile) fRootFile->Close();
         fRootFile=new TFile(fname);
         fun= (TObject*)fRootFile->Get(hname);
         fRootFile->Close(); fRootFile=0;
      }
      if (fun) {
//         cout << "fun->GetClassName(): "<< fun->ClassName() << endl;
         TObject * tmp;
         tmp = fAllFunctions->FindObject(fun->GetName());
         if (tmp) {
            cout << fun->GetName()<< " existing "  << endl;
            TString newname(hname);
            TRegexp sem(";");
            newname(sem)="_v";
            TF1* fun1= (TF1*)fun;
            fun1->SetName(newname.Data());
//               fAllCuts->Remove(tmp);
         }
         fAllFunctions->Add(new TObjString((const char *)sel));
      }
      gDirectory=gROOT;
//     case not already selected
      b->SetFillColor(3);
      b->SetBit(kSelected);
   } else {
//     sel = hname;
//      Int_t isem = sel.Index(";");
//      if (isem > 0) sel.Resize(isem);
//      cout << sel.Data() << endl;
//      fun = fAllFunctions->FindObject(sel.Data());
//      if (fun) {
//         cout << "Remove:" << tobjs.GetString()<< endl; 
         fAllFunctions->Remove(&tobjs);
//      }    
      b->SetFillColor(16);
      b->ResetBit(kSelected);
   }
   b->Modified(kTRUE);
   b->Update();
}
//________________________________________________________________________________

void HistPresent::LoadWindow(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
   TString sel = fname;
   TObject * wdw = 0;
   sel = sel + " " + hname;
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
   if (!b->TestBit(kSelected)) {
//   if (color != 3) {
      if (!strncmp("Memory",fname,6)) {
         cout << " Window was removed??" << endl;
      } else if (sel.Contains(".map")) {
          TMapFile *mfile;
          mfile = TMapFile::Create(fname);
          wdw=NULL;
          wdw   = (TObject*) mfile->Get(hname, 0);
          mfile->Close();
      } else {
         if (fRootFile) fRootFile->Close();
         fRootFile=new TFile(fname);
         wdw= (TObject*)fRootFile->Get(hname);
      }
      if (wdw) {
//         cout << "wdw->GetClassName(): "<< wdw->ClassName() << endl;
         TObject * tmp;
         if (wdw->InheritsFrom("TCutG")) {
           CheckList(fAllCuts, "TMrbWindow");
           if ( (tmp = fAllCuts->FindObject(wdw->GetName()) ) )
            if (tmp) {
               cout << wdw->GetName()<< " existing "  << endl;
               TString newname(hname);
               TRegexp sem(";");
               newname(sem)="_v";
               TMrbWindow2D* wdw2d= (TMrbWindow2D*)wdw;
               wdw2d->SetName(newname.Data());
            }
            fAllCuts->Add(wdw);
         } else {
//            wdw->Dump();
            CheckList(fAllWindows, "TMrbWindow");
            tmp = fAllWindows->FindObject(wdw->GetName());
            if (tmp) {
               cout << wdw->GetName()<< " existing "  << endl;
               TString newname(hname);
               TRegexp sem(";");
               newname(sem)="_v";
               TMrbWindow* wdw1d= (TMrbWindow*)wdw;
               wdw1d->SetName(newname.Data());
//               fAllCuts->Remove(tmp);
            }
            fAllWindows->Add(wdw);
         }
      }
      fSelectWindow->Add(new TObjString((const char *)sel));
      if (fRootFile) {fRootFile->Close(); fRootFile=0;}
      gDirectory=gROOT;
//     case not already selected
      b->SetFillColor(3);
      b->SetBit(kSelected);      
   } else {
      TMrbWindow* cc=(TMrbWindow*)fAllWindows->FindObject(hname);
      TList *tl=gDirectory->GetList();
      TIter next(tl);
      while ( TObject* obj=next()) {
         if (!(strncmp(obj->ClassName(),"FitHist",7))) {
            FitHist *fhist=(FitHist*)obj;
            if (fhist->UseWindow(cc)) {
               cout << "Window: " << hname << 
               " used by FitHist " << fhist->GetName() 
               << " cant remove from memory" << endl;
               return;
            }
         }
      }
      TObjString tobjs((const char *)sel);
//      cout << "remove from list: ";
//      tobjs.Print(" ");
//      TCutG* cc=(TCutG*)gROOT->FindObject(hname);
      fSelectWindow->Remove(&tobjs);    
      fAllWindows->Remove(&tobjs);    
      fAllCuts->Remove(&tobjs);    
      if (cc) {
//         cout << "remove from memory: " << hname << endl;
         gDirectory->GetList()->Remove(cc);        
         delete cc;
      }
      b->SetFillColor(16);
      b->ResetBit(kSelected);      
   }
   b->Modified(kTRUE);
   b->Update();
}
//__________________________________________________________________________________

void HistPresent::ClearSelect() 
{
      if (fHistLists->GetSize() > 0) {
         TIter next(fHistLists);
//         cout << "hl->GetSize() " << hl->GetSize() << endl;
         TCanvas *ca;
         while ( (ca = (TCanvas*)next()) ) {
            TIter nextobj(ca->GetListOfPrimitives());
            TButton *b;
            TObject * obj;
            while ( (obj = nextobj()) ) {
               if (!strcmp(obj->ClassName(),"TButton")) {
                  b = (TButton*)obj;
                  if (b->TestBit(kSelected) && b->TestBit(kSelection)) {
                     b->ResetBit(kSelected);
                     b->SetFillColor(15);
                     b->Modified(kTRUE);
                     b->Update();
                  }
               }
            }
         }
      }
      fSelectHist->Clear();
      fAllFunctions->Clear();
      fSelectCut->Clear();
      fSelectWindow->Clear();
      fSelectContour->Clear();
      fSelectGraph->Clear();
}; 
//__________________________________________________________________________________
  
void HistPresent::ListSelect() 
{
   TObjString *sel;
   cout << "-------------------- " << endl;
   cout << "Selected histogramms " << endl;
   TIter next(fSelectHist);
//   pointers->Sort();
   Int_t count = 0;
   while ( (sel= (TObjString*)next()) ) {
      count ++;
      cout << count << ". ";
      sel->Print(" ");
   }
   if (count>0) cout << "----------------" << endl;
   cout << "Selected cuts " << endl;
   cout << "-------------" << endl;
   TIter nextcut(fSelectCut);
//   pointers->Sort();
   count = 0;
   while ( (sel = (TObjString*)nextcut()) ) {
      count ++;
      cout << count << ". ";
      sel->Print(" ");
   }
   if (count>0) cout << "----------------" << endl;
   cout << "All windows " << endl;
   cout << "----------------" << endl;
   TIter allwdw(fAllWindows);
//   pointers->Sort();
   count = 0;
   while ( TMrbWindow * wdw = (TMrbWindow*)allwdw()) {
      count ++;
      cout << count << ". ";
      wdw->Print(" ");
   }
   if (count>0) cout << "----------------" << endl;
   cout << "Selected windows " << endl;
   cout << "----------------" << endl;
   TIter nextwdw(fSelectWindow);
//   pointers->Sort();
   count = 0;
   while ( (sel= (TObjString*)nextwdw()) ) {
      count ++;
      cout << count << ". ";
      sel->Print(" ");
   }
   if (count>0) cout << "----------------" << endl;
   cout << "Selected Functions " << endl;
   cout << "----------------" << endl;
   fAllFunctions->Print();
   if (count>0) cout << "----------------" << endl;
   cout << "Selected User Contours " << endl;
   cout << "-------------------- " << endl;
   fSelectContour->Print();
   if (count>0) cout << "----------------" << endl;
   cout << "Selected Graphs " << endl;
   cout << "-------------------- " << endl;
   fSelectGraph->Print();
   cout << "-------------------- " << endl;
};
//_______________________________________________________________________
  
TH1* HistPresent::GetHist(const char* fname, const char* dir, const char* hname) 
{
//   TurnButtonGreen(&activeHist);
   TH1* hist=0;
   TString shname = hname;
//    shname = shname.Strip(TString::kBoth);
   if (strstr(fname,".root")) {
      TString newhname = shname;  
      if (fRootFile) fRootFile->Close();
      newhname = fname;   
      Int_t pp = newhname.Index(".");
      if (pp) newhname.Resize(pp);
      newhname = newhname + "_" + shname.Data();
      const char * hn = (const char*)newhname;
      TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
      TString FHname("F");
      FHname += hn;
      while (FHname.Index(notascii) >= 0) {
         FHname(notascii) = "_";
      }
      FitHist *fhist = (FitHist*)gDirectory->GetList()->FindObject(FHname);
      if (fhist) {
//         Warning("In GetHist:"," Delete  existing : %s",FHname.Data());
         gDirectory->GetList()->Remove(fhist);
         delete fhist;
      }
      hist=(TH1*)gROOT->FindObject(hn);

      if(hist) {
//         cout << "delete existing: " << hn << endl;
         CleanWindowLists(hist);
         delete hist; 
         hist = NULL;
      }

      if (!hist) {
      	fRootFile=new TFile(fname);
         if (strlen(dir) > 0) fRootFile->cd(dir);
	//      cout << "GetHist: |" << shname.Data()<< "|"<< endl;
      	hist = (TH1*)gDirectory->Get(shname.Data());
      	if (hist) {
            hist->SetDirectory(gROOT);
            TDatime dt = gDirectory->GetModificationDate();
            hist->SetUniqueID(dt.Convert());
      	   hist->SetName((const char*)newhname);
         } else {
            cout << "Histogram: " << hname << 
                    " not found in: " << fname << endl;
         }
      	fRootFile->Close();
      } 
//       else cout << "Use existing: " << hn << endl;
   
   } else if (strstr(fname,"Socket")) {
      if(!fComSocket){
          cout << setred << "No connection open"  << setblack << endl;
          return NULL;
      }
//      Bool_t ok;
      hist = gethist(hname, fComSocket);
      if (!hist){
         cout << setred << "Cant get hist, connection lost?" << setblack << endl;
         fComSocket->Close("force");
         fComSocket = NULL;
      } else {
         hist->SetUniqueID(0);
      }

   } else if (strstr(fname,".map")) {
//     look if this hist is in a list of histograms of possibly automatically
//     updated hists
      Bool_t inlist = kFALSE;
      if (fHistListList->GetSize() > 0) {
         TIter next(fHistListList);
         TString temp = fname;
         temp += " ";
         temp += hname;
         while ( TList * oc = (TList *)next()) {
//            TObjString * os = (TObjString *)oc->FindObject(temp.Data());
            if (oc->FindObject(temp.Data())) {
               cout << "Found inlist: " << temp.Data() << endl;            
               inlist = kTRUE;
               break;
            }
         }
      }
      if (!inlist) {
         TH1* temp = (TH1*)gROOT->FindObject(shname.Data());
         if (temp) {
 //           cout << "GetHist(map): Deleting existing: " << hname 
 //                << " at: " << temp << endl;
            CleanWindowLists(temp);
            temp->Delete();
         }
      }
      TMapFile *mfile;
      mfile = TMapFile::Create(fname);
      hist  = (TH1 *) mfile->Get(shname.Data());
      hist->SetUniqueID(0);
      if (inlist) {
         shname.Prepend("mf_");
         hist->SetName(shname.Data());
      }
      mfile->Close();
   } else {
      hist=(TH1*)gROOT->GetList()->FindObject(shname.Data());
   }
   gDirectory=gROOT;
   return hist;
}
//_____________________________________________________________________________________________

void HistPresent::CleanWindowLists(TH1* hist)
{
   if (!hist) return;
//   cout << "CleanWindowLists" << endl; 
   TList *lof = hist->GetListOfFunctions();
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
         fAllCuts->Remove(p);
         fAllWindows->Remove(p);
         fAllFunctions->Remove(p);
         fSelectCut->Remove(p);
         fSelectWindow->Remove(p);
      }
   } 
}
//_______________________________________________________________________
  
void HistPresent::ShowHist(const char* fname, const char* dir, const char* hname, const char* bp)
{
   TH1* hist = GetHist(fname, dir, hname);
   if (hist) {
      if (bp) {
         TButton * b;
         b = (TButton *)strtoul(bp, 0, 16);
//         cout << "bp " << b << endl;
         if (b) {
           b->SetBit(kSelected);
           b->SetFillColor(3);
           b->Modified(kTRUE);
           b->Update();}
      }
//      TurnButtonGreen(&activeHist);
      ShowHist(hist, hname);   
   } else     WarnBox("Histogram not found");
}
//_______________________________________________________________________

FitHist * HistPresent::ShowHist(TH1* hist, const char* hname) 
{
   static TString FHnameSave;
   static FitHist *fh=0;
   const char * origname;
   origname = hname;
   if (origname == 0) origname = hist->GetName();
   TString FHname("F");
   FHname += hist->GetName();
//   cout << "FHname " << FHname<< endl;
   TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
   while (FHname.Index(notascii) >= 0) {
      FHname(notascii) = "_";
   }
//   while (FHname.Index(" ") >= 0) {
//      FHname(mspace) = "_";
//   }
//   cout << "FHname " << FHname<< endl;
   if (gDirectory) {
      TList *tl=gDirectory->GetList();   
      FitHist *fhist = (FitHist*)tl->FindObject(FHname);
      if (fhist) {
//         Warning("In ShowHist:"," Delete  existing : %s",FHname.Data());
//         gDirectory->GetList()->Remove(hold);
         delete fhist;
      }
//     Cleaning FitHist objects for which Canvas was closed
      TIter next(tl);
      while ( TObject* obj=next()) {
         if (!(strncmp(obj->ClassName(),"FitHist",7))) {
            fhist=(FitHist*)obj;
            const char * cn=fhist->GetCanvasName();
            if (cn) {
               if (!(gROOT->FindObject(cn))) {
                  cout << "Deleting unused: " << fhist << " " << fhist->GetName() << endl;
                  fhist->Delete();
               }
            }
         }
      }
//      AppendDirectory();
   }
//   gROOT->Reset();
   nHists++;
   if (FHnameSave != FHname) {
      if (fNwindows>0) {       // not the 1. time
         if (fWinshiftx != 0 && fNwindows%2 != 0) fWincurx += fWinshiftx;
         else   {fWincurx = fWintopx; fWincury += fWinshifty;}
      }
      fNwindows++;
      FHnameSave = FHname;
   }
   if (fNwindows > 10) {
//       cout << "fLastWindow " << fLastWindow <<endl;
      if (QuestionBox("More than 10 hists on screen!! Remove them?", fLastWindow))
          CloseAllCanvases();        
   }
   Int_t wwidx; 
   Int_t wwidy; 

   if (hist->GetDimension() == 1) {
     wwidx = fWinwidx_1dim;
     wwidy = fWinwidy_1dim;
   } else {
      wwidx = fWinwidx_2dim;
      wwidy = fWinwidy_2dim;
   }
   fh=new FitHist((const char*)FHname,"A FitHist object",hist, origname,
          fWincurx, fWincury, wwidx, wwidy);
   fLastWindow = fh->GetMyCanvas();
   return fh;
}
//_______________________________________________________________________

void HistPresent::CloseAllCanvases() 
{
//     Cleaning all FitHist objects
   fCanvasList->Delete();
   fNwindows= 0;
   fWincury = fWintopy;
   fWincurx = fWintopx;
   if (fHelpBrowser) fHelpBrowser->Clear();
}
//_______________________________________________________________________
  
void HistPresent::StackSelectedHists(const char *bp) 
{
   StackSelectedHists(fSelectHist);
}
//_______________________________________________________________________
  
void HistPresent::StackSelectedHists(TList * hlist, const char* title) 
{
   Int_t nsel = hlist->GetSize();
   Int_t nx = 1, ny =1;
   TH1* hist = 0;
   if (nsel == 0) {
      WarnBox("No histogram selected");
      return;

   } else if (nsel > 32) {
      WarnBox("Maximum 32 histograms allowed");
      return;
   } 
//  make a copy of  hlist, add its pointer to list of histlists fHistListList
//  pass its pointer to HTCanvas,
//  destructor of HTCanvas shall delete the list and remove its 
//  pointer from fHistListList
   TList * savelist = new TList();
   TIter next(hlist);
   while ( TObjString * objs = (TObjString*)next()) {
      savelist->Add(new TObjString(*objs));
   }
   fHistListList->Add(savelist);
//   savelist->Print();

 	TMrbString wwhx = "HistPresent.WindowXWidth_";
	wwhx += nx;
	wwhx += "x";
	wwhx += ny;
	TMrbString wwhy = "HistPresent.WindowYWidth_";
	wwhy += nx;
	wwhy += "x";
	wwhy += ny;
   TEnv env(".rootrc");		// inspect ROOT's environment
	Int_t wwx   =   env.GetValue(wwhx.Data(), 0);
	if (wwx == 0) wwx   =   env.GetValue("HistPresent.WindowXWidth_many", 800);
	Int_t wwy   =   env.GetValue(wwhy.Data(), 0);
	if (wwy == 0) wwy   =   env.GetValue("HistPresent.WindowYWidth_many", 800);


   TString buf("cstack_");
   buf += fSeqNumberMany++;
   const char * tit = buf.Data();
   if (title) tit = title;
   HTCanvas * cmany = 
       new HTCanvas(buf.Data(), tit, fWincurx, fWincury, wwx, wwy,
                    this, 0, savelist);
   fWincurx = fWintopx; fWincury += fWinshifty;

   fCanvasList->Add(cmany);
   TString hname;
   TString stitle("Stacked: ");
   
//   TString ytitle("");
   Int_t nbins; 
   THStack * hs = new THStack("hstack","");
   fAnyFromSocket = kFALSE;
   for(Int_t i=0; i<nsel; i++) {
      hist = GetSelHistAt(i, hlist); 
      if (!hist) {
//         cout << " Hist not found at: " << i << endl;  
         continue;
      }  
      if (i == 0) {
         nbins = hist->GetNbinsX();
      } else {
         if (nbins != hist->GetNbinsX()) {
            cout << "Number of bins " << hist->GetNbinsX() << " in "
                 << hist->GetName() << " differ, expected: " <<  nbins << endl;
            continue;
         }
      }
      TString fname = ((TObjString *)hlist->At(i))->String();
      if (fname.Index("Socket") == 0) fAnyFromSocket = kTRUE;
      hname = hist->GetName();
 //     cout << "Bef: " << hname << endl;

      Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
      if(last_us >0)hname.Remove(last_us);
      last_us = hname.Last(';');    // chop off version
      if (last_us >0) hname.Remove(last_us);
      if (stitle.Length() > 0) stitle += ", ";
      stitle += hname.Data();
//      cout << "Aft: "  << hname << endl;
      hs->Add(hist);
      hist->SetFillColor(i+2);
      hist->SetFillStyle(1001);
      hist->SetDrawOption("hist");
   }
   if (fRealStack)hs->Draw();
   else           hs->Draw("nostack");
   hs->SetTitle(stitle.Data());
   cmany->Modified();
   cmany->Update();
}
//_______________________________________________________________________
  
void HistPresent::ShowSelectedHists(const char *bp) 
{
   ShowSelectedHists(fSelectHist);
}
//_______________________________________________________________________
  
void HistPresent::ShowSelectedHists(TList * hlist, const char* title) 
{
   Int_t nsel = hlist->GetSize();
   Int_t nx = 1, ny =1;
   TH1* hist = 0;
   if (nsel == 00) {
      WarnBox("No histogram selected");
      return;

   } else if (nsel > 32) {
      WarnBox("Maximum 32 histograms allowed");
      return;
   } 

//  make a copy of  hlist, add its pointer to list of histlists fHistListList
//  pass its pointer to HTCanvas,
//  destructor of HTCanvas shall delete the list and remove its 
//  pointer from fHistListList
   TList * savelist = new TList();
   TIter next(hlist);
   while ( TObjString * objs = (TObjString*)next()) {
      savelist->Add(new TObjString(*objs));
   }
   fHistListList->Add(savelist);
//   savelist->Print();
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

 	TMrbString wwhx = "HistPresent.WindowXWidth_";
	wwhx += nx;
	wwhx += "x";
	wwhx += ny;
	TMrbString wwhy = "HistPresent.WindowYWidth_";
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
   buf += fSeqNumberMany++;
   const char * tit = buf.Data();
   if (title) tit = title;
   HTCanvas * cmany = 
       new HTCanvas(buf.Data(), tit, fWincurx, fWincury, wwx, wwy,
                    this, 0, savelist);
   fWincurx = fWintopx; fWincury += fWinshifty;

   fCanvasList->Add(cmany);
   cmany->Divide(nx, ny);
//   cmany->SetEditable(kTRUE);
   TEnv * lastset = 0;
   TString hname;
//   TString xtitle("");
//   TString ytitle(""); 
   fAnyFromSocket = kFALSE;
   TPad * firstpad = NULL;
   for(Int_t i=0; i<nsel; i++) {
      cmany->cd(i+1);
      TPad * p = (TPad *)gPad;
      if (firstpad == NULL) firstpad = p;
      hist = GetSelHistAt(i, hlist); 
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

      if (is2dim(hist)) {
         hist->Draw(fDrawOpt2Dim->Data());
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
   		TString cmd2("((HistPresent*)gROOT->FindObject(\""); 
   		cmd2 += GetName();
   		cmd2 += "\"))->auto_exec_2()";
         p->AddExec("ex2", cmd2.Data());
      } else {
         if (lastset && lastset->GetValue("LogY", 0) )p->SetLogy();
         TString drawopt;
         gStyle->SetOptTitle(GetShowTitle());
         if (fShowContour) drawopt = "hist";
         if (fShowErrors)  drawopt += "e1";
         if (fFill1Dim) {
            hist->SetFillStyle(1001);
            hist->SetFillColor(44);
         } else hist->SetFillStyle(0);
         hist->Draw(drawopt.Data());
//         hist->Draw(fDrawOpt2Dim->Data());
   		TString cmd1("((HistPresent*)gROOT->FindObject(\""); 
   		cmd1 += GetName();
   		cmd1 += "\"))->auto_exec_1()";
         p->AddExec("ex1", cmd1.Data());
      }  
      SetCurrentHist(hist);
      gPad->Modified(kTRUE);
      gPad->Update();
      
      if (fUseAttributeMacro && !gSystem->AccessPathName(attrname, kFileExists)) {
         gROOT->LoadMacro(attrname);
         TString cmd = attrname;
         cmd.Remove(cmd.Index("."),100);
         cmd = cmd + "(\"" + gPad->GetName()  + "\",\"" + hist->GetName() + "\")";
//         cout << cmd << endl;
         gROOT->ProcessLine((const char *)cmd);
         gPad->Modified(kTRUE);
         gPad->Update();
      }
   }
   if (firstpad) { 
      if (fShowAllAsFirst) 
         ShowAllAsSelected(firstpad, cmany, 0, NULL);
      firstpad->cd();
   }
   cmany->SetEditable(kTRUE);
//   cout << "firstpad " << setbase(16) << firstpad << endl;
}
//____________________________________________________________________________

void HistPresent::WarnBox(const char *message)
{
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), GetMyCanvas(),
                "Warning", message, kMBIconExclamation, kMBDismiss,
                &retval);
}
//____________________________________________________________________________

void HistPresent::DinA4Page(Int_t form)
{
//   gROOT->SetStyle("Plain");
   HTCanvas *c1; 
   if (form == 1) {
      c1= new HTCanvas("dina4page", "A DIN a4 page landscape",
                  350,40,1004, 759, this);
      c1->Range(0, 0, 210. * TMath::Sqrt(2.), 210);
   } else if (form == 0) {
      c1= new HTCanvas("dina4page", "A DIN a4 page portrait",
                  750,40,  712, 1050, this);
      c1->Range(0, 0, 210, 210. * TMath::Sqrt(2.));
   }
   GetCanvasList()->Add(c1);
   c1->SetRightMargin(0);
   c1->SetLeftMargin(0);
   c1->SetBottomMargin(0);
   c1->SetTopMargin(0);
   c1->SetGrid(10, 10);
   c1->Modified(kTRUE);
   c1->Update();
   c1->SetEditable(kTRUE);
   c1->GetCanvasImp()->ShowEditor();
   c1->GetCanvasImp()->ShowToolBar();
}
//________________________________________________________________________________________
// Show Canvas
  
void HistPresent::ShowCanvas(const char* fname, const char* name, const char* bp)
{
   HTCanvas *c;
   if (strstr(fname,".root")) {
      if (fRootFile) fRootFile->Close();
      fRootFile=new TFile(fname);
      c = (HTCanvas*)fRootFile->Get(name);
   } else {
      c=(HTCanvas*)gROOT->FindObject(name);
   }
   if (c) {
      TString tempname(c->GetName());
      if (!tempname.Contains("dina4page")) {
         c->Draw();
         return;
      }
      c->SetName("abcxyz");
      UInt_t ww, wh;
      ww = c->GetWw() + 2 * (gStyle->GetCanvasBorderSize() + 1);
      wh =  c->GetWindowHeight();
      HTCanvas * c1 = new HTCanvas(tempname.Data(), c->GetTitle(),
                         c->GetWindowTopX(), c->GetWindowTopY(),
                         ww, wh, this);
//                         c->GetWindowWidth(), c->GetWindowHeight(), this);

//      c1->SetCanvasSize(c->GetWw(), c->GetWh());

      Double_t x1, y1, x2, y2;
      c->GetRange(x1, y1, x2, y2);
      c1->Range(x1, y1, x2, y2);

//      cout << c->GetWindowWidth() << " " << c->GetWindowHeight() << " "
//           << c->GetWw() << " " << c->GetWh() << " "
//           << x1 << " "<<  y1 << " "<<  x2 << " " << y2 << endl;

      TList * l = c->GetListOfPrimitives();
      TList * l1 = c1->GetListOfPrimitives();
      TIter next(l);
      TObject * obj;
      TH1* h;
      while ( (obj = (TObject*)next()) ) {
         if (obj->InheritsFrom(TH1::Class())) {
            h = (TH1*)obj;
            h->SetDirectory(gROOT);
//            cout << obj->GetName() << endl;
         } else {
   
//            cout << obj->GetName() << endl;
//            obj->Dump();
            if (obj->InheritsFrom("TPad")){
               TPad * newp = (TPad * )obj->Clone();
               newp->SetCanvas(c1);
               l1->Add(newp);
//               newp->Dump();
            } else
              l1->Add(obj);
         }
      }
      delete c;
//      c1->SetName(tempname.Data());

   	c1->SetRightMargin(0);
   	c1->SetLeftMargin(0);
   	c1->SetBottomMargin(0);
   	c1->SetTopMargin(0);
   	c1->SetGrid(10, 10);
   	c1->Modified(kTRUE);
   	c1->Update();
   	c1->SetEditable(kTRUE);
      c1->GetCanvasImp()->ShowEditor();
      c1->GetCanvasImp()->ShowToolBar();
   }
   if (fRootFile) fRootFile->Close();
}
