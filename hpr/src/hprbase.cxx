#include "TFile.h"
#include "TGraphErrors.h"
#include "TFrame.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TList.h"
#include "TMath.h"
#include "TGMsgBox.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
#include <fstream>
#include "hprbase.h"

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
   static Int_t suppress_zeros = 1;
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

TH1 * FindHistInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) {
			return (TH1*)obj;
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
	
// 	naxis = new HprGaxis(x1, y1, x2, y2, ledge, uedge, 510, side.Data());
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
	TQObject::Connect("TPad", "Modified()",
						   "HprGaxis", naxis, "HandlePadModified()");

	return naxis;
};

}   // end namespace Hpr
