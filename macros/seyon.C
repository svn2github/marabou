//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:         seyon.C
// @(#)Purpose:      Connect to serial port
// Syntax:           .x seyon.C(const Char_t * Port,
//                               Int_t BaudRate,
//                               Int_t NofDataBits,
//                               Int_t NofStopBits,
//                               Int_t Parity,
//                               Bool_t Handshake,
//                               Bool_t IgnoreCR,
//                               const Char_t * Prefix,
//                               const Char_t * Postfix)
// Arguments:        Char_t * Port             -- Port
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
// @(#)Date:         Wed Aug  7 10:37:11 2002
// URL:              
// Keywords:
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                seyon.C
//                   Title:               Connect to serial port
//                   Width:               
//                   NofArgs:             9
//                   Arg1.Name:           Port
//                   Arg1.Title:          Port
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      Radio
//                   Arg1.Default:        /dev/ttyS0
//                   Arg1.Values:         /dev/ttyS0:/dev/ttyS1
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           BaudRate
//                   Arg2.Title:          Baud rate
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        9600
//                   Arg2.Values:         9600=9600:19200=19200:38400=38400
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           NofDataBits
//                   Arg3.Title:          Number of data bits
//                   Arg3.Type:           Int_t
//                   Arg3.EntryType:      Radio
//                   Arg3.Default:        8
//                   Arg3.Values:         7=7:8=8
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           NofStopBits
//                   Arg4.Title:          Number of stop bits
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Radio
//                   Arg4.Default:        1
//                   Arg4.Values:         1=1:2=2
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Parity
//                   Arg5.Title:          Parity
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      Radio
//                   Arg5.Default:        none
//                   Arg5.Values:         none=0:odd=1:even=2
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           Handshake
//                   Arg6.Title:          CTS/RTS handshake
//                   Arg6.Type:           Bool_t
//                   Arg6.EntryType:      YesNo
//                   Arg6.Default:        No
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           IgnoreCR
//                   Arg7.Title:          Ignore <CR>
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.Default:        Yes
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           Prefix
//                   Arg8.Title:          Prefix
//                   Arg8.Type:           Char_t *
//                   Arg8.EntryType:      Entry
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           Postfix
//                   Arg9.Title:          Postfix
//                   Arg9.Type:           Char_t *
//                   Arg9.EntryType:      Entry
//                   Arg9.Default:        \r
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////


void seyon(const Char_t * Port = "/dev/ttyS0",
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
	gROOT->Macro("LoadUtilityLibs.C");
	TMrbSerialComm * tty = new TMrbSerialComm(Port);
	cout << "@@ " << BaudRate << " " << NofDataBits << " " << Parity << " " << NofStopBits << endl;
	tty->Setup(BaudRate, NofDataBits, Parity, NofStopBits, Handshake, IgnoreCR);
	tty->SetPrefix(Prefix);
	tty->SetPostfix(Postfix, kFALSE);
	tty->Print();
}
