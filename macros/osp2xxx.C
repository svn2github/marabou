//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         osp2xxx.C
// @(#)Purpose:      OSP to ROOT/ASCII conversion
// Syntax:           .x osp2xxx.C(const Char_t * FileName,
//                                 Int_t ToRoot)
// Arguments:        Char_t * FileName         -- Filename
//                   Int_t ToRoot              -- Convert to
// Description:      OSP to ROOT/ASCII conversion
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Mar 20 12:55:36 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                osp2xxx.C
//                   Title:               OSP to ROOT/ASCII conversion
//                   Width:               
//                   NofArgs:             2
//                   Arg1.Name:           FileName
//                   Arg1.Title:          Filename (*.osp)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      File
//                   Arg1.Values:         OSP Files:*.osp
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           ToAscii
//                   Arg2.Title:          Convert to
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        ROOT
//                   Arg2.Values:         ROOT=0:ASCII=1
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

#include "SetColor.h"

void osp2xxx(const Char_t * FileName, Int_t ToAscii)
//>>_________________________________________________(do not change this line)
//
{
	TString ospFile, rootFile, datFile, cmd;

    gROOT->Macro("LoadUtilityLibs.C"); 	// load utility libs 

	TString progName = ToAscii ? "osp2ascii" : "osp2root";

	if (FileName == NULL) {
		cerr	<< setred
				<< progName << ": File name missing"
				<< setblack << endl;
		return;
	}

	ospFile = FileName;
	Int_t x = ospFile.Index(".osp", 0);
	if (x == -1) {
		cerr	<< setred
				<< progName << ": Not an osp file - " << FileName
				<< setblack << endl;
		return;
	}

	ospFile.Resize(x);		// strip off .osp
	datFile = ospFile;
	rootFile = ospFile;
	ospFile += ".osp";		// append osp ext
	datFile += ".dat";		// dat file
	rootFile += ".root";	// root file
	gSystem->Unlink("tmp.hbook");				// remove temp files
	gSystem->Unlink("tmp.root");				// ...
	cmd = "mlle2hbook tmp.hbook ";				// build mlle2hbook command
	cmd += ospFile;
	gSystem->Exec(cmd.Data());					// "mlle2hbook tmp.hbook ospFile": convert osp to hbook
	gSystem->Exec("h2root tmp.hbook");			// "h2root tmp.hbook": convert hbook to root

	TFile * f = new TFile("tmp.root");		 	// open root file containing histogram
	TH1F * h = (TH1F *) f->Get("h1");			// histogram name is h1 (created by h2root by default)
	Int_t nofChannels = h->GetEntries();		// read number of channels
	cout	<< setblue
			<< progName << ": File " << ospFile << ": " << nofChannels << " channels"
			<< setblack << endl;

	if (ToAscii) {
		ofstream dat;
		dat.open(datFile.Data(), ios::out); 		// try to open
		if (!dat.good()) {
			cerr	<< setred
					<< progName << ": Can't open file " << datFile
					<< setblack << endl;
			return;
		}
		for (Int_t i = 0; i < nofChannels; i++) {	// read channel by channel
			dat << i << "		" << h->GetBinContent(i + 1) << endl;	// output x, y
		}
		dat.close();								// close file
		cout	<< setblue
				<< progName << ": osp data converted to ASCII - " << datFile
				<< setblack << endl;
		gSystem->Unlink("tmp.hbook");				// remove temp files
		gSystem->Unlink("tmp.root");				// ...
	} else {
		cmd = "mv tmp.root ";
		cmd += rootFile;
		gSystem->Exec(cmd.Data());
		cout	<< setblue
				<< progName << ": osp data converted to ROOT - " << rootFile
				<< setblack << endl;
		gSystem->Unlink("tmp.hbook");				// remove temp files
	}
}
