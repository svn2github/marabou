#include "TRegexp.h"
#include "TTree.h"
#include "TBranch.h"
#include "TBranchObject.h"
#include "TLeafObject.h"
#include "TGMsgBox.h"
#include "TRootCanvas.h"
#include "TCanvasImp.h"

#include "HistPresent.h"
#include "FitHist.h"
#include "FhContour.h"
#include "FitHist_Help.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "TMrbStatistics.h"
#include "HTCanvas.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbHelpBrowser.h" 
#include "TGMrbInputDialog.h"
#include "CmdListEntry.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>

//________________________________________________________________________________________
// Show Tree 
  
void HistPresent::ShowTree(const char* fname, const char* dir, const char* tname, const char* bp)
{
  static Int_t ycanvas=5;
//  const Int_t MAXLEAF=33;
  if (fRootFile) fRootFile->Close();
  fRootFile=new TFile(fname);
  fRootFile->ls();
  if (strlen(dir) > 0) fRootFile->cd(dir);
  TTree *tree = (TTree*)gDirectory->Get(tname);
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

//   cmd = "mypres->UseHist()";
//   nam = "Use selected hist";
//   sel = "mypres->ToggleUseHist()";
//   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));

   cmd = "mypres->ShowLeaf(\"";
   cmd = cmd + fname + "\",\"" + dir + "\",\"" + tname + "\",\"\")";
   nam = "Show leafs";
   fCmdLine->Add(new CmdListEntry(cmd, nam, empty, empty));

   TString cmd_base("mypres->ShowLeaf(\"");
   TString sel_base("mypres->SelectLeaf(\"");
   cmd_base = cmd_base + fname + "\",\"" + dir + "\",\"" + tname + "\",\"";
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
   HTCanvas *ccont = CommandPanel("TreeList", fCmdLine, 245, ycanvas, this);
   if (fHistLists)fHistLists->Add(ccont);
   ycanvas += 50;
   if (ycanvas >= 500) ycanvas=5;
   fCmdLine->Delete(); 

}
//________________________________________________________________________________________
  
void HistPresent::SelectLeaf(const char* lname, const char* bp)
{
   if (!bp) return;
//   cout << " Adding leaf " << lname << endl;
   const char * aname[4] = {"X-axis", "Y-axis", "Z-axis", "4-axis"};
   TString sel = lname;
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
   if (b->TestBit(kSelected)) {
      TObjString tobjs((const char *)sel);
      cout << "Remove leaf: " << tobjs.GetString() << endl;
      fSelectLeaf->Remove(&tobjs);
      b->SetFillColor(16);
//      b->SetTitle("");
      b->ResetBit(kSelected);
   } else {
      fSelectLeaf->Add(new TObjString((const char *)sel));     
//      cout << " Add leaf " << lname << " Index: " << fSelectLeaf->LastIndex()<< endl;
//      b->SetTitle(Form("%d", fSelectLeaf->LastIndex()));
      b->SetFillColor(3);
      b->SetBit(kSelected);
   }
//   b->Paint();
   if (fSelectLeaf->GetSize() > 0) {
      cout << "Current selection:" << endl;
      for (Int_t i = 0; i < fSelectLeaf->GetSize(); i++) 
         cout << i << " " << aname[i] << ": " <<  ((TObjString*)fSelectLeaf->At(i))->GetString() << endl;
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
  
void HistPresent::EditExpression(const char* bp)
{
   Bool_t ok;
   *fExpression=GetString("Edit expression",(const char *)*fExpression,
                          &ok, GetMyCanvas(), 0,0,0,0,0,
                          "ntupleCmds.txt");
   if (!ok) return;
   if (strlen(*fExpression) > 1) {
      cout << *fExpression<< endl;
      cout << "Press grey button to activate" << endl;
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

void HistPresent::ShowLeaf( const char* fname, const char* dir, const char* tname,
                            const char* leafname, const char* bp)
{

   TString leaf0;
   TString leaf1;
   TString leaf2;
   TString leaf3;
   TString * leaf[4] = {&leaf0, &leaf1, &leaf2, &leaf3};

   TString cmd;
   TString hname = "hist_";
   Int_t nent = 1;
   Double_t nbin[4] = {0, 0, 0, 0};
   Double_t vmin[4] = {0, 0, 0, 0};
   Double_t vmax[4] = {0, 0, 0, 0};
//   cout << "leafname " << leafname << " " << strlen(leafname) 
//    << " nent " << fSelectLeaf->GetSize() << endl;
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
            cmd.Prepend(":");
            cmd.Prepend(leaf1.Data());
         }
         if (nent == 3) {
            objs = (TObjString *)next();
            leaf2 = objs->String();
            cmd.Prepend(":");
            cmd.Prepend(leaf2.Data());
         }
      }
   }

   if (fApplyExpression) {
      cmd = *fExpression;
      cout << cmd << endl;
      if (nent == 0) {
//         find # of dimensions
         Int_t sind = 0, colind;
   		while (1) {
      		colind = cmd.Index(":", sind);
      		if (colind < 0) break;
      		if (cmd[colind+1] == ':') {
               sind = colind + 2;
               continue;
            }
            *leaf[nent] = cmd(sind, colind-sind);
             
            sind = colind + 1;
            nent++;
   		} 
         *leaf[nent] = cmd(sind, cmd.Length() - sind);
         nent++;
      }
//     invert order
      TString * temp;
      if (nent == 2) {
         temp = leaf[0];
         leaf[0] = leaf[1];
         leaf[1] = temp;
      } else if (nent == 3) {
         temp = leaf[0];
         leaf[0] = leaf[2];
         leaf[2] = temp;
      } else if (nent == 4) {
         temp = leaf[0];
         leaf[0] = leaf[3];
         leaf[3] = temp;
         temp = leaf[1];
         leaf[1] = leaf[2];
         leaf[2] = temp;
      }
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
   if (nent > 0) hname += leaf0;
   else hname += "userdef";
   if (nent > 1) hname += "_"; hname += leaf1;
   if (nent > 2) hname += "_"; hname += leaf2;

   TString option = "goff";

   TString cmd_orig(cmd);
   cmd += ">>";
/*
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
*/
      if (fNtupleVersioning) {
         hname += "_v";
         hname += fNtupleSeqNr;
         fNtupleSeqNr++;
      }
      TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
      while (hname.Index(notascii) >= 0) {
         hname(notascii) = "_";
      }

      TString fh_name(hname);
      fh_name.Prepend("F");

//      cout << "look for:" << fh_name << endl;
      TObject *fobj = (TObject *)gROOT->FindObject(fh_name.Data());
      if (fobj) {
         cout << " deleting " << fh_name << endl;
         delete fobj;
      }
      fobj = (TObject *)gROOT->FindObject(hname.Data());
      if (fobj) {
         cout << " deleting " << hname << endl;
         delete fobj;
      }
//   }
   cmd += hname;

   gDirectory=gROOT;
//   if (nent==3) new TCanvas("ctemp","ctemp",800,800);

   if (fRootFile) fRootFile->Close();
   fRootFile=new TFile(fname);
   if (strlen(dir) > 0) fRootFile->cd(dir);
   TTree *tree = (TTree*)gDirectory->Get(tname);

   gDirectory = gROOT;

   TEnv * env = 0;
   Bool_t limits_defined = kFALSE;
   TString tag = leaf0.Data(); 
   if (nent > 0 && fRememberTreeHists) {
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
            tag = *leaf[i]; tag += ".nbin";
            if (env->Lookup(tag.Data())) {
               nb = (Int_t)nbin[i]; 
               nb = env->GetValue(tag, nb); nbin[i] = nb;
            } else { 
               limits_defined = kFALSE;
            }
            tag = *leaf[i]; tag += ".min";
            if (env->Lookup(tag.Data())) {
               vmin[i] = atof(env->GetValue(tag.Data(), Form("%f",vmin[i])));
            }else {
               limits_defined = kFALSE;
            }

            tag = *leaf[i]; tag += ".max";
            if (env->Lookup(tag.Data())) {
               vmax[i] = atof( env->GetValue(tag.Data(), Form("%f",vmax[i])));
            }else {
               limits_defined = kFALSE;
            }

         }
      }
   }
//  look if we need to redefine limits
   Bool_t must_find_limits = kFALSE;
   for(Int_t i = 0; i < nent; i++) {
     if (nbin[i] == 0) must_find_limits = kTRUE;
   } 
   if (must_find_limits) limits_defined = kFALSE;

   Bool_t modified = kFALSE;
   if (must_find_limits) {
      tree->Draw(cmd_orig.Data(),"","goff");
      TH1* htemp = (TH1*)gDirectory->FindObject("htemp");
      if (htemp) {
         for(Int_t i = 0; i < nent; i++) {
            if (nbin[i] > 0) continue;
            modified = kTRUE;
            TAxis * a;
            if      (i == 0) a = htemp->GetXaxis();
            else if (i == 1) a = htemp->GetYaxis();
            else if (i == 2) a = htemp->GetZaxis();
         	vmin[i] = a->GetXmin();
         	vmax[i] = a->GetXmax();
            cout << i << " " << vmin[i] << " " << vmax[i] << endl;
            nbin[i] = 100;
         }
      }
   }
   if (!limits_defined || fAlwaysNewLimits) {
      TOrdCollection *row_lab = new TOrdCollection(); 
      TOrdCollection *col_lab = new TOrdCollection();
      col_lab->Add(new TObjString("Nbins"));
      col_lab->Add(new TObjString("Min"));
      col_lab->Add(new TObjString("Max"));
      row_lab->Add(new TObjString((*leaf[0]).Data()));
      if (nent > 1 ) row_lab->Add(new TObjString((*leaf[1]).Data()));
      if (nent == 3) row_lab->Add(new TObjString((*leaf[2]).Data()));
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
//      cout << "pwin " << pwin << endl;

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
      for(Int_t i = 0; i < nent; i++) { 
         if   (nbin[i] != xyvals[p]) {
            nbin[i] = xyvals[p];
            modified = kTRUE;
         } 
         p++;
      }
      for(Int_t i = 0; i < nent; i++) { 
         if   (vmin[i] != xyvals[p]) {
            vmin[i] = xyvals[p];
            modified = kTRUE;
         } 
         p++;
      }
      for(Int_t i = 0; i < nent; i++) { 
         if   (vmax[i] != xyvals[p]) {
            vmax[i] = xyvals[p];
            modified = kTRUE;
         } 
         p++;
      }
//      delete [] xyvals;
      if (modified && fRememberTreeHists) {
         Int_t buttons= kMBYes | kMBNo, retval;
         EMsgBoxIcon icontype = kMBIconQuestion;
         new TGMsgBox(gClient->GetRoot(), pwin,
          "Qustion","Save values to ntuplerc",
          icontype, buttons, &retval);
         if (retval == kMBYes) {
            for(Int_t i = 0; i < nent; i++) {
               tag = *leaf[i]; tag += ".nbin";
               env->SetValue(tag.Data(),(Int_t)nbin[i]);
               tag = *leaf[i]; tag += ".min";
               env->SetValue(tag.Data(),vmin[i]);
               tag = *leaf[i]; tag += ".max";
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
   TObject *obj = (TObject *)gROOT->FindObject(hname.Data());
//  if (!obj)WarnBox("No Object"); 
   TH1* hist = (TH1*)obj;
   if (hist) {
         hist->GetXaxis()->SetTitle(*leaf[0]);
         if (nent >= 2) hist->GetYaxis()->SetTitle(*leaf[1]);
         if (nent >= 3) hist->GetZaxis()->SetTitle(*leaf[2]);
         ShowHist(hist);
   } else {
      WarnBox("No hist"); 
   }   
}

//________________________________________________________________________________________
