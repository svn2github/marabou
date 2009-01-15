//________________________________________________________________[QT EXAMPLE]
//////////////////////////////////////////////////////////////////////////////
//! \file
//! \example qenvExample.cxx
//! \verbatim
//! Name:           qenvExample
//! Path:           /home/Rudolf.Lutter/marabou/qt/QEnv/qenvExample
//! Purpose:        example program to use object QEnv
//! Syntax:         qenvExample r file           -- read env file <file>
//!                 qenvExample w ifile ofile    -- write <ifile> to <ofile>
//!                 qenvExample i file key val   -- modify or create key <key>
//!                                                       with int value <val>
//!                 qenvExample d file key val   -- ... with double value <val>
//!                 qenvExample s file key val   -- ... with string value <val>
//!                 qenvExample l file key       -- lookup key <key>, unique match required
//!                 qenvExample a file key       -- lookup key <key>, accept *all* matches
//! Description:    Simple program to show how to use environment lists with QT
//! Keywords:
//! Author:         R. Lutter
//! Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
//! Revision:       $Id: qenvExample.cxx,v 1.2 2009-01-15 12:08:44 Rudolf.Lutter Exp $       
//! Date:           $Date: 2009-01-15 12:08:44 $
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "QEnv.h"

using namespace std;

//! \code
int main(int argc, char * argv[]) {
	QEnv x(argv[2], TRUE);
	switch (*argv[1]) {
		case 'r': x.print(); break;
		case 'w': x.writeFile(argv[3]); break;
		case 'i': x.setValue(argv[3], atoi(argv[4])); x.writeFile(); break;
		case 'd': x.setValue(argv[3], atof(argv[4])); x.writeFile(); break;
		case 's': x.setValue(argv[3], argv[4]); x.writeFile(); break;
		case 'l':
			{
				QEnvRec r = x.lookup(argv[3]);
				if (!r.isEmpty()) cout << "matching: " << r.getKey().toAscii().data() << endl;
			}
			break;
		case 'a':
			{
				QList<QEnvRec> rl;
				Int_t n = x.lookup(argv[3], rl);
				for (Int_t i = 0; i < n; i++) {
					QEnvRec r = rl[i];
					cout << "matching: " << r.getKey().toAscii().data() << endl;
				}
			}
			break;
	}
}
//! \endcode
