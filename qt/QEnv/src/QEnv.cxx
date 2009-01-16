//__________________________________________________________[QT IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file
//! \verbatim
//! Name:           QEnv.cxx
//! Purpose:        QT utilities:
//!                 A class handling environment files used with ROOT (like TEnv)
//! Description:    Implements class methods to manage ROOT's TEnv files
//! Keywords:
//! Author:         R. Lutter
//! Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
//! Revision:       $Id: QEnv.cxx,v 1.9 2009-01-16 12:53:42 Rudolf.Lutter Exp $       
//! Date:           $Date: 2009-01-16 12:53:42 $
//!
//! Format:         Entry of the envrironment database:
//!                      <key>: <value>
//!                 where
//!                      <key> is a dot-separated string like a X resource,
//!                            substrings between dots may be "*" to denote wildcard
//!                      <value> is integer, double, or string
//!                 Setting or getting a key value will be type-checked,
//!                 thus you will get a warning when changing the type on setValue()
//!                 and will get the default value instead of the true one
//!                 when accessing a key with wrong type by getValue().
//!                 Setting/getting a key via type=string is always allowed
//!                 as keys and values are being stored as strings.
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <QFile>
#include <QByteArray>
#include <QStringList>

#include "QEnv.h"

//__________________________________________________________[DOXYGEN MAINPAGE]
//////////////////////////////////////////////////////////////////////////////
//! \mainpage
//! \verbatim
//! Class QEnv: a QT equivalent for ROOT's TEnv class
//! Maintains environment lists. See also TEnv documentation.
//!
//! $Author: Rudolf.Lutter $
//! $Revision: 1.9 $       
//! $Date: 2009-01-16 12:53:42 $
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

using namespace std;

static QEnvRec emptyRec;

//___________________________________________________________________[QT CLASS]
//////////////////////////////////////////////////////////////////////////////
//! \class QEnvRec QEnv.h "/home/Rudolf.Lutter/marabou/QEnv/inc/QEnv.h"
//! \verbatim
//! Name:           QEnvRec::QEnvRec
//! Purpose:        Instantiate a record to be used in QEnv database
//! Arguments:      QString & key       -- key
//!                 QString & value     -- value
//! Description:    Contains a single entry in QEnv database
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

QEnvRec::QEnvRec(QString & key, QString & value) {

	qKey = key;
	qValue = value;
	qRecType = QEnvRec::kString;		// default: string
};

//___________________________________________________________________[QT CLASS]
//////////////////////////////////////////////////////////////////////////////
//! \class QEnv QEnv.h "/home/Rudolf.Lutter/marabou/QEnv/inc/QEnv.h"
//! \verbatim
//! Name:           QEnv::QEnv
//! Purpose:        Instantiate QEnv database
//! Arguments:      Char_t * fileName  -- name of env file
//!                 Bool_t verbose     -- turn verbose flag on/off
//! Description:    Main QEnv object
//! Keywords:
//! \endverbatim
//! \example qenvExample.cxx
//////////////////////////////////////////////////////////////////////////////

QEnv::QEnv(const Char_t * fileName, Bool_t verbose) {

	qVerbose = verbose;
	if (this->readFile(fileName) > 0) qEnvFile = fileName;
	qCurIndex = -1;
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnvRec::setType
//! Purpose:        Set entry type according to data type
//! Arguments:      QString & value     -- value
//! Results:        -- 
//! Description:    Checks if data type is int, double, or string.
//!                 Defaults to string.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

void QEnvRec::setType(QString & value) {

	QString v = value;
	v += " 0";									// append "<space>0"
	Int_t v1, v2;
	istringstream istr(v.toAscii().data()); 	// then decode using format "<int> <int>"
	istr >> v1 >> v2;							// check if we got 2 ints: v1 should be same as <value>, v2 should be "0"
	if (v2 == 0 && v1 == strtol(value.toAscii().data(), NULL, 10)) { qRecType = QEnvRec::kInteger; return; }
	Double_t v3;									// didn't work: try format "<double> <int>"
	istringstream dstr(v.toAscii().data());
	dstr >> v3 >> v2;							// check if we got 1 double and 1 int: v1 should be same as <value>, v2 should be "0"
	if (v2 == 0 && v3 == strtod(value.toAscii().data(), NULL)) { qRecType = QEnvRec::kDouble; return; }
	qRecType = QEnvRec::kString;				// didn't work: take string as default
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnvRec::print
//! Purpose:        Print record
//! Arguments:      Int_t recNo          -- record number
//!                 Int_t lkey           -- max key length
//! Results:        -- 
//! Description:    Outputs key name and key value to cout.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

void QEnvRec::print(Int_t recNo, Int_t lkey) {

	QString type;
	switch (this->getType()) {
		case QEnvRec::kNew: return;
		case QEnvRec::kEmpty: return;
		case QEnvRec::kInteger: type = "int"; break;
		case QEnvRec::kDouble: type = "dbl"; break;
		case QEnvRec::kString: type = "str"; break;
	}
	if (recNo == -1) {
		printf("%-*s [%-3s]  %s\n", lkey, this->getKey().toAscii().data(), type.toAscii().data(), this->getValue().toAscii().data());
	} else {
		printf("%3d    %-*s [%-3s]  %s\n", recNo, lkey, this->getKey().toAscii().data(), type.toAscii().data(), this->getValue().toAscii().data());
	}
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::readFile
//! Purpose:        Read env data from file
//! Arguments:      Char_t * fileName     -- name of env file
//! Results:        Int_t nofEntries      -- number of entries read
//!                                              or -1 if error
//! Description:    Reads env data line by line and builds database
//!                 Call this method more than once to combine different databases.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::readFile(const Char_t * fileName) {

	QFile ifile(fileName);
	if (!ifile.open(QIODevice::ReadOnly)) { 		// try to open file for input
		cerr << "QEnv::readFile(): Can't open file for reading - " << fileName << endl;
		return -1;
	}

	Int_t nofEntries = 0;
	while (!ifile.atEnd()) {						// read line by line
		QString line = ifile.readLine();
		line.remove(line.size() - 1, 1);			// remove traling <lf>
		line = line.simplified();					// skip surrounding spaces
		if (line.isEmpty()) continue;				// nothing in there
		QStringList l = line.split("#");			// look for # comments
		if (l.size() > 1) line = l[0];				// we have one: skip it
		if (line.isEmpty()) continue;				// line starts with # - nothing to do
		l = line.split("//");						// look for // comments
		if (l.size() > 1) line = l[0];				// we have one: take front part
		if (line.isEmpty()) continue;				// line starts with // - nothing to do
		QStringList env = line.split(":");			// look for delimiting ":" (in format "<key>: <value>")
		QString k = env[0].simplified();			// extract key name
		QString v;
		if (env.size() > 2) {						// maybe <value> contains additional ":"s
			env.removeFirst();						// remove key name from list
			v = env.join(":");						// take the rest as <value> again
		} else {
			v = env[1].simplified();				// no more ":"s - we take 2nd list element as <value>
		}
		QEnvRec * e = new QEnvRec(k, v);			// create a new database record
		e->setType(v);								// determine data type and set it
		qEnvList.append(*e);						// add this record to list
		nofEntries++;								// count entries
	}
	ifile.close();									// done.
	qCurIndex = -1; 								// iteration has to be restarted

	cout << "[QEnv::readFile(): " << nofEntries << " entries read from file " << fileName << "]" << endl;
	return nofEntries;
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::writeFile
//! Purpose:        Write env data to file
//! Arguments:      Char_t * fileName     -- name of env file
//! Results:        Int_t nofEntries      -- number of entries written
//!                                          or -1 if error
//! Description:    Writes database to file
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::writeFile(const Char_t * fileName) {

	QString fn; 									// if there is no filename, take the initial one
	if (*fileName == '\0') fn = qEnvFile; else fn = fileName;

	QFile ofile(fn);								// try to open it for output
	if (!ofile.open(QIODevice::ReadWrite)) {
		cerr << "QEnv::writeFile(): Can't open file for writing - " << fn.toAscii().data() << endl;
		return -1;
	}

	Int_t lkey = this->getKeyLength();				// calculate length of longest key name
	Int_t nofEntries = 0;
	for (Int_t i = 0; i < qEnvList.size(); i++) { 	// loop thru database
		QEnvRec e = qEnvList[i];					// next entry
		QString k = e.getKey(); 					// key name
		QString v = e.getValue();					// key value
		QString line = k;							// output format is "<key>:   <value>"
		line += ":";
		for (Int_t j = 0; j < (lkey - k.size()); j++) line += " ";	// pad with blanks
		line += v;
		line += '\n';
		ofile.write(line.toAscii());				// write it to file
		nofEntries++;								// count this entry
	}
	ofile.close();									// all done.
	qCurIndex = -1; 								// iteration has to be restarted

	cout << "[QEnv::writeFile(): " << nofEntries << " entries written to file " << fn.toAscii().data() << "]" << endl;
	return nofEntries;
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::setValue
//! Purpose:        Set or create env entry from integer
//! Arguments:      Char_t * key     -- key name
//!                 Int_t value      -- value
//! Results:        -- 
//! Description:    If entry already exists changes its value.
//!                 Otherwise creates a new entry.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

void QEnv::setValue(const Char_t * key, Int_t value) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);					// check if key already exists
	if (n == 0) {
		QString v;									// not found
		v.setNum(value);							// convert int value to string
		QString k = key;							// define key name
		QEnvRec * e = new QEnvRec(k, v);			// create new database record
		qCurIndex = -1; 							// iteration has to be restarted
		e->setType(QEnvRec::kInteger); 			// type is integer (or octal or hex)
		qEnvList.append(*e);						// add entry to list
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else if (n == 1) {
		QEnvRec r = qEnvList[xl[0]];					// already present at position <n> in list
		QEnvRec::ERecType type = r.getType();		// check if type is integer
		if ((type == QEnvRec::kInteger) == 0) {		// no - warning: type changed
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"int\"" << endl;
		}
		r.setValue(value);							// set new key value
		qEnvList.replace(n, r); 					// replace old entry at position <n> by modified one
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	} else {
		cerr << "QEnv::setValue(): Key " << key << " matches more than one entry" << endl;
	}
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::setValue
//! Purpose:        Set or create env entry from double
//! Arguments:      Char_t * key     -- key name
//!                 Double_t value   -- value
//! Results:        -- 
//! Description:    If entry already exists changes its value.
//!                 Otherwise creates a new entry.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

void QEnv::setValue(const Char_t * key, Double_t value) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);					// check if key already exists
	if (n == 0) {
		QString v;
		v.setNum(value);
		QString k = key;
		QEnvRec * e = new QEnvRec(k, v);
		e->setType(QEnvRec::kDouble);
		qCurIndex = -1; 							// iteration has to be restarted
		qEnvList.append(*e);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else if (n == 1) {
		QEnvRec r = qEnvList[xl[0]];
		QEnvRec::ERecType type = r.getType();
		if (type != QEnvRec::kDouble) {
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"double\"" << endl;
		}
		r.setType(QEnvRec::kDouble);
		r.setValue(value);
		qEnvList.replace(n, r);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	} else {
		cerr << "QEnv::setValue(): Key " << key << " matches more than one entry" << endl;
	}
}


//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::setValue
//! Purpose:        Set or create env entry from string
//! Arguments:      Char_t * key      -- key name
//!                 Char_t *  value   -- value
//! Results:        -- 
//! Description:    If entry already exists changes its value.
//!                 Otherwise creates a new entry.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

void QEnv::setValue(const Char_t * key, const Char_t * value) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);					// check if key already exists
	if (n == 0) {
		QString k = key;
		QString v = value;
		QEnvRec * e = new QEnvRec(k, v);
		e->setType(QEnvRec::kString);
		qCurIndex = -1; 							// iteration has to be restarted
		qEnvList.append(*e);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else if (n == 1) {
		QEnvRec r = qEnvList[xl[0]];
		QEnvRec::ERecType type = r.getType();
		if (type != QEnvRec::kString) {
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"string\"" << endl;
		}
		r.setType(QEnvRec::kString);
		r.setValue(value);
		qEnvList.replace(n, r);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	} else {
		cerr << "QEnv::setValue(): Key " << key << " matches more than one entry" << endl;
	}
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::getValue
//! Purpose:        Get value of given entry
//! Arguments:      Char_t * key        -- key name
//!                 Int_t defVal        -- default value
//! Results:        Int_t result        -- resulting value           
//! Description:    If entry exists *and* its type is int returns value.
//!                 Otherwise returns defVal.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::getValue(const Char_t * key, Int_t defVal) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);						// check if key already exists
	if (n != 1) return defVal; 							// not found or ambiguous: we take default
	QEnvRec r = qEnvList[xl[0]];						// fetch entry from database
	QEnvRec::ERecType type = r.getType();				// check if type is integer
	if (type == QEnvRec::kInteger) {
		return r.getValue().toInt();					// ok, return key value
	} else {											// failed - warning: not the right type
		cerr << "QEnv::getValue(): Key " << r.getKey().toAscii().data() << " - not of type \"int\"" << endl;
		return defVal;									// return default
	}
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::getValue
//! Purpose:        Get value of given entry
//! Arguments:      Char_t * key        -- key name
//!                 Double_t defVal     -- default value
//! Results:        Double_t result     -- resulting value           
//! Description:    If entry exists *and* its type is double returns value.
//!                 Otherwise returns defVal.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Double_t QEnv::getValue(const Char_t * key, Double_t defVal) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);						// check if key already exists
	if (n != 1) return defVal; 							// not found or ambiguous: we take default
	QEnvRec r = qEnvList[xl[0]];						// fetch entry from database
	QEnvRec::ERecType type = r.getType();				// check if type is double
	if (type == QEnvRec::kDouble) {
		return r.getValue().toDouble(); 				// ok, return key val
	} else {											// failed - warning: not the right type
		cerr << "QEnv::getValue(): Key " << r.getKey().toAscii().data() << " - not of type \"double\"" << endl;
		return defVal;
	}
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::getValue
//! Purpose:        Get value of given entry
//! Arguments:      Char_t * key          -- key name
//!                 Char_t * defVal       -- default value
//! Results:        Char_t * result       -- resulting value           
//! Description:    If entry exists returns value (even if type doesn't match).
//!                 Otherwise returns defVal.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

const Char_t * QEnv::getValue(const Char_t * key, const Char_t * defVal) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);						// check if key already exists
	if (n > 1) return defVal; 							// ambiguous: we take default
	return qEnvList[xl[0]].getValue().toAscii().data();
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::lookup
//! Purpose:        Search for a given key
//! Arguments:      Char_t * key        -- name of key
//!                 Bool_t singleMatch  -- kTRUE: look for unique match
//! Results:        QEnvRec & record    -- database record
//! Description:    Searches for a given entry.
//!                 Returns matching record from database.
//!                 Returns empty record
//!                         if key not found
//!                         or singleMatch=kTRUE and more than 1 match
//!                 Empty record can be tested by use of QEnvRec::isEmpty().
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

QEnvRec & QEnv::lookup(const Char_t * key, Bool_t singleMatch) {

	QList<Int_t> xl;
	Int_t n = this->find(key, xl);						// check if key already exists
	if (n == 0) return emptyRec;						// not found: return empty record
	if (singleMatch && (n > 1)) return emptyRec; 		// more than 1 match: return empty record
	return qEnvList[xl[0]]; 							// return matching (=first) entry
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::lookup
//! Purpose:        Search for a given key
//! Arguments:      Char_t * key              -- name of key
//!                 QList<QEnvRec> & recList  -- list of matching records
//! Results:        Int_t nofEntries          -- number of entries in list
//! Description:    Searches for a given entry.
//!                 Returns matching record(s) from database in a list.
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::lookup(const Char_t * key, QList<QEnvRec> & recList) {

	QList<Int_t> xl;
	Int_t n = find(key, xl);
	recList.clear();
	if (n == 0) return(0);
	for (Int_t i = 0; i < n; i++) recList.append(qEnvList[xl[i]]);
	return(n);
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::first
//! Purpose:        Start iteration
//! Arguments:      --
//! Results:        QEnvRec & record     -- first record in database
//! Description:    Starts iteration and returns first record in database
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

QEnvRec & QEnv::first() {

	if (qEnvList.size() == 0) return emptyRec;
	qCurIndex = 0;
	return qEnvList[qCurIndex];
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::next
//! Purpose:        Return next element in list
//! Arguments:      --
//! Results:        QEnvRec & record     -- next record in database
//! Description:    Returns first record in database.
//!                 Use QEnvRec::isEmpty() to check if end of list
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

QEnvRec & QEnv::next() {

	if (qCurIndex == -1) {
		cerr << "QEnv::next(): Iteration not initialized - call QEnv::first()" << endl;
		return emptyRec;
	}
	if (++qCurIndex >= qEnvList.size()) {
		qCurIndex = -1;
		return emptyRec;
	}
	return qEnvList[qCurIndex];
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::find
//! Purpose:        Find entry
//! Arguments:      Char_t * key            -- name of key
//!                 QList<Int_t> & idxList  -- list of indices
//! Results:        Int_t nofEntries        -- number of matching entries
//! Description:    Searches for a given entry
//!                 As keys may contain dot-separated substrings we compare
//!                 these substrings from right to left to check if <key>
//!                 matches (part of) an entry in the list.
//!                 A substring denotes a wildcard by setting it to "*".
//! Keywords:
//! \endverbatim
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::find(const Char_t * key, QList<Int_t> & idxList) {

	QString ks = key;
	QStringList kl = ks.split(".");
	Int_t klng = kl.size();

	idxList.clear();
	for (Int_t i = 0; i < qEnvList.size(); i++) {
		QString es = qEnvList[i].getKey();
		QStringList el = es.split(".");
		Int_t elng = el.size();
		if (klng > elng) continue;
		Bool_t match = kTRUE;
		Int_t ex = elng - 1;
		for (Int_t kx = klng - 1; kx >= 0; kx--, ex--) {
			if (el[ex] == "*") continue;
			if (kl[kx] != el[ex]) { match = kFALSE; break; }
		}
		if (match) idxList.append(i);
	}
	return idxList.size();
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::print
//! Purpose:        Print entries
//! Arguments:      --
//! Results:        --      
//! Description:    Outputs all entries of QEnv database to cout.
//! Keywords:
//! \endverbatim
//! \example rootrc.list
//////////////////////////////////////////////////////////////////////////////

void QEnv::print() {

	Int_t lkey = this->getKeyLength();
	cout << endl << "Environment read from file " << qEnvFile.toAscii().data() << ":" << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	printf("%-3s    %-*s %-6s %s\n", "idx", lkey, "key", "type", "value");
	cout << "-------------------------------------------------------------------------------" << endl;
	for (Int_t i = 0; i < qEnvList.size(); i++) {
		QEnvRec e = qEnvList[i];
		e.print(i, lkey);
	}
	cout << endl;
}

//__________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \verbatim
//! Name:           QEnv::getKeyLength                                      
//! Purpose:        Calculate length of key names                           
//! Arguments:      --                                                      
//! Results:        Int_t lkey           -- length of longest key name        
//! Description:    Calcuates longest length of key                            
//! Keywords: 
//! \endverbatim                                                              
//////////////////////////////////////////////////////////////////////////////

Int_t QEnv::getKeyLength() {

	Int_t lkey = 0;
	for (Int_t i = 0; i < qEnvList.size(); i++) {
		QString k = qEnvList[i].getKey();
		if (k.size() > lkey) lkey = k.size();
	}
	lkey += 2;
	return lkey;
}
