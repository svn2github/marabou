# Module.mk for dgfcontrol module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := dgfcontrol
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

DGFCDIR      := $(MODDIR)
DGFCDIRS     := $(DGFCDIR)/src
DGFCDIRI     := $(DGFCDIR)/inc

##### lib #####
DGFCL        := $(MODDIRI)/LinkDef.h
DGFCDS       := $(MODDIRS)/G__DGFControlDict.cxx
DGFCDO       := $(DGFCDS:.cxx=.o)
DGFCH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
DGFCS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
DGFCOM       := $(DGFCS:.cxx=.o)
DGFCMAINO    := $(MODDIRS)/DGFControl.o
DGFCO        := $(filter-out $(DGFCMAINO),$(DGFCOM))


DGFCDH			:= $(DGFCH)
DGFCDEP      := $(DGFCO:.o=.d) $(DGFCDO:.o=.d)

DGFCEXE      := bin/DGFControl
DGFCLIB       := $(LPATH)/libDGFControl.so

ALLEXECS    += $(DGFCEXE)

ALLLIBS     += $(DGFCLIB)
# used in the DGFControl Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(DGFCH))

# include all dependency files
INCLUDEFILES += $(DGFCDEP)

##### extra libs needed #####
GLIBS         = $(ROOTGLIBS)
#GLIBS     = $(shell root-config --new --glibs)
GLIBS         += -lProof
ODGFCLIBS      := -L$(LPATH) -lN2ll -lTMrbDGF -lTMrbEsone -lEsoneClient \
                 -lTMrbUtils -lTGMrbUtils -lHpr -lTMrbHelpBrowser

##### local rules #####

include/%.h:    $(DGFCDIRI)/%.h
		cp $< $@

$(DGFCEXE):     $(DGFCSO)$(DGFCO) $(DGFCMAINO) $(MRBLIBS)
#		@echo "sources: $(DGFCS)"
		@echo "ODGFCLIBS: $(ODGFCLIBS)"
#		$(LD) -g $(LDFLAGS) $(DGFCMAINO) $(DGFCLIB) $(MRBLIBS) $(GLIBS) \
		@echo "$(DGFCEXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(DGFCMAINO) $(DGFCO) $(DGFCDO) $(ODGFCLIBS) $(GLIBS) \
            -o $(DGFCEXE)

$(DGFCLIB):     $(DGFCDO) $(DGFCO)
		@echo "objs in libDGFControl.so: $(DGFCO)"
#		@echo "objs: $(DGFCO)"
		@echo "$(DGFCEXE) make shared lib ------------------------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libDGFControl.$(SOEXT) $@ "$(DGFCO) $(DGFCDO)"

$(DGFCDS):     $(DGFCDH) $(DGFCL)
		@echo "includes: $(DGFCDH)"
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(DGFCDH)

$(DGFCDO):     $(DGFCDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-dgfcontrol:       $(DGFCEXE)

clean-dgfcontrol:
		@rm -f $(DGFCO) $(DGFCMAINO) $(DGFCDO) $(DGFCDS)

clean::         clean-dgfcontrol

distclean-dgfcontrol: clean-dgfcontrol
		@rm -f $(DGFCDEP) $(DGFCDS) $(DGFCEXE) $(DGFCLIB)

distclean::     distclean-dgfcontrol
