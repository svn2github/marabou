//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         asc2root.C
// @(#)Purpose:      ASCII to ROOT conversion
// Syntax:           .x asc2root.C(const Char_t * FileName,
//                                  const Char_t * Format,
//                                  Bool_t XisInt,
//                                  Int_t DrawFlag)
// Arguments:        Char_t * FileName         -- Filename (*.dat, *.list)
//                   Char_t * Format           -- Format
//                   Bool_t XisInt             -- X integer
//                   Int_t DrawFlag            -- Draw it?
// Description:      ASCII to ROOT conversion
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Dec 11 09:00:25 2002
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                asc2root.C
//                   Title:               ASCII to ROOT conversion
//                   Width:               
//                   NofArgs:             4
//                   Arg1.Name:           FileName
//                   Arg1.Title:          Filename (*.dat, *.list)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         DAT Files:*.dat:File lists:*.list
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Format
//                   Arg2.Title:          Format
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        y
//                   Arg2.Values:         y=y only:xy=format (x,y):ye=format (y,err):xye=format (x,y,err):xy[3]=format (x,y=col3):xy[4]=format (x,y=col4):xy[5]=format (x,y=col5):xy[6]=format (x,y=col6)
//                   Arg2.NofCL:          4
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           XisInt
//                   Arg3.Title:          X integer
//                   Arg3.Type:           Bool_t
//                   Arg3.EntryType:      YesNo
//                   Arg3.Default:        Yes
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           DrawFlag
//                   Arg4.Title:          Draw it?
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      YesNo
//                   Arg4.Default:        no
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

Bool_t isLoaded = kFALSE;

void asc2root(const Char_t * FileName,
              const Char_t * Format = "y",
              Bool_t XisInt = kTRUE,
              Int_t DrawFlag = kFALSE)
//>>_________________________________________________(do not change this line)
//
{
	TString datFile, fileList, rootFile, format, graphName;
	TString line;
	ifstream asc;
	Bool_t yOnly;
	Bool_t withErrors;
	Int_t yCol;
	Double_t x, y, e, yc[7];
	Int_t ix;
	TObjArray fList;
	TMrbSystem ux;
		
	if (!isLoaded) {
		gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 
		gROOT->Macro("LoadColors.C");
		isLoaded = kTRUE;
	}

	format = Format;
	yOnly = (format.Index("x", 0) == -1);
	withErrors = (format.Index("e", 0) > 0);
	if (yOnly && XisInt) XisInt = kFALSE;
	if (!yOnly) {
		yCol = 2;
		if (format.Index("3", 0) > 0) yCol = 3;
		else if (format.Index("4", 0) > 0) yCol = 4;
		else if (format.Index("5", 0) > 0) yCol = 5;
		else if (format.Index("6", 0) > 0) yCol = 6;
	}

	if (FileName == NULL) {
		cerr	<< setred
				<< "asc2root.C: File name missing"
				<< setblack << endl;
		return;
	}

	Int_t nofFiles = 0;
	fileList = FileName;
	if (fileList.Index(".list", 0) > 0) {
		asc.open(fileList.Data(), ios::in);
		if (!asc.good()) {
			cerr    << setred
					<< "asc2root: "
					<< gSystem->GetError() << " - " << fileList
					<< setblack << endl;
			return;
		}
		while (!asc.eof()) {
			line.ReadLine(asc, kFALSE);
			line = line.Strip(TString::kBoth);
			if (line.Length() > 0 && line(0) != '#') {
				fList.Add(new TObjString(line.Data()));
				nofFiles++;
			}
		}
		asc.close();
		if (nofFiles == 0) {
			cerr    << setred
					<< "asc2root: " << fileList << " - No files found"
					<< setblack << endl;
			return;
		} else {
			for (Int_t i = 0; i < nofFiles; i++) {
				asc2root(((TObjString *) fList[i])->GetString(), Format, XisInt);
			}
			cout	<< setblue
					<< "asc2root: [" << ux.GetBaseName(fileList, fileList.Data()) << "]- "
					<< nofFiles << " file(s) converted from ASCII to ROOT"
					<< setblack << endl;
			return;
		}
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
	
	TString histoName;
	ux.GetBaseName(histoName, graphName.Data());

	ux.GetBaseName(rootFile, FileName);
	rootFile.ReplaceAll(".dat", ".root");
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
		if (line.Length() > 0 && line(0) != '#') nofData++;
	}
	asc.close();

	TArrayD xarr(nofData);
	TArrayD yarr(nofData);
	TArrayD earr(nofData);
	
	cout << "asc2root: Converting file " << datFile << " - wait ..." << endl;
	asc.open(datFile.Data(), ios::in);
	Int_t n = 0;
	Double_t xmin = 1e20;
	Double_t xmax = -1e20;
	while (!asc.eof()) {
		line.ReadLine(asc, kFALSE);
		line = line.Strip(TString::kBoth);
		istrstream aline(line.Data());
		if (line.Length() > 0 && line(0) != '#') {
			memset(yc, 0, 7 * sizeof(Double_t));
			if (yOnly) {
				xarr[n] = n;
				aline >> yarr[n];
				if (withErrors) aline >> earr[n];
			} else if (XisInt) {
				aline >> ix >> yc[2] >> yc[3] >> yc[4] >> yc[5] >> yc[6];
				x = (Double_t) ix;
				if (x < xmin) xmin = x;
				if (x > xmax) xmax = x;
				yarr[ix] = yc[yCol];
				if (withErrors) earr[ix] = y[3];
			} else {
				aline >> xarr[n] >> yc[2] >> yc[3] >> yc[4] >> yc[5] >> yc[6];
				yarr[n] = yc[yCol];
				if (withErrors) earr[n] = yc[3];
			}
			if (xarr[n] < xmin) xmin = xarr[n];
			else if (xarr[n] > xmax) xmax = xarr[n];
			n++;
		}
	}
	asc.close();
	
	cout	<< setblue
			<< "asc2root: File " << datFile << " converted to " << rootFile << " (" << nofData << " channels)"
			<< setblack << endl;
	if (yOnly || XisInt) {
		TH1F * h = new TH1F(histoName.Data(), datFile.Data(), nofData, xmin, xmax);
		for (Int_t i = 0; i < nofData; i++) h->Fill((Axis_t) i, yarr[i]);
		h->Write();
		if (DrawFlag) h->Draw();
	} else if (withErrors) {
		TGraphErrors * gre = new TGraphErrors(nofData, xarr.GetArray(), yarr.GetArray(), NULL, earr.GetArray());
		gre->SetName(histoName.Data());
		gre->SetTitle(datFile.Data());
		gre->Write();
		cout << setblue << "asc2root: Data stored using \"TGraphErrors\"" << setblack << endl;
		if (DrawFlag) gr->Draw("AL");
	} else {
		TGraph * gr = new TGraph(nofData, xarr.GetArray(), yarr.GetArray());
		gr->SetName(histoName.Data());
		gr->SetTitle(datFile.Data());
		gr->Write();
		cout << setblue << "asc2root: Data stored using \"TGraph\"" << setblack << endl;
		if (DrawFlag) gr->Draw("AL");
	}
	gEnv->SetValue("MacroBrowser.LastRootFile", rootFile.Data());	
	gEnv->SetValue("MacroBrowser.LastFile", rootFile.Data());	
}
