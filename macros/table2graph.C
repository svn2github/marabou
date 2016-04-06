//
// This program reads a datafile in CSV format a la GNU Plot
// and generates a TGraph.
// In simple cases ([y], [x,y], [x,y,ey] only the
// datafile is needed if no column selection is required
//
// Calling sequence:
//	int table2graph(const char * datafile,		// input file
//						const char * option = "",	// x-y errors selection
//						const char * columns = "")	// list of columns
//
// The datafile may have the following columns:
// 
// y					yvalues only, xvalues are auto provided: 0, 1, 2, ..
// x, y				simple graph, no error bars      
// x, y, ey			symmetric errors y
// x, y, ex, ey	symmetric errors x, y, needs option "XY"
// x, y, ey1, ey2	asymmetric errors y
// x, y, ex			symmetric errors x  , needs option "X"
// x, y, ex1, ex2	asymmetric errors x , needs option "X"
// x, y, ex1, ex2, ey1, ey2   asymmetric errors x and y
//
// Separators may be spaces or commas.
// lines starting with #, !, *, // are taken as comment and skipped
// lines may have embedded text (non floats), text within quotes
// like "line No 3" is completely ignored
//
// The options "X", "XY" are needed to resolve the ambiguity if
// 3 or 4 columns are provided, 
//
// The argument "columns" can be used to select columns in the datafile:
//	e.g. "2:4:5"  : use colums 2, 4 and 5 for e.g. x, y, ey
//	Default is to use all columns from the beginning
// If the file contains more columns than should be used this argument
// is compulsary
//
// The generated graph is written to a ROOT file opened in UPDATE mode
// The ROOT filename is constructed from the datafile name with
// its extension replaced by ".root"
//
// Author: Otto.Schaile@lmu.de
//____________________________________________________________________

#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TRegexp.h"
#include <iostream>
#include <fstream>

namespace std {} using namespace std;
//_____________________________________________________________________

Int_t ValueIsIn(Int_t *arr, Int_t maxv, Int_t val)
{
	for (Int_t i = 0; i < maxv; i++) {
		if (val == arr[i])
			return i;
	}
	return -1;
}
//_____________________________________________________________________

Int_t GetValues(const char *l, Double_t *val, Int_t maxv,
						Int_t * col_select, Int_t maxs) 
{
	TString line(l);
	TString sval;
	TRegexp inquotes("\".*\"");	// matches any quoted text incl " "
	TRegexp space(" +");				// matches any number of spaces
	Ssiz_t len, start = 0;
	// remove text enclosed in quotes
	while ( 1 ) {
		start = inquotes.Index(line, &len);
		if (start < 0 )
			break;
		line.Replace(start, len, " ");
	}
	while ( line.Contains(" ") ) {
		line(space) = ",";
	}
//	cout << line << endl;
	Bool_t ok;
	Int_t vcount = 0;  // count columns in input
	Int_t ccount = 0;  // count selected columns 
	const Char_t * del = ",";
	start = 0;
	while ( (ok = line.Tokenize(sval ,start, del) ) ) {
		// use data field if it is a float, discard otherwise
		if ( sval.IsFloat() ) {
			if (maxs > 0) {
				Int_t ind = ValueIsIn(col_select, maxs, vcount);
				if (ind >= 0) {
					val[ind] = sval.Atof();
					ccount++;
				}
				if (ccount >= maxs) {
//					cerr << "Too many values in: " << l << endl;
					break;
				}
			} else {
				val[ccount] = sval.Atof();
				ccount++;
			}
			vcount ++;
		}
	}
	if ( maxs > 0 && ccount < maxs ) {
		cerr << "Not enough values in: \"" << l << "\" need: "
		<< maxs << endl;
		ccount = -1;
	}
	return ccount;
}
//____________________________________________________________________

Int_t table2graph(const char * datafile,
						  const char * option = "",
						  const char * columns = "")
{
	enum {k_yonly, k_xy, k_xy_ey, k_xy_ex, k_xy_exey,
			k_xy_aey, k_xy_aex, k_xy_aex_aey};
			
	TString sopt(option);	
	const Int_t maxcol = 6;
	TString line;

// 
	TGraph * graph = NULL;
	TGraph * gr = NULL;
	TGraphErrors * grerr = NULL;
	TGraphAsymmErrors * graerr = NULL;
	
	Int_t nval, ncolumns = 0, ncolumns_sel = 0,line_nr = -1, point_nr = 0;
	Int_t kind = -1;
	Double_t values[maxcol];
	Int_t col_select[maxcol];
	TString scolumns(columns);
	// analyze requested column selection if any 
	if (scolumns.Length() > 0 ) {
		for (Int_t i=0; i < maxcol; i++) {
			col_select[i] = 0;
		}
		const char * del = ":";
		Int_t start = 0, vcount = 0;
		TString num;
		Bool_t ok;
		while ( (ok = scolumns.Tokenize(num ,start, del) ) ) {
//			cout << " " << num<< endl;
			if ( num.IsDec() ) {
				col_select[vcount] = num.Atoi();
				vcount ++;
				if (vcount >= maxcol) {
					cerr << "Too many values in: " << columns << endl;
					return -4;
				}
			}
		}
		if ( vcount == 0 ) {
			cerr << "No valid numbers in: " << columns << endl;
			return -5;
		} else {
			ncolumns_sel = vcount;
			cout << "Columns selectet:";
			for ( Int_t i = 0; i < ncolumns_sel; i++ ) {
				cout << " " << col_select[i];
			}
			cout << endl;
		}
	} else {	
		for (Int_t i=0; i < maxcol; i++) {
			col_select[i] = i;
		}
		ncolumns_sel = 0;
	}
	
	ifstream infile;
   infile.open(datafile, ios::in);
	if (!infile.good()) {
		cerr	<< "gnpl_datafile: " << gSystem->GetError() 
				<< " - " << datafile << endl;
		return -1;
	}
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof())
			break;
		line_nr++;
		line = line.Strip(TString::kBoth);
		if (line.BeginsWith("#") || line.BeginsWith("*")
			|| line.BeginsWith("!") || line.BeginsWith("//") )
			continue;
		Int_t nval = GetValues(line.Data(), values, maxcol,
										col_select, ncolumns_sel); 
//		cout << nval << " " << line << endl;
		if ( nval < 0 )
			break;
		if (nval == 0 )
			continue;
// first line , try to find which kind of Graph is needed
		if ( ncolumns == 0 ) {
			cout << "Cols used: " <<nval<< " First row: \" " << line << "\"" << endl;
			ncolumns = nval;
			if ( nval == 1 ) {
				// case: only y values given
				kind = k_yonly;
				
			} else if (nval == 2 ) {
				// simple graph: x, y values 
				kind = k_xy;
				
			} else if (nval == 3 ) {
				
				if (sopt.Contains("X")) {
					// x, y, error x
					kind = k_xy_ex;
				
				} else {
					// x, y, error y
					kind = k_xy_ey;
				}
			} else if (nval == 4 ) {
				if (sopt.Contains("XY")) {
					// x, y, error x, y
					kind = k_xy_exey;
				} else if (sopt.Contains("X")) {
					// x, y, assym error x
					kind = k_xy_aex;
				} else {
					// x, y, assym error y
					kind = k_xy_aey;
				}
			} else if (nval == 6 ) {
				// x, y, assym error x, y
				kind = k_xy_aex_aey;
			} else {
				cerr 	<< "At line: " << line_nr << " illegal number of values: "
				<< nval << " should be <= 6 " << endl;
				return -2;
			}
			// construct graph according to number of columns provided
			if (kind ==  k_yonly || kind == k_xy) {
				gr = new TGraph();
				graph = gr;
			} else if (kind == k_xy_ex || kind == k_xy_ey || kind == k_xy_exey) {
				grerr = new TGraphErrors();
				graph = grerr;
			} else {
				graerr = new TGraphAsymmErrors();
				graph = graerr;
			}
		} else {
			if ( nval != ncolumns ) {
				cerr << "At line: " << line_nr << " illegal number of values: "
				<< nval << " should be " << ncolumns << endl;
				return -3;
			}
		}
		// now add point to the appropriate graph
		
		if      (kind == k_yonly ) {
			gr->SetPoint(point_nr, (Double_t)point_nr, values[0]);
		} else if (kind == k_xy ) {
			gr->SetPoint(point_nr, values[0], values[1]);
		} else if (kind == k_xy_ex ) {
			grerr->SetPoint(point_nr, values[0], values[1]);
			grerr->SetPointError(point_nr, values[2], 0);
		} else if (kind == k_xy_ey ) {
			grerr->SetPoint(point_nr, values[0], values[1]);
			grerr->SetPointError(point_nr, 0, values[2]);
		} else if (kind == k_xy_exey ) {
			grerr->SetPoint(point_nr, values[0], values[1]);
			grerr->SetPointError(point_nr, values[2], values[3]);
		} else if (kind == k_xy_aex ) {
			graerr->SetPoint(point_nr, values[0], values[1]);
			graerr->SetPointError(point_nr, values[2], values[3], 0, 0);
		} else if (kind == k_xy_aey ) {
			graerr->SetPoint(point_nr, values[0], values[1]);
			graerr->SetPointError(point_nr, 0, 0, values[2], values[3]);
		} else if (kind == k_xy_aex_aey ) {
			graerr->SetPoint(point_nr, values[0], values[1]);
			graerr->SetPointError(point_nr, values[2], values[3], 
														values[4], values[5]);
		}
		point_nr ++;
	}
	// display graph and write to ROOT file
	if (graph) {
		TString title(datafile);
		if (scolumns.Length() > 0 ) {
			title += " col";
			if (scolumns.Contains(":"))
				title += "s";
			title += ": ";
			title += columns;
		}
		graph->SetTitle(title);
		TString fname(datafile);
		Int_t sl;
		TRegexp suf("\\..*$");
		fname(suf) = "";
		graph->SetName(fname);
		graph->SetMarkerStyle(20);
		graph->SetMarkerSize(.8);
		graph->Draw("AP");
		
		fname += ".root";
		TFile * outfile = new TFile(fname, "UPDATE");
		graph->Write();
		outfile->Close();
	}	
	cout << "Number of rows used: ";
	return point_nr;
}

