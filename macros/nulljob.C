//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         nulljob.C
// @(#)Purpose:      Show nulljob on dataway display
// Syntax:           .x nulljob.C(const Char_t * Host,
//                                 Int_t Crate,
//                                 Int_t Station,
//                                 Int_t Pattern,
//                                 Int_t Hold)
// Arguments:        Char_t * Host             -- Host ppc-N
//                   Int_t Crate               -- Crate Cx
//                   Int_t Station             -- Station Nxx
//                   Int_t Pattern             -- Bit pattern (hex)
//                   Int_t Hold                -- Hold (msecs)
// Description:      Show nulljob on dataway display
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Jan 10 13:08:03 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                nulljob.C
//                   Title:               Show nulljob on dataway display
//                   Width:               650
//                   Aclic:               none
//                   NofArgs:             5
//                   Arg1.Name:           HostName
//                   Arg1.Title:          Host name
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Combo
//                   Arg1.Width:          100
//                   Arg1.Default:        ppc-0
//                   Arg1.Values:         ppc-0:ppc-1:ppc-2:ppc-3:ppc-4:ppc-5:ppc-6:ppc-7:ppc-8
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Crate
//                   Arg2.Title:          Crate Cx
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      UpDown
//                   Arg2.Default:        1
//                   Arg2.LowerLimit:     0
//                   Arg2.UpperLimit:     7
//                   Arg2.Increment:      1
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Station
//                   Arg3.Title:          Station Nxx
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      UpDown
//                   Arg3.Default:        1
//                   Arg3.LowerLimit:     1
//                   Arg3.UpperLimit:     22
//                   Arg3.Increment:      1
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           Pattern
//                   Arg4.Title:          Bit pattern (hex)
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Entry
//                   Arg4.Width:          100
//                   Arg4.Default:        1
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Hold
//                   Arg5.Title:          Hold (msecs)
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      UpDown
//                   Arg5.Default:        500
//                   Arg5.LowerLimit:     100
//                   Arg5.UpperLimit:     10000
//                   Arg5.Increment:      100
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>

void nulljob(const Char_t * Host = "ppc-0",
             Int_t Crate = 1,
             Int_t Station = 1,
             Int_t Pattern = 1,
             Int_t Hold = 500)
//>>__________________________________________________(do not change this line)
//
{
    gROOT->Macro("LoadEsoneLibs.C");
	TMrbEsone * esone = new TMrbEsone();
	esone->ConnectToHost(Host);
	esone->SetVerboseMode(kFALSE);
	Bool_t up = kTRUE;
	UInt_t patt = Pattern;
	for (;;) {
		esone->ExecCnaf(Crate, Station, 0, 16, patt, kFALSE);
		if (patt & 1) up = kTRUE;
		else if (patt & 0x800000) up = kFALSE;
		if (up) patt <<= 1; else patt >>= 1;
		gSystem->ProcessEvents();
		gSystem->Sleep(Hold);
	}
}
