# Module.mk for polar module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := polar
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

POLARDIR      := $(MODDIR)
POLARDIRS     := $(POLARDIR)/src
POLARDIRI     := $(POLARDIR)/inc

##### lib #####
POLARL        := $(MODDIRI)/LinkDef.h
POLARDS       := $(MODDIRS)/G__TPolControlDict.cxx
POLARDO       := $(POLARDS:.cxx=.o)
POLARH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
POLARS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
POLARO        := $(POLARS:.cxx=.o)

POLARDEP      := $(POLARO:.o=.d) $(POLARDO:.o=.d)

POLARLIB      := $(LPATH)/libTPolControl.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(POLARH))
ALLLIBS     += $(POLARLIB)

# include all dependency files
INCLUDEFILES += $(POLARDEP)

##### local rules #####

include/%.h:    $(POLARDIRI)/%.h
		cp $< $@

$(POLARLIB):     $(POLARDO) $(POLARO) $(MAINLIBS) $(POLARLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTPolControl.$(SOEXT) $@ "$(POLARO) $(POLARDO)" \
		   "$(POLARLIBEXTRA)"

$(POLARDS):     $(POLARH) $(POLARL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c  -Iinclude $(POLARH) $(POLARL)

$(POLARDO):     $(POLARDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-polar:       $(POLARLIB)

clean-polar:
		@rm -f $(POLARO) $(POLARDO) $(POLARDS)

clean::         clean-polar

distclean-polar: clean-polar
		@rm -f $(POLARDEP) $(POLARDS) $(POLARDH) $(POLARLIB)

distclean::     distclean-polar
