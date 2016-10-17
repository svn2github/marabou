# Module.mk for vmecontrol module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile, 29/2/2000
#			13/7/2016 comment away libVMEControl.so

MODDIR       := vmecontrol
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

VMECDIR      := $(MODDIR)
VMECDIRS     := $(VMECDIR)/src
VMECDIRI     := $(VMECDIR)/inc

##### lib #####
VMECL        := $(MODDIRI)/LinkDef.h
VMECDS       := $(MODDIRS)/G__VMEControlDict.cxx
VMECPCM      := $(MODDIRS)/G__VMEControlDict_rdict.pcm
VMECDO       := $(VMECDS:.cxx=.o)
VMECHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
VMECH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(VMECHL))
VMECS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
VMECOM       := $(VMECS:.cxx=.o)
VMECMAINO    := $(MODDIRS)/VMEControl.o
VMECO        := $(filter-out $(VMECMAINO),$(VMECOM))


VMECDH			:= $(VMECH)
VMECDEP      := $(VMECO:.o=.d) $(VMECDO:.o=.d)

VMECEXE      := bin/VMEControl
#VMECLIB       := $(LPATH)/libVMEControl.so

ALLEXECS    += $(VMECEXE)

ALLLIBS     += $(VMECLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(VMECPCM)
endif
# used in the vmeControl Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(VMECHL))

# include all dependency files
INCLUDEFILES += $(VMECDEP)

##### extra libs needed #####
OVMECLIBS      := $(LPATH)/libTMrbC2Lynx.$(SOEXT) $(LPATH)/libTMrbUtils.$(SOEXT) $(LPATH)/libTGMrbUtils.$(SOEXT) 

##### local rules #####

include/%.h:    $(VMECDIRI)/%.h
		cp $< $@

$(VMECEXE): $(OVMECLIBS) $(VMECDO) $(VMECO) $(VMECMAINO)
#		@echo "sources: $(VMECS)"
		@echo "$(VMECEXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(VMECMAINO) $(VMECO) $(VMECDO) $(OVMECLIBS) $(ROOTGLIBS) -lSpectrum \
            -o $(VMECEXE)

#$(VMECLIB):     $(VMECDO) $(VMECO)
#		@echo "objs in libVMEControl.so: $(VMECO)"
#		@echo "objs: $(VMECO)"
#		@echo "$(VMECEXE) make shared lib ------------------------------------"
#		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
#		   "$(SOFLAGS)" libVMEControl.$(SOEXT) $@ "$(VMECO) $(VMECDO)"
#		@(if [ -f $(VMECPCM) ] ; then \
#			echo "cp  $(VMECPCM)----------------------" ; \
#			cp $(VMECPCM) $(LPATH); \
#		fi)

$(VMECDS):     $(VMECHL) $(VMECL)
#		@echo "includes: $(VMECDH)"
		@echo "Generating dictionary $@... for ROOT $(ROOT_MAJOR)"
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -Iinclude $(VMECH) $(VMECL)
else
		rootcling -f $@ $(call dictModule,VMEC) -I$(MARABOU_SRCDIR)/include $(VMECH) $(VMECL)
endif

$(VMECDO):     $(VMECDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-vmecontrol:       $(VMECEXE)

clean-vmecontrol:
		@rm -f $(VMECO) $(VMECMAINO) $(VMECDO) $(VMECDS)

clean::         clean-vmecontrol

distclean-vmecontrol: clean-vmecontrol
		@rm -f $(VMECDEP) $(VMECDS) $(VMECEXE) $(VMECLIB)

distclean::     distclean-vmecontrol
