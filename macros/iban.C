#include <iostream>
#include "TString.h"

void iban() {
	cout << endl << "Programm zur Berechnung und zum Test der IBAN" << endl;
	cout << "Aufruf: root iban.C+" << endl;
	cout << "        genIBAN(UInt_t BLZ, UInt_t Kto, const Char_t * LK = \"DE\");" << endl;
	cout << "        testIBAN(const Char_t * IBAN);" << endl;
	cout << endl;
}

void genIBAN(UInt_t BLZ, UInt_t Kto, const Char_t * LK = "DE") {
	UInt_t lk = ((*LK - 'A' + 10) * 100 + *(LK + 1) - 'A' + 10) * 100;
	ULong64_t pz = (ULong64_t) Kto * 1000000 + lk;
	pz = (BLZ % 97) * 10000000000000000 + pz;
	pz = 98 - (pz % 97);
	cout << "Kto     : " << Kto << endl;
	Char_t str[100];
	sprintf(str, "%d", BLZ);
	TString blz = str;
	cout << "BLZ     : " << blz(0,3) << " " << blz(3,3) << " " << blz(6,2) << endl;
	cout << "LK      : " << LK << endl;
	Char_t iban[25];
	sprintf(iban, "%s%02d%08d%010d", LK, (UInt_t) pz, BLZ, Kto);
	cout << "IBAN    : " << iban << endl;
}

void testIBAN(const Char_t * IBAN) {
	TString iban = IBAN;
	if (iban.Length() != 22) {
		cerr << "Falsches Format: IBAN muss aus 22 Zeichen bestehen" << endl;
		return;
	}
	TString lk = iban(0,2);
	TString pz = iban(2,2);
	TString blz = iban(4,8);
	TString kto = iban(12,10);
	UInt_t ilk = ((*IBAN - 'A' + 10) * 100 + *(IBAN + 1) - 'A' + 10) * 100;
	UInt_t ipz = (UInt_t) atoi(pz);
	UInt_t iblz = (UInt_t) atoi(blz);
	UInt_t ikto = (UInt_t) atoi(kto);
	ULong64_t tpz = (ULong64_t) ikto * 1000000 + ilk;
	tpz = (iblz % 97) * 10000000000000000 + tpz;
	tpz = 98 - (tpz % 97);
	cout << "Kto     : " << kto << endl;
	cout << "BLZ     : " << blz(0,3) << " " << blz(3,3) << " " << blz(6,2) << endl;
	cout << "LK      : " << lk << endl;
	if (tpz != ipz) {
		cerr << "IBAN    : Falsche Prüfsumme - " << pz << ", sollte " << tpz << " sein!" << endl;
		return;
	} else {
		cout << "IBAN    :" << iban << " ... OK!" << endl;
	}
}
