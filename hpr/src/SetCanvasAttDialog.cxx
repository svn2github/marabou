#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFrame.h"
#include "TH1.h"
#include "TStyle.h"
#include "SetCanvasAttDialog.h"
#include <iostream>

namespace std {} using namespace std;

//TString SetCanvasAttDialog::fDrawOpt2Dim = "COLZ";
//_______________________________________________________________________

SetCanvasAttDialog::SetCanvasAttDialog(TGWindow * win)
{
static const Char_t helptext[] =" no help yet";


   RestoreDefaults();

   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();
   
   Int_t ind = 0;
   static Int_t dummy;
   static TString stycmd("SetCanvasAttPerm()");

   fRow_lab->Add(new TObjString("CommentOnly_Pad Attributes"));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("ColorSelect_Color"));       
   fRow_lab->Add(new TObjString("PlainShtVal+BorderSize"));  
   fRow_lab->Add(new TObjString("PlainIntVal+BorderMode"));  
   fRow_lab->Add(new TObjString("Float_Value_BottomMargin"));
   fRow_lab->Add(new TObjString("Float_Value+TopMargin"));   
   fRow_lab->Add(new TObjString("CheckButton+GridY"));       
   fRow_lab->Add(new TObjString("Float_Value_LeftMargin"));  
   fRow_lab->Add(new TObjString("Float_Value+RightMargin")); 
   fRow_lab->Add(new TObjString("CheckButton+GridX"));       
//   fRow_lab->Add(new TObjString("CheckButton+TickX"));       
//   fRow_lab->Add(new TObjString("CheckButton+TickY"));
   fValp[ind++] = &fPadColor;       
   fValp[ind++] = &fPadBorderSize;
   fValp[ind++] = &fPadBorderMode; 
   fValp[ind++] = &fPadBottomMargin;
   fValp[ind++] = &fPadTopMargin;
   fValp[ind++] = &fPadGridY;      
   fValp[ind++] = &fPadLeftMargin; 
   fValp[ind++] = &fPadRightMargin;
   fValp[ind++] = &fPadGridX;      
//   fValp[ind++] = &fPadTickX;     
 //  fValp[ind++] = &fPadTickY;

   fRow_lab->Add(new TObjString("CommentOnly_Frame Attributes"));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("ColorSelect_FillColor"));   
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));   
   fRow_lab->Add(new TObjString("ColorSelect_LineColor"));   
   fRow_lab->Add(new TObjString("LineSSelect+LineStyle"));   
   fRow_lab->Add(new TObjString("PlainShtVal+LineWidth"));   
   fRow_lab->Add(new TObjString("PlainShtVal_BorderSize"));  
   fRow_lab->Add(new TObjString("PlainIntVal+BorderMode"));  
   fValp[ind++] = &fFrameFillColor; 
   fValp[ind++] = &fFrameFillStyle; 
   fValp[ind++] = &fFrameLineColor; 
   fValp[ind++] = &fFrameLineStyle; 
   fValp[ind++] = &fFrameLineWidth;
   fValp[ind++] = &fFrameBorderSize; 
   fValp[ind++] = &fFrameBorderMode; 

   fRow_lab->Add(new TObjString("CommentOnly_Canvas Attributes"));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("ColorSelect_Color"));  	 
   fRow_lab->Add(new TObjString("PlainShtVal+BorderSize")); 
   fRow_lab->Add(new TObjString("PlainIntVal+BorderMode")); 
   fRow_lab->Add(new TObjString("PlainIntVal_DefHeight"));		 
   fRow_lab->Add(new TObjString("PlainIntVal+DefWidth"));		 
   fRow_lab->Add(new TObjString("PlainIntVal_DefXpos"));		 
   fRow_lab->Add(new TObjString("PlainIntVal+DefYpos"));		   
   fValp[ind++] = &fCanvasColor; 	
   fValp[ind++] = &fCanvasBorderSize;
   fValp[ind++] = &fCanvasBorderMode;
   fValp[ind++] = &fCanvasDefH;     
   fValp[ind++] = &fCanvasDefW;     
   fValp[ind++] = &fCanvasDefX;     
   fValp[ind++] = &fCanvasDefY;


   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;

   static Int_t ok; 
   Int_t itemwidth = 360;
   fDialog = 
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

void SetCanvasAttDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) { 
 //     cout << "SetCanvasAttDialog: CloseDialog "  << endl;
      CloseDialog(); 
   }
}
//_______________________________________________________________________

void SetCanvasAttDialog::CloseDialog()
{
//   cout << "SetCanvasAttDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void SetCanvasAttDialog::SetCanvasAtt(TCanvas *canvas)
{
   if (!canvas) return;
//   gStyle->SetPadColor       (fPadColor       ); 
//   gStyle->SetPadBorderSize  (fPadBorderSize  ); 
//   gStyle->SetPadBorderMode  (fPadBorderMode  ); 

   canvas->SetBottomMargin(fPadBottomMargin); 
   canvas->SetTopMargin   (fPadTopMargin   ); 
   canvas->SetLeftMargin  (fPadLeftMargin  ); 
   canvas->SetRightMargin (fPadRightMargin ); 
   canvas->SetGridx       ((Bool_t)fPadGridX); 
   canvas->SetGridy       ((Bool_t)fPadGridY); 
   canvas->SetTickx       (fPadTickX       ); 
   canvas->SetTicky       (fPadTickY       ); 
   canvas->GetFrame()->SetFillColor (fFrameFillColor  );
   canvas->GetFrame()->SetLineColor (fFrameLineColor  );
   canvas->GetFrame()->SetFillStyle (fFrameFillStyle  );
   canvas->GetFrame()->SetLineStyle (fFrameLineStyle  );
   canvas->GetFrame()->SetLineWidth (fFrameLineWidth  );
   canvas->GetFrame()->SetBorderSize(fFrameBorderSize );
   canvas->GetFrame()->SetBorderMode(fFrameBorderMode );
   canvas->SetFillColor  	  (fCanvasColor     );
   canvas->SetBorderSize  (fCanvasBorderSize);
   canvas->SetBorderMode  (fCanvasBorderMode);
//   canvas->SetCanvasDefH		  (fCanvasDefH		  );
//   canvas->SetCanvasDefW		  (fCanvasDefW		  );
//   canvas->SetCanvasDefX		  (fCanvasDefX		  );
//   canvas->SetCanvasDefY		  (fCanvasDefY		  );

	canvas->Pop();
	canvas->cd();
	canvas->Modified();
	canvas->Update();
}                  
//______________________________________________________________________

void SetCanvasAttDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("SetCanvasAttDialog.fPadColor", fPadColor);
   env.SetValue("SetCanvasAttDialog.fPadBorderSize", fPadBorderSize);
   env.SetValue("SetCanvasAttDialog.fPadBorderMode", fPadBorderMode);
   env.SetValue("SetCanvasAttDialog.fPadBottomMargin", fPadBottomMargin);
   env.SetValue("SetCanvasAttDialog.fPadTopMargin", fPadTopMargin);
   env.SetValue("SetCanvasAttDialog.fPadLeftMargin", fPadLeftMargin);
   env.SetValue("SetCanvasAttDialog.fPadRightMargin", fPadRightMargin);
   env.SetValue("SetCanvasAttDialog.fPadGridX", fPadGridX);
   env.SetValue("SetCanvasAttDialog.fPadGridY", fPadGridY);
   env.SetValue("SetCanvasAttDialog.fPadTickX", fPadTickX);
   env.SetValue("SetCanvasAttDialog.fPadTickY", fPadTickY);
   env.SetValue("SetCanvasAttDialog.fFrameFillColor", fFrameFillColor);
   env.SetValue("SetCanvasAttDialog.fFrameLineColor", fFrameLineColor);
   env.SetValue("SetCanvasAttDialog.fFrameFillStyle", fFrameFillStyle);
   env.SetValue("SetCanvasAttDialog.fFrameLineStyle", fFrameLineStyle);
   env.SetValue("SetCanvasAttDialog.fFrameLineWidth", fFrameLineWidth);
   env.SetValue("SetCanvasAttDialog.fFrameBorderSize", fFrameBorderSize);
   env.SetValue("SetCanvasAttDialog.fFrameBorderMode", fFrameBorderMode);
   env.SetValue("SetCanvasAttDialog.fCanvasColor", fCanvasColor);
   env.SetValue("SetCanvasAttDialog.fCanvasBorderSize", fCanvasBorderSize);
   env.SetValue("SetCanvasAttDialog.fCanvasBorderMode", fCanvasBorderMode);
   env.SetValue("SetCanvasAttDialog.fCanvasDefH", fCanvasDefH);
   env.SetValue("SetCanvasAttDialog.fCanvasDefW", fCanvasDefW);
   env.SetValue("SetCanvasAttDialog.fCanvasDefX", fCanvasDefX);
   env.SetValue("SetCanvasAttDialog.fCanvasDefY", fCanvasDefY);
   env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void SetCanvasAttDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fPadColor         = env.GetValue("SetCanvasAttDialog.fPadColor", 0);
   fPadBorderSize    = env.GetValue("SetCanvasAttDialog.fPadBorderSize", 1);
   fPadBorderMode    = env.GetValue("SetCanvasAttDialog.fPadBorderMode", 0);
   fPadBottomMargin  = env.GetValue("SetCanvasAttDialog.fPadBottomMargin", 0.1);
   fPadTopMargin     = env.GetValue("SetCanvasAttDialog.fPadTopMargin", 0.1);
   fPadLeftMargin    = env.GetValue("SetCanvasAttDialog.fPadLeftMargin", 0.1);
   fPadRightMargin   = env.GetValue("SetCanvasAttDialog.fPadRightMargin", 0.1);
   fPadGridX         = env.GetValue("SetCanvasAttDialog.fPadGridX", 0);
   fPadGridY         = env.GetValue("SetCanvasAttDialog.fPadGridY", 0);
//   fPadTickX         = env.GetValue("SetCanvasAttDialog.fPadTickX", 0);
//   fPadTickY         = env.GetValue("SetCanvasAttDialog.fPadTickY", 0);
   fFrameFillColor   = env.GetValue("SetCanvasAttDialog.fFrameFillColor", 0);
   fFrameLineColor   = env.GetValue("SetCanvasAttDialog.fFrameLineColor", 1);
   fFrameFillStyle   = env.GetValue("SetCanvasAttDialog.fFrameFillStyle", 0);
   fFrameLineStyle   = env.GetValue("SetCanvasAttDialog.fFrameLineStyle", 1);
   fFrameLineWidth   = env.GetValue("SetCanvasAttDialog.fFrameLineWidth", 1);
   fFrameBorderSize  = env.GetValue("SetCanvasAttDialog.fFrameBorderSize", 1);
   fFrameBorderMode  = env.GetValue("SetCanvasAttDialog.fFrameBorderMode", 0);
   fCanvasColor      = env.GetValue("SetCanvasAttDialog.fCanvasColor", 0);
   fCanvasBorderSize = env.GetValue("SetCanvasAttDialog.fCanvasBorderSize", 1);
   fCanvasBorderMode = env.GetValue("SetCanvasAttDialog.fCanvasBorderMode", 0);
   fCanvasDefH       = env.GetValue("SetCanvasAttDialog.fCanvasDefH", 0);
   fCanvasDefW       = env.GetValue("SetCanvasAttDialog.fCanvasDefW", 0);
   fCanvasDefX       = env.GetValue("SetCanvasAttDialog.fCanvasDefX", 0);
   fCanvasDefY       = env.GetValue("SetCanvasAttDialog.fCanvasDefY", 0);
}
//______________________________________________________________________

void SetCanvasAttDialog::SetDefaults()
{
   TEnv env(".hprrc");
   gStyle->SetPadColor        (env.GetValue("SetCanvasAttDialog.fPadColor", 0)); 
   gStyle->SetPadBorderSize   (env.GetValue("SetCanvasAttDialog.fPadBorderSize", 1)); 
   gStyle->SetPadBorderMode   (env.GetValue("SetCanvasAttDialog.fPadBorderMode", 1)); 
   gStyle->SetPadBottomMargin (env.GetValue("SetCanvasAttDialog.fPadBottomMargin", 0.1)); 
   gStyle->SetPadTopMargin    (env.GetValue("SetCanvasAttDialog.fPadTopMargin", 0.1)); 
   gStyle->SetPadLeftMargin   (env.GetValue("SetCanvasAttDialog.fPadLeftMargin", 0.1)); 
   gStyle->SetPadRightMargin  (env.GetValue("SetCanvasAttDialog.fPadRightMargin", 0.1)); 
   gStyle->SetPadGridX        (env.GetValue("SetCanvasAttDialog.fPadGridX", 0)); 
   gStyle->SetPadGridY        (env.GetValue("SetCanvasAttDialog.fPadGridY", 0)); 
//   gStyle->SetPadTickX        (env.GetValue("SetCanvasAttDialog.fPadTickX", 0)); 
//   gStyle->SetPadTickY        (env.GetValue("SetCanvasAttDialog.fPadTickY", 0)); 
   gStyle->SetFrameFillColor  (env.GetValue("SetCanvasAttDialog.fFrameFillColor", 1));
   gStyle->SetFrameLineColor  (env.GetValue("SetCanvasAttDialog.fFrameLineColor", 1));
   gStyle->SetFrameFillStyle  (env.GetValue("SetCanvasAttDialog.fFrameFillStyle", 0));
   gStyle->SetFrameLineStyle  (env.GetValue("SetCanvasAttDialog.fFrameLineStyle", 1));
   gStyle->SetFrameLineWidth  (env.GetValue("SetCanvasAttDialog.fFrameLineWidth", 1));
   gStyle->SetFrameBorderSize (env.GetValue("SetCanvasAttDialog.fFrameBorderSize", 1));
   gStyle->SetFrameBorderMode (env.GetValue("SetCanvasAttDialog.fFrameBorderMode", 1));
   gStyle->SetCanvasColor     (env.GetValue("SetCanvasAttDialog.fCanvasColor", 0));
   gStyle->SetCanvasBorderSize(env.GetValue("SetCanvasAttDialog.fCanvasBorderSize", 1));
   gStyle->SetCanvasBorderMode(env.GetValue("SetCanvasAttDialog.fCanvasBorderMode", 1));
   gStyle->SetCanvasDefH      (env.GetValue("SetCanvasAttDialog.fCanvasDefH", 0));
   gStyle->SetCanvasDefW      (env.GetValue("SetCanvasAttDialog.fCanvasDefW", 0));
   gStyle->SetCanvasDefX      (env.GetValue("SetCanvasAttDialog.fCanvasDefX", 0));
   gStyle->SetCanvasDefY      (env.GetValue("SetCanvasAttDialog.fCanvasDefY", 0));
}
//______________________________________________________________________

void SetCanvasAttDialog::SetCanvasAttPerm()
{
   SaveDefaults();
   gStyle->SetPadColor        (fPadColor        ); 
   gStyle->SetPadBorderSize   (fPadBorderSize   ); 
   gStyle->SetPadBorderMode   (fPadBorderMode   ); 
   gStyle->SetPadBottomMargin (fPadBottomMargin ); 
   gStyle->SetPadTopMargin    (fPadTopMargin    ); 
   gStyle->SetPadLeftMargin   (fPadLeftMargin   ); 
   gStyle->SetPadRightMargin  (fPadRightMargin  ); 
   gStyle->SetPadGridX        ((Bool_t)fPadGridX); 
   gStyle->SetPadGridY        ((Bool_t)fPadGridY); 
//   gStyle->SetPadTickX        (fPadTickX        ); 
//   gStyle->SetPadTickY        (fPadTickY        ); 
   gStyle->SetFrameFillColor  (fFrameFillColor  );
   gStyle->SetFrameLineColor  (fFrameLineColor  );
   gStyle->SetFrameFillStyle  (fFrameFillStyle  );
   gStyle->SetFrameLineStyle  (fFrameLineStyle  );
   gStyle->SetFrameLineWidth  (fFrameLineWidth  );
   gStyle->SetFrameBorderSize (fFrameBorderSize );
   gStyle->SetFrameBorderMode (fFrameBorderMode );
   gStyle->SetCanvasColor     (fCanvasColor     );
   gStyle->SetCanvasBorderSize(fCanvasBorderSize);
   gStyle->SetCanvasBorderMode(fCanvasBorderMode);
   gStyle->SetCanvasDefH      (fCanvasDefH      );
   gStyle->SetCanvasDefW      (fCanvasDefW      );
   gStyle->SetCanvasDefX      (fCanvasDefX      );
   gStyle->SetCanvasDefY      (fCanvasDefY      );
}
//______________________________________________________________________

void SetCanvasAttDialog::CloseDown(Int_t wid)
{
   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void SetCanvasAttDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetCanvasAtt(canvas);
}
