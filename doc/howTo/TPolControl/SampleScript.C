//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             SampleScript.C
// Purpose:          Sample script to control the Polarized Ion Source
// Description:      This script contains a collection of commands
//                   how to use the TPolControl object.
//
//                   This script is NOT designed to run AS IS -
//                   user should rather select the appropriate lines, 
//                   modify/add some lines to tailor his own script.
//
//                   For a detailed description see
//                   http://www.bl.physik.uni-muenchen.de/marabou/html/howTo/TPolControl/HowTo.html
// Author:           R. Lutter
// Date:             Dec-2002
// Keywords:
//////////////////////////////////////////////////////////////////////////////

{
// initialization
    gROOT->Macro("LoadPolarLibs.C");                    // load libraries
    TPolControl * pc = new TPolControl("/dev/ttyS1");   // instantiate class object

    pc->SerIO()->Setup(9600, 7, 0, 1);                  // configure serial port
    pc->SerIO()->Open();                                // open it
    pc->SetSubdevs(0, 1, 2, 3, 4);                      // activate subdevices
    pc->ReadNameTable("polNames.dat");                  // read name table

    pc->Print();                                        // print settings
    pc->PrintNameTable();                               // print name table

    pc->SetDac(4, 0, 1.5);                              // set dac (relative addr)
    pc->SetDac(128, 1.5);                               // set dac (absolute addr)
    pc->SetDac("DAC0", 1.5);                            // set dac (by name)

// input/output
    Int_t adcVal;
    adcVal = pc->ReadAdc(4, 3);                         // read adc value
    adcVal = pc->ReadAdc(131);
    adcVal = pc->ReadAdc("ADC3");

    adcVal = pc->SetDac(4, 0, 1.5, 1);                  // set dac and read back resulting adc value
    adcVal = pc->SetDac(128, 1.5, 129);
    adcVal = pc->SetDac("DAC0", 1.5, "ADC1");

// plot
    TH1F * histo;
    histo = pc->Plot("SampleHist", 4, 0, 1, 0.0, 5.0, 0.1, "DAC0", "ADC1");   // plot ADC data vs. DAC value
    histo = pc->Plot("SampleHist", 128, 129, 0.0, 5.0, 0.1, "DAC0", "ADC1");
    histo = pc->Plot("SampleHist", "DAC0", "ADC1", 0.0, 5.0, 0.1);
    histo->Draw();                                      // draw resulting histogram

// monitoring
    TNtuple * ntuple;
    ntuple = pc->Monitor("test.root", "Monitor test", "ADC0:ADC1:ADC2:ADC3", 100, 1);	// monitor adc data
    ntuple = pc->Monitor("test.root", "Monitor test", "128:129:130:131", 100, 1);
    ntuple = pc->Monitor("test.root", "Monitor test", "4-0:4-1:4-2:4-3", 100, 1);

    pc->PrintMonitorLayout();                           // print monitor layout to cout

// replay monitor data

    TNtuple * ntuple;
    ntuple = pc->Monitor("test.root");                  // open file containing monitor data

    TArrayF adcData;                                    // array to store adc data
    UInt_t timeStamp;                                   // current time stamp

    for (Int_t i = 0; i < pc->GetNofEntries(); i++) {   // loop over entries in file
       Int_t n = pc->GetEntry(i, timeStamp, adcData);   // load entry data to local array
       for (Int_t j = 0; j < n; j++) {                  // just to demonstrate: print ADC data to cout
          cout << "ADC #" << j << " has value " << adcData[j] << endl;
       }
    }

    pc->PrintRecords(0, 9);                             // may be used to output records to cout
}
