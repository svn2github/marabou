//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         srim2root.C
// @(#)Purpose:      SRIM to ROOT conversion
// Syntax:           .x srim2root.C(const Char_t * SrimFile,
//                                   Double_t Xmin,
//                                   Double_t Xmax,
//                                   Double_t Ymin,
//                                   Double_t Ymax,
//                                   Double_t Zmin,
//                                   Double_t Zmax,
//                                   Int_t Number of bins,
//                                   Bool_t DrawIt)
// Arguments:        Char_t * SrimFile         -- SRIM file (*.txt, *.list)
//                   Double_t Xmin             -- Xmin
//                   Double_t Xmax             -- Xmax
//                   Double_t Ymin             -- Ymin
//                   Double_t Ymax             -- Ymax
//                   Double_t Zmin             -- Zmin
//                   Double_t Zmax             -- Zmax
//                   Int_t Number of bins      -- NofBins
//                   Bool_t DrawIt             -- Display
// Description:      SRIM to ROOT conversion
// @(#)Author:       rudi
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Mon Jun 20 12:47:31 2005
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                srim2root.C
//                   Title:               SRIM to ROOT conversion
//                   Width:               
//                   NofArgs:             9
//                   Arg1.Name:           SrimFile
//                   Arg1.Title:          SRIM file (*.txt, *.list)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         SRIM files:*.txt:List of SRIM files:*.list
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Xmin
//                   Arg2.Title:          Xmin
//                   Arg2.Type:           Double_t
//                   Arg2.EntryType:      Entry
//                   Arg2.Default:        0.0
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Xmax
//                   Arg3.Title:          Xmax
//                   Arg3.Type:           Double_t
//                   Arg3.EntryType:      Entry
//                   Arg3.Default:        0.0
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           Ymin
//                   Arg4.Title:          Ymin
//                   Arg4.Type:           Double_t
//                   Arg4.EntryType:      Entry
//                   Arg4.Default:        0.0
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Ymax
//                   Arg5.Title:          Ymax
//                   Arg5.Type:           Double_t
//                   Arg5.EntryType:      Entry
//                   Arg5.Default:        0.0
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           Zmin
//                   Arg6.Title:          Zmin
//                   Arg6.Type:           Double_t
//                   Arg6.EntryType:      Entry
//                   Arg6.Default:        0.0
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           Zmax
//                   Arg7.Title:          Zmax
//                   Arg7.Type:           Double_t
//                   Arg7.EntryType:      Entry
//                   Arg7.Default:        0.0
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           NofBins
//                   Arg8.Title:          Number of bins
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      Entry
//                   Arg8.Default:        0
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           DrawIt
//                   Arg9.Title:          Display
//                   Arg9.Type:           Bool_t
//                   Arg9.EntryType:      YesNo
//                   Arg9.Default:        yes
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

Bool_t isLoaded = kFALSE;

void srim2root(const Char_t * SrimFile = NULL,
               Double_t Xmin = 0.0,
               Double_t Xmax = 0.0,
               Double_t Ymin = 0.0,
               Double_t Ymax = 0.0,
               Double_t Zmin = 0.0,
               Double_t Zmax = 0.0,
               Int_t NofBins = 0,
               Bool_t DrawIt = kFALSE)
//>>_________________________________________________(do not change this line)
//
{
	if (!isLoaded) {
		gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 
		gROOT->Macro("LoadColors.C");
		isLoaded = kTRUE;
	}

	if (SrimFile == NULL) {
		cerr	<< setred
				<< "srim2root.C: File name missing"
				<< setblack << endl;
		return;
	}

	ifstream srim;
	TMrbString line;
	TMrbSystem ux;

	Int_t nofFiles = 0;
	TString fileList = SrimFile;
	if (fileList.Index(".list", 0) > 0) {
		srim.open(fileList.Data(), ios::in);
		if (!srim.good()) {
			cerr    << setred
					<< "srim2root: "
					<< gSystem->GetError() << " - " << fileList
					<< setblack << endl;
			return;
		}
		while (!srim.eof()) {
			line.ReadLine(asc, kFALSE);
			line = line.Strip(TString::kBoth);
			if (line.Length() > 0 && line(0) != '#') {
				fList.Add(new TObjString(line.Data()));
				nofFiles++;
			}
		}
		srim.close();
		if (nofFiles == 0) {
			cerr    << setred
					<< "srim2root: " << fileList << " - No files found"
					<< setblack << endl;
			return;
		} else {
			for (Int_t i = 0; i < nofFiles; i++) {
				srim2root(((TObjString *) fList[i])->GetString(), Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, NofBins);
			}
			cout	<< setblue
					<< "srim2root: [" << ux.GetBaseName(fileList, fileList.Data()) << "]- "
					<< nofFiles << " file(s) converted from ASCII to ROOT"
					<< setblack << endl;
			return;
		}
	}

	TString srimFile = SrimFile;
	Int_t idx = srimFile.Index(".txt", 0);
	if (idx == -1) {
		cerr	<< setred
				<< "srim2root.C: Not an SRIM file - " << FileName
				<< setblack << endl;
		return;
	}

	TString rootFile = srimFile;
	rootFile.Resize(idx);
	
	TString histoName;
	ux.GetBaseName(histoName, rootFile.Data());
	ux.GetBaseName(rootFile, rootFile.Data());
	rootFile += ".root";

	TFile * root = new TFile(rootFile.Data(), "RECREATE");
	if (root->IsZombie()) {
		cerr	<< setred
				<< "srim2root: Can't open root file - " << rootFile
				<< setblack << endl;
		return;
	}		
	
	srim.open(srimFile.Data(), ios::in);
	if (!srim.good()) {
		cerr    << setred
				<< "srim2root: "
				<< gSystem->GetError() << " - " << srimFile
				<< setblack << endl;
		return;
	}

	TString hNameX = histoName; hNameX += "-x";
	TH1F * hx = new TH1F(hNameX.Data(), "Depth X", NofBins, Xmin, Xmax);
	TString hNameY = histoName; hNameY += "-y";
	TH1F * hy = new TH1F(hNameY.Data(), "Lateral Y", NofBins, Ymin, Ymax);
	TString hNameZ = histoName; hNameZ += "-z";
	TH1F * hz = new TH1F(hNameZ.Data(), "Lateral Z", NofBins, Zmin, Zmax);

	TCanvas * c = NULL;

	Int_t nofData = 0;
	Int_t lineNo = 0;
	Bool_t isHeader = kTRUE;
	while (!srim.eof()) {
		gSystem->ProcessEvents();
		line.ReadLine(srim, kFALSE);
		lineNo++;
		line = line.Strip(TString::kBoth);
		if (line.BeginsWith("------")) {
			if (!isHeader) {
				cerr	<< setred
						<< "srim2root [" << srimFile << "]: Format error (header mismatch) in line " << lineNo
						<< setblack << endl;
				srim.close();
				root->Write();
				root->Close();
				return;
			}
			isHeader = kFALSE;
			continue;
		}
		if (isHeader) continue;
		if (line.Length() > 0 && line(0) != '#') {
			nofData++;
			if ((nofData % 1000) == 0) cout << "." << ends << flush;
			TObjArray column;
			Int_t nCols = line.Split(column, " ", kTRUE);
			if (nCols != 4)	{		
				cerr	<< setred
						<< "srim2root [" << srimFile << "]: Format error (short line) in line " << lineNo << endl
						<< setgreen << "Line: \"" << setblue << line << setgreen << "\"" << setblack << endl;
				srim.close();
				root->Write();
				root->Close();
				return;
			}
			Double_t x = strtod(((TObjString *) column[1])->GetString().Data(), NULL);
			x *= 1e-10;
			hx->Fill(x);
			Double_t y = strtod(((TObjString *) column[2])->GetString().Data(), NULL);
			y *= 1e-10;
			hy->Fill(y);
			Double_t z = strtod(((TObjString *) column[3])->GetString().Data(), NULL);
			z *= 1e-10;
			hz->Fill(z);

			if ((nofData % 10000) == 0 && DrawIt) {
				if (c == NULL) {
					c = new TCanvas("srim");
					c->Divide(1, 3);
				}
				c->cd(1);
				hx->Draw();
				c->cd(2);
				hy->Draw();
				c->cd(3);
				hz->Draw();
				c->Update();
			}
		}
	}
	srim.close();
	root->Write();
	root->Close();
	cout << endl << setblue << "[" << rootFile << ": " << nofData << " data points]" << setblack << endl;
}
