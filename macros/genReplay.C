//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             genReplay.C
// Purpose:          Generate file list for automatic replay session
// Syntax:           .L genReplay.C
//                   genReplay(Int_t ,
//                                   Int_t RawFileMask,
//                                   const Char_t * HistoFileMask,
//                                   Int_t LimitsParam1,
//                                   Int_t LimitsParam2,
//                                   const Char_t * OutputFile)
// Arguments:        Int_t                     -- 
//                   Char_t * RawFileMask      -- File mask for raw data (use %d as wild card)
//                   Char_t * HistoFileMask    -- File mask for histograms
//                   Int_t limitsParam1        -- Param #1 from ... to (0,0 -> any)
//                   Int_t limitsParam2        -- Param #2 from ... to (-1 -> don't use)
//                   Char_t * OutputFile       -- Output file list to
// Description:      Generate file list for automatic replay session
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: MacroBrowserTemplate.C,v 1.6 2007/09/26 07:43:05 Rudolf.Lutter Exp $
// Date:             Tue Oct 14 12:53:39 2014
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                genReplay.C
//                   Title:               Generate file list for automatic replay session
//                   Author:              
//                   KeyWords:            
//                   Width:               900
//                   Aclic:               +
//                   Modify:              yes
//                   GuiPtrMode:          ArgList
//                   UserStart:           off
//                   RcFile:              .genReplay.rc
//                   NofArgs:             7
//                   Arg1.Name:           RawFileMask
//                   Arg1.Title:          File mask for raw data (use %d as wild card)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Entry
//                   Arg1.NofEntryFields: 1
//                   Arg1.Width:          400
//                   Arg1.Default:        raw%02d.root
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           HistoFileMask
//                   Arg2.Title:          File mask for histograms
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Entry
//                   Arg2.NofEntryFields: 1
//                   Arg2.Width:          400
//                   Arg2.Default:        histo%02d.root
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           LowP1
//                   Arg3.Title:          Param #1 lower limit
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      Entry
//                   Arg3.NofEntryFields: 1
//                   Arg3.Width:          90
//                   Arg3.Default:        0
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           UpP1
//                   Arg4.Title:          Param #1 upper limit
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Entry
//                   Arg4.NofEntryFields: 1
//                   Arg4.Width:          90
//                   Arg4.Default:        1000000
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           LowP2
//                   Arg5.Title:          Param #2 lower limit (-1 -> don't use)
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      Entry
//                   Arg5.NofEntryFields: 1
//                   Arg5.Width:          90
//                   Arg5.Default:        -1
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           UpP2
//                   Arg6.Title:          Param #2 upper limit
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      Entry
//                   Arg6.NofEntryFields: 1
//                   Arg6.Width:          90
//                   Arg6.Default:        -1
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           OutputFile
//                   Arg7.Title:          Output file list to
//                   Arg7.Type:           Char_t *
//                   Arg7.EntryType:      Entry
//                   Arg7.NofEntryFields: 1
//                   Arg7.Width:          300
//                   Arg7.Default:        replay.list
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

//__________________________________________________________[C++ HEADER FILES]
//////////////////////////////////////////////////////////////////////////////
//+IncludeFiles
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TUnixSystem.h"
#include "SetColor.h"
//-IncludeFiles

//______________________________________________________[C++ ENUM DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           E<MacroName>ArgNums
// Format:         kArg<ArgName> = <ArgNumber>
// Description:    Enums to address macro arguments by their numbers
//////////////////////////////////////////////////////////////////////////////
//+ArgNums

enum EGenReplayArgNums {
 		kArgRawFileMask = 1, 
 		kArgHistoFileMask = 2,
 		kArgLowP1 = 3,
 		kArgUpP1 = 4,
 		kArgLowP2 = 5,
 		kArgUpP2 = 6,
 		kArgOutputFile = 7,
 	};
//-ArgNums

//______________________________________________________[C++ ENUM DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           E<MacroName>Enums
// Format:         k<ArgName><ArgState> = <StateValue>
// Description:    Enums defining possible states of radio/check buttons
//////////////////////////////////////////////////////////////////////////////
//+Enums

enum EGenReplayEnums {
 	};
//-Enums

//+CallingSequence
void genReplay(const Char_t * RawFileMask = "raw%02d.root",
               const Char_t * HistoFileMask = "histo%02d.root",
               Int_t LowP1 = 0,
               Int_t UpP1 = 0,
               Int_t LowP2 = -1,
               Int_t UpP2 = -1,
               const Char_t * OutputFile = "replay.list")
//-CallingSequence
//
{
	TString rfm = RawFileMask;
	TObjArray * rawMask = rfm.Tokenize("/");
	Int_t n = rawMask->GetEntriesFast();
	TString rawPath = rfm.BeginsWith("/") ? "/" : "";
	for (Int_t i = 0; i < (n - 1); i++) {
		if (i > 0) rawPath += "/";
		rawPath += ((TObjString *) rawMask->At(i))->GetString().Data();
	}
	rfm = ((TObjString *) rawMask->At(n - 1))->GetString();
	if (!rfm.EndsWith(".root")) rfm += ".root";

	TString hfm = HistoFileMask;
	if (!hfm.EndsWith(".root")) hfm += ".root";
	
	Bool_t p1Only = (LowP2 == -1);
	
	TObjArray * o = rfm.Tokenize("%");
	Int_t nperc;
	if (p1Only) {
		nperc = rfm.BeginsWith("%") ? 1 : 2;
		if (o->GetEntriesFast() != nperc) {
			cout << setred << "genReplay.C: Too many % specs in file mask - " << rfm << " (2nd param unsed!)" << endl;
			return;
		}
	} else {
		nperc = rfm.BeginsWith("%") ? 2 : 3;
		if (o->GetEntriesFast() != nperc) {
			cout << setred << "genReplay.C: Too few % specs in file mask - " << rfm << " (2nd param needed!)" << endl;
			return;
		}
	}
	
	o = hfm.Tokenize("%");
	if (p1Only) {
		nperc = hfm.BeginsWith("%") ? 1 : 2;
		if (o->GetEntriesFast() != nperc) {
			cout << setred << "genReplay.C: Too many % specs in file mask - " << hfm << " (2nd param unsed!)" << endl;
			return;
		}
	} else {
		nperc = hfm.BeginsWith("%") ? 2 : 3;
		if (o->GetEntriesFast() != nperc) {
			cout << setred << "genReplay.C: Too few % specs in file mask - " << hfm << " (2nd param needed!)" << endl;
			return;
		}
	}
	
	ofstream flist;
	TString fileList = OutputFile;
	if (!fileList.EndsWith(".list")) fileList += ".list";
	flist.open(fileList.Data(), ios::out);
	
	void * dirp = gSystem->OpenDirectory(rawPath.Data());
	if (dirp) {
		Char_t * fileName;
		Int_t p1, p2;
		Int_t nofFiles = 0;
		while ((fileName = (Char_t *) gSystem->GetDirEntry(dirp)) != NULL) {
			p1 = -1;
			p2 = -1;
			if (p1Only) {
				sscanf(fileName, rfm.Data(), &p1);
				if (p1 >= LowP1 && p1 <= UpP1) {
					TString histoFile = Form(hfm.Data(), p1);
					flist << fileName << " 0 0 none " << histoFile << " none" << endl;
					nofFiles++;
				}
			} else {
				sscanf(fileName, rfm.Data(), &p1, &p2);
				if (p1 >= LowP1 && p1 <= UpP1 && p2 >= LowP2 && p2 <= UpP2) {
					TString histoFile = Form(hfm.Data(), p1, p2);
					flist << fileName << " 0 0 none " << histoFile << " none" << endl;
					nofFiles++;
				}
			}
		}
		flist.close();
		cout << setblue << "[genReplay.C: " << nofFiles << " line(s) written to file " << fileList << "]" << setblack << endl;
	} else {
		cerr << setred << "genReplay.C: Can't open directory - " << rawPath << setblack << endl;
	}
}
