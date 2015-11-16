# Module.mk for esone module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := esone
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

ESONEDIR      := $(MODDIR)
ESONEDIRS     := $(ESONEDIR)/src
ESONEDIRI     := $(ESONEDIR)/inc

##### lib #####
ESONEL        := $(MODDIRI)/LinkDef.h
ESONEDS       := $(MODDIRS)/G__TMrbEsoneDict.cxx
ESONEPCM       := $(MODDIRS)/G__TMrbEsoneDict_rdict.pcm
ESONEDO       := $(ESONEDS:.cxx=.o)
ESONEH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
ESONEDH       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/TMrb*.h))
ESONES        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
ESONEO        := $(ESONES:.cxx=.o)

ESONEDEP      := $(ESONEO:.o=.d) $(ESONEDO:.o=.d)

ESONELIB      := $(LPATH)/libTMrbEsone.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(ESONEH))
ALLLIBS     += $(ESONELIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(ESONEPCM)
endif

# include all dependency files
INCLUDEFILES += $(ESONEDEP)

##### local rules #####

include/%.h:    $(ESONEDIRI)/%.h
		cp $< $@

$(ESONELIB):     $(ESONEDO) $(ESONEO) $(MAINLIBS) $(ESONELIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbEsone.$(SOEXT) $@ "$(ESONEO) $(ESONEDO)" \
		   "$(ESONELIBEXTRA)"
		@(if [ -f $(ESONEPCM) ] ; then \
			echo "cp  $(ESONEPCM)----------------------" ; \
			cp $(ESONEPCM) $(LPATH); \
		fi)

$(ESONEDS):     $(ESONEH) $(ESONEL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(ESONEDH) $(ESONEL)

$(ESONEDO):     $(ESONEDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-esone:       $(ESONELIB)

clean-esone:
		@rm -f $(ESONEO) $(ESONEDO) $(ESONEDS)

clean::         clean-esone

distclean-esone: clean-esone
		@rm -f $(ESONEDEP) $(ESONEDS) $(ESONELIB)

distclean::     distclean-esone
