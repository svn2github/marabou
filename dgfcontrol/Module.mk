# Module.mk for dgfcontrol module
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
ODGFCLIBS      := -L$(LPATH) -lTMrbDGF -lTMrbC2Lynx -lTMrbEsone -lEsoneClient \
                 -lTMrbUtils -lTGMrbUtils -lHpr -lTMrbHelpBrowser -lGrEdit -lFitCal

##### local rules #####

include/%.h:    $(DGFCDIRI)/%.h
		cp $< $@
$(DGFCEXE):     $(DGFCO) $(DGFCMAINO) $(MRBLIBS)
		$(LD) -g $(LDFLAGS) $(DGFCMAINO) $(DGFCO) $(DGFCDO) $(ODGFCLIBS) $(ROOTGLIBS) -lSpectrum \
            -o $(DGFCEXE)

$(DGFCLIB):     $(DGFCDO) $(DGFCO)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libDGFControl.$(SOEXT) $@ "$(DGFCO) $(DGFCDO)"

$(DGFCDS):     $(DGFCDH) $(DGFCL)
		$(ROOTCINT) -f $@ -c -p -Iinclude $(DGFCDH) $(DGFCL)

$(DGFCDO):     $(DGFCDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-dgfcontrol:       $(DGFCEXE)

clean-dgfcontrol:
		@rm -f $(DGFCO) $(DGFCMAINO) $(DGFCDO) $(DGFCDS)

clean::         clean-dgfcontrol

distclean-dgfcontrol: clean-dgfcontrol
		@rm -f $(DGFCDEP) $(DGFCDS) $(DGFCEXE) $(DGFCLIB)

distclean::     distclean-dgfcontrol
