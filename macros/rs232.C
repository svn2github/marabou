//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         rs232.C
// @(#)Purpose:      Connect to serial port
// Syntax:           .x rs232.C(Bool_t Mode,
//                               const Char_t * Port,
//                               Int_t BaudRate,
//                               Int_t NofDataBits,
//                               Int_t NofStopBits,
//                               Int_t Parity,
//                               Bool_t Handshake,
//                               Bool_t IgnoreCR,
//                               const Char_t * Prefix,
//                               const Char_t * Postfix)
// Arguments:        Bool_t Mode               -- Mode
//                   Char_t * Port             -- Port
//                   Int_t BaudRate            -- Baud rate
//                   Int_t NofDataBits         -- Number of data bits
//                   Int_t NofStopBits         -- Number of stop bits
//                   Int_t Parity              -- Parity
//                   Bool_t Handshake          -- CTS/RTS handshake
//                   Bool_t IgnoreCR           -- Ignore <CR>
//                   Char_t * Prefix           -- Prefix
//                   Char_t * Postfix          -- Postfix
// Description:      Connect to serial port
// @(#)Author:       marabou
// @(#)Revision:     SCCS:  %W%
// @(#)Date:         Wed Aug 21 09:00:31 2002
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                rs232.C
//                   Title:               Connect to serial port
//                   Width:               
//                   NofArgs:             10
//                   Arg1.Name:           Mode
//                   Arg1.Title:          Mode
//                   Arg1.Type:           Int_t
//                   Arg1.EntryType:      Radio
//                   Arg1.Default:        send
//                   Arg1.Values:         send=0:receive=1
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Port
//                   Arg2.Title:          Port
//                   Arg2.Type:           Char_t *
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        /dev/ttyS0
//                   Arg2.Values:         /dev/ttyS0:/dev/ttyS1
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           BaudRate
//                   Arg3.Title:          Baud rate
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      Radio
//                   Arg3.Default:        9600
//                   Arg3.Values:         9600=9600:19200=19200:38400=38400
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           NofDataBits
//                   Arg4.Title:          Number of data bits
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Radio
//                   Arg4.Default:        8
//                   Arg4.Values:         7=7:8=8
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           NofStopBits
//                   Arg5.Title:          Number of stop bits
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      Radio
//                   Arg5.Default:        1
//                   Arg5.Values:         1=1:2=2
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           Parity
//                   Arg6.Title:          Parity
//                   Arg6.Type:           Int_t
//                   Arg6.EntryType:      Radio
//                   Arg6.Default:        none
//                   Arg6.Values:         none=0:odd=1:even=2
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           Handshake
//                   Arg7.Title:          CTS/RTS handshake
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.Default:        No
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           IgnoreCR
//                   Arg8.Title:          Ignore <CR>
//                   Arg8.Type:           Bool_t
//                   Arg8.EntryType:      YesNo
//                   Arg8.Default:        Yes
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           Prefix
//                   Arg9.Title:          Prefix
//                   Arg9.Type:           Char_t *
//                   Arg9.EntryType:      Entry
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          Postfix
//                   Arg10.Title:         Postfix
//                   Arg10.Type:          Char_t *
//                   Arg10.EntryType:     Entry
//                   Arg10.Default:       \r
//                   Arg10.AddLofValues:  No
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////


void rs232(Bool_t Mode = 0,
           const Char_t * Port = "/dev/ttyS0",
           Int_t BaudRate = 9600,
           Int_t NofDataBits = 8,
           Int_t NofStopBits = 1,
           Int_t Parity = 0,
           Bool_t Handshake = kFALSE,
           Bool_t IgnoreCR = kTRUE,
           const Char_t * Prefix,
           const Char_t * Postfix = "\r")
//>>_________________________________________________(do not change this line)
//
{
	Int_t n;
	TString x;
	gROOT->Macro("LoadUtilityLibs.C");
	TMrbSerialComm * tty = new TMrbSerialComm(Port);
	tty->Setup(BaudRate, NofDataBits, Parity, NofStopBits, Handshake, IgnoreCR);
	tty->SetPrefix(Prefix);
	tty->SetPostfix(Postfix, kFALSE);
	tty->Open();
	tty->Print();
	if (Mode == 0) {
		cout << endl << "Port " << tty->GetPort() << " ready for output ..." << endl;
		cout << endl << "To send data execute these commands (use cut&paste!):" << endl;
		cout << "          TMrbSerialComm * tty = (TMrbSerialComm *) gROOT->FindObject(\"" << tty->GetName() << "\");   // get device (once only)" << endl;
		cout << "          tty->WriteData(\"Your message\");   // send data to " << tty->GetPort() << endl << endl;
	} else {
		tty->ReadData(x);
		cout << endl << "Port " << tty->GetPort() << " ready for input ... " << endl << endl;
		while (1) {
			x = "";
			if ((n = tty->ReadData(x)) > 0) cout << n << " ... " << x << endl;
			gSystem->Sleep(100);
			if (x.CompareTo("Exit") == 0) gSystem->Exit(0);
		}
	}
}
