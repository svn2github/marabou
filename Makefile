# Top level Makefile for Marabou System
# Copyright (c) 2000 Rudi Lutter and Otto Schaile
#
# Author: Otto stolen from ROOT (Fons Rademakers), 15/9/2000

MAKE_VERSION_MAJOR := $(word 1,$(subst ., ,$(MAKE_VERSION)))
MAKE_VERSION_MINOR := $(shell echo $(word 2,$(subst ., ,$(MAKE_VERSION))) | \
                              sed 's/\([0-9][0-9]*\).*/\1/')
MAKE_VERSION_MAJOR ?= 0
MAKE_VERSION_MINOR ?= 0
ORDER_ := $(shell test $(MAKE_VERSION_MAJOR) -gt 3 || \
                  test $(MAKE_VERSION_MAJOR) -eq 3 && \
                  test $(MAKE_VERSION_MINOR) -ge 80 && echo '|')


include config/Makefile.config

ifeq ($(EXPLICITLINK),yes)
include config/Makefile.depend
endif

##### include machine dependent macros #####

include config/Makefile.$(ARCH)

##### allow local macros #####

-include MyConfig.mk

EXTRA_CFLAGS   += -g -Wno-switch -Wno-write-strings -Wno-parentheses -I$(ROOTSYS)/include
EXTRA_CXXFLAGS += -g -Wno-switch -Wno-write-strings -Wno-parentheses -I$(ROOTSYS)/include

##### Modules to build #####

# only selected modules need to checked out from CVS if
# not the complete marabou suite is required.

# for HistPresent only the following modules are needed

MODULES       = build utils gutils helpbrowser fitcal gredit hpr

# if offline data analysis in the marabou framework is needed

ifeq ($(findstring $(MAKECMDGOALS),hpr),)
	ifeq ($(shell if [ -d analyze ] ; then echo yes; fi), yes)
	MODULES      += c_analyze analyze mbssetup mbsio transport
	endif

	ifeq ($(shell if [ -f .useXML ] ; then echo yes; fi), yes)
	ifeq ($(shell if [ -d expconf-xml ] ; then echo yes; fi), yes)
	MODULES      += expconf-xml
	endif
	ifeq ($(shell if [ -d xml ] ; then echo yes; fi), yes)
	MODULES      += xml
	endif
	else
	ifeq ($(shell if [ -d expconf ] ; then echo yes; fi), yes)
	MODULES      += expconf
	endif
	endif

	ifeq ($(shell if [ -d tidy ] ; then echo yes; fi), yes)
	MODULES      += tidy tidylib
	endif

	ifeq ($(shell if [ -d gutils-new ] ; then echo yes; fi), yes)
	MODULES      += gutils-new
	endif

	ifeq ($(shell if [ -d macrobrowser ] ; then echo yes; fi), yes)
	MODULES      += macrobrowser
	endif

	ifeq ($(shell if [ -d dgfcomm ] ; then echo yes; fi), yes)
	MODULES      += dgfcomm
	endif

	ifeq ($(shell if [ -d c2lynx ] ; then echo yes; fi), yes)
	MODULES      += c2lynx
	endif

	# if online data acquisition is needed in addition

	ifeq ($(shell if [ -d camcli  ] ; then echo yes; fi), yes)
	MODULES      += camcli esone
	endif

	# the following modules are for special hardware

	ifeq ($(shell if [ -d vmecontrol ] ; then echo yes; fi), yes)
	MODULES      += vmecontrol
	endif

	ifeq ($(shell if [ -d xiadgf ] ; then echo yes; fi), yes)
	MODULES      += xiadgf dgfcontrol cptmcontrol
	endif

	ifeq ($(shell if [ -d polar ] ; then echo yes; fi), yes)
	MODULES      += polar
	endif

	#ifeq ($(shell if [ -d snake-lib ] ; then echo yes; fi), yes)
	#MODULES      += snake-lib
	#endif

	#ifeq ($(shell if [ -d snake-pgm ] ; then echo yes; fi), yes)
	#MODULES      += snake-pgm
	#endif
endif

##### ROOT libraries #####

LPATH         = lib

# no windows support (for the time beeing)
# ifneq ($(ARCH),win32)

RPATH        := -L$(LPATH)
rpath        := -L$(LPATH)

ROOTCFLAGS    := $(shell root-config --cflags)
ROOTLIBS      := $(shell root-config --libs)
#ROOTLIBS      := $(shell root-config --new --libs)
ROOTLIBS      += -lGed
ROOTGLIBS     := $(shell root-config --glibs)
#ROOTGLIBS     := $(shell root-config --new --glibs)
ROOTGLIBS      += -lGed
ROOTGLIBS      += -lHistPainter

ROOTCINT      :=rootcint

#ROOTVERS contains / . and sometimes letters, remove them

RV1 	      := $(shell root-config --version)
RV1           := $(subst /,,$(RV1))
RV1           := $(subst .,,$(RV1))
RV1           := $(subst a,,$(RV1))
RV1           := $(subst b,,$(RV1))
RV1           := $(subst c,,$(RV1))
RV1           := $(subst d,,$(RV1))
RV1           := $(subst e,,$(RV1))
RV1           := $(subst f,,$(RV1))
RV1           := $(subst g,,$(RV1))
ROOTVERS      := $(subst h,,$(RV1))

##### clang or gcc version #####

ifneq ($(findstring clang,$(CXX)),)
CLANG_MAJOR  := $(shell $(CXX) -v 2>&1 | awk '{if (NR==1) print $$3}' | cut -d'.' -f1)
CLANG_MINOR  := $(shell $(CXX) -v 2>&1 | awk '{if (NR==1) print $$3}' | cut -d'.' -f2)
ifeq ($(CLANG_MAJOR),version)
   # Apple version of clang has different -v layout
   CLANG_MAJOR  := $(shell $(CXX) -v 2>&1 | awk '{if (NR==1) print $$4}' | cut -d'.' -f1)
   CLANG_MINOR  := $(shell $(CXX) -v 2>&1 | awk '{if (NR==1) print $$4}' | cut -d'.' -f2)
endif
else
ifneq ($(findstring gnu,$(COMPILER)),)
GCC_MAJOR     := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR     := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_PATCH     := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f3)
GCC_VERS      := gcc-$(GCC_MAJOR).$(GCC_MINOR)
GCC_VERS_FULL := gcc-$(GCC_MAJOR).$(GCC_MINOR).$(GCC_PATCH)
endif
endif

##### utilities #####

MAKEDEP       = build/unix/depend.sh
MAKELIB       = build/unix/makelib.sh
MAKEDIST      = build/unix/makedist.sh
MAKEVERSION   = build/unix/makeversion.sh
IMPORTCINT    = build/unix/importcint.sh
MAKECOMPDATA  = build/unix/compiledata.sh
MAKEMAKEINFO  = build/unix/makeinfo.sh
MAKECHANGELOG = build/unix/makechangelog.sh
MAKEHTML      = root build/unix/makehtml.C
HTMLCLEANUP   = build/unix/htmlcleanup.sh
MAKELOGHTML   = build/unix/makeloghtml.sh
MAKECINTDLLS  = build/unix/makecintdlls.sh
ifeq ($(ARCH),win32)
MAKELIB       = build/win/makelib.sh
MAKEDIST      = build/win/makedist.sh
MAKECOMPDATA  = build/win/compiledata.sh
MAKEMAKEINFO  = build/win/makeinfo.sh
endif

##### compiler directives #####

COMPILEDATA   = include/compiledata.h
MAKEINFO      = cint/MAKEINFO

##### all #####

ALLHDRS      :=
ALLLIBS      :=
# HPRLIBS      := lib/libHpr.so lib/libTMrbHelpBrowser.so lib/libTMrbUtils.so lib/libTGMrbUtils.so lib/libGrEdit.so lib/libFitCal.so
ALLEXECS     :=
INCLUDEFILES :=
ALLOBJ       :=

INSTALLFILES	=	build/unix/installfiles.sh

##### RULES #####

.SUFFIXES: .cxx .d
.PRECIOUS: include/%.h obj/%.o

%.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) -DMARABOUVERS=1 -DROOTVERSION=$(ROOTVERS) -o $@ -c $<

%.o: %.c
	$(CC) $(OPT) $(CFLAGS) -o $@ -c $<


##### TARGETS #####

.PHONY:         all fast config  maraboulibs marabouexecs dist distsrc \
                clean distclean compiledata version html \
                install showbuild \
                $(patsubst %,all-%,$(MODULES)) \
                $(patsubst %,clean-%,$(MODULES)) \
                $(patsubst %,distclean-%,$(MODULES))

all:            marabouexecs

hpr:            hprexecs

fast:           marabouexecs

HistPresent:    hprexecs

include $(patsubst %,%/Module.mk,$(MODULES))

-include MyRules.mk            # allow local rules

ifeq ($(findstring $(MAKECMDGOALS),clean distclean dist distsrc version \
      importcint install showbuild changelog html),)
ifeq ($(findstring $(MAKECMDGOALS),fast),)
include $(INCLUDEFILES)
endif
include build/dummy.d          # must be last include
endif

hprexecs:          compiledata bin/HistPresent

maraboulibs:       compiledata $(ALLLIBS)

marabouexecs:      maraboulibs $(ALLEXECS)

compiledata:       $(COMPILEDATA) $(MAKEINFO)

config config/Makefile.:
	@(if [ ! -f config/Makefile.config ] ; then \
	   echo ""; echo "Please, run ./configure first"; echo ""; \
	   exit 1; \
	fi)

$(COMPILEDATA): config/Makefile.$(ARCH)

	@$(MAKECOMPDATA) $(COMPILEDATA) $(CXX) "$(OPT)" "$(CXXFLAGS)" \
	   "$(SOFLAGS)" "$(LDFLAGS)" "$(SOEXT)" "$(SYSLIBS)" "$(LIBDIR)" \
	   "$(ROOTLIBS)" "$(RINTLIBS)" "$(INCDIR)" "$(MAKESHAREDLIB)" \
	   "$(MAKEEXE)"

$(MAKEINFO): config/Makefile.$(ARCH)
	@$(MAKEMAKEINFO) $(MAKEINFO) $(CXX) $(CC) "$(CPPPREP)"

build/dummy.d: config $(RMKDEP) $(BINDEXP) $(ALLHDRS) $(ALLOBJS)
	@(if [ ! -f $@ ] ; then \
	   touch $@; \
	fi)

printdep:
	@echo "-------------  INCLUDEFILES--------"
	@echo $(INCLUDEFILES)
	@echo "-----------------------------------"

%.d: %.c $(RMKDEP)
	$(MAKEDEP) $@ "$(CFLAGS)" $*.c > $@

%.d: %.cxx $(RMKDEP)
	$(MAKEDEP) $@ "$(CXXFLAGS)" $*.cxx > $@

dist:
	@$(MAKEDIST)

clean::
	@rm -f __compiledata __makeinfo *~ core
	@rm */src/G__*
	@rm */src/*.d

ifeq ($(CXX),KCC)
clean::
	@find . -name "ti_files" -exec rm -rf {} \; >/dev/null 2>&1
endif

distclean:: clean
	@mv -f include/config.h include/config.hh
	@rm -f include/*.h $(MAKEINFO)
	@mv -f include/config.hh include/config.h
	@rm -f build/dummy.d bin/*.dll lib/*.def lib/*.exp lib/*.lib .def
	@rm -rf htmldoc

version: $(CINTTMP)
	@$(MAKEVERSION)


html:
	@$(MAKEHTML)
	@$(HTMLCLEANUP)

clean-html:
	@rm -f html/*.html html/*.h

changelog:
	@$(MAKECHANGELOG)

install:
	@(if [ -z "$(MARABOU_INSTALL_PATH)" ]; then echo "MARABOU_INSTALL_PATH has to be set first!"; else \
		if [ -d $(BINDIR) ]; then \
			inode1=`ls -id $(BINDIR) | awk '{ print $$1 }'`; \
		fi; \
		inode2=`ls -id $$PWD/bin | awk '{ print $$1 }'`; \
		if [ -d $(BINDIR) ] && [ $$inode1 -eq $$inode2 ]; then \
			echo "Everything already installed..."; \
		else \
			echo "Installing binaries in  $(BINDIR)"; \
			$(INSTALLDIR) $(BINDIR); \
			$(INSTALLFILES) $(ALLEXECS) $(BINDIR); \
			echo "Installing libraries in $(LIBDIR)"; \
			$(INSTALLDIR) $(LIBDIR); \
			$(INSTALLFILES) $(ALLLIBS) $(LIBDIR); \
			echo "Installing objs in      $(OBJDIR)"; \
			$(INSTALLDIR) $(OBJDIR); \
			$(INSTALL) $(ALLOBJS) $(OBJDIR); \
			echo "Installing headers in   $(INCDIR)"; \
			$(INSTALLDIR) $(INCDIR); \
			$(INSTALLDATA) include/*.h $(INCDIR); \
			echo "Installing templates in   $(TEMPLDIR)"; \
			$(INSTALLDIR) $(TEMPLDIR); \
			$(INSTALLDATA) templates/* $(TEMPLDIR); \
			echo "Installing data in   $(DATADIR)"; \
			$(INSTALLDIR) $(DATADIR); \
			$(INSTALLDATA) data/* $(DATADIR); \
			echo "Installing macros in   $(MACRODIR)"; \
			$(INSTALLDIR) $(MACRODIR); \
			$(INSTALLDATA) macros/* $(MACRODIR); \
			echo "Installing icons in   $(ICONPATH)"; \
			$(INSTALLDIR) $(ICONPATH); \
			$(INSTALLDATA) icons/* $(ICONPATH); \
			echo "Installing sounds in   $(SOUNDPATH)"; \
			$(INSTALLDIR) $(SOUNDPATH); \
			$(INSTALLDATA) sounds/* $(SOUNDPATH); \
			echo "Installing HistPresents doc in  $(DOCHPRDIR)"; \
			$(INSTALLDIR) $(DOCHPRDIR); \
			$(INSTALLDATA) doc/hpr/* $(DOCHPRDIR); \
		fi \
	fi)

install-others:
	@(if [ -z "$(MARABOU_INSTALL_PATH)" ]; then echo "MARABOU_INSTALL_PATH has to be set first!"; else \
		echo "Installing templates in   $(TEMPLDIR)"; \
		$(INSTALLDIR) $(TEMPLDIR); \
		$(INSTALLDATA) templates/* $(TEMPLDIR); \
		echo "Installing data in   $(DATADIR)"; \
		$(INSTALLDIR) $(DATADIR); \
		$(INSTALLDATA) data/* $(DATADIR); \
		echo "Installing macros in   $(MACRODIR)"; \
		$(INSTALLDIR) $(MACRODIR); \
		$(INSTALLDATA) macros/* $(MACRODIR); \
		echo "Installing icons in   $(ICONPATH)"; \
		$(INSTALLDIR) $(ICONPATH); \
		$(INSTALLDATA) icons/* $(ICONPATH); \
		echo "Installing sounds in   $(SOUNDPATH)"; \
		$(INSTALLDIR) $(SOUNDPATH); \
		$(INSTALLDATA) sounds/* $(SOUNDPATH); \
		echo "Installing HistPresents doc in  $(DOCHPRDIR)"; \
		$(INSTALLDIR) $(DOCHPRDIR); \
		$(INSTALLDATA) doc/hpr/* $(DOCHPRDIR); \
	fi)

install-html:
	@(if [ -z "$(MARABOU_ADMIN)" ]; then echo "MARABOU_ADMIN has to be set first!"; else \
		echo "Installing html files in $(HTMLURL)"; \
		$(INSTALLDIR)	htmldoc \
				htmldoc/marabou \
				htmldoc/gutils \
				htmldoc/hpr \
				htmldoc/c_analyze \
				htmldoc/dgfcontrol \
				htmldoc/snake \
				htmldoc/examples; \
		$(INSTALLDATA) gutils/doc/*.gif htmldoc/gutils; \
		$(INSTALLDATA) doc/hpr/*.html htmldoc/hpr; \
		$(INSTALLDATA) doc/hpr/*.gif htmldoc/hpr; \
		$(INSTALLDATA) c_analyze/doc/*.html htmldoc/c_analyze; \
		$(INSTALLDATA) c_analyze/doc/*.gif htmldoc/c_analyze; \
		$(INSTALLDATA) snake/doc/*.gif htmldoc/snake; \
		$(INSTALLDATA) dgfcontrol/doc/*.gif htmldoc/dgfcontrol; \
		echo "[Enter password for $(MARABOU_ADMIN)]"; \
		$(UPLOADHTML); \
	fi)

install-dist:
	@(if [ -z "$(MARABOU_ADMIN)" ]; then echo "MARABOU_ADMIN has to be set first!"; else \
		echo "Installing distribution files in $(DISTURL)"; \
		echo "[Enter password for $(MARABOU_ADMIN)]"; \
		$(UPLOADDIST) marabou; \
	fi)

install-snake:
	@(if [ -z "$(MARABOU_INSTALL_PATH)" ]; then echo "MARABOU_INSTALL_PATH has to be set first!"; else \
		echo "Installing SNAKE binaries in  $(BINDIR)"; \
		$(INSTALLDIR) $(BINDIR); \
		$(INSTALL) bin/DDAControl bin/DDAExec $(BINDIR); \
		echo "Installing SNAKE libraries in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) lib/libTSnkDDA0816.so $(LIBDIR); \
		echo "Installing SNAKE headers in   $(INCDIR)"; \
		$(INSTALLDIR) $(INCDIR); \
		$(INSTALLDATA) include/TSnk*.h include/DDA*.h $(INCDIR); \
	fi)

install-expconf:
	@(if [ -z "$(MARABOU_INSTALL_PATH)" ]; then echo "MARABOU_INSTALL_PATH has to be set first!"; else \
		echo "Installing CONFIG libraries in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) lib/libTMrbConfig.so $(LIBDIR); \
	fi)

showbuild:
	@echo "MODULES            = $(MODULES)"

	@echo "MARABOU            = $(MARABOU)"
	@echo "ROOTSYS            = $(ROOTSYS)"
	@echo "ROOTGLIBS          = $(ROOTGLIBS)"
	@echo "PLATFORM           = $(PLATFORM)"
	@echo "OPT                = $(OPT)"
	@echo ""
	@echo "CXX                = $(CXX)"
	@echo "CC                 = $(CC)"
	@echo "F77                = $(F77)"
	@echo "CPP                = $(CPP)"
	@echo "LD                 = $(LD)"
	@echo "F77LD              = $(F77LD)"
	@echo ""
	@echo "CXXFLAGS           = $(CXXFLAGS)"
	@echo "CINTCXXFLAGS       = $(CINTCXXFLAGS)"
	@echo "EXTRA_CXXFLAGS     = $(EXTRA_CXXFLAGS)"
	@echo "CFLAGS             = $(CFLAGS)"
	@echo "CINTCFLAGS         = $(CINTCFLAGS)"
	@echo "EXTRA_CFLAGS       = $(EXTRA_CFLAGS)"
	@echo "F77FLAGS           = $(F77FLAGS)"
	@echo "LDFLAGS            = $(LDFLAGS)"
	@echo "EXTRA_LDFLAGS      = $(EXTRA_LDFLAGS)"
	@echo "SOFLAGS            = $(SOFLAGS)"
	@echo "SOEXT              = $(SOEXT)"
	@echo ""
	@echo "SYSLIBS            = $(SYSLIBS)"
	@echo "XLIBS              = $(XLIBS)"
	@echo "CILIBS             = $(CILIBS)"
	@echo "F77LIBS            = $(F77LIBS)"
	@echo ""
	@echo "PYTHIA             = $(PYTHIA)"
	@echo "PYTHIA6            = $(PYTHIA6)"
	@echo "VENUS              = $(VENUS)"
	@echo "STAR               = $(STAR)"
	@echo "XPMLIBDIR          = $(XPMLIBDIR)"
	@echo "TTFLIBDIR          = $(TTFLIBDIR)"
	@echo "TTFINCDIR          = $(TTFINCDIR)"
	@echo "TTFFONTDIR         = $(TTFFONTDIR)"
	@echo "OPENGLULIB         = $(OPENGLULIB)"
	@echo "OPENGLLIB          = $(OPENGLLIB)"
	@echo "OPENGLINCDIR       = $(OPENGLINCDIR)"
	@echo "CERNLIBDIR         = $(CERNLIBDIR)"
	@echo "THREAD             = $(THREAD)"
	@echo "RFIO               = $(RFIO)"
	@echo "MYSQLINCDIR        = $(MYSQLINCDIR)"
	@echo "SRPDIR             = $(SRPDIR)"
	@echo "AFSDIR             = $(AFSDIR)"
	@echo ""
	@echo "INSTALL            = $(INSTALL)"
	@echo "INSTALLDIR         = $(INSTALLDIR)"
	@echo "MAKEDEP            = $(MAKEDEP)"
	@echo "MAKELIB            = $(MAKELIB)"
	@echo "MAKEDIST           = $(MAKEDIST)"
	@echo "MAKEDISTSRC        = $(MAKEDISTSRC)"
	@echo "MAKEVERSION        = $(MAKEVERSION)"
	@echo "MAKEHTML           = $(MAKEHTML)"
	@echo "IMPORTCINT         = $(IMPORTCINT)"

	@echo "Install directories -------------------"
	@echo "BINDIR             = $(BINDIR)"
	@echo "LIBDIR             = $(LIBDIR)"
	@echo "OBJDIR             = $(OBJDIR)"
	@echo "INCDIR             = $(INCDIR)"



