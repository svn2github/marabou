#include "TROOT.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaletteAxis.h"
#include "TPavesText.h"
#include "SetHistOptDialog.h"
#include <iostream>

namespace std {} using namespace std;
/*
Int_t   SetHistOptDialog::fFill1Dim      = 0;
Color_t SetHistOptDialog::fHistFillColor = 0;
Color_t SetHistOptDialog::fHistLineColor = 1;
Style_t SetHistOptDialog::fHistFillStyle = 0;
Style_t SetHistOptDialog::fHistLineStyle = 1;
Width_t SetHistOptDialog::fHistLineWidth = 2;
Float_t SetHistOptDialog::fEndErrorSize  =0.1;
Int_t   SetHistOptDialog::fErrorX        = 1;
Int_t   SetHistOptDialog::fDrawErrors       = 1;
Int_t    SetHistOptDialog::fNdivisionsX = 510;
Int_t    SetHistOptDialog::fNdivisionsY = 510;
Int_t    SetHistOptDialog::fNdivisionsZ = 510;
Color_t  SetHistOptDialog::fAxisColor = 0;
Color_t  SetHistOptDialog::fLabelColor = 0;
Style_t  SetHistOptDialog::fLabelFont = 0;
Float_t  SetHistOptDialog::fLabelOffsetX = 0;
Float_t  SetHistOptDialog::fLabelOffsetY = 0;
Float_t  SetHistOptDialog::fLabelOffsetZ = 0;
Float_t  SetHistOptDialog::fLabelSize = 0;
Float_t  SetHistOptDialog::fTickLength = 0;
Float_t  SetHistOptDialog::fTitleOffsetX = 0;
Float_t  SetHistOptDialog::fTitleOffsetY = 0;
Float_t  SetHistOptDialog::fTitleOffsetZ = 0;
Float_t  SetHistOptDialog::fTitleSize = 0;
Color_t  SetHistOptDialog::fTitleColor = 0;
Style_t  SetHistOptDialog::fTitleFont = 0;
Int_t    SetHistOptDialog::fTitleCenterX = 0;
Int_t    SetHistOptDialog::fTitleCenterY = 0;
Int_t    SetHistOptDialog::fTitleCenterZ = 0;
*/
//_______________________________________________________________________

SetHistOptDialog::SetHistOptDialog(TGWindow * win, TCollection * hlist)
{
   fWindow = win;
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
// are there 1 dim hists?
	fHistList = hlist;
	TIter *next;
	if (fHistList == NULL) {
		next= new TIter(fCanvas->GetListOfPrimitives());
	} else {
		next = new TIter(fHistList);
	}
   TObject *obj;
   fHist = NULL;
   Int_t nh1 = 0, nh2 = 0;
   while ( (obj = (*next)()) ) {
      if (obj->InheritsFrom("TH1")) {
         fHist = (TH1*)obj;
		}
      if (obj->InheritsFrom("TGraph")) {
         fHist = ((TGraph*)obj)->GetHistogram();
		}
      if (fHist && fHist->GetDimension() == 1) nh1++;
      if (fHist && fHist->GetDimension()  > 1) nh2++;
   }
	if ( fHist == NULL ) {
	   cout << "No Histogram in Canvas" << endl;
	}
   RestoreDefaults();
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();
   static void *fValp[100];
   Int_t ind = 0;
   static Int_t dummy;
   static TString sgdcmd("SetHistAttPermLocal()");
   static TString stycmd("SetCustomStyle()");
   Int_t div = TMath::Abs(fNdivisionsX);
   fPdivX = div%100;
   fSdivX = (div/100)%100;
   fTdivX = (div/10000)%100;
   fOptimizedivX = 1;
   if (fNdivisionsX < 0) fOptimizedivX = 0;

   div = TMath::Abs(fNdivisionsY);
   fPdivY = div%100;
   fSdivY = (div/100)%100;
   fTdivY = (div/10000)%100;
   fOptimizedivY = 1;
   if (fNdivisionsY < 0) fOptimizedivY = 0;

   div = TMath::Abs(fNdivisionsZ);
   fPdivZ = div%100;
   fSdivZ = (div/100)%100;
   fTdivZ = (div/10000)%100;
   fOptimizedivZ = 1;
   if (fNdivisionsZ < 0) fOptimizedivZ = 0;

   fTitleCmd1 = ind;
   fRow_lab->Add(new TObjString("CommentOnly_Histogram Title Attributes"));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("ColorSelect_BgColor"));
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fRow_lab->Add(new TObjString("PlainShtVal+BorderSize"));
   fValp[ind++] = &fTitleColor;
   fValp[ind++] = &fTitleStyle;
   fValp[ind++] = &fTitleBorderSize;

   fRow_lab->Add(new TObjString("CfontSelect_TextFont"));
   fRow_lab->Add(new TObjString("ColorSelect+TextColor"));
   fRow_lab->Add(new TObjString("Float_Value+FontSize;0;0.1"));
   fValp[ind++] = &fTitleFont;
   fValp[ind++] = &fTitleTextColor;
   fValp[ind++] = &fTitleFontSize;

   fRow_lab->Add(new TObjString("Float_Value_X;0;1"));
   fRow_lab->Add(new TObjString("Float_Value+Y;0;1"));
   fRow_lab->Add(new TObjString("AlignSelect+Align"));
//   fRow_lab->Add(new TObjString("Float_Value_H"));
//   fRow_lab->Add(new TObjString("AlignSelect+Align"));
   fValp[ind++] = &fTitleX;
   fValp[ind++] = &fTitleY;
   fValp[ind++] = &fTitleAlign;
//   fValp[ind++] = &fTitleH;
   fTitleCmd2 = ind - 1;
   fStatCmd1 = ind;
   fRow_lab->Add(new TObjString("CommentOnly_Statistic Box Attributes"));
   fValp[ind++] = &dummy;
//   fRow_lab->Add(new TObjString("Float_Value_X (upright);0;1"));
//   fRow_lab->Add(new TObjString("Float_Value+Y (upright);0;1"));
   fRow_lab->Add(new TObjString("Float_Value_Width ;0;1        "));
   fRow_lab->Add(new TObjString("Float_Value+Height ;0;1        "));
   fRow_lab->Add(new TObjString("PlainShtVal+BorderSz"));
   fRow_lab->Add(new TObjString("ColorSelect_BGCol"));
   fRow_lab->Add(new TObjString("ColorSelect+TextCol "));
   fRow_lab->Add(new TObjString("Fill_Select+Style (0 transp)"));
   fRow_lab->Add(new TObjString("CfontSelect_Font      "));
   fRow_lab->Add(new TObjString("Float_Value+FontSize;0;1  "));
   fRow_lab->Add(new TObjString("StringValue+Format"));

//    fValp[ind++] = &fStatX;
//    fValp[ind++] = &fStatY;
   fValp[ind++] = &fStatW;
   fValp[ind++] = &fStatH;
   fValp[ind++] = &fStatBorderSize;
   fValp[ind++] = &fStatColor;
   fValp[ind++] = &fStatTextColor;
   fValp[ind++] = &fStatStyle;
//   fStatFont /= 10;
   fValp[ind++] = &fStatFont;
   fValp[ind++] = &fStatFontSize;
   fValp[ind++] = &fStatFormat;
   fStatCmd2 = ind - 1;
 //  fRow_lab->Add(new TObjString("CommentOnly_X Axis Div: PrimX SecX TertX  PrimY SecY TertY Optimize"));
   fRow_lab->Add(new TObjString("CommentOnly_AxisDiv"));
   fRow_lab->Add(new TObjString("CommentOnly+Prim"));
   fRow_lab->Add(new TObjString("CommentOnly+Sec"));
   fRow_lab->Add(new TObjString("CommentOnly+Tert"));
   fRow_lab->Add(new TObjString("CommentOnly+Optimize"));
   fRow_lab->Add(new TObjString("CommentOnly+TickSide"));
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;

   fRow_lab->Add(new TObjString("CommentOnly_X"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("CheckButton+"));
   fRow_lab->Add(new TObjString("StringValue+"));
   fValp[ind++] = &dummy;
   fValp[ind++] = &fPdivX;
   fValp[ind++] = &fSdivX;
   fValp[ind++] = &fTdivX;
   fValp[ind++] = &fOptimizedivX;
   fValp[ind++] = &fTickSideX;

   fRow_lab->Add(new TObjString("CommentOnly_Y"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("CheckButton+"));
   fRow_lab->Add(new TObjString("StringValue+"));
   fValp[ind++] = &dummy;
   fValp[ind++] = &fPdivY;
   fValp[ind++] = &fSdivY;
   fValp[ind++] = &fTdivY;
   fValp[ind++] = &fOptimizedivY;
   fValp[ind++] = &fTickSideY;
   if (nh2 > 0) {
		fRow_lab->Add(new TObjString("CommentOnly_Z"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("CheckButton+"));
		fRow_lab->Add(new TObjString("StringValue+"));
		fValp[ind++] = &dummy;
		fValp[ind++] = &fPdivZ;
		fValp[ind++] = &fSdivZ;
		fValp[ind++] = &fTdivZ;
		fValp[ind++] = &fOptimizedivZ;
      fValp[ind++] = &fTickSideZ;
   }

   fRow_lab->Add(new TObjString("ColorSelect_Axis Color"));
   fRow_lab->Add(new TObjString("Float_Value+TickLength"));
   fValp[ind++] = &fAxisColor;
   fValp[ind++] = &fTickLength;

   fRow_lab->Add(new TObjString("CommentOnly_Label Attributes"));
   fRow_lab->Add(new TObjString("ColorSelect_Col"));
   fRow_lab->Add(new TObjString("CfontSelect+Font"));
   fRow_lab->Add(new TObjString("Float_Value+Size;0;1"));
   fRow_lab->Add(new TObjString("PlainIntVal+MaxDigs"));
   fRow_lab->Add(new TObjString("Float_Value_Offset X"));
   fRow_lab->Add(new TObjString("Float_Value+Offset Y"));
   if (nh2 > 0)
      fRow_lab->Add(new TObjString("Float_Value+Offset Z"));
   fValp[ind++] =  &dummy;
   fValp[ind++] = &fLabelColor;
   fValp[ind++] = &fLabelFont;
   fValp[ind++] = &fLabelSize;
   fValp[ind++] = &fLabelMaxDigits;
   fValp[ind++] = &fLabelOffsetX;
   fValp[ind++] = &fLabelOffsetY;
   if (nh2 > 0)
      fValp[ind++] = &fLabelOffsetZ;
   fRow_lab->Add(new TObjString("CommentOnly_Axis Title Attributes"));
   fRow_lab->Add(new TObjString("ColorSelect_Color"));
   fRow_lab->Add(new TObjString("CfontSelect+Font"));
   fRow_lab->Add(new TObjString("Float_Value+Size;0;1"));
   fValp[ind++] = &dummy;
   fValp[ind++] = &fTitleColorA;
   fValp[ind++] = &fTitleFontA;
   fValp[ind++] = &fTitleSize;
   fRow_lab->Add(new TObjString("Float_Value_Off X"));
   fRow_lab->Add(new TObjString("Float_Value+Off Y"));
   if (nh2 > 0)
      fRow_lab->Add(new TObjString("Float_Value+Offs Z"));
   fRow_lab->Add(new TObjString("CheckButton+Cent X "));
   fRow_lab->Add(new TObjString("CheckButton+CenT Y"));
   if (nh2 > 0)
      fRow_lab->Add(new TObjString("CheckButton+Cent Z"));

   fValp[ind++] = &fTitleOffsetX;
   fValp[ind++] = &fTitleOffsetY;
   if (nh2 > 0)
      fValp[ind++] = &fTitleOffsetZ;
   fValp[ind++] = &fTitleCenterX;
   fValp[ind++] = &fTitleCenterY;
   if (nh2 > 0)
      fValp[ind++] = &fTitleCenterZ;

   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &sgdcmd;
   fRow_lab->Add(new TObjString("CommandButt+Create / Update custom style"));
   fValp[ind++] = &stycmd;

   static Int_t ok;
   Int_t itemwidth = 420;
   fDialog =
      new TGMrbValuesAndText("Axis Attributes", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//_______________________________________________________________________

void SetHistOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
//      cout << "SetHistOptDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void SetHistOptDialog::CloseDialog()
{
 //  cout << "SetHistOptDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void SetHistOptDialog::SetHistAttNow(TCanvas *canvas, Int_t bid)
{
   if (!canvas) return;
   fNdivisionsX  = fTdivX*10000 + fSdivX*100 + fPdivX;
   if (fOptimizedivX == 0) fNdivisionsX *= -1;
   fNdivisionsY  = fTdivY*10000 + fSdivY*100 + fPdivY;
   if (fOptimizedivY == 0) fNdivisionsY *= -1;
   fNdivisionsZ  = fTdivZ*10000 + fSdivZ*100 + fPdivZ;
   if (fOptimizedivZ == 0) fNdivisionsZ *= -1;
   SetHistOptDialog::SetHistAtt(canvas, bid);
}
//_______________________________________________________________________

void SetHistOptDialog::SetHistAtt(TCanvas *canvas, Int_t bid)
{
   if (!canvas) return;
   // remove title and statbox in case they changed,
   // they will be recomputed
//   cout << "SetHistAtt: " << bid<< endl;

//	Bool_t st = gStyle->GetOptTitle();
//	if (st && ((bid == 999) || (bid >= fTitleCmd1 && bid <= fTitleCmd2))) {
//		TObject *obj = canvas->GetListOfPrimitives()->FindObject("title");
//	   if (obj) {
//			cout << "Delete title: " << obj << endl;
//			obj->Delete();
//		}
//	}
	Bool_t mod_statbox =
     gStyle->GetOptStat() && ((bid == 999) || (bid >= fStatCmd1 && bid <= fStatCmd2));
	TIter *next;
	if (fHistList == NULL) {
		next= new TIter(fCanvas->GetListOfPrimitives());
	} else {
		next = new TIter(fHistList);
	}
//   TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   TH1 *hist;

   while ( (obj = (*next)()) ) {
      if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph")) {
         if (obj->InheritsFrom("TH1"))
             hist = (TH1*)obj;
         if (obj->InheritsFrom("TGraph")) {
             hist = ((TGraph*)obj)->GetHistogram();
             if (!hist) continue;
         }
         TAxis *xa = hist->GetXaxis();
         TAxis *ya = hist->GetYaxis();
         TAxis *za = NULL;
         if (hist->GetDimension() > 1)
            za = hist->GetZaxis();
			xa->SetNdivisions( fNdivisionsX);
			xa->SetAxisColor(  fAxisColor);
			xa->SetLabelColor( fLabelColor);
			xa->SetLabelFont(  fLabelFont);
			xa->SetLabelOffset(fLabelOffsetX);
			xa->SetLabelSize(  fLabelSize);
			xa->SetTickLength( fTickLength);
			xa->SetTicks( fTickSideX);
			xa->SetTitleOffset(fTitleOffsetX);
			xa->SetTitleSize(  fTitleSize);
			xa->SetTitleColor( fTitleColorA);
			xa->SetTitleFont(  fTitleFontA);
         if (fTitleCenterX)
            xa->CenterTitle(kTRUE);
         else
            xa->CenterTitle(kFALSE);
//
			ya->SetNdivisions( fNdivisionsY);
			ya->SetAxisColor(  fAxisColor);
			ya->SetLabelColor( fLabelColor);
			ya->SetLabelFont(  fLabelFont);
			ya->SetLabelOffset(fLabelOffsetY);
			ya->SetLabelSize(  fLabelSize);
			ya->SetTickLength( fTickLength);
			ya->SetTicks( fTickSideY);
			ya->SetTitleOffset(fTitleOffsetY);
			ya->SetTitleSize(  fTitleSize);
			ya->SetTitleColor( fTitleColorA);
			ya->SetTitleFont(  fTitleFontA);
         if (za != NULL) {
            TPaletteAxis *pl = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
            if ( pl != NULL ) {
				   pl->SetLabelColor( fLabelColor);
				   pl->SetLabelFont(  fLabelFont);
				   pl->SetLabelOffset(fLabelOffsetZ);
				   pl->SetLabelOffset(fLabelSize);
            }
				za->SetNdivisions( fNdivisionsZ);
				za->SetAxisColor(  fAxisColor);
				za->SetLabelColor( fLabelColor);
				za->SetLabelFont(  fLabelFont);
				za->SetLabelOffset(fLabelOffsetZ);
				za->SetLabelSize(  fLabelSize);
				za->SetTickLength( fTickLength);
				za->SetTicks( fTickSideZ);
				za->SetTitleOffset(fTitleOffsetZ);
				za->SetTitleSize(  fTitleSize);
				za->SetTitleColor( fTitleColorA);
				za->SetTitleFont(  fTitleFontA);
				if (fTitleCenterZ)
					za->CenterTitle(kTRUE);
				else
					za->CenterTitle(kFALSE);
				}
			if (fLabelMaxDigits > 0)  {
//				cout << "SetMaxDigits: " << fLabelMaxDigits<< endl;
				TGaxis::SetMaxDigits(fLabelMaxDigits);
			}
         if (fTitleCenterY)
            ya->CenterTitle(kTRUE);
         else
            ya->CenterTitle(kFALSE);
			SetStatBoxAttr(gStyle);
			TPaveText *pt = (TPaveText*)fCanvas->GetListOfPrimitives()->FindObject("title");
			if (pt) {
			   SetTitleBoxAttr(gStyle);
				pt->Delete();
         }
         if (mod_statbox) {
		      TObject *sto = hist->GetListOfFunctions()->FindObject("stats");
	   	   if (sto) {
//			      cout << "Delete statbox: " << obj << endl;
			      sto->Delete();
		      }
         }
      }
   }

   canvas->Modified();
	canvas->Update();
}
//______________________________________________________________________

void SetHistOptDialog::SetTitleBoxAttr(TStyle *sty)
{
  TPaveText *pt = (TPaveText*)fCanvas->GetListOfPrimitives()->FindObject("title");
  if (pt) {
	 if (fTitleW < 0.01) fTitleW = 0;
	 if (fTitleH < 0.01) fTitleH = 0;
	 if (fTitleFontSize < 0.001) fTitleFontSize = 0;
	 if (fTitleFontSize > 0.1) fTitleFontSize = 0.04;
	 if (fTitleW > 0.9) fTitleW = 0.5;
	 if (fTitleH > 0.4) fTitleH = 0;
	 if (fTitleFontSize < 0.001) fTitleFontSize = 0;
	 sty->SetTitleFillColor (fTitleColor     );
	 sty->SetTitleTextColor (fTitleTextColor );
	 sty->SetTitleBorderSize(fTitleBorderSize);
	 sty->SetTitleFont      (fTitleFont      );
	 sty->SetTitleFontSize  (fTitleFontSize  );
	 sty->SetTitleStyle     (fTitleStyle     );
	 sty->SetTitleAlign     (fTitleAlign     );
	 sty->SetTitleX         (fTitleX         );
	 sty->SetTitleY         (fTitleY         );
	 sty->SetTitleW         (fTitleW         );
	 sty->SetTitleH         (fTitleH         );
  }
}
//______________________________________________________________________

void SetHistOptDialog::SetStatBoxAttr(TStyle *sty)
{
  TPavesText *pt = (TPavesText*)fHist->GetListOfFunctions()->FindObject("stats");
  if (pt) {
	 sty->SetStatColor     (fStatColor     );
	 sty->SetStatTextColor (fStatTextColor );
	 sty->SetStatBorderSize(fStatBorderSize);
	 sty->SetStatFont      (fStatFont      );
	 sty->SetStatFontSize  (fStatFontSize  );
	 sty->SetStatStyle     (fStatStyle     );
	 sty->SetStatFormat    (fStatFormat    );
	 fStatX         = pt->GetX2NDC();
	 fStatY         = pt->GetY2NDC();
//	 fStatW         = pt->GetX2NDC() - pt->GetX1NDC();
//	 fStatH         = pt->GetY2NDC() - pt->GetY1NDC();
//	 cout << "pt->GetY2NDC() " <<pt->GetY2NDC() << " pt->GetY1NDC() " << pt->GetY1NDC()<< endl;
	 sty->SetStatX         (fStatX         );
	 sty->SetStatY         (fStatY         );
	 sty->SetStatW         (fStatW);
	 sty->SetStatH         (fStatH);
  }
}
//______________________________________________________________________

void SetHistOptDialog::SetCustomStyle()
{
   cout << "SetHistOptDialog:: SetHistAttPerm()" << endl;
   Bool_t ok;
	fNewStyle = 1;
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
	fCustomStyleName = ("hpr_custom_styles.root|TStyle|noselection");

   row_lab->Add(new TObjString("FileContReq_File name"));
   valp[ind++] = &fCustomStyleName;
   row_lab->Add(new TObjString("StringValue_Style name"));
   valp[ind++] = &fNewStyleName;
   row_lab->Add(new TObjString("CheckButton_Create a new Style"));
   valp[ind++] = &fNewStyle;
   Int_t itemwidth = 400;
   ok = GetStringExt("Create / Update Custom Style", NULL, itemwidth, fWindow,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

	TString fname, cname, oname;

	TObjArray * oa = fCustomStyleName.Tokenize("|");
	Int_t nent = oa->GetEntries();
   if (nent < 3) {
      cout << "fCustomStyleName not enough (3) fields" << endl;
      return;
   }

	cname =((TObjString*)oa->At(1))->String();
   cout << "fCustomStyleName (1) " << cname << endl;
	oname =((TObjString*)oa->At(2))->String();
   cout << "fCustomStyleName (2) " << oname << endl;
	if ( oname == "noselection" || fNewStyle) {
	   oname = fNewStyleName;
	}
	TStyle *style = NULL;
	TFile *sf  = NULL;
	fname =((TObjString*)oa->At(0))->String();
	if (gSystem->AccessPathName(fname, kFileExists)) {
	   sf = new TFile(fname, "NEW");
	} else {
	   sf = new TFile(fname, "UPDATE");
		if ( fNewStyle == 0 )
		   style = (TStyle*)sf->Get(oname);
	}
   cout << "fCustomStyleName " << oname << endl;
	if ( !style ) {
	  style = new TStyle(oname, oname);
	}
	SetHistAttPerm(style);
	if ( fHist ) {
		style->SetHistFillColor(fHist->GetFillColor());
		style->SetHistLineColor(fHist->GetLineColor());
		style->SetHistFillStyle(fHist->GetFillStyle());
		style->SetHistLineStyle(fHist->GetLineStyle());
		style->SetHistLineWidth(fHist->GetLineWidth());
		style->SetEndErrorSize (gStyle->GetEndErrorSize());
		style->SetErrorX       (gStyle->GetErrorX());
		TString temp = fHist->GetOption();
		if (temp != "none")
			 style->SetDrawOption(temp);
		else
			 style->SetDrawOption("");
		style->SetMarkerColor  (fHist->GetMarkerColor());
		style->SetMarkerStyle  (fHist->GetMarkerStyle());
		style->SetMarkerSize   (fHist->GetMarkerSize());
		style->SetPadBottomMargin (gStyle->GetPadBottomMargin ());
		style->SetPadTopMargin    (gStyle->GetPadTopMargin    ());
		style->SetPadLeftMargin   (gStyle->GetPadLeftMargin   ());
		style->SetPadRightMargin  (gStyle->GetPadRightMargin  ());
		style->SetPadGridX        (gStyle->GetPadGridX        ());
		style->SetPadGridY        (gStyle->GetPadGridY        ());
		style->SetPadTickX        (gStyle->GetPadTickX        ());
		style->SetPadTickY        (gStyle->GetPadTickY        ());
		style->SetFrameFillColor  (gStyle->GetFrameFillColor  ());
		style->SetFrameLineColor  (gStyle->GetFrameLineColor  ());
		style->SetFrameFillStyle  (gStyle->GetFrameFillStyle  ());
		style->SetFrameLineStyle  (gStyle->GetFrameLineStyle  ());
		style->SetFrameLineWidth  (gStyle->GetFrameLineWidth  ());
		style->SetFrameBorderSize (gStyle->GetFrameBorderSize ());
		style->SetFrameBorderMode (gStyle->GetFrameBorderMode ());
		style->SetCanvasColor     (gStyle->GetCanvasColor     ());
		style->SetCanvasDefH      (gStyle->GetCanvasDefH      ());
		style->SetCanvasDefW      (gStyle->GetCanvasDefW      ());
		style->SetCanvasDefX      (gStyle->GetCanvasDefX      ());
      style->SetPalette         (1, NULL);
    }
	style->Write();
	sf->Close();
	gDirectory = gROOT;
}
//______________________________________________________________________

void SetHistOptDialog::SetHistAttPermLocal()
{
//   cout << "SetHistOptDialog:: SetHistAttPerm()" << endl;
   fNdivisionsX  = fTdivX*10000 + fSdivX*100 + fPdivX;
   if (fOptimizedivX == 0) fNdivisionsX *= -1;
   fNdivisionsY  = fTdivY*10000 + fSdivY*100 + fPdivY;
   if (fOptimizedivY == 0) fNdivisionsY *= -1;
   fNdivisionsZ  = fTdivZ*10000 + fSdivZ*100 + fPdivZ;
   if (fNdivisionsZ == 0) fNdivisionsZ = 510;
   if (fOptimizedivZ == 0) fNdivisionsZ *= -1;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void SetHistOptDialog::SetHistAttPerm(TStyle * style)
{
//   cout << "SetHistOptDialog:: SetHistAttPerm()" << endl;
   TStyle * sty = style;
   if (sty == NULL)
      sty = gStyle;
   sty->SetNdivisions   (fNdivisionsX );
   sty->SetAxisColor    (fAxisColor   );
   sty->SetLabelColor   (fLabelColor  );
   sty->SetLabelFont    (fLabelFont   );
   sty->SetLabelOffset  (fLabelOffsetX);
   sty->SetLabelSize    (fLabelSize   );
   sty->SetTickLength   (fTickLength  );
   sty->SetTitleOffset  (fTitleOffsetX);
   sty->SetTitleSize    (fTitleSize   );
   sty->SetTitleColor   (fTitleColorA  );
   sty->SetTitleFont    (fTitleFont   );

   sty->SetNdivisions   (fNdivisionsY, "Y");
   sty->SetAxisColor    (fAxisColor,   "Y");
   sty->SetLabelColor   (fLabelColor,  "Y");
   sty->SetLabelFont    (fLabelFont,   "Y");
   sty->SetLabelOffset  (fLabelOffsetY,"Y");
   sty->SetLabelSize    (fLabelSize,   "Y");
   sty->SetTickLength   (fTickLength,  "Y");
   sty->SetTitleOffset  (fTitleOffsetY,"Y");
   sty->SetTitleSize    (fTitleSize,   "Y");
   sty->SetTitleColor   (fTitleColorA,  "Y");
   sty->SetTitleFont    (fTitleFont,   "Y");

   sty->SetNdivisions   (fNdivisionsZ, "Z");
   sty->SetAxisColor    (fAxisColor,   "Z");
   sty->SetLabelColor   (fLabelColor,  "Z");
   sty->SetLabelFont    (fLabelFont,   "Z");
   sty->SetLabelOffset  (fLabelOffsetZ,"Z");
   sty->SetLabelSize    (fLabelSize,   "Z");
   sty->SetTickLength   (fTickLength,  "Z");
   sty->SetTitleOffset  (fTitleOffsetZ,"Z");
   sty->SetTitleSize    (fTitleSize,   "Z");
   sty->SetTitleColor   (fTitleColorA,  "Z");
   sty->SetTitleFont    (fTitleFont,   "Z");
   if (fLabelMaxDigits > 0)  {
//      cout << "SetMaxDigits" << fLabelMaxDigits<< endl;
      TGaxis::SetMaxDigits(fLabelMaxDigits);
   }
	SetTitleBoxAttr(sty);
	SetStatBoxAttr(sty);
}
//______________________________________________________________________

void SetHistOptDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("SetHistOptDialog.fNdivisionsX",   fNdivisionsX);
   env.SetValue("SetHistOptDialog.fNdivisionsY",   fNdivisionsY);
   env.SetValue("SetHistOptDialog.fNdivisionsZ",   fNdivisionsZ);
   env.SetValue("SetHistOptDialog.fAxisColor",     fAxisColor);
   env.SetValue("SetHistOptDialog.fLabelColor",    fLabelColor);
   env.SetValue("SetHistOptDialog.fLabelFont",     fLabelFont);
   env.SetValue("SetHistOptDialog.fLabelOffsetX",  fLabelOffsetX);
   env.SetValue("SetHistOptDialog.fLabelOffsetY",  fLabelOffsetY);
   env.SetValue("SetHistOptDialog.fLabelOffsetZ",  fLabelOffsetZ);
   env.SetValue("SetHistOptDialog.fLabelSize",     fLabelSize);
   env.SetValue("SetHistOptDialog.fLabelMaxDigits",fLabelMaxDigits);
   env.SetValue("SetHistOptDialog.fTickLength",    fTickLength);
   env.SetValue("SetHistOptDialog.fTickSideX",      fTickSideX);
   env.SetValue("SetHistOptDialog.fTickSideY",      fTickSideY);
   env.SetValue("SetHistOptDialog.fTickSideZ",      fTickSideZ);
   env.SetValue("SetHistOptDialog.fTitleOffsetX",  fTitleOffsetX);
   env.SetValue("SetHistOptDialog.fTitleOffsetY",  fTitleOffsetY);
   env.SetValue("SetHistOptDialog.fTitleOffsetZ",  fTitleOffsetZ);
   env.SetValue("SetHistOptDialog.fTitleSize",     fTitleSize);
   env.SetValue("SetHistOptDialog.fTitleColorA",    fTitleColorA);
   env.SetValue("SetHistOptDialog.fTitleFontA",     fTitleFontA);
   env.SetValue("SetHistOptDialog.fTitleCenterX",  fTitleCenterX);
   env.SetValue("SetHistOptDialog.fTitleCenterY",  fTitleCenterY);
   env.SetValue("SetHistOptDialog.fTitleCenterZ",  fTitleCenterZ);
   env.SetValue("SetHistOptDialog.TitleColor",      fTitleColor     );
   env.SetValue("SetHistOptDialog.TitleTextColor",  fTitleTextColor );
   env.SetValue("SetHistOptDialog.TitleBorderSize", fTitleBorderSize);
   env.SetValue("SetHistOptDialog.TitleFont",       fTitleFont      );
   env.SetValue("SetHistOptDialog.TitleFontSize",   fTitleFontSize  );
   env.SetValue("SetHistOptDialog.TitleStyle",      fTitleStyle     );
   env.SetValue("SetHistOptDialog.TitleX",          fTitleX         );
   env.SetValue("SetHistOptDialog.TitleY",          fTitleY         );
   env.SetValue("SetHistOptDialog.TitleW",          fTitleW         );
   env.SetValue("SetHistOptDialog.TitleH",          fTitleH         );
   env.SetValue("SetHistOptDialog.TitleAlign",      fTitleAlign     );
   env.SetValue("SetHistOptDialog.StatColor",       fStatColor      );
   env.SetValue("SetHistOptDialog.StatTextColor",   fStatTextColor  );
   env.SetValue("SetHistOptDialog.StatBorderSize",  fStatBorderSize );
   env.SetValue("SetHistOptDialog.StatFont",        fStatFont       );
   env.SetValue("SetHistOptDialog.fStatFormat",     fStatFormat     );
   env.SetValue("SetHistOptDialog.StatFontSize",    fStatFontSize   );
   env.SetValue("SetHistOptDialog.StatStyle",       fStatStyle      );
   env.SetValue("SetHistOptDialog.StatX",           fStatX          );
   env.SetValue("SetHistOptDialog.StatY",           fStatY          );
   env.SetValue("SetHistOptDialog.StatW",           fStatW          );
   env.SetValue("SetHistOptDialog.StatH",           fStatH          );
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void SetHistOptDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fNdivisionsX   = env.GetValue("SetHistOptDialog.fNdivisionsX", 510);
   fNdivisionsY   = env.GetValue("SetHistOptDialog.fNdivisionsY", 510);
   fNdivisionsZ   = env.GetValue("SetHistOptDialog.fNdivisionsZ", 510);
   fAxisColor     = env.GetValue("SetHistOptDialog.fAxisColor", 1);
   fLabelColor    = env.GetValue("SetHistOptDialog.fLabelColor", 1);
   fLabelFont     = env.GetValue("SetHistOptDialog.fLabelFont", 62);
   fLabelOffsetX  = env.GetValue("SetHistOptDialog.fLabelOffsetX", 0.01);
   fLabelOffsetY  = env.GetValue("SetHistOptDialog.fLabelOffsetY", 0.01);
   fLabelOffsetZ  = env.GetValue("SetHistOptDialog.fLabelOffsetZ", 0.01);
   fLabelSize     = env.GetValue("SetHistOptDialog.fLabelSize", 0.02);
   fLabelMaxDigits= env.GetValue("SetHistOptDialog.fLabelMaxDigits", 4);
   fTickLength    = env.GetValue("SetHistOptDialog.fTickLength", 0.01);
   fTickSideX     = env.GetValue("SetHistOptDialog.fTickSideX", "+");
   fTickSideY     = env.GetValue("SetHistOptDialog.fTickSideY", "-");
   fTickSideZ     = env.GetValue("SetHistOptDialog.fTickSideZ", "+");
   fTitleOffsetX  = env.GetValue("SetHistOptDialog.fTitleOffsetX", 1.);
   fTitleOffsetY  = env.GetValue("SetHistOptDialog.fTitleOffsetY", 1.);
   fTitleOffsetZ  = env.GetValue("SetHistOptDialog.fTitleOffsetZ", 1.);
   fTitleSize     = env.GetValue("SetHistOptDialog.fTitleSize", 0.03);
   fTitleColorA   = env.GetValue("SetHistOptDialog.fTitleColorA", 1);
   fTitleFontA    = env.GetValue("SetHistOptDialog.fTitleFontA", 62);
   fTitleCenterX  = env.GetValue("SetHistOptDialog.fTitleCenterX", 0);
   fTitleCenterY  = env.GetValue("SetHistOptDialog.fTitleCenterY", 0);
   fTitleCenterZ  = env.GetValue("SetHistOptDialog.fTitleCenterZ", 0);
	if ( fHist ) {
		TAxis *xa = fHist->GetXaxis();
		TAxis *ya = fHist->GetYaxis();
		TAxis *za = NULL;
		if (fHist && fHist->GetDimension() > 1)
			za = fHist->GetZaxis();
		fAxisColor    = xa->GetAxisColor  ();
		fLabelColor   = xa->GetLabelColor ();
		fLabelFont    = xa->GetLabelFont  ();
		fLabelSize    = xa->GetLabelSize  ();
		fTickLength   = xa->GetTickLength ();
		fTitleSize    = xa->GetTitleSize  ();
		fTitleColorA  = xa->GetTitleColor ();
		fTitleFontA   = xa->GetTitleFont  ();

		fNdivisionsX  = xa->GetNdivisions ();
		fLabelOffsetX = xa->GetLabelOffset();
		fTickSideX    = xa->GetTicks      ();
		fTitleOffsetX = xa->GetTitleOffset();
		fTitleCenterX = xa->GetCenterTitle();

		fNdivisionsY  = ya->GetNdivisions ();
		fLabelOffsetY = ya->GetLabelOffset();
		if ( !ya->TestBit(TAxis::kTickPlus) &&
			!ya->TestBit(TAxis::kTickMinus) )
			ya->SetTicks("-");
		fTickSideY    = ya->GetTicks      ();
		fTitleOffsetY = ya->GetTitleOffset();
		fTitleCenterY = ya->GetCenterTitle();

		if (za != NULL) {
			fNdivisionsZ  = za->GetNdivisions ();
			fLabelOffsetZ = za->GetLabelOffset();
			fTickSideZ    = za->GetTicks      ();
			fTitleOffsetZ = za->GetTitleOffset();
			fTitleCenterZ = za->GetCenterTitle();
	/*
			TPaletteAxis *pl = (TPaletteAxis*)fHist->GetListOfFunctions()->FindObject("palette");
			if ( pl != NULL ) {
				pl->SetLabelColor( fLabelColor);
				pl->SetLabelFont(  fLabelFont);
				pl->SetLabelOffset(fLabelOffsetZ);
				pl->SetLabelOffset(fLabelSize);
			}
	*/
		}
		TPavesText *pt = (TPavesText*)fHist->GetListOfFunctions()->FindObject("stats");
		if (pt) {
			fStatColor     = pt->GetFillColor();
			fStatTextColor = pt->GetTextColor();
			fStatBorderSize= pt->GetBorderSize();
			fStatFont      = pt->GetTextFont();
			fStatFontSize  = pt->GetTextSize();
			fStatStyle     = pt->GetFillStyle();
			fStatFormat    = gStyle->GetStatFormat();
			//         fStatFormat   = pt->Get();
			fStatX         = pt->GetX2NDC();
			fStatY         = pt->GetY2NDC();
			fStatW         = gStyle->GetStatW();
			fStatH         = gStyle->GetStatH();
		}
	}
	fLabelMaxDigits = TGaxis::GetMaxDigits();
   fStatColor=     env.GetValue("SetHistOptDialog.StatColor", 19);
   fStatTextColor= env.GetValue("SetHistOptDialog.StatTextColor", 1);
   fStatBorderSize=env.GetValue("SetHistOptDialog.StatBorderSize", 1);
   fStatFont=      env.GetValue("SetHistOptDialog.StatFont", 62);
   fStatFormat=    env.GetValue("SetHistOptDialog.fStatFormat", "6.2g");
   fStatFontSize=  env.GetValue("SetHistOptDialog.StatFontSize", 0.02);
   fStatStyle=     env.GetValue("SetHistOptDialog.StatStyle", 1001);
   fStatX=         env.GetValue("SetHistOptDialog.StatX", 0.98);
   fStatY=         env.GetValue("SetHistOptDialog.StatY", 0.995);
   fStatW=         env.GetValue("SetHistOptDialog.StatW", 0.2);
   fStatH=         env.GetValue("SetHistOptDialog.StatH", 0.16);
   if (fStatFont < 12 || fStatFont > 123) fStatFont = 62;
   fTitleColor     = env.GetValue("SetHistOptDialog.TitleColor",     0);
   fTitleStyle     = env.GetValue("SetHistOptDialog.TitleStyle",     1001);
   fTitleTextColor = env.GetValue("SetHistOptDialog.TitleTextColor", 1);
   fTitleBorderSize= env.GetValue("SetHistOptDialog.TitleBorderSize",1);
   fTitleFont      = env.GetValue("SetHistOptDialog.TitleFont",      62);
   if (fTitleFont < 12 || fTitleFont > 123) fTitleFont = 62;
   fTitleFont      = env.GetValue("SetHistOptDialog.TitleX",    0.5);
	fTitleX         = env.GetValue("SetHistOptDialog.TitleX",    0.5);
	fTitleY         = env.GetValue("SetHistOptDialog.TitleY",   .995);
	fTitleW         = env.GetValue("SetHistOptDialog.TitleW",     0.);
	fTitleH         = env.GetValue("SetHistOptDialog.TitleH",     0.);
	fTitleAlign     = env.GetValue("SetHistOptDialog.TitleAlign", 23);
	fTitleFontSize  = env.GetValue("SetHistOptDialog.TitleFontSize", 0);
	TPaveText *tit = (TPaveText*)fCanvas->GetListOfPrimitives()->FindObject("title");
	if ( tit ) {
		fTitleColor =       tit->GetFillColor();
		fTitleStyle =       tit->GetFillStyle();
		fTitleTextColor =   tit->GetTextColor();
		fTitleBorderSize =  tit->GetBorderSize();
		fTitleFont =        tit->GetTextFont();
   }
}
//______________________________________________________________________

void SetHistOptDialog::SetDefaults()
{
//   cout << "SetHistOptDialog:: SetHistAttPerm" << endl;
   TEnv env(".hprrc");
   gStyle->SetNdivisions   (env.GetValue("SetHistOptDialog.fNdivisionsX", 510));
   gStyle->SetAxisColor    (env.GetValue("SetHistOptDialog.fAxisColor", 1));
   gStyle->SetLabelColor   (env.GetValue("SetHistOptDialog.fLabelColor", 1));
   gStyle->SetLabelFont    (env.GetValue("SetHistOptDialog.fLabelFont", 62));
   gStyle->SetLabelOffset  (env.GetValue("SetHistOptDialog.fLabelOffsetX", 0.01));
   gStyle->SetLabelSize    (env.GetValue("SetHistOptDialog.fLabelSize", 0.02));
   gStyle->SetTickLength   (env.GetValue("SetHistOptDialog.fTickLength", 0.01));
   gStyle->SetTitleOffset  (env.GetValue("SetHistOptDialog.fTitleOffsetX", 0.01));
   gStyle->SetTitleSize    (env.GetValue("SetHistOptDialog.fTitleSize", 0.03));
   gStyle->SetTitleColor   (env.GetValue("SetHistOptDialog.fTitleColorA", 1));
   gStyle->SetTitleFont    (env.GetValue("SetHistOptDialog.fTitleFontA", 62));

   gStyle->SetNdivisions   (env.GetValue("SetHistOptDialog.fNdivisionsY", 510), "Y");
   gStyle->SetAxisColor    (env.GetValue("SetHistOptDialog.fAxisColor", 1), "Y");
   gStyle->SetLabelColor   (env.GetValue("SetHistOptDialog.fLabelColor", 1), "Y");
   gStyle->SetLabelFont    (env.GetValue("SetHistOptDialog.fLabelFont", 62),   "Y");
   gStyle->SetLabelOffset  (env.GetValue("SetHistOptDialog.fLabelOffsetY", 0.01),"Y");
   gStyle->SetLabelSize    (env.GetValue("SetHistOptDialog.fLabelSize", 0.02),   "Y");
   gStyle->SetTickLength   (env.GetValue("SetHistOptDialog.fTickLength", 0.01),  "Y");
   gStyle->SetTitleOffset  (env.GetValue("SetHistOptDialog.fTitleOffsetY", 0.01),"Y");
   gStyle->SetTitleSize    (env.GetValue("SetHistOptDialog.fTitleSize", 0.03),   "Y");
   gStyle->SetTitleColor   (env.GetValue("SetHistOptDialog.fTitleColorA", 1),  "Y");
   gStyle->SetTitleFont    (env.GetValue("SetHistOptDialog.fTitleFontA", 62),   "Y");

   gStyle->SetNdivisions   (env.GetValue("SetHistOptDialog.fNdivisionsZ", 510), "Z");
   gStyle->SetAxisColor    (env.GetValue("SetHistOptDialog.fAxisColor", 1),   "Z");
   gStyle->SetLabelColor   (env.GetValue("SetHistOptDialog.fLabelColor", 1),  "Z");
   gStyle->SetLabelFont    (env.GetValue("SetHistOptDialog.fLabelFont", 62),   "Z");
   gStyle->SetLabelOffset  (env.GetValue("SetHistOptDialog.fLabelOffsetZ", 0.10),"Z");
   gStyle->SetLabelSize    (env.GetValue("SetHistOptDialog.fLabelSize", 0.02),   "Z");
   gStyle->SetTickLength   (env.GetValue("SetHistOptDialog.fTickLength", 0.01),  "Z");
   gStyle->SetTitleOffset  (env.GetValue("SetHistOptDialog.fTitleOffsetZ", 0.01),"Z");
   gStyle->SetTitleSize    (env.GetValue("SetHistOptDialog.fTitleSize", 0.03),   "Z");
   gStyle->SetTitleColor   (env.GetValue("SetHistOptDialog.fTitleColorA", 1),  "Z");
   gStyle->SetTitleFont    (env.GetValue("SetHistOptDialog.fTitleFontA", 62),   "Z");
   TGaxis::SetMaxDigits    (env.GetValue("SetHistOptDialog.fLabelMaxDigits", 4));

   gStyle->SetTitleColor     (env.GetValue("SetHistOptDialog.TitleColor",     0), "t");
   gStyle->SetTitleTextColor (env.GetValue("SetHistOptDialog.TitleTextColor", 1));
   gStyle->SetTitleBorderSize(env.GetValue("SetHistOptDialog.TitleBorderSize",1));
   gStyle->SetTitleFont      (env.GetValue("SetHistOptDialog.TitleFont",      62), "t");
   gStyle->SetTitleFontSize  (env.GetValue("SetHistOptDialog.TitleFontSize",  0.02));
   gStyle->SetTitleStyle     (env.GetValue("SetHistOptDialog.TitleStyle",     1001));
   gStyle->SetTitleX         (env.GetValue("SetHistOptDialog.TitleX",         0.5));
   gStyle->SetTitleY         (env.GetValue("SetHistOptDialog.TitleY",         .995));
   gStyle->SetTitleW         (env.GetValue("SetHistOptDialog.TitleW",         0.));
   gStyle->SetTitleH         (env.GetValue("SetHistOptDialog.TitleH",         0.));
   gStyle->SetTitleAlign     (env.GetValue("SetHistOptDialog.TitleAlign",    13));

   gStyle->SetStatColor     (env.GetValue("SetHistOptDialog.StatColor", 19));
   gStyle->SetStatTextColor (env.GetValue("SetHistOptDialog.StatTextColor", 1));
   gStyle->SetStatBorderSize(env.GetValue("SetHistOptDialog.StatBorderSize", 1));
   gStyle->SetStatFont      (env.GetValue("SetHistOptDialog.StatFont", 62));
   gStyle->SetStatFontSize  (env.GetValue("SetHistOptDialog.StatFontSize", 0.02));
   gStyle->SetStatStyle     (env.GetValue("SetHistOptDialog.StatStyle", 1001));
   gStyle->SetStatX         (env.GetValue("SetHistOptDialog.StatX", 0.98));
   gStyle->SetStatY         (env.GetValue("SetHistOptDialog.StatY", 0.995));
   gStyle->SetStatW         (env.GetValue("SetHistOptDialog.StatW", 0.2));
   gStyle->SetStatH         (env.GetValue("SetHistOptDialog.StatH", 0.16));
}
//______________________________________________________________________

void SetHistOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void SetHistOptDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetHistAttNow(canvas, bid);
}
