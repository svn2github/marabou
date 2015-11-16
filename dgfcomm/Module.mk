# Module.mk for dgfcomm module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR         := dgfcomm
MODDIRS        := $(MODDIR)/src
MODDIRI        := $(MODDIR)/inc

DGFCOMDIR      := $(MODDIR)
DGFCOMDIRS     := $(DGFCOMDIR)/src
DGFCOMDIRI     := $(DGFCOMDIR)/inc

DGFCOML        := $(MODDIRI)/LinkDef.h
DGFCOMDS       := $(MODDIRS)/G__TMrbDGFCommonDict.cxx
DGFCOMPCM       := $(MODDIRS)/G__TMrbDGFCommonDict_rdict.pcm
DGFCOMDO       := $(DGFCOMDS:.cxx=.o)
DGFCOMH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
DGFCOMS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
DGFCOMO        := $(DGFCOMS:.cxx=.o)

DGFCOMDEP      := $(DGFCOMO:.o=.d) $(DGFCOMDO:.o=.d)

DGFCOMLIB      := $(LPATH)/libTMrbDGFCommon.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(DGFCOMH))
ALLLIBS     += $(DGFCOMLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(DGFCOMPCM)
endif

# include all dependency files
INCLUDEFILES += $(DGFCOMDEP)

##### local rules #####

include/%.h:    $(DGFCOMDIRI)/%.h
		cp $< $@

$(DGFCOMLIB):     $(DGFCOMDO) $(DGFCOMO) $(MAINLIBS) $(DGFCOMDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbDGFCommon.$(SOEXT) $@ "$(DGFCOMO) $(DGFCOMDO)" \
		   "$(DGFCOMLIBEXTRA)"
		@(if [ -f $(DGFCOMPCM) ] ; then \
			echo "cp  $(DGFCOMPCM)----------------------" ; \
			cp $(DGFCOMPCM) $(LPATH); \
		fi)

$(DGFCOMDS):     $(DGFCOMH) $(DGFCOML)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(DGFCOMH) $(DGFCOML)

$(DGFCOMDO):     $(DGFCOMDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-dgfcomm:       $(DGFCOMLIB)

clean-dgfcomm:
		@rm -f $(DGFCOMO) $(DGFCOMDO)

clean::         clean-dgfcomm

distclean-dgfcomm: clean-dgfcomm
		@rm -f $(DGFCOMDEP) $(DGFCOMDS) $(DGFCOMDH) $(DGFCOMLIB)

distclean::     distclean-dgfcomm
