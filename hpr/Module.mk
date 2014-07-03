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


HPRDH			:= $(HPRDIRI)/defineMarabou.h \
 					$(HPRDIRI)/HprTh3Dialog.h \
               $(HPRDIRI)/HTCanvas.h \
 					$(HPRDIRI)/HandleMenus.h \
 					$(HPRDIRI)/FitHist.h \
 					$(HPRDIRI)/HistPresent.h \
 					$(HPRDIRI)/TMrbArrayD.h \
 					$(HPRDIRI)/TMrbArrayF.h \
					$(HPRDIRI)/FhContour.h \
					$(HPRDIRI)/GroupOfHists.h \
 					$(HPRDIRI)/Ascii2GraphDialog.h \
 					$(HPRDIRI)/Ascii2HistDialog.h \
 					$(HPRDIRI)/Ascii2NtupleDialog.h \
 					$(HPRDIRI)/EmptyHistDialog.h \
 					$(HPRDIRI)/SetHistOptDialog.h \
 					$(HPRDIRI)/Set1DimOptDialog.h \
 					$(HPRDIRI)/Set2DimOptDialog.h \
 					$(HPRDIRI)/Set2DimGraphDialog.h \
 					$(HPRDIRI)/Set3DimOptDialog.h \
 					$(HPRDIRI)/SetColorModeDialog.h \
 					$(HPRDIRI)/SetCanvasAttDialog.h \
 					$(HPRDIRI)/WhatToShowDialog.h \
 					$(HPRDIRI)/GeneralAttDialog.h \
 					$(HPRDIRI)/GraphAttDialog.h \
					$(HPRDIRI)/WindowSizeDialog.h \
					$(HPRDIRI)/Rebin2DimDialog.h \
 					$(HPRDIRI)/AddFitMenus.h \
 					$(HPRDIRI)/HprStack.h \
 					$(HPRDIRI)/HprGaxis.h \
 					$(HPRDIRI)/HprLegend.h \
 					$(HPRDIRI)/TText3D.h \
 					$(HPRDIRI)/ShiftScale.h \
  					$(HPRDIRI)/LinkDef.h

HPRDEP      := $(HPRO:.o=.d) $(HPRDO:.o=.d)
HPRDEP      += $(MODDIRS)/main.d

HPREXE      := bin/HistPresent
HPRLIB      := $(LPATH)/libHpr.so
HPRDUMMYLIB := $(LPATH)/libHprDummy.so

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
		

$(HPRDUMMYLIB):     $(HPRDUMMY)
		   
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libHprDummy.$(SOEXT) $(HPRDUMMYLIB) $(HPRDUMMY)

$(HPRDS):     $(HPRDH) $(HPRL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@  -c -p -Iinclude $(HPRDH)

$(HPRDO):     $(HPRDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-hpr:       $(HPREXE) $(HPRDUMMYLIB)

clean-hpr:
		@rm -f $(HPRO) $(HPRMAINO) $(HPRDO) $(HPRDS)

clean::         clean-hpr

distclean-hpr: clean-hpr
		@rm -f $(HPRDEP) $(HPRDS) $(HPREXE) $(HPRLIB)

distclean::     distclean-hpr
