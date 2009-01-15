//________________________________________________________________[QT EXAMPLE]
//////////////////////////////////////////////////////////////////////////////
//! \file
//! \example qenvExample.cxx
//! \verbatim
//! Name:           qenvExample.cxx
//! Purpose:        example program to use object QEnv
//! Syntax:         ./qenvExample r file           -- read env file <file>
//!                 ./qenvExample w ifile ofile    -- write <ifile> to <ofile>
//!                 ./qenvExample i file key val   -- modify or create key <key>
//!                                                       with int value <val>
//!                 ./qenvExample d file key val   -- ... with double value <val>
//!                 ./qenvExample s file key val   -- ... with string value <val>
//! Description:    Simple program to show how to use environment lists with QT
//! Keywords:
//! Author:         R. Lutter
//! Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
//! Revision:       $Id: qenvExample.cxx,v 1.1 2009-01-15 07:22:30 Rudolf.Lutter Exp $       
//! Date:           $Date: 2009-01-15 07:22:30 $
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "QEnv.h"

using namespace std;

int main(int argc, char * argv[]) {
	QEnv x(argv[2], TRUE);
	switch (*argv[1]) {
		case 'r': x.print(); break;
		case 'w': x.writeFile(argv[3]); break;
		case 'i': x.setValue(argv[3], atoi(argv[4])); x.writeFile(); break;
		case 'd': x.setValue(argv[3], atof(argv[4])); x.writeFile(); break;
		case 's': x.setValue(argv[3], argv[4]); x.writeFile(); break;
	}
}
