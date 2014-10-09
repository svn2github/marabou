//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             genReplay.C
// Purpose:          Generate automatic replay session
// Syntax:           .L genReplay.C
//                   genReplay(      const Char_t * RawFile,
//                                   Int_t FromRun,
//                                   Int_t ToRun,
//                                   Int_t Digits,
//                                   const Char_t * HistoFile,
//                                   const Char_t * ParamFile,
//                                   const Char_t * FileList)
// Arguments:        Char_t * RawFile          -- Raw data file (.root)
//                   Int_t FromRun             -- From run number
//                   Int_t ToRun               -- To run number
//                   Int_t Digits              -- Number of digits
//                   Char_t * HistoFile        -- Histogram file (.root)
//                   Char_t * ParamFile        -- Parameter file (.root or .par)
// Result:           Char_t * FileList         -- file list (.list)
// Description:      Generate automatic replay session
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: MacroBrowserTemplate.C,v 1.6 2007/09/26 07:43:05 Rudolf.Lutter Exp $
// Date:             Wed Oct  8 14:12:37 2014
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                genReplay.C
//                   Title:               Generate automatic replay session
//                   Author:              
//                   KeyWords:            
//                   Width:               800
//                   Aclic:               +
//                   Modify:              yes
//                   GuiPtrMode:          ArgList
//                   UserStart:           off
//                   RcFile:              .genReplay.rc
//                   NofArgs:             7
//                   Arg1.Name:           RawFile
//                   Arg1.Title:          Raw data file (.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Entry
//                   Arg1.NofEntryFields: 1
//                   Arg1.Width:          200
//                   Arg1.Default:        raw$R
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           FromRun
//                   Arg2.Title:          From run number
//                   Arg2.Type:           Int_t
//                   Arg2.NofEntryFields: 1
//                   Arg2.Width:          50
//                   Arg2.Default:        1
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           ToRun
//                   Arg3.Title:          To run number
//                   Arg3.Type:           Int_t
//                   Arg3.NofEntryFields: 1
//                   Arg3.Width:          50
//                   Arg3.Default:        999
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           Digits
//                   Arg4.Title:          Number of digits
//                   Arg4.Type:           Int_t
//                   Arg4.NofEntryFields: 1
//                   Arg4.Width:          50
//                   Arg4.Default:        3
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           HistoFile
//                   Arg5.Title:          Histogram file (.root)
//                   Arg5.Type:           Char_t *
//                   Arg5.NofEntryFields: 1
//                   Arg5.Width:          200
//                   Arg5.Default:        histo$R
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           ParamFile
//                   Arg6.Title:          Parameter file (.root or .par)
//                   Arg6.Type:           Char_t *
//                   Arg6.NofEntryFields: 1
//                   Arg6.Width:          200
//                   Arg6.Default:        none
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           FileList
//                   Arg7.Title:          Output to file list (.list)
//                   Arg7.Type:           Char_t *
//                   Arg7.NofEntryFields: 1
//                   Arg7.Width:          200
//                   Arg7.Default:        replay
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
  		kArgRawFile = 1,
 		kArgFromRun = 2,
 		kArgToRun = 3,
 		kArgDigits = 4,
 		kArgHistoFile = 5,
 		kArgParamFile = 6,
 		kArgFileList = 7
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
void genReplay(const Char_t * RawFile = "raw$R",
               Int_t FromRun = 1,
               Int_t ToRun = 999,
               Int_t Digits = 3,
               const Char_t * HistoFile = "histo$R",
               const Char_t * ParamFile = "none",
               const Char_t * FileList = "replay")
//-CallingSequence
//
{
	if (FromRun < 0 || ToRun < 0 || (ToRun - FromRun) < 0) {
		std::cerr << setred << "genReplay: Wrong from ... to values" << setblack << std::endl;
		return;
	}
	if (Digits <= 0) Digits = 1;
	TString fileList = FileList;
	fileList += ".list";
	std::ofstream flist;
	flist.open(fileList.Data(), ios::out);
	for (Int_t run = FromRun; run <= ToRun; run++) {
		TString rawFile = RawFile;
		rawFile.ReplaceAll("$R", Form("%0*d", Digits, run));
		rawFile += ".root";
		TString histoFile = HistoFile;
		histoFile.ReplaceAll("$R", Form("%0*d", Digits, run));
		histoFile += ".root";
		TString paramFile = ParamFile;
		if (paramFile.CompareTo("none") != 0) paramFile += ".root";
		flist << rawFile << " 0 0 " << paramFile << " " << histoFile << " none" << std::endl;
	}
	flist.close();
	cout << setblue << "[genReplay.C: Generating file " << fileList << " (" << (ToRun - FromRun + 1) << " entries)]" << setblack << endl;
	cout << setblue << "[Use program \"C_analyze\" together with option \"file list\" to process this file]" << setblack << endl;
}
