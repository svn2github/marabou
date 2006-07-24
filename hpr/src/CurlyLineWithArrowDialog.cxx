#include "TCurlyLineWithArrow.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TObjString.h"
#include "TGMrbValuesAndText.h"
#include "CurlyLineWithArrowDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(CurlyLineWithArrowDialog)

CurlyLineWithArrowDialog::CurlyLineWithArrowDialog()
{
   cout << "InsertCurlyLineWithArrow()" << endl;
   RestoreDefaults();
   static void *valp[25];
   Int_t ind = 0;
   static TString exc("ExecuteInsert()");
   TList * row_lab = new TList(); 
   row_lab->Add(new TObjString("DoubleValue_Ampl"));
   valp[ind++] = &fWaveLength;
   row_lab->Add(new TObjString("DoubleValue+WaveL"));
   valp[ind++] = &fAmplitude;
   row_lab->Add(new TObjString("ColorSelect_LColor"));
   valp[ind++] = &fColor;
   row_lab->Add(new TObjString("PlainShtVal+LWidth"));
   valp[ind++] = &fWidth;
   row_lab->Add(new TObjString("LineSSelect+LStyle"));
   valp[ind++] = &fStyle;
   row_lab->Add(new TObjString("CheckButton_Arrow@Start"));
   valp[ind++] = &fArrowAtStart;
   row_lab->Add(new TObjString("CheckButton+Arrow@End"));
   valp[ind++] = &fArrowAtEnd;
   row_lab->Add(new TObjString("Float_Value_ArLength"));
   valp[ind++] = &fArrowSize;
//   row_lab->Add(new TObjString("DoubleValue+ArAngle"));
//   valp[ind++] = &fArrowAngle;
   row_lab->Add(new TObjString("CommandButt_Execute_Insert"));  
   valp[ind++] = &exc;

   Bool_t ok; 
   Int_t itemwidth = 320;
   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   ok = GetStringExt("Curly Arrow Params", NULL, itemwidth, rc,
                      NULL, NULL, row_lab, valp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//_________________________________________________________________________
            
CurlyLineWithArrowDialog::~CurlyLineWithArrowDialog() 
{
   SaveDefaults();
};
//______________________________________________________________________________

void CurlyLineWithArrowDialog::ExecuteInsert()
{   	
//   const char * ArrowOption[] = 
//     {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
   TLine * gr = (TLine*)gPad->WaitPrimitive("TLine");
   gr = (TLine *)gPad->GetListOfPrimitives()->Last();
   Int_t where = 0;
   if (fArrowAtStart) where += 1;
   if (fArrowAtEnd)   where += 2;
   TCurlyLineWithArrow* cwa = 
     new TCurlyLineWithArrow(gr->GetX1(), gr->GetY1(), gr->GetX2(), gr->GetY2(), 
                             fWaveLength, fAmplitude, where, fArrowSize);
   cwa->Draw();
   cwa->SetLineColor(fColor);
   cwa->SetLineWidth(fWidth);
   cwa->SetLineStyle(fStyle);
   delete gr;
//   cwa->SetDrawOption(ArrowOption[fArrowStyle]);
//   cwa->SetOption(ArrowOption[fArrowStyle]);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void CurlyLineWithArrowDialog::SaveDefaults()
{
   cout << "CurlyLineWithArrowDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
   env.SetValue("CurlyLineWithArrowDialog.fWaveLength" , fWaveLength );
   env.SetValue("CurlyLineWithArrowDialog.fAmplitude"  , fAmplitude  );
   env.SetValue("CurlyLineWithArrowDialog.fColor"		 , fColor 	   ); 
   env.SetValue("CurlyLineWithArrowDialog.fWidth"		 , fWidth 	   );
   env.SetValue("CurlyLineWithArrowDialog.fStyle"      , fStyle      ); 
   env.SetValue("CurlyLineWithArrowDialog.fArrowAngle" , fArrowAngle ); 
   env.SetValue("CurlyLineWithArrowDialog.fArrowSize"  , fArrowSize  ); 
   env.SetValue("CurlyLineWithArrowDialog.fArrowStyle" , fArrowStyle ); 
   env.SetValue("CurlyLineWithArrowDialog.fArrowAtStart", fArrowAtStart ); 
   env.SetValue("CurlyLineWithArrowDialog.fArrowAtEnd"  , fArrowAtEnd ); 
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void CurlyLineWithArrowDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fWaveLength = env.GetValue("CurlyLineWithArrowDialog.fWaveLength" , 0.02);
   fAmplitude  = env.GetValue("CurlyLineWithArrowDialog.fAmplitude"  , 0.01);
   fColor   	= env.GetValue("CurlyLineWithArrowDialog.fColor"      , 1); 
   fStyle      = env.GetValue("CurlyLineWithArrowDialog.fStyle"      , 1); 
   fWidth   	= env.GetValue("CurlyLineWithArrowDialog.fWidth"  	   , 2);
   fArrowAngle = env.GetValue("CurlyLineWithArrowDialog.fArrowAngle" , 30.); 
   fArrowSize  = env.GetValue("CurlyLineWithArrowDialog.fArrowSize"  , 0.03); 
   fArrowStyle = env.GetValue("CurlyLineWithArrowDialog.fArrowStyle" , 3); 
   fArrowAtStart = env.GetValue("CurlyLineWithArrowDialog.fArrowAtStart" , 0); 
   fArrowAtEnd   = env.GetValue("CurlyLineWithArrowDialog.fArrowAtEnd" , 1); 
} 
//_________________________________________________________________________
            
void CurlyLineWithArrowDialog::CloseDown()
{
   cout << "CurlyLineWithArrowDialog::CloseDown()" << endl;
   delete this;
}

