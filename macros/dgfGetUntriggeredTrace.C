//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         dgfGetUntriggeredTrace.C
// @(#)Purpose:      Perform a trace
// Syntax:           .x dgfGetUntriggeredTrace.C(const Char_t * Module,
//                                                const Char_t * TriggerPolarity,
//                                                Int_t Xwait,
//                                                Int_t NofTraces,
//                                                Int_t TraceLength,
//                                                Double_t ConstError,
//                                                Double_t A0,
//                                                Double_t A1,
//                                                Double_t A2,
//                                                Int_t From,
//                                                Int_t To,
//                                                Int_t DisplayTrace,
//                                                Int_t DisplayChannel)
// Arguments:        Char_t * Module           -- Module name (dgfN, N=Camac Station)
//                   Char_t * TriggerPolarity  -- Trigger polarity
//                   Int_t Xwait               -- XWait States
//                   Int_t NofTraces           -- Number of traces
//                   Int_t TraceLength         -- Trace length
//                   Double_t ConstError       -- Constant error
//                   Double_t A0               -- Fit param A0
//                   Double_t A1               -- Fit param A1
//                   Double_t A2               -- Fit param A2
//                   Int_t From                -- Fit region (lower)
//                   Int_t To                  -- Fit region (upper)
//                   Int_t DisplayTrace        -- Display trace/event
//                   Int_t DisplayChannel      -- Display channel
// Description:      Perform a trace
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Nov 21 16:03:10 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                dgfGetUntriggeredTrace.C
//                   Title:               Perform a trace
//                   Width:               
//                   NofArgs:             13
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
//                   Arg2.Name:           TriggerPolarity
//                   Arg2.Title:          Trigger polarity
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        neg
//                   Arg2.Values:         neg:pos
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Xwait
//                   Arg3.Title:          XWait States
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      UpDown-X
//                   Arg3.Default:        0
//                   Arg3.AddLofValues:   No
//                   Arg3.LowerLimit:     0
//                   Arg3.UpperLimit:     200
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           NofTraces
//                   Arg4.Title:          Number of traces
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      UpDown
//                   Arg4.Default:        1
//                   Arg4.AddLofValues:   No
//                   Arg4.Increment:      1
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           TraceLength
//                   Arg5.Title:          Trace length
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      UpDown
//                   Arg5.Default:        200
//                   Arg5.AddLofValues:   No
//                   Arg5.Increment:      100
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           ConstError
//                   Arg6.Title:          Constant error
//                   Arg6.Type:           Double_t
//                   Arg6.EntryType:      UpDown
//                   Arg6.Default:        2.0
//                   Arg6.AddLofValues:   No
//                   Arg6.Increment:      1
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           A0
//                   Arg7.Title:          Fit param A0
//                   Arg7.Type:           Double_t
//                   Arg7.EntryType:      UpDown
//                   Arg7.Default:        1000
//                   Arg7.AddLofValues:   No
//                   Arg7.LowerLimit:     0.0
//                   Arg7.UpperLimit:     10000
//                   Arg7.Increment:      100
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           A1
//                   Arg8.Title:          Fit param A1
//                   Arg8.Type:           Double_t
//                   Arg8.EntryType:      UpDown
//                   Arg8.Default:        100
//                   Arg8.AddLofValues:   No
//                   Arg8.LowerLimit:     0
//                   Arg8.UpperLimit:     100000
//                   Arg8.Increment:      100
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           A2
//                   Arg9.Title:          Fit param A2
//                   Arg9.Type:           Double_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        2000
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     0
//                   Arg9.UpperLimit:     10000
//                   Arg9.Increment:      100
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          From
//                   Arg10.Title:         Fit region (lower)
//                   Arg10.Type:          Int_t
//                   Arg10.EntryType:     UpDown
//                   Arg10.Default:       85
//                   Arg10.AddLofValues:  No
//                   Arg10.Increment:     5
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          To
//                   Arg11.Title:         Fit region (upper)
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     UpDown
//                   Arg11.Default:       175
//                   Arg11.AddLofValues:  No
//                   Arg11.Increment:     5
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          DisplayTrace
//                   Arg12.Title:         Display trace/event
//                   Arg12.Type:          Int_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       1
//                   Arg12.AddLofValues:  No
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          DisplayChannel
//                   Arg13.Title:         Display channel
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     Radio
//                   Arg13.Default:       0
//                   Arg13.Values:        0:1:2:3
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////


void dgfGetUntriggeredTrace(const Char_t * Module = "<TMrbDGF.LastSelected",
                            const Char_t * TriggerPolarity = "neg",
                            Int_t Xwait = 0,
                            Int_t NofTraces = 1,
                            Int_t TraceLength = 200,
                            Double_t ConstError = 2.0,
                            Double_t A0 = 1000,
                            Double_t A1 = 100,
                            Double_t A2 = 2000,
                            Int_t From = 85,
                            Int_t To = 175,
                            Int_t DisplayTrace = 1,
                            Int_t DisplayChannel = 0)
//>>_________________________________________________(do not change this line)
{
	TMrbDGFEventBuffer buffer;
	Int_t n;
	TMrbDGF * dgf;
	Int_t chn;

	gROOT->Macro("LoadColors.C");

	dgf = (TMrbDGF *) gROOT->FindObject(Module);
	if (dgf == NULL) {
		cerr	<< setred
				<< "dgfGetUntriggeredTrace.C: Can't locate DGF-4C (\"" << Module << "\") - not defined?"
				<< setblack << endl;
		return;
	}

	dgf->ClearTriggerMask();
	dgf->SetChannelMask(GoodChannels);
	dgf->SetTriggerMask(TriggerMask, TriggerPolarity);
	dgf->SetXwait(Xwait);
	
	gEnv->SetValue("TMrbDGF.LastSelected", Module, kEnvChange);

	TCanvas * c = gROOT->FindObject("c1");

	buffer.SetModule(dgf);
	
	n = dgf->GetTrace(NofTraces, TraceLength, buffer, ConstError);
	if (n > 0) {
		buffer.Print();
		buffer.SaveTraces();
		if (DisplayTrace <= buffer.GetNofEvents()) {
			buffer.Print(DisplayTrace - 1);
			chn = 0;
			for (Int_t i = 0; i < 4; i++) {
				if (DisplayChannel & 1) break;
				DisplayChannel >>= 1;
				chn++;
			}
			buffer.CalcTau(DisplayTrace - 1, chn, 0, A0, A1, A2, From, To);
			if (c != NULL) c->Update();
		} else {
			cerr	<< setred
					<< "dgfGetUntriggeredTrace.C: Illegal trace number - " << DisplayTrace
					<< " (should be in [1," << buffer.GetNofEvents() << "])"
					<< setblack << endl;
			return;
		}
	}
}	
