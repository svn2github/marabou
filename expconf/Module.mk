# Module.mk for expconf module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR         := expconf
MODDIRS         := $(MODDIR)/src
MODDIRI         := $(MODDIR)/inc

EXPCONFDIR       := $(MODDIR)
EXPCONFDIRS     := $(EXPCONFDIR)/src
EXPCONFDIRI     := $(EXPCONFDIR)/inc

##### lib #####
EXPCONFL        := $(MODDIRI)/LinkDef.h
EXPCONFDS       := $(MODDIRS)/G__TMrbConfigDict.cxx
EXPCONFDO       := $(EXPCONFDS:.cxx=.o)
EXPCONFH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
EXPCONFS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
EXPCONFO        := $(EXPCONFS:.cxx=.o)

EXPCONFDEP      := $(EXPCONFO:.o=.d) $(EXPCONFDO:.o=.d)

EXPCONFLIB      := $(LPATH)/libTMrbConfig.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(EXPCONFH))
ALLLIBS     += $(EXPCONFLIB)

# include all dependency files
INCLUDEFILES += $(EXPCONFDEP)

##### local rules #####

include/%.h:    $(EXPCONFDIRI)/%.h
		cp $< $@

$(EXPCONFLIB):     $(EXPCONFDO) $(EXPCONFO) $(MAINLIBS) $(EXPCONFLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbConfig.$(SOEXT) $@ "$(EXPCONFO) $(EXPCONFDO)" \
		   "$(EXPCONFLIBEXTRA)"

$(EXPCONFDS):     $(EXPCONFH) $(EXPCONFL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(EXPCONFH) $(EXPCONFL)

$(EXPCONFDO):     $(EXPCONFDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-expconf:       $(EXPCONFLIB)

clean-expconf:
		@rm -f $(EXPCONFO) $(EXPCONFDO) $(EXPCONFDS)

clean::         clean-expconf

distclean-expconf: clean-expconf
		@rm -f $(EXPCONFDEP) $(EXPCONFDS) $(EXPCONFDH) $(EXPCONFLIB)

distclean::     distclean-expconf
