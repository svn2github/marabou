#ifdef __CINT__
//___________________________________________________________[CINT DIRECTIVES]
//////////////////////////////////////////////////////////////////////////////
// @Name:          ExaAnalyzeLinkDef.h
// @Purpose:       Directives telling CINT how to compile ExaAnalyze
//                 To be used with user files ExaAnalyze.cxx/.h
// Description:    Contains a "#pragma link C++ class" statement
//                 for each user-defined class
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// classes supplied by system
#pragma link C++ class TMrbIOSpec+;
#pragma link C++ class TMrbAnalyze+;
#pragma link C++ class TUsrEvent+;
#pragma link C++ class TUsrEvtStart+;
#pragma link C++ class TUsrEvtStop+;
// classes defined by user
#pragma link C++ class TUsrEvtReadout+;
#pragma link C++ class TUsrSevtData+;
#endif
