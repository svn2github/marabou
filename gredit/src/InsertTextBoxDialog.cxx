#include "TROOT.h"
#include "TBox.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TMarker.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "THprTextBox.h"
#include "InsertTextBoxDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertTextBoxDialog)

InsertTextBoxDialog::InsertTextBoxDialog()
{
static const Char_t helptext[] =
"no help yet\n\
";
   gROOT->GetListOfCleanups()->Add(this);
   fCanvas = gPad->GetCanvas();
   TRootCanvas* win = NULL;
   if (fCanvas)
      win = (TRootCanvas*)fCanvas->GetCanvasImp();
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
    static TString execute_cmd("ExecuteInsert()");
   fX1 = fY1 = 0;
   fRow_lab->Add(new TObjString("DoubleValue_X0"));
   fValp[ind++] = &fX1;
   fRow_lab->Add(new TObjString("DoubleValue+Y0"));
   fValp[ind++] = &fY1;
   fRow_lab->Add(new TObjString("DoubleValue_X Width"));
   fValp[ind++] = &fDx;
   fRow_lab->Add(new TObjString("DoubleValue+Y Width"));
   fValp[ind++] = &fDy;
   fRow_lab->Add(new TObjString("CheckButton_Show shadow"));
   fValp[ind++] = &fShowShadow;
   fRow_lab->Add(new TObjString("PlainIntVal+Shwidth"));
   fValp[ind++] = &fBorderSize;
   fRow_lab->Add(new TObjString("AlignSelect+ShPos"));
   fValp[ind++] = &fShadowPosition;
   fRow_lab->Add(new TObjString("CheckButton_Round Corners"));
   fValp[ind++] = &fRoundCorners;
   fRow_lab->Add(new TObjString("DoubleValue+Corner Radius"));
   fValp[ind++] = &fCornerRadius;
   fRow_lab->Add(new TObjString("ColorSelect_LineColor"));
   fValp[ind++] = &fColor;
   fRow_lab->Add(new TObjString("LineSSelect+Style"));
   fValp[ind++] = &fStyle;
   fRow_lab->Add(new TObjString("PlainShtVal+Width"));
   fValp[ind++] = &fWidth;
   fRow_lab->Add(new TObjString("ColorSelect_FillColor"));
   fValp[ind++] = &fFillColor;
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fValp[ind++] = &fFillStyle;
   fRow_lab->Add(new TObjString("RadioButton_No align"));
   fValp[ind++] = &fNoAlign;
   fRow_lab->Add(new TObjString("RadioButton+Weak align"));
   fValp[ind++] = &fWeakAlign;
   fRow_lab->Add(new TObjString("RadioButton+Strong align"));
   fValp[ind++] = &fStrongAlign;
   fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
   fValp[ind++] = &execute_cmd;
   Int_t itemwidth = 280;
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Text Box", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//____________________________________________________________________________

void InsertTextBoxDialog::ExecuteInsert()
{
   TPave *m1 = NULL;
   TMarker *ma = NULL;
   Double_t X2=0, Y2=0;
   if ((fX1 == 0 && fY1 == 0) || (fDx == 0 || fDy == 0)) {
		if (fDx == 0 || fDy == 0) {
			cout << "Define a box " << endl;
			m1  = (TPave*)gPad->WaitPrimitive("TPave");
			if (m1 == NULL) {
				cout << "Interrupted Input" << endl;
				return;
			}
			m1 = (TPave *)gPad->GetListOfPrimitives()->Last();
			if (!m1) return;
			fX1 = m1->GetX1();
			fY1 = m1->GetY1();
			X2 = m1->GetX2();
			Y2 = m1->GetY2();
		} else {
			cout << "Define lower left corner" << endl;
			ma  = (TMarker*)gPad->WaitPrimitive("TMarker");
			if (ma == NULL) {
				cout << "Interrupted Input" << endl;
				return;
			}
			ma = (TMarker *)gPad->GetListOfPrimitives()->Last();
			if (!ma) return;
			fX1 = ma->GetX();
			fY1 = ma->GetY();
         X2 = fX1 + fDx;
         Y2 = fY1 + fDy;
		}
   } else {
      X2 = fX1 + fDx;
      Y2 = fY1 + fDy;
   }
   if (m1) delete m1;
   if (ma) delete ma;
   THprTextBox *tb;
   tb = new THprTextBox(fX1, fY1, X2, Y2);
   Int_t halign = fShadowPosition / 10;
   Int_t valign = fShadowPosition % 10;
   TString opt;
   if (fRoundCorners != 0) {
      fCornerRadius = TMath::Min(fCornerRadius, 0.49999);
      tb->SetCornerRadius(fCornerRadius);
      opt += "arc";
   }
   if (valign == 3) opt += "T";
   else             opt += "B";
   if (halign == 1) opt += "L";
   else             opt += "R";
   tb->Draw(opt);
   tb->SetLineColor(fColor);
   tb->SetLineStyle(fStyle);
   tb->SetLineWidth(fWidth);
   tb->SetFillColor(fFillColor);
   tb->SetFillStyle(fFillStyle);
   if (fShowShadow != 0) tb->SetBorderSize(fBorderSize);
   else                  tb->SetBorderSize(1);
   if      (fNoAlign)   tb->SetAlignType(0);
   else if (fWeakAlign) tb->SetAlignType(1);
   else                 tb->SetAlignType(2);
   cout << opt << endl;
   fX1 = fY1 = 0;
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
};
//____________________________________________________________________________

void InsertTextBoxDialog::SaveDefaults()
{
//   cout << "InsertTextBoxDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertTextBoxDialog.fDx"            ,fDx);
   env.SetValue("InsertTextBoxDialog.fDy"            ,fDy);
   env.SetValue("InsertTextBoxDialog.fColor"         ,fColor);
   env.SetValue("InsertTextBoxDialog.fStyle"         ,fStyle);
   env.SetValue("InsertTextBoxDialog.fWidth"         ,fWidth);
   env.SetValue("InsertTextBoxDialog.fFillColor"     ,fFillColor);
   env.SetValue("InsertTextBoxDialog.fFillStyle"     ,fFillStyle);
   env.SetValue("InsertTextBoxDialog.fCornerRadius"  ,fCornerRadius);
   env.SetValue("InsertTextBoxDialog.fRoundCorners"  ,fRoundCorners);
   env.SetValue("InsertTextBoxDialog.fShowShadow"    ,fShowShadow);
   env.SetValue("InsertTextBoxDialog.fBorderSize"    ,fBorderSize);
   env.SetValue("InsertTextBoxDialog.fShadowPosition",fShadowPosition);
   env.SetValue("InsertTextBoxDialog.fNoAlign"       ,fNoAlign);
   env.SetValue("InsertTextBoxDialog.fWeakAlign"     ,fWeakAlign);
   env.SetValue("InsertTextBoxDialog.fStrongAlign"   ,fStrongAlign);
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________

void InsertTextBoxDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fDx            = env.GetValue("InsertTextBoxDialog.fDx" ,0.);
   fDy            = env.GetValue("InsertTextBoxDialog.fDy" ,0.);
   fColor         = env.GetValue("InsertTextBoxDialog.fColor" ,1);
   fStyle         = env.GetValue("InsertTextBoxDialog.fStyle" ,1);
   fWidth         = env.GetValue("InsertTextBoxDialog.fWidth" ,2);
   fFillColor     = env.GetValue("InsertTextBoxDialog.fFillColor" ,1);
   fFillStyle     = env.GetValue("InsertTextBoxDialog.fFillStyle" ,1);
   fCornerRadius  = env.GetValue("InsertTextBoxDialog.fCornerRadius" ,0.02);
   fRoundCorners  = env.GetValue("InsertTextBoxDialog.fRoundCorners" ,0);
   fShowShadow    = env.GetValue("InsertTextBoxDialog.fShowShadow" ,1);
   fBorderSize    = env.GetValue("InsertTextBoxDialog.fBorderSize" ,4);
   fShadowPosition= env.GetValue("InsertTextBoxDialog.fShadowPosition" ,31);
   fNoAlign       = env.GetValue("InsertTextBoxDialog.fNoAlign"    ,0);
   fWeakAlign     = env.GetValue("InsertTextBoxDialog.fWeakAlign"  ,1);
   fStrongAlign   = env.GetValue("InsertTextBoxDialog.fStrongAlign",0);
}
//_________________________________________________________________________

InsertTextBoxDialog::~InsertTextBoxDialog()
{
   gROOT->GetListOfCleanups()->Remove(this);
   fRow_lab->Delete();
   delete fRow_lab;
};
//_______________________________________________________________________

void InsertTextBoxDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "InsertTextBoxDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void InsertTextBoxDialog::CloseDialog()
{
//   cout << "InsertTextBoxDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertTextBoxDialog::CloseDown(Int_t wid)
{
//   cout << "InsertTextBoxDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}


