/********************************************************************
* MRevDict.h
********************************************************************/
#ifdef __CINT__
#error MRevDict.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableMRevDict();
extern void G__cpp_setup_inheritanceMRevDict();
extern void G__cpp_setup_typetableMRevDict();
extern void G__cpp_setup_memvarMRevDict();
extern void G__cpp_setup_globalMRevDict();
extern void G__cpp_setup_memfuncMRevDict();
extern void G__cpp_setup_funcMRevDict();
extern void G__set_cpp_environmentMRevDict();
}


#include "TROOT.h"
#include "TMemberInspector.h"
#include "MRevBuffer.h"

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__MRevDictLN_TClass;
extern G__linked_taginfo G__MRevDictLN_TObject;
extern G__linked_taginfo G__MRevDictLN_TSocket;
extern G__linked_taginfo G__MRevDictLN_REvent;
extern G__linked_taginfo G__MRevDictLN_MRevBuffer;

/* STUB derived class for protected member access */
