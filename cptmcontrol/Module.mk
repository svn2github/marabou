# Module.mk for CptmControl module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := cptmcontrol
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

CptmCDIR      := $(MODDIR)
CptmCDIRS     := $(CptmCDIR)/src
CptmCDIRI     := $(CptmCDIR)/inc

##### lib #####
CptmCL        := $(MODDIRI)/LinkDef.h
CptmCDS       := $(MODDIRS)/G__CptmControlDict.cxx
CptmCDO       := $(CptmCDS:.cxx=.o)
CptmCH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
CptmCS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
CptmCOM       := $(CptmCS:.cxx=.o)
CptmCMAINO    := $(MODDIRS)/CptmControl.o
CptmCO        := $(filter-out $(CptmCMAINO),$(CptmCOM))


CptmCDH			:= $(CptmCH)
CptmCDEP      := $(CptmCO:.o=.d) $(CptmCDO:.o=.d)

CptmCEXE      := bin/CptmControl
CptmCLIB       := $(LPATH)/libCptmControl.so

ALLEXECS    += $(CptmCEXE)

ALLLIBS     += $(CptmCLIB)
# used in the CptmControl Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(CptmCH))

# include all dependency files
INCLUDEFILES += $(CptmCDEP)

##### extra libs needed #####
OCptmCLIBS      := -L$(LPATH) -lTMrbDGF -lTMrbC2Lynx -lTMrbEsone -lEsoneClient \
                 -lTMrbUtils -lTGMrbUtils -lHpr -lTMrbHelpBrowser -lGrEdit -lFitCal

##### local rules #####

include/%.h:    $(CptmCDIRI)/%.h
		cp $< $@

$(CptmCEXE):     $(CptmCSO)$(CptmCO) $(CptmCMAINO) $(MRBLIBS)
#		@echo "sources: $(CptmCS)"
		@echo "OCptmCLIBS: $(OCptmCLIBS)"
		@echo "$(CptmCEXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(CptmCMAINO) $(CptmCO) $(CptmCDO) $(OCptmCLIBS) $(ROOTGLIBS) -lSpectrum \
            -o $(CptmCEXE)

$(CptmCLIB):     $(CptmCDO) $(CptmCO)
		@echo "objs in libCptmControl.so: $(CptmCO)"
#		@echo "objs: $(CptmCO)"
		@echo "$(CptmCEXE) make shared lib ------------------------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libCptmControl.$(SOEXT) $@ "$(CptmCO) $(CptmCDO)"

$(CptmCDS):     $(CptmCDH) $(CptmCL)
		@echo "includes: $(CptmCDH)"
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(CptmCDH) $(CptmCL)

$(CptmCDO):     $(CptmCDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-cptmcontrol:       $(CptmCEXE)

clean-cptmcontrol:
		@rm -f $(CptmCO) $(CptmCMAINO) $(CptmCDO) $(CptmCDS)

clean::         clean-cptmcontrol

distclean-cptmcontrol: clean-cptmcontrol
		@rm -f $(CptmCDEP) $(CptmCDS) $(CptmCEXE) $(CptmCLIB)

distclean::     distclean-cptmcontrol
