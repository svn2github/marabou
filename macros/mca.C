//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         mca.C
// @(#)Purpose:      Accumulate MCA histogram
// Syntax:           .x mca.C(const Char_t * Module,
//                             Int_t AccuTime,
//                             const Char_t * TimeScale,
//                             Int_t GoodChannels,
//                             Int_t TriggerMask,
//                             Int_t TriggerPolarity,
//                             Bool_t BaseLine)
// Arguments:        Char_t * Module           -- Module name (dgfN, N=Camac Station)
//                   Int_t AccuTime            -- Accu time
//                   Char_t * TimeScale        -- Time scale
//                   Int_t GoodChannels        -- Good channels
//                   Int_t TriggerMask         -- Trigger channels
//                   Int_t TriggerPolarity     -- Trigger polarity
//                   Bool_t BaseLine           -- Accu baseline
// Description:      Accumulate MCA histogram
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Tue Feb 20 15:49:09 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                mca.C
//                   Title:               Accumulate MCA histogram
//                   KeyWords:            DGF               
//                   Width:               
//                   NofArgs:             7
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
//                   Arg2.Name:           AccuTime
//                   Arg2.Title:          Accu time
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      UpDown
//                   Arg2.Default:        10
//                   Arg2.AddLofValues:   No
//                   Arg2.Increment:      1
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           TimeScale
//                   Arg3.Title:          Time scale
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      Radio
//                   Arg3.Default:        secs
//                   Arg3.Values:         hours:mins:secs
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           GoodChannels
//                   Arg4.Title:          Good channels
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Check
//                   Arg4.Default:        0
//                   Arg4.Values:         0:1:2:3
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           TriggerMask
//                   Arg5.Title:          Trigger channels
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      Check
//                   Arg5.Default:        0
//                   Arg5.Values:         0:1:2:3
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           TriggerPolarity
//                   Arg6.Title:          Trigger polarity
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      Radio
//                   Arg6.Default:        neg
//                   Arg6.Values:         neg:pos
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           BaseLine
//                   Arg7.Title:          Accu baseline
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.Default:        No
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include "SetColor.h"

void mca(const Char_t * Module = "<TMrbDGF.LastSelected",
         Int_t AccuTime = 10,
         const Char_t * TimeScale = "secs",
         Int_t GoodChannels = 1,
         Int_t TriggerMask = 1,
         Int_t TriggerPolarity = neg,
         Bool_t BaseLine = kFALSE)
//>>_________________________________________________(do not change this line)
{
	TMrbDGF * dgf;

	dgf = (TMrbDGF *) gROOT->FindObject(Module);
	if (dgf == NULL) {
		cerr	<< setred
				<< "mca.C: Can't locate DGF-4C (\"" << Module << "\") - not defined?"
				<< setblack << endl;
		return;
	}

	dgf->ClearChannelMask();
	dgf->SetChannelMask(GoodChannels);
	dgf->ClearTriggerMask();
	dgf->SetTriggerMask(TriggerMask, TriggerPolarity);

	TMrbDGFHistogramBuffer hbuf;
	hbuf.SetModule(dgf);
	if (dgf->AccuHistograms(AccuTime, TimeScale, BaseLine)) {
		Int_t n = dgf->ReadHistogramBuffer(hbuf);
		if (n > 0) {
			hbuf.Print();
			hbuf.Save();
		}
	}
	gEnv->SetValue("TMrbDGF.LastSelected", Module, kEnvChange);
}
