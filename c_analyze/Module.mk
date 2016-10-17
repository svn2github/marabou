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
MBSCL      := $(MODDIRI)/LinkDef.h
MBSCDS      := $(MODDIRS)/G__$(MBSC)Dict.cxx
MBSCPCM     := $(MODDIRS)/G__$(MBSC)Dict_rdict.pcm
MBSCDO      := $(MBSCDS:.cxx=.o)

MBSCS       := $(MODDIRS)/$(MBSC).cxx
MBSCH       := $(MODDIRI)/$(MBSC).h
MBSCO       := $(MBSCS:.cxx=.o)

# MBSCDO       := $(MBSCDS:.cxx=.o)
# MBSCHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
# MBSCH        := $(patsubst $(MODDIRI)/%.h,%.h,$(MBSCHL))
# MBSCS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
# MBSCO        := $(MBSCS:.cxx=.o)
 
CANH        := $(MODDIRI)/$(CAN).h
CANS        := $(MODDIRS)/$(CAN).cxx
CANO        := $(CANS:.cxx=.o)


CANDEP      := $(CANO:.o=.d) $(CANDO:.o=.d)
MBSCDEP     := $(MBSCO:.o=.d) $(MBSCDO:.o=.d)

MBSCLIB     := $(LPATH)/lib$(MBSC).$(SOEXT)
CANEXE      := bin/$(CAN)

ALLEXECS    += $(CANEXE)
ALLLIBS     += $(MBSCLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(MBSCPCM)
endif
# $(info MBSCHL $(MBSCHL))
# used in the main Makefile
ALLHDRS     += include/$(MBSC).h include/$(CAN).h include/$(CAN)_Help.h

# include all dependency files
INCLUDEFILES += $(CANDEP)
INCLUDEFILES += $(MBSCDEP)

##### extra libs needed #####
CANLIBS       = $(LPATH)/libTMrbUtils.$(SOEXT) \
				$(LPATH)/libTGMrbUtils.$(SOEXT) \
				$(LPATH)/libTMrbHelpBrowser.$(SOEXT) \
				$(LPATH)/libTMbsSetup.$(SOEXT)

##### local rules #####

include/%.h:    $(CANDIRI)/%.h
		cp $< $@

$(CANEXE):     $(CANDO) $(CANO) $(CANLIBS) $(MBSCLIB)
		@echo "$(CANEXE) start linking"
		$(LD) -g $(LDFLAGS) $(CANO) $(CANLIBS) $(MBSCLIB) $(ROOTGLIBS) \
            -o $(CANEXE)

$(MBSCLIB):     $(MBSCDO) $(MBSCO)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" lib$(MBSC).$(SOEXT) $@ "$(MBSCO) $(MBSCDO) $(MBSCLIBEXTRA)"
		@(if [ -f $(MBSCPCM) ] ; then \
			echo "cp  $(MBSCPCM)----------------------" ; \
			cp $(MBSCPCM) $(LPATH); \
		fi)

$(MBSCDS):     $(MBSCHL) $(MBSCL)
		@echo "Generating dictionary $@..."
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include $(MBSCH) $(MBSCL)
else
		rootcling -f $@ $(call dictModule,MBSC) $(MBSCH) $(MBSCL)
endif

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
