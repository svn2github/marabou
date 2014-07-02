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
UTILSH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
UTILSS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
UTILSO        := $(UTILSS:.cxx=.o)

UTILSDEP      := $(UTILSO:.o=.d) $(UTILSDO:.o=.d)

UTILSLIB      := $(LPATH)/libTMrbUtils.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(UTILSH))
ALLLIBS     += $(UTILSLIB)

# include all dependency files
INCLUDEFILES += $(UTILSDEP)

##### local rules #####

include/%.h:    $(UTILSDIRI)/%.h
		cp $< $@

$(UTILSLIB):     $(UTILSDO) $(UTILSO) $(MAINLIBS) $(UTILSLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbUtils.$(SOEXT) $@ "$(UTILSO) $(UTILSDO)" \
		   "$(UTILSLIBEXTRA)"
		@(if [ -f $(UTILSDIRS)/$(UTILSPCM) ] ; then \
			echo "cp  $(UTILSDIRS)/$(UTILSPCM)----------------------" ; \
			cp $(UTILSDIRS)/$(UTILSPCM) $(LPATH); \
		fi)

$(UTILSDS):     $(UTILSH) $(UTILSL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(UTILSH) $(UTILSL)

$(UTILSDO):     $(UTILSDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<

all-utils:       $(UTILSLIB)

clean-utils:
		@rm -f $(UTILSO) $(UTILSDO) $(UTILSDS)

clean::         clean-utils

distclean-utils: clean-utils
		@rm -f $(UTILSDEP) $(UTILSDS) $(UTILSDH) $(UTILSLIB)

distclean::     distclean-utils
