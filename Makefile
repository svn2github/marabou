# Top level Makefile for Marabou System
# Copyright (c) 2000 Rudi Lutter and Otto Schaile
#
# Author: Otto stolen from ROOT (Fons Rademakers), 15/9/2000


##### include path/location macros (result of ./configure) #####

include config/Makefile.config

EXTRA_CFLAGS   += -g -Wno-switch -I$(ROOTSYS)/include
EXTRA_CXXFLAGS += -g -Wno-switch -I$(ROOTSYS)/include 
##### include machine dependent macros #####

include config/Makefile.$(ARCH)

##### allow local macros #####

-include MyConfig.mk

##### Modules to build #####

MODULES       = build utils gutils \
                helpbrowser hpr \
                tidy tidylib \
                expconf analyze c_analyze  macrobrowser \
                camcli esone mbssetup mbsio transport \
	        xiadgf dgfcontrol \
                polar snake

##### ROOT libraries #####

LPATH         = lib

ifneq ($(ARCH),win32)
RPATH        := -L$(LPATH)
# CINTLIBS     := -lCint
# ROOTLIBS     := -lNew -lCore -lCint -lHist -lGraf -lGraf3d -lTree -lMatrix
# RINTLIBS     := -lRint
# PROOFLIBS    := -lGpad -lProof -lTreePlayer
# CERNPATH     := -L$(CERNLIBDIR)
# CERNLIBS     := -lpacklib -lkernlib
else
# CINTLIBS     := $(LPATH)/libCint.lib
# ROOTLIBS     := $(LPATH)/libNew.lib $(LPATH)/libCore.lib $(LPATH)/libCint.lib \
#                 $(LPATH)/libHist.lib $(LPATH)/libGraf.lib \
#                 $(LPATH)/libGraf3d.lib $(LPATH)/libTree.lib \
#                 $(LPATH)/libMatrix.lib
# RINTLIBS     := $(LPATH)/libRint.lib
# PROOFLIBS    := $(LPATH)/libGpad.lib $(LPATH)/libProof.lib \
#                 $(LPATH)/libTreePlayer.lib
# CERNLIBS     := '$(shell cygpath -w -- $(CERNLIBDIR)/packlib.lib)' \
#                 '$(shell cygpath -w -- $(CERNLIBDIR)/kernlib.lib)'
endif

ROOTCFLAGS    := $(shell root-config --cflags)
#ROOTLIBS      := $(shell root-config --libs)
ROOTLIBS      := $(shell root-config --new --libs)
ROOTLIBS      += -lGed
#ROOTGLIBS     := $(shell root-config --glibs)
ROOTGLIBS     := $(shell root-config --new --glibs)
ROOTGLIBS      += -lGed

ROOTCINT      :=rootcint
##### utilities #####

MAKEDEP       = build/unix/depend.sh
MAKELIB       = build/unix/makelib.sh
MAKEDIST      = build/unix/makedist.sh
MAKEVERSION   = build/unix/makeversion.sh
IMPORTCINT    = build/unix/importcint.sh
MAKECOMPDATA  = build/unix/compiledata.sh
MAKEMAKEINFO  = build/unix/makeinfo.sh
MAKECHANGELOG = build/unix/makechangelog.sh
MAKEHTML      = build/unix/makehtml.sh
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
HPRLIBS      := lib/libHpr.so lib/libTMrbHelpBrowser.so lib/libTMrbUtils.so lib/libTGMrbUtils.so
ALLEXECS     :=
INCLUDEFILES :=
ALLOBJ       :=

##### RULES #####

.SUFFIXES: .cxx .d
.PRECIOUS: include/%.h obj/%.o

%.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) -o $@ -c $<

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

hprexecs:          compiledata $(HPRLIBS) bin/HistPresent

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
#	@rm -f tutorials/*.root tutorials/*.ps tutorials/*.gif so_locations
#	@rm -f tutorials/pca.C tutorials/*.so
	@rm -rf htmldoc
#	@cd test && $(MAKE) distclean

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
	@(if [ -d $(BINDIR) ]; then \
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
	fi)

install-others:
		@echo "Installing templates in   $(TEMPLDIR)"; \
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

install-html:
		@echo "Installing html files in $(HTMLURL)"; \
		$(INSTALLDIR)	html/marabou \
						html/gutils \
						html/hpr \
						html/c_analyze \
						html/dgfcontrol \
						html/snake \
						html/examples; \
		$(INSTALLDATA) gutils/doc/*.gif html/gutils; \
		$(INSTALLDATA) hpr/doc/*.html html/hpr; \
		$(INSTALLDATA) hpr/doc/*.gif html/hpr; \
		$(INSTALLDATA) c_analyze/doc/*.html html/c_analyze; \
		$(INSTALLDATA) c_analyze/doc/*.gif html/c_analyze; \
		$(INSTALLDATA) snake/doc/*.gif html/snake; \
		$(INSTALLDATA) dgfcontrol/doc/*.gif html/dgfcontrol; \
		echo "[Enter (normal) root password]"; \
		$(UPLOADHTML)

install-dist:
		@echo "Installing distribution files in $(DISTURL)"; \
		echo "[Enter (normal) root password]"; \
		$(UPLOADDIST) marabou

install-snake:
		@echo "Installing SNAKE binaries in  $(BINDIR)"; \
		$(INSTALLDIR) $(BINDIR); \
		$(INSTALL) bin/DDAControl bin/DDAExec $(BINDIR); \
		echo "Installing SNAKE libraries in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) lib/libTSnkDDA0816.so $(LIBDIR); \
		echo "Installing SNAKE headers in   $(INCDIR)"; \
		$(INSTALLDIR) $(INCDIR); \
		$(INSTALLDATA) include/TSnk*.h include/DDA*.h $(INCDIR)

install-expconf:
		echo "Installing CONFIG libraries in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) lib/libTMrbConfig.so $(LIBDIR)

showbuild:
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



