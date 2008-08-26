# Module.mk for vmecontrol module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := vmecontrol
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

VMECDIR      := $(MODDIR)
VMECDIRS     := $(VMECDIR)/src
VMECDIRI     := $(VMECDIR)/inc

##### lib #####
VMECL        := $(MODDIRI)/LinkDef.h
VMECDS       := $(MODDIRS)/G__VMEControlDict.cxx
VMECDO       := $(VMECDS:.cxx=.o)
VMECH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
VMECS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
VMECOM       := $(VMECS:.cxx=.o)
VMECMAINO    := $(MODDIRS)/VMEControl.o
VMECO        := $(filter-out $(VMECMAINO),$(VMECOM))


VMECDH			:= $(VMECH)
VMECDEP      := $(VMECO:.o=.d) $(VMECDO:.o=.d)

VMECEXE      := bin/VMEControl
VMECLIB       := $(LPATH)/libVMEControl.so

ALLEXECS    += $(VMECEXE)

ALLLIBS     += $(VMECLIB)
# used in the vmeControl Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(VMECH))

# include all dependency files
INCLUDEFILES += $(VMECDEP)

##### extra libs needed #####
OVMECLIBS      := -L$(LPATH) -lTMrbC2Lynx -lTMrbUtils -lTGMrbUtils

##### local rules #####

include/%.h:    $(VMECDIRI)/%.h
		cp $< $@

$(VMECEXE):     $(VMECSO)$(VMECO) $(VMECMAINO) $(MRBLIBS)
#		@echo "sources: $(VMECS)"
		@echo "OVMECLIBS: $(OVMECLIBS)"
		@echo "$(VMECEXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(VMECMAINO) $(VMECO) $(VMECDO) $(OVMECLIBS) $(ROOTGLIBS) -lSpectrum \
            -o $(VMECEXE)

$(VMECLIB):     $(VMECDO) $(VMECO)
		@echo "objs in libVMEControl.so: $(VMECO)"
#		@echo "objs: $(VMECO)"
		@echo "$(VMECEXE) make shared lib ------------------------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libVMEControl.$(SOEXT) $@ "$(VMECO) $(VMECDO)"

$(VMECDS):     $(VMECDH) $(VMECL)
		@echo "includes: $(VMECDH)"
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -p -Iinclude $(VMECDH) $(VMECL)

$(VMECDO):     $(VMECDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-vmecontrol:       $(VMECEXE)

clean-vmecontrol:
		@rm -f $(VMECO) $(VMECMAINO) $(VMECDO) $(VMECDS)

clean::         clean-vmecontrol

distclean-vmecontrol: clean-vmecontrol
		@rm -f $(VMECDEP) $(VMECDS) $(VMECEXE) $(VMECLIB)

distclean::     distclean-vmecontrol
