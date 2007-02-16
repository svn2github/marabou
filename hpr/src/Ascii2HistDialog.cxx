#include "Ascii2HistDialog.h"
#include "TGMrbValuesAndText.h"

#ifdef MARABOUVERS
#include "HistPresent.h"
#endif

#include "TMath.h"
#include "TEnv.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TObjString.h"
#include "TSystem.h"
#include <fstream>

//Double_t LocMin(Int_t n, Double_t *x)
//{
//   Double xmin = 

ClassImp(Ascii2HistDialog)

Ascii2HistDialog::Ascii2HistDialog( TGWindow * win) 
{
  
static const Char_t helpText[] = 
"Read values from ASCII file and fill histogram\n\
Input data can have the formats:\n\
W:          1 dim: Spectrum, Channel Contents only\n\
                   no X values\n\
X:          1 dim: X values to be filled\n\
X, W:       1 dim: X, Weight\n\
X, Y:       2 dim: X, Y values to be filled\n\
X, Y, W:    2 dim: X, Y , Weight \n\
X, Ym Z:    3 dim: X, Y, Z values to be filled\n\
X, Y, Z, W: 3 dim: X, Y ,Z, Weight \n\
\n\
A common error is applied to all channels\n\
if this value is > 0\n\
\n\
Before the histogram is build the values must be read\n\
by \"Read_Input()\". At this moment the format\n\
should have been selected so meaningful values\n\
for the axis can be calculated if the option\n\
\"Keep limits\" is off.\n\
";

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fNvalues = 0;
   fCommand = "Draw_The_Hist()";
   fReadCommand = "Read_Input()";
   fCommandHead = "Show_Head_of_File()";
   RestoreDefaults();
   TList *row_lab = new TList(); 
   row_lab->Add(new TObjString("RadioButton_1 Dim, Spectrum, channel content, no X "));
   row_lab->Add(new TObjString("RadioButton_1 Dim, X values to be filled"));
   row_lab->Add(new TObjString("RadioButton_1 Dim, X, Weight"));
   row_lab->Add(new TObjString("RadioButton_2 Dim, X, Y values to be filled"));
   row_lab->Add(new TObjString("RadioButton_2 Dim, X, Y, Weight"));
   row_lab->Add(new TObjString("RadioButton_3 Dim, X, Y, Z values to be filled"));
   row_lab->Add(new TObjString("RadioButton_3 Dim, X, Y, Z, Weight"));
   row_lab->Add(new TObjString("FileRequest_Name of Inputfile   "));
   row_lab->Add(new TObjString("StringValue_Name"));
   row_lab->Add(new TObjString("StringValue_Title"));
   row_lab->Add(new TObjString("PlainIntVal_NbX"));
   row_lab->Add(new TObjString("DoubleValue+Xl"));
   row_lab->Add(new TObjString("DoubleValue+Xu"));
   row_lab->Add(new TObjString("PlainIntVal_NbY"));
   row_lab->Add(new TObjString("DoubleValue+Yl"));
   row_lab->Add(new TObjString("DoubleValue+Yu"));
   row_lab->Add(new TObjString("PlainIntVal_NbZ"));
   row_lab->Add(new TObjString("DoubleValue+Zl"));
   row_lab->Add(new TObjString("DoubleValue+Zu"));
   row_lab->Add(new TObjString("CheckButton_Keep limits"));
   row_lab->Add(new TObjString("DoubleValue+Common error"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt_Read_Input"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Histogram"));

   valp[ind++] = &fSpectrum;      
   valp[ind++] = &f1Dim;          
   valp[ind++] = &f1DimWithWeight;
   valp[ind++] = &f2Dim;          
   valp[ind++] = &f2DimWithWeight;
   valp[ind++] = &f3Dim;          
   valp[ind++] = &f3DimWithWeight;
   valp[ind++] = &fHistFileName;
   valp[ind++] = &fHistName;
   valp[ind++] = &fHistTitle;
   valp[ind++] = &fNbinsX; 
   valp[ind++] = &fXlow;  
   valp[ind++] = &fXup;   
   valp[ind++] = &fNbinsY; 
   valp[ind++] = &fYlow;  
   valp[ind++] = &fYup;   
   valp[ind++] = &fNbinsZ; 
   valp[ind++] = &fZlow;  
   valp[ind++] = &fZup;   
   valp[ind++] = &fKeepLimits; 
   valp[ind++] = &fError; 
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fReadCommand;
   valp[ind++] = &fCommand;

   Int_t itemwidth = 380;
   ok = GetStringExt("Hists parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};  
//_________________________________________________________________________
            
Ascii2HistDialog::~Ascii2HistDialog() 
{
   SaveDefaults();
};
//_________________________________________________________________________
            
void Ascii2HistDialog::Read_Input()
{
   fNvalues = 0;
   ifstream infile;
   infile.open(fHistFileName.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "Ascii2HistDialog: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return;
	}
	fXval.Set(100);
	fYval.Set(100);
	fZval.Set(100);
	fWval.Set(100);
   Bool_t ok = kTRUE;
   Double_t x[6];
   Int_t nval;
   if      (fSpectrum || f1Dim)       nval = 1;
   else if (f1DimWithWeight || f2Dim) nval = 2;
   else if (f2DimWithWeight || f3Dim) nval = 3;
   else if (f3DimWithWeight)          nval = 4;
   else {
      cout << "Invalid mode: " << endl;
      return;
   }
  
   TString line;
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
         if      (i == 0) fXval.AddAt(x[i], fNvalues);
         else if (i == 1) fYval.AddAt(x[i], fNvalues);
         else if (i == 2) fZval.AddAt(x[i], fNvalues);
         else if (i == 3) fWval.AddAt(x[i], fNvalues);
         i++;
      }
      if (!ok) break;
      fNvalues++;
//      cout << fNvalues<< " " << fXval.GetSize()<< endl;
      if (fNvalues >= fXval.GetSize()){
                       fXval.Set(fNvalues+100);
         if (nval > 1) fYval.Set(fNvalues+100);
         if (nval > 2) fZval.Set(fNvalues+100);
         if (nval > 3) fWval.Set(fNvalues+100);
      }
   }
   infile.close();
   cout << "entries " << fNvalues;
   if (fSpectrum) {
      fNbinsX = fNvalues;
      fXlow = 0;
      fXup  = fNvalues;
   } else {
      if (fKeepLimits <= 0) {
   	   if (f1DimWithWeight ) fNbinsX = fNvalues;
         fXlow = fXval[TMath::LocMin(fNvalues, fXval.GetArray())]; 
         fXup  = fXval[TMath::LocMax(fNvalues, fXval.GetArray())];
         Double_t binw2 = 0.5 * (fXup - fXlow) / (Double_t) fNbinsX;
         fXlow -= binw2;
         fXup  += binw2;
         cout << " fXlow " << fXlow  << " fXup " << fXup;
      }
   	if (f2Dim ||f2DimWithWeight ) {
         if (fKeepLimits <= 0) {
      	   fYlow = fYval[TMath::LocMin(fNvalues, fYval.GetArray())]; 
      	   fYup  = fYval[TMath::LocMax(fNvalues, fYval.GetArray())]; 
      	   Double_t binw2 = 0.5 * (fYup - fYlow) / (Double_t) fNbinsY;
      	   fYlow -= binw2;
      	   fYup  += binw2;
            cout << " fYlow " << fYlow  << " fYup " << fYup;
         }
    	}
      if (f3Dim || f3DimWithWeight) {
         if (fKeepLimits <= 0) {
      	   fZlow = fZval[TMath::LocMin(fNvalues, fZval.GetArray())]; 
      	   fZup  = fZval[TMath::LocMax(fNvalues, fZval.GetArray())]; 
      	   Double_t binw2 = 0.5 * (fZup - fZlow) / (Double_t) fNbinsZ;
      	   fZlow -= binw2;
      	   fZup  += binw2;
            cout << " fZlow " << fZlow  << " fZup " << fZup;
   	   }
   	}
   }

   cout << endl;
   return;
}
//_________________________________________________________________________
            
void Ascii2HistDialog::Draw_The_Hist()
{
   if (fNvalues <= 0) {
      cout << "No values, do: Read_Input first" << endl;
      return;
   }
   TH1 * hist = 0;
   if (fSpectrum ||f1Dim || f1DimWithWeight) {
      TH1D * hist1 = new TH1D(fHistName, fHistTitle, fNbinsX, fXlow, fXup);
      hist = hist1;
      if (fSpectrum) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->SetBinContent(i+1, fXval[i]);
         }
      } else if (f1Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->Fill(fXval[i]);
         }
      } else if (f1DimWithWeight) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->Fill(fXval[i], fYval[i]);
         }
      }
      if (fError > 0) {
         for (Int_t i = 0; i < fNbinsX; i++) {
            if (hist1->GetBinError(i+1) != 0)
               hist1->SetBinError(i+1, fError);
         }
      }    
   }
   
   if (f2Dim || f2DimWithWeight) {
      TH2F * hist2 = new TH2F(fHistName, fHistTitle, fNbinsX, fXlow, fXup, 
                                            fNbinsY, fYlow, fYup);
      hist = hist2;
      if (f2Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist2->Fill(fXval[i], fYval[i]);
         }
      } else if (f2DimWithWeight) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist2->Fill(fXval[i], fYval[i], fZval[i]);
         }
      }
      if (fError > 0) {
         for (Int_t i = 0; i < fNbinsX; i++) {
            for (Int_t k = 0; k < fNbinsY; k++) {
               if (hist2->GetCellError(i+1, k+1) != 0)
                  hist2->SetCellError(i+1, k+1, fError);
            }
         }
      }    
   }
   if (f3Dim || f3DimWithWeight) {
      TH3F * hist3 = new TH3F(fHistName, fHistTitle, fNbinsX, fXlow, fXup, 
                             fNbinsY, fYlow, fYup, fNbinsZ, fZlow, fZup);
      hist = hist3;
      if (f3Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist3->Fill(fXval[i], fYval[i], fZval[i]);
         }
      } else if (f3DimWithWeight) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist3->Fill(fXval[i], fYval[i], fZval[i], fWval[i]);
         }
      }
   }

#ifdef MARABOUVERS
   HistPresent * hpr = (HistPresent*)gROOT->GetList()->FindObject("mypres");
   if (hpr) hpr->ShowHist(hist);
   else     hist->Draw();
#else
   hist->Draw();
#endif
   SaveDefaults();
   return;

}
//_________________________________________________________________________
            
void Ascii2HistDialog::Show_Head_of_File()
{
   TString cmd(fHistFileName.Data());
   cmd.Prepend("head ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________
            
void Ascii2HistDialog::SaveDefaults()
{
   cout << "Ascii2HistDialog::SaveDefaults() " << endl;
   TEnv env(".rootrc");
   env.SetValue("Ascii2HistDialog.Ascii2HistHistFileName",   fHistFileName);
   env.SetValue("Ascii2HistDialog.Ascii2HistHistName",		  fHistName);
   env.SetValue("Ascii2HistDialog.Ascii2HistHistTitle",		  fHistTitle);
   env.SetValue("Ascii2HistDialog.Ascii2HistSpectrum",		  fSpectrum); 		
   env.SetValue("Ascii2HistDialog.Ascii2Hist1Dim", 			  f1Dim);  			
   env.SetValue("Ascii2HistDialog.Ascii2Hist1DimWithWeight", f1DimWithWeight); 
   env.SetValue("Ascii2HistDialog.Ascii2Hist2Dim", 			  f2Dim);  			
   env.SetValue("Ascii2HistDialog.Ascii2Hist2DimWithWeight", f2DimWithWeight); 
   env.SetValue("Ascii2HistDialog.Ascii2Hist3Dim", 			  f3Dim);  			
   env.SetValue("Ascii2HistDialog.Ascii2Hist3DimWithWeight", f3DimWithWeight); 
   env.SetValue("Ascii2HistDialog.Ascii2HistError",  		     fError); 
   env.SetValue("Ascii2HistDialog.Ascii2HistNbinsX",  		  fNbinsX); 
   env.SetValue("Ascii2HistDialog.Ascii2HistXlow",  			  fXlow);  
   env.SetValue("Ascii2HistDialog.Ascii2HistXup",   			  fXup);	
   env.SetValue("Ascii2HistDialog.Ascii2HistNbinsY",  		  fNbinsY); 
   env.SetValue("Ascii2HistDialog.Ascii2HistYlow",  			  fYlow);  
   env.SetValue("Ascii2HistDialog.Ascii2HistYup",   			  fYup);	
   env.SetValue("Ascii2HistDialog.Ascii2HistNbinsZ",  		  fNbinsZ); 
   env.SetValue("Ascii2HistDialog.Ascii2HistZlow",  			  fZlow);  
   env.SetValue("Ascii2HistDialog.Ascii2HistZup",   			  fZup);	
   env.SetValue("Ascii2HistDialog.Ascii2HistKeepLimits",   		  fKeepLimits);	
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
				
void Ascii2HistDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fHistFileName    = env.GetValue("Ascii2HistDialog.Ascii2HistHistFileName",  "values.dat");
   fHistName 		  = env.GetValue("Ascii2HistDialog.Ascii2HistHistName",		  "h1");
   fHistTitle  	  = env.GetValue("Ascii2HistDialog.Ascii2HistHistTitle",  	  "h1");
   fSpectrum        = env.GetValue("Ascii2HistDialog.Ascii2HistSpectrum",		   0);	  
   f1Dim            = env.GetValue("Ascii2HistDialog.Ascii2Hist1Dim", 			   0); 		  
   f1DimWithWeight  = env.GetValue("Ascii2HistDialog.Ascii2Hist1DimWithWeight", 1); 
   f2Dim            = env.GetValue("Ascii2HistDialog.Ascii2Hist2Dim", 			   0); 		  
   f2DimWithWeight  = env.GetValue("Ascii2HistDialog.Ascii2Hist2DimWithWeight", 0); 
   f3Dim            = env.GetValue("Ascii2HistDialog.Ascii2Hist3Dim", 			   0); 		  
   f3DimWithWeight  = env.GetValue("Ascii2HistDialog.Ascii2Hist3DimWithWeight", 0); 
   fError           = env.GetValue("Ascii2HistDialog.Ascii2HistError",  		  0.0); 
   fNbinsX  		  = env.GetValue("Ascii2HistDialog.Ascii2HistNbinsX",  	    100); 
   fXlow    		  = env.GetValue("Ascii2HistDialog.Ascii2HistXlow", 			   0.);  
   fXup     		  = env.GetValue("Ascii2HistDialog.Ascii2HistXup",  			 100.); 
   fNbinsY  		  = env.GetValue("Ascii2HistDialog.Ascii2HistNbinsY",  		 100); 
   fYlow    		  = env.GetValue("Ascii2HistDialog.Ascii2HistYlow", 			   0.);  
   fYup     		  = env.GetValue("Ascii2HistDialog.Ascii2HistYup",  			 100.); 
   fNbinsZ  		  = env.GetValue("Ascii2HistDialog.Ascii2HistNbinsZ",  		 100); 
   fZlow    		  = env.GetValue("Ascii2HistDialog.Ascii2HistZlow", 			   0.);  
   fZup     		  = env.GetValue("Ascii2HistDialog.Ascii2HistZup",  			 100.); 
   fKeepLimits      = env.GetValue("Ascii2HistDialog.Ascii2HistKeepLimits",  		0 ); 
   cout << "fXup " << fXup<< endl;
}
//_________________________________________________________________________
            
void Ascii2HistDialog::CloseDown()
{
   cout << "Ascii2HistDialog::CloseDown() " << endl;
   delete this;
}
