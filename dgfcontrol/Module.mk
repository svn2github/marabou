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
DGFCPCM      := $(MODDIRS)/G__DGFControlDict_rdict.pcm
DGFCDO       := $(DGFCDS:.cxx=.o)
DGFCHL       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
DGFCH        :=  $(patsubst $(MODDIRI)/%.h,%.h,$(DGFCHL))
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
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(DGFCOMPCM)
endif
# used in the DGFControl Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(DGFCHL))

# include all dependency files
INCLUDEFILES += $(DGFCDEP)

##### extra libs needed #####
ODGFCLIBS	:= $(LPATH)/libTMrbDGF.$(SOEXT)   $(LPATH)/libTMrbEsone.$(SOEXT)   $(LPATH)/libTMrbC2Lynx.$(SOEXT)   $(LPATH)/libEsoneClient.$(SOEXT) \
					$(LPATH)/libTMrbUtils.$(SOEXT)   $(LPATH)/libTGMrbUtils.$(SOEXT)   $(LPATH)/libHpr.$(SOEXT)   $(LPATH)/libTMrbHelpBrowser.$(SOEXT) $(LPATH)/libGrEdit.$(SOEXT) \
					$(LPATH)/libFitCal.$(SOEXT)  
# ODGFCLIBS      := -L$(LPATH) -lTMrbDGF -lTMrbEsone -lTMrbC2Lynx -lEsoneClient \
#                 -lTMrbUtils -lTGMrbUtils -lHpr -lTMrbHelpBrowser -lGrEdit -lFitCal -lProof

##### local rules #####

include/%.h:    $(DGFCDIRI)/%.h
		cp $< $@
$(DGFCEXE):   $(ODGFCLIBS) $(DGFCO) $(DGFCMAINO) 
		$(LD) -g $(LDFLAGS) $(DGFCMAINO) $(DGFCO) $(DGFCDO) $(ODGFCLIBS) $(ROOTGLIBS) -lProof -lSpectrum \
            -o $(DGFCEXE)

$(DGFCLIB):     $(DGFCDO) $(DGFCO) $(ODGFCLIBS)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libDGFControl.$(SOEXT) $@ "$(DGFCO) $(DGFCDO)" "$(ODGFCLIBS) $(DGFCONTROLLIBEXTRA)"
		@(if [ -f $(DGFCPCM) ] ; then \
			echo "cp  $(DGFCPCM)----------------------" ; \
			cp $(DGFCPCM) $(LPATH); \
		fi)

$(DGFCDS):     $(DGFCDHL) $(DGFCL)
ifneq ($(ROOTV6), 1)
		$(ROOTCINT) -f $@ -c -p -Iinclude $(DGFCDH) $(DGFCL)
else
		rootcling -f $@ $(call dictModule,DGFC)  -I$(MARABOU_SRCDIR)/include $(DGFCH) $(DGFCL)
endif

$(DGFCDO):     $(DGFCDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-dgfcontrol:       $(DGFCEXE)

clean-dgfcontrol:
		@rm -f $(DGFCO) $(DGFCMAINO) $(DGFCDO) $(DGFCDS)

clean::         clean-dgfcontrol

distclean-dgfcontrol: clean-dgfcontrol
		@rm -f $(DGFCDEP) $(DGFCDS) $(DGFCEXE) $(DGFCLIB)

distclean::     distclean-dgfcontrol
