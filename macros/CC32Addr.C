#include <iomanip.h>
#include <fstream.h>
#include <strstream.h>

void cnaf2vme(Int_t n = -1, Int_t a = -1, Int_t f = -1)
{
	if (n == -1) {
		ofstream of;
		of.open("cc32Addr.dat", ios::out);
		cout << "cnaf2vme(): Writing CC32 addr to file \"cc32Addr.dat\" - wait " << flush;
		for (Int_t n = 0; n <= 31; n++) {
			cout << "." << flush;
			for (Int_t a = 0; a <= 15; a++) {
				for (Int_t f = 0; f <= 31; f++) {
					UInt_t addr = (n << 10) + (a << 6) + ((f & 0xF) << 2);
					of	<< "N" << n << ".A" << a << ".F" << f << "	= 0x"
						<< setbase(16) << addr << setbase(10) << endl;
				}
			}
		}
		of.close();
		cout << " done." << endl;
	} else {
		UInt_t addr = (n << 10) + (a << 6) + ((f & 0xF) << 2);
		cout	<< "cnaf2vme(): N" << n << ".A" << a << ".F" << f << "	= 0x"
				<< setbase(16) << addr << setbase(10) << endl;
	}
}

void vme2cnaf(const Char_t * vmeAddr)
{
	TString str;
	UInt_t addr;
	str = vmeAddr;
	if (str.Index("0x", 0) == 0) {
		str = str(2, str.Length() - 2);
		istrstream is(str.Data());
		is >> setbase(16) >> addr;
		if (addr % 4) {
			cerr	<< "vme2cnaf(): Addr not aligned to 32 bits - 0x" << setbase(16) << addr << setbase(10) << endl;
			return;
		}
	} else {
		cerr	<< "vme2cnaf(): Hex number required: \"0xNNNN\"" << endl;
		return;
	}
	Int_t n = (addr >> 10) & 0x1F;
	Int_t a = (addr >> 6) & 0xF;
	Int_t f = (addr >> 2) & 0xF;
	cout	<< setbase(16) << "vme2cnaf(): 0x" << addr << setbase(10)
			<< " = N" << n
			<< ".A" << a
			<< ".F" << f << "/F" << f + 16 << endl;
}
