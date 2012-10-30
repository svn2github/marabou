#include "TROOT.h"
#include "TEnv.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TFrame.h"
#include "THStack.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TList.h"
#include "TMath.h"
#include "TGMsgBox.h"
#include "TRootCanvas.h"
#include "TRegexp.h"
#include "TGMrbValuesAndText.h"
#include "TGMrbTableFrame.h"
#include <iostream>
#include <fstream>
#include "hprbase.h"
#include "HistPresent.h"
#include "SetColor.h"

using std::cout;
using std::cerr;
using std::endl;

namespace Hpr 
{

void WriteHistasASCII(TH1 *hist, TGWindow *window, Int_t ascii_graph)
{
// *INDENT-OFF*
  const char helpTextASCII[] =
"As default only the channel contents are written\n\
to the file.\n\
\"Errors X\" and \"Errors Cont(Y)\" are useful if file\n\
should be used as input to Ascii2Graph: X, Y, ErrX, ErrY\n\
If First_binX and Last_binX are 0 all channels arr written";
  const char helpTextGraph[] =
"Store BinCenters, BinContents in a TGraphErrors object\n\
and write the Graph to a ROOT file\n\
If \"First_binX\" and \"Last_binX\" are 0 all channels arr written";
// *INDENT-ON*
	
   if ( hist->GetDimension() == 3 ) {
      WarnBox(" WriteHistasASCII: 3-dim not yet supported ", window);
      return;
   }
   if ( hist->GetDimension() != 1 && ascii_graph == 1) {
      WarnBox(" WriteHistasGraph: only 1-dim supported", window);
      return;
   }
   static Int_t channels   = 0;
   static Int_t bincenters = 1;
   static Int_t binw2      = 0;
   static Int_t errors     = 1;
   static Int_t first_binX  = 0;
   static Int_t last_binX   = 0;
   static Int_t first_binY  = 0;
   static Int_t last_binY   = 0;
	Int_t transX             = 0;
	Double_t offsetX = 0;
	Double_t slope = 1;
//	Int_t dummy;
   static Int_t suppress_zeros = 0;
	TFile *rfile = NULL;
	TGraphErrors *graph = NULL;
   TString fname;
   TString gname;
   TString gtitle;
	const char *helpText = NULL;
	if ( ascii_graph == 0 ) {
		fname = hist->GetName();
		fname += ".ascii";
		helpText =  &helpTextASCII[0];
	} else {
		gname  = hist->GetName();
		gtitle = hist->GetTitle();
		fname = "workfile.root";
		helpText =  &helpTextGraph[0];
	}

   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;

	if ( ascii_graph == 0 ) {
		row_lab->Add(new TObjString("StringValue_File name"));
		valp[ind++] = &fname;
	} else {
		row_lab->Add(new TObjString("StringValue_  Rootfile"));
		valp[ind++] = &fname;
		row_lab->Add(new TObjString("StringValue_Graph name"));
		valp[ind++] = &gname;
		row_lab->Add(new TObjString("StringValue_ Graph tit"));
		valp[ind++] = &gtitle;
	}
   if (hist->GetDimension() == 1) {
		if (  ascii_graph == 0 ) {
			row_lab->Add(new TObjString("CheckButton_ChannelNumbs"));
			valp[ind++] = &channels;
			row_lab->Add(new TObjString("CheckButton+   Bin Centers"));
			valp[ind++] = &bincenters;
		}
   }
	row_lab->Add(new TObjString("CheckButton_    Errors X"));
	valp[ind++] = &binw2;
   row_lab->Add(new TObjString("CheckButton+Errors Cont(Y)"));
   valp[ind++] = &errors;
   row_lab->Add(new TObjString("PlainIntVal_First_binX"));
   valp[ind++] = &first_binX;
   row_lab->Add(new TObjString("PlainIntVal+Last_binX"));
   valp[ind++] = &last_binX;
   if (hist->GetDimension() == 2 && ascii_graph == 0) {
      row_lab->Add(new TObjString("PlainIntVal_First_binY"));
      valp[ind++] = &first_binY;
      row_lab->Add(new TObjString("PlainIntVal_Last_binY"));
      valp[ind++] = &last_binY;
   }
   row_lab->Add(new TObjString("CheckButton_     Suppress empty channels"));
   valp[ind++] = &suppress_zeros;
	row_lab->Add(new TObjString("CheckButton_Apply linear trans to Xscale"));
	valp[ind++] = &transX;
   row_lab->Add(new TObjString("DoubleValue_OffsetX"));
   valp[ind++] = &offsetX;
   row_lab->Add(new TObjString("DoubleValue+Slope"));
   valp[ind++] = &slope;

   Int_t itemwidth=280;
	TString tit;
	if (  ascii_graph == 0 ) {
		tit = "Hist to ASCII-file";
	} else {
		tit = "Hist to Graph";
	}
   ok = GetStringExt(tit, NULL, itemwidth, window,
                   NULL, NULL, row_lab, valp, NULL, NULL, helpText);
   if (!ok) {
      return;
   }
   Int_t nl = 0;
   Int_t nbx1 = 1;
   Int_t nbx2 = hist->GetNbinsX();
   if (first_binX > 0) nbx1 = first_binX;
   if (last_binX > 0)  nbx2 = last_binX;
	Int_t ntot = 0;
   if ( hist->GetDimension() == 1 ) {
      for (Int_t i = nbx1; i <= nbx2; i++) {
         if ( hist->GetBinContent(i) == 0 && suppress_zeros )
				continue;
			ntot++;
		}
	}
	ofstream *outfile;
	if ( ascii_graph == 0 ) {
		if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
	//      cout << fname << " exists" << endl;
			if ( !QuestionBox("File exists, overwrite?",window) ) 
				return;
		}
		outfile= new ofstream((const char *) fname);
		if (outfile->fail()) {
			cout << "Opening: " << fname << " failed" << endl;
			return;
		}
	} else {
		rfile = new TFile(fname, "UPDATE");
		graph = new TGraphErrors(ntot);
		graph->SetName(gname);
		graph->SetTitle(gtitle);
	}
   if ( hist->GetDimension() == 1 ) {
      for (Int_t i = nbx1; i <= nbx2; i++) {
			Double_t xerr = hist->GetBinWidth(i) / TMath::Sqrt(12);
			Double_t x = hist->GetBinCenter(i);
			if ( transX )
				x = offsetX + slope * x;
			if ( ascii_graph == 0 && binw2 == 0) 
				xerr = 0;
         if (bincenters && suppress_zeros &&
             hist->GetBinContent(i)  == 0) continue;
			if ( ascii_graph == 0 ) {
				if (channels)
					*outfile << i << "\t";
				if (bincenters)
					*outfile << x << "\t";
				*outfile << hist->GetBinContent(i);
				if (binw2)
					*outfile << "\t" <<  xerr;
				if (errors)
					*outfile << "\t" << hist->GetBinError(i);
				*outfile << std::endl;
			} else {
				graph->SetPoint(nl, x, hist->GetBinContent(i)); 
				if (errors)
					graph->SetPointError(nl,xerr,hist->GetBinError(i)); 
			}
         nl++;
      }

   } else {
      Int_t nby1 = 1;
      Int_t nby2 = hist->GetNbinsY();
      if (first_binY > 0) nby1 = first_binY;
      if (last_binY > 0)  nby2 = last_binY;
      TAxis * xa = hist->GetXaxis();
      TAxis * ya = hist->GetYaxis();
      for (Int_t i = nbx1; i <= nbx2; i++) {
         for (Int_t k = nby1; k <= nby2; k++) {
            if (suppress_zeros && hist->GetCellContent(i, k)  == 0) continue;
            *outfile << xa->GetBinCenter(i) << "\t";
            *outfile << ya->GetBinCenter(k) << "\t";

            *outfile << hist->GetCellContent(i,k);
            if (errors)
            *outfile << "\t" << hist->GetCellError(i,k);
            *outfile << std::endl;
            nl++;
         }
      }
   }
   if ( ascii_graph == 0 ) {
		cout << nl << " lines written to: " << (const char *) fname << endl;
		outfile->close();
	} else {
		graph->Write();
		rfile->Close();
		cout << "Graph with " << nl << " points written to: " << fname << endl;
	}
};
//___________________________________________________________________________

void WarnBox(const char *message, TGWindow *window)
{
   Int_t retval = 0;
   new TGMsgBox(gClient->GetRoot(), window,
                "Warning", message, kMBIconExclamation, kMBDismiss,
                &retval);
}
//___________________________________________________________________________

Bool_t QuestionBox(const char *message, TGWindow *window)
{
      int buttons = kMBOk | kMBDismiss, retval = 0;
      new TGMsgBox(gClient->GetRoot(), window, "Question", message,
                   kMBIconQuestion, buttons, &retval);
       return ( retval != kMBDismiss );
}

//_______________________________________________________________________________________

TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t push_pop)
{
   if (!ca) return NULL;
   TIter next(ca->GetListOfPrimitives());
   while (TObject * obj = next()) {
      if (obj->InheritsFrom("TH1")) {
         TList *lof = ((TH1*)obj)->GetListOfFunctions();
         TObject *o = lof->FindObject(fname);
         if (o) {
            if (push_pop != 0) {
               lof->Remove(o);
               if (push_pop > 0) 
                  lof->AddFirst(o);
               else
                  lof->Add(0);
            }
            return (TH1*)o;
         }
      }
   }
   return NULL;
};
//_______________________________________________________________________________________

TH1 * FindHistInPad(TVirtualPad * ca, Int_t lfgraph, Int_t lfstack, TObject **parent)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if ( parent )
			*parent = obj;
		if (obj->InheritsFrom("TH1")) {
			return (TH1*)obj;
		}
		if (lfgraph && obj->InheritsFrom("TGraph")) {
			return ((TGraph*)obj)->GetHistogram();
		}
		if (lfstack && obj->InheritsFrom("THStack")) {
			return ((THStack*)obj)->GetHistogram();
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TGraph * FindGraphInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph")) {
			return (TGraph*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TLegend * FindLegendInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TLegend")) {
			return (TLegend*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

Bool_t HistLimitsMatch(TH1* h1, TH1* h2)
{
//	cout << " HistLimitsMatch : " << h1->GetName() 
//		<< " " << h2->GetName()<< endl;
	if (h1->GetDimension() != h2->GetDimension() ) 
		return kFALSE;
	TAxis * a1 = h1->GetXaxis();
	TAxis * a2 = h2->GetXaxis();
	if ( a1->GetNbins() != a2->GetNbins() )
		return kFALSE;
	if ( a1->GetXmin() != a2->GetXmin() )
		return kFALSE;
	if ( a1->GetXmax() != a2->GetXmax() )
		return kFALSE;
	if (h1->GetDimension() > 1 && h2->GetDimension() > 1 ) {
		a1 = h1->GetYaxis();
		a2 = h2->GetYaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	if (h1->GetDimension() > 2 && h2->GetDimension() > 2 ) {
		a1 = h1->GetZaxis();
		a2 = h2->GetZaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	return kTRUE;
}
//__________________________________________________________________

HprGaxis * DoAddAxis(TCanvas * canvas, TH1 *hist, Int_t where, 
			Double_t ledge, Double_t uedge, Double_t axis_offset, Color_t col)
{	
	canvas->cd();
//	GrCanvas* hc = (GrCanvas*)canvas;
//   canvas->Add2ConnectedClasses(this);
// we dont have a pointer to Set1DimOptDialog,
// it might not even exist (yet), so use static method
//	TQObject::Connect("Set1DimOptDialog", "LinLogChanged(TObject*)",
//						   "FitHist", this, "HandleLinLogChanged(TObject*)");
//	cout << "DoAddAxis " << ledge << " " << uedge << endl;
	Axis_t x1=0, y1=0, x2=1, y2=1;
	TIter next(canvas->GetListOfPrimitives());
//	canvas->GetListOfPrimitives()->ls();
	
	TString side("S");
	TAxis *orig_axis = NULL;
	HprGaxis *naxis;
	Double_t offset = 0, ratio = 1;
	if (where == 1) {
		if (canvas->GetLogx()) 
			side += "G";
		side += "-";
		x1 = canvas->GetFrame()->GetX1();
		x2 = canvas->GetFrame()->GetX2();
		y1 = canvas->GetFrame()->GetY2();
		// axis offset in X
		y1 = y1 + (y1 - canvas->GetFrame()->GetY1()) * axis_offset;
		y2 = y1;
		orig_axis = hist->GetXaxis();
		if ( canvas->GetLogx() ) {
			x1 = TMath::Power(10, x1);
			x2 = TMath::Power(10, x2);
		}
		offset = ledge - x1;
		ratio  = (uedge - ledge) / (x2 - x1);
	} else {
		if (canvas->GetLogy()) 
			side += "G";
		side += "+L";
		x1 = canvas->GetFrame()->GetX2();
		// axis offset in X
		x1 = x1 + (x1 - canvas->GetFrame()->GetX1()) * axis_offset;
		x2 = x1;
		y1 = canvas->GetFrame()->GetY1();
		y2 = canvas->GetFrame()->GetY2();
		orig_axis = hist->GetYaxis();
		if ( canvas->GetLogy() ) {
			y1 = TMath::Power(10, y1);
			y2 = TMath::Power(10, y2);
		}
		offset = ledge - y1;
		ratio  = (uedge - ledge) / (y2 - y1);
	}
	if ( where == 2 && canvas->GetLogx() ) {
		x1 = TMath::Power(10, x1);
		x2 = TMath::Power(10, x2);
	}
	if ( where == 1 &&  canvas->GetLogy() ) {
		y1 = TMath::Power(10, y1);
		y2 = TMath::Power(10, y2);
	}
	
	Int_t nd   ;
	Size_t ls  ;
	Color_t lc ;
	Font_t  lf ;
	Double_t lo;
	Double_t tl;
	if( orig_axis ) {
		// a TAxis
		nd = orig_axis->GetNdivisions();
		ls = orig_axis->GetLabelSize();
		lc = orig_axis->GetLabelColor();
		lf = orig_axis->GetLabelFont();
		lo = orig_axis->GetLabelOffset();
		tl = orig_axis->GetTickLength();
	}
	if ( col > 0 ) {
		lc = col;
	}	
	
	if (gDebug > 0)
	 cout   << "DoAddAxis: x1"
			  << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2 << " ledge " 
			  << ledge << " uedge " << uedge << " ratio " << ratio << " offset " << offset
			  << " axis_offset " << axis_offset << endl;
	
	naxis = new HprGaxis(canvas, x1, y1, x2, y2, ledge, uedge, nd, side.Data());
	naxis->SetWhere(where);
	naxis->SetOffset(offset);
	naxis->SetAxisOffset(axis_offset);
	naxis->SetRatio(ratio);
	
	naxis->SetLabelSize(ls);
	naxis->SetLineColor(lc);
	naxis->SetLabelColor(lc);
	naxis->SetLabelFont(lf);
	naxis->SetLabelOffset(lo);
	naxis->SetTickSize(tl);
	naxis->Draw();
	canvas->Modified(kTRUE);
	canvas->Update();

	return naxis;
};
//____________________________________________________________________________________

void SuperImpose(TCanvas * canvas, TH1 * selhist, Int_t mode)
{
	static const char helptext[] =
	"\n\
	A selected hist is drawn in the same pad\n\
	The histogram may be scaled automatically: \n\
	The scale is adjusted such that the maximum value\n\
	of the superimposed histogram is equal to the maximum\n\
	of the original histogram in the displayed range.\n\
	So a certain peak can used as reference by zooming in\n\
	on this peak. Any value may be selected manually.\n\
	\n\
	An extra axis may be drawn on the right side:\n\
	The AxOffs determines its position, 0 is on top\n\
	of the right edge of the frame, negative is left\n\
	of it in the pad, positive at the right side\n\
	This is useful if more than one histogram is superimposed\n\
	The color of the extra axis equals the color of the hist.\n\
	With option \"Incr Colors\" Line-, Mark- FillColors are\n\
	automatically incremented (red->green->blue etc.)\n\
	With \"Skip Dialog\" this widget is only shown once\n\
	for the first histogram\n\
		\n\
		";
//	HprLegend * fLegend = (HprLegend*)canvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
	HprLegend * fLegend = (HprLegend*)FindLegendInPad(canvas);
	TH1 *hist;
	//   TPaveLabel *tname;
	//  choose from histo list
	//   Int_t nh = gHpr->GetSelectedHist()->GetSize();
	if (gHpr->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
		if (gHpr->GetSelectedHist()->GetSize() > 1) {
			Hpr::WarnBox("More than 1 selection,\n please choose only one");
			return;
		}
		hist = gHpr->GetSelHistAt(0);
	} else {
		hist = Hpr::GetOneHist(selhist);      // look in memory
	}
	if ( !hist ) {
//		Hpr::WarnBox("No hist selected");
		cout << setred << "No histogram selected" << setblack << endl;
		return;
	}
	hist->SetMinimum(-1111);
	hist->SetMaximum(-1111);
	
	//    cout << "hist->GetName() " << hist->GetName() << endl;
	TEnv env(".hprrc");
	static Int_t   lLegend      = env.GetValue("SuperImposeHist.DrawLegend", 1);
	static Int_t   lIncrColors  = env.GetValue("SuperImposeHist.AutoIncrColors", 0);
	static Int_t   lSkipDialog  = env.GetValue("SuperImposeHist.SkipDialog", 0);
	static Int_t   lWarnDiffBin = env.GetValue("SuperImposeHist.WarnDiffBin", 1);
	//	static Int_t   lNoStatBox   = env.GetValue("SuperImposeHist.NoStatBox", 1);
	
	if (hist->GetDimension() != selhist->GetDimension()) {
		Hpr::WarnBox("Dimensions of histograms differ");
		return;
	}
	//	Double_t rightmax = 1.1*ymax;
	//	canvas->cd();
	TRootCanvas * win = (TRootCanvas*)canvas->GetCanvasImp();
	TIter next(canvas->GetListOfPrimitives());
	Int_t nhists = 0 ;
	TObject *obj;
	TH1* horig = NULL;
	while ( obj  = next() ) {
		if ( obj->InheritsFrom("TH1") ) {
			horig = (TH1*)obj;
			TString oname(obj->GetName());
			TString hname(hist->GetName());
			//			if ( obj == selhist ) {
		//				oname = fHname;
		//			}
		//			cout << "oname, hname " << oname << " " << hname << endl;
		//			if ( oname == hname ) {
			//				if ( !QuestionBox("Hist already in canvas, really draw again?",win) )
			//					return;
			//			}
			if ( lWarnDiffBin && !Hpr::HistLimitsMatch(horig, hist) ) {
				if ( !QuestionBox("Hist limits or bins differ, really superimpose?",win) ) {
					return;
				} else {
					hist->GetXaxis()->Set(horig->GetXaxis()->GetNbins(),
												 horig->GetXaxis()->GetXmin(), horig->GetXaxis()->GetXmax());
												 if (horig->GetDimension() == 2) {
													 hist->GetYaxis()->Set(horig->GetYaxis()->GetNbins(),
																				  horig->GetYaxis()->GetXmin(), horig->GetYaxis()->GetXmax());
												 }
				}
			}
			nhists++;
		}
	}
	if ( !horig ) 
		horig = selhist;
	Int_t    do_scale;
	do_scale = mode;
	Int_t    auto_scale;
	auto_scale = mode;
	static Double_t axis_offset;
	static Double_t label_offset = 0.01;
	static Color_t  axis_color;
	static Color_t lLColor   = axis_color; 
	static Color_t lMColor   = axis_color;
	static Color_t lFillColor= axis_color; 
	if ( nhists < 2 ) {
		axis_color = 2;
		lLColor   = axis_color;
		lMColor   = axis_color;
		lFillColor  = axis_color;
		axis_offset = 0.;
	}
	static Style_t lFStyle;
	static Style_t lLStyle;
	static Width_t lLWidth;
	static Style_t lMStyle;
	static Size_t  lMSize;
	static Double_t lStatBoxDX1;
	static Double_t lStatBoxDX2;
	static Double_t lStatBoxDY1;
	static Double_t lStatBoxDY2;
	// 	static Int_t   dummy;
	lFStyle     = horig->GetFillStyle();
	lLStyle     = horig->GetLineStyle();
	lLWidth     = horig->GetLineWidth();
	lMStyle     = horig->GetMarkerStyle();
	lMSize      = horig->GetMarkerSize();
	TPaveStats *sbo = (TPaveStats*)horig->GetListOfFunctions()->FindObject("stats");
	if ( sbo ) {
		lStatBoxDX1 = sbo->GetX1NDC();
		lStatBoxDX2 = sbo->GetX2NDC();
		lStatBoxDY1 = sbo->GetY1NDC();
		lStatBoxDY2 = sbo->GetY2NDC();
	}
	if (hist->GetDimension() == 1 ) {
		if ( !sbo ) {
			lStatBoxDX1 = env.GetValue("Set1DimOptDialog.StatBox1D.fX1", 0.8);
			lStatBoxDX2 = env.GetValue("Set1DimOptDialog.StatBox1D.fX2", 0.9);
			lStatBoxDY1 = env.GetValue("Set1DimOptDialog.StatBox1D.fY1", 0.8);
			lStatBoxDY2 = env.GetValue("Set1DimOptDialog.StatBox1D.fY2", 0.9);
		}
	} else {
		
		if ( !sbo ) {
			lStatBoxDX1 = env.GetValue("Set2DimOptDialog.StatBox2D.fX1", 0.8);
			lStatBoxDX2 = env.GetValue("Set2DimOptDialog.StatBox2D.fX2", 0.9);
			lStatBoxDY1 = env.GetValue("Set2DimOptDialog.StatBox2D.fY1", 0.8);
			lStatBoxDY2 = env.GetValue("Set2DimOptDialog.StatBox2D.fY2", 0.9);
		}
	}
	
	if ( lMSize <= 0 ) lMSize = 1;
	Double_t new_scale = 1;   
	static TString axis_title;
	axis_title= hist->GetYaxis()->GetTitle();
	static Int_t new_axis = kTRUE;
	canvas->cd();
	TString drawopt = horig->GetDrawOption();
	if ( gDebug  > 0 )
		cout << "horig->GetDrawOption() " << hist << " drawopt  " << drawopt<< endl;
	if ( lSkipDialog == 0 || nhists < 2 ) {
		// Error modes 
		static void *valp[50];                    
		Int_t ind = 0;                            
		TList *row_lab = new TList();
		Bool_t ok = kTRUE;
		row_lab->Add(new TObjString("CheckButton_New scale  "));
		valp[ind++] = &do_scale;
		row_lab->Add(new TObjString("CheckButton+Auto scale "));
		valp[ind++] = &auto_scale;
		row_lab->Add(new TObjString("DoubleValue+Factor"));
		valp[ind++] = &new_scale;
		row_lab->Add(new TObjString("CheckButton_Extra axis "));
		valp[ind++] = &new_axis;
		row_lab->Add(new TObjString("DoubleValue+AxOffs;-1;1 "));
		valp[ind++] = &axis_offset;
		row_lab->Add(new TObjString("DoubleValue+LabOffs"));
		valp[ind++] = &label_offset;
		row_lab->Add(new TObjString("ColorSelect_AxisColor"));
		valp[ind++] = &axis_color;
		row_lab->Add(new TObjString("StringValue+AxTitle"));
		valp[ind++] = &axis_title;
		row_lab->Add(new TObjString("CheckButton+Legend  "));
		valp[ind++] = &lLegend;
		row_lab->Add(new TObjString("ColorSelect_MarkColor"));
		valp[ind++] = &lMColor;
		row_lab->Add(new TObjString("Mark_Select+MarkStyle"));
		valp[ind++] = &lMStyle;
		row_lab->Add(new TObjString("Float_Value+MSize"));
		valp[ind++] = &lMSize;
		row_lab->Add(new TObjString("ColorSelect_LineColor"));
		valp[ind++] = &lLColor;
		row_lab->Add(new TObjString("LineSSelect+LStyle"));
		valp[ind++] = &lLStyle;
		row_lab->Add(new TObjString("PlainShtVal+LineWidth"));
		valp[ind++] = &lLWidth;
		row_lab->Add(new TObjString("ColorSelect_FillColor"));
		valp[ind++] = &lFillColor;
		row_lab->Add(new TObjString("Fill_Select+FillStyle"));
		valp[ind++] = &lFStyle;
		// 		row_lab->Add(new TObjString("CommentOnly+-"));
		// 		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("StringValue+DrawOpt"));
		valp[ind++] = &drawopt;
		row_lab->Add(new TObjString("CheckButton_Incr Colors "));
		valp[ind++] = &lIncrColors;
		row_lab->Add(new TObjString("CheckButton+Skip Dialog"));
		valp[ind++] = &lSkipDialog;
		row_lab->Add(new TObjString("CheckButton+WarnDiffBin"));
		valp[ind++] = &lWarnDiffBin;
		//		row_lab->Add(new TObjString("CheckButton+NoStatBox"));
		//		valp[ind++] = &lNoStatBox;
		
		Int_t itemwidth = 380;
		ok = GetStringExt("Superimpose Histogram", NULL, itemwidth, win,
								NULL, NULL, row_lab, valp,
								NULL, NULL, helptext);
								if (!ok )
									return;
	}
	if ( do_scale != 0 && auto_scale != 0 ) {
		//		new_scale = hymax / rightmax;
		cout << "Scale will be auto adjusted " << endl;
	}
	
	//	TGaxis *naxis = 0;
	TH1 *hdisp = (TH1 *) hist->Clone();
	if (do_scale && selhist->GetDimension() != 2)  {
		//		cout << "!!!!!!!!!!!!!!!!" << endl;
		if ( auto_scale ) {
			Stat_t maxy = 0;
			if (selhist->GetXaxis()->GetNbins() != hist->GetXaxis()->GetNbins()) {
				//  case expanded histogram
				Axis_t x, xmin, xmax;
				xmin = selhist->GetXaxis()->GetXmin();
				xmax = selhist->GetXaxis()->GetXmax();
				for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++) {
					x = hist->GetBinCenter(i);
					if (x >= xmin && x < xmax && hist->GetBinContent(i) > maxy)
						maxy = hist->GetBinContent(i);
				}
			} else {
				//  case zoomed histogram
				for (Int_t i = selhist->GetXaxis()->GetFirst();
				i <= selhist->GetXaxis()->GetLast();
				i++) {
					if (hist->GetBinContent(i) > maxy)
						maxy = hist->GetBinContent(i);
				}
				
			}
			if ( maxy == 0 ) {
				cout << "Max = 0 " << endl;
				return;
			}
			new_scale = selhist->GetMaximum() / maxy;
		} else {
			//			new_scale = 1;
		}
		TString name = hdisp->GetName();
		name += "_scaled";
		hdisp->SetName(name.Data());
		cout << "Scale " << hdisp->GetName() << " by " << new_scale << endl;
		hdisp->Scale(new_scale);
		for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
			hdisp->SetBinError(i, new_scale * hist->GetBinError(i));
		}
		cout << "Superimpose: Scale errors linearly" << endl;
	}
	canvas->cd();
	hdisp->SetLineColor(lLColor);
	hdisp->SetLineStyle(lLStyle);
	hdisp->SetFillStyle(lFStyle);
	hdisp->SetLineColor(lLColor);
	if ( hist->GetDimension() == 2 ) 
		hdisp->SetFillColor(lFillColor);
	hdisp->SetLineWidth(lLWidth);
	hdisp->SetMarkerColor(lMColor);
	hdisp->SetMarkerStyle(lMStyle);
	hdisp->SetMarkerSize(lMSize);
	if ( hist->GetDimension() == 1 ) {
		if (!drawopt.Contains("E", TString::kIgnoreCase)) {
			if (!drawopt.Contains("hist",TString::kIgnoreCase)) {
				drawopt += "hist";
			}
		}
		if ( env.GetValue("Set1DimOptDialog.fFill1Dim", 0 ) > 0 ){ 
			hdisp->SetFillColor(lFillColor);
			drawopt += "F";
			axis_color = lFillColor;
		} else {
			hdisp->SetFillColor(0);
			if (drawopt.Contains("hist",TString::kIgnoreCase)) {
				axis_color = lLColor;
			} else {
				axis_color = lMColor;
			}
		}
	}
	if (hdisp->GetDimension() == 2) {
		TRegexp zopt("Z");
		drawopt(zopt)="";
	}
	if ( !drawopt.Contains("SAME") )
		drawopt += "SAME";
	if (gDebug > 0) 
		cout << "DrawCopy(drawopt) " << drawopt << endl;
	TH1 * hdrawn = hdisp->DrawCopy(drawopt.Data());
	TPaveStats *sb = (TPaveStats*)hdrawn->GetListOfFunctions()->FindObject("stats");
	if ( sb ) {
		cout << "SetStatBox: "  
		<< " lStatBoxDX1 " << lStatBoxDX1<< 
		" lStatBoxDX2 " << lStatBoxDX2<< 
		" lStatBoxDY1 " << lStatBoxDY1<< 
		" lStatBoxDY2 " << lStatBoxDY2<< 
		endl;
		sb->SetX1NDC(lStatBoxDX1);
		sb->SetX2NDC(lStatBoxDX2);
		sb->SetY1NDC(lStatBoxDY1 - nhists*(lStatBoxDY2 - lStatBoxDY1) );
		sb->SetY2NDC(lStatBoxDY2 - nhists*(lStatBoxDY2 - lStatBoxDY1));
	}
	if ( new_axis != 0 && hist->GetDimension() == 1 ) {
		//		TString opt("+SL");->GetFrame()->GetX1()
		Double_t ledge = gPad->GetFrame()->GetY1();
		Double_t uedge = gPad->GetFrame()->GetY2();
		if ( gPad->GetLogy() ) {
			ledge = TMath::Power(10, ledge);
			uedge = TMath::Power(10, uedge);
		}
		ledge /= new_scale;
		uedge /= new_scale;
		HprGaxis *axis = Hpr::DoAddAxis(canvas, hdisp, 2, ledge, uedge, axis_offset, axis_color);
		
		TString ax_name("axis_");
		ax_name += hdisp->GetTitle();
		axis->SetName(ax_name);
		axis->SetLineColor(axis_color);
		axis->SetLabelColor(axis_color);
		axis->SetTickSize   (env.GetValue("SetHistOptDialog.fTickLength", 0.01));
		axis->SetLabelFont  (env.GetValue("SetHistOptDialog.fLabelFont", 62));
		axis->SetLabelOffset(label_offset);
		axis->SetLabelOffset(env.GetValue("SetHistOptDialog.fLabelOffsetY", 0.01));
		axis->SetLabelSize  (env.GetValue("SetHistOptDialog.fLabelSize", 0.03));
		axis->SetMaxDigits  (env.GetValue("SetHistOptDialog.fLabelMaxDigits", 4));
		if (axis_title.Length() > 0) {
			axis->SetTitle(axis_title);
			axis->SetTitleColor(axis_color);
			axis->SetTitleFont( env.GetValue("SetHistOptDialog.fTitleFont", 62));
			axis->SetTitleSize( env.GetValue("SetHistOptDialog.fTitleSize",0.03));
			axis->SetTitleOffset( 1. );
			if ( env.GetValue("SetHistOptDialog.fTitleCenterY", 0) == 1 ) 
				axis->CenterTitle();
		}
	}
	
	if ( lLegend != 0 ) {
//		TEnv env(".hprrc");
		Double_t x1 = env.GetValue("SuperImposeHist.fLegendX1", 0.11);
		Double_t x2 = env.GetValue("SuperImposeHist.fLegendX2", 0.3);
		Double_t y1 = env.GetValue("SuperImposeHist.fLegendY1", 0.8);
		Double_t y2 = env.GetValue("SuperImposeHist.fLegendY2", 0.95);
		TString opt;
		TString dopt;
		if ( fLegend == NULL ) {
			fLegend = new HprLegend(x1, y1, x2, y2, "", "brNDC");
			fLegend->SetName("Legend_SuperImposeHist");
			dopt = selhist->GetDrawOption();
			if (selhist->GetDimension() == 2 ) {
				if ( dopt.Contains("SCAT", TString::kIgnoreCase) ) opt = "P";
				if ( dopt.Contains("BOX", TString::kIgnoreCase) && 
					!dopt.Contains("BOX1", TString::kIgnoreCase) ) opt += "L";
				if ( dopt.Contains("BOX", TString::kIgnoreCase) &&
					selhist->GetFillColor() != 0 && selhist->GetFillStyle() != 0 || 
					dopt.Contains("BOX1", TString::kIgnoreCase) ) opt+= "F";
			} else {
				if (dopt.Length() == 0 || dopt.Contains("HIST",TString::kIgnoreCase) ||
					dopt.Contains("E", TString::kIgnoreCase) )
					opt = "L";
				if (selhist->GetFillStyle() != 0 )
					opt+= "F";
				if (dopt.Contains("P",TString::kIgnoreCase) )
					opt += "P";
			}
			fLegend->AddEntry(selhist, "", opt);
			//			cout << "fLegend->AddEntry: " << opt << endl;
			fLegend->Draw();
		}
		opt = "";
		dopt = hdrawn->GetDrawOption();
		if (selhist->GetDimension() == 2 ) {
			if ( dopt.Contains("SCAT", TString::kIgnoreCase) ) opt = "P";
			if ( dopt.Contains("BOX", TString::kIgnoreCase) && 
				!dopt.Contains("BOX1", TString::kIgnoreCase) ) opt += "L";
			if ( (dopt.Contains("BOX", TString::kIgnoreCase) &&
				lFillColor != 0 && lFStyle != 0) || 
				dopt.Contains("BOX1", TString::kIgnoreCase)) opt+= "F";
		} else {
			if (dopt.Length() == 0 || dopt.Contains("HIST",TString::kIgnoreCase) ||
				dopt.Contains("E", TString::kIgnoreCase) )
				opt = "L";
			if (hdrawn->GetFillStyle() != 0 )
				opt+= "F";
			if (dopt.Contains("P",TString::kIgnoreCase) )
				opt += "P";
		}
		//		cout << "fLegend->AddEntry: " << opt << endl;
		fLegend->AddEntry(hdrawn, "", opt);
	}
	canvas->Update();
	if ( hdrawn ) {
		TRegexp sa("SAME");
		TString dro(hdrawn->GetDrawOption());
		dro(sa)="";
		//		hdrawn->SetDrawOption(dro);
		//		THistPainter *hp = (THistPainter*)hdrawn->GetPainter();
		//		hdrawn->Pop();
		canvas->Modified();
		canvas->Update();
	}
	if (lIncrColors > 0) {
		lLColor++;
		lMColor++;
		axis_color++;
		lFillColor++;
		axis_offset += 0.05;
	}
	env.SetValue("SuperImposeHist.DrawLegend", lLegend);
	env.SetValue("SuperImposeHist.AutoIncrColors", lIncrColors);
	env.SetValue("SuperImposeHist.SkipDialog", lSkipDialog);
	env.SetValue("SuperImposeHist.WarnDiffBin", lWarnDiffBin);
	env.SaveLevel(kEnvLocal);
}
//____________________________________________________________________________________

TH1 * GetOneHist(TH1 * selhist)
{
	Int_t nhists = 0;
	TOrdCollection *entries = new TOrdCollection();
	TH1 * hist = NULL;
	TList *tl = gDirectory->GetList();
	TIter next(tl);
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) {
			hist = (TH1 *) obj;
			if (hist != selhist) {
				entries->Add(new TObjString(hist->GetName()));
				nhists++;
			}
		}
	}
	if (nhists <= 0) {
		Hpr::WarnBox("No Histogram found");
		return 0;
	}
	
	TArrayI flags(nhists);
	flags.Reset();
	Int_t retval = 0;
	
	Int_t itemwidth = 240;
	new TGMrbTableFrame(NULL, &retval, "Select a histogram", itemwidth, 1, nhists,
		entries, 0, 0, &flags, nhists);
	if (retval < 0){
		  return NULL;
	}
	TObjString * objs;
	TString s;
	for (Int_t i = 0; i < nhists; i++) {
		  //      Char_t sel = selected[i];
		  if (flags[i] == 1) {
			  objs = (TObjString *) entries->At(i);
			  s = objs->String();
			  hist = (TH1 *) gROOT->FindObject((const char *) s);
			  break;
		  }
	}
	delete entries;
	if (!hist) {
		  Hpr::WarnBox("No Histogram found");
	}
	if (hist == selhist) {
		  Hpr::WarnBox("Cant use same histogram");
		  hist = NULL;
	}
	return hist;
}
//__________________________________________________________________

void ResizeStatBox(TPaveStats * st, Int_t ndim) 
{
	TEnv env(".hprrc");
	TString envname("SetHistOptDialog.Stat");
	TString sdim;
	if (ndim == 2)
		sdim = "2D";
	TString resname;
	resname = envname + "X"  + sdim;
	Float_t statX = env.GetValue(resname, 0.9);
	st->SetX2NDC(statX);
	resname = envname + "W"  + sdim;
	Float_t statW  = env.GetValue(resname, 0.2);
	st->SetX1NDC(statX - statW);
	resname = envname + "Y" + sdim;
	Float_t statY =  env.GetValue(resname, 9);
	st->SetY2NDC(statY);
	resname = envname + "H" + sdim;
	Float_t statH  = env.GetValue(resname, 0.16);
	st->SetY1NDC(statY - statH);
	if (gDebug > 0) {
		cout << " ResizeStatBox statY"  << resname << " " << statY << endl;
	}	
}
//__________________________________________________________________

Bool_t IsSelected(const char * name, TString * mask, Int_t use_regexp)
{
	enum e_HsOp {kHsOp_None, kHsOp_And, kHsOp_Or, kHsOp_Not};
	Int_t SelOp;
	Bool_t wildcard = kTRUE;
	if ( use_regexp > 0)
		wildcard = kFALSE;
	if ( mask->Length() <= 0 )
		return kTRUE;
	TString mask_1;
	TString mask_2;
	if      (mask->Contains("&")) {
		SelOp = kHsOp_And;
		Int_t indop = mask->Index("&");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else if (mask->Contains("|")) {
		SelOp = kHsOp_Or;
		Int_t indop = mask->Index("|");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else if (mask->Contains("!")) {
		SelOp = kHsOp_Not;
		Int_t indop = mask->Index("!");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else  {
		SelOp = kHsOp_None;
	}
	mask_1 = mask_1.Strip(TString::kBoth);
	mask_2 = mask_2.Strip(TString::kBoth);
	TString sname(name);
	if        (SelOp == kHsOp_None) {
		TRegexp re((const char *)*mask, wildcard);
		if (sname.Index(re) <0) return kFALSE;
		
	} else if ( wildcard == 0 ) {
		TRegexp re1((const char *)mask_1);
		TRegexp re2((const char *)mask_2);
		if (SelOp == kHsOp_And) {
			if (sname.Index(re1) < 0 ||
				sname.Index(re2) < 0)  return kFALSE;
		} else if (SelOp == kHsOp_Or) {
			if (sname.Index(re1) < 0 &&
				sname.Index(re2) < 0)  return kFALSE;
		} else if (SelOp == kHsOp_Not) {
			if (( mask_1.Length() > 0 &&
				sname.Index(re1) < 0) ||
				(sname.Index(re2) >=0))  return kFALSE;
		}
	} else {
		if (SelOp == kHsOp_And) {
			if (!sname.Contains(mask_1.Data()) ||
				!sname.Contains(mask_2.Data()))  return kFALSE;
		} else if (SelOp == kHsOp_Or) {
			if (!sname.Contains(mask_1.Data()) &&
				!sname.Contains(mask_2.Data()))  return kFALSE;
		} else if (SelOp == kHsOp_Not) {
			if (( mask_1.Length() > 0 &&
				!sname.Contains(mask_1.Data())) ||
				(sname.Contains(mask_2.Data())))  return kFALSE;
		}
	}
	return kTRUE;
}

}   // end namespace Hpr
