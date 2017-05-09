//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         esone.C
// @(#)Purpose:      Connect to ESONE server
// Syntax:           .x esone.C(const Char_t * HostName)
// Arguments:        Char_t * HostName         -- Host name
// Description:      Connect to ESONE server
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Mon Feb 12 13:56:29 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                esone.C
//                   Title:               Connect to ESONE server
//                   KeyWords:            CAMAC:DGF
//                   Width:               
//                   Aclic:               none
//                   NofArgs:             1
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
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>

#include "SetColor.h"

void esone(const Char_t * HostName = "ppc-0")
//>>__________________________________________________(do not change this line)
//
{
	gROOT->Macro("LoadEsoneLibs.C");

	TMrbEsone * cold = new TMrbEsone();
	if (cold->IsZombie()) {
		cerr	<< setred
				<< "esone.C: Can't connect to ESONE server @ " << HostName << ". Sorry."
				<< setblack << endl;
		return;
	} else {
		cout << "[esone.C: Cold start for ESONE server / MBS @ " << HostName << "]" << endl;
		cold->StartServer(HostName);
	}
	return;
}
