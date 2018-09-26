//___________________________________________________________[C++ HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           ExaAnalyzeGlobals.h
// Purpose:        Header file to define global pointers
// Description:    Example configuration
//
//                 This file contains most of user-defined pointers
//                 such as pointers to events, histograms, windows etc.
//
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
// URL:            
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TH2.h"

#include "TMrbVar.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"

//_______________________________________________________________[GLOBAL DEFS]
//////////////////////////////////////////////////////////////////////////////
// Purpose:        Define pointers to event classes
//////////////////////////////////////////////////////////////////////////////

static TUsrEvtReadout * gReadout = NULL;	// event readout, trigger 1

//_______________________________________________________________[GLOBAL DEFS]
//////////////////////////////////////////////////////////////////////////////
// Purpose:        Define global instances of user's variables/windows
// Comment:        TMrbVariableX and TMrbVarArrayX are defined as
//                 STATIC objects so they can be addressed DIRECTLY
//                 TMrbWindowX objects are defined by pointers to the heap.
//////////////////////////////////////////////////////////////////////////////

static TMrbVarF v5("v5", 1.235);
static TMrbVarF v6("v6", 1.235);

static TMrbVarArrayI v1("v1", 3, 1000);
static TMrbVarArrayI v2("v2", 4, 1000);
static TMrbVarArrayI v3("v3", 5, 1000);
static TMrbVarArrayF v4("v4", 2, 1.235);

static TMrbWindowI * w1 = NULL;
static TMrbWindowI * w3 = NULL;
static TMrbWindowI * w2 = NULL;

//_______________________________________________________________[GLOBAL DEFS]
//////////////////////////////////////////////////////////////////////////////
// Purpose:        Define pointers to histograms for (sub)events
//////////////////////////////////////////////////////////////////////////////

static TH1F * hEtot = NULL; 	// data.etot
static TH1F * hDe1 = NULL; 	// data.de1
static TH1F * hDe2 = NULL; 	// data.de2
static TH1F * hDe3 = NULL; 	// data.de3
static TH1F * hDe4 = NULL; 	// data.de4
static TH1F * hVeto = NULL; 	// data.veto
static TH1F * hPos1r = NULL; 	// data.pos1r
static TH1F * hPos1l = NULL; 	// data.pos1l
static TH1F * hPos2r = NULL; 	// data.pos2r
static TH1F * hPos2l = NULL; 	// data.pos2l

