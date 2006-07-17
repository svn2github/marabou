#include "TArrow.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "FeynmanDiagramDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(FeynmanDiagramDialog)

FeynmanDiagramDialog::FeynmanDiagramDialog()
{
 //  Int_t win_width = 160
   static void *valp[25];
   Int_t ind = 0;
   RestoreDefaults();
   TList * labels = new TList;
   static TString far("FeynmanArrow()");
   static TString fwl("FeynmanWavyLine()");
   static TString fcl("FeynmanCurlyLine()");
   static TString fwa("FeynmanWavyArc()");
   static TString fca("FeynmanCurlyArc()");
   static TString fsl("FeynmanSolidLine()");
   static TString fdl("FeynmanDashedLine()");

   labels->Add(new TObjString("DoubleValue_Wavelength"));
   valp[ind++] = &fWaveLength;
   labels->Add(new TObjString("DoubleValue+Aplitude"));
   valp[ind++] = &fAmplitude;
   labels->Add(new TObjString("ColorSelect_Color"));
   valp[ind++] = &fColor;
   labels->Add(new TObjString("PlainShtVal+Width"));
   valp[ind++] = &fWidth;
   labels->Add(new TObjString("CommandButt_FeynmanArrow()")); 	
   valp[ind++] = &far;
   labels->Add(new TObjString("CommandButt_FeynmanWavyLine()")); 
   valp[ind++] = &fwl;
   labels->Add(new TObjString("CommandButt_FeynmanCurlyLine()"));
   valp[ind++] = &fcl;
   labels->Add(new TObjString("PlainIntVal_Phi start"));
   valp[ind++] = &fPhi1;
   labels->Add(new TObjString("PlainIntVal+Phi end"));
   valp[ind++] = &fPhi2;
   labels->Add(new TObjString("CommandButt_FeynmanWavyArc()"));   
   valp[ind++] = &fwa;
   labels->Add(new TObjString("CommandButt_FeynmanCurlyArc()"));  
   valp[ind++] = &fca;
   labels->Add(new TObjString("CommandButt_FeynmanSolidLine()")); 
   valp[ind++] = &fsl;
   labels->Add(new TObjString("CommandButt_FeynmanDashedLine()"));
   valp[ind++] = &fdl;
   Bool_t ok;
   Int_t itemwidth = 320;

   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   ok = GetStringExt("Feynman diagram", NULL, itemwidth, rc,
                      NULL, NULL, labels,valp,
                      NULL, NULL, NULL, this, this->ClassName());
}
//_________________________________________________________________________
            
FeynmanDiagramDialog::~FeynmanDiagramDialog() 
{
   SaveDefaults();
};
//______________________________________________________________________________

void FeynmanDiagramDialog::FeynmanArrow()
{   	
   TArrow * a = (TArrow*)gPad->WaitPrimitive("TArrow");
   a = (TArrow *)gPad->GetListOfPrimitives()->Last();
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyLine()
{
//	TCurlyLine::SetDefaultIsCurly(kFALSE);
   TCurlyLine * a = (TCurlyLine*)gPad->WaitPrimitive("TCurlyLine");
   a = (TCurlyLine *)gPad->GetListOfPrimitives()->Last();
   a->SetWavy();
   a->SetAmplitude(fAmplitude);
   a->SetWaveLength(fWaveLength);
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyArc()
{
//	TCurlyArc::SetDefaultIsCurly(kFALSE);
   TCurlyArc * a = (TCurlyArc*)gPad->WaitPrimitive("TCurlyArc");
   a = (TCurlyArc *)gPad->GetListOfPrimitives()->Last();
   a->SetWavy();
   a->SetAmplitude(fAmplitude);
   a->SetWaveLength(fWaveLength);
   a->SetPhimin(fPhi1);
   a->SetPhimax(fPhi2);
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyLine()
{
//	TCurlyLine::SetDefaultIsCurly(kTRUE);
   TCurlyLine * a = (TCurlyLine*)gPad->WaitPrimitive("TCurlyLine");
   a = (TCurlyLine *)gPad->GetListOfPrimitives()->Last();
   a->SetCurly();
   a->SetAmplitude(fAmplitude);
   a->SetWaveLength(fWaveLength);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyArc()
{
//	TCurlyArc::SetDefaultIsCurly(kTRUE);
   TCurlyArc * a = (TCurlyArc*)gPad->WaitPrimitive("TCurlyArc");
   a = (TCurlyArc *)gPad->GetListOfPrimitives()->Last();
//   cout << "TCurlyArc" <<endl;
   a->SetCurly();
   a->SetAmplitude(fAmplitude);
   a->SetWaveLength(fWaveLength);
   a->SetPhimin(fPhi1);
   a->SetPhimax(fPhi2);
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanSolidLine()
{
//   Style_t save = gStyle->GetLineStyle();
//   gStyle->SetLineStyle(kSolid);    
   TLine * a = (TLine*)gPad->WaitPrimitive("TLine");
   a = (TLine *)gPad->GetListOfPrimitives()->Last();
//   gStyle->SetLineStyle(save);    
   a->SetLineStyle(kSolid);    
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanDashedLine()
{
//   Style_t save = gStyle->GetLineStyle();
//   gStyle->SetLineStyle(kDashed);    
   TLine * a = (TLine*)gPad->WaitPrimitive("TLine");
   a = (TLine *)gPad->GetListOfPrimitives()->Last();
   a->SetLineStyle(kDashed);    
   a->SetLineColor(fColor);
   a->SetLineWidth(fWidth);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void FeynmanDiagramDialog::SaveDefaults()
{
   cout << "FeynmanDiagramDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
   env.SetValue("FeynmanDiagramDialog.fWaveLength" , fWaveLength);
   env.SetValue("FeynmanDiagramDialog.fAmplitude"  , fAmplitude );
   env.SetValue("FeynmanDiagramDialog.fColor"		, fColor 	 ); 
   env.SetValue("FeynmanDiagramDialog.fWidth"		, fWidth 	 );
   env.SetValue("FeynmanDiagramDialog.fPhi1" 		, fPhi1  	 );
   env.SetValue("FeynmanDiagramDialog.fPhi2" 		, fPhi2  	 ); 
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void FeynmanDiagramDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fWaveLength = env.GetValue("FeynmanDiagramDialog.fWaveLength" , 0.02);
   fAmplitude  = env.GetValue("FeynmanDiagramDialog.fAmplitude"  , 0.01);
   fColor   	= env.GetValue("FeynmanDiagramDialog.fColor"      , 1); 
   fWidth   	= env.GetValue("FeynmanDiagramDialog.fWidth"  	  , 2);
   fPhi1	 	   = env.GetValue("FeynmanDiagramDialog.fPhi1"  	  , 0);
   fPhi2	 	   = env.GetValue("FeynmanDiagramDialog.fPhi2"  	  , 180); 
} 
//_________________________________________________________________________
            
void FeynmanDiagramDialog::CloseDown()
{
   cout << "FeynmanDiagramDialog::CloseDown()" << endl;
   delete this;
}

