# Module.mk for tidy module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := tidy
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

TIDYDIR      := $(MODDIR)
TIDYDIRS     := $(TIDYDIR)/src
TIDYDIRI     := $(TIDYDIR)/inc

##### lib #####
TIDYL        := $(MODDIRI)/LinkDef.h
TIDYDS       := $(MODDIRS)/G__TMrbTidyDict.cxx
TIDYDO       := $(TIDYDS:.cxx=.o)
TIDYH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
TIDYS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
TIDYO        := $(TIDYS:.cxx=.o)

TIDYDEP      := $(TIDYO:.o=.d) $(TIDYDO:.o=.d)

TIDYLIB      := $(LPATH)/libTMrbTidy.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(TIDYH))
ALLLIBS     += $(TIDYLIB)

# include all dependency files
INCLUDEFILES += $(TIDYDEP)

##### local rules #####

include/%.h:    $(TIDYDIRI)/%.h
		cp $< $@

$(TIDYLIB):     $(TIDYDO) $(TIDYO) $(MAINLIBS) $(TIDYLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbTidy.$(SOEXT) $@ "$(TIDYO) $(TIDYDO)" \
		   "$(TIDYLIBEXTRA)"

$(TIDYDS):     $(TIDYH) $(TIDYL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(TIDYH) $(TIDYL)

$(TIDYDO):     $(TIDYDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-tidy:       $(TIDYLIB)

clean-tidy:
		@rm -f $(TIDYO) $(TIDYDO) $(TIDYDS)

clean::         clean-tidy

distclean-tidy: clean-tidy
		@rm -f $(TIDYDEP) $(TIDYDS) $(TIDYDH) $(TIDYLIB)

distclean::     distclean-tidy
