//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         dgfCalcTau.C
// @(#)Purpose:      Calculate best tau value
// Syntax:           .x dgfCalcTau.C(const Char_t * Module,
//                             Int_t TriggerChannel,
//                             const Char_t * TriggerPolarity,
//                             Int_t NofTraces,
//                             Int_t TraceLength,
//                             Double_t A0,
//                             Double_t A1,
//                             Int_t From,
//                             Int_t To,
//                             Double_t ChiSquare)
// Arguments:        Char_t * Module           -- Module name (dgfN, N=Camac Station)
//                   Int_t TriggerChannel      -- Trigger Channel
//                   Char_t * TriggerPolarity  -- Trigger polarity
//                   Int_t NofTraces           -- Number of traces
//                   Int_t TraceLength         -- Trace length
//                   Double_t A0               -- Fit param A0
//                   Double_t A1               -- Fit param A1
//                   Int_t From                -- Fit region (lower)
//                   Int_t To                  -- Fit region (upper)
//                   Double_t ChiSquare        -- Chi square limit
// Description:      Calculate best tau value
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Feb 20 16:21:28 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                dgfCalcTau.C
//                   Title:               Calculate best tau value
//                   Width:               
//                   NofArgs:             10
//                   Arg1.Name:           Module
//                   Arg1.Title:          Module name (dgfN, N=Camac Station)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Combo
//                   Arg1.Width:          100
//                   Arg1.Default:        <TMrbDGF.LastSelected
//                   Arg1.Values:         dgf1:dgf3:dgf5:dgf7:dgf9:dgf11:dgf13:dgf15:dgf17:dgf19:dgf21:dgf23
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           TriggerChannel
//                   Arg2.Title:          Trigger Channel
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        0
//                   Arg2.Values:         0:1:2:3
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           TriggerPolarity
//                   Arg3.Title:          Trigger polarity
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      Radio
//                   Arg3.Default:        neg
//                   Arg3.Values:         neg:pos
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           NofTraces
//                   Arg4.Title:          Number of traces
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      UpDown
//                   Arg4.Default:        10
//                   Arg4.Increment:      1
//                   Arg5.Name:           TraceLength
//                   Arg5.Title:          Trace length
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      UpDown
//                   Arg5.Default:        200
//                   Arg5.Increment:      100
//                   Arg6.Name:           A0
//                   Arg6.Title:          Fit param A0
//                   Arg6.Type:           Double_t
//                   Arg6.EntryType:      UpDown
//                   Arg6.Default:        100
//                   Arg6.LowerLimit:     0.0
//                   Arg6.UpperLimit:     400.0
//                   Arg6.Increment:      10
//                   Arg7.Name:           A1
//                   Arg7.Title:          Fit param A1
//                   Arg7.Type:           Double_t
//                   Arg7.EntryType:      UpDown
//                   Arg7.Default:        -0.01
//                   Arg7.LowerLimit:     -10.0
//                   Arg7.UpperLimit:     10.0
//                   Arg7.Increment:      0.01
//                   Arg8.Name:           From
//                   Arg8.Title:          Fit region (lower)
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      UpDown
//                   Arg8.Default:        85
//                   Arg8.Increment:      5
//                   Arg9.Name:           To
//                   Arg9.Title:          Fit region (upper)
//                   Arg9.Type:           Int_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        350
//                   Arg9.Increment:      5
//                   Arg10.Name:          ChiSquare
//                   Arg10.Title:         Chi square limit
//                   Arg10.Type:          Double_t
//                   Arg10.EntryType:     UpDown
//                   Arg10.Default:       5.0
//                   Arg10.LowerLimit:    1
//                   Arg10.UpperLimit:    10
//                   Arg10.Increment:     1
//-Exec
//////////////////////////////////////////////////////////////////////////////


void dgfCalcTau(const Char_t * Module = "<TMrbDGF.LastSelected",
         Int_t TriggerChannel = 0,
         const Char_t * TriggerPolarity = "neg",
         Int_t NofTraces = 10,
         Int_t TraceLength = 200,
         Double_t A0 = 100,
         Double_t A1 = -0.01,
         Int_t From = 85,
         Int_t To = 350,
         Double_t ChiSquare = 5.0)
//>>_________________________________________________(do not change this line)
{
	TMrbDGF * dgf;

	gROOT->Macro("LoadColors.C");

	dgf = (TMrbDGF *) gROOT->FindObject(Module);
	if (dgf == NULL) {
		cerr	<< setred
				<< "dgfCalcTau.C: Can't locate DGF-4C (\"" << Module << "\") - not defined?"
				<< setblack << endl;
		return;
	}

	dgf->ClearChannelMask();
	dgf->SetGoodChannel(GoodChannels);
	dgf->ClearTriggerMask();
	dgf->SetTriggerBit(TriggerChannel, TriggerPolarity);

	TCanvas * c = gROOT->FindObject("c1");
	if (c != NULL) delete c;

	dgf->TauFit(TriggerChannel, NofTraces, TraceLength, A0, A1, From, To, 0, ChiSquare);
	gEnv->SetValue("TMrbDGF.LastSelected", Module, kEnvChange);
}	
