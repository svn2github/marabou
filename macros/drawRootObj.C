//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         drawRootObj.C
// @(#)Purpose:      Draw Objects from ROOT file
// Syntax:           .x drawRootObj.C(const Char_t * File)
// Arguments:        Char_t * File             -- File (*.root)
// Description:      Draw Objects from ROOT file
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Dec 11 11:06:57 2002
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                drawRootObj.C
//                   Title:               Draw Objects from ROOT file
//                   Width:               
//                   NofArgs:             1
//                   Arg1.Name:           File
//                   Arg1.Title:          File (*.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      FObjCombo
//                   Arg1.Values:         *.root:ROOT files
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

void drawRootObj(const Char_t * File)
//>>_________________________________________________(do not change this line)
//
{
	TMrbString fileName = File;
	TObjArray fo;
	fileName.Split(fo);
	fileName = ((TObjString *) fo[0])->GetString();
	TString objName = ((TObjString *) fo[1])->GetString();
	TFile * f = new TFile(fileName.Data());
	if (!f->IsOpen()) {
		cerr	<< setred
				<< "drawRootObj.C: Not a ROOT file - " << fileName
				<< setblack << endl;
	} else {
		TObject * obj = f->Get(objName.Data());
		if (obj == NULL) {
			cerr	<< setred
					<< "drawRootObj.C [" << fileName << "]: No such ROOT object - " << objName
					<< setblack << endl;
		} else {
			if (obj.InheritsFrom("TH1")) ((TH1 *) obj)->Draw();
			else if (obj.InheritsFrom("TGraph")) ((TGraph *) obj)->Draw("AL");
			else {
				cerr	<< setred
						<< "drawRootObj.C [" << fileName << "]: object not drawable - " << objName
						<< setblack << endl;
			}
		}
	}
}
