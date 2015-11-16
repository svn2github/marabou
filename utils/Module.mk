# Module.mk for utils module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := utils
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

UTILSDIR      := $(MODDIR)
UTILSDIRS     := $(UTILSDIR)/src
UTILSDIRI     := $(UTILSDIR)/inc

##### lib #####
UTILSL        := $(MODDIRI)/LinkDef.h
UTILSDS       := $(MODDIRS)/G__TMrbUtilsDict.cxx
UTILSPCM       := $(MODDIRS)/G__TMrbUtilsDict_rdict.pcm
UTILSDO       := $(UTILSDS:.cxx=.o)
UTILSHL        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
UTILSH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(UTILSHL))
$(info UTILSH: $(UTILSH))
UTILSS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
UTILSO        := $(UTILSS:.cxx=.o)

UTILSDEP      := $(UTILSO:.o=.d) $(UTILSDO:.o=.d)

UTILSLIB      := $(LPATH)/libTMrbUtils.$(SOEXT)
UTILSRMAP     := $(LPATH)/libTMrbUtils.rootmap
UTILSLIBDEP   := $(ROOTSYS)/lib/libGraf.so

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(UTILSHL))
ALLLIBS     += $(UTILSLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(UTILSPCM)
endif

# include all dependency files
INCLUDEFILES += $(UTILSDEP)

##### local rules #####

include/%.h:    $(UTILSDIRI)/%.h
		cp $< $@

$(UTILSLIB):     $(UTILSDO) $(UTILSO) $(MAINLIBS) $(UTILSLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbUtils.$(SOEXT) $@ "$(UTILSO) $(UTILSDO)" \
		   "$(UTILSLIBEXTRA)"
		@(if [ -f $(UTILSPCM) ] ; then \
			echo "cp  $(UTILSPCM)----------------------" ; \
			cp $(UTILSPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
		@$(RLIBMAP) -o $(UTILSRMAP) -l $(UTILSLIB) -d $(UTILSLIBDEP) -c $(UTILSL)
endif

$(UTILSDS):     $(UTILSHL) $(UTILSL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -I$(MARABOU_SRCDIR)/include $(UTILSH) $(UTILSL)

$(UTILSDO):     $(UTILSDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<

all-utils:       $(UTILSLIB)

clean-utils:
		@rm -f $(UTILSO) $(UTILSDO) $(UTILSDS)

clean::         clean-utils

distclean-utils: clean-utils
		@rm -f $(UTILSDEP) $(UTILSDS) $(UTILSDH) $(UTILSLIB)

distclean::     distclean-utils
