# Module.mk for hpr module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := hpr
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

HPRDIR      := $(MODDIR)
HPRDIRS     := $(HPRDIR)/src
HPRDIRI     := $(HPRDIR)/inc

##### lib #####
HPRL        := $(MODDIRI)/LinkDef.h
HPRDS       := $(MODDIRS)/G__HistPresentDict.cxx
HPRDO       := $(HPRDS:.cxx=.o)
HPRH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
HPRS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
HPROM       := $(HPRS:.cxx=.o)
HPRMAINO    := $(MODDIRS)/main.o
HPRO        := $(filter-out $(HPRMAINO),$(HPROM))


HPRDH			:= $(HPRDIRI)/HTCanvas.h \
 					$(HPRDIRI)/HandleMenus.h \
 					$(HPRDIRI)/FitHist.h \
 					$(HPRDIRI)/HistPresent.h \
 					$(HPRDIRI)/TMrbArrayD.h \
 					$(HPRDIRI)/TMrbArrayF.h \
  					$(HPRDIRI)/FhPeak.h \
					$(HPRDIRI)/FhContour.h \
               $(HPRDIRI)/FhRegion.h \
               $(HPRDIRI)/GroupOfGObjects.h \
               $(HPRDIRI)/HprImage.h \
 					$(HPRDIRI)/EditMarker.h 

HPRDEP      := $(HPRO:.o=.d) $(HPRDO:.o=.d)
HPRDEP      += $(MODDIRS)/main.d

HPREXE      := bin/HistPresent
HPRLIB       := $(LPATH)/libHpr.so

ALLEXECS    += $(HPREXE)

ALLLIBS     += $(HPRLIB)
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(HPRH))

# include all dependency files
INCLUDEFILES += $(HPRDEP)

##### extra libs needed #####
GLIBS         = $(ROOTGLIBS)
OHPRLIBS      := -L$(LPATH) -lTMrbUtils -lTGMrbUtils -lTMrbHelpBrowser

##### local rules #####

include/%.h:    $(HPRDIRI)/%.h
		cp $< $@

$(HPREXE):     $(HPRSO)$(HPRO) $(HPRMAINO) $(MRBLIBS)
#		@echo "sources: $(HPRS)"
		@echo "OHPRLIBS: $(OHPRLIBS)"
#		$(LD) -g $(LDFLAGS) $(HPRMAINO) $(HPRLIB) $(MRBLIBS) $(GLIBS) \
		@echo "$(HPREXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(HPRMAINO) $(HPRO) $(HPRDO) $(OHPRLIBS) $(GLIBS) \
            -o $(HPREXE)

$(HPRLIB):     $(HPRDO) $(HPRO)
		@echo "objs in libHpr.so: $(HPRO)"
#		@echo "objs: $(HPRO)"
		@echo "$(HPREXE) make shared lib ------------------------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libHpr.$(SOEXT) $@ "$(HPRO) $(HPRDO)"

$(HPRDS):     $(HPRDH) $(HPRL)
		@echo "includes: $(HPRDH)"
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(HPRDH)

$(HPRDO):     $(HPRDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-hpr:       $(HPREXE)

clean-hpr:
		@rm -f $(HPRO) $(HPRMAINO) $(HPRDO) $(HPRDS)

clean::         clean-hpr

distclean-hpr: clean-hpr
		@rm -f $(HPRDEP) $(HPRDS) $(HPREXE) $(HPRLIB)

distclean::     distclean-hpr
