#include "TROOT.h"
#include "TEnv.h"
#include "TPad.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TString.h"
#include "TGWindow.h"
#ifdef MARABOUVERS
#include "HTCanvas.h"
#else
#include "TCanvas.h"
#endif
#include "GraphAttDialog.h"
#include "SetHistOptDialog.h"
#include "Ascii2GraphDialog.h"
#include "FitOneDimDialog.h"
#include "TGMrbValuesAndText.h"
//#include "support.h"
#include <fstream>
//____________________________________________________________________________

void ExecGausFitG(TGraph * graph, Int_t type)
{
   if (!gPad || !graph) return;
   graph->SetBit(kMustCleanup);
   new FitOneDimDialog(graph, type);
}
//______________________________________________________________________________________
//_______________________________________________________________________________________

ClassImp(Ascii2GraphDialog)

Ascii2GraphDialog::Ascii2GraphDialog(TGWindow * win, Int_t winx,  Int_t winy)
              : fWinx(winx), fWiny(winy)
{

static const Char_t helpText[] =
"Read values from ASCII file and construct a graph\n\
Input data can have the formats:\n\
X, Y:                     simple graph, no errors\n\
X, Y, Ex, Ey:             symmetric errors in X and Y\n\
Note: if only 3 values are given the 3rd is assumed to be Ey\n\
X, Y, Exl, Exu, Eyl, Eyu: asymmetric errors in X and Y\n\
   Values after Y may be ommitted, they are set to 0\n\
\n\
Select columns:\n\
   Select 2 columns to be used as X, Y of a simple graph\n\
   White space or comma are used as separators\n\
   Unused columns may contain any characters \n\
   Columns are counted from 1\n\
The graph can be drawn / overlayed in a selected pad.\n\
Default is to construct a new canvas\n\
";

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fGraphSerialNr = 0;
   fCommand = "Draw_The_Graph()";
   fCommandHead = "Show_Head_of_File()";
   fCommandTail = "Show_Tail_of_File()";
   RestoreDefaults();
   fGraphSelPad = 0;    // start with new canvas as default
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("RadioButton_Empty pad only               "));
   row_lab->Add(new TObjString("RadioButton_Simple: X, Y no errors       "));
   row_lab->Add(new TObjString("RadioButton_Sym Errors: X, Y, (Ex), Ey   "));
   row_lab->Add(new TObjString("RadioButton_Asym Ers: X,Y,Exl,Exu,Eyl,Eyu"));
   row_lab->Add(new TObjString("RadioButton_Select columns, X, Y         "));
   row_lab->Add(new TObjString("PlainIntVal_Column Sel"));
   row_lab->Add(new TObjString("PlainIntVal+Column Sel"));

   row_lab->Add(new TObjString("FileRequest_Inputfile"));
   row_lab->Add(new TObjString("StringValue_GraphName"));
   row_lab->Add(new TObjString("StringValue_Title X  "));
   row_lab->Add(new TObjString("StringValue+Title Y  "));
   row_lab->Add(new TObjString("DoubleValue_Xaxis min"));
   row_lab->Add(new TObjString("DoubleValue+Xaxis max"));
   row_lab->Add(new TObjString("DoubleValue_Yaxis min"));
   row_lab->Add(new TObjString("DoubleValue+Yaxis max"));

   row_lab->Add(new TObjString("CheckButton_Draw/Overlay in a sel pad"));
//   row_lab->Add(new TObjString("CheckButton_Draw in a new canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Xsize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal-Ysize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div X of canvas"));
   row_lab->Add(new TObjString("PlainIntVal+Div Y of canvas"));
   row_lab->Add(new TObjString("CheckButton_Marker"));
   row_lab->Add(new TObjString("CheckButton+Simple line"));
	row_lab->Add(new TObjString("CheckButton+Smooth line"));
	row_lab->Add(new TObjString("Mark_Select_MaStyle"));
   row_lab->Add(new TObjString("ColorSelect+MaColor"));
   row_lab->Add(new TObjString("Float_Value+MaSize"));
   row_lab->Add(new TObjString("ColorSelect_LineC"));
   row_lab->Add(new TObjString("PlainShtVal+Width"));
   row_lab->Add(new TObjString("LineSSelect+Style"));
	row_lab->Add(new TObjString("CheckButton_Fill area"));
   row_lab->Add(new TObjString("Fill_Select+Fill Style"));
   row_lab->Add(new TObjString("ColorSelect+Fill color"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt+Show_Tail_of_File"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Graph"));
//   row_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = row_lab->GetSize();

   valp[ind++] = &fEmptyPad;
   valp[ind++] = &fGraph_Simple;
   valp[ind++] = &fGraph_Error;
   valp[ind++] = &fGraph_AsymError;
   valp[ind++] = &fGraphColSelect;
   valp[ind++] = &fGraphColSel1;
   valp[ind++] = &fGraphColSel2;
   valp[ind++] = &fGraphFileName;
   valp[ind++] = &fGraphName;
   valp[ind++] = &fGraphXtitle;
   valp[ind++] = &fGraphYtitle;
   valp[ind++] = &fXaxisMin;
   valp[ind++] = &fXaxisMax;
   valp[ind++] = &fYaxisMin;
   valp[ind++] = &fYaxisMax;
   valp[ind++] = &fGraphSelPad;
//   valp[ind++] = &fGraphNewPad;
   valp[ind++] = &fGraphXsize;
   valp[ind++] = &fGraphYsize;
   valp[ind++] = &fGraphXdiv;
   valp[ind++] = &fGraphYdiv;
   valp[ind++] = &fGraphPolyMarker;
   valp[ind++] = &fGraphSimpleLine;
	valp[ind++] = &fGraphSmoothLine;
	valp[ind++] = &fGraphMarkerStyle;
   valp[ind++] = &fGraphMarkerColor;
   valp[ind++] = &fGraphMarkerSize;
   valp[ind++] = &fGraphLineColor;
   valp[ind++] = &fGraphLineWidth;
   valp[ind++] = &fGraphLineStyle;
   valp[ind++] = &fGraphFill;
	valp[ind++] = &fGraphFillStyle;
	valp[ind++] = &fGraphFillColor;
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fCommandTail;
   valp[ind++] = &fCommand;
   Int_t itemwidth = 360;
   ok = GetStringExt("Graphs parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};
//_________________________________________________________________________

Ascii2GraphDialog::~Ascii2GraphDialog()
{
   SaveDefaults();
};
//_________________________________________________________________________

void Ascii2GraphDialog::Draw_The_Graph()
{
   if (fEmptyPad != 0) {
      cout << "Empty pad only" << endl;
      TGraph *graph = new TGraph();
#ifdef MARABOUVERS
      HTCanvas * cg = new HTCanvas("Empty", "Empty", fWinx, fWiny,
                      fGraphXsize, fGraphYsize, NULL, NULL, graph);
//                         fGraphXsize, fGraphYsize, fHistPresent, 0, graph);
#else
       TCanvas * cg = new TCanvas("Empty", "Empty", fWinx, fWiny,
                     fGraphXsize, fGraphYsize);
#endif
      if (fGraphXdiv > 1 || fGraphYdiv > 1) {
         cg->Divide(fGraphXdiv, fGraphYdiv);
         cg->cd(1);
      }
      Double_t xmin = 0, xmax = 100;
      Double_t ymin = 0, ymax = 100;
      if (fXaxisMin != 0 || fXaxisMax != 0) {
         xmin = fXaxisMin;
         xmax = fXaxisMax;
      }
      if (fYaxisMin != 0 || fYaxisMax != 0) {
         ymin = fYaxisMin;
         ymax = fYaxisMax;
      }
      gStyle->SetOptStat(0);
      TH1D * gh = new TH1D(fGraphName, fGraphName, 100, xmin, xmax);
      gh->Draw();
      gh->SetMinimum(ymin);
      gh->SetMaximum(ymax);
      if (fGraphXtitle.Length() > 0)
         gh->GetXaxis()->SetTitle(fGraphXtitle.Data());
      if (fGraphYtitle.Length() > 0)
         gh->GetYaxis()->SetTitle(fGraphYtitle.Data());
      graph->SetHistogram(gh);
      gPad->Update();
      return;
   }

   TArrayD xval(100), yval(100), zval(100), wval(100), eyl(100), eyh(100);
   ifstream infile;
   infile.open(fGraphFileName.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "Ascii2GraphDialog: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return;
	}
   Int_t n = 0;
   Double_t x[6];

   TString line;
   TString del(" ,\t");
   TObjArray * oa;
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
		oa = line.Tokenize(del);
		Int_t nent = oa->GetEntries();
      if (nent < 2) {
         cout << "Not enough entries at: " << n+1 << endl;
         break;
      }
      for ( Int_t i = 0; i < 6; i++ )
         x[i] = 0;
		for (Int_t i = 0; i < nent; i++) {
			TString val = ((TObjString*)oa->At(i))->String();
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << n+1 << endl;
			   break;
         }
			x[i] = val.Atof();
		}
      if (fGraphColSelect) {
        if ( fGraphColSel1 > nent || fGraphColSel2 > nent) {
            cout << "Not enough entries at: " << n << endl;
            break;
         }
         xval.AddAt(x[fGraphColSel1-1], n);
         yval.AddAt(x[fGraphColSel2-1], n);
         n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	yval.Set(n+100);
         }
      } else {
         xval.AddAt(x[0], n);
         yval.AddAt(x[1], n);
//       if only 3 values  assume x, y, ye
         if (nent == 3) {
            x[3] = x[2];
            x[2] = 0;
         }
         if (fGraph_Error == 1 || fGraph_AsymError == 1) {
            zval.AddAt(x[2], n);
            wval.AddAt(x[3], n);
            if ( fGraph_AsymError == 1 ) {
               eyl.AddAt(x[4], n);
               eyh.AddAt(x[5], n);
            }
         }
      	n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	yval.Set(n+100);
         	zval.Set(n+100);
         	wval.Set(n+100);
         	eyl.Set(n+100);
         	eyh.Set(n+100);
      	}
   	}
   }
   infile.close();

   cout << "entries " << n << endl;
   if (n < 1) return;


   TString hname = fGraphFileName;
   hname = gSystem->BaseName(hname);
   Int_t ip = hname.Index(".");
   if (ip > 0) hname.Resize(ip);
   TString htitle = hname;
   htitle.Prepend("Values from ");
   hname.Prepend("g_");

//   TMrbString temp;

//  TGraph part

   TGraph * graph = 0;
   if (fGraph_Simple == 1 || fGraphColSelect){
      graph = new TGraph(n, xval.GetArray(), yval.GetArray());
   } else if (fGraph_Error == 1) {
      graph = new TGraphErrors(n, xval.GetArray(), yval.GetArray(),
                                  zval.GetArray(), wval.GetArray());
   } else if (fGraph_AsymError == 1) {
      graph = new TGraphAsymmErrors(n, xval.GetArray(), yval.GetArray(),
                                       zval.GetArray(), wval.GetArray(),
                                       eyl.GetArray(),  eyh.GetArray());
   }
   if (graph) {
      if (fGraphName.Length() <= 0) {
         graph->SetName(fGraphFileName.Data());
         graph->SetTitle(fGraphFileName.Data());
      } else  {
         graph->SetName(fGraphName.Data());
         graph->SetTitle(fGraphName.Data());
      }

//      graph->SetTitle(graph->GetName());
      TString drawopt("A");           // draw axis as default
      if (fGraphPolyMarker) drawopt+= "P";
      if (fGraphSimpleLine) drawopt+= "L";
		if (fGraphSmoothLine) drawopt+= "C";
		if (fGraphBarChart)   drawopt+= "B";
		if (fGraphFill)       drawopt+= "F";
//         cout << "gPad->GetName() " <<gPad->GetName() << endl;
      if (fGraphSelPad) {
         Int_t ngr = FindGraphs(gPad, NULL, NULL);
         if (ngr > 0) {
            TString oo(drawopt);
            Int_t inda = drawopt.Index("a", 0, TString::kIgnoreCase);
            if (inda>=0) drawopt.Remove(inda,1);
//            cout << "oo: " << oo<< endl;

            drawopt += "SAME";
            graph->Draw(drawopt);
            if (fGraphXtitle.Length() > 0)
               graph->GetHistogram()->GetXaxis()->SetTitle(fGraphXtitle.Data());
            if (fGraphYtitle.Length() > 0)
               graph->GetHistogram()->GetYaxis()->SetTitle(fGraphYtitle.Data());

         } else {
            graph->Draw(drawopt);
         }
//            gPad->Modified();
//            gPad->Update();
      } else {
//            cout << "New graph: " << endl;
         TString cname = hname;
         cname.Prepend("C_");
         TObject * cc = gROOT->GetListOfCanvases()->FindObject(cname.Data());
         if (cc != NULL) {
            cname += "_";
            cname += fGraphSerialNr ++;
         }
#ifdef MARABOUVERS
         HTCanvas * cg = new HTCanvas(cname, htitle, fWinx, fWiny,
                         fGraphXsize, fGraphYsize, NULL, NULL, graph);
//                         fGraphXsize, fGraphYsize, fHistPresent, 0, graph);
#else
         TCanvas * cg = new TCanvas(cname, htitle, fWinx, fWiny,
                         fGraphXsize, fGraphYsize);
#endif
         if (fGraphXdiv > 1 || fGraphYdiv > 1) {
            cg->Divide(fGraphXdiv, fGraphYdiv);
            cg->cd(1);
         }
         graph->Draw(drawopt);
//            gPad->Modified();
//            gPad->Update();
      }
      TH1 * gh = graph->GetHistogram();
      if (fGraphXtitle.Length() > 0)
         gh->GetXaxis()->SetTitle(fGraphXtitle.Data());
      if (fGraphYtitle.Length() > 0)
         gh->GetYaxis()->SetTitle(fGraphYtitle.Data());
      gh->SetLineWidth(1);
		gh->SetLineColor(0);
		if ( SetHistOptDialog::fTitleCenterX )
			gh->GetXaxis()->CenterTitle(kTRUE);
		if ( SetHistOptDialog::fTitleCenterY )
			gh->GetYaxis()->CenterTitle(kTRUE);
		
      graph->SetMarkerStyle(fGraphMarkerStyle);
      graph->SetMarkerColor(fGraphMarkerColor);
      graph->SetMarkerSize(fGraphMarkerSize);
      graph->SetLineStyle(fGraphLineStyle);
      graph->SetLineColor(fGraphLineColor);
      graph->SetFillStyle(fGraphFillStyle);
      graph->SetFillColor(fGraphFillColor);
      graph->SetLineWidth(fGraphLineWidth);
      if (fXaxisMin != 0 || fXaxisMax) graph->GetXaxis()->SetLimits(fXaxisMin, fXaxisMax);
      if (fYaxisMin != 0) graph->SetMinimum(fYaxisMin);
      if (fYaxisMax != 0) graph->SetMaximum(fYaxisMin);
      gPad->Modified();
      gPad->Update();
      cout << "TGraph *gr = (TGraph*)" << graph << endl;
   }
   SaveDefaults();
   return;
};
//_______________________________________________________________________________________

Int_t Ascii2GraphDialog::FindGraphs(TVirtualPad * ca, TList * logr, TList * pads)
{
   if (!ca) return -1;
   Int_t ngr = 0;
   TIter next(ca->GetListOfPrimitives());
   while (TObject * obj = next()) {
      if (obj->InheritsFrom("TGraph")) {
          ngr++;
          if (logr) logr->Add(obj);
          if (pads) pads->Add(ca);
      }
   }
// look for subpads
   TIter next1(ca->GetListOfPrimitives());
   while (TObject * obj = next1()) {
      if (obj->InheritsFrom("TPad")) {
          TPad * p = (TPad*)obj;
          TIter next2(p->GetListOfPrimitives());
          while (TObject * obj = next2()) {
             if (obj->InheritsFrom("TGraph")) {
                ngr++;
                if (logr) logr->Add(obj);
                if (pads) pads->Add(p);
             }
          }
      }
   }
   return ngr;
};

//_________________________________________________________________________

void Ascii2GraphDialog::SaveDefaults()
{
   cout << "Ascii2GraphDialog::SaveDefaults() " << endl;
   TEnv env(".hprrc");
   env.SetValue("Ascii2GraphDialog.fEmptyPad"  	    , fEmptyPad        );
   env.SetValue("Ascii2GraphDialog.Graph_Simple"	 , fGraph_Simple    );
   env.SetValue("Ascii2GraphDialog.Graph_Error" 	 , fGraph_Error     );
   env.SetValue("Ascii2GraphDialog.Graph_AsymError" , fGraph_AsymError );
   env.SetValue("Ascii2GraphDialog.fGraphColSelect" , fGraphColSelect  );
   env.SetValue("Ascii2GraphDialog.fGraphColSel1"   , fGraphColSel1    );
   env.SetValue("Ascii2GraphDialog.fGraphColSel2"   , fGraphColSel2    );
   env.SetValue("Ascii2GraphDialog.fGraphColSel3"   , fGraphColSel3    );
   env.SetValue("Ascii2GraphDialog.GraphFileName"   , fGraphFileName   );
   env.SetValue("Ascii2GraphDialog.GraphName"		 , fGraphName       );
   env.SetValue("Ascii2GraphDialog.GraphSelPad" 	 , fGraphSelPad     );
   env.SetValue("Ascii2GraphDialog.GraphNewPad" 	 , fGraphNewPad     );
   env.SetValue("Ascii2GraphDialog.GraphXsize"  	 , fGraphXsize      );
   env.SetValue("Ascii2GraphDialog.GraphYsize"  	 , fGraphYsize      );
   env.SetValue("Ascii2GraphDialog.GraphXtitle" 	 , fGraphXtitle     );
   env.SetValue("Ascii2GraphDialog.GraphYtitle" 	 , fGraphYtitle     );
   env.SetValue("Ascii2GraphDialog.XaxisMin"  		 , fXaxisMin        );
   env.SetValue("Ascii2GraphDialog.YaxisMin"  		 , fYaxisMin        );
   env.SetValue("Ascii2GraphDialog.XaxisMax"  		 , fXaxisMax        );
   env.SetValue("Ascii2GraphDialog.YaxisMax"  		 , fYaxisMax        );
   env.SetValue("Ascii2GraphDialog.GraphXdiv"		 , fGraphXdiv       );
   env.SetValue("Ascii2GraphDialog.GraphYdiv"		 , fGraphYdiv       );
/*
   env.SetValue("Ascii2GraphDialog.GraphDrawMark"   , fGraphDrawMark   );
   env.SetValue("Ascii2GraphDialog.GraphDrawLine"   , fGraphSimpleLine   );
   env.SetValue("Ascii2GraphDialog.GraphMarkerStyle", fGraphMarkerStyle);
   env.SetValue("Ascii2GraphDialog.GraphMarkerColor", fGraphMarkerColor);
   env.SetValue("Ascii2GraphDialog.GraphMarkerSize" , fGraphMarkerSize );
   env.SetValue("Ascii2GraphDialog.GraphLineStyle"  , fGraphLineStyle  );
   env.SetValue("Ascii2GraphDialog.GraphLineColor"  , fGraphLineColor  );
   env.SetValue("Ascii2GraphDialog.GraphFillStyle"  , fGraphFillStyle  );
   env.SetValue("Ascii2GraphDialog.GraphFillColor"  , fGraphFillColor  );
   env.SetValue("Ascii2GraphDialog.GraphLineWidth"  , fGraphLineWidth  );
*/
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Ascii2GraphDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fEmptyPad         = env.GetValue("Ascii2GraphDialog.fEmptyPad"  	   , 0);
   fGraph_Simple     = env.GetValue("Ascii2GraphDialog.Graph_Simple"  	, 0);
   fGraph_Error      = env.GetValue("Ascii2GraphDialog.Graph_Error"		, 1);
   fGraph_AsymError  = env.GetValue("Ascii2GraphDialog.Graph_AsymError" , 0);
   fGraphColSelect   = env.GetValue("Ascii2GraphDialog.fGraphColSelect" , 0);
   fGraphColSel1     = env.GetValue("Ascii2GraphDialog.fGraphColSel1"   , -1);
   fGraphColSel2     = env.GetValue("Ascii2GraphDialog.fGraphColSel2"   , -1);
   fGraphColSel3     = env.GetValue("Ascii2GraphDialog.fGraphColSel3"   , -1);
   fGraphFileName    = env.GetValue("Ascii2GraphDialog.GraphFileName" 	, "hs.dat");
   fGraphName        = env.GetValue("Ascii2GraphDialog.GraphName"  		, "hs");
   fGraphSelPad      = env.GetValue("Ascii2GraphDialog.GraphSelPad"		, 0);
   fGraphNewPad      = env.GetValue("Ascii2GraphDialog.GraphNewPad"		, 1);
   fGraphXsize       = env.GetValue("Ascii2GraphDialog.GraphXsize" 		, 800);
   fGraphYsize       = env.GetValue("Ascii2GraphDialog.GraphYsize" 		, 800);
   fGraphXtitle      = env.GetValue("Ascii2GraphDialog.GraphXtitle"	   , "Xvalues");
   fGraphYtitle      = env.GetValue("Ascii2GraphDialog.GraphYtitle"		, "Yvalues");
   fGraphXdiv        = env.GetValue("Ascii2GraphDialog.GraphXdiv"  		, 1);
   fXaxisMin         = env.GetValue("Ascii2GraphDialog.XaxisMin"  		, 0);
   fYaxisMin         = env.GetValue("Ascii2GraphDialog.YaxisMin"  		, 0);
   fXaxisMax         = env.GetValue("Ascii2GraphDialog.XaxisMax"  		, 0);
   fYaxisMax         = env.GetValue("Ascii2GraphDialog.YaxisMax"  		, 0);
   fGraphYdiv        = env.GetValue("Ascii2GraphDialog.GraphYdiv"  		, 1);
	fGraphSimpleLine  = GraphAttDialog::fGraphSimpleLine;
	fGraphSmoothLine  = GraphAttDialog::fGraphSmoothLine;
	fGraphFill        = GraphAttDialog::fGraphFill;
	fGraphPolyMarker  = GraphAttDialog::fGraphPolyMarker;
	fGraphBarChart    = GraphAttDialog::fGraphBarChart;
	
	fGraphLineStyle   = GraphAttDialog::fGraphLStyle;
	fGraphLineWidth   = GraphAttDialog::fGraphLWidth;
	fGraphLineColor   = GraphAttDialog::fGraphLColor;
	fGraphMarkerStyle = GraphAttDialog::fGraphMStyle;
	fGraphMarkerSize  = GraphAttDialog::fGraphMSize;
	fGraphMarkerColor = GraphAttDialog::fGraphMColor;
	fGraphFillStyle = GraphAttDialog::fGraphFStyle;
	fGraphFillColor = GraphAttDialog::fGraphFColor;
	
/*	
   fGraphMarkerStyle = env.GetValue("Ascii2GraphDialog.GraphMarkerStyle", 20);	
   fGraphMarkerColor = env.GetValue("Ascii2GraphDialog.GraphMarkerColor", 1);
   fGraphMarkerSize  = env.GetValue("Ascii2GraphDialog.GraphMarkerSize" , 1);
   fGraphLineStyle   = env.GetValue("Ascii2GraphDialog.GraphLineStyle"	, 1);
   fGraphLineColor   = env.GetValue("Ascii2GraphDialog.GraphLineColor"	, 1);
   fGraphFillStyle   = env.GetValue("Ascii2GraphDialog.GraphFillStyle"	, 0);
   fGraphFillColor   = env.GetValue("Ascii2GraphDialog.GraphFillColor"	, 0);
   fGraphLineWidth   = env.GetValue("Ascii2GraphDialog.GraphLineWidth"  , 2);
*/
}
//_________________________________________________________________________

void Ascii2GraphDialog::Show_Head_of_File()
{
   TString cmd(fGraphFileName.Data());
   cmd.Prepend("head ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2GraphDialog::Show_Tail_of_File()
{
   TString cmd(fGraphFileName.Data());
   cmd.Prepend("tail ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2GraphDialog::CloseDown(Int_t wid)
{
   cout << "Ascii2GraphDialog::CloseDown() " << endl;
   delete this;
}
