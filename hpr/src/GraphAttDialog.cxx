#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TGraph.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "GraphAttDialog.h"
#include "support.h"
#include <iostream>

namespace std {} using namespace std;
TString GraphAttDialog::fDrawOptGraph("AL");
Int_t   GraphAttDialog::fGraphSimpleLine;
Int_t   GraphAttDialog::fGraphSmoothLine;
Int_t   GraphAttDialog::fGraphFill;
Int_t   GraphAttDialog::fGraphPolyMarker;
Int_t   GraphAttDialog::fGraphBarChart;
Int_t   GraphAttDialog::fGraphShowAxis;
Int_t   GraphAttDialog::fGraphShowTitle;
Style_t GraphAttDialog::fGraphLStyle;
Width_t GraphAttDialog::fGraphLWidth;
Color_t GraphAttDialog::fGraphLColor;
Style_t GraphAttDialog::fGraphMStyle;
Size_t  GraphAttDialog::fGraphMSize;
Color_t GraphAttDialog::fGraphMColor;
Style_t GraphAttDialog::fGraphFStyle;
Color_t GraphAttDialog::fGraphFColor;
Int_t   GraphAttDialog::fGraphLogX = 0;
Int_t   GraphAttDialog::fGraphLogY = 0;
Int_t   GraphAttDialog::fGraphLogZ = 0;



//_______________________________________________________________________

GraphAttDialog::GraphAttDialog(TGWindow * win)
{
static const char helptext[] =
"\n\
no help yet\n\
____________________________________________________________\n\
";
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
   static Int_t dummy;
   static TString stycmd("SetAsDefault()");
   RestoreDefaults();
	fRow_lab->Add(new TObjString("CheckButton_Simple line "));
   fValp[ind++] = &fGraphSimpleLine;
	fRow_lab->Add(new TObjString("CheckButton+Smooth Curve"));
   fValp[ind++] = &fGraphSmoothLine;
	fRow_lab->Add(new TObjString("CheckButton+Marker      "));
   fValp[ind++] = &fGraphPolyMarker;
	fRow_lab->Add(new TObjString("CheckButton_Bar chart   "));
   fValp[ind++] = &fGraphBarChart;
	fRow_lab->Add(new TObjString("CheckButton+Show Axis   "));
   fValp[ind++] = &fGraphShowAxis;
	fRow_lab->Add(new TObjString("CheckButton+Show Title  "));
	fValp[ind++] = &fGraphShowTitle;
	fRow_lab->Add(new TObjString("CheckButton_Fill area"));
	fValp[ind++] = &fGraphFill;
	fRow_lab->Add(new TObjString("Fill_Select+F Style"));
   fValp[ind++] = &fGraphFStyle;
	fRow_lab->Add(new TObjString("ColorSelect+F Color"));
	fValp[ind++] = &fGraphFColor;
	fRow_lab->Add(new TObjString("LineSSelect_L Style"));
	fValp[ind++] = &fGraphLStyle;
	fRow_lab->Add(new TObjString("PlainShtVal+L Width"));
   fValp[ind++] = &fGraphLWidth;
	fRow_lab->Add(new TObjString("ColorSelect+L Color"));
   fValp[ind++] = &fGraphLColor;
	fRow_lab->Add(new TObjString("Mark_Select_M Style"));
   fValp[ind++] = &fGraphMStyle;
	fRow_lab->Add(new TObjString("Float_Value+M Size"));
   fValp[ind++] = &fGraphMSize;
	fRow_lab->Add(new TObjString("ColorSelect+M Color"));
   fValp[ind++] = &fGraphMColor;
	fRow_lab->Add(new TObjString("CheckButton_Log X scale"));
	fValp[ind++] = &fGraphLogX;
	fRow_lab->Add(new TObjString("CheckButton+Log Y scale"));
	fValp[ind++] = &fGraphLogY;
	Int_t logz = ind;
	fRow_lab->Add(new TObjString("CheckButton+Log Z scale"));
	fValp[ind++] = &fGraphLogZ;
	fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;

//	fRow_lab->Add(new TObjString("CheckButton_ylow=rwymin"};
 //  fValp[ind++] = &;

   static Int_t ok;
   Int_t itemwidth = 360;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	fDialog->DisableButton(logz);
}
//_______________________________________________________________________

void GraphAttDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "GraphAttDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void GraphAttDialog::CloseDialog()
{
//   cout << "GraphAttDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void GraphAttDialog::SetGraphAtt(TCanvas *ca, Int_t bid)
{
   fDrawOptGraph  = "";
   if (fGraphSimpleLine) fDrawOptGraph += "L";
   if (fGraphSmoothLine) fDrawOptGraph += "C";
   if (fGraphFill)       fDrawOptGraph += "F";
   if (fGraphPolyMarker) fDrawOptGraph += "P";
   if (fGraphBarChart)   fDrawOptGraph += "B";
   if (fGraphShowAxis)   fDrawOptGraph += "A";
	if (fGraphShowTitle) 
		gStyle->SetOptTitle(kTRUE);
	else 
		gStyle->SetOptTitle(kFALSE);
   if (ca) {
		if ( fGraphLogX )
			ca->SetLogx();
		else
			ca->SetLogx(kFALSE);
		if ( fGraphLogY )
			ca->SetLogy();
		else
			ca->SetLogy(kFALSE);
		TList logr;
      Int_t ngr = FindGraphs((TPad*)ca, &logr);
      if (ngr > 0) {
         TIter next(&logr);
         TObject * obj;
         while ( (obj = next()) ) {
            TGraph *gr =(TGraph*)obj;
//            gr->Paint(fDrawOptGraph);
            ca->cd();
            gr->SetDrawOption(fDrawOptGraph);
            if (fGraphSimpleLine || fGraphSmoothLine) {
               gr->SetLineStyle(fGraphLStyle);
               gr->SetLineWidth(fGraphLWidth);
               gr->SetLineColor(fGraphLColor);
					if ( fGraphFill ) {
						gr->SetFillStyle(fGraphFStyle);
						gr->SetFillColor(fGraphFColor);
					}
            }
            if (fGraphPolyMarker) {
               gr->SetMarkerStyle(fGraphMStyle);
               gr->SetMarkerSize (fGraphMSize );
               gr->SetMarkerColor(fGraphMColor);
            }
//            cout <<"fDrawOptGraph " << fDrawOptGraph<< endl;
         }
         ca->Modified();
         ca->Update();
      }
   }
}
//_______________________________________________________________________

void GraphAttDialog::SetAsDefault()
{
	SaveDefaults();
}
//_______________________________________________________________________

void GraphAttDialog::SaveDefaults()
{
   TEnv env(".hprrc");  
   env.SetValue("GraphAttDialog.DrawOptGraph",     fDrawOptGraph.Data());
   env.SetValue("GraphAttDialog.fGraphSmoothLine", fGraphSmoothLine);
   env.SetValue("GraphAttDialog.fGraphSimpleLine", fGraphSimpleLine);
   env.SetValue("GraphAttDialog.fGraphFill",       fGraphFill);
   env.SetValue("GraphAttDialog.fGraphPolyMarker", fGraphPolyMarker);
   env.SetValue("GraphAttDialog.fGraphBarChart",   fGraphBarChart);
   env.SetValue("GraphAttDialog.fGraphShowAxis",   fGraphShowAxis);
   env.SetValue("GraphAttDialog.fGraphLStyle",     fGraphLStyle);
   env.SetValue("GraphAttDialog.fGraphLWidth",     fGraphLWidth);
   env.SetValue("GraphAttDialog.fGraphLColor",     fGraphLColor);
   env.SetValue("GraphAttDialog.fGraphMStyle",     fGraphMStyle);
   env.SetValue("GraphAttDialog.fGraphMSize",      fGraphMSize);
   env.SetValue("GraphAttDialog.fGraphMColor",     fGraphMColor);
	env.SetValue("GraphAttDialog.fGraphFStyle",     fGraphFStyle);
	env.SetValue("GraphAttDialog.fGraphFColor",     fGraphFColor);
	env.SetValue("GraphAttDialog.fGraphLogX",       fGraphLogX);
	env.SetValue("GraphAttDialog.fGraphLogY",       fGraphLogY);
	env.SetValue("GraphAttDialog.fGraphLogZ",       fGraphLogZ);
	env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void GraphAttDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fDrawOptGraph    = env.GetValue("GraphAttDialog.DrawOptGraph", "A*");
   fGraphSmoothLine = env.GetValue("GraphAttDialog.fGraphSmoothLine", 0);
   fGraphSimpleLine = env.GetValue("GraphAttDialog.fGraphSimpleLine", 0);
	fGraphFill       = env.GetValue("GraphAttDialog.fGraphFill", 0);
   fGraphPolyMarker = env.GetValue("GraphAttDialog.fGraphPolyMarker", 1);
   fGraphBarChart   = env.GetValue("GraphAttDialog.fGraphBarChart", 0);
   fGraphShowAxis   = env.GetValue("GraphAttDialog.fGraphShowAxis", 1);
   fGraphLStyle     = env.GetValue("GraphAttDialog.fGraphLStyle", 1);
   fGraphLWidth     = env.GetValue("GraphAttDialog.fGraphLWidth", 1);
   fGraphLColor     = env.GetValue("GraphAttDialog.fGraphLColor", 1);
   fGraphMStyle     = env.GetValue("GraphAttDialog.fGraphMStyle", 7);
   fGraphMSize      = env.GetValue("GraphAttDialog.fGraphMSize",  1);
   fGraphMColor     = env.GetValue("GraphAttDialog.fGraphMColor", 1);
	fGraphFStyle     = env.GetValue("GraphAttDialog.fGraphFStyle", 0);
	fGraphFColor     = env.GetValue("GraphAttDialog.fGraphFColor", 1);
	fGraphLogX       = env.GetValue("GraphAttDialog.fGraphLogX", 0);
	fGraphLogY       = env.GetValue("GraphAttDialog.fGraphLogY", 0);
	fGraphLogZ       = env.GetValue("GraphAttDialog.fGraphLogZ", 0);
//	gStyle->SetOptLogx      (fGraphLogX);
//	gStyle->SetOptLogy      (fGraphLogY);
}
//______________________________________________________________________

void GraphAttDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void GraphAttDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//  cout << ClassName() << " CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetGraphAtt(canvas, bid);
}

