# Module.mk for c_analyze module
#
# Author: O.Schaile 24/11/2000


MODDIR       := c_analyze
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

CANDIR      := $(MODDIR)
CANDIRS     := $(CANDIR)/src
CANDIRI     := $(CANDIR)/inc

CAN         := C_analyze
MBSC        := TMbsControl

##### lib #####
MBSCDL       := $(MODDIRI)/LinkDef.h
MBSCDS      := $(MODDIRS)/G__$(MBSC)Dict.cxx
MBSCDO      := $(MBSCDS:.cxx=.o)

MBSCS       := $(MODDIRS)/$(MBSC).cxx
MBSCH       := $(MODDIRI)/$(MBSC).h
MBSCO       := $(MBSCS:.cxx=.o)

CANH        := $(MODDIRI)/$(CAN).h
CANS        := $(MODDIRS)/$(CAN).cxx
CANO        := $(CANS:.cxx=.o)


CANDEP      := $(CANO:.o=.d) $(CANDO:.o=.d)
MBSCDEP     := $(MBSCO:.o=.d) $(MBSCDO:.o=.d)

MBSCLIB     := $(LPATH)/lib$(MBSC).$(SOEXT)
CANEXE      := bin/$(CAN)

ALLEXECS    += $(CANEXE)
ALLLIBS     += $(MBSCLIB)

# used in the main Makefile
ALLHDRS     += include/$(MBSC).h include/$(CAN).h include/$(CAN)_Help.h

# include all dependency files
INCLUDEFILES += $(CANDEP)
INCLUDEFILES += $(MBSCDEP)

##### extra libs needed #####
GLIBS         = $(ROOTGLIBS)
CANLIBS       = $(LPATH)/libTMrbUtils.$(SOEXT) \
				$(LPATH)/libTGMrbUtils.$(SOEXT) \
				$(LPATH)/libTMrbHelpBrowser.$(SOEXT) \
				$(LPATH)/libTMbsSetup.$(SOEXT)

##### local rules #####

include/%.h:    $(CANDIRI)/%.h
		cp $< $@

$(CANEXE):     $(CANDO) $(CANO) $(CANLIBS) $(MBSCLIB)
		@echo "$(CANEXE) start linking"
		$(LD) -g $(LDFLAGS) $(CANO) $(CANLIBS) $(MBSCLIB) $(GLIBS) \
            -o $(CANEXE)

$(MBSCLIB):     $(MBSCDO) $(MBSCO) $(MAINLIBS) $(MBSCLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" lib$(MBSC).$(SOEXT) $@ "$(MBSCO) $(MBSCDO)"

$(MBSCDS):     $(MBSCH) $(MBSCDL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c $(MBSCH) $(MBSCDL)

$(MBSCDO):     $(MBSCDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

$(CANDO):     $(CANDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-c_analyze:       $(CANEXE) $(MBSCLIB)

clean-c_analyze:
		@rm -f $(CANO) $(MBSCO) $(MBSCDO) $(MBSCDS) $(CANEXE) $(MBSCLIB)

clean::         clean-c_analyze

distclean-c_analyze: clean-c_analyze
		@rm -f $(CANDEP) $(CANDS) $(CANDH) $(CANEXE)

distclean::     distclean-c_analyze

install-c_analyze:	
	@(if [ -d $(BINDIR) ]; then \
	   inode1=`ls -id $(BINDIR) | awk '{ print $$1 }'`; \
	fi; \
	inode2=`ls -id $$PWD/bin | awk '{ print $$1 }'`; \
	if [ -d $(BINDIR) ] && [ $$inode1 -eq $$inode2 ]; then \
		echo "Everything already installed..."; \
	else \
		echo "Installing $(CANEXE) in $(BINDIR)"; \
		$(INSTALLDIR) $(BINDIR); \
		$(INSTALL) $(CANEXE) $(BINDIR); \
		echo "Installing $(MBSCLIB) in $(LIBDIR)"; \
		$(INSTALLDIR) $(LIBDIR); \
		$(INSTALL) $(MBSCLIB) $(LIBDIR); \
	fi)
