#include "TBox.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TMarker.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "TextBox.h"
#include "InsertTextBoxDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertTextBoxDialog)

InsertTextBoxDialog::InsertTextBoxDialog()
{
   static void *valp[25];
   Int_t ind = 0;
   RestoreDefaults();
   TList * labels = new TList;
   static TString execute_cmd("ExecuteInsert()");
   fX1 = fY1 = 0;
   labels->Add(new TObjString("DoubleValue_X0"));
   valp[ind++] = &fX1;
   labels->Add(new TObjString("DoubleValue+Y0"));
   valp[ind++] = &fY1;
   labels->Add(new TObjString("DoubleValue_X Width"));
   valp[ind++] = &fDx;
   labels->Add(new TObjString("DoubleValue+Y Width"));
   valp[ind++] = &fDy;
   labels->Add(new TObjString("CheckButton_Show shadow"));
   valp[ind++] = &fShowShadow;
   labels->Add(new TObjString("PlainIntVal+Shwidth"));
   valp[ind++] = &fBorderSize;
   labels->Add(new TObjString("AlignSelect+ShPos"));
   valp[ind++] = &fShadowPosition;
   labels->Add(new TObjString("CheckButton_Round Corners"));
   valp[ind++] = &fRoundCorners;
   labels->Add(new TObjString("DoubleValue+Corner Radius"));
   valp[ind++] = &fCornerRadius;
   labels->Add(new TObjString("ColorSelect_LineColor"));
   valp[ind++] = &fColor;
   labels->Add(new TObjString("LineSSelect+Style"));
   valp[ind++] = &fStyle;
   labels->Add(new TObjString("PlainShtVal+Width"));
   valp[ind++] = &fWidth;
   labels->Add(new TObjString("ColorSelect_FillColor"));
   valp[ind++] = &fFillColor;
   labels->Add(new TObjString("Fill_Select+FillStyle"));
   valp[ind++] = &fFillStyle;
   labels->Add(new TObjString("RadioButton_No align")); 	
   valp[ind++] = &fNoAlign;
   labels->Add(new TObjString("RadioButton+Weak align")); 	
   valp[ind++] = &fWeakAlign;
   labels->Add(new TObjString("RadioButton+Strong align")); 	
   valp[ind++] = &fStrongAlign;
   labels->Add(new TObjString("CommandButt_Execute Insert()")); 	
   valp[ind++] = &execute_cmd;
   Bool_t ok;
   Int_t itemwidth = 280;

   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   ok = GetStringExt("TextBox", NULL, itemwidth, rc,
                      NULL, NULL, labels,valp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//_________________________________________________________________________
            
InsertTextBoxDialog::~InsertTextBoxDialog() 
{
   SaveDefaults();
};
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
			m1 = (TPave *)gPad->GetListOfPrimitives()->Last();
			if (!m1) return;
			fX1 = m1->GetX1();
			fY1 = m1->GetY1();
			X2 = m1->GetX2();
			Y2 = m1->GetY2();
		} else {
			cout << "Define lower left corner" << endl;
			ma  = (TMarker*)gPad->WaitPrimitive("TMarker");
			ma = (TMarker *)gPad->GetListOfPrimitives()->Last();
			if (!ma) return;
			fX1 = ma->GetX();
			fY1 = ma->GetY();
         X2 = fX1 + fDx;
         Y2 = fY1 + fDy;
		}
   }
   if (m1) delete m1;
   if (ma) delete ma;
   TextBox *tb;
   tb = new TextBox(fX1, fY1, X2, Y2);
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
   TEnv env(".rootrc");
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
   TEnv env(".rootrc");
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
            
void InsertTextBoxDialog::CloseDown()
{
   cout << "InsertTextBoxDialog::CloseDown()" << endl;
   delete this;
}
