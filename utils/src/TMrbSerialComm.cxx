//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbSerialComm.cxx
// Purpose:        MARaBOU utilities: serial line I/O
// Description:    Implements methods to control a serial port
//                 (inspired by class c_SerialComm written by Mario Schubert)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>

#include "TROOT.h"
#include "TSystem.h"

#include "TMrbSerialComm.h"

#include "TMrbLogger.h"
#include "SetColor.h"

ClassImp(TMrbSerialComm)

extern TMrbLogger * gMrbLog;			// access to message logger

TMrbSerialComm::TMrbSerialComm(const Char_t * Device) : TNamed(Device, Device) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm
// Purpose:        Control a serial port
// Description:    Class to control I/O from/to a serial port.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString device;
	TString prefix;
	Int_t n;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	device = Device;
	device.SplitOffInteger(prefix, n);
	device = "tty";
	device += n;
	this->SetName(device.Data());

	fFd = -1;
	fBaudRate = 19200;
	fUseRtsCts = kFALSE;
	fDelay = 0.0;
	fNofDataBits = 8;
	fParity = TMrbSerialComm::kMrbParityNone;
	fNofStopBits = 1;
	fIgnoreCR = kTRUE;
	fLineTerm1 = '\000';
	fLineTerm2 = '\000';
	fPrefix ="";
	fPostfix="\r";
	fBytesReceived = 0;
	fBytesTransmitted = 0;

	gROOT->Append(this);
}

Bool_t TMrbSerialComm::Open() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::Open
// Purpose:        Open serial port
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens port given by GetName(). Uses default settings
//                 or those defined by method Setup().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	termios * ntp;

	if (this->IsOpen()) {
		gMrbLog->Err()	<< "Device already open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		return(kFALSE);
	}

	fFd = open(this->GetPort(), O_RDWR | O_NOCTTY | O_NONBLOCK); 
	if (fFd < 0) {
		gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName());
		fFd = -1;
		return(kFALSE);
	}

	fOldTIO = (void *) new termios;
	tcgetattr(fFd, (termios *) fOldTIO);				// save current serial port settings

	fNewTIO = (void *) new termios; 					// struct to hold new settings
	ntp = (termios *) fNewTIO;
	memset(ntp, 0, sizeof(termios));					// tabula rasa

//  see termios(3)
//		BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
//		CRTSCTS : output hardware flow control (only used if the cable has all necessary lines. See sect. 7 of Serial-HOWTO)
//		CS8     : 8n1 (8bit, no parity, 1 stopbit)
//		CS7     : 7bit
//		PARENB  : enable parity
//		PARODD  : parity is odd
//		CSTOPB  : set two stop bits, rather than one.
//		CLOCAL  : local connection, no modem contol
//		CREAD   : enable receiving characters

	ntp->c_cflag = CLOCAL | CREAD;
	if (fBaudRate == 19200) {
		ntp->c_cflag |= B19200;
	} else if (fBaudRate == 9600) {
		ntp->c_cflag |= B9600;
	} else if (fBaudRate == 38400) {
		ntp->c_cflag |= B38400;
	} else if (cfsetspeed(ntp, fBaudRate) == -1) {
		gMrbLog->Err()	<< "[" << this->GetName() << "]: Unsupported baud rate - " << fBaudRate
						<< ", using default = 19200" << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		fBaudRate = 19200;
		ntp->c_cflag |= B19200;
    }
 
	if (fNofDataBits == 8) {
		ntp->c_cflag |= CS8;
	} else if (fNofDataBits == 7) {
		ntp->c_cflag |= CS7;
	} else {
		gMrbLog->Err()	<< "[" << this->GetName() << "]: Unsupported number of DATA bits - " << fNofDataBits
						<< ", using default = 8" << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		ntp->c_cflag |= CS8;
		fNofDataBits = 8;
	}

	switch (fParity) {
		case kMrbParityNone:
			break;
		case kMrbParityOdd:
			ntp->c_cflag |= PARENB;
			ntp->c_cflag |= PARODD;
			break;
		case kMrbParityEven:
			ntp->c_cflag |= PARENB;
			break;
		default:
			gMrbLog->Err()	<< "[" << this->GetName() << "]: Unsupported parity - " << fParity
						<< ", using default = none(0)" << endl;
			gMrbLog->Flush(this->ClassName(), "Open");
			fParity = kMrbParityNone;
			break;
	}

	if (fNofStopBits == 2) {
		ntp->c_cflag |= CSTOPB;
	} else if (fNofStopBits != 1) {
		gMrbLog->Err()	<< "[" << this->GetName() << "]: Unsupported number of STOP bits - " << fNofStopBits
						<< ", using default = 1" << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		fNofStopBits = 1;
	}

	if (fUseRtsCts) ntp->c_cflag |= CRTSCTS;

// IGNPAR  : ignore bytes with parity errors
// ICRNL   : map CR to NL (otherwise a CR input on the other computer will not terminate input)

	ntp->c_iflag = 0;
	switch (fParity) {
		case kMrbParityNone:
			break;
		case kMrbParityOdd:
		case kMrbParityEven:
			ntp->c_iflag = INPCK | PARMRK;
			break;
	}

	if (fIgnoreCR) {
		ntp->c_iflag |= IGNCR;
	} else {
		ntp->c_iflag |= ICRNL;
	}

	ntp->c_oflag = 0;
     
// ICANON  : enable canonical input, disable all echo functionality, and don't send signals to calling program
	ntp->c_lflag = ICANON;
     
// initialize all control characters 
// default values can be found in /usr/include/termios.h
 
	ntp->c_cc[VMIN] = 1; 			// blocking read until 1 character arrives
	ntp->c_cc[VEOL] = fLineTerm1;
	ntp->c_cc[VEOL2] = fLineTerm2;

	tcflush(fFd, TCIFLUSH);				// clean the modem line and activate the settings for the port
	tcsetattr(fFd, TCSANOW, (termios *) fNewTIO);
	return(kTRUE);
}

Bool_t TMrbSerialComm::Close() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::Close()
// Purpose:        Close connection
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (this->IsOpen()) {
		tcflush(fFd, TCIOFLUSH);
		tcsetattr(fFd, TCSANOW, (termios *) fOldTIO);
		if (close(fFd) == -1) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "Close");
			return(kFALSE);
		} else {
			fFd = -1;
			return(kTRUE);
		}
	} else {
		gMrbLog->Err()	<< "Device not open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Close");
		return(kTRUE);
	}
}

void TMrbSerialComm::Setup(Int_t BaudRate, Int_t NofDataBits, EMrbParity Parity, Int_t NofStopBits,
																Bool_t UseRtsCts, Bool_t IgnoreCR) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::Setup
// Purpose:        Set device parameters
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Stores device settings to be used with method Open().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsOpen()) {
		gMrbLog->Err()	<< "Device already open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Setup");
	} else {
		fBaudRate = BaudRate;
		fNofDataBits = NofDataBits;
		fParity = Parity;
		fNofStopBits = NofStopBits;
		fUseRtsCts = UseRtsCts;
		fIgnoreCR = IgnoreCR;
	}
}

Int_t TMrbSerialComm::ReadData(TMrbString & Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::ReadData
// Purpose:        Read data from serial device
// Arguments:      TMrbString & Data      -- where to store data
// Results:        Int_t NofBytes         -- number of bytes read
// Exceptions:
// Description:    Inputs data from serial port.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t buf[256];
	Int_t nBytes;

	if (!this->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ReadData");
		return(-1);
	}

	memset(buf, 0, 256);

	nBytes = read(fFd, buf, 255);
	if (nBytes == -1) {
		if (gSystem->GetErrno() == EAGAIN) {
			return(0);
		} else {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "ReadData");
			return(nBytes);
		}
	}

	for (Int_t i = 0; i < nBytes; i++) {	// replace zeros by space
		if (buf[i] == '\000') buf[i] = ' ';
	}

	fBytesReceived += nBytes;				// book keeping
	Data = buf;								// pass buffer to caller
	return(nBytes); 						// number of bytes read
}

Int_t TMrbSerialComm::WriteData(const Char_t * Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::WriteData
// Purpose:        Write data to serial port
// Arguments:      Char_t * Data       -- where to get data from
// Results:        Int_t NofBytes      -- number of bytes written
// Exceptions:
// Description:    Outputs data to serial port.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString buf;
	Int_t nBytes;

	if (!this->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteData");
		return(-1);
	}

	buf = fPrefix + Data + fPostfix;

	nBytes = write(fFd, buf.Data(), buf.Length());
	if (nBytes != buf.Length()) {
		gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteData");
		return(-1);
	}
	fBytesTransmitted += nBytes;
	return(nBytes);
}

Int_t TMrbSerialComm::WriteSingleBytes(const Char_t * Data, Double_t Delay) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::WriteSingleBytes
// Purpose:        Write data to serial port
// Arguments:      Char_t * Data       -- where to get data from
//                 Double_t Delay      -- delay in s to wait between chars
// Results:        Int_t NofBytes      -- number of bytes written
// Exceptions:
// Description:    Outputs data to serial port.
//                 Writes single bytes waiting a certain delay in between
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString buf;
	struct timeval tv;
	Int_t sts;
	Double_t delay;
 	Char_t c;

	if (!this->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteSingleBytes");
		return(-1);
	}

	delay = (Delay < 0) ? fDelay : Delay;
	buf = fPrefix + Data + fPostfix;

	for (Int_t i = 0; i < buf.Length(); i++) {
		c = buf(i);
		sts = write(fFd, &c, 1);
		if (sts != 1) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteSingleBytes");
			return(-1);
		}

		sts = tcdrain(fFd);
    	if (sts != 0) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteSingleBytes");
			return(-1);
		} 

     	tv.tv_sec = (Int_t) delay;
     	tv.tv_usec= (Int_t) ((delay - tv.tv_sec) * 1000000);
    	sts = select(0, NULL, NULL, NULL, &tv);
    	if (sts != 0) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteSingleBytes");
			return(-1);
		}
	}
	fBytesTransmitted += buf.Length();
	return(buf.Length());
}

Int_t TMrbSerialComm::WriteWithDelay(const Char_t * Data, Double_t Delay) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::WriteWithDelay
// Purpose:        Write data to serial port
// Arguments:      Char_t * Data       -- where to get data from
//                 Double_t Delay      -- delay in s to wait
// Results:        Int_t NofBytes      -- number of bytes written
// Exceptions:
// Description:    Outputs data to serial port.
//                 Waits a certain delay after output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString buf;
	struct timeval tv;
	Double_t delay;
	Int_t sts;

	if (!this->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteWithDelay");
		return(-1);
	}

	delay = (Delay < 0) ? fDelay : Delay;
	buf = fPrefix + Data + fPostfix;

	sts = write(fFd, buf.Data(), buf.Length());
	if (sts != buf.Length()) {
		gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteWithDelay");
		return(-1);
	}

	if (delay > 0) {
		sts = tcdrain(fFd);
    	if (sts != 0) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteWithDelay");
			return(-1);
		} 

		tv.tv_sec = (Int_t) delay;
		tv.tv_usec= (Int_t) ((delay - tv.tv_sec) * 1000000);
		sts = select(0, NULL, NULL, NULL, &tv);
    	if (sts != 0) {
			gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << this->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "WriteWithDelay");
			return(-1);
		}
	}
	fBytesTransmitted += buf.Length();
	return(buf.Length());
}

void TMrbSerialComm::Print() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::Print
// Purpose:        Output settings to cout
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs settings to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString str;

	cout	<< endl;
	cout	<< "RS232 Settings:" << endl;
	cout	<< "-----------------------------------------------------------------" << endl;
	cout	<< "Name                   : " << this->GetName() << endl;
	cout	<< "Port                   : " << this->GetPort() << endl;
	cout	<< "State                  : ";
	if (fFd == -1) cout << "not open"; else cout << "open"; cout << endl;
	cout	<< "Baud rate              : " << fBaudRate << endl;
	cout	<< "Number of data bits    : " << fNofDataBits << endl;
	cout	<< "Number of stop bits    : " << fNofStopBits << endl;
	cout	<< "Parity                 : ";
	switch (fParity) {
		case kMrbParityNone:	cout	<< "none" << endl; break;
		case kMrbParityOdd: 	cout	<< "odd" << endl; break;
		case kMrbParityEven:	cout	<< "even" << endl; break;
	}
	cout	<< "Terminator #1          : " << Form("\\%03o", fLineTerm1) << endl;
	cout	<< "           #2          : " << Form("\\%03o", fLineTerm2) << endl;
	cout	<< "Handshake              : ";
	if (fUseRtsCts) cout << "RTS / CTS" << endl; else cout << "none" << endl;
	cout	<< "Ignore <CR>            : ";
	if (fIgnoreCR) cout << "yes" << endl; else cout << "no" << endl;
	cout	<< "Delay                  : " << fDelay << " s" << endl;
	cout	<< "Prefix                 : " << this->MakePrintable(str, fPrefix) << endl;
	cout	<< "Postfix                : " << this->MakePrintable(str, fPostfix) << endl;
	cout	<< "Bytes received         : " << fBytesReceived << endl;
	cout	<< "Bytes transmitted      : " << fBytesTransmitted << endl;
	cout	<< endl;
}

const Char_t * TMrbSerialComm::MakePrintable(TString & PrintString, const Char_t * String) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSerialComm::MakePrintable
// Purpose:        Convert unprintable chars to printable ones
// Arguments:      TString & PrintString    -- where to put printable output
//                 Char_t * String          -- original string
// Results:        Char_t * PrintString     -- pointer to output string
// Exceptions:
// Description:    Converts a string: makes unprintable chars visible.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString str;
	PrintString = "";
	str = String;

	for (Int_t i = 0; i < str.Length(); i++) {
		if (str(i) == '\r') {
			PrintString += "\\r";
		} else if (str(i) == '\n') {
			PrintString += "\\n";
		} else if ( (isalpha(str(i)) != 0)
				||	(isdigit(str(i)) != 0)
				||	(ispunct(str(i)) != 0)
				||	(isspace(str(i)) != 0)) {
			PrintString += str(i);
		} else {
			PrintString += Form("\\03o", str(i));
		}
	}
	return(PrintString.Data());
}
