#include "TROOT.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TRootCanvas.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TSystem.h"
#include "Set2DimOptDialog.h"
#include <iostream>

namespace std {} using namespace std;

//TString Set2DimOptDialog::fDrawOpt2Dim = "COLZ";
//_______________________________________________________________________

Set2DimOptDialog::Set2DimOptDialog(Int_t batch)
{
	if (batch);
	cout << "ctor Set2DimOptDialog, non interactive" <<endl;
	fDialog = NULL;
}
//_______________________________________________________________________

Set2DimOptDialog::Set2DimOptDialog(TGWindow * win)
{
static const Char_t helptext[] =
"Note: Changeing options only influence the current histogram\n\
       To make them active for subsequently displayed histograms\n\
		 press: \"Set as global default\"\n\
\n\
\"Reset all to default\" sets \"factory \" defaults\n\
To make these permanent also needs \"Set as global def\"\n\
\n\
Most of the value in this widget are self explaining\n\
If \"Live statbox\" a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values.\n\
\n\
X, Y and Z scales can be set by default to logarithmic or linear.\n\
This can still be reset for individual histograms.\n\
In detail: If a canvas is closed and its lin-log state differs\n\
from the global default its state is stored and restored when\n\
the histogram is shown again.\n\
\n\
SCAT  : Draw a scatter-plot (default)\n\
BOX   : a box is drawn for each cell with surface proportional to the\n\
          content's absolute value. A negative content is marked with a X.\n\
BOX1  : a button is drawn for each cell with surface proportional to\n\
          content's absolute value. A sunken button is drawn for negative values\n\
          a raised one for positive.\n\
COL   : a box is drawn for each cell with a color scale varying with contents\n\
COLZ  : same as COL. In addition the color palette is also drawn\n\
CONT  : Draw a contour plot (same as CONT0)\n\
CONT0 : Draw a contour plot using surface colors to distinguish contours\n\
CONT1 : Draw a contour plot using line styles to distinguish contours\n\
CONT2 : Draw a contour plot using the same line style for all contours\n\
CONT3 : Draw a contour plot using fill area colors\n\
CONT4 : Draw a contour plot using surface colors (SURF option at theta = 0)\n\
CONT5 : (TGraph2D only) Draw a contour plot using Delaunay triangles\n\
LEGO  : Draw a lego plot with hidden line removal\n\
LEGO1 : Draw a lego plot with hidden surface removal\n\
LEGO2 : Draw a lego plot using colors to show the cell contents\n\
SURF  : Draw a surface plot with hidden line removal\n\
SURF1 : Draw a surface plot with hidden surface removal\n\
SURF2 : Draw a surface plot using colors to show the cell contents\n\
SURF3 : same as SURF with in addition a contour view drawn on the top\n\
SURF4 : Draw a surface using Gouraud shading\n\
SURF5 : Same as SURF3 but only the colored contour is drawn. Used with\n\
          option CYL, SPH or PSR it allows to draw colored contours on a\n\
          sphere, a cylinder or a in pseudo rapidy space. In cartesian\n\
          or polar coordinates, option SURF3 is used.\n\
ARR   : arrow mode. Shows gradient between adjacent cells\n\
TEXT  : Draw bin contents as text (format set via gStyle->SetPaintTextFormat)\n\
        TextSize is taken from MarkerSize (ts = 0.03 * ms)\n\
\n\
CYL	: Use Cylindrical coordinates. The X coordinate is mapped on the angle\n\
        and the Y coordinate on the cylinder length.\n\
POL	: Use Polar coordinates. The X coordinate is mapped on the angle and\n\
        the Y coordinate on the radius.\n\
SPH	: Use Spherical coordinates. The X coordinate is mapped on the latitude\n\
        and the Y coordinate on the longitude.\n\
PSR	: Use PseudoRapidity/Phi coordinates. The X coordinate is mapped on Phi.\n\
\n\
Option GL:\n\
Use OpenGL to display histogram together with the LEGO or SURF options\n\
\n\
Interaction with the OpenGL plots \n\
Selectable parts\n\
Different parts of the plot can be selected:\n\
\n\
    * xoz, yoz, xoy back planes: When such a plane is selected, it is highlighted\n\
	 * in green if the dynamic slicing by this plane is supported, and it is highlighted\n\
	 * in red, if the dynamic slicing is not supported.\n\
    * The plot itself: On surfaces, the selected surface is outlined in red.\n\
	 * (TF3 and ISO are not outlined).\n\
	 * On lego plots, the selected bin is highlighted.\n\
	 * The bin number and content are displayed in pad's status bar.\n\
	 * In box plots, the box or sphere is highlighted and the bin info is displayed\n\
	 * in pad's status bar.\n\
\n\
Rotation and zooming\n\
\n\
    * Rotation: When the plot is selected, it can be rotated by pressing and holding\n\
	 * the left mouse button and move the cursor.\n\
    * Zoom/Unzoom: Mouse wheel or 'j', 'J', 'k', 'K' keys. \n\
\n\
Panning\n\
The selected plot can be moved in a pad's area by pressing and holding the left mouse button\n\
and the shift key.\n\
\n\
Box cut\n\
Surface, iso, box, TF3 and parametric painters support box cut by pressing the 'c' or 'C' key\n\
when the mouse cursor is in a plot's area. That will display a transparent box, \n\
cutting away part of the surface (or boxes) in order to show internal part of plot. \n\
This box can be moved inside the plot's area (the full size of the box is equal to the\n\
plot's surrounding box) by selecting one of the box cut axes and pressing the left mouse\n\
button to move it.\n\
\n\
Plot specific interactions (dynamic slicing etc.)\n\
Currently, all gl-plots support some form of slicing. When back plane is selected \n\
(and if it's highlighted in green) you can press and hold left mouse button and shift key \n\
and move this back plane inside plot's area, creating the slice. During this \"slicing\" \n\
plot becomes semi-transparent. To remove all slices (and projected curves for surfaces)\n\
double click with left mouse button in a plot's area.\n\
\n\
Surface with option \"GLSURF\"\n\
The surface profile is displayed on the slicing plane. The profile projection is drawn\n\
on the back plane by pressing 'p' or 'P' key.\n\
For further details contact ROOTs documentation.\n\
";

   const char *fDrawOpt2[kNdrawopt] =
   {"SCAT", "BOX",    "BOX1",  "COL",   "ARR",   "TEXT",
   "CONT0", "CONT1" , "CONT2", "CONT3", "CONT4", "SURF0",
	"SURF1", "SURF2",  "SURF3", "SURF4", "SURF5", "LEGO",
	"LEGO1", "LEGO2",  "POL",   "CYL",    "SPH",  "PSR"};
	fBidTEXT = 5;
	
   TRootCanvas *rc = (TRootCanvas*)win;
   fCanvas = rc->Canvas();
   fHist = NULL;
   Int_t nh1 = 0, nh2 = 0;
   TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH2")) {
         fHist = (TH2*)obj;
         if (obj->InheritsFrom("TGraph2D")) {
            fHist = ((TGraph2D*)obj)->GetHistogram();
//            if (!fHist) continue;
		   }
         if (fHist->GetDimension() == 1) nh1++;
         if (fHist->GetDimension() > 1)  nh2++;
//			break;
      }
   }
	if ( fHist == NULL ) {
	   cout << "No Histogram in Canvas" << endl;
//		return;
	} else {
		if ( gDebug > 0 )
			cout << "Set2DimOptDialog: " << endl
			<< "TCanvas* canvas = (TCanvas*)" << fCanvas
			<< "; TH2* hist = (TH2*)" << fHist 
			<< "; Set2DimOptDialog * d2 = (Set2DimOptDialog*)" << this << endl;
	}
   RestoreDefaults();
//   Int_t selected = -1;
   for (Int_t i = 0; i < kNdrawopt; i++) {
      fDrawOpt2DimArray[i] = fDrawOpt2[i];
	}
/*		TString temp(fDrawOpt2[i]);
		temp = temp.Strip(TString::kBoth);
      if (fDrawOpt2Dim.Contains(temp) ) {
         if (selected < 0) {
				fOptRadio[i] = 1;
            selected = i;
         }
      } else {
         fOptRadio[i] = 0;
      }
   }*/
	fSameOpt = "";
   if ( fHist ) {
		GetValuesFromHist();
	}
	if (fDrawOpt2Dim.Contains("Z")) fShowZScale = 1;
	else                            fShowZScale = 0;
	if (fDrawOpt2Dim.Contains("BB")) fHideBackBox = 1;
	else                             fHideBackBox = 0;
	if (fDrawOpt2Dim.Contains("FB")) fHideFrontBox = 1;
	else                             fHideFrontBox = 0;
//	if (fDrawOpt2Dim.Contains("GL")) fUseGL = 1;
//	else                             fUseGL = 0;
// extract draw option
	for (Int_t i = kNdrawopt-1; i >= 0; i--) {
		Int_t ind = fDrawOpt2Dim.Index(fDrawOpt2DimArray[i]);
		if ( ind >= 0 ) {
			Int_t len = strlen(fDrawOpt2DimArray[i]);
			fDrawOpt2Dim = fDrawOpt2Dim.Replace(ind, len, "");
			fOptRadio[i] = 1;
		} else {
			fOptRadio[i] = 0;
		}
	}
	fHistNo = 0;
   gROOT->GetListOfCleanups()->Add(this);
   fRow_lab = new TList();

   Int_t ind = 0;
   Int_t indopt = 0;
//   static Int_t dummy;
   static TString stycmd("SetHistAttPermLocal()");
   static TString sadcmd("SetAllToDefault()");

//   fRow_lab->Add(new TObjString("CommentOnly_Scatter, Boxes, Colorplots"));
//	fRow_lab->Add(new TObjString("CommentOnly_Scatter, Boxes, Colorplots"));
//	fValp[ind++] = &dummy;
	fBidSCAT = 1;
	fBidBOX  = 2;
	fBidBOX1 = 3;
	fBidARR =  5;
	
	for (Int_t i = 0; i < kNdrawopt; i++) {
//   cout << " indopt  " <<  indopt << endl;
       TString text("CheckButton");
       if ( i%6 == 0 )text += "_";
       else       text += "+";
       if (strlen(fDrawOpt2DimArray[indopt]) < 5)
			 text += " ";
		 if (strlen(fDrawOpt2DimArray[indopt]) < 4)
			 text += " ";
		 text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
//   cout << " indopt  " <<  indopt << endl;
/*
   fRow_lab->Add(new TObjString("CommentOnly_Contour options"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("CheckButton");
       if (i == 0)text += "_";
       else       text += "+";
       if (strlen(fDrawOpt2DimArray[indopt]) < 5)
			 text += " ";
		 if (strlen(fDrawOpt2DimArray[indopt]) < 4)
			 text += " ";
		 text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
   fRow_lab->Add(new TObjString("CommentOnly_Surface options"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("CheckButton");
       if (i == 0)text += "_";
       else       text += "+";
       if (strlen(fDrawOpt2DimArray[indopt]) < 5)
			 text += " ";
		 if (strlen(fDrawOpt2DimArray[indopt]) < 4)
			 text += " ";
		 text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
   fRow_lab->Add(new TObjString("CommentOnly_LEGO, Arrows, Text"));
   fValp[ind++] = &dummy;
   for (Int_t i = 0; i < 5; i++) {
       TString text("CheckButton");
       if (i == 0)text += "_";
       else       text += "+";
       if (strlen(fDrawOpt2DimArray[indopt]) < 5)
			 text += " ";
		 if (strlen(fDrawOpt2DimArray[indopt]) < 4)
			 text += " ";
		 text += fDrawOpt2DimArray[indopt];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
*/
   fRow_lab->Add(new TObjString("ColorSelect_FillCol"));
   fBidFillColor = ind; fValp[ind++] = &fHistFillColor2Dim;
	fRow_lab->Add(new TObjString("Fill_Select+FillSty"));
	fValp[ind++] = &fHistFillStyle2Dim;
	fRow_lab->Add(new TObjString("ColorSelect+BgColor"));
	fValp[ind++] = &f2DimBackgroundColor;
	fRow_lab->Add(new TObjString("ColorSelect_LineCol"));
   fValp[ind++] = &fHistLineColor2Dim;
	fRow_lab->Add(new TObjString("LineSSelect+LStyle "));
	fValp[ind++] = &fHistLineStyle2Dim;
	fRow_lab->Add(new TObjString("PlainShtVal+LineWid"));
	fValp[ind++] = &fHistLineWidth2Dim;
	
   fRow_lab->Add(new TObjString("ColorSelect_MColor "));
   fBidMarkerColor = ind; fValp[ind++] = &fMarkerColor2Dim;
   fRow_lab->Add(new TObjString("Mark_Select+MStyle "));
   fBidMarkerStyle = ind; fValp[ind++] = &fMarkerStyle2Dim;
   fRow_lab->Add(new TObjString("Float_Value+MSize  "));
   fBidMarkerSize = ind; fValp[ind++] = &fMarkerSize2Dim;
	
	fRow_lab->Add(new TObjString("CheckButton_Use GL (3D)"));
   fRow_lab->Add(new TObjString("CheckButton+    No Fbox"));
   fRow_lab->Add(new TObjString("CheckButton+    No Bbox"));
	fValp[ind++] = &fUseGL;
	fValp[ind++] = &fHideFrontBox;
	fValp[ind++] = &fHideBackBox;
	fRow_lab->Add(new TObjString("CheckButton_    Z Scale"));
   fValp[ind++] = &fShowZScale;
   fRow_lab->Add(new TObjString("CheckButton+  Live stat"));
	fBidLiveStat = ind;
   fValp[ind++] = &fLiveStat2Dim;
	fRow_lab->Add(new TObjString("PlainIntVal+Cont Levs"));
	fValp[ind++] = &fContourLevels;
	//   fRow_lab->Add(new TObjString("DoubleValue_LogScaleMin"));
//   fRow_lab->Add(new TObjString("DoubleValue+LogScaleMax"));
//   fValp[ind++] = &fLogScaleMin;
//   fValp[ind++] = &fLogScaleMax;
	fRow_lab->Add(new TObjString("CheckButton_      Log X"));
	fRow_lab->Add(new TObjString("CheckButton+      Log Y"));
	fRow_lab->Add(new TObjString("CheckButton+      Log Z"));
	fValp[ind++] = &fTwoDimLogX;
	fValp[ind++] = &fTwoDimLogY;
	fValp[ind++] = &fTwoDimLogZ;
//	fRow_lab->Add(new TObjString("PlainIntVal-Hist No"));
//	fBidHistNo = ind;
//	fValp[ind++] = &fHistNo;


   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;
   fRow_lab->Add(new TObjString("CommandButt+Reset all to default"));
   fValp[ind++] = &sadcmd;

   static Int_t ok;
   Int_t itemwidth = 380;
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
/*
	if ( fDrawOpt2Dim.Contains("SCAT") ||   fDrawOpt2Dim.Contains("TEXT")) {
	   fDialog->EnableButton(fBidMarkerColor);
		fDialog->EnableButton(fBidMarkerStyle);
		fDialog->EnableButton(fBidMarkerSize);
   } else {
	   fDialog->DisableButton(fBidMarkerColor);
		fDialog->DisableButton(fBidMarkerStyle);
		fDialog->DisableButton(fBidMarkerSize);
	}
	if ( fDrawOpt2Dim.Contains("BOX") || fDrawOpt2Dim.Contains("ARR") ) {
	   fDialog->EnableButton(fBidFillColor);
		fDialog->EnableButton(fBidLineColor);
   } else {
	   fDialog->DisableButton(fBidFillColor);
		fDialog->DisableButton(fBidLineColor);
	}
*/
}
//_______________________________________________________________________

void Set2DimOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "Set2DimOptDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void Set2DimOptDialog::CloseDialog()
{
//   cout << "Set2DimOptDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void Set2DimOptDialog::SetHistAttNow(TCanvas *canvas)
{
	fDrawOpt2Dim = "";
   for (Int_t i = 0; i < kNdrawopt; i++) {
      if (fOptRadio[i] == 1) {
//         cout << "fDrawOpt2DimArray[" << i << "]" << endl;
         fDrawOpt2Dim += fDrawOpt2DimArray[i];
        if (fUseGL && !fDrawOpt2Dim.Contains("GL"))
			  fDrawOpt2Dim.Prepend("GL");

      }
   }
   if (!canvas) return;
	if ( fHist ) {
		SetHistAtt((TPad*)canvas, fHist); 
/*		if ( gDebug > 0 ) 
			cout << "SetHistAttNow: " << fDrawOpt2Dim << endl;
		fHist->SetDrawOption(fDrawOpt2Dim);*/
	} else {
		SetHistAttAll(canvas);
	}
	canvas->Pop();
	canvas->Modified();
	canvas->Update();
}
//_______________________________________________________________________

void Set2DimOptDialog::SetHistAttAll(TCanvas *canvas)
{
	TIter next(canvas->GetListOfPrimitives());
	TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TGraph2D")) {
         ((TGraph2D*)obj)->SetDrawOption(fDrawOpt);
		} else if ((obj->InheritsFrom("TPad"))) {
         TPad *pad = (TPad*)obj;
         TIter next1(pad->GetListOfPrimitives());
         TObject *obj1;
         pad->cd();
			while ( (obj1 = next1()) ) {
				if (obj1->InheritsFrom("TH2")) {
               SetHistAtt(pad, ((TH2*)obj1));
            }
         }
      }
   }
}
//_______________________________________________________________________

void Set2DimOptDialog::SetHistAtt(TPad *pad, TH2 * hist)
{
   if ( !pad || !hist ) return;
	pad->cd();
	pad->SetLogx(fTwoDimLogX);
	pad->SetLogy(fTwoDimLogY);
	pad->SetLogz(fTwoDimLogZ);
   fDrawOpt = fDrawOpt2Dim;
   if ( fShowZScale && (
        fDrawOpt.Contains("COL")   || fDrawOpt.Contains("CONT0")
      ||fDrawOpt.Contains("CONT1") || fDrawOpt.Contains("CONT4")
      ||fDrawOpt.Contains("CONT5")  || fDrawOpt.Contains("SURF1")
      ||fDrawOpt.Contains("SURF2") || fDrawOpt.Contains("SURF3")
      ||fDrawOpt.Contains("LEGO2") ) && fUseGL == 0 ) {
      fDrawOpt += "Z";
	}
	if ( fDrawOpt.Contains("TEXT") && fMarkerSize2Dim < 0.01 ) {
		fMarkerSize2Dim = 1;
	}
   if (fHideFrontBox) fDrawOpt += "FB";
   if (fHideBackBox)  fDrawOpt += "BB";
	if (fUseGL && ( fDrawOpt.Contains("LEGO") ||fDrawOpt.Contains("SURF"))) {
		fDrawOpt.Prepend("GL");
		gStyle->SetCanvasPreferGL(kTRUE);

	} else {
		gStyle->SetCanvasPreferGL(kFALSE);
	}
	if ( !fDrawOpt.Contains("SAME") )
		fDrawOpt += fSameOpt;
	if ( gDebug > 0 ) 
			cout << "SetHistAtt: " << fDrawOpt << endl;
	hist->SetDrawOption(fDrawOpt);
	hist->SetOption(fDrawOpt);
	hist->SetFillColor(fHistFillColor2Dim);
	hist->SetLineColor(fHistLineColor2Dim);
	hist->SetLineWidth(fHistLineWidth2Dim);
	hist->SetLineStyle(fHistLineStyle2Dim);
	hist->SetFillStyle(fHistFillStyle2Dim);
	if ( fDrawOpt.Contains("SCAT") || fDrawOpt.Contains("TEXT")){
		hist->SetMarkerColor(fMarkerColor2Dim);  
		hist->SetMarkerStyle(fMarkerStyle2Dim);  
		hist->SetMarkerSize (fMarkerSize2Dim); 
	} else {
		hist->SetMarkerColor(0);  
		hist->SetMarkerStyle(0);  
		hist->SetMarkerSize (0.); 
	}
	if ( hist->GetContour() != fContourLevels ) {
		hist->SetContour(fContourLevels);
	}
   if (f2DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         pad->GetFrame()->SetFillStyle(0);
      } else {
      	pad->GetFrame()->SetFillStyle(1001);
      	pad->GetFrame()->SetFillColor(10);
   	}
   } else {
      pad->GetFrame()->SetFillStyle(1001);
      pad->GetFrame()->SetFillColor(f2DimBackgroundColor);
   }
   
}
//______________________________________________________________________

void Set2DimOptDialog::SetHistAttPermLocal()
{
//   cout << "Set2DimOptDialog:: SetHistAttPerm()" << endl;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void Set2DimOptDialog::SetHistAttPerm()
{
    cout << "Set2DimOptDialog:: SetHistAttPerm()" << endl;
   TEnv env(".hprrc");
   env.SetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fDrawOpt2Dim", fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fLiveStat2Dim", fLiveStat2Dim);
   env.SetValue("Set2DimOptDialog.fShowZScale", fShowZScale);
	env.SetValue("Set2DimOptDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimOptDialog.fHistFillColor2Dim",fHistFillColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistFillStyle2Dim",fHistFillStyle2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineColor2Dim",fHistLineColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineStyle2Dim",fHistLineStyle2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineWidth2Dim",fHistLineWidth2Dim);
	env.SetValue("Set2DimOptDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
	env.SetValue("Set2DimOptDialog.fTwoDimLogX",       fTwoDimLogX       );
	env.SetValue("Set2DimOptDialog.fTwoDimLogY",       fTwoDimLogY       );
	env.SetValue("Set2DimOptDialog.fTwoDimLogZ",       fTwoDimLogZ       );
	env.SetValue("Set2DimOptDialog.fUseGL",            fUseGL            );
	
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set2DimOptDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("Set2DimOptDialog.fDrawOpt2Dim",      fDrawOpt2Dim);
   env.SetValue("Set2DimOptDialog.fLiveStat2Dim",     fLiveStat2Dim);
   env.SetValue("Set2DimOptDialog.fShowZScale",       fShowZScale);
	env.SetValue("Set2DimOptDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimOptDialog.fHistFillColor2Dim",fHistFillColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistFillStyle2Dim",fHistFillStyle2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineColor2Dim",fHistLineColor2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineStyle2Dim",fHistLineStyle2Dim);
	env.SetValue("Set2DimOptDialog.fHistLineWidth2Dim",fHistLineWidth2Dim);
	env.SetValue("Set2DimOptDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimOptDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
	env.SetValue("Set2DimOptDialog.fTwoDimLogX",       fTwoDimLogX       );
	env.SetValue("Set2DimOptDialog.fTwoDimLogY",       fTwoDimLogY       );
	env.SetValue("Set2DimOptDialog.fTwoDimLogZ",       fTwoDimLogZ       );
	env.SetValue("Set2DimOptDialog.fUseGL",            fUseGL            );
	env.SetValue("Set2DimOptDialog.fContourLevels",    fContourLevels    );
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set2DimOptDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void Set2DimOptDialog::GetValuesFromHist()
{
   if ( !fHist ) return;
	fCanvas->cd();
	fDrawOpt2Dim = fHist->GetDrawOption();
	if ( fDrawOpt2Dim.Contains("SAME") )
		fSameOpt = "SAME";
	if ( gDebug > 0 )
		cout << "fDrawOpt2Dim::GetValuesFromHist() " << fDrawOpt2Dim << endl;
	fTwoDimLogX        = fCanvas->GetLogx();
	fTwoDimLogY        = fCanvas->GetLogy();
	fTwoDimLogZ        = fCanvas->GetLogz();
/*	for (Int_t i = kNdrawopt-1; i >= 0; i--) {
		Int_t ind = fDrawOpt2Dim.Index(fDrawOpt2DimArray[i]);
		if ( ind >= 0 ) {
			Int_t len = strlen(fDrawOpt2DimArray[i]);
			fDrawOpt2Dim = fDrawOpt2Dim.Replace(ind, len, "");
			fOptRadio[i] = 1;
		} else {
			fOptRadio[i] = 0;
		}
	}*/
	fHistFillColor2Dim = fHist->GetFillColor();
	fHistFillStyle2Dim = fHist->GetFillStyle();
	fHistLineColor2Dim = fHist->GetLineColor();
	fHistLineStyle2Dim = fHist->GetLineStyle();
	fHistLineWidth2Dim = fHist->GetLineWidth();
	fMarkerColor2Dim   = fHist->GetMarkerColor();
	fMarkerStyle2Dim   = fHist->GetMarkerStyle();
	fMarkerSize2Dim    = fHist->GetMarkerSize();
}
//______________________________________________________________________

void Set2DimOptDialog::RestoreDefaults(Int_t resetall)
{
	if ( gDebug > 0 )
		cout << "Set2DimOptDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
   TEnv env(envname);
   fDrawOpt2Dim       = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
   fShowZScale        = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
   fLiveStat2Dim      = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
	f2DimBackgroundColor = env.GetValue("Set2DimOptDialog.f2DimBackgroundColor", 0);
	fHistFillColor2Dim = env.GetValue("Set2DimOptDialog.fHistFillColor2Dim", 1);
	fHistFillStyle2Dim = env.GetValue("Set2DimOptDialog.fHistFillStyle2Dim", 0);
	fHistLineColor2Dim = env.GetValue("Set2DimOptDialog.fHistLineColor2Dim", 1);
	fHistLineStyle2Dim = env.GetValue("Set2DimOptDialog.fHistLineStyle2Dim", 1);
	fHistLineWidth2Dim = env.GetValue("Set2DimOptDialog.fHistLineWidth2Dim", 1);
	fMarkerColor2Dim   = env.GetValue("Set2DimOptDialog.fMarkerColor2Dim",   1);
	fMarkerStyle2Dim   = env.GetValue("Set2DimOptDialog.fMarkerStyle2Dim",   1);
	fMarkerSize2Dim    = env.GetValue("Set2DimOptDialog.fMarkerSize2Dim",    1);
	fTwoDimLogX        = env.GetValue("Set2DimOptDialog.fTwoDimLogX",        0);
	fTwoDimLogY        = env.GetValue("Set2DimOptDialog.fTwoDimLogY",        0);
	fTwoDimLogZ        = env.GetValue("Set2DimOptDialog.fTwoDimLogZ",        0);
	fUseGL             = env.GetValue("Set2DimOptDialog.fUseGL",             0);
	fContourLevels     = env.GetValue("Set2DimOptDialog.fContourLevels",    20);
}
//______________________________________________________________________

void Set2DimOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void Set2DimOptDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
	if ( gDebug > 0 )
		cout << "CRButtonPressed:" << wid<< ", " <<bid << " canvas "  << canvas << endl;
//	if ( bid == fBidHistNo ) 
//		return;
/*
	const char *fDrawOpt2[kNdrawopt] =
	{"SCAT", "BOX",    "BOX1",  "COL",   "ARR",   "TEXT",
	"CONT0", "CONT1" , "CONT2", "CONT3", "CONT4", "SURF0",
	"SURF1", "SURF2",  "SURF3", "SURF4", "SURF5", "LEGO",
	"LEGO1", "LEGO2",  "POL",   "CYL",    "SPH",  "PSR"};
*/
	if ( bid == fBidLiveStat ) {
		TEnv env(".hprrc");
		env.SetValue("Set2DimOptDialog.fLiveStat2Dim",     fLiveStat2Dim);
		env.SaveLevel(kEnvLocal);
		return;
	}

	// 2-dim options exclude 3-dim
	if ( bid >= 0 && bid <= 10) {
		for ( Int_t i = 11; i <= 23; i++ ) {
			fOptRadio[i] = 0;
			fDialog->SetCheckButton(i, 0);
		}
	}
	// 3-dim options are mutually exclusive
	if ( bid >= 11 && bid <= 19) {
		for ( Int_t i = 0; i <= 19; i++ ) {
			fOptRadio[i] = 0;
			fDialog->SetCheckButton(i, 0);
		}
		fOptRadio[bid] = 1;
		fDialog->SetCheckButton(bid, 1);
	}
	// non cartesian options are mutually  exclusive
	if ( bid >= 20 && bid <= 23) {
		for ( Int_t i = 20; i <= 23; i++ ) {
			if ( gDebug > 0 )
				cout << "CRButtonPressed: fOptRadio[" << i <<"] = " << fOptRadio[i]<< endl;
			if ( bid != i ) {
				if ( fOptRadio[bid] != 0 ) {
					fOptRadio[i] = 0;
					fDialog->SetCheckButton(i, 0);
					if ( gDebug > 0 )
						cout << "CRButtonPressed: uncheck " << i << endl;
				}
			}
		}
	}
	
   SetHistAttNow(canvas);
}

