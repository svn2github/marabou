//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         root2asc.C
// @(#)Purpose:      ROOT to ASCII conversion
// Syntax:           .x root2asc.C(const Char_t * FileNHisto)
// Arguments:        Char_t * FileNHisto       -- Filename (*.root) + Histogram
// Description:      ROOT to ASCII conversion
// @(#)Author:       R. Lutter
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         20-Aug-2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                root2asc.C
//                   Title:               ROOT to ASCII conversion
//                   Width:               
//                   NofArgs:             1
//                   Arg1.Name:           FileNHisto
//                   Arg1.Title:          File (*.root) & Histogram
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      FObjCombo
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

void root2asc(const Char_t * FileNHisto)
//>>_________________________________________________(do not change this line)
//
{
	TMrbSystem ux;
	TString datFile, rootFile;
	TString histName;
	TString line;
	Int_t nofData;
	ofstream asc;
	
	gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 
	gROOT->Macro("LoadColors.C");

	if (FileNHisto == NULL) {
		cerr	<< setred
				<< "root2asc.C: File name and/or object name missing"
				<< setblack << endl;
		return;
	}

	rootFile = FileNHisto;
	Int_t idx = rootFile.Index(":");
	if (idx == -1) {
		cerr	<< setred
				<< "root2asc.C: Syntax error - " << FileNHisto << " (should be \"file:histogram\")"
				<< setblack << endl;
		return;
	}
	histName = rootFile(idx + 1, rootFile.Length());
		
	rootFile.Resize(idx);
	Int_t idx = rootFile.Index(".root", 0);
	if (idx == -1) {
		cerr	<< setred
				<< "root2asc.C: Not a ROOT file - " << FileName
				<< setblack << endl;
		return;
	}

	TFile * root = new TFile(rootFile.Data());
	if (!root.IsOpen()) {
		cerr	<< setred
				<< "root2asc.C: Can't open ROOT file - " << rootFile
				<< setblack << endl;
		return;
	}
	
	TObject * obj = root->Get(histName.Data());
	if (obj == NULL) {
		cerr	<< setred
				<< "root2asc.C:" << rootFile << ": No such ROOT object - " << histName
				<< setblack << endl;
		return;
	}
	if (!obj->InheritsFrom(TH1::Class())) {
		cerr	<< setred
				<< "root2asc.C:" << rootFile << ": Not a histogram (class TH1) - " << histName
				<< setblack << endl;
		return;
	}
	
	ux.GetBaseName(datFile, rootFile.Data());
	idx = datFile.Index(".root", 0);
	datFile.Resize(idx);
	datFile += ".";
	datFile += histName;
	datFile += ".dat";
	
	asc.open(datFile.Data(), ios::out);
	if (!asc.good()) {
		cerr    << setred
				<< "root2asc.C: " << gSystem->GetError() << " - " << datFile
				<< setblack << endl;
		return;
	}

	hist = (TH1 *) obj;
	Int_t dim = hist->GetDimension();
	Int_t nchans = 0;
	if (dim == 1) {
		Int_t binx = hist->GetNbinsX();
		for (Int_t ix = 0; ix < binx; ix++) {
			asc << setw(5) << ix << setw(10) << hist->GetBinContent(ix) << endl;
			nchans++;
		}
	} else if (dim == 2) {
		Int_t binx = hist->GetNbinsX();
		Int_t biny = hist->GetNbinsY();
		for (Int_t ix = 0; ix < binx; ix++) {
			for (Int_t iy = 0; iy < biny; iy++) {
				asc << setw(5) << ix << setw(5) << iy << setw(10) << hist->GetCellContent(ix, iy) << endl;
				nchans++;
			}
		}
	}
	asc.close();
	
	cout	<< setblue
			<< "root2asc.C: File " << rootFile << " (histogram " << histName << ") converted to " << datFile << " (" << nchans << " channels)"
			<< setblack << endl;
}
