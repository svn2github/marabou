#include "TROOT.h"
#include "THprArrow.h"
#include "THprCurlyArc.h"
#include "THprCurlyLine.h"
#include "THprLine.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "FeynmanDiagramDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(FeynmanDiagramDialog)

FeynmanDiagramDialog::FeynmanDiagramDialog()
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
   static TString far("FeynmanArrow()");
   static TString fwl("FeynmanWavyLine()");
   static TString fcl("FeynmanCurlyLine()");
   static TString fwa("FeynmanWavyArc()");
   static TString fca("FeynmanCurlyArc()");
   static TString fsl("FeynmanSolidLine()");
   static TString fdl("FeynmanDashedLine()");

   fRow_lab->Add(new TObjString("DoubleValue_WaveL"));
   fValp[ind++] = &fWaveLength;
   fRow_lab->Add(new TObjString("DoubleValue-Amp"));
   fValp[ind++] = &fAmplitude;
   fRow_lab->Add(new TObjString("ArrowSelect_AShape"));
   fValp[ind++] = &fArrowStyle;
   fRow_lab->Add(new TObjString("Float_Value-ASize"));
   fValp[ind++] = &fArrowSize;
   fRow_lab->Add(new TObjString("ColorSelect_Color"));
   fValp[ind++] = &fColor;
   fRow_lab->Add(new TObjString("PlainShtVal+Width"));
   fValp[ind++] = &fWidth;
   fRow_lab->Add(new TObjString("CommandButt_Arrow()"));
   fValp[ind++] = &far;
   fRow_lab->Add(new TObjString("CommandButt+Line()"));
   fValp[ind++] = &fsl;
   fRow_lab->Add(new TObjString("CommandButt+DashedL()"));
   fValp[ind++] = &fdl;
   fRow_lab->Add(new TObjString("CommandButt_WavyLine()"));
   fValp[ind++] = &fwl;
   fRow_lab->Add(new TObjString("CommandButt+CurlyLine()"));
   fValp[ind++] = &fcl;
   fRow_lab->Add(new TObjString("CommandButt_WavyArc()"));
   fValp[ind++] = &fwa;
   fRow_lab->Add(new TObjString("CommandButt+CurlyArc()"));
   fValp[ind++] = &fca;
   fRow_lab->Add(new TObjString("PlainIntVal_Phi start"));
   fValp[ind++] = &fPhi1;
   fRow_lab->Add(new TObjString("PlainIntVal+Phi end"));
   fValp[ind++] = &fPhi2;
   Int_t itemwidth = 220;
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Feynman diagram", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//______________________________________________________________________________

void FeynmanDiagramDialog::FeynmanArrow()
{
   const char * ArrowOption[] =
      {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
   TArrow * a = (TArrow*)gPad->WaitPrimitive("TArrow");
   if (a == NULL) {
      cout << "Interrupted TArrow" << endl;
      return;
   }
   a = (TArrow *)gPad->GetListOfPrimitives()->Last();
   THprArrow * aa = new THprArrow(a->GetX1(), a->GetY1(),
                                  a->GetX2(), a->GetY2());
   delete a;
   aa->Draw();
   aa->SetFillColor(fColor);
   aa->SetLineColor(fColor);
   aa->SetLineWidth(fWidth);
   aa->SetArrowSize(fArrowSize);
   aa->SetDrawOption(ArrowOption[fArrowStyle]);
   aa->SetOption(ArrowOption[fArrowStyle]);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyLine()
{
   FeynmanCurlyWavyLine(0);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyLine()
{
   FeynmanCurlyWavyLine(1);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyWavyLine(Int_t curly)
{
   TCurlyLine * a = (TCurlyLine*)gPad->WaitPrimitive("TCurlyLine");
   if (a == NULL) {
      cout << "Interrupted TCurlyLine" << endl;
      return;
   }
   a = (TCurlyLine *)gPad->GetListOfPrimitives()->Last();
   THprCurlyLine * ha =  new THprCurlyLine(a->GetStartX(), a->GetStartY(),
                         a->GetEndX(), a->GetEndY());
   delete a;
   ha->Draw();
   if (curly)
     ha->SetCurly();
   else 
     ha->SetWavy();
   ha->SetAmplitude(fAmplitude);
   ha->SetWaveLength(fWaveLength);
   ha->SetLineColor(fColor);
   ha->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyArc()
{
   FeynmanCurlyWavyArc(0);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyArc()
{
   FeynmanCurlyWavyArc(1);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyWavyArc(Int_t curly)
{
//	TCurlyArc::SetDefaultIsCurly(kTRUE);
   TCurlyArc * a = (TCurlyArc*)gPad->WaitPrimitive("TCurlyArc");
   a = (TCurlyArc *)gPad->GetListOfPrimitives()->Last();
   if (a == NULL) {
      cout << "Interrupted Input" << endl;
      return;
   }
   THprCurlyArc * ha = new THprCurlyArc(a->GetStartX(), a->GetStartY(),
                         a->GetRadius(), a->GetPhimin(), a->GetPhimax());
   delete a;
   ha->Draw();

   if (curly)
     ha->SetCurly();
   else 
     ha->SetWavy();
//   cout << "TCurlyArc" <<endl;
   ha->SetCurly();
   ha->SetAmplitude(fAmplitude);
   ha->SetWaveLength(fWaveLength);
   ha->SetPhimin(fPhi1);
   ha->SetPhimax(fPhi2);
   ha->SetLineColor(fColor);
   ha->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanSolidLine()
{
  FeynmanLine(kSolid);
}
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanDashedLine()
{
  FeynmanLine(7);
}
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanLine(Style_t lstyle )
{
//   Style_t save = gStyle->GetLineStyle();
//   gStyle->SetLineStyle(kDashed);
   TLine * a = (TLine*)gPad->WaitPrimitive("TLine");
   if (a == NULL) {
      cout << "Interrupted TLine" << endl;
      return;
   }
   a = (TLine *)gPad->GetListOfPrimitives()->Last();
   THprLine * la = new THprLine(a->GetX1(), a->GetY1(), a->GetX2(), a->GetY2());
   delete a;
   la->Draw();
   if (lstyle > 1 && fWidth > 3)lstyle = 9;
   la->SetLineStyle(lstyle);
   la->SetLineColor(fColor);
   la->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void FeynmanDiagramDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("FeynmanDiagramDialog.fWaveLength" , fWaveLength);
   env.SetValue("FeynmanDiagramDialog.fAmplitude"  , fAmplitude );
   env.SetValue("FeynmanDiagramDialog.fColor"		, fColor 	 );
   env.SetValue("FeynmanDiagramDialog.fWidth"		, fWidth 	 );
   env.SetValue("FeynmanDiagramDialog.fPhi1" 		, fPhi1  	 );
   env.SetValue("FeynmanDiagramDialog.fPhi2" 		, fPhi2  	 );
   env.SetValue("FeynmanDiagramDialog.fArrowSize"  ,fArrowSize  );
   env.SetValue("FeynmanDiagramDialog.fArrowStyle" ,fArrowStyle  );
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________

void FeynmanDiagramDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fWaveLength = env.GetValue("FeynmanDiagramDialog.fWaveLength" , 0.02);
   fAmplitude  = env.GetValue("FeynmanDiagramDialog.fAmplitude"  , 0.01);
   fColor   	= env.GetValue("FeynmanDiagramDialog.fColor"      , 1);
   fWidth   	= env.GetValue("FeynmanDiagramDialog.fWidth"  	  , 2);
   fPhi1	 	   = env.GetValue("FeynmanDiagramDialog.fPhi1"  	  , 0);
   fPhi2	 	   = env.GetValue("FeynmanDiagramDialog.fPhi2"  	  , 180);
   fArrowSize  = env.GetValue("FeynmanDiagramDialog.fArrowSize"  , 0.03);
   fArrowStyle  = env.GetValue("FeynmanDiagramDialog.fArrowStyle"  , 7);
}
//______________________________________________________________________

void FeynmanDiagramDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << " FeynmanDiagramDialog::CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
}

//_________________________________________________________________________

FeynmanDiagramDialog::~FeynmanDiagramDialog()
{
   gROOT->GetListOfCleanups()->Remove(this);
   fRow_lab->Delete();
   delete fRow_lab;
};
//_______________________________________________________________________

void FeynmanDiagramDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "FeynmanDiagramDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void FeynmanDiagramDialog::CloseDialog()
{
//   cout << "FeynmanDiagramDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void FeynmanDiagramDialog::CloseDown(Int_t wid)
{
//   cout << "FeynmanDiagramDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}

