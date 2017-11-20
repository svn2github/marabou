#include "HTCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TList.h"
#include "TString.h"
#include <iostream>
using namespace std;
TCanvas * HT2TC(HTCanvas *htc)
{
	Int_t ww = htc->GetWindowWidth();
	Int_t wh = htc->GetWindowHeight()-26;	// account for decor
	TString cn = htc->GetName();
	// make name differ
	if (cn.BeginsWith("C_")){
		cn = cn(2,100);
	} else {
		cn.Prepend("TC_");
	}
	TString ct = htc->GetTitle();
	TCanvas * tc = new TCanvas(cn, ct, ww,wh);
	tc->Draw();
	TList *lop = tc->GetListOfPrimitives();
	TIter next( htc->GetListOfPrimitives());
	TObject *obj;
	while ( (obj = next()) ){
		lop->Add(obj);
	}
	tc->Draw();
	return tc;
}
//___________________________________________________________

int HT2TCanvas(const char *infile,  const char *outfile,
					const char * mode="UPDATE")
{
	// look for all HTCanvas in infile and write as TCanvas
	// to outfile
	TFile *fout = new TFile(outfile, mode);
	if (!fout->IsOpen() )
		return -2;
	TFile *fin = new TFile(infile);
	if (!fin->IsOpen() )
		return -1;
	int nc = 0;
	fin->cd();	
	TIter next(gDirectory->GetListOfKeys());
	HTCanvas *htc;
   TKey* key;
   // loop on HTCanvas
   while( (key = (TKey*)next()) ) {
      if(!strncmp(key->GetClassName(),"HTCanvas",8)) {
         htc = (HTCanvas*)key->ReadObj(); 
         TCanvas *tc = HT2TC(htc);
			fout->cd();
			tc->Write();
			nc++;
		}
	}
	return nc;
}
