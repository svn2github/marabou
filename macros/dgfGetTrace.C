//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         dgfGetTrace.C
// @(#)Purpose:      Perform a trace
// Syntax:           .x dgfGetTrace.C(const Char_t * Module,
//                                     Int_t GoodChannels,
//                                     Int_t TriggerMask,
//                                     const Char_t * TriggerPolarity,
//                                     Int_t Xwait,
//                                     Int_t NofTraces,
//                                     Int_t TraceLength,
//                                     Double_t ConstError,
//                                     Double_t A0,
//                                     Double_t A1,
//                                     Double_t A2,
//                                     Int_t From,
//                                     Int_t To,
//                                     Int_t DisplayTrace,
//                                     Int_t DisplayChannel)
// Arguments:        Char_t * Module           -- Module name (dgfN, N=Camac Station)
//                   Int_t GoodChannels        -- Good channels
//                   Int_t TriggerMask         -- Trigger Channels
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
// @(#)Date:         Mon Oct 22 17:44:36 2001
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                dgfGetTrace.C
//                   Title:               Perform a trace
//                   Width:               
//                   NofArgs:             15
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
//                   Arg2.Name:           GoodChannels
//                   Arg2.Title:          Good channels
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Check
//                   Arg2.Default:        0
//                   Arg2.Values:         0:1:2:3
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           TriggerMask
//                   Arg3.Title:          Trigger Channels
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      Check
//                   Arg3.Default:        0
//                   Arg3.Values:         0:1:2:3
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           TriggerPolarity
//                   Arg4.Title:          Trigger polarity
//                   Arg4.Type:           Char_t *
//                   Arg4.EntryType:      Radio
//                   Arg4.Default:        neg
//                   Arg4.Values:         neg:pos
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Xwait
//                   Arg5.Title:          XWait States
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      UpDown-X
//                   Arg5.Default:        0
//                   Arg5.AddLofValues:   No
//                   Arg5.LowerLimit:     0
//                   Arg5.UpperLimit:     200
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           NofTraces
//                   Arg6.Title:          Number of traces
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      UpDown
//                   Arg6.Default:        1
//                   Arg6.AddLofValues:   No
//                   Arg6.Increment:      1
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           TraceLength
//                   Arg7.Title:          Trace length
//                   Arg7.Type:           Int_t
//                   Arg7.EntryType:      UpDown
//                   Arg7.Default:        200
//                   Arg7.AddLofValues:   No
//                   Arg7.Increment:      100
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           ConstError
//                   Arg8.Title:          Constant error
//                   Arg8.Type:           Double_t
//                   Arg8.EntryType:      UpDown
//                   Arg8.Default:        2.0
//                   Arg8.AddLofValues:   No
//                   Arg8.Increment:      1
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           A0
//                   Arg9.Title:          Fit param A0
//                   Arg9.Type:           Double_t
//                   Arg9.EntryType:      UpDown
//                   Arg9.Default:        1000
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     0.0
//                   Arg9.UpperLimit:     10000
//                   Arg9.Increment:      100
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          A1
//                   Arg10.Title:         Fit param A1
//                   Arg10.Type:          Double_t
//                   Arg10.EntryType:     UpDown
//                   Arg10.Default:       100
//                   Arg10.AddLofValues:  No
//                   Arg10.LowerLimit:    0
//                   Arg10.UpperLimit:    100000
//                   Arg10.Increment:     100
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          A2
//                   Arg11.Title:         Fit param A2
//                   Arg11.Type:          Double_t
//                   Arg11.EntryType:     UpDown
//                   Arg11.Default:       2000
//                   Arg11.AddLofValues:  No
//                   Arg11.LowerLimit:    0
//                   Arg11.UpperLimit:    10000
//                   Arg11.Increment:     100
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          From
//                   Arg12.Title:         Fit region (lower)
//                   Arg12.Type:          Int_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       85
//                   Arg12.AddLofValues:  No
//                   Arg12.Increment:     5
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          To
//                   Arg13.Title:         Fit region (upper)
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     UpDown
//                   Arg13.Default:       175
//                   Arg13.AddLofValues:  No
//                   Arg13.Increment:     5
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          DisplayTrace
//                   Arg14.Title:         Display trace/event
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     UpDown
//                   Arg14.Default:       1
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          DisplayChannel
//                   Arg15.Title:         Display channel
//                   Arg15.Type:          Int_t
//                   Arg15.EntryType:     Radio
//                   Arg15.Default:       0
//                   Arg15.Values:        0:1:2:3
//                   Arg15.AddLofValues:  No
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////


void dgfGetTrace(const Char_t * Module = "<TMrbDGF.LastSelected",
                 Int_t GoodChannels = 0,
                 Int_t TriggerMask = 0,
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
				<< "dgfGetTrace.C: Can't locate DGF-4C (\"" << Module << "\") - not defined?"
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
	
	n = dgf->GetTrace(TriggerMask, TriggerPolarity, GoodChannels, Xwait, NofTraces, TraceLength, buffer, ConstError);
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
					<< "dgfGetTrace.C: Illegal trace number - " << DisplayTrace
					<< " (should be in [1," << buffer.GetNofEvents() << "])"
					<< setblack << endl;
			return;
		}
	}
}	
