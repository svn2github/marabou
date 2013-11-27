#ifdef __CINT__
//___________________________________________________________[CINT DIRECTIVES]
//////////////////////////////////////////////////////////////////////////////
// @Name:          SResAnalyzeLinkDef.h
// @Purpose:       Directives telling CINT how to compile SResAnalyze
//                 To be used with user files SResAnalyze.cxx/.h
// Description:    Contains a "#pragma link C++ class" statement
//                 for each user-defined class
// Author:         lshabs
// Revision:         
// Date:           Mon Nov 25 18:12:34 2013
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class Motor+;
#endif
