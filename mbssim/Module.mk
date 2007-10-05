# Module.mk for mbssim module
#
# Author: O.Schaile 24/11/2000

MODDIR       := mbssim
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

MBSSIMDIR      := $(MODDIR)
MBSSIMDIRS     := $(MBSSIMDIR)/src
MBSSIMDIRI     := $(MBSSIMDIR)/inc

##### lib #####
MBSSIML        := $(MODDIRI)/LinkDef.h
MBSSIMDS       := $(MODDIRS)/G__MbsSimRdoDict.cxx
MBSSIMDO       := $(MBSSIMDS:.cxx=.o)
MBSSIMH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
MBSSIMS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
MBSSIMO        := $(MBSSIMS:.cxx=.o)

MBSSIMDEP      := $(MBSSIMO:.o=.d) $(MBSSIMDO:.o=.d)

MBSSIMLIB      := $(LPATH)/libMbsSimRdo.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(MBSSIMH))
ALLLIBS     += $(MBSSIMLIB)

# include all dependency files
INCLUDEFILES += $(MBSSIMDEP)

# add simulation flag

CXXFLAGS += -DMBS_SIMUL_RDO

##### local rules #####

include/%.h:    $(MBSSIMDIRI)/%.h
		cp $< $@

$(MBSSIMLIB):     $(MBSSIMDO) $(MBSSIMO) $(MAINLIBS) $(MBSSIMLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libMbsSimRdo.$(SOEXT) $@ "$(MBSSIMO) $(MBSSIMDO)" \
		   "$(MBSSIMLIBEXTRA)"

$(MBSSIMDS):     $(MBSSIMH) $(MBSSIML)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -DMBS_SIMUL_RDO -Iinclude $(MBSSIMH) $(MBSSIML)

$(MBSSIMDO):     $(MBSSIMDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-mbssim:       $(MBSSIMLIB)

clean-mbssim:
		@rm -f $(MBSSIMO) $(MBSSIMDO) $(MBSSIMDS)

clean::         clean-mbssim

distclean-mbssim: clean-mbssim
		@rm -f $(MBSSIMDEP) $(MBSSIMDS) $(MBSSIMDH) $(MBSSIMLIB)

distclean::     distclean-mbssim
