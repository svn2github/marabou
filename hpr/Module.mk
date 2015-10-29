# Module.mk for hpr module
#
# Author: Otto Schaile, 29/2/2000

MODDIR       := hpr
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

HPRDIR      := $(MODDIR)
HPRDIRS     := $(HPRDIR)/src
HPRDIRI     := $(HPRDIR)/inc
HPRDUMMY    := $(MODDIRS)/hpr_dummy.o

##### lib #####
HPRL        := $(MODDIRI)/LinkDef.h
HPRDS       := $(MODDIRS)/G__HistPresentDict.cxx
HPRPCM      := $(MODDIRS)/G__HistPresentDict_rdict.pcm
HPRDO       := $(HPRDS:.cxx=.o)
HPRH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
HPRS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
HPROM       := $(HPRS:.cxx=.o)
HPRMAINO    := $(MODDIRS)/main.o
HPROA       := $(filter-out $(HPRMAINO),$(HPROM))
# hpr_dummy is used to make a dummy libHprDummy with only the pointer gHpr=0
# must not be linked in standard libHpr
HPRO        := $(filter-out $(HPRDUMMY), $(HPROA))


HPRDH			:= include/defineMarabou.h \
 					include/HprTh3Dialog.h \
               include/HTCanvas.h \
 					include/HandleMenus.h \
 					include/FitHist.h \
 					include/HistPresent.h \
 					include/TMrbArrayD.h \
 					include/TMrbArrayF.h \
					include/FhContour.h \
					include/GroupOfHists.h \
 					include/Ascii2GraphDialog.h \
 					include/Ascii2HistDialog.h \
 					include/Ascii2NtupleDialog.h \
 					include/EmptyHistDialog.h \
 					include/SetHistOptDialog.h \
 					include/Set1DimOptDialog.h \
 					include/Set2DimOptDialog.h \
 					include/Set2DimGraphDialog.h \
 					include/Set3DimOptDialog.h \
 					include/SetColorModeDialog.h \
 					include/SetCanvasAttDialog.h \
 					include/WhatToShowDialog.h \
 					include/GeneralAttDialog.h \
 					include/GraphAttDialog.h \
					include/WindowSizeDialog.h \
					include/Rebin2DimDialog.h \
 					include/AddFitMenus.h \
 					include/HprStack.h \
 					include/HprFunc2.h \
 					include/HprGaxis.h \
 					include/HprLegend.h \
 					include/TText3D.h \
 					include/ShiftScale.h 

HPRDEP      := $(HPRO:.o=.d) $(HPRDO:.o=.d)
HPRDEP      += $(MODDIRS)/main.d

HPREXE      := bin/HistPresent
HPRLIB      := $(LPATH)/libHpr.so
HPRDUMMYLIB := $(LPATH)/libHprDummy.so
HPRRMAP     := $(LPATH)/libHpr.rootmap

ALLEXECS    += $(HPREXE)

ALLLIBS     += $(HPRLIB)
ifeq ($(ROOTV6), 1)
	ALLPCMS += $(HPRPCM)
endif
ALLLIBS     += $(HPRDUMMYLIB)
# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(HPRH))

# include all dependency files
INCLUDEFILES += $(HPRDEP)

##### extra libs needed #####
MRBUTILSLIB   := $(LPATH)/libTMrbUtils.$(SOEXT)
MRBGUTILSLIB  := $(LPATH)/libTGMrbUtils.$(SOEXT)
HELPBRLIB     := $(LPATH)/libTMrbHelpBrowser.$(SOEXT)
FITCALLIB     := $(LPATH)/libFitCal.$(SOEXT)
GREDITLIB     := $(LPATH)/libGrEdit.$(SOEXT)

OHPRLIBS      := $(MRBUTILSLIB) $(MRBGUTILSLIB) $(HELPBRLIB) $(FITCALLIB) $(GREDITLIB)

HPRLIBDEP   := $(ROOTSYS)/lib/libGraf.so $(ROOTSYS)/lib/libProof.so $(OHPRLIBS)

##### local rules #####

include/%.h:    $(HPRDIRI)/%.h
		cp $< $@

$(HPREXE):      $(HPRMAINO) $(OHPRLIBS) $(HPRLIB)
		@echo "other libs: $(OHPRLIBS)"
		@echo "$(HPREXE) linking exe ----------------------------------"
		$(LD) -g $(LDFLAGS) $(HPRMAINO) $(HPRLIB) $(OHPRLIBS) $(ROOTGLIBS) -lRGL -lSpectrum -lProof \
            -o $(HPREXE)

$(HPRLIB):     $(HPRDO) $(HPRO)
#		@echo "objs: $(HPRO)"
		@echo "make shared lib  $(HPRLIB)----------------------"
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libHpr.$(SOEXT) $@ "$(HPRO) $(HPRDO) $(HPRLIBEXTRA)"
		@(if [ -f $(HPRPCM) ] ; then \
			echo "cp  $(HPRPCM)----------------------" ; \
			cp $(HPRPCM) $(LPATH); \
		fi)
ifneq ($(ROOTV6), 1)
		@$(RLIBMAP) -o $(HPRRMAP) -l $(HPRLIB) -d $(HPRLIBDEP) -c $(HPRL)
endif

$(HPRDUMMYLIB):     $(HPRDUMMY)
		   
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libHprDummy.$(SOEXT) $(HPRDUMMYLIB) $(HPRDUMMY)

$(HPRDS):     $(HPRDH) $(HPRL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@  -c -p -Iinclude $(HPRDH) $(HPRL)

$(HPRDO):     $(HPRDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<

all-hpr:       $(HPREXE) $(HPRDUMMYLIB)

clean-hpr:
		@rm -f $(HPRO) $(HPRMAINO) $(HPRDO) $(HPRDS)

clean::         clean-hpr

distclean-hpr: clean-hpr
		@rm -f $(HPRDEP) $(HPRDS) $(HPREXE) $(HPRLIB)

distclean::     distclean-hpr
