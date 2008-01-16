
#include "TROOT.h"
#include "TBox.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TMarker.h"
#include "TPad.h"
#include "TPave.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "HTPad.h"
#include "InsertPadDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertPadDialog)

InsertPadDialog::InsertPadDialog()
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
   fRow_lab->Add(new TObjString("PlainShtVal_BorderSize"));
   fValp[ind++] = &fBorderSize;
   fRow_lab->Add(new TObjString("PlainShtVal+BorderMode"));
   fValp[ind++] = &fBorderMode;
   fRow_lab->Add(new TObjString("ColorSelect_FillColor"));
   fValp[ind++] = &fFillColor;
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fValp[ind++] = &fFillStyle;
   fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
   fValp[ind++] = &execute_cmd;
   Int_t itemwidth = 280;
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Graphics Pad", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_________________________________________________________________________

InsertPadDialog::~InsertPadDialog()
{
   SaveDefaults();
};
//_______________________________________________________________________

void InsertPadDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "FeynmanDiagramDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void InsertPadDialog::CloseDialog()
{
//   cout << "FeynmanDiagramDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//____________________________________________________________________________

void InsertPadDialog::ExecuteInsert()
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

   Double_t px1 = gPad->GetX1();
   Double_t py1 = gPad->GetY1();
   Double_t px2 = gPad->GetX2();
   Double_t py2 = gPad->GetY2();

   Int_t  n = 0;
   TObject *obj;
   TIter next(gPad->GetListOfPrimitives());

   while ((obj = next())) {
      if (obj->InheritsFrom(TPad::Class())) {
         n++;
      }
   }
	Double_t xlow = (fX1 - px1)/(px2 - px1);
	Double_t ylow = (fY1 - py1)/(py2 - py1);
	Double_t xup  = (X2 - px1) /(px2 - px1);
	Double_t yup  = (Y2 - py1) /(py2 - py1);

   HTPad *tb;
   tb = new HTPad(Form("%s_%d",gPad->GetName(),n+1), "HprPad",
                    xlow, ylow, xup, yup);
   cout << xlow << " " <<ylow << " " << xup << " " << yup<< endl;

   tb->Draw();
   tb->SetFillColor(fFillColor);
   tb->SetFillStyle(fFillStyle);
   tb->SetBorderSize(fBorderSize);
   tb->SetBorderSize(fBorderMode);
   fX1 = fY1 = 0;
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
};
//____________________________________________________________________________

void InsertPadDialog::SaveDefaults()
{
//   cout << "InsertPadDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
   env.SetValue("InsertPadDialog.fDx"            ,fDx);
   env.SetValue("InsertPadDialog.fDy"            ,fDy);
   env.SetValue("InsertPadDialog.fFillColor"     ,fFillColor);
   env.SetValue("InsertPadDialog.fFillStyle"     ,fFillStyle);
   env.SetValue("InsertPadDialog.fBorderSize"    ,fBorderSize);
   env.SetValue("InsertPadDialog.fBorderMode"    ,fBorderMode);
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________

void InsertPadDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fDx            = env.GetValue("InsertPadDialog.fDx" ,0.);
   fDy            = env.GetValue("InsertPadDialog.fDy" ,0.);
   fFillColor     = env.GetValue("InsertPadDialog.fFillColor" ,1);
   fFillStyle     = env.GetValue("InsertPadDialog.fFillStyle" ,0);
   fBorderSize    = env.GetValue("InsertPadDialog.fBorderSize" ,-1);
   fBorderMode    = env.GetValue("InsertPadDialog.fBorderMode" ,-2);
}
//_________________________________________________________________________

void InsertPadDialog::CloseDown(Int_t wid)
{
   cout << "InsertPadDialog::CloseDown()" << endl;
   delete this;
}
