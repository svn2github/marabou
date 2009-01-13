//_________________________________________________________[QT IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv.cxx
// Purpose:        QT utilities:
//                 A class handling environment files used with ROOT (like TEnv)
// Description:    Implements class methods to manage ROOT's TEnv files
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: QEnv.cxx,v 1.1 2009-01-13 07:41:38 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <QFile>
#include <QByteArray>
#include <QStringList>

#include "QEnv.h"

using namespace std;

QEnvRec::QEnvRec(QString & key, QString & value) {
//__________________________________________________________________[QT CLASS]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnvRec::QEnvRec
// Purpose:        Instantiate a record to be used in QEnv database
// Arguments:      QString & key       -- key
//                 QString & value     -- value
// Description:    Contains a single entry in QEnv database
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	qKey = key;
	qValue = value;
	qRecType = QEnvRec::isString;		// default: string
};

void QEnvRec::setType(QString & value) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnvRec::setType
// Purpose:        Set entry type according to data type
// Arguments:      QString & value     -- value
// Results:        -- 
// Description:    Checks if data type is int, double, or string.
//                 Defaults to string.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	QString v = value;
	v += " 0";									// append "<space>0"
	int v1, v2;
	istringstream istr(v.toAscii().data()); 	// then decode using format "<int> <int>"
	istr >> v1 >> v2;							// check if we got 2 ints: v1 should be same as <value>, v2 should be "0"
	if (v2 == 0 && v1 == strtol(value.toAscii().data(), NULL, 10)) { qRecType = QEnvRec::isInteger; return; }
	double v3;									// didn't work: try format "<double> <int>"
	istringstream dstr(v.toAscii().data());
	dstr >> v3 >> v2;							// check if we got 1 double and 1 int: v1 should be same as <value>, v2 should be "0"
	if (v2 == 0 && v3 == strtod(value.toAscii().data(), NULL)) { qRecType = QEnvRec::isDouble; return; }
	qRecType = QEnvRec::isString;				// didn't work: take string as default
}

void QEnvRec::print(int recNo, int lkey) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnvRec::setType
// Purpose:        Set entry type according to data type
// Arguments:      QString & value     -- value
// Results:        -- 
// Description:    Checks if data type is int, double, or string.
//                 Defaults to string.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	QString type;
	switch (this->getType()) {
		case QEnvRec::isNew: return;
		case QEnvRec::isInteger: type = "int"; break;
		case QEnvRec::isDouble: type = "dbl"; break;
		case QEnvRec::isString: type = "str"; break;
	}
	if (recNo == -1) {
		printf("%-*s [%-3s]  %s\n", lkey, this->getKey().toAscii().data(), type.toAscii().data(), this->getValue().toAscii().data());
	} else {
		printf("%3d    %-*s [%-3s]  %s\n", recNo, lkey, this->getKey().toAscii().data(), type.toAscii().data(), this->getValue().toAscii().data());
	}
}

QEnv::QEnv(const char * fileName, bool verbose) {
//__________________________________________________________________[QT CLASS]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::QEnv
// Purpose:        Instantiate QEnv database
// Arguments:      const char * fileName  -- name of env file
//                 bool verbose           -- turn verbose flag on/off
// Description:    Main QEnv object
// Keywords:
//////////////////////////////////////////////////////////////////////////////
	qVerbose = verbose;
	if (this->readFile(fileName) > 0) qEnvFile = fileName;
}

int QEnv::readFile(const char * fileName) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::readFile
// Purpose:        Read env data from file
// Arguments:      const char * fileName   -- name of env file
// Results:        int nofEntries          -- number of entries read
//                                            or -1 if error
// Description:    Reads env dat line by line and builds database
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	QFile ifile(fileName);
	if (!ifile.open(QIODevice::ReadOnly)) { 		// try to open file for input
		cerr << "QEnv::readFile(): Can't open file for reading - " << fileName << endl;
		return -1;
	}

	int nofEntries = 0;
	while (!ifile.atEnd()) {						// read line by line
		QString line = ifile.readLine();
		line.remove(line.size() - 1, 1);			// remove traling <lf>
		line = line.simplified();					// skip surrounding spaces
		if (line.isEmpty()) continue;				// nothing in there
		QStringList l = line.split("#");			// look for comments
		if (l.size() > 1) line = l[0];				// we have one: skip it
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
	cout << "[QEnv::readFile(): " << nofEntries << " entries read from file " << fileName << "]" << endl;
	return nofEntries;
}

int QEnv::writeFile(const char * fileName) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::writeFile
// Purpose:        Write env data to file
// Arguments:      const char * fileName   -- name of env file
// Results:        int nofEntries          -- number of entries written
//                                            or -1 if error
// Description:    Writes database to file
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	QString fn; 									// if there is no filename, take the initial one
	if (*fileName == '\0') fn = qEnvFile; else fn = fileName;

	QFile ofile(fn);								// try to open it for output
	if (!ofile.open(QIODevice::ReadWrite)) {
		cerr << "QEnv::writeFile(): Can't open file for writing - " << fn.toAscii().data() << endl;
		return -1;
	}

	int lkey = this->getKeyLength();				// calculate length of longest key name
	int nofEntries = 0;
	for (int i = 0; i < qEnvList.size(); i++) { 	// loop thru database
		QEnvRec e = qEnvList[i];					// next entry
		QString k = e.getKey(); 					// key name
		QString v = e.getValue();					// key value
		QString line = k;							// output format is "<key>:   <value>"
		line += ":";
		for (int j = 0; j < (lkey - k.size()); j++) line += " ";	// pad with blanks
		line += v;
		line += '\n';
		ofile.write(line.toAscii());				// write it to file
		nofEntries++;								// count this entry
	}
	ofile.close();									// all done.
	cout << "[QEnv::writeFile(): " << nofEntries << " entries written to file " << fn.toAscii().data() << "]" << endl;
	return nofEntries;
}

void QEnv::setValue(const char * key, int value) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::setValue
// Purpose:        Set or create env entry from integer
// Arguments:      const char * key   -- key name
//                 int value          -- value
// Results:        -- 
// Description:    If entry already exists changes its value.
//                 Otherwise creates a new entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);						// check if key already exists
	if (n == -1) {
		QString v;									// not found
		v.setNum(value);							// convert int value to string
		QString k = key;							// define key name
		QEnvRec * e = new QEnvRec(k, v);			// create new database record
		e->setType(QEnvRec::isInteger); 			// type is integer
		qEnvList.append(*e);						// add entry to list
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else {
		QEnvRec r = qEnvList[n];					// already present at position <n> in list
		QEnvRec::ERecType type = r.getType();		// check if type is integer
		if (type != QEnvRec::isInteger) {			// no - warning: type changed
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"int\"" << endl;
		}
		r.setValue(value);							// set new key value
		qEnvList.replace(n, r); 					// replace old entry at position <n> by modified one
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	}
}

void QEnv::setValue(const char * key, double value) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::setValue
// Purpose:        Set or create env entry from double
// Arguments:      const char * key   -- key name
//                 double value       -- value
// Results:        -- 
// Description:    If entry already exists changes its value.
//                 Otherwise creates a new entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);
	if (n == -1) {
		QString v;
		v.setNum(value);
		QString k = key;
		QEnvRec * e = new QEnvRec(k, v);
		e->setType(QEnvRec::isDouble);
		qEnvList.append(*e);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else {
		QEnvRec r = qEnvList[n];
		QEnvRec::ERecType type = r.getType();
		if (type != QEnvRec::isDouble) {
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"double\"" << endl;
		}
		r.setType(QEnvRec::isDouble);
		r.setValue(value);
		qEnvList.replace(n, r);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	}
}


void QEnv::setValue(const char * key, const char * value) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::setValue
// Purpose:        Set or create env entry from string
// Arguments:      const char * key      -- key name
//                 const char *  value   -- value
// Results:        -- 
// Description:    If entry already exists changes its value.
//                 Otherwise creates a new entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);
	if (n == -1) {
		QString k = key;
		QString v = value;
		QEnvRec * e = new QEnvRec(k, v);
		e->setType(QEnvRec::isString);
		qEnvList.append(*e);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Creating entry \"" << key << "\", value = " << value << endl;
	} else {
		QEnvRec r = qEnvList[n];
		QEnvRec::ERecType type = r.getType();
		if (type != QEnvRec::isString) {
			cerr << "QEnv::setValue(): Key " << r.getKey().toAscii().data() << " - changed type to \"string\"" << endl;
		}
		r.setType(QEnvRec::isString);
		r.setValue(value);
		qEnvList.replace(n, r);
		if (this->isVerbose()) cout << "[QEnv::setValue():: Changing entry \"" << key << "\", value = " << value << endl;
	}
}

int QEnv::getValue(const char * key, int defVal) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::getValue
// Purpose:        Get value of given entry
// Arguments:      const char * key      -- key name
//                 int defVal            -- default value
// Results:        int result            -- resulting value           
// Description:    If entry exists and its type is int returns value.
//                 Otherwise returns defVal.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);							// check if key is present
	if (n == -1) return defVal; 						// not found: we take default
	QEnvRec r = qEnvList[n];							// fetch entry from database
	QEnvRec::ERecType type = r.getType();				// check if type is integer
	if (type == QEnvRec::isInteger) {
		return r.getValue().toInt();					// ok, return key value
	} else {											// failed - warning: not the right type
		cerr << "QEnv::getValue(): Key " << r.getKey().toAscii().data() << " - not of type \"int\"" << endl;
		return defVal;									// return default
	}
}

double QEnv::getValue(const char * key, double defVal) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::getValue
// Purpose:        Get value of given entry
// Arguments:      const char * key      -- key name
//                 double defVal         -- default value
// Results:        double result         -- resulting value           
// Description:    If entry exists and its type is double returns value.
//                 Otherwise returns defVal.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);
	if (n == -1) return defVal;
	QEnvRec r = qEnvList[n];
	QEnvRec::ERecType type = r.getType();
	if (type == QEnvRec::isDouble) {
		return r.getValue().toDouble();
	} else {
		cerr << "QEnv::getValue(): Key " << r.getKey().toAscii().data() << " - not of type \"double\"" << endl;
		return defVal;
	}
}

const char * QEnv::getValue(const char * key, const char * defVal) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::getValue
// Purpose:        Get value of given entry
// Arguments:      const char * key      -- key name
//                 const char * defVal   -- default value
// Results:        const char * result   -- resulting value           
// Description:    If entry exists and its type is string returns value.
//                 Otherwise returns defVal.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int n = this->find(key);
	if (n == -1) return defVal;
	QEnvRec r = qEnvList[n];
	QEnvRec::ERecType type = r.getType();
	if (type == QEnvRec::isString) {
		return r.getValue().toAscii().data();
	} else {
		cerr << "QEnv::getValue(): Key " << r.getKey().toAscii().data() << " - not of type \"string\"" << endl;
		return defVal;
	}
}

void QEnv::print() {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::print
// Purpose:        Print entries
// Arguments:      --
// Results:        --      
// Description:    Prints all entries of QEnv database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int lkey = this->getKeyLength();
	cout << endl << "Environment read from file " << qEnvFile.toAscii().data() << ":" << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	printf("%-3s    %-*s %-6s %s\n", "idx", lkey, "key", "type", "value");
	cout << "-------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < qEnvList.size(); i++) {
		QEnvRec e = qEnvList[i];
		e.print(i, lkey);
	}
	cout << endl;
}

int QEnv::find(const char * key) {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::find
// Purpose:        Print entries
// Arguments:      const char * key    -- name of key
// Results:        int index           -- index of key in database
//                                        or -1 if not found      
// Description:    Searches for a given entry
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < qEnvList.size(); i++) {
		if (qEnvList[i].getKey() == key) return i;
	}
	return -1;
}

int QEnv::getKeyLength() {
//_________________________________________________________________[QT METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           QEnv::getKeyLength
// Purpose:        Calculate length of key names
// Arguments:      --
// Results:        int lkey           -- length of longest key name
// Description:    Calcuates late length of ke
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	int lkey = 0;
	for (int i = 0; i < qEnvList.size(); i++) {
		QString k = qEnvList[i].getKey();
		if (k.size() > lkey) lkey = k.size();
	}
	lkey += 2;
	return lkey;
}
