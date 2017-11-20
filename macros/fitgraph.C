//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         fitgraph.C
// @(#)Purpose:      ASCII to ROOT conversion
// Syntax:           .x fitgraph.C(const Char_t * FileName)
// Arguments:        Char_t * FileName         -- Filename (*.dat)
// Description:      Fit TGraph/TGraphErrors objects
// @(#)Author:       R. Lutter
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Apr 24 12:31:37 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                fitgraph.C
//                   Title:               Fit TGraph/TGraphErrors objects
//                   Width:               
//                   NofArgs:             1
//                   Arg1.Name:           FileName
//                   Arg1.Title:          Filename (*.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         ROOT Files:*.root:All files:*
//                   Arg1.Default:        <MacroBrowser.LastRootFile
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "TH1.h"
#include "TGraph.h"
#include "TGraphErrors.h"

#include "SetColor.h"

void fitgraph(const Char_t * FileName)
//>>_________________________________________________(do not change this line)
//
{
	TGraph * gr;
	TGraphErrors *gre;
	TKey * key;
	TObject * obj;
					
	gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 

	TFile * f = new TFile(FileName);
	if (!f->IsOpen()) {
		cerr	<< setred
				<< "fitgraph.C: Can't open file - " << FileName
				<< setblack << endl;
		return;
	}
	key = (TKey *) f->GetListOfKeys()->First();
	while (key) {
		obj = key->ReadObj();
		if (obj) {
			if (obj->InheritsFrom("TGraphErrors")) {
				gre = (TGraphErrors *) obj;
				gre->Fit("gaus");
				gre->Draw("PA");
				return;
			} else if (obj->InheritsFrom("TGraph")) {
				gr = (TGraph *) obj;
				gr->Fit("gaus");
				gr->Draw("PA");
				return;
			}
		}
		key = (TKey *) f->GetListOfKeys()->After(key);
	}
	cerr << setred << "fitgraph.C: No TGraph / TGraphErrors object found in file " << FileName << setblack << endl;
	return;
}
