#include "TPad.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TString.h"
#include "TMrbString.h"
#include "TGWindow.h"
#include "HprGraph.h"
#include "HTCanvas.h"
#include "HistPresent.h"
#include "TGMrbValuesAndText.h"
#include "support.h"
#include <fstream>

ClassImp(HprGraph)

HprGraph::HprGraph(HistPresent * hpr, TGWindow * win) 
{
  
static const Char_t helpText[] = 
"Read values from ASCII file and construct a graph\n\
Input data can have the formats:\n\
X, Y:                     simple graph, no errors\n\
X, Y, Ex, Ey:             symmetric errors in X and Y\n\
X, Y, Exl, Exu, Eyl, Eyu: asymmetric errors in X and Y\n\
Select columns:           Select 2 columns to be used \n\
                          as X, Y of a simple graph\n\
                          columns are counted from 1\n\
The graph can be drawn / overlayed in a selected pad.\n\
Default is to construct a new canvas\n\
";

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fHistPresent = hpr;
   fGraphSerialNr = 0;
   fCommand = "Draw_The_Graph()";
   fCommandHead = "Show_Head_of_File()";
   RestoreDefaults();
   TList *row_lab = new TList(); 
   row_lab->Add(new TObjString("RadioButton_Simple: X, Y no errors"));
   row_lab->Add(new TObjString("RadioButton_Sym Errors: X, Y, Ex, Ey"));
   row_lab->Add(new TObjString("RadioButton_Asym Ers: X,Y,Exl,Exu,Eyl,Eyu"));
   row_lab->Add(new TObjString("RadioButton_Select columns, X, Y"));
   row_lab->Add(new TObjString("PlainIntVal_Col Sel"));
   row_lab->Add(new TObjString("PlainIntVal+Col Sel"));

   row_lab->Add(new TObjString("FileRequest_Name of Inputfile"));
   row_lab->Add(new TObjString("StringValue_Name of Graph"));
   row_lab->Add(new TObjString("StringValue_Title of Xaxis"));
   row_lab->Add(new TObjString("StringValue_Title of Yaxis"));
   row_lab->Add(new TObjString("CheckButton_Draw / Overlay in a selected pad"));
//   row_lab->Add(new TObjString("CheckButton_Draw in a new canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Xsize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Ysize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div X of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div Y of canvas"));
   row_lab->Add(new TObjString("CheckButton_Draw marker"));
   row_lab->Add(new TObjString("CheckButton+Draw line"));
   row_lab->Add(new TObjString("Mark_Select_MarkStyle"));
   row_lab->Add(new TObjString("ColorSelect+MarkColor"));
   row_lab->Add(new TObjString("Float_Value+MarkSize"));
   row_lab->Add(new TObjString("ColorSelect_LineC"));
   row_lab->Add(new TObjString("PlainShtVal+Width"));
   row_lab->Add(new TObjString("LineSSelect+Style"));
   row_lab->Add(new TObjString("Fill_Select_Fill Style"));
   row_lab->Add(new TObjString("ColorSelect+Fill color"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Graph"));
//   row_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = row_lab->GetSize();

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
   valp[ind++] = &fGraphSelPad;
//   valp[ind++] = &fGraphNewPad;
   valp[ind++] = &fGraphXsize;
   valp[ind++] = &fGraphYsize;
   valp[ind++] = &fGraphXdiv;
   valp[ind++] = &fGraphYdiv;
   valp[ind++] = &fGraphDrawMark;
   valp[ind++] = &fGraphDrawLine;
   valp[ind++] = &fGraphMarkerStyle;
   valp[ind++] = &fGraphMarkerColor;
   valp[ind++] = &fGraphMarkerSize;
   valp[ind++] = &fGraphLineColor;
   valp[ind++] = &fGraphLineWidth;
   valp[ind++] = &fGraphLineStyle;
   valp[ind++] = &fGraphFillStyle;
   valp[ind++] = &fGraphFillColor;
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fCommand;
   Int_t itemwidth = 320;
   ok = GetStringExt("Graphs parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};  
//_________________________________________________________________________
            
HprGraph::~HprGraph() 
{
   SaveDefaults();
};
//_________________________________________________________________________
            
void HprGraph::Draw_The_Graph()
{
   TArrayD xval(100), yval(100), zval(100), wval(100), eyl(100), eyh(100);
   ifstream infile;
   infile.open(fGraphFileName.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "HprGraph: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return;
	}
   Int_t n = 0;
//   Double_t x, y, w;
   Bool_t ok = kTRUE;
   Double_t x[6];
   Int_t nval;
   
   TString line;

   if (fGraphColSelect) {
      TString del(" ,");
      TObjArray * oa;
    	while ( 1 ) {
         line.ReadLine(infile);
         if (infile.eof()) break;
         oa = line.Tokenize(del);
         Int_t nent = oa->GetEntries();
         if ( fGraphColSel1 > nent || fGraphColSel2 > nent) {
            cout << "Not enough entries: " << n << endl;
            break;
         }
         TString val = ((TObjString*)oa->At(fGraphColSel1-1))->String();
         if (!val.IsFloat()) {
            cout << "Illegal double: " << val << " at line: " << n+1 << endl;
            break;
         }
         xval.AddAt(val.Atof(), n);

         val = ((TObjString*)oa->At(fGraphColSel2-1))->String();
         if (!val.IsFloat()) {
            cout << "Illegal double: " << val << " at line: " << n+1 << endl;
            break;
         }
         yval.AddAt(val.Atof(), n);
         n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	yval.Set(n+100);
         }
      }
   } else {
   	if (fGraph_Simple == 1)    nval = 2;
   	if (fGraph_Error == 1)     nval = 4;
   	if (fGraph_AsymError == 1) nval = 6;
   	while ( 1 ) {
      	Int_t i = 0;
      	while (i < nval) {
         	infile >> x[i];
	      	if (infile.eof()) {
            	 if (i != 0) {
               	cout << "Warning: Number of input data not multiple of: " << nval << endl;
               	cout << "Discard the " << i << " value(s) at end of file " << endl;
               	cout << ": " << x[0] << endl;
               	if (i > 1) cout << ": " << x[1] << endl;
               	if (i > 2) cout << ": " << x[2] << endl;
               	if (i > 3) cout << ": " << x[3] << endl;
               	if (i > 4) cout << ": " << x[4] << endl;
            	 }
            	 ok = kFALSE;
			   	 break;
		   	}
         	if (!infile.good()) {
	//          discard non white space separator,comma etc.
            	infile.clear();
            	infile >> line;
            	continue;
         	}
         	if      (i == 0) xval.AddAt(x[i], n);
         	else if (i == 1) yval.AddAt(x[i], n);
         	else if (i == 2) zval.AddAt(x[i], n);
         	else if (i == 3) wval.AddAt(x[i], n);
         	else if (i == 4) eyl.AddAt(x[i], n);
         	else if (i == 5) eyh.AddAt(x[i], n);
         	i++;
      	}
      	if (!ok) break;
      	n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	if (nval > 1) yval.Set(n+100);
         	if (nval > 2) zval.Set(n+100);
         	if (nval > 3) wval.Set(n+100);
         	if (nval > 4) eyl.Set(n+100);
         	if (nval > 5) eyh.Set(n+100);
      	}
   	}
   }
   infile.close();

   cout << "entries " << n << endl;
   if (n < 1) return;


   TString hname = fGraphFileName;
   Int_t ip = hname.Index(".");
   if (ip > 0) hname.Resize(ip);
   TString htitle = hname;
   htitle.Prepend("Values from ");
   hname.Prepend("g_");

   TMrbString temp;

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
      if (fGraphName.Length() <= 0)
         graph->SetName(fGraphFileName.Data());
      else 
         graph->SetName(fGraphName.Data());

//      graph->SetTitle(graph->GetName());
      TString drawopt("A");           // draw axis as default
      if (fGraphDrawMark) drawopt+= "P";
      if (fGraphDrawLine) drawopt+= "L";
      if (fGraphFillStyle)  drawopt+= "F";
//         cout << "gPad->GetName() " <<gPad->GetName() << endl;
      if (fGraphSelPad) {
         Int_t ngr = FindGraphs(gPad);
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
         Int_t winx = 100,  winy = 100;
         if (fHistPresent != NULL) fHistPresent->GetWindowPosition(&winx, &winy);
         HTCanvas * cg = new HTCanvas(cname, htitle, winx, winy,
                         fGraphXsize, fGraphYsize, fHistPresent, 0, 0, graph);
         if (fHistPresent != NULL)fHistPresent->GetCanvasList()->Add(cg);
         if (fGraphXdiv > 1 || fGraphYdiv > 1) {
            cg->Divide(fGraphXdiv, fGraphYdiv);
            cg->cd(1);
         }
         graph->Draw(drawopt);
//            gPad->Modified();
//            gPad->Update();
      }

      if (fGraphXtitle.Length() > 0)
         graph->GetHistogram()->GetXaxis()->SetTitle(fGraphXtitle.Data());
      if (fGraphYtitle.Length() > 0)
         graph->GetHistogram()->GetYaxis()->SetTitle(fGraphYtitle.Data());
      graph->SetMarkerStyle(fGraphMarkerStyle);
      graph->SetMarkerColor(fGraphMarkerColor);
      graph->SetMarkerSize(fGraphMarkerSize);
      graph->SetLineStyle(fGraphLineStyle);
      graph->SetLineColor(fGraphLineColor);
      graph->SetFillColor(fGraphFillStyle);
      graph->SetFillColor(fGraphFillColor);
      graph->SetLineWidth(fGraphLineWidth);
      gPad->Modified();
      gPad->Update();
   }
   SaveDefaults();
   return;
};
//_________________________________________________________________________
            
void HprGraph::SaveDefaults()
{
   cout << "HprGraph::SaveDefaults() " << endl;
   TEnv env(".rootrc");
   env.SetValue("HistPresent.Graph_Simple"	 , fGraph_Simple    );
   env.SetValue("HistPresent.Graph_Error" 	 , fGraph_Error     );
   env.SetValue("HistPresent.Graph_AsymError" , fGraph_AsymError );
   env.SetValue("HistPresent.fGraphColSelect" , fGraphColSelect);
   env.SetValue("HistPresent.fGraphColSel1"   , fGraphColSel1);
   env.SetValue("HistPresent.fGraphColSel2"   , fGraphColSel2);
   env.SetValue("HistPresent.fGraphColSel3"   , fGraphColSel3);
   env.SetValue("HistPresent.GraphFileName"   , fGraphFileName   );
   env.SetValue("HistPresent.GraphName"		 , fGraphName       );
   env.SetValue("HistPresent.GraphSelPad" 	 , fGraphSelPad     );
   env.SetValue("HistPresent.GraphNewPad" 	 , fGraphNewPad     );
   env.SetValue("HistPresent.GraphXsize"  	 , fGraphXsize      );
   env.SetValue("HistPresent.GraphYsize"  	 , fGraphYsize      );
   env.SetValue("HistPresent.GraphXtitle" 	 , fGraphXtitle     );
   env.SetValue("HistPresent.GraphYtitle" 	 , fGraphYtitle     );
   env.SetValue("HistPresent.GraphXdiv"		 , fGraphXdiv       );
   env.SetValue("HistPresent.GraphYdiv"		 , fGraphYdiv       );
   env.SetValue("HistPresent.GraphDrawMark"   , fGraphDrawMark   );
   env.SetValue("HistPresent.GraphDrawLine"   , fGraphDrawLine   );
   env.SetValue("HistPresent.GraphMarkerStyle", fGraphMarkerStyle);
   env.SetValue("HistPresent.GraphMarkerColor", fGraphMarkerColor);
   env.SetValue("HistPresent.GraphMarkerSize" , fGraphMarkerSize );
   env.SetValue("HistPresent.GraphLineStyle"  , fGraphLineStyle  );
   env.SetValue("HistPresent.GraphLineColor"  , fGraphLineColor  );
   env.SetValue("HistPresent.GraphFillStyle"  , fGraphFillStyle  );
   env.SetValue("HistPresent.GraphFillColor"  , fGraphFillColor  );
   env.SetValue("HistPresent.GraphLineWidth"  , fGraphLineWidth  );
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void HprGraph::RestoreDefaults()
{
   TEnv env(".rootrc");
   fGraph_Simple     = env.GetValue("HistPresent.Graph_Simple"  	, 0);
   fGraph_Error      = env.GetValue("HistPresent.Graph_Error"		, 1);
   fGraph_AsymError  = env.GetValue("HistPresent.Graph_AsymError" , 0);
   fGraphColSelect   = env.GetValue("HistPresent.fGraphColSelect" , 0);
   fGraphColSel1     = env.GetValue("HistPresent.fGraphColSel1"   , -1);
   fGraphColSel2     = env.GetValue("HistPresent.fGraphColSel2"   , -1);
   fGraphColSel3     = env.GetValue("HistPresent.fGraphColSel3"   , -1);
   fGraphFileName    = env.GetValue("HistPresent.GraphFileName" 	, "hs.dat");
   fGraphName        = env.GetValue("HistPresent.GraphName"  		, "hs");
   fGraphSelPad      = env.GetValue("HistPresent.GraphSelPad"		, 0);
   fGraphNewPad      = env.GetValue("HistPresent.GraphNewPad"		, 1);
   fGraphXsize       = env.GetValue("HistPresent.GraphXsize" 		, 800);
   fGraphYsize       = env.GetValue("HistPresent.GraphYsize" 		, 800);
   fGraphXtitle      = env.GetValue("HistPresent.GraphXtitle"	   , "Xvalues");
   fGraphYtitle      = env.GetValue("HistPresent.GraphYtitle"		, "Yvalues");
   fGraphXdiv        = env.GetValue("HistPresent.GraphXdiv"  		, 1);
   fGraphYdiv        = env.GetValue("HistPresent.GraphYdiv"  		, 1);
   fGraphDrawMark    = env.GetValue("HistPresent.GraphDrawMark" 	, 1);
   fGraphDrawLine    = env.GetValue("HistPresent.GraphDrawLine" 	, 0);
   fGraphMarkerStyle = env.GetValue("HistPresent.GraphMarkerStyle", 20);
   fGraphMarkerColor = env.GetValue("HistPresent.GraphMarkerColor", 1);
   fGraphMarkerSize  = env.GetValue("HistPresent.GraphMarkerSize" , 1);
   fGraphLineStyle   = env.GetValue("HistPresent.GraphLineStyle"	, 1);
   fGraphLineColor   = env.GetValue("HistPresent.GraphLineColor"	, 1);
   fGraphFillStyle   = env.GetValue("HistPresent.GraphFillStyle"	, 0);
   fGraphFillColor   = env.GetValue("HistPresent.GraphFillColor"	, 0);
   fGraphLineWidth   = env.GetValue("HistPresent.GraphLineWidth"  , 2);
}
//_________________________________________________________________________
            
void HprGraph::Show_Head_of_File()
{
   TString cmd(fGraphFileName.Data());
   cmd.Prepend("head ");
//   cmd.Append("\")");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________
            
void HprGraph::CloseDown()
{
   cout << "HprGraph::CloseDown() " << endl;
   delete this;
}
