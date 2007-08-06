# Module.mk for fitcal module
#
# Author: O.Schaile, R.Lutter 20/11/2000


MODDIR       := fitcal
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

FITCALDIR      := $(MODDIR)
FITCALDIRS     := $(FITCALDIR)/src
FITCALDIRI     := $(FITCALDIR)/inc

##### lib #####
FITCALL        := $(MODDIRI)/LinkDef.h
FITCALDS       := $(MODDIRS)/G__FitCalDict.cxx
FITCALDO       := $(FITCALDS:.cxx=.o)
FITCALH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
FITCALS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
FITCALO        := $(FITCALS:.cxx=.o)

FITCALDEP      := $(FITCALO:.o=.d) $(FITCALDO:.o=.d)

FITCALLIB      := $(LPATH)/libFitCal.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(FITCALH))
ALLLIBS     += $(FITCALLIB)

# include all dependency files
INCLUDEFILES += $(FITCALDEP)

##### local rules #####

include/%.h:    $(FITCALDIRI)/%.h
		cp $< $@

$(FITCALLIB):     $(FITCALDO) $(FITCALO) $(MAINLIBS) $(FITCALLIBDEP)
			@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libFitCal.$(SOEXT) $@ "$(FITCALDO) $(FITCALO)" \
		   "$(FITCALLIBEXTRA)"

$(FITCALDS):     $(FITCALH) $(FITCALL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude defineMarabou.h $(FITCALH) $(FITCALL)

$(FITCALDO):     $(FITCALDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-fitcal:       $(FITCALLIB)

clean-fitcal:
		@rm -f $(FITCALO) $(FITCALDO) $(FITCALDS)

clean::         clean-fitcal

distclean-fitcal: clean-fitcal
		@rm -f $(FITCALDEP) $(FITCALDS) $(FITCALDH) $(FITCALLIB)

distclean::     distclean-fitcal
