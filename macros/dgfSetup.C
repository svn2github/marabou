//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         dgfSetup.C
// @(#)Purpose:      Setup DGF params
// Syntax:           .x dgfSetup.C(const Char_t * Module,
//                               Int_t GoodChannels,
//                               Double_t Gain,
//                               Double_t Offset,
//                               Double_t Delay,
//                               Double_t Tau,
//                               Double_t PeakingTime,
//                               Double_t GapTime,
//                               Double_t FastPeakingTime,
//                               Double_t FastGapTime,
//                               Double_t Threshold)
// Arguments:        Char_t * Module           -- Module name (dgf-N, N=Camac Station)
//                   Int_t GoodChannels        -- Good channels
//                   Double_t Gain             -- Gain (V/V)
//                   Double_t Offset           -- Offset (Volt)
//                   Double_t Delay            -- Delay (usec)
//                   Double_t Tau              -- Tau
//                   Double_t PeakingTime      -- Peaking time (usec)
//                   Double_t GapTime          -- Gap time (usec)
//                   Double_t FastPeakingTime  -- Fast peaking time (usec)
//                   Double_t FastGapTime      -- Fast gap time (usec)
//                   Double_t Threshold        -- Threshold
// Description:      Perform a trace
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Fri Jan 26 09:55:20 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                dgfSetup.C
//                   Title:               Setup DGF params
//                   Width:               
//                   NofArgs:             11
//                   Arg1.Name:           Module
//                   Arg1.Title:          Module name (dgf-N, N=Camac Station)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Combo
//                   Arg1.Width:          100
//                   Arg1.Default:        <TMrbDGF.LastSelected
//                   Arg1.Values:         *ALL*:dgf1:dgf3:dgf5:dgf7:dgf9:dgf11:dgf13:dgf15:dgf17:dgf19:dgf21:dgf23
//                   Arg1.AddLofValues:   Yes
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           GoodChannels
//                   Arg2.Title:          Good channels
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Check
//                   Arg2.Default:        0
//                   Arg2.Values:         0:1:2:3
//                   Arg3.Name:           Gain
//                   Arg3.Title:          Gain (V/V)
//                   Arg3.Type:           Double_t
//                   Arg3.EntryType:      UpDown
//                   Arg3.Default:        2.0
//                   Arg3.LowerLimit:     0.0
//                   Arg3.UpperLimit:     20.0
//                   Arg3.Increment:      .1
//                   Arg4.Name:           Offset
//                   Arg4.Title:          Offset (Volt)
//                   Arg4.Type:           Double_t
//                   Arg4.EntryType:      UpDown
//                   Arg4.Default:        0.286
//                   Arg4.LowerLimit:     -3.0
//                   Arg4.UpperLimit:     3.0
//                   Arg4.Increment:      .1
//                   Arg5.Name:           Delay
//                   Arg5.Title:          Delay (usec)
//                   Arg5.Type:           Double_t
//                   Arg5.EntryType:      UpDown
//                   Arg5.Default:        1.8
//                   Arg5.LowerLimit:     0.0
//                   Arg5.UpperLimit:     100000.0
//                   Arg5.Increment:      .1
//                   Arg6.Name:           Tau
//                   Arg6.Title:          Tau
//                   Arg6.Type:           Double_t
//                   Arg6.EntryType:      UpDown
//                   Arg6.Default:        30.0
//                   Arg6.LowerLimit:     0.0
//                   Arg6.UpperLimit:     1000.0
//                   Arg6.Increment:      .5
//                   Arg7.Name:           PeakingTime
//                   Arg7.Title:          Peaking time (usec)
//                   Arg7.Type:           Double_t
//                   Arg7.EntryType:      UpDown
//                   Arg7.Default:        9.6
//                   Arg7.LowerLimit:     0.0
//                   Arg7.UpperLimit:     1000.0
//                   Arg7.Increment:      .1
//                   Arg8.Name:           GapTime
//                   Arg8.Title:          Gap time (usec)
//                   Arg8.Type:           Double_t
//                   Arg8.EntryType:      UpDown
//                   Arg8.Default:        1.2
//                   Arg8.LowerLimit:     0.0
//                   Arg8.UpperLimit:     1000.0
//                   Arg8.Increment:      .1
//                   Arg9.Name:           FastPeakingTime
//                   Arg9.Title:          Fast peaking time (usec)
//                   Arg9.Type:           Double_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        0.1
//                   Arg9.LowerLimit:     0.0
//                   Arg9.UpperLimit:     1000.0
//                   Arg9.Increment:      .1
//                   Arg10.Name:          FastGapTime
//                   Arg10.Title:         Fast gap time (usec)
//                   Arg10.Type:          Double_t
//                   Arg10.EntryType:     UpDown
//                   Arg10.Default:       0.0
//                   Arg10.LowerLimit:    0.0
//                   Arg10.UpperLimit:    1000.0
//                   Arg10.Increment:     .1
//                   Arg11.Name:          Threshold
//                   Arg11.Title:         Threshold
//                   Arg11.Type:          Double_t
//                   Arg11.EntryType:     UpDown
//                   Arg11.Default:       30.0
//                   Arg11.LowerLimit:    0.0
//                   Arg11.UpperLimit:    1000.0
//                   Arg11.Increment:     .5
//-Exec
//////////////////////////////////////////////////////////////////////////////


void dgfSetup(const Char_t * Module,
           const Char_t * ModuleValues = "*ALL*:dgf1:dgf3:dgf5:dgf7:dgf9:dgf11:dgf13:dgf15:dgf17:dgf19:dgf21:dgf23",
           Int_t GoodChannels = 0,
           Double_t Gain = 2.0,
           Double_t Offset = 0.286,
           Double_t Delay = 1.8,
           Double_t Tau = 30.0,
           Double_t PeakingTime = 9.6,
           Double_t GapTime = 1.2,
           Double_t FastPeakingTime = 0.1,
           Double_t FastGapTime = 0.0,
           Double_t Threshold = 30.0)
//>>_________________________________________________(do not change this line)
{
	TMrbDGF * dgf;
	TMrbLofNamedX dgfNameArray;
	TMrbNamedX * nx;
	TString dgfName;
	Bool_t setupAll;

	gROOT->Macro("LoadColors.C");

	dgfName = Module;
	setupAll = (dgfName.CompareTo("*ALL*") == 0);

	dgfNameArray.AddNamedX(ModuleValues);
	nx = (TMrbNamedX *) dgfNameArray[1];
	while (nx) {
		dgfName = nx->GetName();
		if (setupAll || dgfName.CompareTo(Module) == 0) {
			dgf = (TMrbDGF *) gROOT->FindObject(dgfName.Data());
			if (dgf == NULL) {
				cerr	<< setred
						<< "dgfSetup.C: Can't locate DGF-4C (\"" << dgfName << "\") - not defined?"
						<< setblack << endl;
				return;
			}

			dgf->SetChannelMask(GoodChannels);

// set registers
			for (Int_t chn = 0; chn <= 3; chn++) {
				if (dgf->IsGoodChannel(chn)) {
					dgf->ResetPGTime(chn);
					dgf->SetGain(chn, Gain);
					dgf->SetOffset(chn, Offset);
					dgf->SetDelay(chn, Delay);
					dgf->SetTau(chn, Tau);
					dgf->SetPeakTime(chn, PeakingTime);
					dgf->SetGapTime(chn, GapTime);
					dgf->SetFastPeakTime(chn, FastPeakingTime);
					dgf->SetFastGapTime(chn, FastGapTime);
					dgf->SetThreshold(chn, Threshold);
					dgf->SetBinning(chn, 1, 1, 1);
				}
			}

// calculate corrections
			dgf->CalcCorrections();

// update param memory
			dgf->WriteParamMemory();

			gEnv->SetValue("TMrbDGF.LastSelected", dgfName.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) dgfNameArray.After(nx);
	}
}
