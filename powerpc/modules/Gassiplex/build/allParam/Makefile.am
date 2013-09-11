VPATH = @srcdir@ @srcdir@/ca/client @srcdir@/ca/server @srcdir@/file \
 @srcdir@/ora @srcdir@/tcl

#SUFFIXES += .pc

PROC=@PROC@
PROCFLAGS=@PROCFLAGS@
PROC_INCLUDES=@PROC_INCLUDES@ include=@top_srcdir@/include

.pc.c :
	$(PROC) $(PROCFLAGS) $(PROC_INCLUDES) iname=$< oname=$@

include_HEADERS = @ALLPARAM_H@
EXTRA_HEADERS = include/allParam.h

bin_PROGRAMS = @PAS@
EXTRA_PROGRAMS = pas

lib_LIBRARIES = @LIBCAPARAM_A@ @LIBFILEPARAM_A@ @LIBORAPARAM_A@ @LIBTCLPARAM_A@
EXTRA_LIBRARIES = libcaParam.a libfileParam.a liboraParam.a libtclParam.a

libcaParam_a_SOURCES = caParam.c
libfileParam_a_SOURCES = fileParam.c
liboraParam_a_SOURCES = oraParam.c
libtclParam_a_SOURCES = tclParam.c
pas_SOURCES = arrayDest.cc arrayDest.h paramBlobRecord.cc paramBlobRecord.h \
 paramFilenameRecord.cc paramFilenameRecord.h \
 paramIntRecord.cc paramIntRecord.h paramRecord.cc paramRecord.h \
 paramRecordSet.cc paramRecordSet.h paramStringRecord.cc paramStringRecord.h \
 record.cc record.h paramServer.cc

