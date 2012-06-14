#include "TFile.h"
#include "TGMrbValuesAndText.h"
#include "Rebin2DimDialog.h"

#include <iostream>

ClassImp(Rebin2DimDialog)

Rebin2DimDialog::Rebin2DimDialog(TH2* hist, TGWindow * win)
{
	static const char helptext[] =
"Select a region in a 2dim histogram and rebin\n\
As default the whole histogram is used.\n\
If number of channels in X or Y are not a multiple\n\
of the rebin values, channels at upper X and Y are\n\
discarded.\n\
";
	
	fCanvas = ((TRootCanvas*)win)->Canvas();
	fHist = hist;
	gROOT->GetListOfCleanups()->Add(this);
	fRow_lab = new TList();
	Int_t ind = 0;
	fNewName = hist->GetName();
	fNewName += "_rebinned";
	fNewTitle = hist->GetTitle();
	fNewTitle += "_rebinned";
	
	fBinLowX =  1;
	fBinUpX  = hist->GetNbinsX();
	fBinLowY = 1;
	fBinUpY  = hist->GetNbinsY();
	fRebinX  = 2;
	fRebinY  = 2;
	fNewNbinsX = fBinUpX / fRebinX;
	fNewNbinsY = fBinUpY / fRebinY;
	static TString rebincmd("ExecuteRebin()");
	static TString printcmd("PrintHistParams()");
	
	fRow_lab->Add(new TObjString("StringValue_New hist name "));
	fValp[ind++] = &fNewName;
	fRow_lab->Add(new TObjString("StringValue_New hist title"));
	fValp[ind++] = &fNewTitle;
	fRow_lab->Add(new TObjString("PlainIntVal_Low_binX"));
	fBidBinLowX = ind;
	fValp[ind++] = &fBinLowX;
	fRow_lab->Add(new TObjString("PlainIntVal+Up_binX"));
	fBidBinUpX = ind;
	fValp[ind++] = &fBinUpX;
	fRow_lab->Add(new TObjString("PlainIntVal_Low_binY"));
	fBidBinLowY = ind;
	fValp[ind++] = &fBinLowY;
	fRow_lab->Add(new TObjString("PlainIntVal+Up_binY"));
	fBidBinUpY = ind;
	fValp[ind++] = &fBinUpY;
	fRow_lab->Add(new TObjString("PlainIntVal_ RebinX"));
	fBidRebinX = ind;
	fValp[ind++] = &fRebinX;
	fRow_lab->Add(new TObjString("PlainIntVal+ RebinY"));
	fBidRebinY = ind;
	fValp[ind++] = &fRebinY;
	fRow_lab->Add(new TObjString("PlainIntVal_NewNbinsX"));
	fBidNewNbinsX = ind;
	fValp[ind++] = &fNewNbinsX;
	fRow_lab->Add(new TObjString("PlainIntVal+NewNbinsY"));
	fBidNewNbinsY = ind;
	fValp[ind++] = &fNewNbinsY;
	fRow_lab->Add(new TObjString("CommandButt_Print new params"));
	fValp[ind++] = &printcmd;
	fRow_lab->Add(new TObjString("CommandButt+Execute Rebin"));
	fValp[ind++] = &rebincmd;
	static Int_t ok;
	Int_t itemwidth = 300;
	fDialog =
	new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
								  NULL, NULL, fRow_lab, fValp,
								  NULL, NULL, helptext, this, this->ClassName());
	fDialog->DisableButton(fBidNewNbinsX);
	fDialog->DisableButton(fBidNewNbinsY);
}

//______________________________________________________________________
	
void Rebin2DimDialog::PrintHistParams()
{
	cout << "Original histogram:" <<endl;
	cout << "Low X: " << fHist->GetXaxis()->GetXmin()
		  << " Up X: " << fHist->GetXaxis()->GetXmax()
		  << " NbinX : " << fHist->GetNbinsX() 
		  << " XbinWidth: " << fHist->GetXaxis()->GetBinWidth(1) << endl;
	cout << "Low Y: " << fHist->GetYaxis()->GetXmin()
		  << " Up Y: " << fHist->GetYaxis()->GetXmax()
		  << " NbinY : " << fHist->GetNbinsY() 
		  << " YbinWidth: " << fHist->GetYaxis()->GetBinWidth(1) << endl;
		  
	cout << "New histogram:" <<endl;
	Double_t xmin = fHist->GetXaxis()->GetBinLowEdge(fBinLowX);
	Double_t xmax = fHist->GetXaxis()->GetBinUpEdge(fBinLowX + fRebinX*fNewNbinsX-1);
	Double_t xwid = (xmax - xmin) / (Double_t)fNewNbinsX;
	Double_t ymin = fHist->GetYaxis()->GetBinLowEdge(fBinLowY);
	Double_t ymax = fHist->GetYaxis()->GetBinUpEdge(fBinLowY + fRebinY*fNewNbinsY-1);
	Double_t ywid = (ymax - ymin) / (Double_t)fNewNbinsY;
	cout << "Low X: " << xmin
		  << " Up X: " << xmax
		  << " NbinX : " << fNewNbinsX 
		  << " XbinWidth: " << xwid<< endl;
	cout << "Low Y: " << ymin
		  << " Up Y: " << ymax
		  << " NbinY : " << fNewNbinsY
		  << " YbinWidth: " << ywid << endl;
}
//______________________________________________________________________

void Rebin2DimDialog::ExecuteRebin()
{
	if ( fRebinX <= 0 || fRebinY <= 0) {
		cout << "Rebin value must be > 0" << endl;
		return;
	}
	Double_t xmin = fHist->GetXaxis()->GetBinLowEdge(fBinLowX);
	Double_t xmax = fHist->GetXaxis()->GetBinUpEdge(fBinLowX + fRebinX*fNewNbinsX-1);
	Double_t ymin = fHist->GetYaxis()->GetBinLowEdge(fBinLowY);
	Double_t ymax = fHist->GetYaxis()->GetBinUpEdge(fBinLowY + fRebinY*fNewNbinsY-1);
	TH2F * hr = new TH2F(fNewName, fNewTitle, fNewNbinsX,xmin,xmax, fNewNbinsY,ymin, ymax);
	Int_t binx = fBinLowX;
	Int_t biny = fBinLowY;
	Int_t nfill = 0;
	for (Int_t ix = 1; ix <= fNewNbinsX; ix++) {
		Int_t binys = biny;
		for (Int_t iy = 1; iy <= fNewNbinsY; iy++) {
			Float_t cont = 0.;
			for (Int_t ixo = binx; ixo <= binx+fRebinX-1; ixo++) {
				for (Int_t iyo = binys; iyo <= binys+fRebinY-1; iyo++) {
					cont += fHist->GetCellContent(ixo, iyo);
				}
			}
			hr->SetCellContent(ix, iy, cont);
			nfill++;
			binys += fRebinY;
		}
		binx += fRebinX;
	}
	hr->SetEntries(nfill);
	gHpr->ShowHist(hr);
			
}
//______________________________________________________________________

void Rebin2DimDialog::RestoreDefaults()
{
}
//______________________________________________________________________
	
void Rebin2DimDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
	//	TCanvas *canvas = (TCanvas *)obj;
	//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
	//   if (obj) cout  << ", " << canvas->GetName() << ")";
	   cout << endl;
		if ( fRebinX <= 0 || fRebinY <= 0) {
//			cout << "Rebin value must be > 0" << endl;
			return;
		}
	if (bid == fBidRebinX || bid == fBidBinLowX || bid == fBidBinUpX) {
		fNewNbinsX = (fBinUpX - fBinLowX + 1) / fRebinX;
		fDialog->ReloadValues();
	}
	if (bid == fBidRebinY || bid == fBidBinLowY || bid == fBidBinUpY) {
		fNewNbinsY = (fBinUpY - fBinLowY + 1) / fRebinY;
		fDialog->ReloadValues();
	}
/*	if (bid == fBidNewNbinsX) {
		fBinUpX = fBinLowX + fNewNbinsX * fRebinX -1;
		if (fBinUpX > fHist->GetNbinsX() ) {
			cout << "fBinUpX exceeds  hist->GetNbinsX() " << endl;
			fBinUpX = fHist->GetNbinsX();
			fDialog->ReloadValues();
		}
	}
	if (bid == fBidNewNbinsY) {
		fBinUpY = fBinLowY + fNewNbinsY * fRebinY -1;
		if (fBinUpY > fHist->GetNbinsY() ) {
			cout << "fBinUpY exceeds  hist->GetNbinsY() " << endl;
			fBinUpY = fHist->GetNbinsY();
			fDialog->ReloadValues();
		}
	}*/
}
//_______________________________________________________________________

void Rebin2DimDialog::RecursiveRemove(TObject * obj)
{
	if (obj == fCanvas) {
		//     cout << "Set2DimOptDialog: CloseDialog "  << endl;
		CloseDialog();
	}
}
//______________________________________________________________________

void Rebin2DimDialog::CloseDialog()
{
	//   cout << "Set3DimOptDialog::CloseDialog() " << endl;
	gROOT->GetListOfCleanups()->Remove(this);
	if (fDialog) fDialog->CloseWindowExt();
	fRow_lab->Delete();
	delete fRow_lab;
	delete this;
}
//______________________________________________________________________

void Rebin2DimDialog::CloseDown(Int_t /*wid*/)
{
	gROOT->GetListOfCleanups()->Remove(this);
	//   cout << "CloseDown(" << wid<< ")" <<endl;
//	fDialog = NULL;
//	if (wid == -1)
//		SaveDefaults();
}
