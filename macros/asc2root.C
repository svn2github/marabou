//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         asc2root.C
// @(#)Purpose:      ASCII to ROOT conversion
// Syntax:           .x asc2root.C(const Char_t * FileName,
//                                  const Char_t * Format)
// Arguments:        Char_t * FileName         -- Filename (*.dat)
//                   Char_t * Format           -- Format
// Description:      ASCII to ROOT conversion
// @(#)Author:       R. Lutter
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Apr 24 12:31:37 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                asc2root.C
//                   Title:               ASCII to ROOT conversion
//                   Width:               
//                   NofArgs:             2
//                   Arg1.Name:           FileName
//                   Arg1.Title:          Filename (*.dat)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         DAT Files:*.dat:All files:*
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Format
//                   Arg2.Title:          Format
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        y
//                   Arg2.Values:         y:xy:ye:xye
//                   Arg2.AddLofValues:   No
//                   Arg2.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>
#include "TH1.h"
#include "TGraph.h"
#include "TGraphErrors.h"

void asc2root(const Char_t * FileName,
              const Char_t * Format = "y")
//>>_________________________________________________(do not change this line)
//
{
	TString datFile, rootFile, format, graphName;
	TString line;
	ifstream asc;
	Bool_t yOnly;
	Bool_t withErrors;
	Double_t x, y, e;
		
	gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 
	gROOT->Macro("LoadColors.C");

	format = Format;
	yOnly = (format.Index("x", 0) == -1);
	withErrors = (format.Index("e", 0) > 0);

	if (FileName == NULL) {
		cerr	<< setred
				<< "asc2root.C: File name missing"
				<< setblack << endl;
		return;
	}

	datFile = FileName;
	Int_t idx = datFile.Index(".dat", 0);
	if (idx == -1) {
		cerr	<< setred
				<< "asc2root.C: Not an dat file - " << FileName
				<< setblack << endl;
		return;
	}

	datFile.Resize(idx);		// strip off .dat
	graphName = datFile;
	rootFile = datFile;
	rootFile += ".root";		// root file
	datFile += ".dat";
	
	TFile * root = new TFile(rootFile.Data(), "RECREATE");
	if (root->IsZombie()) {
		cerr	<< setread
				<< "asc2root: Can't open root file - " << rootFile
				<< setblack << endl;
		return;
	}		
	
	asc.open(datFile.Data(), ios::in);
	if (!asc.good()) {
		cerr    << setred
				<< "asc2root: "
				<< gSystem->GetError() << " - " << datFile
				<< setblack << endl;
		return;
	}

	Int_t nofData = 0;
	while (!asc.eof()) {
		line.ReadLine(asc, kFALSE);
		line = line.Strip(TString::kBoth);
		istrstream aline(line.Data());
		if (line.Length() > 0 && line(0) != '#') nofData++;
	}
	asc.close();

	TArrayD xarr(nofData);
	TArrayD yarr(nofData);
	TArrayD earr(nofData);
	
	asc.open(datFile.Data(), ios::in);
	Int_t n = 0;
	Double_t xmin = 1e20;
	Double_t xmax = -1e20;
	while (!asc.eof()) {
		line.ReadLine(asc, kFALSE);
		line = line.Strip(TString::kBoth);
		istrstream aline(line.Data());
		if (line.Length() > 0 && line(0) != '#') {
			if (yOnly) {
				xarr[n] = n;
				aline >> yarr[n];
			} else {
				aline >> xarr[n] >> yarr[n];
			}
			if (withErrors) aline >> earr[n];
			if (xarr[n] < xmin) xmin = xarr[n];
			else if (xarr[n] > xmax) xmax = xarr[n];
			n++;
		}
	}
	asc.close();
	
	cout	<< setblue
			<< "asc2root: File " << datFile << " converted to " << rootFile << " (" << nofData << " channels)"
			<< endl;
	if (yOnly) {
		TH1F * h = new TH1F(graphName.Data(), datFile.Data(), nofData, xmin, xmax);
		for (Int_t i = 0; i < nofData; i++) h->Fill((Axis_t) i, yarr[i]);
		h->Write();
		cout << "asc2root: Data stored using \"TH1F\"" << setblack << endl;
	} else if (withErrors) {
		TGraphErrors * gre = new TGraphErrors(nofData, xarr.GetArray(), yarr.GetArray(), NULL, earr.GetArray());
		gre->SetName(graphName.Data());
		gre->SetTitle(datFile.Data());
		gre->Write();
		cout << "asc2root: Data stored using \"TGraphErrors\"" << setblack << endl;
	} else {
		TGraph * gr = new TGraph(nofData, xarr.GetArray(), yarr.GetArray());
		gr->SetName(graphName.Data());
		gr->SetTitle(datFile.Data());
		gr->Write();
		cout << "asc2root: Data stored using \"TGraph\"" << setblack << endl;
	}
	gEnv->SetValue("MacroBrowser.LastRootFile", rootFile.Data());	
	gEnv->SetValue("MacroBrowser.LastFile", rootFile.Data());	
	root->Close();
}
